#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/FileAnalyse/MPEGFileAnalyse.h"
#include "Sync/Thread.h"

UInt32 __stdcall IO::FileAnalyse::MPEGFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::MPEGFileAnalyse *me = (IO::FileAnalyse::MPEGFileAnalyse*)userObj;
	UInt8 readBuff[256];
	UInt64 currOfst;
	UInt64 readOfst;
	UOSInt buffSize;
	UOSInt readSize;
	UOSInt frameSize;
	UOSInt j;
	IO::FileAnalyse::MPEGFileAnalyse::PackInfo *pack;

	me->threadRunning = true;
	me->threadStarted = true;
	currOfst = 0;
	readOfst = 0;
	buffSize = 0;
	while (!me->threadToStop)
	{
		if (me->pauseParsing)
		{
			Sync::Thread::Sleep(100);
		}
		else
		{
			if (buffSize < 256)
			{
				readSize = me->fd->GetRealData(readOfst, 256 - buffSize, &readBuff[buffSize]);
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
				pack = MemAlloc(IO::FileAnalyse::MPEGFileAnalyse::PackInfo, 1);
				pack->fileOfst = currOfst + j;
				pack->packSize = 4;
				pack->packType = 0xb9;
				me->packs->Add(pack);
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
				pack = MemAlloc(IO::FileAnalyse::MPEGFileAnalyse::PackInfo, 1);
				pack->fileOfst = currOfst + j;
				pack->packSize = frameSize;
				pack->packType = 0xba;
				me->packs->Add(pack);
			}
			else
			{
				frameSize = 6 + (UOSInt)ReadMUInt16(&readBuff[j + 4]);
				pack = MemAlloc(IO::FileAnalyse::MPEGFileAnalyse::PackInfo, 1);
				pack->fileOfst = currOfst + j;
				pack->packSize = frameSize;
				pack->packType = readBuff[j + 3];
				me->packs->Add(pack);
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
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::MPEGFileAnalyse::MPEGFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	NEW_CLASS(this->packs, Data::SyncArrayList<IO::FileAnalyse::MPEGFileAnalyse::PackInfo*>());
	fd->GetRealData(0, 256, buff);
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
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	Sync::Thread::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
}

IO::FileAnalyse::MPEGFileAnalyse::~MPEGFileAnalyse()
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
	DEL_CLASS(this->packs);
}

UOSInt IO::FileAnalyse::MPEGFileAnalyse::GetFrameCount()
{
	return this->packs->GetCount();
}

Bool IO::FileAnalyse::MPEGFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::MPEGFileAnalyse::PackInfo *pack;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->Append((const UTF8Char*)": Type=0x");
	sb->AppendHexBuff(&pack->packType, 1, 0, Text::LBT_NONE);
	switch (pack->packType)
	{
	case 0xb9:
		sb->Append((const UTF8Char*)" (MPEG program end)");
		break;
	case 0xba:
		sb->Append((const UTF8Char*)" (Pack start)");
		break;
	case 0xbb:
		sb->Append((const UTF8Char*)" (System header)");
		break;
	case 0xbc:
		sb->Append((const UTF8Char*)" (Program Stream map)");
		break;
	case 0xbd:
		sb->Append((const UTF8Char*)" (Private Stream 1)");
		break;
	case 0xbe:
		sb->Append((const UTF8Char*)" (Padding Stream)");
		break;
	case 0xbf:
		sb->Append((const UTF8Char*)" (Private Stream 2)");
		break;
	case 0xc0:
		sb->Append((const UTF8Char*)" (Audio Stream 1)");
		break;
	case 0xe0:
		sb->Append((const UTF8Char*)" (Video Stream)");
		break;
	}
	sb->Append((const UTF8Char*)", size=");
	sb->AppendI32((Int32)pack->packSize);
	return true;
}

