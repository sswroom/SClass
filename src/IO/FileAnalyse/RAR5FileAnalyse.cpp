#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/RAR5FileAnalyse.h"
#include "Sync/Thread.h"

// https://www.rarlab.com/technote.htm

const UInt8 *IO::FileAnalyse::RAR5FileAnalyse::ReadVInt(const UInt8 *buffPtr, UInt64 *val)
{
	UInt64 v = 0;
	OSInt sh = 0;
	UInt8 b;
	while (true)
	{
		b = *buffPtr++;
		v |= ((UInt64)(b & 0x7f)) << sh;
		if ((b & 0x80) == 0)
		{
			break;
		}
		sh += 7;
	}
	*val = v;
	return buffPtr;
}

UInt32 __stdcall IO::FileAnalyse::RAR5FileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::RAR5FileAnalyse *me = (IO::FileAnalyse::RAR5FileAnalyse*)userObj;
	UInt8 buff[128];
	const UInt8 *buffPtr;
	UInt64 iVal;
	UInt64 headerFlags;
	UInt64 currOfst = 8;
	UInt64 endOfst = me->fd->GetDataSize();
	IO::FileAnalyse::RAR5FileAnalyse::BlockInfo *block;
	me->threadRunning = true;
	me->threadStarted = true;
	while (!me->threadToStop && currOfst + 8 <= endOfst)
	{
		me->fd->GetRealData(currOfst, 128, buff);
		buffPtr = buff + 4;
		buffPtr = ReadVInt(buffPtr, &iVal);
		block = MemAlloc(IO::FileAnalyse::RAR5FileAnalyse::BlockInfo, 1);
		block->fileOfst = currOfst;
		if (iVal >= 1048576 * 2)
		{
			MemFree(block);
			break;
		}
		block->headerSize = (UInt32)(iVal + (UOSInt)(buffPtr - buff));
		buffPtr = ReadVInt(buffPtr, &iVal);
		if (iVal > 5)
		{
			MemFree(block);
			break;
		}
		block->headerType = (UInt32)iVal;
		buffPtr = ReadVInt(buffPtr, &iVal);
		headerFlags = iVal;
		if (headerFlags & 1)
		{
			buffPtr = ReadVInt(buffPtr, &iVal); //extraSize;
		}
		if (headerFlags & 2)
		{
			buffPtr = ReadVInt(buffPtr, &iVal);
			block->dataSize = iVal;
		}
		else
		{
			block->dataSize = 0;
		}
		me->packs->Add(block);
		currOfst += block->headerSize + block->dataSize;
	}
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::RAR5FileAnalyse::RAR5FileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	NEW_CLASS(this->packs, Data::SyncArrayList<IO::FileAnalyse::RAR5FileAnalyse::BlockInfo*>());
	fd->GetRealData(0, 256, buff);
	if (ReadInt32(&buff[0]) != 0x21726152 || ReadInt32(&buff[4]) != 0x0001071A)
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

IO::FileAnalyse::RAR5FileAnalyse::~RAR5FileAnalyse()
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

UOSInt IO::FileAnalyse::RAR5FileAnalyse::GetFrameCount()
{
	return this->packs->GetCount();
}

Bool IO::FileAnalyse::RAR5FileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::RAR5FileAnalyse::BlockInfo *pack;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->Append((const UTF8Char*)": Type=");
	sb->AppendU32(pack->headerType);
	sb->Append((const UTF8Char*)", HeaderSize=");
	sb->AppendU32(pack->headerSize);
	sb->Append((const UTF8Char*)", DataSize=");
	sb->AppendU64(pack->dataSize);
	return true;
}

