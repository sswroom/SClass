#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/RIFFFileAnalyse.h"
#include "Sync/Thread.h"

void IO::FileAnalyse::RIFFFileAnalyse::ParseRange(UInt64 ofst, UInt64 size)
{
	UInt8 buff[12];
	UInt64 endOfst = ofst + size;
	UInt32 sz;
	PackInfo *pack;

	while (ofst <= (endOfst - 12) && !this->threadToStop)
	{
		if (this->pauseParsing)
		{
			Sync::Thread::Sleep(100);
		}
		else
		{
			this->fd->GetRealData(ofst, 12, buff);
			sz = ReadUInt32(&buff[4]);
			if (ofst + sz + 8 > endOfst)
			{
				return;
			}
			pack = MemAlloc(PackInfo, 1);
			pack->fileOfst = ofst;
			pack->packSize = sz + 8;
			pack->packType = ReadNInt32(buff);
			if (pack->packType == *(Int32*)"RIFF" || pack->packType == *(Int32*)"LIST")
			{
				pack->subPackType = ReadNInt32(&buff[8]);
			}
			else
			{
				pack->subPackType = 0;
			}
			this->packs->Add(pack);
			
			if (pack->subPackType != 0)
			{
				ParseRange(ofst + 12, sz - 4);
			}
			ofst += sz + 8;
			if (sz & 1)
			{
				ofst++;
			}
		}
	}
}

UInt32 __stdcall IO::FileAnalyse::RIFFFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::RIFFFileAnalyse *me = (IO::FileAnalyse::RIFFFileAnalyse*)userObj;
	me->threadRunning = true;
	me->threadStarted = true;
	me->ParseRange(0, me->fd->GetDataSize());
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::RIFFFileAnalyse::RIFFFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	NEW_CLASS(this->packs, Data::SyncArrayList<PackInfo*>());
	fd->GetRealData(0, 256, buff);
	if (ReadNInt32(buff) != *(Int32*)"RIFF")
	{
		return;
	}
	UInt32 size = ReadUInt32(&buff[4]);
	if (size + 8 > fd->GetDataSize())
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

IO::FileAnalyse::RIFFFileAnalyse::~RIFFFileAnalyse()
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
	DEL_LIST_FUNC(this->packs, MemFree);
	DEL_CLASS(this->packs);
}

UOSInt IO::FileAnalyse::RIFFFileAnalyse::GetFrameCount()
{
	return this->packs->GetCount();
}

Bool IO::FileAnalyse::RIFFFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	PackInfo *pack;
	UInt8 buff[5];
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->Append((const UTF8Char*)": Type=");
	*(Int32*)buff = pack->packType;
	buff[4] = 0;
	sb->Append((UTF8Char*)buff);
	if (pack->subPackType != 0)
	{
		sb->Append((const UTF8Char*)", SubType=");
		*(Int32*)buff = pack->subPackType;
		buff[4] = 0;
		sb->Append((UTF8Char*)buff);
	}
	sb->Append((const UTF8Char*)", size=");
	sb->AppendUOSInt(pack->packSize);
	return true;
}

