#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/FileAnalyse/MPEGFileAnalyse.h"
#include "Sync/SimpleThread.h"

void __stdcall IO::FileAnalyse::MPEGFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::MPEGFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::MPEGFileAnalyse>();
	UInt8 readBuff[256];
	UInt64 currOfst;
	UInt64 readOfst;
	UOSInt buffSize;
	UOSInt readSize;
	UOSInt frameSize;
	UOSInt j;
	NN<IO::FileAnalyse::MPEGFileAnalyse::PackInfo> pack;

	currOfst = 0;
	readOfst = 0;
	buffSize = 0;
	while (!thread->IsStopping())
	{
		if (me->pauseParsing)
		{
			Sync::SimpleThread::Sleep(100);
		}
		else
		{
			if (buffSize < 256)
			{
				readSize = me->fd->GetRealData(readOfst, 256 - buffSize, BYTEARR(readBuff).SubArray(buffSize));
				buffSize += readSize;
				readOfst += readSize;
			}
			j = 0;
			while (readBuff[j] != 0 || readBuff[j + 1] != 0 || readBuff[j + 2] != 1)
			{
				j++;
				if (j >= buffSize - 4)
				{
					break;
				}
			}
			if (j >= buffSize - 4 && buffSize > 4)
			{
				MemCopyO(readBuff, &readBuff[j], buffSize - j);
				buffSize -= j;
				currOfst += j;
				continue;
			}
			if (readBuff[j] != 0 || readBuff[j + 1] != 0 || readBuff[j + 2] != 1 || buffSize < 4)
			{
				break;
			}
			if (readBuff[j + 3] == 0xB9) //End Of File
			{
				pack = MemAllocNN(IO::FileAnalyse::MPEGFileAnalyse::PackInfo);
				pack->fileOfst = currOfst + j;
				pack->packSize = 4;
				pack->packType = 0xb9;
				me->packs.Add(pack);
				break;
			}
			if (readBuff[j + 3] == 0xba) 
			{
				if ((readBuff[j + 4] & 0xc0) == 0x40)
				{
					frameSize = 14 + (UOSInt)(readBuff[j + 13] & 7);
				}
				else if ((readBuff[j + 4] & 0xf0) == 0x20)
				{
					frameSize = 12;
				}
				else
				{
					break;
				}
				pack = MemAllocNN(IO::FileAnalyse::MPEGFileAnalyse::PackInfo);
				pack->fileOfst = currOfst + j;
				pack->packSize = frameSize;
				pack->packType = 0xba;
				me->packs.Add(pack);
			}
			else
			{
				frameSize = 6 + (UOSInt)ReadMUInt16(&readBuff[j + 4]);
				pack = MemAllocNN(IO::FileAnalyse::MPEGFileAnalyse::PackInfo);
				pack->fileOfst = currOfst + j;
				pack->packSize = frameSize;
				pack->packType = readBuff[j + 3];
				me->packs.Add(pack);
			}
			if (j + frameSize < buffSize)
			{
				MemCopyO(readBuff, &readBuff[j + frameSize], buffSize - j - frameSize);
				buffSize -= j + frameSize;
				currOfst += j + frameSize;
			}
			else
			{
				currOfst += j + frameSize;
				readOfst += j + frameSize - buffSize;
				buffSize = 0;
			}
		}
	}
}