Bool IO::FileAnalyse::RAR5FileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::RAR5FileAnalyse::BlockInfo *pack;
	UInt8 *packBuff;
	UInt64 iVal;
	UInt64 headerFlags;
	UInt64 extraSize;
	const UInt8 *packPtr;
	const UInt8 *extraEnd;
	const UInt8 *packEnd;
	const UInt8 *nextPtr;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;

	sb->AppendU64(pack->fileOfst);
	sb->Append((const UTF8Char*)":");
	packBuff = MemAlloc(UInt8, pack->headerSize);
	this->fd->GetRealData(pack->fileOfst, pack->headerSize, packBuff);

	sb->Append((const UTF8Char*)"\r\nBlock CRC = 0x");
	sb->AppendHex32(ReadUInt32(&packBuff[0]));
	packPtr = packBuff + 4;
	packEnd = packBuff + pack->headerSize;
	packPtr = ReadVInt(packPtr, &iVal);
	sb->Append((const UTF8Char*)"\r\nHeader Size = ");
	sb->AppendU64(iVal);
	packPtr = ReadVInt(packPtr, &iVal);
	sb->Append((const UTF8Char*)"\r\nHeader Type = ");
	sb->AppendU64(iVal);
	switch (iVal)
	{
	case 1:
		sb->Append((const UTF8Char*)" (Main archive header)");
		break;
	case 2:
		sb->Append((const UTF8Char*)" (File header)");
		break;
	case 3:
		sb->Append((const UTF8Char*)" (Service header)");
		break;
	case 4:
		sb->Append((const UTF8Char*)" (Archive encryption header)");
		break;
	case 5:
		sb->Append((const UTF8Char*)" (End of archive header)");
		break;
	}
	packPtr = ReadVInt(packPtr, &headerFlags);
	sb->Append((const UTF8Char*)"\r\nHeader Flags = 0x");
	sb->AppendHex16((UInt16)iVal);
	extraSize = 0;
	if (headerFlags & 1)
	{
		packPtr = ReadVInt(packPtr, &extraSize);
		sb->Append((const UTF8Char*)"\r\nExtra Area Size = ");
		sb->AppendU64(extraSize);
	}
	if (headerFlags & 2)
	{
		packPtr = ReadVInt(packPtr, &iVal);
		sb->Append((const UTF8Char*)"\r\nData Size = ");
		sb->AppendU64(iVal);
	}

	if (pack->headerType == 1)
	{
		packPtr = ReadVInt(packPtr, &iVal);
		sb->Append((const UTF8Char*)"\r\nArchive flags = 0x");
		sb->AppendHex16((UInt16)iVal);
		if (iVal & 1)
		{
			sb->Append((const UTF8Char*)" (Volume)");
		}
		if (iVal & 2)
		{
			sb->Append((const UTF8Char*)" (Volume number field is present)");
		}
		if (iVal & 4)
		{
			sb->Append((const UTF8Char*)" (Solid archive)");
		}
		if (iVal & 8)
		{
			sb->Append((const UTF8Char*)" (Recovery record is present)");
		}
		if (iVal & 16)
		{
			sb->Append((const UTF8Char*)" (Locked archive)");
		}

		if (iVal & 2)
		{
			packPtr = ReadVInt(packPtr, &iVal);
			sb->Append((const UTF8Char*)"\r\nVolume number = ");
			sb->AppendU64(iVal);
		}
		extraEnd = packPtr + extraSize;
		if (extraEnd <= packEnd)
		{
			while (packPtr < extraEnd)
			{
				packPtr = ReadVInt(packPtr, &extraSize);
				nextPtr = packPtr + extraSize;
				if (nextPtr > extraEnd)
				{
					break;
				}
				packPtr = ReadVInt(packPtr, &iVal);
				sb->Append((const UTF8Char*)"\r\nExtra Rec Size = ");
				sb->AppendU64(extraSize);
				sb->Append((const UTF8Char*)", Type = ");
				sb->AppendU64(iVal);
				if (iVal == 1)
				{
					sb->Append((const UTF8Char*)" (Locator)");
					packPtr = ReadVInt(packPtr, &iVal);
					sb->Append((const UTF8Char*)", Flags = 0x");
					sb->AppendHex16((UInt16)iVal);
					if (iVal & 1)
					{
						packPtr = ReadVInt(packPtr, &extraSize);
						sb->Append((const UTF8Char*)", Quick open offset = 0x");
						sb->AppendHex64V(extraSize);
					}
					if (iVal & 2)
					{
						packPtr = ReadVInt(packPtr, &extraSize);
						sb->Append((const UTF8Char*)", Recovery record offset = 0x");
						sb->AppendHex64V(extraSize);
					}
				}
				packPtr = nextPtr;
			}
		}
	}
	else if (pack->headerType == 2 || pack->headerType == 3)
	{
		packPtr = ReadVInt(packPtr, &headerFlags);
		sb->Append((const UTF8Char*)"\r\nFile flags = 0x");
		sb->AppendHex16((UInt16)headerFlags);
		if (headerFlags & 1)
		{
			sb->Append((const UTF8Char*)" (Directory file system object)");
		}
		if (headerFlags & 2)
		{
			sb->Append((const UTF8Char*)" (Time field in Unix format is present)");
		}
		if (headerFlags & 4)
		{
			sb->Append((const UTF8Char*)" (CRC32 field is present)");
		}
		if (headerFlags & 8)
		{
			sb->Append((const UTF8Char*)" (Unpacked size is unknown)");
		}

		packPtr = ReadVInt(packPtr, &iVal);
		sb->Append((const UTF8Char*)"\r\nUnpacked size = ");
		sb->AppendU64(iVal);
		packPtr = ReadVInt(packPtr, &iVal);
		sb->Append((const UTF8Char*)"\r\nAttributes = ");
		sb->AppendU64(iVal);
		if (headerFlags & 2)
		{
			sb->Append((const UTF8Char*)"\r\nmtime = ");
			sb->AppendU32(ReadUInt32(packPtr));
			packPtr += 4;
		}
		if (headerFlags & 4)
		{
			sb->Append((const UTF8Char*)"\r\nData CRC32 = 0x");
			sb->AppendHex32(ReadUInt32(packPtr));
			packPtr += 4;
		}
		packPtr = ReadVInt(packPtr, &iVal);
		sb->Append((const UTF8Char*)"\r\nCompression version = ");
		sb->AppendU32((UInt8)(iVal & 0x3f));
		sb->Append((const UTF8Char*)"\r\nCompression Solid Flag = ");
		sb->AppendU32((UInt8)((iVal & 0x40) >> 6));
		sb->Append((const UTF8Char*)"\r\nCompression method = ");
		sb->AppendU32((UInt8)((iVal & 0x380) >> 7));
		sb->Append((const UTF8Char*)"\r\nCompression dir size = ");
		sb->AppendU32((UInt8)((iVal & 0x3c00) >> 10));
		packPtr = ReadVInt(packPtr, &iVal);
		sb->Append((const UTF8Char*)"\r\nHost OS = ");
		sb->AppendU64(iVal);
		packPtr = ReadVInt(packPtr, &iVal);
		sb->Append((const UTF8Char*)"\r\nName length = ");
		sb->AppendU64(iVal);
		sb->Append((const UTF8Char*)"\r\nName = ");
		sb->AppendC(packPtr, (UOSInt)iVal);
		packPtr += iVal;

		extraEnd = packPtr + extraSize;
		if (extraEnd <= packEnd)
		{
			while (packPtr < extraEnd)
			{
				packPtr = ReadVInt(packPtr, &extraSize);
				nextPtr = packPtr + extraSize;
				if (nextPtr > extraEnd)
				{
					break;
				}
				packPtr = ReadVInt(packPtr, &iVal);
				sb->Append((const UTF8Char*)"\r\nExtra Rec Size = ");
				sb->AppendU64(extraSize);
				sb->Append((const UTF8Char*)", Type = ");
				sb->AppendU64(iVal);
				if (iVal == 1)
				{
					sb->Append((const UTF8Char*)" (File encryption)");
				}
				else if (iVal == 2)
				{
					sb->Append((const UTF8Char*)" (File hash)");
				}
				else if (iVal == 3)
				{
					Data::DateTime dt;
					sb->Append((const UTF8Char*)" (File time)");
					packPtr = ReadVInt(packPtr, &headerFlags);
					sb->Append((const UTF8Char*)", Flags = 0x");
					sb->AppendHex16((UInt16)headerFlags);
					if (headerFlags & 2)
					{
						if (headerFlags & 1)
						{
							dt.SetUnixTimestamp(ReadUInt32(packPtr));
							packPtr += 4;
						}
						else
						{
							dt.SetValueFILETIME((void*)packPtr);
							packPtr += 8;
						}
						sb->Append((const UTF8Char*)", mtime = ");
						sb->AppendDate(&dt);
					}
					if (headerFlags & 4)
					{
						if (headerFlags & 1)
						{
							dt.SetUnixTimestamp(ReadUInt32(packPtr));
							packPtr += 4;
						}
						else
						{
							dt.SetValueFILETIME((void*)packPtr);
							packPtr += 8;
						}
						sb->Append((const UTF8Char*)", ctime = ");
						sb->AppendDate(&dt);
					}
					if (headerFlags & 8)
					{
						if (headerFlags & 1)
						{
							dt.SetUnixTimestamp(ReadUInt32(packPtr));
							packPtr += 4;
						}
						else
						{
							dt.SetValueFILETIME((void*)packPtr);
							packPtr += 8;
						}
						sb->Append((const UTF8Char*)", atime = ");
						sb->AppendDate(&dt);
					}
				}
				else if (iVal == 4)
				{
					sb->Append((const UTF8Char*)" (File version)");
				}
				else if (iVal == 5)
				{
					sb->Append((const UTF8Char*)" (Redirection)");
				}
				else if (iVal == 6)
				{
					sb->Append((const UTF8Char*)" (Unix owner)");
				}
				else if (iVal == 7)
				{
					sb->Append((const UTF8Char*)" (Service data)");
				}
				packPtr = nextPtr;
			}
		}
	}
	else if (pack->headerType == 5)
	{
		packPtr = ReadVInt(packPtr, &headerFlags);
		sb->Append((const UTF8Char*)"\r\nEnd of archive flags = 0x");
		sb->AppendHex16((UInt16)headerFlags);
	}

	/*	sb->Append((const UTF8Char*)"\r\nMicroSec Per Frame = ");
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
	sb->AppendU32(ReadUInt32(&packBuff[36]));*/

	MemFree(packBuff);
	return true;
}

Bool IO::FileAnalyse::RAR5FileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::RAR5FileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::RAR5FileAnalyse::TrimPadding(const UTF8Char *outputFile)
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
