#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/RAWMonitor.h"
#include "IO/FileAnalyse/PCapngFileAnalyse.h"
#include "Net/PacketAnalyzer.h"
#include "Net/SocketUtil.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall IO::FileAnalyse::PCapngFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::PCapngFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::PCapngFileAnalyse>();
	UInt64 ofst;
	UInt64 dataSize;
	UInt32 thisSize;
	NN<IO::FileAnalyse::PCapngFileAnalyse::BlockInfo> block;
	UInt8 packetHdr[16];
	Data::ArrayList<UInt16> linkTypeList;
	Data::ArrayList<Int8> resList;
	ofst = 0;
	dataSize = me->fd->GetDataSize();
	while (ofst < dataSize - 16 && !thread->IsStopping())
	{
		if (me->fd->GetRealData(ofst, 12, BYTEARR(packetHdr)) != 12)
			break;
		
		if (me->isBE)
		{
			thisSize = ReadMUInt32(&packetHdr[4]);
		}
		else
		{
			thisSize = ReadUInt32(&packetHdr[4]);
		}
		if (thisSize < 12 || thisSize + ofst > dataSize)
		{
			break;
		}
		block = MemAllocNN(IO::FileAnalyse::PCapngFileAnalyse::BlockInfo);
		block->ofst = ofst;
		if (me->isBE)
		{
			block->blockType = ReadMUInt32(&packetHdr[0]);
		}
		else
		{
			block->blockType = ReadUInt32(&packetHdr[0]);
		}
		block->blockLength = thisSize;
		block->timeResol = 0;
		if (block->blockType == 0x0a0d0d0a)
		{
			resList.Clear();
			linkTypeList.Clear();
		}
		else if (block->blockType == 1)
		{
			Data::ByteBuffer packetBuff(thisSize);
			me->fd->GetRealData(ofst, thisSize, packetBuff);
			UInt16 linkType;
			Int8 timeResol = 0;
			UInt16 optCode;
			UInt16 optLeng;
			if (me->isBE)
			{
				linkType = ReadMUInt16(&packetBuff[8]);
			}
			else
			{
				linkType = ReadUInt16(&packetBuff[8]);
			}
			
			UOSInt i = 16;
			while (i < block->blockLength - 4)
			{
				if (me->isBE)
				{
					optCode = ReadMUInt16(&packetBuff[i]);
					optLeng = ReadMUInt16(&packetBuff[i + 2]);
				}
				else
				{
					optCode = ReadUInt16(&packetBuff[i]);
					optLeng = ReadUInt16(&packetBuff[i + 2]);
				}
				if (i + 4 + optLeng > block->blockLength)
				{
					break;
				}
				if (optCode == 0)
				{
					break;
				}
				else if (optCode == 9)
				{
					timeResol = (Int8)packetBuff[i + 4];
					break;
				}
				i += 4 + (UOSInt)optLeng;
				if (i & 3)
				{
					i += 4 - (i & 3);
				}
			}
			resList.Add(timeResol);
			linkTypeList.Add(linkType);
		}
		else if (block->blockType == 6 || block->blockType == 5)
		{
			UInt32 ifId;
			if (me->isBE)
			{
				ifId = ReadMUInt32(&packetHdr[8]);
			}
			else
			{
				ifId = ReadUInt32(&packetHdr[8]);
			}
			block->timeResol = resList.GetItem(ifId);
			block->linkType = linkTypeList.GetItem(ifId);
		}

		me->blockList.Add(block);
		ofst += thisSize;
	}
}

IO::FileAnalyse::PCapngFileAnalyse::PCapngFileAnalyse(NN<IO::StreamData> fd) : packetBuff(65536), thread(ParseThread, this, CSTR("PCapngFileAnaly"))
{
	UInt8 buff[16];
	this->fd = 0;
	this->pauseParsing = false;
	this->isBE = false;
	if (fd->GetRealData(0, 16, BYTEARR(buff)) != 16)
	{
		return;
	}
	if (ReadNInt32(buff) != 0x0a0d0d0a)
	{
		return;
	}
	if (ReadInt32(&buff[8]) == 0x1a2b3c4d)
	{
		this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
		this->isBE = false;
	}
	else if (ReadMInt32(&buff[8]) == 0x1a2b3c4d)
	{
		this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
		this->isBE = true;
	}
	else
	{
		return;
	}
	this->thread.Start();
}

IO::FileAnalyse::PCapngFileAnalyse::~PCapngFileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
	this->blockList.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::PCapngFileAnalyse::GetFormatName()
{
	return CSTR("pcapng");
}

UOSInt IO::FileAnalyse::PCapngFileAnalyse::GetFrameCount()
{
	return this->blockList.GetCount();
}

