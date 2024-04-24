#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/RAR5FileAnalyse.h"
#include "Text/StringBuilderUTF8.h"

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

Data::ByteArray IO::FileAnalyse::RAR5FileAnalyse::ReadVInt(Data::ByteArray buffPtr, UInt64 *val)
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

const UInt8 *IO::FileAnalyse::RAR5FileAnalyse::AddVInt(NotNullPtr<IO::FileAnalyse::FrameDetail> frame, UOSInt ofst, Text::CStringNN name, const UInt8 *buffPtr)
{
	UInt64 iVal;
	const UInt8 *nextPtr = ReadVInt(buffPtr, &iVal);
	frame->AddUInt64V(ofst, (UOSInt)(nextPtr - buffPtr), name, iVal);
	return nextPtr;
}

const UInt8 *IO::FileAnalyse::RAR5FileAnalyse::AddVInt(NotNullPtr<IO::FileAnalyse::FrameDetail> frame, UOSInt ofst, Text::CStringNN name, const UInt8 *buffPtr, OptOut<UInt64> val)
{
	UInt64 iVal;
	const UInt8 *nextPtr = ReadVInt(buffPtr, &iVal);
	frame->AddUInt64V(ofst, (UOSInt)(nextPtr - buffPtr), name, iVal);
	val.Set(iVal);
	return nextPtr;
}

Data::ByteArray IO::FileAnalyse::RAR5FileAnalyse::AddVInt(NotNullPtr<IO::FileAnalyse::FrameDetail> frame, UOSInt ofst, Text::CStringNN name, Data::ByteArray buffPtr)
{
	UInt64 iVal;
	Data::ByteArray nextPtr = ReadVInt(buffPtr, &iVal);
	frame->AddUInt64V(ofst, (UOSInt)(nextPtr - buffPtr), name, iVal);
	return nextPtr;
}

Data::ByteArray IO::FileAnalyse::RAR5FileAnalyse::AddVInt(NotNullPtr<IO::FileAnalyse::FrameDetail> frame, UOSInt ofst, Text::CStringNN name, Data::ByteArray buffPtr, OptOut<UInt64> val)
{
	UInt64 iVal;
	Data::ByteArray nextPtr = ReadVInt(buffPtr, &iVal);
	frame->AddUInt64V(ofst, (UOSInt)(nextPtr - buffPtr), name, iVal);
	val.Set(iVal);
	return nextPtr;
}

const UInt8 *IO::FileAnalyse::RAR5FileAnalyse::AddVHex(NotNullPtr<IO::FileAnalyse::FrameDetail> frame, UOSInt ofst, Text::CStringNN name, const UInt8 *buffPtr, OptOut<UInt64> val)
{
	UInt64 iVal;
	const UInt8 *nextPtr = ReadVInt(buffPtr, &iVal);
	frame->AddHex64V(ofst, (UOSInt)(nextPtr - buffPtr), name, iVal);
	val.Set(iVal);
	return nextPtr;
}

Data::ByteArray IO::FileAnalyse::RAR5FileAnalyse::AddVHex(NotNullPtr<IO::FileAnalyse::FrameDetail> frame, UOSInt ofst, Text::CStringNN name, Data::ByteArray buffPtr, OptOut<UInt64> val)
{
	UInt64 iVal;
	Data::ByteArray nextPtr = ReadVInt(buffPtr, &iVal);
	frame->AddHex64V(ofst, (UOSInt)(nextPtr - buffPtr), name, iVal);
	val.Set(iVal);
	return nextPtr;
}

