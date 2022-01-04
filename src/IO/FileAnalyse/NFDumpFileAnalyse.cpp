#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/Compress/LZODecompressor.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/FileAnalyse/NFDumpFileAnalyse.h"
#include "Manage/Process.h"
#include "Net/SocketFactory.h"
#include "Sync/Thread.h"

#define LZOPROGRAM L"LZOBlockDecomp.exe"

UInt32 __stdcall IO::FileAnalyse::NFDumpFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::NFDumpFileAnalyse *me = (IO::FileAnalyse::NFDumpFileAnalyse*)userObj;
	UInt64 endOfst;
	UInt64 ofst;
	UInt8 buff[12];
	UInt32 sz;
	IO::FileAnalyse::NFDumpFileAnalyse::PackInfo *pack;
	me->threadRunning = true;
	me->threadStarted = true;

	pack = MemAlloc(IO::FileAnalyse::NFDumpFileAnalyse::PackInfo, 1);
	pack->fileOfst = 0;
	pack->packSize = 140;
	pack->packType = 0;
	me->packs->Add(pack);
	pack = MemAlloc(IO::FileAnalyse::NFDumpFileAnalyse::PackInfo, 1);
	pack->fileOfst = 140;
	pack->packSize = 136;
	pack->packType = 1;
	me->packs->Add(pack);

	endOfst = me->fd->GetDataSize();
	ofst = 276;
	while (ofst <= (endOfst - 12) && !me->threadToStop)
	{
		if (me->pauseParsing)
		{
			Sync::Thread::Sleep(100);
		}
		else
		{
			me->fd->GetRealData(ofst, 12, buff);
			sz = ReadUInt32(&buff[4]);
			if (ofst + sz + 12 > endOfst)
			{
				break;
			}

			pack = MemAlloc(IO::FileAnalyse::NFDumpFileAnalyse::PackInfo, 1);
			pack->fileOfst = ofst;
			pack->packSize = 12;
			pack->packType = 2;
			me->packs->Add(pack);
			
			pack = MemAlloc(IO::FileAnalyse::NFDumpFileAnalyse::PackInfo, 1);
			pack->fileOfst = ofst + 12;
			pack->packSize = sz;
			pack->packType = 3;
			me->packs->Add(pack);

			ofst += sz + 12;
		}
	}
	me->threadRunning = false;
	return 0;
}

UOSInt IO::FileAnalyse::NFDumpFileAnalyse::LZODecompBlock(UInt8 *srcBlock, UOSInt srcSize, UInt8 *outBlock, UOSInt maxOutSize)
{
	Data::Compress::LZODecompressor dec;
	UOSInt destSize;
	if (dec.Decompress(outBlock, &destSize, srcBlock, srcSize))
	{
		return destSize;
	}
	else
	{
		return 0;
	}
}

IO::FileAnalyse::NFDumpFileAnalyse::NFDumpFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	NEW_CLASS(this->extMap, Data::Int32Map<UInt8*>());

	this->hasLZODecomp = true;

	NEW_CLASS(this->packs, Data::SyncArrayList<IO::FileAnalyse::NFDumpFileAnalyse::PackInfo*>());
	fd->GetRealData(0, 256, buff);
	if (buff[0] != 0x0c || buff[1] != 0xa5 || buff[2] != 1 || buff[3] != 0)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	Sync::Thread::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
}

IO::FileAnalyse::NFDumpFileAnalyse::~NFDumpFileAnalyse()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}
	SDEL_CLASS(this->fd);
	LIST_FREE_FUNC(this->packs, MemFree);
	Data::ArrayList<UInt8*> *extList = this->extMap->GetValues();
	LIST_FREE_FUNC(extList, MemFree);
	DEL_CLASS(this->extMap);
	DEL_CLASS(this->packs);
}

const UTF8Char *IO::FileAnalyse::NFDumpFileAnalyse::GetFormatName()
{
	return (const UTF8Char*)"NFDump";
}

UOSInt IO::FileAnalyse::NFDumpFileAnalyse::GetFrameCount()
{
	return this->packs->GetCount();
}

Bool IO::FileAnalyse::NFDumpFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::NFDumpFileAnalyse::PackInfo *pack;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	if (pack->packType == 0)
	{
		sb->AppendC(UTF8STRC("File header"));
	}
	else if (pack->packType == 1)
	{
		sb->AppendC(UTF8STRC("Stat Record"));
	}
	else if (pack->packType == 2)
	{
		sb->AppendC(UTF8STRC("Data Block Header"));
	}
	else if (pack->packType == 3)
	{
		sb->AppendC(UTF8STRC("Data Block"));
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendI32((Int32)pack->packSize);
	return true;
}