Bool IO::FileAnalyse::PCapngFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::PCapngFileAnalyse::BlockInfo> block;
	if (index >= this->blockList.GetCount())
	{
		return false;
	}
	block = this->blockList.GetItemNoCheck(index);
	fd->GetRealData(block->ofst, block->blockLength, this->packetBuff);
	sb->AppendU64(block->ofst);
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendU32(block->blockLength);
	if (block->blockType == 0x0a0d0d0a)
	{
		sb->AppendC(UTF8STRC(", SHB"));
	}
	else if (block->blockType == 1)
	{
		sb->AppendC(UTF8STRC(", IDB"));
	}
	else if (block->blockType == 6)
	{
		UInt32 psize;
		sb->AppendC(UTF8STRC(", EPB"));
		if (this->isBE)
		{
			psize = ReadMUInt32(&this->packetBuff[20]);
		}
		else
		{
			psize = ReadUInt32(&this->packetBuff[20]);
		}
		if (psize + 32 <= block->blockLength)
		{
			sb->AppendC(UTF8STRC(", "));
			if (!Net::PacketAnalyzer::PacketDataGetName(block->linkType, &this->packetBuff[28], psize, sb))
			{
				sb->AppendC(UTF8STRC("Unknown"));
			}
		}
	}
	else if (block->blockType == 3)
	{
		UInt32 psize;
		sb->AppendC(UTF8STRC(", SPB"));
		if (this->isBE)
		{
			psize = ReadMUInt32(&this->packetBuff[8]);
		}
		else
		{
			psize = ReadUInt32(&this->packetBuff[8]);
		}
		if (psize + 32 <= block->blockLength)
		{
			sb->AppendC(UTF8STRC(", "));
			if (!Net::PacketAnalyzer::PacketDataGetName(block->linkType, &this->packetBuff[28], psize, sb))
			{
				sb->AppendC(UTF8STRC("Unknown"));
			}
		}
	}
	else if (block->blockType == 4)
	{
		sb->AppendC(UTF8STRC(", NRB"));
	}
	else if (block->blockType == 5)
	{
		sb->AppendC(UTF8STRC(", ISB"));
	}
	else if (block->blockType == 9)
	{
		sb->AppendC(UTF8STRC(", JEB"));
	}
	else if (block->blockType == 10)
	{
		sb->AppendC(UTF8STRC(", DSB"));
	}
	else if (block->blockType == 0xBAD)
	{
		sb->AppendC(UTF8STRC(", CB"));
	}
	else if (block->blockType == 0x40000BAD)
	{
		sb->AppendC(UTF8STRC(", CBNC"));
	}
	return true;
}