IO::FileAnalyse::MPEGFileAnalyse::MPEGFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("MPEGFileAnalyse"))
{
	UInt8 buff[256];
	this->fd = 0;
	this->pauseParsing = false;
	fd->GetRealData(0, 256, BYTEARR(buff));
	if (ReadMInt32(buff) != 0x000001ba)
	{
		return;
	}
	if ((buff[4] & 0xc0) == 0x40)
	{
		this->mpgVer = 2;
	}
	else if ((buff[4] & 0xf0) == 0x20)
	{
		this->mpgVer = 1;
	}
	else
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::MPEGFileAnalyse::~MPEGFileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
	this->packs.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::MPEGFileAnalyse::GetFormatName()
{
	return CSTR("MPEG");
}

UOSInt IO::FileAnalyse::MPEGFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::MPEGFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::MPEGFileAnalyse::PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type=0x"));
	sb->AppendHexBuff(&pack->packType, 1, 0, Text::LineBreakType::None);
	switch (pack->packType)
	{
	case 0xb9:
		sb->AppendC(UTF8STRC(" (MPEG program end)"));
		break;
	case 0xba:
		sb->AppendC(UTF8STRC(" (Pack start)"));
		break;
	case 0xbb:
		sb->AppendC(UTF8STRC(" (System header)"));
		break;
	case 0xbc:
		sb->AppendC(UTF8STRC(" (Program Stream map)"));
		break;
	case 0xbd:
		sb->AppendC(UTF8STRC(" (Private Stream 1)"));
		break;
	case 0xbe:
		sb->AppendC(UTF8STRC(" (Padding Stream)"));
		break;
	case 0xbf:
		sb->AppendC(UTF8STRC(" (Private Stream 2)"));
		break;
	case 0xc0:
		sb->AppendC(UTF8STRC(" (Audio Stream 1)"));
		break;
	case 0xe0:
		sb->AppendC(UTF8STRC(" (Video Stream)"));
		break;
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendI32((Int32)pack->packSize);
	return true;
}

Bool IO::FileAnalyse::MPEGFileAnalyse::GetFrameDetail(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::MPEGFileAnalyse::PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;

	Data::ByteBuffer packBuff(pack->packSize);
	this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);

	switch (packBuff[3])
	{
	case 0xb9:
		sb->AppendC(UTF8STRC("MPEG program end"));
		break;
	case 0xba:
		sb->AppendC(UTF8STRC("Pack start"));
		if ((packBuff[4] & 0xc0) == 0x40)
		{
			Int64 scr_base;
			Int32 scr_ext;
			sb->AppendC(UTF8STRC("\r\nMPEG Version = 2"));
			scr_base = (((Int64)(packBuff[4] & 0x38)) << 27) | ((packBuff[4] & 3) << 28) | (packBuff[5] << 20) | ((packBuff[6] & 0xf8) << 12) | ((packBuff[6] & 3) << 13) | (packBuff[7] << 5) | (packBuff[8] >> 3);
			scr_ext = ((packBuff[8] & 3) << 7) | (packBuff[9] >> 1);
			sb->AppendC(UTF8STRC("\r\nSystem Clock Reference Base = "));
			sb->AppendI64(scr_base);
			sb->AppendC(UTF8STRC("\r\nSystem Clock Reference Extension = "));
			sb->AppendI32(scr_ext);
			sb->AppendC(UTF8STRC("\r\nProgram Mux Rate = "));
			sb->AppendU32(ReadMUInt24(&packBuff[10]) >> 2);
			sb->AppendC(UTF8STRC("\r\nPack Stuffing Length = "));
			sb->AppendU16(packBuff[13] & 7);
		}
		else if ((packBuff[4] & 0xf0) == 0x20)
		{
			Int64 scr_base;
			sb->AppendC(UTF8STRC("\r\nMPEG Version = 1"));
			scr_base = (((Int64)(packBuff[4] & 0xe)) << 29) | (packBuff[5] << 22) | ((packBuff[6] & 0xfe) << 14) | (packBuff[7] << 7) | ((packBuff[8] & 0xfe) >> 1);
			sb->AppendC(UTF8STRC("\r\nSystem Clock Reference = "));
			sb->AppendI64(scr_base);
			sb->AppendC(UTF8STRC("\r\nProgram Mux Rate = "));
			sb->AppendU32((ReadMUInt24(&packBuff[9]) >> 1) & 0x3fffff);
		}
		else
		{
			break;
		}
		break;
	case 0xbb:
		{
			UOSInt i;
			sb->AppendC(UTF8STRC("System header"));
			sb->AppendC(UTF8STRC("\r\nRate Bound = "));
			sb->AppendU32((ReadMUInt24(&packBuff[6]) >> 1) & 0x3fffff);
			sb->AppendC(UTF8STRC("\r\nAudio Bound = "));
			sb->AppendU16((UInt16)(packBuff[9] >> 2));
			sb->AppendC(UTF8STRC("\r\nFixed Flag = "));
			sb->AppendU16((UInt16)((packBuff[9] & 2) >> 1));
			sb->AppendC(UTF8STRC("\r\nCSPS Flag = "));
			sb->AppendU16(packBuff[9] & 1);
			sb->AppendC(UTF8STRC("\r\nSystem Audio Lock Flag = "));
			sb->AppendU16((packBuff[10] & 0x80) >> 7);
			sb->AppendC(UTF8STRC("\r\nSystem Video Lock Flag = "));
			sb->AppendU16((packBuff[10] & 0x40) >> 6);
			sb->AppendC(UTF8STRC("\r\nVideo Bound = "));
			sb->AppendU16(packBuff[10] & 0x1f);
			sb->AppendC(UTF8STRC("\r\nPacket Rate Restriction Flag = "));
			sb->AppendU16((packBuff[11] & 0x80) >> 7);
			i = 12;
			while (i < pack->packSize)
			{
				if (packBuff[i] & 0x80)
				{
					sb->AppendC(UTF8STRC("\r\n{"));
					sb->AppendC(UTF8STRC("\r\n\tStream ID = 0x"));
					sb->AppendHexBuff(&packBuff[i], 1, 0, Text::LineBreakType::None);
					sb->AppendC(UTF8STRC("\r\n\tSTD Buffer Bound Scale = "));
					sb->AppendU16((packBuff[i + 1] >> 5) & 1);
					sb->AppendC(UTF8STRC("\r\n\tSTD Buffer Size Bound = "));
					sb->AppendI16(ReadMInt16(&packBuff[i + 1]) & 0x1fff);
					sb->AppendC(UTF8STRC("\r\n}"));
				}
				i += 3;
			}
		}
		break;
	case 0xbc:
		sb->AppendC(UTF8STRC("Program Stream map"));
		break;
	case 0xbd:
		sb->AppendC(UTF8STRC("Private Stream 1"));
		if (this->mpgVer == 2)
		{
			UOSInt i;
			Int64 pts;
			sb->AppendC(UTF8STRC("\r\nPES Scrambling Control = "));
			sb->AppendU16((packBuff[6] & 0x30) >> 4);
			sb->AppendC(UTF8STRC("\r\nPES Priority = "));
			sb->AppendU16((packBuff[6] & 0x8) >> 3);
			sb->AppendC(UTF8STRC("\r\nData Alignment Indicator = "));
			sb->AppendU16((packBuff[6] & 0x4) >> 2);
			sb->AppendC(UTF8STRC("\r\nCopyright = "));
			sb->AppendU16((packBuff[6] & 0x2) >> 1);
			sb->AppendC(UTF8STRC("\r\nOriginal or Copy = "));
			sb->AppendU16(packBuff[6] & 0x1);
			sb->AppendC(UTF8STRC("\r\nPTS DTS flags = "));
			sb->AppendU16((packBuff[7] & 0xc0) >> 6);
			sb->AppendC(UTF8STRC("\r\nESCR flag = "));
			sb->AppendU16((packBuff[7] & 0x20) >> 5);
			sb->AppendC(UTF8STRC("\r\nES Rate Flag = "));
			sb->AppendU16((packBuff[7] & 0x10) >> 4);
			sb->AppendC(UTF8STRC("\r\nDSM Trick Mode flag = "));
			sb->AppendU16((packBuff[7] & 0x8) >> 3);
			sb->AppendC(UTF8STRC("\r\nAdditional Copy Info flag = "));
			sb->AppendU16((packBuff[7] & 0x4) >> 2);
			sb->AppendC(UTF8STRC("\r\nPES CRC flag = "));
			sb->AppendU16((packBuff[7] & 0x2) >> 1);
			sb->AppendC(UTF8STRC("\r\nPES Extension flag = "));
			sb->AppendU16(packBuff[7] & 0x1);
			sb->AppendC(UTF8STRC("\r\nPES Header Data Length = "));
			sb->AppendU16(packBuff[8]);
			i = 9;
			if ((packBuff[7] & 0xc0) == 0x80)
			{
				pts = (((Int64)(packBuff[9] & 0xe)) << 29) | (packBuff[10] << 22) | ((packBuff[11] & 0xfe) << 14) | (packBuff[12] << 7) | (packBuff[13] >> 1);
				sb->AppendC(UTF8STRC("\r\nPresentation Time Stamp = "));
				sb->AppendI64(pts);
				i = 14;
			}
			else if ((packBuff[7] & 0xc0) == 0xc0)
			{
				pts = (((Int64)(packBuff[9] & 0xe)) << 29) | (packBuff[10] << 22) | ((packBuff[11] & 0xfe) << 14) | (packBuff[12] << 7) | (packBuff[13] >> 1);
				sb->AppendC(UTF8STRC("\r\nPresentation Time Stamp = "));
				sb->AppendI64(pts);
				pts = (((Int64)(packBuff[14] & 0xe)) << 29) | (packBuff[15] << 22) | ((packBuff[16] & 0xfe) << 14) | (packBuff[17] << 7) | (packBuff[18] >> 1);
				sb->AppendC(UTF8STRC("\r\nDecoding Time Stamp = "));
				sb->AppendI64(pts);
				i = 19;
			}
			if (packBuff[7] & 0x20)
			{
				Int64 escr_base;
				Int32 escr_ext;
				escr_base = (((Int64)(packBuff[i] & 0x38)) << 27) | ((packBuff[i] & 3) << 28) | (packBuff[i + 1] << 20) | ((packBuff[i + 2] & 0xf8) << 12) | ((packBuff[i + 2] & 3) << 13) | (packBuff[i + 3] << 5) | (packBuff[i + 4] >> 3);
				escr_ext = ((packBuff[i + 4] & 3) << 7) | (packBuff[i + 5] >> 1);
				sb->AppendC(UTF8STRC("\r\nESCR Base = "));
				sb->AppendI64(escr_base);
				sb->AppendC(UTF8STRC("\r\nESCR Extension = "));
				sb->AppendI32(escr_ext);
				i += 6;
			}
			if (packBuff[7] & 0x10)
			{
				sb->AppendC(UTF8STRC("\r\nES Rate = "));
				sb->AppendU32((ReadMUInt24(&packBuff[i]) >> 1) & 0x3fffff);
				i += 3;
			}
			if (packBuff[7] & 0x8)
			{
				/////////////////////////////////
				sb->AppendC(UTF8STRC("\r\nTrick Mode Control = "));
				sb->AppendU16((packBuff[i] & 0xe0) >> 5);
				i += 1;
			}
			if (packBuff[7] & 0x4)
			{
				sb->AppendC(UTF8STRC("\r\nAdditional Copy Info = "));
				sb->AppendU16(packBuff[i] & 0x7f);
				i += 1;
			}
			if (packBuff[7] & 0x2)
			{
				sb->AppendC(UTF8STRC("\r\nPrevious PES packet CRC = 0x"));
				sb->AppendHex16(ReadMUInt16(&packBuff[i]));
				i += 2;
			}
			if (packBuff[7] & 0x1)
			{
				sb->AppendC(UTF8STRC("\r\nPES Private Data Flag = "));
				sb->AppendU16((packBuff[i] & 0x80) >> 7);
				sb->AppendC(UTF8STRC("\r\nPack Header Field Flag = "));
				sb->AppendU16((packBuff[i] & 0x40) >> 6);
				sb->AppendC(UTF8STRC("\r\nProgram Packet Sequence Counter Flag = "));
				sb->AppendU16((packBuff[i] & 0x20) >> 5);
				sb->AppendC(UTF8STRC("\r\nP-STD Buffer Flag = "));
				sb->AppendU16((packBuff[i] & 0x10) >> 4);
				sb->AppendC(UTF8STRC("\r\nPES Extension Flag 2 = "));
				sb->AppendU16(packBuff[i] & 0x1);
				////////////////////////////////////
				i += 1;
			}

			i = 9 + (UOSInt)packBuff[8];
			sb->AppendC(UTF8STRC("\r\nStream Type = 0x"));
			sb->AppendHex8(packBuff[i]);
			if ((packBuff[i] & 0xf0) == 0xa0)
			{
				sb->AppendC(UTF8STRC(" (VOB LPCM Audio)"));
				sb->AppendC(UTF8STRC("\r\nNo. of Channels = "));
				sb->AppendU16((UInt16)((packBuff[i + 5] & 7) + 1));
				sb->AppendC(UTF8STRC("\r\nBits per Sample = "));
				switch (packBuff[i + 5] & 0xc0)
				{
				case 0x0:
					sb->AppendC(UTF8STRC("16"));
					break;
				case 0x40:
					sb->AppendC(UTF8STRC("20"));
					break;
				case 0x80:
					sb->AppendC(UTF8STRC("24"));
					break;
				}
				sb->AppendC(UTF8STRC("\r\nSampling Frequency = "));
				sb->AppendU32((packBuff[i + 5] & 0x30)?96000:48000);
				sb->AppendC(UTF8STRC("\r\nVOB LPCM Header = "));
				sb->AppendHexBuff(&packBuff[i], 7, ' ', Text::LineBreakType::None);
				i += 7;
			}
			else if ((packBuff[i] & 0xf0) == 0x80)
			{
				sb->AppendC(UTF8STRC(" (VOB AC3 Audio)"));
				sb->AppendC(UTF8STRC("\r\nVOB AC3 Header = "));
				sb->AppendHexBuff(&packBuff[i], 4, ' ', Text::LineBreakType::None);
				i += 4;
			}
			else if (packBuff[i] == 0xff && packBuff[i + 1] == 0xa0)
			{
				sb->AppendC(UTF8STRC(" (PSS LPCM Audio)"));
				sb->AppendC(UTF8STRC("\r\nPSS Audio Header = "));
				sb->AppendHexBuff(&packBuff[i], 4, ' ', Text::LineBreakType::None);
				i += 4;
			}
			else if (packBuff[i] == 0xff && packBuff[i + 1] == 0xa1)
			{
				sb->AppendC(UTF8STRC(" (PSS ADPCM Audio)"));
				sb->AppendC(UTF8STRC("\r\nPSS Audio Header = "));
				sb->AppendHexBuff(&packBuff[i], 4, ' ', Text::LineBreakType::None);
				i += 4;
			}
		
			sb->AppendC(UTF8STRC("\r\nContent Size = "));
			sb->AppendUOSInt((pack->packSize - i));
			sb->AppendC(UTF8STRC("\r\nContent:\r\n"));
			sb->AppendHexBuff(&packBuff[i], pack->packSize - i, ' ', Text::LineBreakType::CRLF);
		}
		else
		{
			UOSInt i;
			Int64 pts;
			i = 6;
			while (packBuff[i] & 0x80)
			{
				i++;
			}
			if (i > 6)
			{
				sb->AppendC(UTF8STRC("\r\nStuffing Byte = "));
				sb->AppendI32((Int32)(i - 6));
			}
			if ((packBuff[i] & 0xc0) == 0x40)
			{
				sb->AppendC(UTF8STRC("\r\nSTD Buffer Scale = "));
				sb->AppendU16((packBuff[i] & 0x20) >> 5);
				sb->AppendC(UTF8STRC("\r\nSTD Buffer Size = "));
				sb->AppendI16(ReadMInt16(&packBuff[i]) & 0x1fff);
				i += 2;
			}

			if ((packBuff[i] & 0xf0) == 0x20)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				sb->AppendC(UTF8STRC("\r\nPresentation Time Stamp = "));
				sb->AppendI64(pts);
				i += 5;
			}
			else if ((packBuff[i] & 0xf0) == 0x30)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				sb->AppendC(UTF8STRC("\r\nPresentation Time Stamp = "));
				sb->AppendI64(pts);
				pts = (((Int64)packBuff[i + 5] & 0xe) << 29) | (packBuff[i + 6] << 22) | ((packBuff[i + 7] & 0xfe) << 14) | (packBuff[i + 8] << 7) | (packBuff[i + 9] >> 1);;
				sb->AppendC(UTF8STRC("\r\nDecoding Time Stamp = "));
				sb->AppendI64(pts);
				i += 10;
			}
			else if (packBuff[i] == 0xf)
			{
				i++;
			}
			else
			{
				break;
			}

			sb->AppendC(UTF8STRC("\r\nContent Size = "));
			sb->AppendI32((Int32)(pack->packSize - i));
			sb->AppendC(UTF8STRC("\r\nContent:\r\n"));
			sb->AppendHexBuff(&packBuff[i], pack->packSize - i, ' ', Text::LineBreakType::CRLF);
		}
		break;
	case 0xbe:
		sb->AppendC(UTF8STRC("Padding Stream"));
		sb->AppendC(UTF8STRC("\r\nPadding Size = "));
		sb->AppendI32((Int32)pack->packSize);
		break;
	case 0xbf:
		sb->AppendC(UTF8STRC("Private Stream 2"));
		sb->AppendC(UTF8STRC("\r\nContent Size = "));
		sb->AppendI32((Int32)pack->packSize - 6);
		sb->AppendC(UTF8STRC("\r\nContent:\r\n"));
		sb->AppendHexBuff(&packBuff[6], pack->packSize - 6, ' ', Text::LineBreakType::CRLF);
		break;
	case 0xc0:
		{
			UOSInt i;
			Int64 pts;
			sb->AppendC(UTF8STRC("Audio Stream 1"));
			i = 6;
			while (packBuff[i] & 0x80)
			{
				i++;
			}
			if (i > 6)
			{
				sb->AppendC(UTF8STRC("\r\nStuffing Byte = "));
				sb->AppendI32((Int32)(i - 6));
			}
			if ((packBuff[i] & 0xc0) == 0x40)
			{
				sb->AppendC(UTF8STRC("\r\nSTD Buffer Scale = "));
				sb->AppendU16((packBuff[i] & 0x20) >> 5);
				sb->AppendC(UTF8STRC("\r\nSTD Buffer Size = "));
				sb->AppendI16(ReadMInt16(&packBuff[i]) & 0x1fff);
				i += 2;
			}

			if ((packBuff[i] & 0xf0) == 0x20)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				sb->AppendC(UTF8STRC("\r\nPresentation Time Stamp = "));
				sb->AppendI64(pts);
				i += 5;
			}
			else if ((packBuff[i] & 0xf0) == 0x30)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				sb->AppendC(UTF8STRC("\r\nPresentation Time Stamp = "));
				sb->AppendI64(pts);
				pts = (((Int64)packBuff[i + 5] & 0xe) << 29) | (packBuff[i + 6] << 22) | ((packBuff[i + 7] & 0xfe) << 14) | (packBuff[i + 8] << 7) | (packBuff[i + 9] >> 1);;
				sb->AppendC(UTF8STRC("\r\nDecoding Time Stamp = "));
				sb->AppendI64(pts);
				i += 10;
			}
			else if (packBuff[i] == 0xf)
			{
				i++;
			}
			else
			{
				break;
			}

			sb->AppendC(UTF8STRC("\r\nContent Size = "));
			sb->AppendI32((Int32)(pack->packSize - i));
			sb->AppendC(UTF8STRC("\r\nContent:\r\n"));
			sb->AppendHexBuff(&packBuff[i], pack->packSize - i, ' ', Text::LineBreakType::CRLF);
		}
		break;
	case 0xe0:
		sb->AppendC(UTF8STRC("Video Stream"));
		if (this->mpgVer == 2)
		{
			OSInt i;
			Int64 pts;
			sb->AppendC(UTF8STRC("\r\nPES Scrambling Control = "));
			sb->AppendU16((packBuff[6] & 0x30) >> 4);
			sb->AppendC(UTF8STRC("\r\nPES Priority = "));
			sb->AppendU16((packBuff[6] & 0x8) >> 3);
			sb->AppendC(UTF8STRC("\r\nData Alignment Indicator = "));
			sb->AppendU16((packBuff[6] & 0x4) >> 2);
			sb->AppendC(UTF8STRC("\r\nCopyright = "));
			sb->AppendU16((packBuff[6] & 0x2) >> 1);
			sb->AppendC(UTF8STRC("\r\nOriginal or Copy = "));
			sb->AppendU16(packBuff[6] & 0x1);
			sb->AppendC(UTF8STRC("\r\nPTS DTS flags = "));
			sb->AppendU16((packBuff[7] & 0xc0) >> 6);
			sb->AppendC(UTF8STRC("\r\nESCR flag = "));
			sb->AppendU16((packBuff[7] & 0x20) >> 5);
			sb->AppendC(UTF8STRC("\r\nES Rate Flag = "));
			sb->AppendU16((packBuff[7] & 0x10) >> 4);
			sb->AppendC(UTF8STRC("\r\nDSM Trick Mode flag = "));
			sb->AppendU16((packBuff[7] & 0x8) >> 3);
			sb->AppendC(UTF8STRC("\r\nAdditional Copy Info flag = "));
			sb->AppendU16((packBuff[7] & 0x4) >> 2);
			sb->AppendC(UTF8STRC("\r\nPES CRC flag = "));
			sb->AppendU16((packBuff[7] & 0x2) >> 1);
			sb->AppendC(UTF8STRC("\r\nPES Extension flag = "));
			sb->AppendU16(packBuff[7] & 0x1);
			sb->AppendC(UTF8STRC("\r\nPES Header Data Length = "));
			sb->AppendU16(packBuff[8]);
			i = 9;
			if ((packBuff[7] & 0xc0) == 0x80)
			{
				pts = (((Int64)(packBuff[9] & 0xe)) << 29) | (packBuff[10] << 22) | ((packBuff[11] & 0xfe) << 14) | (packBuff[12] << 7) | (packBuff[13] >> 1);
				sb->AppendC(UTF8STRC("\r\nPresentation Time Stamp = "));
				sb->AppendI64(pts);
				i = 14;
			}
			else if ((packBuff[7] & 0xc0) == 0xc0)
			{
				pts = (((Int64)(packBuff[9] & 0xe)) << 29) | (packBuff[10] << 22) | ((packBuff[11] & 0xfe) << 14) | (packBuff[12] << 7) | (packBuff[13] >> 1);
				sb->AppendC(UTF8STRC("\r\nPresentation Time Stamp = "));
				sb->AppendI64(pts);
				pts = (((Int64)(packBuff[14] & 0xe)) << 29) | (packBuff[15] << 22) | ((packBuff[16] & 0xfe) << 14) | (packBuff[17] << 7) | (packBuff[18] >> 1);
				sb->AppendC(UTF8STRC("\r\nDecoding Time Stamp = "));
				sb->AppendI64(pts);
				i = 19;
			}
			if (packBuff[7] & 0x20)
			{
				Int64 escr_base;
				Int32 escr_ext;
				escr_base = (((Int64)(packBuff[i] & 0x38)) << 27) | ((packBuff[i] & 3) << 28) | (packBuff[i + 1] << 20) | ((packBuff[i + 2] & 0xf8) << 12) | ((packBuff[i + 2] & 3) << 13) | (packBuff[i + 3] << 5) | (packBuff[i + 4] >> 3);
				escr_ext = ((packBuff[i + 4] & 3) << 7) | (packBuff[i + 5] >> 1);
				sb->AppendC(UTF8STRC("\r\nESCR Base = "));
				sb->AppendI64(escr_base);
				sb->AppendC(UTF8STRC("\r\nESCR Extension = "));
				sb->AppendI32(escr_ext);
				i += 6;
			}
			if (packBuff[7] & 0x10)
			{
				sb->AppendC(UTF8STRC("\r\nES Rate = "));
				sb->AppendU32((ReadMUInt24(&packBuff[i]) >> 1) & 0x3fffff);
				i += 3;
			}
			if (packBuff[7] & 0x8)
			{
				/////////////////////////////////
				sb->AppendC(UTF8STRC("\r\nTrick Mode Control = "));
				sb->AppendU16((packBuff[i] & 0xe0) >> 5);
				i += 1;
			}
			if (packBuff[7] & 0x4)
			{
				sb->AppendC(UTF8STRC("\r\nAdditional Copy Info = "));
				sb->AppendU16(packBuff[i] & 0x7f);
				i += 1;
			}
			if (packBuff[7] & 0x2)
			{
				sb->AppendC(UTF8STRC("\r\nPrevious PES packet CRC = 0x"));
				sb->AppendHex16(ReadMUInt16(&packBuff[i]));
				i += 2;
			}
			if (packBuff[7] & 0x1)
			{
				sb->AppendC(UTF8STRC("\r\nPES Private Data Flag = "));
				sb->AppendU16((packBuff[i] & 0x80) >> 7);
				sb->AppendC(UTF8STRC("\r\nPack Header Field Flag = "));
				sb->AppendU16((packBuff[i] & 0x40) >> 6);
				sb->AppendC(UTF8STRC("\r\nProgram Packet Sequence Counter Flag = "));
				sb->AppendU16((packBuff[i] & 0x20) >> 5);
				sb->AppendC(UTF8STRC("\r\nP-STD Buffer Flag = "));
				sb->AppendU16((packBuff[i] & 0x10) >> 4);
				sb->AppendC(UTF8STRC("\r\nPES Extension Flag 2 = "));
				sb->AppendU16(packBuff[i] & 0x1);
				////////////////////////////////////
				i += 1;
			}
		
			sb->AppendC(UTF8STRC("\r\nContent Size = "));
			sb->AppendI32((Int32)(pack->packSize - 9 - packBuff[8]));
			sb->AppendC(UTF8STRC("\r\nContent:\r\n"));
			sb->AppendHexBuff(&packBuff[9 + packBuff[8]], pack->packSize - 9 - packBuff[8], ' ', Text::LineBreakType::CRLF);
		}
		else
		{
			UOSInt i;
			Int64 pts;
			i = 6;
			while (packBuff[i] & 0x80)
			{
				i++;
			}
			if (i > 6)
			{
				sb->AppendC(UTF8STRC("\r\nStuffing Byte = "));
				sb->AppendI32((Int32)(i - 6));
			}
			if ((packBuff[i] & 0xc0) == 0x40)
			{
				sb->AppendC(UTF8STRC("\r\nSTD Buffer Scale = "));
				sb->AppendU16((packBuff[i] & 0x20) >> 5);
				sb->AppendC(UTF8STRC("\r\nSTD Buffer Size = "));
				sb->AppendI16(ReadMInt16(&packBuff[i]) & 0x1fff);
				i += 2;
			}

			if ((packBuff[i] & 0xf0) == 0x20)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				sb->AppendC(UTF8STRC("\r\nPresentation Time Stamp = "));
				sb->AppendI64(pts);
				i += 5;
			}
			else if ((packBuff[i] & 0xf0) == 0x30)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				sb->AppendC(UTF8STRC("\r\nPresentation Time Stamp = "));
				sb->AppendI64(pts);
				pts = (((Int64)packBuff[i + 5] & 0xe) << 29) | (packBuff[i + 6] << 22) | ((packBuff[i + 7] & 0xfe) << 14) | (packBuff[i + 8] << 7) | (packBuff[i + 9] >> 1);;
				sb->AppendC(UTF8STRC("\r\nDecoding Time Stamp = "));
				sb->AppendI64(pts);
				i += 10;
			}
			else if (packBuff[i] == 0xf)
			{
				i++;
			}
			else
			{
				break;
			}

			sb->AppendC(UTF8STRC("\r\nContent Size = "));
			sb->AppendI32((Int32)(pack->packSize - i));
			sb->AppendC(UTF8STRC("\r\nContent:\r\n"));
			sb->AppendHexBuff(&packBuff[i], pack->packSize - i, ' ', Text::LineBreakType::CRLF);
		}
		break;
	}
	return true;
}