void __stdcall IO::FileAnalyse::RAR5FileAnalyse::ParseThread(NotNullPtr<Sync::Thread> thread)
{
	NotNullPtr<IO::FileAnalyse::RAR5FileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::RAR5FileAnalyse>();
	UInt8 buff[128];
	const UInt8 *buffPtr;
	UInt64 iVal;
	UInt64 headerFlags;
	UInt64 currOfst = 8;
	UInt64 endOfst = me->fd->GetDataSize();
	NN<IO::FileAnalyse::RAR5FileAnalyse::BlockInfo> block;
	while (!thread->IsStopping() && currOfst + 8 <= endOfst)
	{
		me->fd->GetRealData(currOfst, 128, BYTEARR(buff));
		buffPtr = buff + 4;
		buffPtr = ReadVInt(buffPtr, &iVal);
		block = MemAllocNN(IO::FileAnalyse::RAR5FileAnalyse::BlockInfo);
		block->fileOfst = currOfst;
		if (iVal >= 1048576 * 2)
		{
			MemFreeNN(block);
			break;
		}
		block->headerSize = (UInt32)(iVal + (UOSInt)(buffPtr - buff));
		buffPtr = ReadVInt(buffPtr, &iVal);
		if (iVal > 5)
		{
			MemFreeNN(block);
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
		me->packs.Add(block);
		currOfst += block->headerSize + block->dataSize;
	}
}

IO::FileAnalyse::RAR5FileAnalyse::RAR5FileAnalyse(NotNullPtr<IO::StreamData> fd) : thread(ParseThread, this, CSTR("RAR5FileAnalyse"))
{
	UInt8 buff[256];
	this->fd = 0;
	this->pauseParsing = false;
	fd->GetRealData(0, 256, BYTEARR(buff));
	if (ReadInt32(&buff[0]) != 0x21726152 || ReadInt32(&buff[4]) != 0x0001071A)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::RAR5FileAnalyse::~RAR5FileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
	this->packs.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::RAR5FileAnalyse::GetFormatName()
{
	return CSTR("RAR5");
}

UOSInt IO::FileAnalyse::RAR5FileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::RAR5FileAnalyse::GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::RAR5FileAnalyse::BlockInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->AppendU32(pack->headerType);
	sb->AppendC(UTF8STRC(", HeaderSize="));
	sb->AppendU32(pack->headerSize);
	sb->AppendC(UTF8STRC(", DataSize="));
	sb->AppendU64(pack->dataSize);
	return true;
}

Bool IO::FileAnalyse::RAR5FileAnalyse::GetFrameDetail(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::RAR5FileAnalyse::BlockInfo> pack;
	UInt64 iVal;
	UInt64 headerFlags;
	UInt64 extraSize;
	Data::ByteArray packPtr;
	Data::ByteArray extraEnd;
	Data::ByteArray packEnd;
	Data::ByteArray nextPtr;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;

	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(":"));
	Data::ByteBuffer packBuff(pack->headerSize);
	this->fd->GetRealData(pack->fileOfst, pack->headerSize, packBuff);

	sb->AppendC(UTF8STRC("\r\nBlock CRC = 0x"));
	sb->AppendHex32(ReadUInt32(&packBuff[0]));
	packPtr = packBuff + 4;
	packEnd = packBuff + pack->headerSize;
	packPtr = ReadVInt(packPtr, &iVal);
	sb->AppendC(UTF8STRC("\r\nHeader Size = "));
	sb->AppendU64(iVal);
	packPtr = ReadVInt(packPtr, &iVal);
	sb->AppendC(UTF8STRC("\r\nHeader Type = "));
	sb->AppendU64(iVal);
	switch (iVal)
	{
	case 1:
		sb->AppendC(UTF8STRC(" (Main archive header)"));
		break;
	case 2:
		sb->AppendC(UTF8STRC(" (File header)"));
		break;
	case 3:
		sb->AppendC(UTF8STRC(" (Service header)"));
		break;
	case 4:
		sb->AppendC(UTF8STRC(" (Archive encryption header)"));
		break;
	case 5:
		sb->AppendC(UTF8STRC(" (End of archive header)"));
		break;
	}
	packPtr = ReadVInt(packPtr, &headerFlags);
	sb->AppendC(UTF8STRC("\r\nHeader Flags = 0x"));
	sb->AppendHex16((UInt16)iVal);
	extraSize = 0;
	if (headerFlags & 1)
	{
		packPtr = ReadVInt(packPtr, &extraSize);
		sb->AppendC(UTF8STRC("\r\nExtra Area Size = "));
		sb->AppendU64(extraSize);
	}
	if (headerFlags & 2)
	{
		packPtr = ReadVInt(packPtr, &iVal);
		sb->AppendC(UTF8STRC("\r\nData Size = "));
		sb->AppendU64(iVal);
	}

	if (pack->headerType == 1)
	{
		packPtr = ReadVInt(packPtr, &iVal);
		sb->AppendC(UTF8STRC("\r\nArchive flags = 0x"));
		sb->AppendHex16((UInt16)iVal);
		if (iVal & 1)
		{
			sb->AppendC(UTF8STRC(" (Volume)"));
		}
		if (iVal & 2)
		{
			sb->AppendC(UTF8STRC(" (Volume number field is present)"));
		}
		if (iVal & 4)
		{
			sb->AppendC(UTF8STRC(" (Solid archive)"));
		}
		if (iVal & 8)
		{
			sb->AppendC(UTF8STRC(" (Recovery record is present)"));
		}
		if (iVal & 16)
		{
			sb->AppendC(UTF8STRC(" (Locked archive)"));
		}

		if (iVal & 2)
		{
			packPtr = ReadVInt(packPtr, &iVal);
			sb->AppendC(UTF8STRC("\r\nVolume number = "));
			sb->AppendU64(iVal);
		}
		extraEnd = packPtr + (UOSInt)extraSize;
		if (extraEnd <= packEnd)
		{
			while (packPtr < extraEnd)
			{
				packPtr = ReadVInt(packPtr, &extraSize);
				nextPtr = packPtr + (UOSInt)extraSize;
				if (nextPtr > extraEnd)
				{
					break;
				}
				packPtr = ReadVInt(packPtr, &iVal);
				sb->AppendC(UTF8STRC("\r\nExtra Rec Size = "));
				sb->AppendU64(extraSize);
				sb->AppendC(UTF8STRC(", Type = "));
				sb->AppendU64(iVal);
				if (iVal == 1)
				{
					sb->AppendC(UTF8STRC(" (Locator)"));
					packPtr = ReadVInt(packPtr, &iVal);
					sb->AppendC(UTF8STRC(", Flags = 0x"));
					sb->AppendHex16((UInt16)iVal);
					if (iVal & 1)
					{
						packPtr = ReadVInt(packPtr, &extraSize);
						sb->AppendC(UTF8STRC(", Quick open offset = 0x"));
						sb->AppendHex64V(extraSize);
					}
					if (iVal & 2)
					{
						packPtr = ReadVInt(packPtr, &extraSize);
						sb->AppendC(UTF8STRC(", Recovery record offset = 0x"));
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
		sb->AppendC(UTF8STRC("\r\nFile flags = 0x"));
		sb->AppendHex16((UInt16)headerFlags);
		if (headerFlags & 1)
		{
			sb->AppendC(UTF8STRC(" (Directory file system object)"));
		}
		if (headerFlags & 2)
		{
			sb->AppendC(UTF8STRC(" (Time field in Unix format is present)"));
		}
		if (headerFlags & 4)
		{
			sb->AppendC(UTF8STRC(" (CRC32 field is present)"));
		}
		if (headerFlags & 8)
		{
			sb->AppendC(UTF8STRC(" (Unpacked size is unknown)"));
		}

		packPtr = ReadVInt(packPtr, &iVal);
		sb->AppendC(UTF8STRC("\r\nUnpacked size = "));
		sb->AppendU64(iVal);
		packPtr = ReadVInt(packPtr, &iVal);
		sb->AppendC(UTF8STRC("\r\nAttributes = "));
		sb->AppendU64(iVal);
		if (headerFlags & 2)
		{
			sb->AppendC(UTF8STRC("\r\nmtime = "));
			sb->AppendU32(ReadUInt32(&packPtr[0]));
			packPtr += 4;
		}
		if (headerFlags & 4)
		{
			sb->AppendC(UTF8STRC("\r\nData CRC32 = 0x"));
			sb->AppendHex32(ReadUInt32(&packPtr[0]));
			packPtr += 4;
		}
		packPtr = ReadVInt(packPtr, &iVal);
		sb->AppendC(UTF8STRC("\r\nCompression version = "));
		sb->AppendU32((UInt8)(iVal & 0x3f));
		sb->AppendC(UTF8STRC("\r\nCompression Solid Flag = "));
		sb->AppendU32((UInt8)((iVal & 0x40) >> 6));
		sb->AppendC(UTF8STRC("\r\nCompression method = "));
		sb->AppendU32((UInt8)((iVal & 0x380) >> 7));
		sb->AppendC(UTF8STRC("\r\nCompression dir size = "));
		sb->AppendU32((UInt8)((iVal & 0x3c00) >> 10));
		packPtr = ReadVInt(packPtr, &iVal);
		sb->AppendC(UTF8STRC("\r\nHost OS = "));
		sb->AppendU64(iVal);
		packPtr = ReadVInt(packPtr, &iVal);
		sb->AppendC(UTF8STRC("\r\nName length = "));
		sb->AppendU64(iVal);
		sb->AppendC(UTF8STRC("\r\nName = "));
		sb->AppendC(packPtr.Ptr(), (UOSInt)iVal);
		packPtr += (UOSInt)iVal;

		extraEnd = packPtr + (UOSInt)extraSize;
		if (extraEnd <= packEnd)
		{
			while (packPtr < extraEnd)
			{
				packPtr = ReadVInt(packPtr, &extraSize);
				nextPtr = packPtr + (UOSInt)extraSize;
				if (nextPtr > extraEnd)
				{
					break;
				}
				packPtr = ReadVInt(packPtr, &iVal);
				sb->AppendC(UTF8STRC("\r\nExtra Rec Size = "));
				sb->AppendU64(extraSize);
				sb->AppendC(UTF8STRC(", Type = "));
				sb->AppendU64(iVal);
				if (iVal == 1)
				{
					sb->AppendC(UTF8STRC(" (File encryption)"));
				}
				else if (iVal == 2)
				{
					sb->AppendC(UTF8STRC(" (File hash)"));
				}
				else if (iVal == 3)
				{
					Data::Timestamp ts;
					sb->AppendC(UTF8STRC(" (File time)"));
					packPtr = ReadVInt(packPtr, &headerFlags);
					sb->AppendC(UTF8STRC(", Flags = 0x"));
					sb->AppendHex16((UInt16)headerFlags);
					if (headerFlags & 2)
					{
						if (headerFlags & 1)
						{
							ts = Data::Timestamp::FromEpochSec(ReadUInt32(&packPtr[0]), 0);
							packPtr += 4;
						}
						else
						{
							ts = Data::Timestamp::FromFILETIME((void*)packPtr.Ptr(), 0);
							packPtr += 8;
						}
						sb->AppendC(UTF8STRC(", mtime = "));
						sb->AppendTSNoZone(ts);
					}
					if (headerFlags & 4)
					{
						if (headerFlags & 1)
						{
							ts = Data::Timestamp::FromEpochSec(ReadUInt32(&packPtr[0]), 0);
							packPtr += 4;
						}
						else
						{
							ts = Data::Timestamp::FromFILETIME((void*)packPtr.Ptr(), 0);
							packPtr += 8;
						}
						sb->AppendC(UTF8STRC(", ctime = "));
						sb->AppendTSNoZone(ts);
					}
					if (headerFlags & 8)
					{
						if (headerFlags & 1)
						{
							ts = Data::Timestamp::FromEpochSec(ReadUInt32(&packPtr[0]), 0);
							packPtr += 4;
						}
						else
						{
							ts = Data::Timestamp::FromFILETIME((void*)packPtr.Ptr(), 0);
							packPtr += 8;
						}
						sb->AppendC(UTF8STRC(", atime = "));
						sb->AppendTSNoZone(ts);
					}
				}
				else if (iVal == 4)
				{
					sb->AppendC(UTF8STRC(" (File version)"));
				}
				else if (iVal == 5)
				{
					sb->AppendC(UTF8STRC(" (Redirection)"));
				}
				else if (iVal == 6)
				{
					sb->AppendC(UTF8STRC(" (Unix owner)"));
				}
				else if (iVal == 7)
				{
					sb->AppendC(UTF8STRC(" (Service data)"));
				}
				packPtr = nextPtr;
			}
		}
	}
	else if (pack->headerType == 5)
	{
		packPtr = ReadVInt(packPtr, &headerFlags);
		sb->AppendC(UTF8STRC("\r\nEnd of archive flags = 0x"));
		sb->AppendHex16((UInt16)headerFlags);
	}

	/*	sb->AppendC(UTF8STRC("\r\nMicroSec Per Frame = "));
	sb->AppendU32(ReadUInt32(&packBuff[0]));
	sb->AppendC(UTF8STRC("\r\nMax Bytes Per Second = "));
	sb->AppendU32(ReadUInt32(&packBuff[4]));
	sb->AppendC(UTF8STRC("\r\nPadding Granularity = "));
	sb->AppendU32(ReadUInt32(&packBuff[8]));
	sb->AppendC(UTF8STRC("\r\nFlags = 0x"));
	sb->AppendHex32(ReadUInt32(&packBuff[12]));
	sb->AppendC(UTF8STRC("\r\nTotal Frames = "));
	sb->AppendU32(ReadUInt32(&packBuff[16]));
	sb->AppendC(UTF8STRC("\r\nInitial Frames = "));
	sb->AppendU32(ReadUInt32(&packBuff[20]));
	sb->AppendC(UTF8STRC("\r\nStream Count = "));
	sb->AppendU32(ReadUInt32(&packBuff[24]));
	sb->AppendC(UTF8STRC("\r\nSuggested Buffer Size = "));
	sb->AppendU32(ReadUInt32(&packBuff[28]));
	sb->AppendC(UTF8STRC("\r\nWidth = "));
	sb->AppendU32(ReadUInt32(&packBuff[32]));
	sb->AppendC(UTF8STRC("\r\nHeight = "));
	sb->AppendU32(ReadUInt32(&packBuff[36]));*/
	return true;
}

UOSInt IO::FileAnalyse::RAR5FileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs.GetCount() - 1;
	OSInt k;
	NN<BlockInfo> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItemNoCheck((UOSInt)k);
		if (ofst < pack->fileOfst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->fileOfst + pack->headerSize + pack->dataSize)
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::RAR5FileAnalyse::GetFrameDetail(UOSInt index)
{
	NotNullPtr<IO::FileAnalyse::FrameDetail> frame;
	NN<IO::FileAnalyse::RAR5FileAnalyse::BlockInfo> pack;
	UInt64 iVal;
	UInt64 headerFlags;
	UInt64 extraSize;
	Text::CString vName;
	Data::ByteArray packPtr;
	Data::ByteArray extraEnd;
	Data::ByteArray packEnd;
	Data::ByteArray nextPtr;
	Data::ByteArray nextPtr2;
	if (!this->packs.GetItem(index).SetTo(pack))
		return 0;

	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, (pack->headerSize + pack->dataSize)));
	Data::ByteBuffer packBuff(pack->headerSize);
	this->fd->GetRealData(pack->fileOfst, pack->headerSize, packBuff);
	frame->AddHex32(0, CSTR("Block CRC"), ReadUInt32(&packBuff[0]));
	packPtr = packBuff + 4;
	packEnd = packBuff + pack->headerSize;
	packPtr = AddVInt(frame, (UOSInt)(packPtr - packBuff), CSTR("Header Size"), packPtr);
	nextPtr = ReadVInt(packPtr, &iVal);
	vName = CSTR_NULL;
	switch (iVal)
	{
	case 1:
		vName = CSTR("Main archive header");
		break;
	case 2:
		vName = CSTR("File header");
		break;
	case 3:
		vName = CSTR("Service header");
		break;
	case 4:
		vName = CSTR("Archive encryption header");
		break;
	case 5:
		vName = CSTR("End of archive header");
		break;
	}
	frame->AddUInt64Name((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr - packPtr), CSTR("Header Type"), iVal, vName);
	packPtr = nextPtr;
	nextPtr = ReadVInt(packPtr, &headerFlags);
	frame->AddHex64V((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr - packPtr), CSTR("Header Flags"), headerFlags);
	packPtr = nextPtr;
	extraSize = 0;
	if (headerFlags & 1)
	{
		packPtr = AddVInt(frame, (UOSInt)(packPtr - packBuff), CSTR("Extra Area Size"), packPtr, extraSize);
	}
	if (headerFlags & 2)
	{
		packPtr = AddVInt(frame, (UOSInt)(packPtr - packBuff), CSTR("Data Size"), packPtr);
	}

	if (pack->headerType == 1)
	{
		nextPtr = ReadVInt(packPtr, &iVal);
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("0x"));
		sb.AppendHex16((UInt16)iVal);
		if (iVal & 1)
		{
			sb.AppendC(UTF8STRC(" (Volume)"));
		}
		if (iVal & 2)
		{
			sb.AppendC(UTF8STRC(" (Volume number field is present)"));
		}
		if (iVal & 4)
		{
			sb.AppendC(UTF8STRC(" (Solid archive)"));
		}
		if (iVal & 8)
		{
			sb.AppendC(UTF8STRC(" (Recovery record is present)"));
		}
		if (iVal & 16)
		{
			sb.AppendC(UTF8STRC(" (Locked archive)"));
		}
		frame->AddField((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr - packPtr), CSTR("Archive flags"), sb.ToCString());
		packPtr = nextPtr;

		if (iVal & 2)
		{
			packPtr = AddVInt(frame, (UOSInt)(packPtr - packBuff), CSTR("Volume number"), packPtr);
		}
		extraEnd = packPtr + (UOSInt)extraSize;
		if (extraEnd <= packEnd)
		{
			while (packPtr < extraEnd)
			{
				nextPtr2 = ReadVInt(packPtr, &extraSize);
				nextPtr = nextPtr2 + (UOSInt)extraSize;
				if (nextPtr > extraEnd)
				{
					break;
				}
				frame->AddUInt64V((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr2 - packPtr), CSTR("Extra Rec Size"), extraSize);
				packPtr = nextPtr2;
				nextPtr2 = ReadVInt(packPtr, &iVal);
				if (iVal == 1)
				{
					frame->AddUInt64Name((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr2 - packPtr), CSTR("Type"), iVal, CSTR("Locator"));
					packPtr = nextPtr2;
					packPtr = AddVHex(frame, (UOSInt)(packPtr - packBuff), CSTR("Flags"), packPtr, iVal);
					if (iVal & 1)
					{
						packPtr = AddVHex(frame, (UOSInt)(packPtr - packBuff), CSTR("Quick open offset"), packPtr, iVal);
					}
					if (iVal & 2)
					{
						packPtr = AddVHex(frame, (UOSInt)(packPtr - packBuff), CSTR("Recovery record offset"), packPtr, iVal);
					}
				}
				else
				{
					frame->AddUInt64Name((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr2 - packPtr), CSTR("Type"), iVal, CSTR_NULL);
				}
				packPtr = nextPtr;
			}
		}
	}
	else if (pack->headerType == 2 || pack->headerType == 3)
	{
		nextPtr = ReadVInt(packPtr, &headerFlags);
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("0x"));
		sb.AppendHex16((UInt16)headerFlags);
		if (headerFlags & 1)
		{
			sb.AppendC(UTF8STRC(" (Directory file system object)"));
		}
		if (headerFlags & 2)
		{
			sb.AppendC(UTF8STRC(" (Time field in Unix format is present)"));
		}
		if (headerFlags & 4)
		{
			sb.AppendC(UTF8STRC(" (CRC32 field is present)"));
		}
		if (headerFlags & 8)
		{
			sb.AppendC(UTF8STRC(" (Unpacked size is unknown)"));
		}
		frame->AddField((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr - packPtr), CSTR("File flags"), sb.ToCString());
		packPtr = nextPtr;
		packPtr = AddVInt(frame, (UOSInt)(packPtr - packBuff), CSTR("Unpacked size"), packPtr, 0);
		packPtr = AddVInt(frame, (UOSInt)(packPtr - packBuff), CSTR("Attributes"), packPtr, 0);
		if (headerFlags & 2)
		{
			frame->AddUInt((UOSInt)(packPtr - packBuff), 4, CSTR("mtime"), ReadUInt32(&packPtr[0]));
			packPtr += 4;
		}
		if (headerFlags & 4)
		{
			frame->AddHex32((UOSInt)(packPtr - packBuff), CSTR("Data CRC32"), ReadUInt32(&packPtr[0]));
			packPtr += 4;
		}
		nextPtr = ReadVInt(packPtr, &iVal);
		frame->AddUInt64V((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr - packPtr), CSTR("Compression version"), iVal & 0x3f);
		frame->AddUInt64V((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr - packPtr), CSTR("Compression Solid Flag"), (iVal & 0x40) >> 6);
		frame->AddUInt64V((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr - packPtr), CSTR("Compression method"), (iVal & 0x380) >> 7);
		frame->AddUInt64V((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr - packPtr), CSTR("Compression dir size"), (iVal & 0x3c00) >> 10);
		packPtr = nextPtr;
		packPtr = AddVInt(frame, (UOSInt)(packPtr - packBuff), CSTR("Host OS"), packPtr);
		packPtr = AddVInt(frame, (UOSInt)(packPtr - packBuff), CSTR("Name length"), packPtr, iVal);
		frame->AddStrC((UOSInt)(packPtr - packBuff), (UOSInt)iVal, CSTR("Name"), packPtr.Ptr());
		packPtr += (UOSInt)iVal;

		extraEnd = packPtr + (UOSInt)extraSize;
		if (extraEnd <= packEnd)
		{
			while (packPtr < extraEnd)
			{
				nextPtr2 = ReadVInt(packPtr, &extraSize);
				nextPtr = nextPtr2 + (UOSInt)extraSize;
				if (nextPtr > extraEnd)
				{
					break;
				}
				frame->AddUInt64V((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr2 - packPtr), CSTR("Extra Rec Size"), extraSize);
				packPtr = nextPtr2;
				nextPtr2 = ReadVInt(packPtr, &iVal);
				vName = CSTR_NULL;
				switch (iVal)
				{
				case 1:
					vName = CSTR("File encryption");
					break;
				case 2:
					vName = CSTR("File hash");
					break;
				case 3:
					vName = CSTR("File time");
					break;
				case 4:
					vName = CSTR("File version");
					break;
				case 5:
					vName = CSTR("Redirection");
					break;
				case 6:
					vName = CSTR("Unix owner");
					break;
				case 7:
					vName = CSTR("Service data");
					break;
				}
				frame->AddUInt64Name((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr2 - packPtr), CSTR("Type"), iVal, vName);
				packPtr = nextPtr2;

				if (iVal == 3)
				{
					Data::DateTime dt;
					UTF8Char sbuff[64];
					UTF8Char *sptr;
					packPtr = AddVInt(frame, (UOSInt)(packPtr - packBuff), CSTR("Flags"), packPtr, headerFlags);
					if (headerFlags & 2)
					{
						if (headerFlags & 1)
						{
							dt.SetUnixTimestamp(ReadUInt32(&packPtr[0]));
							nextPtr2 = packPtr + 4;
						}
						else
						{
							dt.SetValueFILETIME((void*)packPtr.Ptr());
							nextPtr2 = packPtr + 8;
						}
						sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
						frame->AddField((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr2 - packPtr), CSTR("mtime"), CSTRP(sbuff, sptr));
						packPtr = nextPtr2;
					}
					if (headerFlags & 4)
					{
						if (headerFlags & 1)
						{
							dt.SetUnixTimestamp(ReadUInt32(&packPtr[0]));
							nextPtr2 = packPtr + 4;
						}
						else
						{
							dt.SetValueFILETIME((void*)packPtr.Ptr());
							nextPtr2 = packPtr + 8;
						}
						sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
						frame->AddField((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr2 - packPtr), CSTR("ctime"), CSTRP(sbuff, sptr));
						packPtr = nextPtr2;
					}
					if (headerFlags & 8)
					{
						if (headerFlags & 1)
						{
							dt.SetUnixTimestamp(ReadUInt32(&packPtr[0]));
							nextPtr2 = packPtr + 4;
						}
						else
						{
							dt.SetValueFILETIME((void*)packPtr.Ptr());
							nextPtr2 = packPtr + 8;
						}
						sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
						frame->AddField((UOSInt)(packPtr - packBuff), (UOSInt)(nextPtr2 - packPtr), CSTR("atime"), CSTRP(sbuff, sptr));
						packPtr = nextPtr2;
					}
				}
				packPtr = nextPtr;
			}
		}
	}
	else if (pack->headerType == 5)
	{
		packPtr = AddVHex(frame, (UOSInt)(packPtr - packBuff), CSTR("End of archive flags"), packPtr, headerFlags);
	}

	/*	sb->AppendC(UTF8STRC("\r\nMicroSec Per Frame = "));
	sb->AppendU32(ReadUInt32(&packBuff[0]));
	sb->AppendC(UTF8STRC("\r\nMax Bytes Per Second = "));
	sb->AppendU32(ReadUInt32(&packBuff[4]));
	sb->AppendC(UTF8STRC("\r\nPadding Granularity = "));
	sb->AppendU32(ReadUInt32(&packBuff[8]));
	sb->AppendC(UTF8STRC("\r\nFlags = 0x"));
	sb->AppendHex32(ReadUInt32(&packBuff[12]));
	sb->AppendC(UTF8STRC("\r\nTotal Frames = "));
	sb->AppendU32(ReadUInt32(&packBuff[16]));
	sb->AppendC(UTF8STRC("\r\nInitial Frames = "));
	sb->AppendU32(ReadUInt32(&packBuff[20]));
	sb->AppendC(UTF8STRC("\r\nStream Count = "));
	sb->AppendU32(ReadUInt32(&packBuff[24]));
	sb->AppendC(UTF8STRC("\r\nSuggested Buffer Size = "));
	sb->AppendU32(ReadUInt32(&packBuff[28]));
	sb->AppendC(UTF8STRC("\r\nWidth = "));
	sb->AppendU32(ReadUInt32(&packBuff[32]));
	sb->AppendC(UTF8STRC("\r\nHeight = "));
	sb->AppendU32(ReadUInt32(&packBuff[36]));*/
	return frame;
}

Bool IO::FileAnalyse::RAR5FileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::RAR5FileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::RAR5FileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
/*	UInt8 *readBuff;
	OSInt readSize;
	OSInt buffSize;
	OSInt j;
	OSInt frameSize;
	Int64 readOfst;
	Bool valid = true;
	IO::FileStream *dfs;
	NEW_CLASS(dfs, IO::FileStream(outputFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