Bool IO::FileAnalyse::NFDumpFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[64];
	UInt8 *extBuff;
	IO::FileAnalyse::NFDumpFileAnalyse::PackInfo *pack;
	UInt8 *packBuff;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;

	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	if (pack->packType == 0)
	{
		sb->AppendC(UTF8STRC("File header"));
	}
	else if (pack->packType == 1)
	{
		sb->AppendC(UTF8STRC("Stat Record"));
	}
	else if (pack->packType == 2)
	{
		sb->AppendC(UTF8STRC("Data Block Header"));
	}
	else if (pack->packType == 3)
	{
		sb->AppendC(UTF8STRC("Data Block"));
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendI32((Int32)pack->packSize);

	if (pack->packType == 0)
	{
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);

		sb->AppendC(UTF8STRC("\r\nVersion = "));
		sb->AppendU16(ReadUInt16(&packBuff[2]));
		sb->AppendC(UTF8STRC("\r\nFlags = 0x"));
		sb->AppendHex32V(ReadUInt32(&packBuff[4]));
		if (packBuff[4] & 1)
		{
			sb->AppendC(UTF8STRC(" Compressed"));
		}
		if (packBuff[4] & 2)
		{
			sb->AppendC(UTF8STRC(" Anonimized"));
		}
		if (packBuff[4] & 4)
		{
			sb->AppendC(UTF8STRC(" Catalog"));
		}
		sb->AppendC(UTF8STRC("\r\nNumber of Blocks = "));
		sb->AppendU32(ReadUInt32(&packBuff[8]));
		sb->AppendC(UTF8STRC("\r\nIdentifier = "));
		sb->Append((UTF8Char*)&packBuff[12]);

		MemFree(packBuff);
	}
	else if (pack->packType == 1)
	{
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);

		sb->AppendC(UTF8STRC("\r\nNumber of flows = "));
		sb->AppendI64(ReadInt64(&packBuff[0]));
		sb->AppendC(UTF8STRC("\r\nNumber of bytes = "));
		sb->AppendI64(ReadInt64(&packBuff[8]));
		sb->AppendC(UTF8STRC("\r\nNumber of packets = "));
		sb->AppendI64(ReadInt64(&packBuff[16]));
		sb->AppendC(UTF8STRC("\r\nNumber of flows (TCP) = "));
		sb->AppendI64(ReadInt64(&packBuff[24]));
		sb->AppendC(UTF8STRC("\r\nNumber of flows (UDP) = "));
		sb->AppendI64(ReadInt64(&packBuff[32]));
		sb->AppendC(UTF8STRC("\r\nNumber of flows (ICMP) = "));
		sb->AppendI64(ReadInt64(&packBuff[40]));
		sb->AppendC(UTF8STRC("\r\nNumber of flows (Other) = "));
		sb->AppendI64(ReadInt64(&packBuff[48]));
		sb->AppendC(UTF8STRC("\r\nNumber of bytes (TCP) = "));
		sb->AppendI64(ReadInt64(&packBuff[56]));
		sb->AppendC(UTF8STRC("\r\nNumber of bytes (UDP) = "));
		sb->AppendI64(ReadInt64(&packBuff[64]));
		sb->AppendC(UTF8STRC("\r\nNumber of bytes (ICMP) = "));
		sb->AppendI64(ReadInt64(&packBuff[72]));
		sb->AppendC(UTF8STRC("\r\nNumber of bytes (Other) = "));
		sb->AppendI64(ReadInt64(&packBuff[80]));
		sb->AppendC(UTF8STRC("\r\nNumber of packets (TCP) = "));
		sb->AppendI64(ReadInt64(&packBuff[88]));
		sb->AppendC(UTF8STRC("\r\nNumber of packets (UDP) = "));
		sb->AppendI64(ReadInt64(&packBuff[96]));
		sb->AppendC(UTF8STRC("\r\nNumber of packets (ICMP) = "));
		sb->AppendI64(ReadInt64(&packBuff[104]));
		sb->AppendC(UTF8STRC("\r\nNumber of packets (Other) = "));
		sb->AppendI64(ReadInt64(&packBuff[112]));
		sb->AppendC(UTF8STRC("\r\nFirst seen = "));
		sb->AppendU32(ReadUInt32(&packBuff[120]));
		sb->AppendC(UTF8STRC("\r\nLast seen = "));
		sb->AppendU32(ReadUInt32(&packBuff[124]));
		sb->AppendC(UTF8STRC("\r\nmSec First = "));
		sb->AppendU16(ReadUInt16(&packBuff[128]));
		sb->AppendC(UTF8STRC("\r\nmSec Last = "));
		sb->AppendU16(ReadUInt16(&packBuff[130]));
		sb->AppendC(UTF8STRC("\r\nSequence Failure = "));
		sb->AppendU32(ReadUInt32(&packBuff[132]));

		MemFree(packBuff);
	}
	else if (pack->packType == 2)
	{
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);

		sb->AppendC(UTF8STRC("\r\nNumber of records = "));
		sb->AppendU32(ReadUInt32(&packBuff[0]));
		sb->AppendC(UTF8STRC("\r\nBlock size = "));
		sb->AppendU32(ReadUInt32(&packBuff[4]));
		sb->AppendC(UTF8STRC("\r\nBlock ID = "));
		sb->AppendU16(ReadUInt16(&packBuff[8]));
		sb->AppendC(UTF8STRC("\r\nFlags = "));
		sb->AppendU16(ReadUInt16(&packBuff[10]));
		if (packBuff[10] == 0)
		{
			sb->AppendC(UTF8STRC(" kompatibility"));
		}
		else if (packBuff[10] == 1)
		{
			sb->AppendC(UTF8STRC(" block uncompressed"));
		}
		else if (packBuff[10] == 2)
		{
			sb->AppendC(UTF8STRC(" block compressed"));
		}
		MemFree(packBuff);
	}
	else if (pack->packType == 3)
	{
		UOSInt size = pack->packSize;
		UOSInt dispSize = size;
		if (dispSize > 256)
			dispSize = 256;
		packBuff = MemAlloc(UInt8, size);
		this->fd->GetRealData(pack->fileOfst, size, packBuff);

		sb->AppendC(UTF8STRC("\r\n\r\n"));
		sb->AppendHexBuff(packBuff, dispSize, ' ', Text::LineBreakType::CRLF);
		if (this->hasLZODecomp)
		{
			Data::DateTime dt;
			UOSInt decBuffSize = 1048576 * 5;
			UInt8 *decBuff = MemAlloc(UInt8, decBuffSize);
			decBuffSize = this->LZODecompBlock(packBuff, size, decBuff, decBuffSize);
			if (decBuffSize > 0)
			{
				UInt32 recType;
				UInt32 recSize;
				sb->AppendC(UTF8STRC("\r\n"));
				i = 0;
				while (i < decBuffSize)
				{
					recType = ReadUInt16(&decBuff[i]);
					recSize = ReadUInt16(&decBuff[i + 2]);
					sb->AppendC(UTF8STRC("\r\nRec Size = "));
					sb->AppendU32(recSize);
					sb->AppendC(UTF8STRC(", Rec Type = "));
					sb->AppendU32(recType);
					if (recSize < 4)
						break;
					if (recType == 1)
					{
						sb->AppendC(UTF8STRC(" (CommonRecordV0)"));
						sb->AppendC(UTF8STRC(", Flags = 0x"));
						sb->AppendHex8(decBuff[i + 4]);
						sb->AppendC(UTF8STRC(", Exporter Sys ID = "));
						sb->AppendU16(decBuff[i + 5]);
						sb->AppendC(UTF8STRC(", Ext Map ID = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 6]));
						dt.SetUnixTimestamp(ReadUInt32(&decBuff[i + 12]));
						dt.AddMS(ReadUInt16(&decBuff[i + 8]));
						dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fffzz");
						sb->AppendC(UTF8STRC(", first = "));
						sb->Append(sbuff);
						dt.SetUnixTimestamp(ReadUInt32(&decBuff[i + 16]));
						dt.AddMS(ReadUInt16(&decBuff[i + 10]));
						dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fffzz");
						sb->AppendC(UTF8STRC(", last = "));
						sb->Append(sbuff);

						sb->AppendC(UTF8STRC(", Fwd Status = "));
						sb->AppendU16(decBuff[i + 20]);
						sb->AppendC(UTF8STRC(", TCP Flags = "));
						sb->AppendU16(decBuff[i + 21]);
						sb->AppendC(UTF8STRC(", Prot = "));
						sb->AppendU16(decBuff[i + 22]);
						sb->AppendC(UTF8STRC(", Tos = "));
						sb->AppendU16(decBuff[i + 23]);
						sb->AppendC(UTF8STRC(", src Port = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 24]));
						sb->AppendC(UTF8STRC(", dest port = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 26]));
						j = 28;
						if (decBuff[i + 4] & 1) //IPv6
						{
							sb->AppendC(UTF8STRC(", src IP = "));
//							Net::SocketUtil::GetIPv6Name(sbuff, ReadUInt32(&decBuff[i + j]));
//							sb->Append(sbuff);
							sb->AppendC(UTF8STRC(", dest IP = "));
//							Net::SocketUtil::GetIPv6Name(sbuff, ReadUInt32(&decBuff[i + j + 16]));
//							sb->Append(sbuff);
							j += 32;
						}
						else //IPv4
						{
							sb->AppendC(UTF8STRC(", src IP = "));
							Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
							sb->Append(sbuff);
							sb->AppendC(UTF8STRC(", dest IP = "));
							Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j + 4]));
							sb->Append(sbuff);
							j += 8;
						}
						sb->AppendC(UTF8STRC(", In Pkts = "));
						if (decBuff[i + 4] & 2) //64 bit
						{
							sb->AppendU64(ReadUInt64(&decBuff[i + j]));
							j += 8;
						}
						else //32 bit
						{
							sb->AppendU32(ReadUInt32(&decBuff[i + j]));
							j += 4;
						}
						sb->AppendC(UTF8STRC(", In Bytes = "));
						if (decBuff[i + 4] & 4) //64 bit
						{
							sb->AppendU64(ReadUInt64(&decBuff[i + j]));
							j += 8;
						}
						else //32 bit
						{
							sb->AppendU32(ReadUInt32(&decBuff[i + j]));
							j += 4;
						}
						extBuff = this->extMap->Get(ReadUInt16(&decBuff[i + 6]));
						if (extBuff)
						{
							OSInt extId;
							k = ReadUInt16(&extBuff[2]);
							l = 4;
							while (l < k)
							{
								extId = ReadUInt16(&extBuff[l]);
								if (extId == 4) //EX_IO_SNMP_2
								{
									sb->AppendC(UTF8STRC(", input = "));
									sb->AppendU16(ReadUInt16(&decBuff[i + j]));
									sb->AppendC(UTF8STRC(", output = "));
									sb->AppendU16(ReadUInt16(&decBuff[i + j + 2]));
									j += 4;
								}
								else if (extId == 5) //EX_IO_SNMP_4
								{
									sb->AppendC(UTF8STRC(", input = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j]));
									sb->AppendC(UTF8STRC(", output = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j + 4]));
									j += 8;
								}
								else if (extId == 6) //EX_AS_2
								{
									sb->AppendC(UTF8STRC(", Src AS = "));
									sb->AppendU16(ReadUInt16(&decBuff[i + j]));
									sb->AppendC(UTF8STRC(", Dest AS = "));
									sb->AppendU16(ReadUInt16(&decBuff[i + j + 2]));
									j += 4;
								}
								else if (extId == 7) //EX_AS_4
								{
									sb->AppendC(UTF8STRC(", Src AS = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j]));
									sb->AppendC(UTF8STRC(", dest AS = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j + 4]));
									j += 8;
								}
								else if (extId == 8) //EX_MULTIPLE
								{
									sb->AppendC(UTF8STRC(", Dst Tos = "));
									sb->AppendU16(decBuff[i + j]);
									sb->AppendC(UTF8STRC(", Dir = "));
									sb->AppendU16(decBuff[i + j + 1]);
									sb->AppendC(UTF8STRC(", Src Mask = "));
									sb->AppendU16(decBuff[i + j + 2]);
									sb->AppendC(UTF8STRC(", Dst Mask = "));
									sb->AppendU16(decBuff[i + j + 3]);
									j += 4;
								}
								else if (extId == 9) //EX_NEXT_HOP_v4
								{
									sb->AppendC(UTF8STRC(", Next HOP IP = "));
									Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
									sb->Append(sbuff);
									j += 4;
								}
								else if (extId == 10) //EX_NEXT_HOP_v6
								{
									sb->AppendC(UTF8STRC(", Next HOP IP = "));
//									Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
//									sb->Append(sbuff);
									j += 16;
								}
								else if (extId == 11) //EX_NEXT_HOP_BGP_v4
								{
									sb->AppendC(UTF8STRC(", BGP Next IP = "));
									Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
									sb->Append(sbuff);
									j += 4;
								}
								else if (extId == 12) //EX_NEXT_HOP_BGP_v6
								{
									sb->AppendC(UTF8STRC(", BGP Next IP = "));
//									Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
//									sb->Append(sbuff);
									j += 16;
								}
								else if (extId == 13) //EX_VLAN
								{
									sb->AppendC(UTF8STRC(", Src VLAN = "));
									sb->AppendU16(ReadUInt16(&decBuff[i + j]));
									sb->AppendC(UTF8STRC(", Dst VLAN = "));
									sb->AppendU16(ReadUInt16(&decBuff[i + j + 2]));
									j += 4;
								}
								else if (extId == 14) //EX_OUT_PKG_4
								{
									sb->AppendC(UTF8STRC(", Out Pkts = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j]));
									j += 4;
								}
								else if (extId == 15) //EX_OUT_PKG_8
								{
									sb->AppendC(UTF8STRC(", Out Pkts = "));
									sb->AppendU64(ReadUInt64(&decBuff[i + j]));
									j += 8;
								}
								else if (extId == 16) //EX_OUT_BYTES_4
								{
									sb->AppendC(UTF8STRC(", Out Bytes = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j]));
									j += 4;
								}
								else if (extId == 17) //EX_OUT_BYTES_8
								{
									sb->AppendC(UTF8STRC(", Out Bytes = "));
									sb->AppendU64(ReadUInt64(&decBuff[i + j]));
									j += 8;
								}
								else if (extId == 18) //EX_AGGR_FLOWS_4
								{
									sb->AppendC(UTF8STRC(", Aggr Flows = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j]));
									j += 4;
								}
								else if (extId == 19) //EX_AGGR_FLOWS_8
								{
									sb->AppendC(UTF8STRC(", Aggr Flows = "));
									sb->AppendU64(ReadUInt64(&decBuff[i + j]));
									j += 8;
								}
								else if (extId == 20) //EX_MAC_1
								{
									sb->AppendC(UTF8STRC(", In Src MAC = "));
									sb->AppendHexBuff(&decBuff[i + j + 2], 6, 0, Text::LineBreakType::None);
									sb->AppendC(UTF8STRC(", Out Dst MAC = "));
									sb->AppendHexBuff(&decBuff[i + j + 10], 6, 0, Text::LineBreakType::None);
									j += 16;
								}
								else if (extId == 21) //EX_MAC_2
								{
									sb->AppendC(UTF8STRC(", In Dst MAC = "));
									sb->AppendHexBuff(&decBuff[i + j + 2], 6, 0, Text::LineBreakType::None);
									sb->AppendC(UTF8STRC(", Out Src MAC = "));
									sb->AppendHexBuff(&decBuff[i + j + 10], 6, 0, Text::LineBreakType::None);
									j += 16;
								}
								else if (extId == 22) //EX_MPLS
								{
									sb->AppendC(UTF8STRC(", MPLS LABEL 1 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 2 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 3 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 4 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 5 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 6 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 7 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 8 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 9 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 10 = "));
									j += 40;
								}
								else if (extId == 23) //EX_ROUTER_IP_v4
								{
									sb->AppendC(UTF8STRC(", Router IP = "));
									Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
									sb->Append(sbuff);
									j += 4;
								}
								else if (extId == 24) //EX_ROUTER_IP_v6
								{
									sb->AppendC(UTF8STRC(", Router IP = "));
//									Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
//									sb->Append(sbuff);
									j += 16;
								}
								else if (extId == 25) //EX_ROUTER_ID
								{
									sb->AppendC(UTF8STRC(", Engine Type = "));
									sb->AppendU16(decBuff[i + j + 2]);
									sb->AppendC(UTF8STRC(", Engine ID = "));
									sb->AppendU16(decBuff[i + j + 3]);
									j += 4;
								}
								else if (extId == 26) //EX_BGPADJ
								{
									sb->AppendC(UTF8STRC(", BGP Next Adjacent AS Number = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j]));
									sb->AppendC(UTF8STRC(", BGP Prev Adjacent AS Number = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j + 4]));
									j += 8;
								}
								else if (extId == 27) //EX_BGPADJ
								{
									sb->AppendC(UTF8STRC(", T Received = "));
									dt.SetTicks(ReadInt64(&decBuff[i + j]));
									dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fffzz");
									sb->Append(sbuff);
									j += 8;
								}
								if (j >= recSize)
									break;
								l += 2;
							}
						}
					}
					else if (recType == 2)
					{
						sb->AppendC(UTF8STRC(" (ExtensionMap)"));
						sb->AppendC(UTF8STRC(", Map ID = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 4]));
						sb->AppendC(UTF8STRC(", Extension Size = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 6]));
						j = 1;
						k = 8;
						while (k < recSize)
						{
							l = ReadUInt16(&decBuff[i + k]);
							if (l == 0)
								break;
							sb->AppendC(UTF8STRC(", Extension ID"));
							sb->AppendUOSInt(j);
							sb->AppendC(UTF8STRC(" = "));
							sb->AppendUOSInt(l);
							j++;
							k += 2;
						}
						extBuff = this->extMap->Get(ReadUInt16(&decBuff[i + 4]));
						if (extBuff == 0)
						{
							extBuff = MemAlloc(UInt8, recSize - 4);
							MemCopyNO(extBuff, &decBuff[i + 4], recSize - 4);
							WriteInt16(&extBuff[2], recSize - 4);
							this->extMap->Put(ReadUInt16(&decBuff[i + 4]), extBuff);
						}
					}
					else if (recType == 3)
					{
						sb->AppendC(UTF8STRC(" (PortHistogram)"));
					}
					else if (recType == 4)
					{
						sb->AppendC(UTF8STRC(" (BppHistogram)"));
					}
					else if (recType == 5)
					{
						sb->AppendC(UTF8STRC(" (ExporterRecord)"));
					}
					else if (recType == 6)
					{
						sb->AppendC(UTF8STRC(" (SamplerRecord)"));
					}
					else if (recType == 7)
					{
						sb->AppendC(UTF8STRC(" (ExporterInfoRecord)"));
						sb->AppendC(UTF8STRC(", version = "));
						sb->AppendU32(ReadUInt32(&decBuff[i + 4]));
						sb->AppendC(UTF8STRC(", SA Family = "));
						sb->AppendUOSInt(j = ReadUInt16(&decBuff[i + 24]));
						sb->AppendC(UTF8STRC(", IP = "));
						if (j == 2) //AF_INET
						{
							Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + 16]));
							sb->Append(sbuff);
						}
						else if (j == 23) //AF_INET6
						{
//							Net::SocketUtil::GetIPv4Name(sbuff, ReadUInt32(&decBuff[i + 16]));
//							sb->Append(sbuff);
						}
						else
						{
							sb->AppendHexBuff(&decBuff[i + 8], 16, 0, Text::LineBreakType::None);
						}
						sb->AppendC(UTF8STRC(", Sys ID = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 26]));
						sb->AppendC(UTF8STRC(", SA Family = "));
						sb->AppendU32(ReadUInt32(&decBuff[i + 28]));
					}
					else if (recType == 8)
					{
						sb->AppendC(UTF8STRC(" (ExporterStatRecord)"));
						sb->AppendC(UTF8STRC(", Stat Cnt = "));
						j = 8;
						l = ReadUInt32(&decBuff[i + 4]);
						k = 0;
						sb->AppendU32((UInt32)l);
						while (k < l)
						{
							sb->AppendC(UTF8STRC(", Sys ID"));
							sb->AppendUOSInt(k);
							sb->AppendC(UTF8STRC(" = "));
							sb->AppendU32(ReadUInt32(&decBuff[i + j]));
							sb->AppendC(UTF8STRC(", Seq Failure"));
							sb->AppendUOSInt(k);
							sb->AppendC(UTF8STRC(" = "));
							sb->AppendU32(ReadUInt32(&decBuff[i + j + 4]));
							sb->AppendC(UTF8STRC(", Packets"));
							sb->AppendUOSInt(k);
							sb->AppendC(UTF8STRC(" = "));
							sb->AppendU64(ReadUInt64(&decBuff[i + j + 8]));
							sb->AppendC(UTF8STRC(", Flows"));
							sb->AppendUOSInt(k);
							sb->AppendC(UTF8STRC(" = "));
							sb->AppendU64(ReadUInt64(&decBuff[i + j + 16]));
							j += 24;
							k++;
						}
					}
					else if (recType == 9)
					{
						sb->AppendC(UTF8STRC(" (SamplerInfoRecord)"));
						sb->AppendC(UTF8STRC(", ID = "));
						sb->AppendI32(ReadInt32(&decBuff[i + 4]));
						sb->AppendC(UTF8STRC(", Interval = "));
						sb->AppendU32(ReadUInt32(&decBuff[i + 8]));
						sb->AppendC(UTF8STRC(", Mode = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 12]));
						sb->AppendC(UTF8STRC(", Exporter Sys ID = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 14]));
					}
					else if (recType == 10)
					{
						sb->AppendC(UTF8STRC(" (CommonRecord)"));
					}
					i += recSize;
				}
			}
			MemFree(decBuff);
		}
		MemFree(packBuff);
	}
	return true;
}

UOSInt IO::FileAnalyse::NFDumpFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs->GetCount() - 1;
	OSInt k;
	PackInfo *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs->GetItem((UOSInt)k);
		if (ofst < pack->fileOfst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->fileOfst + pack->packSize)
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

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::NFDumpFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail *frame;
	IO::FileAnalyse::NFDumpFileAnalyse::PackInfo *pack;
	UInt8 *packBuff;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return 0;

	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, (UInt32)pack->packSize));
	if (pack->packType == 0)
	{
		frame->AddHeader((const UTF8Char*)"Type=File header");
	}
	else if (pack->packType == 1)
	{
		frame->AddHeader((const UTF8Char*)"Type=Stat Record");
	}
	else if (pack->packType == 2)
	{
		frame->AddHeader((const UTF8Char*)"Type=Data Block Header");
	}
	else if (pack->packType == 3)
	{
		frame->AddHeader((const UTF8Char*)"Type=Data Block");
	}

	if (pack->packType == 0)
	{
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		frame->AddUInt(2, 2, "Version", ReadUInt16(&packBuff[2]));
		frame->AddHex32(4, "Flags", ReadUInt32(&packBuff[4]));
		if (packBuff[4] & 1)
		{
			frame->AddField(4, 4, (const UTF8Char*)"Flags", (const UTF8Char*)" Compressed");
		}
		if (packBuff[4] & 2)
		{
			frame->AddField(4, 4, (const UTF8Char*)"Flags", (const UTF8Char*)" Anonimized");
		}
		if (packBuff[4] & 4)
		{
			frame->AddField(4, 4, (const UTF8Char*)"Flags", (const UTF8Char*)" Catalog");
		}
		frame->AddUInt(8, 4, "Number of Blocks", ReadUInt32(&packBuff[8]));
		UOSInt strLen = Text::StrCharCnt(&packBuff[12]);
		frame->AddStrS(12, strLen + 1, "Identifier", &packBuff[12]);
		MemFree(packBuff);
	}
	else if (pack->packType == 1)
	{
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		frame->AddUInt64(0, "Number of flows", ReadUInt64(&packBuff[0]));
		frame->AddUInt64(8, "Number of bytes", ReadUInt64(&packBuff[8]));
		frame->AddUInt64(16, "Number of packets", ReadUInt64(&packBuff[16]));
		frame->AddUInt64(24, "Number of flows (TCP)", ReadUInt64(&packBuff[24]));
		frame->AddUInt64(32, "Number of flows (UDP)", ReadUInt64(&packBuff[32]));
		frame->AddUInt64(40, "Number of flows (ICMP)", ReadUInt64(&packBuff[40]));
		frame->AddUInt64(48, "Number of flows (Other)", ReadUInt64(&packBuff[48]));
		frame->AddUInt64(56, "Number of bytes (TCP)", ReadUInt64(&packBuff[56]));
		frame->AddUInt64(64, "Number of bytes (UDP)", ReadUInt64(&packBuff[64]));
		frame->AddUInt64(72, "Number of bytes (ICMP)", ReadUInt64(&packBuff[72]));
		frame->AddUInt64(80, "Number of bytes (Other)", ReadUInt64(&packBuff[80]));
		frame->AddUInt64(88, "Number of packets (TCP)", ReadUInt64(&packBuff[88]));
		frame->AddUInt64(96, "Number of packets (UDP)", ReadUInt64(&packBuff[96]));
		frame->AddUInt64(104, "Number of packets (ICMP)", ReadUInt64(&packBuff[104]));
		frame->AddUInt64(112, "Number of packets (Other)", ReadUInt64(&packBuff[112]));
		frame->AddUInt(120, 4, "Number of flows", ReadUInt32(&packBuff[120]));
		frame->AddUInt(124, 4, "Number of flows", ReadUInt32(&packBuff[124]));
		frame->AddUInt(128, 2, "Number of flows", ReadUInt16(&packBuff[128]));
		frame->AddUInt(130, 2, "Number of flows", ReadUInt16(&packBuff[130]));
		frame->AddUInt(132, 4, "Number of flows", ReadUInt32(&packBuff[132]));

		MemFree(packBuff);
	}
	else if (pack->packType == 2)
	{
		packBuff = MemAlloc(UInt8, pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		frame->AddUInt(0, 4, "Number of records", ReadUInt32(&packBuff[0]));
		frame->AddUInt(4, 4, "Block size", ReadUInt32(&packBuff[4]));
		frame->AddUInt(8, 2, "Block ID", ReadUInt16(&packBuff[8]));
		frame->AddUInt(10, 2, "Flags", ReadUInt16(&packBuff[10]));
		if (packBuff[10] == 0)
		{
			frame->AddField(10, 2, (const UTF8Char*)"Flags", (const UTF8Char*)"kompatibility");
		}
		else if (packBuff[10] == 1)
		{
			frame->AddField(10, 2, (const UTF8Char*)"Flags", (const UTF8Char*)"block uncompressed");
		}
		else if (packBuff[10] == 2)
		{
			frame->AddField(10, 2, (const UTF8Char*)"Flags", (const UTF8Char*)"block compressed");
		}
		MemFree(packBuff);
	}
	else if (pack->packType == 3)
	{
		UOSInt size = pack->packSize;
		UOSInt dispSize = size;
		packBuff = MemAlloc(UInt8, size);
		this->fd->GetRealData(pack->fileOfst, size, packBuff);

		if (dispSize > 256)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(packBuff, 256, ' ', Text::LineBreakType::CRLF);
			frame->AddField(0, dispSize, (const UTF8Char*)"LZO Compressed", sb.ToString());
			dispSize = 256;
		}
		else
		{
			frame->AddHexBuff(0, dispSize, "LZO Compressed", packBuff, true);
		}

		if (this->hasLZODecomp)
		{
			Data::DateTime dt;
			UOSInt decBuffSize = 1048576 * 5;
			UInt8 *decBuff = MemAlloc(UInt8, decBuffSize);
			decBuffSize = this->LZODecompBlock(packBuff, size, decBuff, decBuffSize);
			if (decBuffSize > 0)
			{
/*				UInt8 *extBuff;
				UOSInt i;
				UOSInt j;
				UOSInt k;
				UOSInt l;
				UInt32 recType;
				UInt32 recSize;
				sb->AppendC(UTF8STRC("\r\n"));
				i = 0;
				while (i < decBuffSize)
				{
					recType = ReadUInt16(&decBuff[i]);
					recSize = ReadUInt16(&decBuff[i + 2]);
					sb->AppendC(UTF8STRC("\r\nRec Size = "));
					sb->AppendU32(recSize);
					sb->AppendC(UTF8STRC(", Rec Type = "));
					sb->AppendU32(recType);
					if (recSize < 4)
						break;
					if (recType == 1)
					{
						sb->AppendC(UTF8STRC(" (CommonRecordV0)"));
						sb->AppendC(UTF8STRC(", Flags = 0x"));
						sb->AppendHex8(decBuff[i + 4]);
						sb->AppendC(UTF8STRC(", Exporter Sys ID = "));
						sb->AppendU16(decBuff[i + 5]);
						sb->AppendC(UTF8STRC(", Ext Map ID = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 6]));
						dt.SetUnixTimestamp(ReadUInt32(&decBuff[i + 12]));
						dt.AddMS(ReadUInt16(&decBuff[i + 8]));
						dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fffzz"));
						sb->AppendC(UTF8STRC(", first = "));
						sb->Append(sbuff);
						dt.SetUnixTimestamp(ReadUInt32(&decBuff[i + 16]));
						dt.AddMS(ReadUInt16(&decBuff[i + 10]));
						dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fffzz"));
						sb->AppendC(UTF8STRC(", last = "));
						sb->Append(sbuff);

						sb->AppendC(UTF8STRC(", Fwd Status = "));
						sb->AppendU16(decBuff[i + 20]);
						sb->AppendC(UTF8STRC(", TCP Flags = "));
						sb->AppendU16(decBuff[i + 21]);
						sb->AppendC(UTF8STRC(", Prot = "));
						sb->AppendU16(decBuff[i + 22]);
						sb->AppendC(UTF8STRC(", Tos = "));
						sb->AppendU16(decBuff[i + 23]);
						sb->AppendC(UTF8STRC(", src Port = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 24]));
						sb->AppendC(UTF8STRC(", dest port = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 26]));
						j = 28;
						if (decBuff[i + 4] & 1) //IPv6
						{
							sb->AppendC(UTF8STRC(", src IP = "));
//							Net::SocketUtil::GetIPv6Name(sbuff, ReadUInt32(&decBuff[i + j]));
//							sb->Append(sbuff);
							sb->AppendC(UTF8STRC(", dest IP = "));
//							Net::SocketUtil::GetIPv6Name(sbuff, ReadUInt32(&decBuff[i + j + 16]));
//							sb->Append(sbuff);
							j += 32;
						}
						else //IPv4
						{
							sb->AppendC(UTF8STRC(", src IP = "));
							Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
							sb->Append(sbuff);
							sb->AppendC(UTF8STRC(", dest IP = "));
							Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j + 4]));
							sb->Append(sbuff);
							j += 8;
						}
						sb->AppendC(UTF8STRC(", In Pkts = "));
						if (decBuff[i + 4] & 2) //64 bit
						{
							sb->AppendU64(ReadUInt64(&decBuff[i + j]));
							j += 8;
						}
						else //32 bit
						{
							sb->AppendU32(ReadUInt32(&decBuff[i + j]));
							j += 4;
						}
						sb->AppendC(UTF8STRC(", In Bytes = "));
						if (decBuff[i + 4] & 4) //64 bit
						{
							sb->AppendU64(ReadUInt64(&decBuff[i + j]));
							j += 8;
						}
						else //32 bit
						{
							sb->AppendU32(ReadUInt32(&decBuff[i + j]));
							j += 4;
						}
						extBuff = this->extMap->Get(ReadUInt16(&decBuff[i + 6]));
						if (extBuff)
						{
							OSInt extId;
							k = ReadUInt16(&extBuff[2]);
							l = 4;
							while (l < k)
							{
								extId = ReadUInt16(&extBuff[l]);
								if (extId == 4) //EX_IO_SNMP_2
								{
									sb->AppendC(UTF8STRC(", input = "));
									sb->AppendU16(ReadUInt16(&decBuff[i + j]));
									sb->AppendC(UTF8STRC(", output = "));
									sb->AppendU16(ReadUInt16(&decBuff[i + j + 2]));
									j += 4;
								}
								else if (extId == 5) //EX_IO_SNMP_4
								{
									sb->AppendC(UTF8STRC(", input = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j]));
									sb->AppendC(UTF8STRC(", output = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j + 4]));
									j += 8;
								}
								else if (extId == 6) //EX_AS_2
								{
									sb->AppendC(UTF8STRC(", Src AS = "));
									sb->AppendU16(ReadUInt16(&decBuff[i + j]));
									sb->AppendC(UTF8STRC(", Dest AS = "));
									sb->AppendU16(ReadUInt16(&decBuff[i + j + 2]));
									j += 4;
								}
								else if (extId == 7) //EX_AS_4
								{
									sb->AppendC(UTF8STRC(", Src AS = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j]));
									sb->AppendC(UTF8STRC(", dest AS = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j + 4]));
									j += 8;
								}
								else if (extId == 8) //EX_MULTIPLE
								{
									sb->AppendC(UTF8STRC(", Dst Tos = "));
									sb->AppendU16(decBuff[i + j]);
									sb->AppendC(UTF8STRC(", Dir = "));
									sb->AppendU16(decBuff[i + j + 1]);
									sb->AppendC(UTF8STRC(", Src Mask = "));
									sb->AppendU16(decBuff[i + j + 2]);
									sb->AppendC(UTF8STRC(", Dst Mask = "));
									sb->AppendU16(decBuff[i + j + 3]);
									j += 4;
								}
								else if (extId == 9) //EX_NEXT_HOP_v4
								{
									sb->AppendC(UTF8STRC(", Next HOP IP = "));
									Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
									sb->Append(sbuff);
									j += 4;
								}
								else if (extId == 10) //EX_NEXT_HOP_v6
								{
									sb->AppendC(UTF8STRC(", Next HOP IP = "));
//									Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
//									sb->Append(sbuff);
									j += 16;
								}
								else if (extId == 11) //EX_NEXT_HOP_BGP_v4
								{
									sb->AppendC(UTF8STRC(", BGP Next IP = "));
									Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
									sb->Append(sbuff);
									j += 4;
								}
								else if (extId == 12) //EX_NEXT_HOP_BGP_v6
								{
									sb->AppendC(UTF8STRC(", BGP Next IP = "));
//									Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
//									sb->Append(sbuff);
									j += 16;
								}
								else if (extId == 13) //EX_VLAN
								{
									sb->AppendC(UTF8STRC(", Src VLAN = "));
									sb->AppendU16(ReadUInt16(&decBuff[i + j]));
									sb->AppendC(UTF8STRC(", Dst VLAN = "));
									sb->AppendU16(ReadUInt16(&decBuff[i + j + 2]));
									j += 4;
								}
								else if (extId == 14) //EX_OUT_PKG_4
								{
									sb->AppendC(UTF8STRC(", Out Pkts = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j]));
									j += 4;
								}
								else if (extId == 15) //EX_OUT_PKG_8
								{
									sb->AppendC(UTF8STRC(", Out Pkts = "));
									sb->AppendU64(ReadUInt64(&decBuff[i + j]));
									j += 8;
								}
								else if (extId == 16) //EX_OUT_BYTES_4
								{
									sb->AppendC(UTF8STRC(", Out Bytes = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j]));
									j += 4;
								}
								else if (extId == 17) //EX_OUT_BYTES_8
								{
									sb->AppendC(UTF8STRC(", Out Bytes = "));
									sb->AppendU64(ReadUInt64(&decBuff[i + j]));
									j += 8;
								}
								else if (extId == 18) //EX_AGGR_FLOWS_4
								{
									sb->AppendC(UTF8STRC(", Aggr Flows = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j]));
									j += 4;
								}
								else if (extId == 19) //EX_AGGR_FLOWS_8
								{
									sb->AppendC(UTF8STRC(", Aggr Flows = "));
									sb->AppendU64(ReadUInt64(&decBuff[i + j]));
									j += 8;
								}
								else if (extId == 20) //EX_MAC_1
								{
									sb->AppendC(UTF8STRC(", In Src MAC = "));
									sb->AppendHexBuff(&decBuff[i + j + 2], 6, 0, Text::LineBreakType::None);
									sb->AppendC(UTF8STRC(", Out Dst MAC = "));
									sb->AppendHexBuff(&decBuff[i + j + 10], 6, 0, Text::LineBreakType::None);
									j += 16;
								}
								else if (extId == 21) //EX_MAC_2
								{
									sb->AppendC(UTF8STRC(", In Dst MAC = "));
									sb->AppendHexBuff(&decBuff[i + j + 2], 6, 0, Text::LineBreakType::None);
									sb->AppendC(UTF8STRC(", Out Src MAC = "));
									sb->AppendHexBuff(&decBuff[i + j + 10], 6, 0, Text::LineBreakType::None);
									j += 16;
								}
								else if (extId == 22) //EX_MPLS
								{
									sb->AppendC(UTF8STRC(", MPLS LABEL 1 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 2 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 3 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 4 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 5 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 6 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 7 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 8 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 9 = "));
									sb->AppendC(UTF8STRC(", MPLS LABEL 10 = "));
									j += 40;
								}
								else if (extId == 23) //EX_ROUTER_IP_v4
								{
									sb->AppendC(UTF8STRC(", Router IP = "));
									Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
									sb->Append(sbuff);
									j += 4;
								}
								else if (extId == 24) //EX_ROUTER_IP_v6
								{
									sb->AppendC(UTF8STRC(", Router IP = "));
//									Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + j]));
//									sb->Append(sbuff);
									j += 16;
								}
								else if (extId == 25) //EX_ROUTER_ID
								{
									sb->AppendC(UTF8STRC(", Engine Type = "));
									sb->AppendU16(decBuff[i + j + 2]);
									sb->AppendC(UTF8STRC(", Engine ID = "));
									sb->AppendU16(decBuff[i + j + 3]);
									j += 4;
								}
								else if (extId == 26) //EX_BGPADJ
								{
									sb->AppendC(UTF8STRC(", BGP Next Adjacent AS Number = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j]));
									sb->AppendC(UTF8STRC(", BGP Prev Adjacent AS Number = "));
									sb->AppendU32(ReadUInt32(&decBuff[i + j + 4]));
									j += 8;
								}
								else if (extId == 27) //EX_BGPADJ
								{
									sb->AppendC(UTF8STRC(", T Received = "));
									dt.SetTicks(ReadInt64(&decBuff[i + j]));
									dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fffzz"));
									sb->Append(sbuff);
									j += 8;
								}
								if (j >= recSize)
									break;
								l += 2;
							}
						}
					}
					else if (recType == 2)
					{
						sb->AppendC(UTF8STRC(" (ExtensionMap)"));
						sb->AppendC(UTF8STRC(", Map ID = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 4]));
						sb->AppendC(UTF8STRC(", Extension Size = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 6]));
						j = 1;
						k = 8;
						while (k < recSize)
						{
							l = ReadUInt16(&decBuff[i + k]);
							if (l == 0)
								break;
							sb->AppendC(UTF8STRC(", Extension ID"));
							sb->AppendUOSInt(j);
							sb->AppendC(UTF8STRC(" = "));
							sb->AppendUOSInt(l);
							j++;
							k += 2;
						}
						extBuff = this->extMap->Get(ReadUInt16(&decBuff[i + 4]));
						if (extBuff == 0)
						{
							extBuff = MemAlloc(UInt8, recSize - 4);
							MemCopyNO(extBuff, &decBuff[i + 4], recSize - 4);
							WriteInt16(&extBuff[2], recSize - 4);
							this->extMap->Put(ReadUInt16(&decBuff[i + 4]), extBuff);
						}
					}
					else if (recType == 3)
					{
						sb->AppendC(UTF8STRC(" (PortHistogram)"));
					}
					else if (recType == 4)
					{
						sb->AppendC(UTF8STRC(" (BppHistogram)"));
					}
					else if (recType == 5)
					{
						sb->AppendC(UTF8STRC(" (ExporterRecord)"));
					}
					else if (recType == 6)
					{
						sb->AppendC(UTF8STRC(" (SamplerRecord)"));
					}
					else if (recType == 7)
					{
						sb->AppendC(UTF8STRC(" (ExporterInfoRecord)"));
						sb->AppendC(UTF8STRC(", version = "));
						sb->AppendU32(ReadUInt32(&decBuff[i + 4]));
						sb->AppendC(UTF8STRC(", SA Family = "));
						sb->AppendUOSInt(j = ReadUInt16(&decBuff[i + 24]));
						sb->AppendC(UTF8STRC(", IP = "));
						if (j == 2) //AF_INET
						{
							Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32(&decBuff[i + 16]));
							sb->Append(sbuff);
						}
						else if (j == 23) //AF_INET6
						{
//							Net::SocketUtil::GetIPv4Name(sbuff, ReadUInt32(&decBuff[i + 16]));
//							sb->Append(sbuff);
						}
						else
						{
							sb->AppendHexBuff(&decBuff[i + 8], 16, 0, Text::LineBreakType::None);
						}
						sb->AppendC(UTF8STRC(", Sys ID = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 26]));
						sb->AppendC(UTF8STRC(", SA Family = "));
						sb->AppendU32(ReadUInt32(&decBuff[i + 28]));
					}
					else if (recType == 8)
					{
						sb->AppendC(UTF8STRC(" (ExporterStatRecord)"));
						sb->AppendC(UTF8STRC(", Stat Cnt = "));
						j = 8;
						l = ReadUInt32(&decBuff[i + 4]);
						k = 0;
						sb->AppendU32((UInt32)l);
						while (k < l)
						{
							sb->AppendC(UTF8STRC(", Sys ID"));
							sb->AppendUOSInt(k);
							sb->AppendC(UTF8STRC(" = "));
							sb->AppendU32(ReadUInt32(&decBuff[i + j]));
							sb->AppendC(UTF8STRC(", Seq Failure"));
							sb->AppendUOSInt(k);
							sb->AppendC(UTF8STRC(" = "));
							sb->AppendU32(ReadUInt32(&decBuff[i + j + 4]));
							sb->AppendC(UTF8STRC(", Packets"));
							sb->AppendUOSInt(k);
							sb->AppendC(UTF8STRC(" = "));
							sb->AppendU64(ReadUInt64(&decBuff[i + j + 8]));
							sb->AppendC(UTF8STRC(", Flows"));
							sb->AppendUOSInt(k);
							sb->AppendC(UTF8STRC(" = "));
							sb->AppendU64(ReadUInt64(&decBuff[i + j + 16]));
							j += 24;
							k++;
						}
					}
					else if (recType == 9)
					{
						sb->AppendC(UTF8STRC(" (SamplerInfoRecord)"));
						sb->AppendC(UTF8STRC(", ID = "));
						sb->AppendI32(ReadInt32(&decBuff[i + 4]));
						sb->AppendC(UTF8STRC(", Interval = "));
						sb->AppendU32(ReadUInt32(&decBuff[i + 8]));
						sb->AppendC(UTF8STRC(", Mode = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 12]));
						sb->AppendC(UTF8STRC(", Exporter Sys ID = "));
						sb->AppendU16(ReadUInt16(&decBuff[i + 14]));
					}
					else if (recType == 10)
					{
						sb->AppendC(UTF8STRC(" (CommonRecord)"));
					}
					i += recSize;
				}*/
			}
			MemFree(decBuff);
		}
		MemFree(packBuff);
	}
	return frame;
}

Bool IO::FileAnalyse::NFDumpFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::NFDumpFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::NFDumpFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
	return false;
}