UOSInt IO::FileAnalyse::MPEGFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs.GetCount() - 1;
	OSInt k;
	NN<PackInfo> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItemNoCheck((UOSInt)k);
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::MPEGFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	NN<IO::FileAnalyse::MPEGFileAnalyse::PackInfo> pack;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (!this->packs.GetItem(index).SetTo(pack))
		return 0;

	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));

	Data::ByteBuffer packBuff(pack->packSize);
	this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);

	switch (packBuff[3])
	{
	case 0xb9:
		frame->AddField(0, 4, CSTR("Sequence"), CSTR("MPEG program end"));
		break;
	case 0xba:
		frame->AddField(0, 4, CSTR("Sequence"), CSTR("Pack start"));
		if ((packBuff[4] & 0xc0) == 0x40)
		{
			Int64 scr_base;
			Int32 scr_ext;
			frame->AddField(4, 1, CSTR("MPEG Version"), CSTR("2"));
			scr_base = (((Int64)(packBuff[4] & 0x38)) << 27) | ((packBuff[4] & 3) << 28) | (packBuff[5] << 20) | ((packBuff[6] & 0xf8) << 12) | ((packBuff[6] & 3) << 13) | (packBuff[7] << 5) | (packBuff[8] >> 3);
			scr_ext = ((packBuff[8] & 3) << 7) | (packBuff[9] >> 1);
			frame->AddInt64V(4, 5, CSTR("System Clock Reference Base"), scr_base);
			frame->AddInt(8, 2, CSTR("System Clock Reference Extension"), scr_ext);
			frame->AddUInt(10, 3, CSTR("Program Mux Rate"), ReadMUInt24(&packBuff[10]) >> 2);
			frame->AddUInt(13, 1, CSTR("Pack Stuffing Length"), packBuff[13] & 7);
		}
		else if ((packBuff[4] & 0xf0) == 0x20)
		{
			Int64 scr_base;
			frame->AddField(4, 1, CSTR("MPEG Version"), CSTR("1"));
			scr_base = (((Int64)(packBuff[4] & 0xe)) << 29) | (packBuff[5] << 22) | ((packBuff[6] & 0xfe) << 14) | (packBuff[7] << 7) | ((packBuff[8] & 0xfe) >> 1);
			frame->AddInt64V(4, 5, CSTR("System Clock Reference Base"), scr_base);
			frame->AddUInt(9, 3, CSTR("Program Mux Rate"), (ReadMUInt24(&packBuff[9]) >> 1) & 0x3fffff);
		}
		else
		{
			break;
		}
		break;
	case 0xbb:
		{
			UOSInt i;
			frame->AddField(0, 4, CSTR("Sequence"), CSTR("System header"));
			frame->AddUInt(6, 3, CSTR("Rate Bound"), (ReadMUInt24(&packBuff[6]) >> 1) & 0x3fffff);
			frame->AddUInt(9, 1, CSTR("Audio Bound"), (UInt16)(packBuff[9] >> 2));
			frame->AddUInt(9, 1, CSTR("Fixed Flag"), (UInt16)((packBuff[9] & 2) >> 1));
			frame->AddUInt(9, 1, CSTR("CSPS Flag"), packBuff[9] & 1);
			frame->AddUInt(10, 1, CSTR("System Audio Lock Flag"), (packBuff[10] & 0x80) >> 7);
			frame->AddUInt(10, 1, CSTR("System Video Lock Flag"), (packBuff[10] & 0x40) >> 6);
			frame->AddUInt(10, 1, CSTR("Video Bound"), packBuff[10] & 0x1f);
			frame->AddUInt(11, 1, CSTR("Packet Rate Restriction Flag"), (packBuff[11] & 0x80) >> 7);
			i = 12;
			while (i < pack->packSize)
			{
				if (packBuff[i] & 0x80)
				{
					frame->AddText(i, CSTR("{"));
					frame->AddHex8(i, CSTR("\tStream ID"), packBuff[i]);
					frame->AddUInt(i + 1, 1, CSTR("\tSTD Buffer Bound Scale"), (packBuff[i + 1] >> 5) & 1);
					frame->AddInt(i + 1, 2, CSTR("\tSTD Buffer Size Bound"), ReadMInt16(&packBuff[i + 1]) & 0x1fff);
					frame->AddText(i + 3, CSTR("}"));
				}
				i += 3;
			}
		}
		break;
	case 0xbc:
		frame->AddField(0, 4, CSTR("Sequence"), CSTR("Program Stream map"));
		break;
	case 0xbd:
		frame->AddField(0, 4, CSTR("Sequence"), CSTR("Private Stream 1"));
		if (this->mpgVer == 2)
		{
			UOSInt i;
			Int64 pts;
			frame->AddUInt(6, 1, CSTR("PES Scrambling Control"), (packBuff[6] & 0x30) >> 4);
			frame->AddUInt(6, 1, CSTR("PES Priority"), (packBuff[6] & 0x8) >> 3);
			frame->AddUInt(6, 1, CSTR("Data Alignment Indicator"), (packBuff[6] & 0x4) >> 2);
			frame->AddUInt(6, 1, CSTR("Copyright"), (packBuff[6] & 0x2) >> 1);
			frame->AddUInt(6, 1, CSTR("Original or Copy"), packBuff[6] & 0x1);
			frame->AddUInt(7, 1, CSTR("PTS DTS flags"), (packBuff[7] & 0xc0) >> 6);
			frame->AddUInt(7, 1, CSTR("ESCR flag"), (packBuff[7] & 0x20) >> 5);
			frame->AddUInt(7, 1, CSTR("ES Rate Flag"), (packBuff[7] & 0x10) >> 4);
			frame->AddUInt(7, 1, CSTR("DSM Trick Mode flag"), (packBuff[7] & 0x8) >> 3);
			frame->AddUInt(7, 1, CSTR("Additional Copy Info flag"), (packBuff[7] & 0x4) >> 2);
			frame->AddUInt(7, 1, CSTR("PES CRC flag"), (packBuff[7] & 0x2) >> 1);
			frame->AddUInt(7, 1, CSTR("PES Extension flag"), packBuff[7] & 0x1);
			frame->AddUInt(8, 1, CSTR("PES Header Data Length"), packBuff[8]);
			i = 9;
			if ((packBuff[7] & 0xc0) == 0x80)
			{
				pts = (((Int64)(packBuff[9] & 0xe)) << 29) | (packBuff[10] << 22) | ((packBuff[11] & 0xfe) << 14) | (packBuff[12] << 7) | (packBuff[13] >> 1);
				frame->AddInt64V(9, 5, CSTR("Presentation Time Stamp"), pts);
				i = 14;
			}
			else if ((packBuff[7] & 0xc0) == 0xc0)
			{
				pts = (((Int64)(packBuff[9] & 0xe)) << 29) | (packBuff[10] << 22) | ((packBuff[11] & 0xfe) << 14) | (packBuff[12] << 7) | (packBuff[13] >> 1);
				frame->AddInt64V(9, 5, CSTR("Presentation Time Stamp"), pts);
				pts = (((Int64)(packBuff[14] & 0xe)) << 29) | (packBuff[15] << 22) | ((packBuff[16] & 0xfe) << 14) | (packBuff[17] << 7) | (packBuff[18] >> 1);
				frame->AddInt64V(14, 5, CSTR("Decoding Time Stamp"), pts);
				i = 19;
			}
			if (packBuff[7] & 0x20)
			{
				Int64 escr_base;
				Int32 escr_ext;
				escr_base = (((Int64)(packBuff[i] & 0x38)) << 27) | ((packBuff[i] & 3) << 28) | (packBuff[i + 1] << 20) | ((packBuff[i + 2] & 0xf8) << 12) | ((packBuff[i + 2] & 3) << 13) | (packBuff[i + 3] << 5) | (packBuff[i + 4] >> 3);
				escr_ext = ((packBuff[i + 4] & 3) << 7) | (packBuff[i + 5] >> 1);
				frame->AddInt64V(i, 5, CSTR("ESCR Base"), escr_base);
				frame->AddInt(i + 4, 2, CSTR("ESCR Extension"), escr_ext);
				i += 6;
			}
			if (packBuff[7] & 0x10)
			{
				frame->AddUInt(i, 3, CSTR("ES Rate"), (ReadMUInt24(&packBuff[i]) >> 1) & 0x3fffff);
				i += 3;
			}
			if (packBuff[7] & 0x8)
			{
				/////////////////////////////////
				frame->AddUInt(i, 1, CSTR("Trick Mode Control"), (packBuff[i] & 0xe0) >> 5);
				i += 1;
			}
			if (packBuff[7] & 0x4)
			{
				frame->AddUInt(i, 1, CSTR("Additional Copy Info"), packBuff[i] & 0x7f);
				i += 1;
			}
			if (packBuff[7] & 0x2)
			{
				frame->AddHex16(i, CSTR("Previous PES packet CRC"), ReadMUInt16(&packBuff[i]));
				i += 2;
			}
			if (packBuff[7] & 0x1)
			{
				frame->AddUInt(i, 1, CSTR("PES Private Data Flag"), (packBuff[i] & 0x80) >> 7);
				frame->AddUInt(i, 1, CSTR("Pack Header Field Flag "), (packBuff[i] & 0x40) >> 6);
				frame->AddUInt(i, 1, CSTR("Program Packet Sequence Counter Flag"), (packBuff[i] & 0x20) >> 5);
				frame->AddUInt(i, 1, CSTR("P-STD Buffer Flag"), (packBuff[i] & 0x10) >> 4);
				frame->AddUInt(i, 1, CSTR("PES Extension Flag 2"), packBuff[i] & 0x1);
				////////////////////////////////////
				i += 1;
			}

			i = 9 + (UOSInt)packBuff[8];
			frame->AddHex8(i, CSTR("Stream Type"), packBuff[i]);
			if ((packBuff[i] & 0xf0) == 0xa0)
			{
				frame->AddField(i, 1, CSTR("Stream Type"), CSTR("VOB LPCM Audio"));
				frame->AddUInt(i + 5, 1, CSTR("No. of Channels"), (UInt16)((packBuff[i + 5] & 7) + 1));
				switch (packBuff[i + 5] & 0xc0)
				{
				case 0x0:
					frame->AddUInt(i + 5, 1, CSTR("Bits per Sample"), 16);
					break;
				case 0x40:
					frame->AddUInt(i + 5, 1, CSTR("Bits per Sample"), 20);
					break;
				case 0x80:
					frame->AddUInt(i + 5, 1, CSTR("Bits per Sample"), 24);
					break;
				}
				frame->AddUInt(i + 5, 1, CSTR("Sampling Frequency"), (packBuff[i + 5] & 0x30)?96000:48000);
				frame->AddHexBuff(i, 7, CSTR("VOB LPCM Header"), &packBuff[i], false);
				i += 7;
			}
			else if ((packBuff[i] & 0xf0) == 0x80)
			{
				frame->AddField(i, 1, CSTR("Stream Type"), CSTR("VOB AC3 Audio"));
				frame->AddHexBuff(i, 4, CSTR("VOB AC3 Header"), &packBuff[i], false);
				i += 4;
			}
			else if (packBuff[i] == 0xff && packBuff[i + 1] == 0xa0)
			{
				frame->AddField(i, 1, CSTR("Stream Type"), CSTR("PSS LPCM Audio"));
				frame->AddHexBuff(i, 4, CSTR("PSS Audio Header"), &packBuff[i], false);
				i += 4;
			}
			else if (packBuff[i] == 0xff && packBuff[i + 1] == 0xa1)
			{
				frame->AddField(i, 1, CSTR("Stream Type"), CSTR("PSS ADPCM Audio"));
				frame->AddHexBuff(i, 4, CSTR("PSS Audio Header"), &packBuff[i], false);
				i += 4;
			}

			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Content Size=")), pack->packSize - i);
			frame->AddText(i, CSTRP(sbuff, sptr));
			frame->AddHexBuff(i, pack->packSize - i, CSTR("Content"), &packBuff[i], true);
		}
		else
		{
			UOSInt i;
			Int64 pts;
			i = 6;
			while (packBuff[i] & 0x80)
			{
				i++;
			}
			if (i > 6)
			{
				frame->AddHexBuff(6, i - 6, CSTR("Stuffing Byte"), &packBuff[6], false);
			}
			if ((packBuff[i] & 0xc0) == 0x40)
			{
				frame->AddUInt(i, 1, CSTR("STD Buffer Scale"), (packBuff[i] & 0x20) >> 5);
				frame->AddUInt(i, 2, CSTR("STD Buffer Size"), ReadMUInt16(&packBuff[i]) & 0x1fff);
				i += 2;
			}

			if ((packBuff[i] & 0xf0) == 0x20)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);
				frame->AddInt64V(i, 5, CSTR("Presentation Time Stamp"), pts);
				i += 5;
			}
			else if ((packBuff[i] & 0xf0) == 0x30)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				frame->AddInt64V(i, 5, CSTR("Presentation Time Stamp"), pts);
				pts = (((Int64)packBuff[i + 5] & 0xe) << 29) | (packBuff[i + 6] << 22) | ((packBuff[i + 7] & 0xfe) << 14) | (packBuff[i + 8] << 7) | (packBuff[i + 9] >> 1);;
				frame->AddInt64V(i + 5, 5, CSTR("Decoding Time Stamp"), pts);
				i += 10;
			}
			else if (packBuff[i] == 0xf)
			{
				i++;
			}
			else
			{
				break;
			}

			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Content Size=")), pack->packSize - i);
			frame->AddText(i, CSTRP(sbuff, sptr));
			frame->AddHexBuff(i, pack->packSize - i, CSTR("Content"), &packBuff[i], true);
		}
		break;
	case 0xbe:
		frame->AddField(0, 4, CSTR("Sequence"), CSTR("Padding Stream"));
		frame->AddUInt(4, pack->packSize - 4, CSTR("Padding Size"), pack->packSize);
		break;
	case 0xbf:
		frame->AddField(0, 4, CSTR("Sequence"), CSTR("Private Stream 2"));
		sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Content Size=")), pack->packSize - 6);
		frame->AddText(6, CSTRP(sbuff, sptr));
		frame->AddHexBuff(6, pack->packSize - 6, CSTR("Content"), &packBuff[6], true);
		break;
	case 0xc0:
		{
			UOSInt i;
			Int64 pts;
			frame->AddField(0, 4, CSTR("Sequence"), CSTR("Audio Stream 1"));
			i = 6;
			while (packBuff[i] & 0x80)
			{
				i++;
			}
			if (i > 6)
			{
				frame->AddHexBuff(6, i - 6, CSTR("Stuffing Byte"), &packBuff[6], false);
			}
			if ((packBuff[i] & 0xc0) == 0x40)
			{
				frame->AddUInt(i, 1, CSTR("STD Buffer Scale"), (packBuff[i] & 0x20) >> 5);
				frame->AddUInt(i, 2, CSTR("STD Buffer Size"), ReadMUInt16(&packBuff[i]) & 0x1fff);
				i += 2;
			}

			if ((packBuff[i] & 0xf0) == 0x20)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				frame->AddInt64V(i, 5, CSTR("Presentation Time Stamp"), pts);
				i += 5;
			}
			else if ((packBuff[i] & 0xf0) == 0x30)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				frame->AddInt64V(i, 5, CSTR("Presentation Time Stamp"), pts);
				pts = (((Int64)packBuff[i + 5] & 0xe) << 29) | (packBuff[i + 6] << 22) | ((packBuff[i + 7] & 0xfe) << 14) | (packBuff[i + 8] << 7) | (packBuff[i + 9] >> 1);;
				frame->AddInt64V(i + 5, 5, CSTR("Decoding Time Stamp"), pts);
				i += 10;
			}
			else if (packBuff[i] == 0xf)
			{
				i++;
			}
			else
			{
				break;
			}

			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Content Size=")), pack->packSize - i);
			frame->AddText(i, CSTRP(sbuff, sptr));
			frame->AddHexBuff(i, pack->packSize - i, CSTR("Content"), &packBuff[i], true);
		}
		break;
	case 0xe0:
		frame->AddField(0, 4, CSTR("Sequence"), CSTR("Video Stream"));
		if (this->mpgVer == 2)
		{
			UOSInt i;
			Int64 pts;
			frame->AddUInt(6, 1, CSTR("PES Scrambling Control"), (packBuff[6] & 0x30) >> 4);
			frame->AddUInt(6, 1, CSTR("PES Priority"), (packBuff[6] & 0x8) >> 3);
			frame->AddUInt(6, 1, CSTR("Data Alignment Indicator"), (packBuff[6] & 0x4) >> 2);
			frame->AddUInt(6, 1, CSTR("Copyright"), (packBuff[6] & 0x2) >> 1);
			frame->AddUInt(6, 1, CSTR("Original or Copy"), packBuff[6] & 0x1);
			frame->AddUInt(7, 1, CSTR("PTS DTS flags"), (packBuff[7] & 0xc0) >> 6);
			frame->AddUInt(7, 1, CSTR("ESCR flag"), (packBuff[7] & 0x20) >> 5);
			frame->AddUInt(7, 1, CSTR("ES Rate Flag"), (packBuff[7] & 0x10) >> 4);
			frame->AddUInt(7, 1, CSTR("DSM Trick Mode flag"), (packBuff[7] & 0x8) >> 3);
			frame->AddUInt(7, 1, CSTR("Additional Copy Info flag"), (packBuff[7] & 0x4) >> 2);
			frame->AddUInt(7, 1, CSTR("PES CRC flag"), (packBuff[7] & 0x2) >> 1);
			frame->AddUInt(7, 1, CSTR("PES Extension flag"), packBuff[7] & 0x1);
			frame->AddUInt(8, 1, CSTR("PES Header Data Length"), packBuff[8]);
			i = 9;
			if ((packBuff[7] & 0xc0) == 0x80)
			{
				pts = (((Int64)(packBuff[9] & 0xe)) << 29) | (packBuff[10] << 22) | ((packBuff[11] & 0xfe) << 14) | (packBuff[12] << 7) | (packBuff[13] >> 1);
				frame->AddInt64V(9, 5, CSTR("Presentation Time Stamp"), pts);
				i = 14;
			}
			else if ((packBuff[7] & 0xc0) == 0xc0)
			{
				pts = (((Int64)(packBuff[9] & 0xe)) << 29) | (packBuff[10] << 22) | ((packBuff[11] & 0xfe) << 14) | (packBuff[12] << 7) | (packBuff[13] >> 1);
				frame->AddInt64V(9, 5, CSTR("Presentation Time Stamp"), pts);
				pts = (((Int64)(packBuff[14] & 0xe)) << 29) | (packBuff[15] << 22) | ((packBuff[16] & 0xfe) << 14) | (packBuff[17] << 7) | (packBuff[18] >> 1);
				frame->AddInt64V(14, 5, CSTR("Decoding Time Stamp"), pts);
				i = 19;
			}
			if (packBuff[7] & 0x20)
			{
				Int64 escr_base;
				Int32 escr_ext;
				escr_base = (((Int64)(packBuff[i] & 0x38)) << 27) | ((packBuff[i] & 3) << 28) | (packBuff[i + 1] << 20) | ((packBuff[i + 2] & 0xf8) << 12) | ((packBuff[i + 2] & 3) << 13) | (packBuff[i + 3] << 5) | (packBuff[i + 4] >> 3);
				escr_ext = ((packBuff[i + 4] & 3) << 7) | (packBuff[i + 5] >> 1);
				frame->AddInt64V(i, 5, CSTR("ESCR Base"), escr_base);
				frame->AddInt(i + 4, 2, CSTR("ESCR Extension"), escr_ext);
				i += 6;
			}
			if (packBuff[7] & 0x10)
			{
				frame->AddUInt(i, 3, CSTR("ES Rate"), (ReadMUInt24(&packBuff[i]) >> 1) & 0x3fffff);
				i += 3;
			}
			if (packBuff[7] & 0x8)
			{
				/////////////////////////////////
				frame->AddUInt(i, 1, CSTR("Trick Mode Control"), (packBuff[i] & 0xe0) >> 5);
				i += 1;
			}
			if (packBuff[7] & 0x4)
			{
				frame->AddUInt(i, 1, CSTR("Additional Copy Info"), packBuff[i] & 0x7f);
				i += 1;
			}
			if (packBuff[7] & 0x2)
			{
				frame->AddHex16(i, CSTR("Previous PES packet CRC"), ReadMUInt16(&packBuff[i]));
				i += 2;
			}
			if (packBuff[7] & 0x1)
			{
				frame->AddUInt(i, 1, CSTR("PES Private Data Flag"), (packBuff[i] & 0x80) >> 7);
				frame->AddUInt(i, 1, CSTR("Pack Header Field Flag "), (packBuff[i] & 0x40) >> 6);
				frame->AddUInt(i, 1, CSTR("Program Packet Sequence Counter Flag"), (packBuff[i] & 0x20) >> 5);
				frame->AddUInt(i, 1, CSTR("P-STD Buffer Flag"), (packBuff[i] & 0x10) >> 4);
				frame->AddUInt(i, 1, CSTR("PES Extension Flag 2"), packBuff[i] & 0x1);
				////////////////////////////////////
				i += 1;
			}
		
			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Content Size=")), pack->packSize - 9 - packBuff[8]);
			frame->AddText(i, CSTRP(sbuff, sptr));
			frame->AddHexBuff(9 + (UOSInt)packBuff[8], pack->packSize - 9 - packBuff[8], CSTR("Content"), &packBuff[9 + packBuff[8]], true);
		}
		else
		{
			UOSInt i;
			Int64 pts;
			i = 6;
			while (packBuff[i] & 0x80)
			{
				i++;
			}
			if (i > 6)
			{
				frame->AddHexBuff(6, i - 6, CSTR("Stuffing Byte"), &packBuff[6], false);
			}
			if ((packBuff[i] & 0xc0) == 0x40)
			{
				frame->AddUInt(i, 1, CSTR("STD Buffer Scale"), (packBuff[i] & 0x20) >> 5);
				frame->AddUInt(i, 2, CSTR("STD Buffer Size"), ReadMUInt16(&packBuff[i]) & 0x1fff);
				i += 2;
			}

			if ((packBuff[i] & 0xf0) == 0x20)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				frame->AddInt64V(i, 5, CSTR("Presentation Time Stamp"), pts);
				i += 5;
			}
			else if ((packBuff[i] & 0xf0) == 0x30)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				frame->AddInt64V(i, 5, CSTR("Presentation Time Stamp"), pts);
				pts = (((Int64)packBuff[i + 5] & 0xe) << 29) | (packBuff[i + 6] << 22) | ((packBuff[i + 7] & 0xfe) << 14) | (packBuff[i + 8] << 7) | (packBuff[i + 9] >> 1);;
				frame->AddInt64V(i + 5, 5, CSTR("Decoding Time Stamp"), pts);
				i += 10;
			}
			else if (packBuff[i] == 0xf)
			{
				i++;
			}
			else
			{
				break;
			}

			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Content Size=")), pack->packSize - i);
			frame->AddText(i, CSTRP(sbuff, sptr));
			frame->AddHexBuff(i, pack->packSize - i, CSTR("Content"), &packBuff[i], true);
		}
		break;
	}
	return frame;
}