Bool IO::FileAnalyse::MPEGFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::MPEGFileAnalyse::PackInfo *pack;
	UInt8 *packBuff;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;

	packBuff = MemAlloc(UInt8, pack->packSize);
	this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);

	switch (packBuff[3])
	{
	case 0xb9:
		sb->Append((const UTF8Char*)"MPEG program end");
		break;
	case 0xba:
		sb->Append((const UTF8Char*)"Pack start");
		if ((packBuff[4] & 0xc0) == 0x40)
		{
			Int64 scr_base;
			Int32 scr_ext;
			sb->Append((const UTF8Char*)"\r\nMPEG Version = 2");
			scr_base = (((Int64)(packBuff[4] & 0x38)) << 27) | ((packBuff[4] & 3) << 28) | (packBuff[5] << 20) | ((packBuff[6] & 0xf8) << 12) | ((packBuff[6] & 3) << 13) | (packBuff[7] << 5) | (packBuff[8] >> 3);
			scr_ext = ((packBuff[8] & 3) << 7) | (packBuff[9] >> 1);
			sb->Append((const UTF8Char*)"\r\nSystem Clock Reference Base = ");
			sb->AppendI64(scr_base);
			sb->Append((const UTF8Char*)"\r\nSystem Clock Reference Extension = ");
			sb->AppendI32(scr_ext);
			sb->Append((const UTF8Char*)"\r\nProgram Mux Rate = ");
			sb->AppendU32(ReadMUInt24(&packBuff[10]) >> 2);
			sb->Append((const UTF8Char*)"\r\nPack Stuffing Length = ");
			sb->AppendU16(packBuff[13] & 7);
		}
		else if ((packBuff[4] & 0xf0) == 0x20)
		{
			Int64 scr_base;
			sb->Append((const UTF8Char*)"\r\nMPEG Version = 1");
			scr_base = (((Int64)(packBuff[4] & 0xe)) << 29) | (packBuff[5] << 22) | ((packBuff[6] & 0xfe) << 14) | (packBuff[7] << 7) | ((packBuff[8] & 0xfe) >> 1);
			sb->Append((const UTF8Char*)"\r\nSystem Clock Reference = ");
			sb->AppendI64(scr_base);
			sb->Append((const UTF8Char*)"\r\nProgram Mux Rate = ");
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
			sb->Append((const UTF8Char*)"System header");
			sb->Append((const UTF8Char*)"\r\nRate Bound = ");
			sb->AppendU32((ReadMUInt24(&packBuff[6]) >> 1) & 0x3fffff);
			sb->Append((const UTF8Char*)"\r\nAudio Bound = ");
			sb->AppendU16((UInt16)(packBuff[9] >> 2));
			sb->Append((const UTF8Char*)"\r\nFixed Flag = ");
			sb->AppendU16((UInt16)((packBuff[9] & 2) >> 1));
			sb->Append((const UTF8Char*)"\r\nCSPS Flag = ");
			sb->AppendU16(packBuff[9] & 1);
			sb->Append((const UTF8Char*)"\r\nSystem Audio Lock Flag = ");
			sb->AppendU16((packBuff[10] & 0x80) >> 7);
			sb->Append((const UTF8Char*)"\r\nSystem Video Lock Flag = ");
			sb->AppendU16((packBuff[10] & 0x40) >> 6);
			sb->Append((const UTF8Char*)"\r\nVideo Bound = ");
			sb->AppendU16(packBuff[10] & 0x1f);
			sb->Append((const UTF8Char*)"\r\nPacket Rate Restriction Flag = ");
			sb->AppendU16((packBuff[11] & 0x80) >> 7);
			i = 12;
			while (i < pack->packSize)
			{
				if (packBuff[i] & 0x80)
				{
					sb->Append((const UTF8Char*)"\r\n{");
					sb->Append((const UTF8Char*)"\r\n\tStream ID = 0x");
					sb->AppendHexBuff(&packBuff[i], 1, 0, Text::LBT_NONE);
					sb->Append((const UTF8Char*)"\r\n\tSTD Buffer Bound Scale = ");
					sb->AppendU16((packBuff[i + 1] >> 5) & 1);
					sb->Append((const UTF8Char*)"\r\n\tSTD Buffer Size Bound = ");
					sb->AppendI16(ReadMInt16(&packBuff[i + 1]) & 0x1fff);
					sb->Append((const UTF8Char*)"\r\n}");
				}
				i += 3;
			}
		}
		break;
	case 0xbc:
		sb->Append((const UTF8Char*)"Program Stream map");
		break;
	case 0xbd:
		sb->Append((const UTF8Char*)"Private Stream 1");
		if (this->mpgVer == 2)
		{
			UOSInt i;
			Int64 pts;
			sb->Append((const UTF8Char*)"\r\nPES Scrambling Control = ");
			sb->AppendU16((packBuff[6] & 0x30) >> 4);
			sb->Append((const UTF8Char*)"\r\nPES Priority = ");
			sb->AppendU16((packBuff[6] & 0x8) >> 3);
			sb->Append((const UTF8Char*)"\r\nData Alignment Indicator = ");
			sb->AppendU16((packBuff[6] & 0x4) >> 2);
			sb->Append((const UTF8Char*)"\r\nCopyright = ");
			sb->AppendU16((packBuff[6] & 0x2) >> 1);
			sb->Append((const UTF8Char*)"\r\nOriginal or Copy = ");
			sb->AppendU16(packBuff[6] & 0x1);
			sb->Append((const UTF8Char*)"\r\nPTS DTS flags = ");
			sb->AppendU16((packBuff[7] & 0xc0) >> 6);
			sb->Append((const UTF8Char*)"\r\nESCR flag = ");
			sb->AppendU16((packBuff[7] & 0x20) >> 5);
			sb->Append((const UTF8Char*)"\r\nES Rate Flag = ");
			sb->AppendU16((packBuff[7] & 0x10) >> 4);
			sb->Append((const UTF8Char*)"\r\nDSM Trick Mode flag = ");
			sb->AppendU16((packBuff[7] & 0x8) >> 3);
			sb->Append((const UTF8Char*)"\r\nAdditional Copy Info flag = ");
			sb->AppendU16((packBuff[7] & 0x4) >> 2);
			sb->Append((const UTF8Char*)"\r\nPES CRC flag = ");
			sb->AppendU16((packBuff[7] & 0x2) >> 1);
			sb->Append((const UTF8Char*)"\r\nPES Extension flag = ");
			sb->AppendU16(packBuff[7] & 0x1);
			sb->Append((const UTF8Char*)"\r\nPES Header Data Length = ");
			sb->AppendU16(packBuff[8]);
			i = 9;
			if ((packBuff[7] & 0xc0) == 0x80)
			{
				pts = (((Int64)(packBuff[9] & 0xe)) << 29) | (packBuff[10] << 22) | ((packBuff[11] & 0xfe) << 14) | (packBuff[12] << 7) | (packBuff[13] >> 1);
				sb->Append((const UTF8Char*)"\r\nPresentation Time Stamp = ");
				sb->AppendI64(pts);
				i = 14;
			}
			else if ((packBuff[7] & 0xc0) == 0xc0)
			{
				pts = (((Int64)(packBuff[9] & 0xe)) << 29) | (packBuff[10] << 22) | ((packBuff[11] & 0xfe) << 14) | (packBuff[12] << 7) | (packBuff[13] >> 1);
				sb->Append((const UTF8Char*)"\r\nPresentation Time Stamp = ");
				sb->AppendI64(pts);
				pts = (((Int64)(packBuff[14] & 0xe)) << 29) | (packBuff[15] << 22) | ((packBuff[16] & 0xfe) << 14) | (packBuff[17] << 7) | (packBuff[18] >> 1);
				sb->Append((const UTF8Char*)"\r\nDecoding Time Stamp = ");
				sb->AppendI64(pts);
				i = 19;
			}
			if (packBuff[7] & 0x20)
			{
				Int64 escr_base;
				Int32 escr_ext;
				escr_base = (((Int64)(packBuff[i] & 0x38)) << 27) | ((packBuff[i] & 3) << 28) | (packBuff[i + 1] << 20) | ((packBuff[i + 2] & 0xf8) << 12) | ((packBuff[i + 2] & 3) << 13) | (packBuff[i + 3] << 5) | (packBuff[i + 4] >> 3);
				escr_ext = ((packBuff[i + 4] & 3) << 7) | (packBuff[i + 5] >> 1);
				sb->Append((const UTF8Char*)"\r\nESCR Base = ");
				sb->AppendI64(escr_base);
				sb->Append((const UTF8Char*)"\r\nESCR Extension = ");
				sb->AppendI32(escr_ext);
				i += 6;
			}
			if (packBuff[7] & 0x10)
			{
				sb->Append((const UTF8Char*)"\r\nES Rate = ");
				sb->AppendU32((ReadMUInt24(&packBuff[i]) >> 1) & 0x3fffff);
				i += 3;
			}
			if (packBuff[7] & 0x8)
			{
				/////////////////////////////////
				sb->Append((const UTF8Char*)"\r\nTrick Mode Control = ");
				sb->AppendU16((packBuff[i] & 0xe0) >> 5);
				i += 1;
			}
			if (packBuff[7] & 0x4)
			{
				sb->Append((const UTF8Char*)"\r\nAdditional Copy Info = ");
				sb->AppendU16(packBuff[i] & 0x7f);
				i += 1;
			}
			if (packBuff[7] & 0x2)
			{
				sb->Append((const UTF8Char*)"\r\nPrevious PES packet CRC = 0x");
				sb->AppendHex16(ReadMUInt16(&packBuff[i]));
				i += 2;
			}
			if (packBuff[7] & 0x1)
			{
				sb->Append((const UTF8Char*)"\r\nPES Private Data Flag = ");
				sb->AppendU16((packBuff[i] & 0x80) >> 7);
				sb->Append((const UTF8Char*)"\r\nPack Header Field Flag = ");
				sb->AppendU16((packBuff[i] & 0x40) >> 6);
				sb->Append((const UTF8Char*)"\r\nProgram Packet Sequence Counter Flag = ");
				sb->AppendU16((packBuff[i] & 0x20) >> 5);
				sb->Append((const UTF8Char*)"\r\nP-STD Buffer Flag = ");
				sb->AppendU16((packBuff[i] & 0x10) >> 4);
				sb->Append((const UTF8Char*)"\r\nPES Extension Flag 2 = ");
				sb->AppendU16(packBuff[i] & 0x1);
				////////////////////////////////////
				i += 1;
			}

			i = 9 + (UOSInt)packBuff[8];
			sb->Append((const UTF8Char*)"\r\nStream Type = 0x");
			sb->AppendHex8(packBuff[i]);
			if ((packBuff[i] & 0xf0) == 0xa0)
			{
				sb->Append((const UTF8Char*)" (VOB LPCM Audio)");
				sb->Append((const UTF8Char*)"\r\nNo. of Channels = ");
				sb->AppendU16((UInt16)((packBuff[i + 5] & 7) + 1));
				sb->Append((const UTF8Char*)"\r\nBits per Sample = ");
				switch (packBuff[i + 5] & 0xc0)
				{
				case 0x0:
					sb->Append((const UTF8Char*)"16");
					break;
				case 0x40:
					sb->Append((const UTF8Char*)"20");
					break;
				case 0x80:
					sb->Append((const UTF8Char*)"24");
					break;
				}
				sb->Append((const UTF8Char*)"\r\nSampling Frequency = ");
				sb->AppendU32((packBuff[i + 5] & 0x30)?96000:48000);
				sb->Append((const UTF8Char*)"\r\nVOB LPCM Header = ");
				sb->AppendHexBuff(&packBuff[i], 7, ' ', Text::LBT_NONE);
				i += 7;
			}
			else if ((packBuff[i] & 0xf0) == 0x80)
			{
				sb->Append((const UTF8Char*)" (VOB AC3 Audio)");
				sb->Append((const UTF8Char*)"\r\nVOB AC3 Header = ");
				sb->AppendHexBuff(&packBuff[i], 4, ' ', Text::LBT_NONE);
				i += 4;
			}
			else if (packBuff[i] == 0xff && packBuff[i + 1] == 0xa0)
			{
				sb->Append((const UTF8Char*)" (PSS LPCM Audio)");
				sb->Append((const UTF8Char*)"\r\nPSS Audio Header = ");
				sb->AppendHexBuff(&packBuff[i], 4, ' ', Text::LBT_NONE);
				i += 4;
			}
			else if (packBuff[i] == 0xff && packBuff[i + 1] == 0xa1)
			{
				sb->Append((const UTF8Char*)" (PSS ADPCM Audio)");
				sb->Append((const UTF8Char*)"\r\nPSS Audio Header = ");
				sb->AppendHexBuff(&packBuff[i], 4, ' ', Text::LBT_NONE);
				i += 4;
			}
		
			sb->Append((const UTF8Char*)"\r\nContent Size = ");
			sb->AppendUOSInt((pack->packSize - i));
			sb->Append((const UTF8Char*)"\r\nContent:\r\n");
			sb->AppendHexBuff(&packBuff[i], pack->packSize - i, ' ', Text::LBT_CRLF);
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
				sb->Append((const UTF8Char*)"\r\nStuffing Byte = ");
				sb->AppendI32((Int32)(i - 6));
			}
			if ((packBuff[i] & 0xc0) == 0x40)
			{
				sb->Append((const UTF8Char*)"\r\nSTD Buffer Scale = ");
				sb->AppendU16((packBuff[i] & 0x20) >> 5);
				sb->Append((const UTF8Char*)"\r\nSTD Buffer Size = ");
				sb->AppendI16(ReadMInt16(&packBuff[i]) & 0x1fff);
				i += 2;
			}

			if ((packBuff[i] & 0xf0) == 0x20)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				sb->Append((const UTF8Char*)"\r\nPresentation Time Stamp = ");
				sb->AppendI64(pts);
				i += 5;
			}
			else if ((packBuff[i] & 0xf0) == 0x30)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				sb->Append((const UTF8Char*)"\r\nPresentation Time Stamp = ");
				sb->AppendI64(pts);
				pts = (((Int64)packBuff[i + 5] & 0xe) << 29) | (packBuff[i + 6] << 22) | ((packBuff[i + 7] & 0xfe) << 14) | (packBuff[i + 8] << 7) | (packBuff[i + 9] >> 1);;
				sb->Append((const UTF8Char*)"\r\nDecoding Time Stamp = ");
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

			sb->Append((const UTF8Char*)"\r\nContent Size = ");
			sb->AppendI32((Int32)(pack->packSize - i));
			sb->Append((const UTF8Char*)"\r\nContent:\r\n");
			sb->AppendHexBuff(&packBuff[i], pack->packSize - i, ' ', Text::LBT_CRLF);
		}
		break;
	case 0xbe:
		sb->Append((const UTF8Char*)"Padding Stream");
		sb->Append((const UTF8Char*)"\r\nPadding Size = ");
		sb->AppendI32((Int32)pack->packSize);
		break;
	case 0xbf:
		sb->Append((const UTF8Char*)"Private Stream 2");
		sb->Append((const UTF8Char*)"\r\nContent Size = ");
		sb->AppendI32((Int32)pack->packSize - 6);
		sb->Append((const UTF8Char*)"\r\nContent:\r\n");
		sb->AppendHexBuff(&packBuff[6], pack->packSize - 6, ' ', Text::LBT_CRLF);
		break;
	case 0xc0:
		{
			UOSInt i;
			Int64 pts;
			sb->Append((const UTF8Char*)"Audio Stream 1");
			i = 6;
			while (packBuff[i] & 0x80)
			{
				i++;
			}
			if (i > 6)
			{
				sb->Append((const UTF8Char*)"\r\nStuffing Byte = ");
				sb->AppendI32((Int32)(i - 6));
			}
			if ((packBuff[i] & 0xc0) == 0x40)
			{
				sb->Append((const UTF8Char*)"\r\nSTD Buffer Scale = ");
				sb->AppendU16((packBuff[i] & 0x20) >> 5);
				sb->Append((const UTF8Char*)"\r\nSTD Buffer Size = ");
				sb->AppendI16(ReadMInt16(&packBuff[i]) & 0x1fff);
				i += 2;
			}

			if ((packBuff[i] & 0xf0) == 0x20)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				sb->Append((const UTF8Char*)"\r\nPresentation Time Stamp = ");
				sb->AppendI64(pts);
				i += 5;
			}
			else if ((packBuff[i] & 0xf0) == 0x30)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				sb->Append((const UTF8Char*)"\r\nPresentation Time Stamp = ");
				sb->AppendI64(pts);
				pts = (((Int64)packBuff[i + 5] & 0xe) << 29) | (packBuff[i + 6] << 22) | ((packBuff[i + 7] & 0xfe) << 14) | (packBuff[i + 8] << 7) | (packBuff[i + 9] >> 1);;
				sb->Append((const UTF8Char*)"\r\nDecoding Time Stamp = ");
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

			sb->Append((const UTF8Char*)"\r\nContent Size = ");
			sb->AppendI32((Int32)(pack->packSize - i));
			sb->Append((const UTF8Char*)"\r\nContent:\r\n");
			sb->AppendHexBuff(&packBuff[i], pack->packSize - i, ' ', Text::LBT_CRLF);
		}
		break;
	case 0xe0:
		sb->Append((const UTF8Char*)"Video Stream");
		if (this->mpgVer == 2)
		{
			OSInt i;
			Int64 pts;
			sb->Append((const UTF8Char*)"\r\nPES Scrambling Control = ");
			sb->AppendU16((packBuff[6] & 0x30) >> 4);
			sb->Append((const UTF8Char*)"\r\nPES Priority = ");
			sb->AppendU16((packBuff[6] & 0x8) >> 3);
			sb->Append((const UTF8Char*)"\r\nData Alignment Indicator = ");
			sb->AppendU16((packBuff[6] & 0x4) >> 2);
			sb->Append((const UTF8Char*)"\r\nCopyright = ");
			sb->AppendU16((packBuff[6] & 0x2) >> 1);
			sb->Append((const UTF8Char*)"\r\nOriginal or Copy = ");
			sb->AppendU16(packBuff[6] & 0x1);
			sb->Append((const UTF8Char*)"\r\nPTS DTS flags = ");
			sb->AppendU16((packBuff[7] & 0xc0) >> 6);
			sb->Append((const UTF8Char*)"\r\nESCR flag = ");
			sb->AppendU16((packBuff[7] & 0x20) >> 5);
			sb->Append((const UTF8Char*)"\r\nES Rate Flag = ");
			sb->AppendU16((packBuff[7] & 0x10) >> 4);
			sb->Append((const UTF8Char*)"\r\nDSM Trick Mode flag = ");
			sb->AppendU16((packBuff[7] & 0x8) >> 3);
			sb->Append((const UTF8Char*)"\r\nAdditional Copy Info flag = ");
			sb->AppendU16((packBuff[7] & 0x4) >> 2);
			sb->Append((const UTF8Char*)"\r\nPES CRC flag = ");
			sb->AppendU16((packBuff[7] & 0x2) >> 1);
			sb->Append((const UTF8Char*)"\r\nPES Extension flag = ");
			sb->AppendU16(packBuff[7] & 0x1);
			sb->Append((const UTF8Char*)"\r\nPES Header Data Length = ");
			sb->AppendU16(packBuff[8]);
			i = 9;
			if ((packBuff[7] & 0xc0) == 0x80)
			{
				pts = (((Int64)(packBuff[9] & 0xe)) << 29) | (packBuff[10] << 22) | ((packBuff[11] & 0xfe) << 14) | (packBuff[12] << 7) | (packBuff[13] >> 1);
				sb->Append((const UTF8Char*)"\r\nPresentation Time Stamp = ");
				sb->AppendI64(pts);
				i = 14;
			}
			else if ((packBuff[7] & 0xc0) == 0xc0)
			{
				pts = (((Int64)(packBuff[9] & 0xe)) << 29) | (packBuff[10] << 22) | ((packBuff[11] & 0xfe) << 14) | (packBuff[12] << 7) | (packBuff[13] >> 1);
				sb->Append((const UTF8Char*)"\r\nPresentation Time Stamp = ");
				sb->AppendI64(pts);
				pts = (((Int64)(packBuff[14] & 0xe)) << 29) | (packBuff[15] << 22) | ((packBuff[16] & 0xfe) << 14) | (packBuff[17] << 7) | (packBuff[18] >> 1);
				sb->Append((const UTF8Char*)"\r\nDecoding Time Stamp = ");
				sb->AppendI64(pts);
				i = 19;
			}
			if (packBuff[7] & 0x20)
			{
				Int64 escr_base;
				Int32 escr_ext;
				escr_base = (((Int64)(packBuff[i] & 0x38)) << 27) | ((packBuff[i] & 3) << 28) | (packBuff[i + 1] << 20) | ((packBuff[i + 2] & 0xf8) << 12) | ((packBuff[i + 2] & 3) << 13) | (packBuff[i + 3] << 5) | (packBuff[i + 4] >> 3);
				escr_ext = ((packBuff[i + 4] & 3) << 7) | (packBuff[i + 5] >> 1);
				sb->Append((const UTF8Char*)"\r\nESCR Base = ");
				sb->AppendI64(escr_base);
				sb->Append((const UTF8Char*)"\r\nESCR Extension = ");
				sb->AppendI32(escr_ext);
				i += 6;
			}
			if (packBuff[7] & 0x10)
			{
				sb->Append((const UTF8Char*)"\r\nES Rate = ");
				sb->AppendU32((ReadMUInt24(&packBuff[i]) >> 1) & 0x3fffff);
				i += 3;
			}
			if (packBuff[7] & 0x8)
			{
				/////////////////////////////////
				sb->Append((const UTF8Char*)"\r\nTrick Mode Control = ");
				sb->AppendU16((packBuff[i] & 0xe0) >> 5);
				i += 1;
			}
			if (packBuff[7] & 0x4)
			{
				sb->Append((const UTF8Char*)"\r\nAdditional Copy Info = ");
				sb->AppendU16(packBuff[i] & 0x7f);
				i += 1;
			}
			if (packBuff[7] & 0x2)
			{
				sb->Append((const UTF8Char*)"\r\nPrevious PES packet CRC = 0x");
				sb->AppendHex16(ReadMUInt16(&packBuff[i]));
				i += 2;
			}
			if (packBuff[7] & 0x1)
			{
				sb->Append((const UTF8Char*)"\r\nPES Private Data Flag = ");
				sb->AppendU16((packBuff[i] & 0x80) >> 7);
				sb->Append((const UTF8Char*)"\r\nPack Header Field Flag = ");
				sb->AppendU16((packBuff[i] & 0x40) >> 6);
				sb->Append((const UTF8Char*)"\r\nProgram Packet Sequence Counter Flag = ");
				sb->AppendU16((packBuff[i] & 0x20) >> 5);
				sb->Append((const UTF8Char*)"\r\nP-STD Buffer Flag = ");
				sb->AppendU16((packBuff[i] & 0x10) >> 4);
				sb->Append((const UTF8Char*)"\r\nPES Extension Flag 2 = ");
				sb->AppendU16(packBuff[i] & 0x1);
				////////////////////////////////////
				i += 1;
			}
		
			sb->Append((const UTF8Char*)"\r\nContent Size = ");
			sb->AppendI32((Int32)(pack->packSize - 9 - packBuff[8]));
			sb->Append((const UTF8Char*)"\r\nContent:\r\n");
			sb->AppendHexBuff(&packBuff[9 + packBuff[8]], pack->packSize - 9 - packBuff[8], ' ', Text::LBT_CRLF);
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
				sb->Append((const UTF8Char*)"\r\nStuffing Byte = ");
				sb->AppendI32((Int32)(i - 6));
			}
			if ((packBuff[i] & 0xc0) == 0x40)
			{
				sb->Append((const UTF8Char*)"\r\nSTD Buffer Scale = ");
				sb->AppendU16((packBuff[i] & 0x20) >> 5);
				sb->Append((const UTF8Char*)"\r\nSTD Buffer Size = ");
				sb->AppendI16(ReadMInt16(&packBuff[i]) & 0x1fff);
				i += 2;
			}

			if ((packBuff[i] & 0xf0) == 0x20)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				sb->Append((const UTF8Char*)"\r\nPresentation Time Stamp = ");
				sb->AppendI64(pts);
				i += 5;
			}
			else if ((packBuff[i] & 0xf0) == 0x30)
			{
				pts = (((Int64)packBuff[i] & 0xe) << 29) | (packBuff[i + 1] << 22) | ((packBuff[i + 2] & 0xfe) << 14) | (packBuff[i + 3] << 7) | (packBuff[i + 4] >> 1);;
				sb->Append((const UTF8Char*)"\r\nPresentation Time Stamp = ");
				sb->AppendI64(pts);
				pts = (((Int64)packBuff[i + 5] & 0xe) << 29) | (packBuff[i + 6] << 22) | ((packBuff[i + 7] & 0xfe) << 14) | (packBuff[i + 8] << 7) | (packBuff[i + 9] >> 1);;
				sb->Append((const UTF8Char*)"\r\nDecoding Time Stamp = ");
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

			sb->Append((const UTF8Char*)"\r\nContent Size = ");
			sb->AppendI32((Int32)(pack->packSize - i));
			sb->Append((const UTF8Char*)"\r\nContent:\r\n");
			sb->AppendHexBuff(&packBuff[i], pack->packSize - i, ' ', Text::LBT_CRLF);
		}
		break;
	}

	MemFree(packBuff);
	return true;
}