Bool IO::FileAnalyse::RIFFFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
{
	PackInfo *pack;
	UInt8 *packBuff;
	UInt8 buff[5];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;

	sb->AppendU64(pack->fileOfst);
	sb->Append((const UTF8Char*)": Type=");
	*(Int32*)buff = pack->packType;
	buff[4] = 0;
	sb->Append((UTF8Char*)buff);
	if (pack->subPackType != 0)
	{
		sb->Append((const UTF8Char*)", SubType=");
		*(Int32*)buff = pack->subPackType;
		buff[4] = 0;
		sb->Append((UTF8Char*)buff);
	}
	sb->Append((const UTF8Char*)", size=");
	sb->AppendUOSInt(pack->packSize);

	if (pack->packType == *(Int32*)"avih")
	{
		packBuff = MemAlloc(UInt8, pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->Append((const UTF8Char*)"\r\nMicroSec Per Frame = ");
		sb->AppendU32(ReadUInt32(&packBuff[0]));
		sb->Append((const UTF8Char*)"\r\nMax Bytes Per Second = ");
		sb->AppendU32(ReadUInt32(&packBuff[4]));
		sb->Append((const UTF8Char*)"\r\nPadding Granularity = ");
		sb->AppendU32(ReadUInt32(&packBuff[8]));
		sb->Append((const UTF8Char*)"\r\nFlags = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[12]));
		sb->Append((const UTF8Char*)"\r\nTotal Frames = ");
		sb->AppendU32(ReadUInt32(&packBuff[16]));
		sb->Append((const UTF8Char*)"\r\nInitial Frames = ");
		sb->AppendU32(ReadUInt32(&packBuff[20]));
		sb->Append((const UTF8Char*)"\r\nStream Count = ");
		sb->AppendU32(ReadUInt32(&packBuff[24]));
		sb->Append((const UTF8Char*)"\r\nSuggested Buffer Size = ");
		sb->AppendU32(ReadUInt32(&packBuff[28]));
		sb->Append((const UTF8Char*)"\r\nWidth = ");
		sb->AppendU32(ReadUInt32(&packBuff[32]));
		sb->Append((const UTF8Char*)"\r\nHeight = ");
		sb->AppendU32(ReadUInt32(&packBuff[36]));

		MemFree(packBuff);
	}
	else if (pack->packType == *(Int32*)"strh")
	{
		packBuff = MemAlloc(UInt8, pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->Append((const UTF8Char*)"\r\nfccType = ");
		*(Int32*)buff = *(Int32*)&packBuff[0];
		sb->Append((UTF8Char*)buff);
		sb->Append((const UTF8Char*)"\r\nfccHandler = ");
		if (*(Int32*)&packBuff[4] == 0)
		{
			sb->Append((const UTF8Char*)"0");
		}
		else
		{
			*(Int32*)buff = *(Int32*)&packBuff[4];
			sb->Append((UTF8Char*)buff);
		}
		sb->Append((const UTF8Char*)"\r\nFlags = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[8]));
		sb->Append((const UTF8Char*)"\r\nPriority = ");
		sb->AppendU32(ReadUInt32(&packBuff[12]));
		sb->Append((const UTF8Char*)"\r\nInitial Frames = ");
		sb->AppendU32(ReadUInt32(&packBuff[16]));
		sb->Append((const UTF8Char*)"\r\nScale = ");
		sb->AppendU32(ReadUInt32(&packBuff[20]));
		sb->Append((const UTF8Char*)"\r\nRate = ");
		sb->AppendU32(ReadUInt32(&packBuff[24]));
		sb->Append((const UTF8Char*)"\r\nStart = ");
		sb->AppendU32(ReadUInt32(&packBuff[28]));
		sb->Append((const UTF8Char*)"\r\nLength = ");
		sb->AppendU32(ReadUInt32(&packBuff[32]));
		sb->Append((const UTF8Char*)"\r\nSuggested Buffer Size = ");
		sb->AppendU32(ReadUInt32(&packBuff[36]));
		sb->Append((const UTF8Char*)"\r\nQuality = ");
		sb->AppendU32(ReadUInt32(&packBuff[40]));
		sb->Append((const UTF8Char*)"\r\nSample Size = ");
		sb->AppendU32(ReadUInt32(&packBuff[44]));

		MemFree(packBuff);
	}
	else if (pack->packType == *(Int32*)"strf")
	{
		packBuff = MemAlloc(UInt8, pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		if (ReadUInt32(packBuff) == pack->packSize - 8)
		{
			sb->Append((const UTF8Char*)"\r\nbiSize = ");
			sb->AppendU32(ReadUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nbiWidth = ");
			sb->AppendU32(ReadUInt32(&packBuff[4]));
			sb->Append((const UTF8Char*)"\r\nbiHeight = ");
			sb->AppendU32(ReadUInt32(&packBuff[8]));
			sb->Append((const UTF8Char*)"\r\nbiPlanes = ");
			sb->AppendU16(ReadUInt16(&packBuff[12]));
			sb->Append((const UTF8Char*)"\r\nbiBitCount = ");
			sb->AppendU16(ReadUInt16(&packBuff[14]));
			sb->Append((const UTF8Char*)"\r\nbiCompression = ");
			if (ReadUInt32(&packBuff[16]) == 0)
			{
				sb->Append((const UTF8Char*)"0");
			}
			else
			{
				*(Int32*)buff = *(Int32*)&packBuff[16];
				sb->Append((UTF8Char*)buff);
			}
			sb->Append((const UTF8Char*)"\r\nbiSizeImage = ");
			sb->AppendU32(ReadUInt32(&packBuff[20]));
			sb->Append((const UTF8Char*)"\r\nbiXPelsPerMeter = ");
			sb->AppendU32(ReadUInt32(&packBuff[24]));
			sb->Append((const UTF8Char*)"\r\nbiYPelsPerMeter = ");
			sb->AppendU32(ReadUInt32(&packBuff[28]));
			sb->Append((const UTF8Char*)"\r\nbiClrUsed = ");
			sb->AppendU32(ReadUInt32(&packBuff[32]));
			sb->Append((const UTF8Char*)"\r\nbiClrImportant = ");
			sb->AppendU32(ReadUInt32(&packBuff[36]));
		}
		else
		{
			sb->Append((const UTF8Char*)"\r\nwFormatTag = 0x");
			sb->AppendHex16(ReadUInt16(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nnChannels = ");
			sb->AppendU16(ReadUInt16(&packBuff[2]));
			sb->Append((const UTF8Char*)"\r\nnSamplesPerSecond = ");
			sb->AppendU32(ReadUInt32(&packBuff[4]));
			sb->Append((const UTF8Char*)"\r\nnAvgBytesPerSec = ");
			sb->AppendU32(ReadUInt32(&packBuff[8]));
			sb->Append((const UTF8Char*)"\r\nnBlockAlign = ");
			sb->AppendU16(ReadUInt16(&packBuff[12]));
			sb->Append((const UTF8Char*)"\r\nwBitsPerSample = ");
			sb->AppendU16(ReadUInt16(&packBuff[14]));
		}

		MemFree(packBuff);
	}
	else if (pack->packType == *(Int32*)"JUNK")
	{
		packBuff = MemAlloc(UInt8, pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(packBuff, pack->packSize - 8, ' ', Text::LBT_CRLF);

		MemFree(packBuff);
	}
	else if (pack->packType == *(Int32*)"dmlh")
	{
		packBuff = MemAlloc(UInt8, pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->Append((const UTF8Char*)"\r\nFrame Count = ");
		sb->AppendU32(ReadUInt32(&packBuff[0]));

		MemFree(packBuff);
	}
	else if (pack->packType == *(Int32*)"00db" || pack->packType == *(Int32*)"00dc" || pack->packType == *(Int32*)"01wb" || pack->packType == *(Int32*)"02wb")
	{
		packBuff = MemAlloc(UInt8, pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(packBuff, pack->packSize - 8, ' ', Text::LBT_CRLF);

		MemFree(packBuff);
	}
	else if (pack->packType == *(Int32*)"idx1")
	{
		packBuff = MemAlloc(UInt8, pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		i = 0;
		j = (pack->packSize - 8) >> 4;
		k = 0;
		sb->Append((const UTF8Char*)"\r\nIndex List:");
		while (i < j)
		{
			*(Int32*)buff = *(Int32*)&packBuff[k];
			sb->Append((const UTF8Char*)"\r\nType=");
			sb->Append((UTF8Char*)buff);
			sb->Append((const UTF8Char*)", Offset=");
			sb->AppendU32(ReadUInt32(&packBuff[k + 8]));
			sb->Append((const UTF8Char*)", Size=");
			sb->AppendU32(ReadUInt32(&packBuff[k + 12]));
			if (packBuff[k + 4] != 0)
			{
				sb->Append((const UTF8Char*)" (key)");
			}
			k += 16;
			i++;
		}

		MemFree(packBuff);
	}
	else if (pack->packType == *(Int32*)"anih")
	{
		packBuff = MemAlloc(UInt8, pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->Append((const UTF8Char*)"\r\ncbSize = ");
		sb->AppendU32(ReadUInt32(&packBuff[0]));
		sb->Append((const UTF8Char*)"\r\nNumber of Frames = ");
		sb->AppendU32(ReadUInt32(&packBuff[4]));
		sb->Append((const UTF8Char*)"\r\nNumber of Steps = ");
		sb->AppendU32(ReadUInt32(&packBuff[8]));
		sb->Append((const UTF8Char*)"\r\nWidth");
		sb->AppendU32(ReadUInt32(&packBuff[12]));
		sb->Append((const UTF8Char*)"\r\nHeight = ");
		sb->AppendU32(ReadUInt32(&packBuff[16]));
		sb->Append((const UTF8Char*)"\r\nBits per Pixel = ");
		sb->AppendU32(ReadUInt32(&packBuff[20]));
		sb->Append((const UTF8Char*)"\r\nNumber of color planes = ");
		sb->AppendU32(ReadUInt32(&packBuff[24]));
		sb->Append((const UTF8Char*)"\r\nDefault frame display rate = ");
		sb->AppendU32(ReadUInt32(&packBuff[28]));
		sb->Append((const UTF8Char*)"\r\nAttributes Flags = 0x");
		sb->AppendHex32(ReadUInt32(&packBuff[32]));

		MemFree(packBuff);
	}
	else if (pack->packType == *(Int32*)"icon")
	{
		packBuff = MemAlloc(UInt8, pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->Append((const UTF8Char*)"\r\nFile Header = ");
		sb->AppendU16(ReadUInt16(&packBuff[0]));
		sb->Append((const UTF8Char*)"\r\nFile Format = ");
		sb->AppendU16(ReadUInt16(&packBuff[2]));
		sb->Append((const UTF8Char*)"\r\nNumber of Images = ");
		sb->AppendU16(ReadUInt16(&packBuff[4]));
		OSInt i = 0;
		OSInt j = ReadUInt16(&packBuff[4]);
		while (i < j)
		{
			sb->Append((const UTF8Char*)"\r\nImage ");
			sb->AppendOSInt(i);
			sb->Append((const UTF8Char*)":");

			sb->Append((const UTF8Char*)"\r\nWidth = ");
			sb->AppendU16(packBuff[6 + (i << 4)]);
			sb->Append((const UTF8Char*)"\r\nHeight = ");
			sb->AppendU16(packBuff[7 + (i << 4)]);
			sb->Append((const UTF8Char*)"\r\nNumber of Color = ");
			sb->AppendU16(packBuff[8 + (i << 4)]);
			sb->Append((const UTF8Char*)"\r\nReserved = ");
			sb->AppendU16(packBuff[9 + (i << 4)]);
			sb->Append((const UTF8Char*)"\r\nHotSpot X = ");
			sb->AppendU16(ReadUInt16(&packBuff[10 + (i << 4)]));
			sb->Append((const UTF8Char*)"\r\nHotSpot Y = ");
			sb->AppendU16(ReadUInt16(&packBuff[12 + (i << 4)]));
			sb->Append((const UTF8Char*)"\r\nImage Size = ");
			sb->AppendU32(ReadUInt32(&packBuff[14 + (i << 4)]));
			sb->Append((const UTF8Char*)"\r\nOffset = ");
			sb->AppendU32(ReadUInt32(&packBuff[18 + (i << 4)]));
			
			i++;
		}

		MemFree(packBuff);
	}
	return true;
}

Bool IO::FileAnalyse::RIFFFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::RIFFFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::RIFFFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
/*	UInt8 *readBuff;
	OSInt readSize;
	OSInt buffSize;
	OSInt j;
	OSInt frameSize;
	Int64 readOfst;
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
			MemCopy(readBuff, &readBuff[j], buffSize - j);
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
				frameSize = 14 + (readBuff[j + 13] & 7);
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
			frameSize = 6 + ReadMUInt16(&readBuff[j + 4]);
		}
		if (j + frameSize <= buffSize)
		{
			dfs->Write(&readBuff[j], frameSize);
			if (j + frameSize < buffSize)
			{
				MemCopy(readBuff, &readBuff[j + frameSize], buffSize - j - frameSize);
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
	return valid;*/
	return false;
}