Bool IO::FileAnalyse::MPEGFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::MPEGFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::MPEGFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	UOSInt readSize;
	UOSInt buffSize;
	UOSInt j;
	UOSInt frameSize;
	UInt64 readOfst;
	Bool valid = true;
	IO::FileStream *dfs;
	NEW_CLASS(dfs, IO::FileStream(outputFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (dfs->IsError())
	{
		DEL_CLASS(dfs);
		return false;
	}
	Data::ByteBuffer readBuff(1048576);
	buffSize = 0;
	readOfst = 0;
	while (true)
	{
		if (buffSize < 256)
		{
			readSize = this->fd->GetRealData(readOfst, 256, readBuff.SubArray(buffSize));
			readOfst += readSize;
			buffSize += readSize;
		}
		if (buffSize < 4)
			break;
		j = 0;
		while (readBuff[j] != 0 || readBuff[j + 1] != 0 || readBuff[j + 2] != 1)
		{
			j++;
			if (j >= buffSize - 4)
			{
				break;
			}
		}
		if (j >= buffSize - 4 && buffSize > 4)
		{
			readBuff.CopyInner(0, j, buffSize - j);
			buffSize -= j;
			continue;
		}
		if (readBuff[j] != 0 || readBuff[j + 1] != 0 || readBuff[j + 2] != 1)
		{
			valid = false;
			break;
		}
		if (readBuff[j + 3] == 0xB9) //End Of File
		{
			dfs->Write(Data::ByteArrayR(&readBuff[j], 4));
			break;
		}
		if (readBuff[j + 3] == 0xba) 
		{
			if ((readBuff[j + 4] & 0xc0) == 0x40)
			{
				frameSize = 14 + (UOSInt)(readBuff[j + 13] & 7);
			}
			else if ((readBuff[j + 4] & 0xf0) == 0x20)
			{
				frameSize = 12;
			}
			else
			{
				valid = false;
				break;
			}
		}
		else
		{
			frameSize = 6 + (UOSInt)ReadMUInt16(&readBuff[j + 4]);
		}
		if (j + frameSize <= buffSize)
		{
			dfs->Write(Data::ByteArrayR(&readBuff[j], frameSize));
			if (j + frameSize < buffSize)
			{
				readBuff.CopyInner(0, j + frameSize, buffSize - j - frameSize);
				buffSize -= j + frameSize;
			}
			else
			{
				buffSize = 0;
			}
		}
		else
		{
			readSize = this->fd->GetRealData(readOfst, j + frameSize - buffSize, readBuff.SubArray(buffSize));
			readOfst += readSize;
			if (readSize == j + frameSize - buffSize)
			{
				dfs->Write(Data::ByteArrayR(&readBuff[j], frameSize));
				buffSize = 0;
			}
			else
			{
				valid = false;
				break;
			}
		}
	}
	DEL_CLASS(dfs);
	return valid;
}