UOSInt IO::FileAnalyse::MPEGFileAnalyse::GetFrameIndex(UInt64 ofst)
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

Bool IO::FileAnalyse::MPEGFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::MPEGFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::MPEGFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
	UInt8 *readBuff;
	UOSInt readSize;
	UOSInt buffSize;
	UOSInt j;
	UOSInt frameSize;
	UInt64 readOfst;
	Bool valid = true;
	IO::FileStream *dfs;
	NEW_CLASS(dfs, IO::FileStream(outputFile, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (dfs->IsError())
	{
		DEL_CLASS(dfs);
		return false;
	}
	readBuff = MemAlloc(UInt8, 1048576);
	buffSize = 0;
	readOfst = 0;
	while (true)
	{
		if (buffSize < 256)
		{
			readSize = this->fd->GetRealData(readOfst, 256, &readBuff[buffSize]);
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
			MemCopyO(readBuff, &readBuff[j], buffSize - j);
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
			dfs->Write(&readBuff[j], 4);
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
			dfs->Write(&readBuff[j], frameSize);
			if (j + frameSize < buffSize)
			{
				MemCopyO(readBuff, &readBuff[j + frameSize], buffSize - j - frameSize);
				buffSize -= j + frameSize;
			}
			else
			{
				buffSize = 0;
			}
		}
		else
		{
			readSize = this->fd->GetRealData(readOfst, j + frameSize - buffSize, &readBuff[buffSize]);
			readOfst += readSize;
			if (readSize == j + frameSize - buffSize)
			{
				dfs->Write(&readBuff[j], frameSize);
				buffSize = 0;
			}
			else
			{
				valid = false;
				break;
			}
		}
	}

	MemFree(readBuff);
	DEL_CLASS(dfs);
	return valid;
}