Bool IO::FileAnalyse::PCapngFileAnalyse::GetFrameDetail(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::FileAnalyse::PCapngFileAnalyse::BlockInfo> block;
	if (index >= this->blockList.GetCount())
	{
		return false;
	}
	block = this->blockList.GetItemNoCheck(index);
	fd->GetRealData(block->ofst, block->blockLength, this->packetBuff);
	sb->AppendC(UTF8STRC("Offset="));
	sb->AppendU64(block->ofst);
	sb->AppendC(UTF8STRC("\r\nType=0x"));
	sb->AppendHex32(block->blockType);
	sb->AppendC(UTF8STRC("\r\nTotalSize="));
	sb->AppendU32(block->blockLength);
	sb->AppendC(UTF8STRC("\r\n"));
	if (block->blockType == 0x0a0d0d0a)
	{
		sb->AppendC(UTF8STRC("\r\nSection Header Block:"));
		sb->AppendC(UTF8STRC("\r\nByte Order="));
		UInt16 majorVer;
		UInt16 minorVer;
		Int64 sectionLength;
		if (this->isBE)
		{
			sb->AppendC(UTF8STRC("Big-endian"));
			majorVer = ReadMUInt16(&this->packetBuff[12]);
			minorVer = ReadMUInt16(&this->packetBuff[14]);
			sectionLength = ReadMInt64(&this->packetBuff[16]);
		}
		else
		{
			sb->AppendC(UTF8STRC("Little-endian"));
			majorVer = ReadUInt16(&this->packetBuff[12]);
			minorVer = ReadUInt16(&this->packetBuff[14]);
			sectionLength = ReadInt64(&this->packetBuff[16]);
		}
		sb->AppendC(UTF8STRC("\r\nMajor Version="));
		sb->AppendU16(majorVer);
		sb->AppendC(UTF8STRC("\r\nMinor Version="));
		sb->AppendU16(minorVer);
		sb->AppendC(UTF8STRC("\r\nSection Length="));
		sb->AppendI64(sectionLength);
		UInt16 optCode;
		UInt16 optLeng;
		UOSInt i = 24;
		while (i < block->blockLength - 4)
		{
			if (this->isBE)
			{
				optCode = ReadMUInt16(&this->packetBuff[i]);
				optLeng = ReadMUInt16(&this->packetBuff[i + 2]);
			}
			else
			{
				optCode = ReadUInt16(&this->packetBuff[i]);
				optLeng = ReadUInt16(&this->packetBuff[i + 2]);
			}
			sb->AppendC(UTF8STRC("\r\nOption Code="));
			sb->AppendU16(optCode);
			sb->AppendC(UTF8STRC("\r\nOption Length="));
			sb->AppendU16(optLeng);
			if (i + 4 + optLeng > block->blockLength)
			{
				break;
			}
			if (optCode == 0)
			{
				break;
			}
			else if (optCode == 1)
			{
				sb->AppendC(UTF8STRC("\r\nComment="));
				sb->AppendC(&this->packetBuff[i + 4], optLeng);
			}
			else if (optCode == 2)
			{
				sb->AppendC(UTF8STRC("\r\nHardware="));
				sb->AppendC(&this->packetBuff[i + 4], optLeng);
			}
			else if (optCode == 3)
			{
				sb->AppendC(UTF8STRC("\r\nOS="));
				sb->AppendC(&this->packetBuff[i + 4], optLeng);
			}
			else if (optCode == 4)
			{
				sb->AppendC(UTF8STRC("\r\nUser Application="));
				sb->AppendC(&this->packetBuff[i + 4], optLeng);
			}

			i += 4 + (UOSInt)optLeng;
			if (i & 3)
			{
				i += 4 - (i & 3);
			}
		}
	}
	else if (block->blockType == 1)
	{
		sb->AppendC(UTF8STRC("\r\nInterface Description Block:"));
		UInt16 linkType;
		UInt16 reserved;
		UInt32 snapLen;
		if (this->isBE)
		{
			linkType = ReadMUInt16(&this->packetBuff[8]);
			reserved = ReadMUInt16(&this->packetBuff[10]);
			snapLen = ReadMUInt32(&this->packetBuff[12]);
		}
		else
		{
			linkType = ReadUInt16(&this->packetBuff[8]);
			reserved = ReadUInt16(&this->packetBuff[10]);
			snapLen = ReadUInt32(&this->packetBuff[12]);
		}
		sb->AppendC(UTF8STRC("\r\nLinkType="));
		sb->AppendU16(linkType);
		Text::CStringNN cstr;
		if (IO::RAWMonitor::LinkTypeGetName(linkType).SetTo(cstr))
		{
			sb->AppendC(UTF8STRC(" ("));
			sb->Append(cstr);
			sb->AppendC(UTF8STRC(")"));
		}
		sb->AppendC(UTF8STRC("\r\nReserved=0x"));
		sb->AppendU16(reserved);
		sb->AppendC(UTF8STRC("\r\nSnap Length="));
		sb->AppendU32(snapLen);
		UInt16 optCode;
		UInt16 optLeng;
		UOSInt i = 16;
		while (i < block->blockLength - 4)
		{
			if (this->isBE)
			{
				optCode = ReadMUInt16(&this->packetBuff[i]);
				optLeng = ReadMUInt16(&this->packetBuff[i + 2]);
			}
			else
			{
				optCode = ReadUInt16(&this->packetBuff[i]);
				optLeng = ReadUInt16(&this->packetBuff[i + 2]);
			}
			sb->AppendC(UTF8STRC("\r\nOption Code="));
			sb->AppendU16(optCode);
			sb->AppendC(UTF8STRC("\r\nOption Length="));
			sb->AppendU16(optLeng);
			if (i + 4 + optLeng > block->blockLength)
			{
				break;
			}
			if (optCode == 0)
			{
				break;
			}
			else if (optCode == 1)
			{
				sb->AppendC(UTF8STRC("\r\nComment="));
				sb->AppendC(&this->packetBuff[i + 4], optLeng);
			}
			else if (optCode == 2)
			{
				sb->AppendC(UTF8STRC("\r\nName="));
				sb->AppendC(&this->packetBuff[i + 4], optLeng);
			}
			else if (optCode == 3)
			{
				sb->AppendC(UTF8STRC("\r\nDescription="));
				sb->AppendC(&this->packetBuff[i + 4], optLeng);
			}
			else if (optCode == 4)
			{
				sb->AppendC(UTF8STRC("\r\nIPv4 Address="));
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&this->packetBuff[i + 4]));
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sb->AppendC(UTF8STRC("\r\nNetmask="));
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&this->packetBuff[i + 8]));
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			}
			else if (optCode == 5)
			{
				Net::SocketUtil::AddressInfo addr;
				Net::SocketUtil::SetAddrInfoV6(addr, &this->packetBuff[i + 4], 0);
				sb->AppendC(UTF8STRC("\r\nIPv6 Address="));
				sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sb->AppendC(UTF8STRC("/"));
				sb->AppendU16(this->packetBuff[i + 20]);
			}
			else if (optCode == 6)
			{
				sb->AppendC(UTF8STRC("\r\nMAC Address="));
				sb->AppendHexBuff(&this->packetBuff[i + 4], 6, ':', Text::LineBreakType::None);
			}
			else if (optCode == 7)
			{
				sb->AppendC(UTF8STRC("\r\nEUI Address="));
				sb->AppendHexBuff(&this->packetBuff[i + 4], 8, ' ', Text::LineBreakType::None);
			}
			else if (optCode == 8)
			{
				Int64 speed;
				if (this->isBE)
				{
					speed = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					speed = ReadInt64(&this->packetBuff[i + 4]);
				}
				sb->AppendC(UTF8STRC("\r\nSpeed="));
				sb->AppendI64(speed);
				sb->AppendC(UTF8STRC("bps"));
			}
			else if (optCode == 9)
			{
				sb->AppendC(UTF8STRC("\r\nTimestamps resolution="));
				sb->AppendI16((Int8)this->packetBuff[i + 4]);
			}
			else if (optCode == 10)
			{
				Int32 tzone;
				if (this->isBE)
				{
					tzone = ReadMInt32(&this->packetBuff[i + 4]);
				}
				else
				{
					tzone = ReadInt32(&this->packetBuff[i + 4]);
				}
				sb->AppendC(UTF8STRC("\r\nTime Zone="));
				sb->AppendI32(tzone);
			}
			else if (optCode == 11)
			{
				sb->AppendC(UTF8STRC("\r\nFilter="));
				sb->AppendU16(this->packetBuff[i + 4]);
				if (optLeng > 1)
				{
					sb->AppendC(UTF8STRC(" ("));
					sb->AppendC(&this->packetBuff[i + 5], (UOSInt)optLeng - 1);
					sb->AppendC(UTF8STRC(")"));
				}
			}
			else if (optCode == 12)
			{
				sb->AppendC(UTF8STRC("\r\nOS="));
				sb->AppendC(&this->packetBuff[i + 4], (UOSInt)optLeng);
			}
			else if (optCode == 13)
			{
				sb->AppendC(UTF8STRC("\r\nFrame Check Sequence Length="));
				sb->AppendU16(this->packetBuff[i + 4]);
			}
			else if (optCode == 14)
			{
				Int64 tsOffset;
				if (this->isBE)
				{
					tsOffset = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					tsOffset = ReadInt64(&this->packetBuff[i + 4]);
				}
				sb->AppendC(UTF8STRC("\r\nTS Offset="));
				sb->AppendI64(tsOffset);
				sb->AppendC(UTF8STRC("sec."));
			}
			else if (optCode == 15)
			{
				sb->AppendC(UTF8STRC("\r\nHardware="));
				sb->AppendHexBuff(&this->packetBuff[i + 4], 8, ' ', Text::LineBreakType::None);
			}

			i += 4 + (UOSInt)optLeng;
			if (i & 3)
			{
				i += 4 - (i & 3);
			}
		}
	}
	else if (block->blockType == 6)
	{
		sb->AppendC(UTF8STRC("\r\nEnhanced Packet Block:"));
		UInt32 ifId;
		Int64 ts;
		UInt32 capPSize;
		UInt32 oriPSize;
		Data::DateTime dt;
		if (this->isBE)
		{
			ifId = ReadMUInt32(&this->packetBuff[8]);
			ts = ReadMInt64(&this->packetBuff[12]);
			capPSize = ReadMUInt32(&this->packetBuff[20]);
			oriPSize = ReadMUInt32(&this->packetBuff[24]);
		}
		else
		{
			ifId = ReadUInt32(&this->packetBuff[8]);
			ts = (((Int64)ReadInt32(&this->packetBuff[12])) << 32) | ReadUInt32(&this->packetBuff[16]);
			capPSize = ReadUInt32(&this->packetBuff[20]);
			oriPSize = ReadUInt32(&this->packetBuff[24]);
		}
		sb->AppendC(UTF8STRC("\r\nInterface ID="));
		sb->AppendU32(ifId);
		SetTime(&dt, ts, block->timeResol);
		dt.ToLocalTime();
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		sb->AppendC(UTF8STRC("\r\nTime="));
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendC(UTF8STRC("\r\nCaptured Packet Length="));
		sb->AppendU32(capPSize);
		sb->AppendC(UTF8STRC("\r\nOriginal Packet Length="));
		sb->AppendU32(oriPSize);
		if (capPSize + 32 <= block->blockLength)
		{
			Net::PacketAnalyzer::PacketDataGetDetail(block->linkType, &this->packetBuff[28], capPSize, sb);

			UInt16 optCode;
			UInt16 optLeng;
			UOSInt i = 28 + capPSize;
			if (i & 3)
			{
				i += 4 - (i & 3);
			}
			while (i < block->blockLength - 4)
			{
				if (this->isBE)
				{
					optCode = ReadMUInt16(&this->packetBuff[i]);
					optLeng = ReadMUInt16(&this->packetBuff[i + 2]);
				}
				else
				{
					optCode = ReadUInt16(&this->packetBuff[i]);
					optLeng = ReadUInt16(&this->packetBuff[i + 2]);
				}
				sb->AppendC(UTF8STRC("\r\nOption Code="));
				sb->AppendU16(optCode);
				sb->AppendC(UTF8STRC("\r\nOption Length="));
				sb->AppendU16(optLeng);
				if (i + 4 + optLeng > block->blockLength)
				{
					break;
				}
				if (optCode == 0)
				{
					break;
				}
				else if (optCode == 1)
				{
					sb->AppendC(UTF8STRC("\r\nComment="));
					sb->AppendC(&this->packetBuff[i + 4], optLeng);
				}
				else if (optCode == 2)
				{
					sb->AppendC(UTF8STRC("\r\nHardware="));
					sb->AppendC(&this->packetBuff[i + 4], optLeng);
				}
				else if (optCode == 3)
				{
					sb->AppendC(UTF8STRC("\r\nOS="));
					sb->AppendC(&this->packetBuff[i + 4], optLeng);
				}
				else if (optCode == 4)
				{
					sb->AppendC(UTF8STRC("\r\nUser Application="));
					sb->AppendC(&this->packetBuff[i + 4], optLeng);
				}

				i += 4 + (UOSInt)optLeng;
				if (i & 3)
				{
					i += 4 - (i & 3);
				}
			}
		}
		else
		{
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendHexBuff(&this->packetBuff[28], block->blockLength - 32, ' ', Text::LineBreakType::CRLF);
		}
	}
	else if (block->blockType == 5)
	{
		sb->AppendC(UTF8STRC("\r\nInterface Statistics Block:"));
		UInt32 ifId;
		Int64 ts;
		Data::DateTime dt;
		if (this->isBE)
		{
			ifId = ReadMUInt32(&this->packetBuff[8]);
			ts = ReadMInt64(&this->packetBuff[12]);
		}
		else
		{
			ifId = ReadUInt32(&this->packetBuff[8]);
			ts = (((Int64)ReadInt32(&this->packetBuff[12])) << 32) | ReadUInt32(&this->packetBuff[16]);
		}
		sb->AppendC(UTF8STRC("\r\nInterface ID="));
		sb->AppendU32(ifId);
		SetTime(&dt, ts, block->timeResol);
		dt.ToLocalTime();
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		sb->AppendC(UTF8STRC("\r\nTime="));
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		UInt16 optCode;
		UInt16 optLeng;
		UOSInt i = 20;
		while (i < block->blockLength - 4)
		{
			if (this->isBE)
			{
				optCode = ReadMUInt16(&this->packetBuff[i]);
				optLeng = ReadMUInt16(&this->packetBuff[i + 2]);
			}
			else
			{
				optCode = ReadUInt16(&this->packetBuff[i]);
				optLeng = ReadUInt16(&this->packetBuff[i + 2]);
			}
			sb->AppendC(UTF8STRC("\r\nOption Code="));
			sb->AppendU16(optCode);
			sb->AppendC(UTF8STRC("\r\nOption Length="));
			sb->AppendU16(optLeng);
			if (i + 4 + optLeng > block->blockLength)
			{
				break;
			}
			if (optCode == 0)
			{
				break;
			}
			else if (optCode == 1)
			{
				sb->AppendC(UTF8STRC("\r\nComment="));
				sb->AppendC(&this->packetBuff[i + 4], optLeng);
			}
			else if (optCode == 2)
			{
				sb->AppendC(UTF8STRC("\r\nStart Time="));
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = (((Int64)ReadInt32(&this->packetBuff[i + 4])) << 32) | ReadUInt32(&this->packetBuff[i + 8]);
				}
				SetTime(&dt, ts, block->timeResol);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			}
			else if (optCode == 3)
			{
				sb->AppendC(UTF8STRC("\r\nEnd Time="));
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = (((Int64)ReadInt32(&this->packetBuff[i + 4])) << 32) | ReadUInt32(&this->packetBuff[i + 8]);
				}
				SetTime(&dt, ts, block->timeResol);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			}
			else if (optCode == 4)
			{
				sb->AppendC(UTF8STRC("\r\nReceived Packets="));
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = ReadInt64(&this->packetBuff[i + 4]);
				}
				sb->AppendI64(ts);
			}
			else if (optCode == 5)
			{
				sb->AppendC(UTF8STRC("\r\nDropped Packets="));
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = ReadInt64(&this->packetBuff[i + 4]);
				}
				sb->AppendI64(ts);
			}
			else if (optCode == 6)
			{
				sb->AppendC(UTF8STRC("\r\nPackets Accepted by Filter="));
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = ReadInt64(&this->packetBuff[i + 4]);
				}
				sb->AppendI64(ts);
			}
			else if (optCode == 7)
			{
				sb->AppendC(UTF8STRC("\r\nPackets Dropped by OS="));
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = ReadInt64(&this->packetBuff[i + 4]);
				}
				sb->AppendI64(ts);
			}
			else if (optCode == 8)
			{
				sb->AppendC(UTF8STRC("\r\nPackets Delivered to the user="));
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = ReadInt64(&this->packetBuff[i + 4]);
				}
				sb->AppendI64(ts);
			}

			i += 4 + (UOSInt)optLeng;
			if (i & 3)
			{
				i += 4 - (i & 3);
			}
		}
	}
	return true;
}

UOSInt IO::FileAnalyse::PCapngFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->blockList.GetCount() - 1;
	OSInt k;
	NN<BlockInfo> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->blockList.GetItemNoCheck((UOSInt)k);
		if (ofst < pack->ofst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->ofst + pack->blockLength)
		{
			i = k + 1;
		}
		else
		{
			return (UOSInt)k;
		}
	}
	return INVALID_INDEX;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::PCapngFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::FileAnalyse::PCapngFileAnalyse::BlockInfo> block;
	if (index >= this->blockList.GetCount())
	{
		return 0;
	}
	block = this->blockList.GetItemNoCheck(index);
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(block->ofst, block->blockLength));
	fd->GetRealData(block->ofst, block->blockLength, this->packetBuff);
	if (this->isBE)
	{
		frame->AddHex32(0, CSTR("Type"), this->packetBuff.ReadMU32(0));
		frame->AddUInt(4, 4, CSTR("TotalSize"), ReadMUInt32(&this->packetBuff[4]));
	}
	else
	{
		frame->AddHex32(0, CSTR("Type"), this->packetBuff.ReadU32(0));
		frame->AddUInt(4, 4, CSTR("TotalSize"), ReadUInt32(&this->packetBuff[4]));
	}
	Text::StringBuilderUTF8 sb;

	if (block->blockType == 0x0a0d0d0a)
	{
		frame->AddFieldSeperstor(8, CSTR("Section Header Block:"));
		UInt16 majorVer;
		UInt16 minorVer;
		Int64 sectionLength;
		if (this->isBE)
		{
			frame->AddField(8, 4, CSTR("Byte Order"), CSTR("Big-endian"));
			majorVer = ReadMUInt16(&this->packetBuff[12]);
			minorVer = ReadMUInt16(&this->packetBuff[14]);
			sectionLength = ReadMInt64(&this->packetBuff[16]);
		}
		else
		{
			frame->AddField(8, 4, CSTR("Byte Order"), CSTR("Little-endian"));
			majorVer = ReadUInt16(&this->packetBuff[12]);
			minorVer = ReadUInt16(&this->packetBuff[14]);
			sectionLength = ReadInt64(&this->packetBuff[16]);
		}
		frame->AddUInt(12, 2, CSTR("Major Version"), majorVer);
		frame->AddUInt(14, 2, CSTR("Minor Version"), minorVer);
		frame->AddInt64(16, CSTR("Section Length"), sectionLength);

		UInt16 optCode;
		UInt16 optLeng;
		UInt32 i = 24;
		while (i < block->blockLength - 4)
		{
			if (this->isBE)
			{
				optCode = ReadMUInt16(&this->packetBuff[i]);
				optLeng = ReadMUInt16(&this->packetBuff[i + 2]);
			}
			else
			{
				optCode = ReadUInt16(&this->packetBuff[i]);
				optLeng = ReadUInt16(&this->packetBuff[i + 2]);
			}
			frame->AddUInt(i, 2, CSTR("Option Code"), optCode);
			frame->AddUInt(i + 2, 2, CSTR("Option Length"), optLeng);
			if (i + 4 + optLeng > block->blockLength)
			{
				break;
			}
			if (optCode == 0)
			{
				i += 4;
				break;
			}
			else if (optCode == 1)
			{
				frame->AddStrC(i + 4, optLeng, CSTR("Comment"), &this->packetBuff[i + 4]);
			}
			else if (optCode == 2)
			{
				frame->AddStrC(i + 4, optLeng, CSTR("Hardware"), &this->packetBuff[i + 4]);
			}
			else if (optCode == 3)
			{
				frame->AddStrC(i + 4, optLeng, CSTR("OS"), &this->packetBuff[i + 4]);
			}
			else if (optCode == 4)
			{
				frame->AddStrC(i + 4, optLeng, CSTR("User Application"), &this->packetBuff[i + 4]);
			}

			i += 4 + (UInt32)optLeng;
			if (i & 3)
			{
				frame->AddHexBuff(i, 4 - (i & 3), CSTR("Padding"), &this->packetBuff[i], false);
				i += 4 - (i & 3);
			}
		}
		if (i == block->blockLength - 4)
		{
			if (this->isBE)
			{
				frame->AddUInt(i, 4, CSTR("TotalSize"), ReadMUInt32(&this->packetBuff[i]));
			}
			else
			{
				frame->AddUInt(i, 4, CSTR("TotalSize"), ReadUInt32(&this->packetBuff[i]));
			}
		}
	}
	else if (block->blockType == 1)
	{
		frame->AddFieldSeperstor(8, CSTR("Interface Description Block:"));
		UInt16 linkType;
		UInt16 reserved;
		UInt32 snapLen;
		if (this->isBE)
		{
			linkType = ReadMUInt16(&this->packetBuff[8]);
			reserved = ReadMUInt16(&this->packetBuff[10]);
			snapLen = ReadMUInt32(&this->packetBuff[12]);
		}
		else
		{
			linkType = ReadUInt16(&this->packetBuff[8]);
			reserved = ReadUInt16(&this->packetBuff[10]);
			snapLen = ReadUInt32(&this->packetBuff[12]);
		}
		frame->AddUIntName(8, 2, CSTR("LinkType"), linkType, IO::RAWMonitor::LinkTypeGetName(linkType));
		frame->AddHex16(10, CSTR("Reserved"), reserved);
		frame->AddUInt(12, 4, CSTR("Snap Length"), snapLen);
		UInt16 optCode;
		UInt16 optLeng;
		UInt32 i = 16;
		while (i < block->blockLength - 4)
		{
			if (this->isBE)
			{
				optCode = ReadMUInt16(&this->packetBuff[i]);
				optLeng = ReadMUInt16(&this->packetBuff[i + 2]);
			}
			else
			{
				optCode = ReadUInt16(&this->packetBuff[i]);
				optLeng = ReadUInt16(&this->packetBuff[i + 2]);
			}
			frame->AddUInt(i, 2, CSTR("Option Code"), optCode);
			frame->AddUInt(i + 2, 2, CSTR("Option Length"), optLeng);
			if (i + 4 + optLeng > block->blockLength)
			{
				break;
			}
			if (optCode == 0)
			{
				i += 4;
				break;
			}
			else if (optCode == 1)
			{
				frame->AddStrC(i + 4, optLeng, CSTR("Comment"), &this->packetBuff[i + 4]);
			}
			else if (optCode == 2)
			{
				frame->AddStrC(i + 4, optLeng, CSTR("Name"), &this->packetBuff[i + 4]);
			}
			else if (optCode == 3)
			{
				frame->AddStrC(i + 4, optLeng, CSTR("Description"), &this->packetBuff[i + 4]);
			}
			else if (optCode == 4)
			{
				frame->AddIPv4(i + 4, CSTR("IPv4 Address"), &this->packetBuff[i + 4]);
				frame->AddIPv4(i + 8, CSTR("Netmask"), &this->packetBuff[i + 8]);
			}
			else if (optCode == 5)
			{
				Net::SocketUtil::AddressInfo addr;
				Net::SocketUtil::SetAddrInfoV6(addr, &this->packetBuff[i + 4], 0);
				sptr = Text::StrUInt16(Text::StrConcatC(Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff), UTF8STRC("/")), this->packetBuff[i + 20]);
				frame->AddField(i + 4, 17, CSTR("IPv6 Address"), CSTRP(sbuff, sptr));
			}
			else if (optCode == 6)
			{
				frame->AddMACAddr(i + 4, CSTR("MAC Address"), &this->packetBuff[i + 4], true);
			}
			else if (optCode == 7)
			{
				sb.ClearStr();
				sb.AppendHexBuff(&this->packetBuff[i + 4], 8, ':', Text::LineBreakType::None);
				frame->AddField(i + 4, 8, CSTR("EUI Address"), sb.ToCString());
			}
			else if (optCode == 8)
			{
				Int64 speed;
				if (this->isBE)
				{
					speed = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					speed = ReadInt64(&this->packetBuff[i + 4]);
				}
				sptr = Text::StrConcatC(Text::StrInt64(sbuff, speed), UTF8STRC("bps"));
				frame->AddField(i + 4, 8, CSTR("Speed"), CSTRP(sbuff, sptr));
			}
			else if (optCode == 9)
			{
				frame->AddInt(i + 4, 1, CSTR("Timestamps resolution"), (Int8)this->packetBuff[i + 4]);
			}
			else if (optCode == 10)
			{
				Int32 tzone;
				if (this->isBE)
				{
					tzone = ReadMInt32(&this->packetBuff[i + 4]);
				}
				else
				{
					tzone = ReadInt32(&this->packetBuff[i + 4]);
				}
				frame->AddInt(i + 4, 4, CSTR("Time Zone"), tzone);
			}
			else if (optCode == 11)
			{
				sb.ClearStr();
				sb.AppendU16(this->packetBuff[i + 4]);
				if (optLeng > 1)
				{
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendC(&this->packetBuff[i + 5], (UOSInt)optLeng - 1);
					sb.AppendC(UTF8STRC(")"));
				}
				frame->AddField(i + 4, optLeng, CSTR("Filter"), sb.ToCString());
			}
			else if (optCode == 12)
			{
				frame->AddStrC(i + 4, optLeng, CSTR("OS"), &this->packetBuff[i + 4]);
			}
			else if (optCode == 13)
			{
				frame->AddUInt(i + 4, 1, CSTR("Frame Check Sequence Length"), this->packetBuff[i + 4]);
			}
			else if (optCode == 14)
			{
				Int64 tsOffset;
				if (this->isBE)
				{
					tsOffset = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					tsOffset = ReadInt64(&this->packetBuff[i + 4]);
				}
				sptr = Text::StrConcatC(Text::StrInt64(sbuff, tsOffset), UTF8STRC("sec."));
				frame->AddField(i + 4, 8, CSTR("TS Offset"), CSTRP(sbuff, sptr));
			}
			else if (optCode == 15)
			{
				frame->AddStrC(i + 4, optLeng, CSTR("Hardware"), &this->packetBuff[i + 4]);
			}

			i += 4 + (UInt32)optLeng;
			if (i & 3)
			{
				frame->AddHexBuff(i, 4 - (i & 3), CSTR("Padding"), &this->packetBuff[i], false);
				i += 4 - (i & 3);
			}
		}
		if (i == block->blockLength - 4)
		{
			if (this->isBE)
			{
				frame->AddUInt(i, 4, CSTR("TotalSize"), ReadMUInt32(&this->packetBuff[i]));
			}
			else
			{
				frame->AddUInt(i, 4, CSTR("TotalSize"), ReadUInt32(&this->packetBuff[i]));
			}
		}
	}
	else if (block->blockType == 6)
	{
		frame->AddFieldSeperstor(8, CSTR("Enhanced Packet Block:"));
		UInt32 ifId;
		Int64 ts;
		UInt32 capPSize;
		UInt32 oriPSize;
		Data::DateTime dt;
		if (this->isBE)
		{
			ifId = ReadMUInt32(&this->packetBuff[8]);
			ts = ReadMInt64(&this->packetBuff[12]);
			capPSize = ReadMUInt32(&this->packetBuff[20]);
			oriPSize = ReadMUInt32(&this->packetBuff[24]);
		}
		else
		{
			ifId = ReadUInt32(&this->packetBuff[8]);
			ts = (((Int64)ReadInt32(&this->packetBuff[12])) << 32) | ReadUInt32(&this->packetBuff[16]);
			capPSize = ReadUInt32(&this->packetBuff[20]);
			oriPSize = ReadUInt32(&this->packetBuff[24]);
		}
		frame->AddUInt(8, 4, CSTR("Interface ID"), ifId);
		SetTime(&dt, ts, block->timeResol);
		dt.ToLocalTime();
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		frame->AddField(12, 8, CSTR("Time"), CSTRP(sbuff, sptr));
		frame->AddUInt(20, 4, CSTR("Captured Packet Length"), capPSize);
		frame->AddUInt(24, 4, CSTR("Original Packet Length"), oriPSize);
		UInt32 i;
		if (capPSize + 32 <= block->blockLength)
		{
			Net::PacketAnalyzer::PacketDataGetDetail(block->linkType, &this->packetBuff[28], capPSize, 28, frame);

			UInt16 optCode;
			UInt16 optLeng;
			i = 28 + capPSize;
			if (i & 3)
			{
				frame->AddHexBuff(i, 4 - (i & 3), CSTR("Padding"), &this->packetBuff[i], false);
				i += 4 - (i & 3);
			}
			while (i < block->blockLength - 4)
			{
				if (this->isBE)
				{
					optCode = ReadMUInt16(&this->packetBuff[i]);
					optLeng = ReadMUInt16(&this->packetBuff[i + 2]);
				}
				else
				{
					optCode = ReadUInt16(&this->packetBuff[i]);
					optLeng = ReadUInt16(&this->packetBuff[i + 2]);
				}
				frame->AddUInt(i, 2, CSTR("Option Code"), optCode);
				frame->AddUInt(i + 2, 2, CSTR("Option Length"), optLeng);
				if (i + 4 + optLeng > block->blockLength)
				{
					break;
				}
				if (optCode == 0)
				{
					i += 4;
					break;
				}
				else if (optCode == 1)
				{
					frame->AddStrC(i + 4, optLeng, CSTR("Comment"), &this->packetBuff[i + 4]);
				}
				else if (optCode == 2)
				{
					frame->AddStrC(i + 4, optLeng, CSTR("Hardware"), &this->packetBuff[i + 4]);
				}
				else if (optCode == 3)
				{
					frame->AddStrC(i + 4, optLeng, CSTR("OS"), &this->packetBuff[i + 4]);
				}
				else if (optCode == 4)
				{
					frame->AddStrC(i + 4, optLeng, CSTR("User Application"), &this->packetBuff[i + 4]);
				}

				i += 4 + (UInt32)optLeng;
				if (i & 3)
				{
					frame->AddHexBuff(i, 4 - (i & 3), CSTR("Padding"), &this->packetBuff[i], false);
					i += 4 - (i & 3);
				}
			}
		}
		else
		{
			frame->AddHexBuff(28, block->blockLength - 32, CSTR("Unknown"), &this->packetBuff[28], true);
			i = block->blockLength - 4;
		}
		if (i == block->blockLength - 4)
		{
			if (this->isBE)
			{
				frame->AddUInt(i, 4, CSTR("TotalSize"), ReadMUInt32(&this->packetBuff[i]));
			}
			else
			{
				frame->AddUInt(i, 4, CSTR("TotalSize"), ReadUInt32(&this->packetBuff[i]));
			}
		}
	}
	else if (block->blockType == 5)
	{
		frame->AddFieldSeperstor(8, CSTR("Interface Statistics Block:"));
		UInt32 ifId;
		Int64 ts;
		Data::DateTime dt;
		if (this->isBE)
		{
			ifId = ReadMUInt32(&this->packetBuff[8]);
			ts = ReadMInt64(&this->packetBuff[12]);
		}
		else
		{
			ifId = ReadUInt32(&this->packetBuff[8]);
			ts = (((Int64)ReadInt32(&this->packetBuff[12])) << 32) | ReadUInt32(&this->packetBuff[16]);
		}
		frame->AddUInt(8, 4, CSTR("Interface ID"), ifId);
		SetTime(&dt, ts, block->timeResol);
		dt.ToLocalTime();
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		frame->AddField(12, 8, CSTR("Time"), CSTRP(sbuff, sptr));
		UInt16 optCode;
		UInt16 optLeng;
		UInt32 i = 20;
		while (i < block->blockLength - 4)
		{
			if (this->isBE)
			{
				optCode = ReadMUInt16(&this->packetBuff[i]);
				optLeng = ReadMUInt16(&this->packetBuff[i + 2]);
			}
			else
			{
				optCode = ReadUInt16(&this->packetBuff[i]);
				optLeng = ReadUInt16(&this->packetBuff[i + 2]);
			}
			frame->AddUInt(i, 2, CSTR("Option Code"), optCode);
			frame->AddUInt(i + 2, 2, CSTR("Option Length"), optLeng);
			if (i + 4 + optLeng > block->blockLength)
			{
				break;
			}
			if (optCode == 0)
			{
				i += 4;
				break;
			}
			else if (optCode == 1)
			{
				frame->AddStrC(i + 4, optLeng, CSTR("Comment"), &this->packetBuff[i + 4]);
			}
			else if (optCode == 2)
			{
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = (((Int64)ReadInt32(&this->packetBuff[i + 4])) << 32) | ReadUInt32(&this->packetBuff[i + 8]);
				}
				SetTime(&dt, ts, block->timeResol);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				frame->AddField(i + 4, 8, CSTR("Start Time"), CSTRP(sbuff, sptr));
			}
			else if (optCode == 3)
			{
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = (((Int64)ReadInt32(&this->packetBuff[i + 4])) << 32) | ReadUInt32(&this->packetBuff[i + 8]);
				}
				SetTime(&dt, ts, block->timeResol);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				frame->AddField(i + 4, 8, CSTR("End Time"), CSTRP(sbuff, sptr));
			}
			else if (optCode == 4)
			{
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = ReadInt64(&this->packetBuff[i + 4]);
				}
				frame->AddInt64(i + 4, CSTR("Received Packets"), ts);
			}
			else if (optCode == 5)
			{
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = ReadInt64(&this->packetBuff[i + 4]);
				}
				frame->AddInt64(i + 4, CSTR("Dropped Packets"), ts);
			}
			else if (optCode == 6)
			{
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = ReadInt64(&this->packetBuff[i + 4]);
				}
				frame->AddInt64(i + 4, CSTR("Packets Accepted by Filter"), ts);
			}
			else if (optCode == 7)
			{
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = ReadInt64(&this->packetBuff[i + 4]);
				}
				frame->AddInt64(i + 4, CSTR("Packets Dropped by OS"), ts);
			}
			else if (optCode == 8)
			{
				if (this->isBE)
				{
					ts = ReadMInt64(&this->packetBuff[i + 4]);
				}
				else
				{
					ts = ReadInt64(&this->packetBuff[i + 4]);
				}
				frame->AddInt64(i + 4, CSTR("Packets Delivered to the user"), ts);
			}

			i += 4 + (UInt32)optLeng;
			if (i & 3)
			{
				frame->AddHexBuff(i, 4 - (i & 3), CSTR("Padding"), &this->packetBuff[i], false);
				i += 4 - (i & 3);
			}
		}
		if (i == block->blockLength - 4)
		{
			if (this->isBE)
			{
				frame->AddUInt(i, 4, CSTR("TotalSize"), ReadMUInt32(&this->packetBuff[i]));
			}
			else
			{
				frame->AddUInt(i, 4, CSTR("TotalSize"), ReadUInt32(&this->packetBuff[i]));
			}
		}
	}
	return frame;
}

Bool IO::FileAnalyse::PCapngFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::PCapngFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::PCapngFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}

void IO::FileAnalyse::PCapngFileAnalyse::SetTime(Data::DateTime *dt, Int64 ts, Int8 timeResol)
{
	switch (timeResol)
	{
	case 6:
		dt->SetInstant(Data::TimeInstant(ts / 1000000, (UInt32)(ts % 1000000) * 1000));
		break;
	case 9:
		dt->SetInstant(Data::TimeInstant(ts / 1000000000, (UInt32)(ts % 1000000000)));
		break;
	default:
		dt->SetTicks(ts);
		break;
	}
}
