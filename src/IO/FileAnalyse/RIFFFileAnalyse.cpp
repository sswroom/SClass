#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/RIFFFileAnalyse.h"
#include "Media/EXIFData.h"
#include "Media/ICCProfile.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

void IO::FileAnalyse::RIFFFileAnalyse::ParseRange(UOSInt lev, UInt64 ofst, UInt64 size)
{
	UInt8 buff[12];
	UInt64 endOfst = ofst + size;
	UInt32 sz;
	PackInfo *pack;

	while (ofst <= (endOfst - 12) && !this->threadToStop)
	{
		if (this->pauseParsing)
		{
			Sync::SimpleThread::Sleep(100);
		}
		else
		{
			this->fd->GetRealData(ofst, 12, BYTEARR(buff));
			sz = ReadUInt32(&buff[4]);
			if (ofst + sz + 8 > endOfst)
			{
				return;
			}
			pack = MemAlloc(PackInfo, 1);
			pack->lev = lev;
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
			if (this->maxLev < lev)
			{
				this->maxLev = lev;
			}
			this->packs.Add(pack);
			
			if (pack->subPackType != 0)
			{
				ParseRange(lev + 1, ofst + 12, sz - 4);
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
	me->ParseRange(0, 0, me->fd->GetDataSize());
	me->threadRunning = false;
	return 0;
}

UOSInt IO::FileAnalyse::RIFFFileAnalyse::GetFrameIndex(UOSInt lev, UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs.GetCount() - 1;
	OSInt k;
	PackInfo *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItem((UOSInt)k);
		if (ofst < pack->fileOfst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->fileOfst + pack->packSize)
		{
			i = k + 1;
		}
		else if (pack->lev < lev)
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

IO::FileAnalyse::RIFFFileAnalyse::RIFFFileAnalyse(NotNullPtr<IO::StreamData> fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	this->maxLev = 0;
	fd->GetRealData(0, 256, BYTEARR(buff));
	if (ReadNInt32(buff) != *(Int32*)"RIFF")
	{
		return;
	}
	UInt32 size = ReadUInt32(&buff[4]);
	if (size + 8 > fd->GetDataSize())
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	Sync::ThreadUtil::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

IO::FileAnalyse::RIFFFileAnalyse::~RIFFFileAnalyse()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}
	SDEL_CLASS(this->fd);
	LIST_FREE_FUNC(&this->packs, MemFree);
}

Text::CString IO::FileAnalyse::RIFFFileAnalyse::GetFormatName()
{
	return CSTR("RIFF");
}

UOSInt IO::FileAnalyse::RIFFFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::RIFFFileAnalyse::GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	PackInfo *pack;
	UInt8 buff[5];
	pack = this->packs.GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	*(Int32*)buff = pack->packType;
	buff[4] = 0;
	sb->AppendSlow((UTF8Char*)buff);
	if (pack->subPackType != 0)
	{
		sb->AppendC(UTF8STRC(", SubType="));
		*(Int32*)buff = pack->subPackType;
		buff[4] = 0;
		sb->AppendSlow((UTF8Char*)buff);
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUOSInt(pack->packSize);
	return true;
}

Bool IO::FileAnalyse::RIFFFileAnalyse::GetFrameDetail(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	PackInfo *pack;
	UInt8 buff[5];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	pack = this->packs.GetItem(index);
	if (pack == 0)
		return false;

	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	*(Int32*)buff = pack->packType;
	buff[4] = 0;
	sb->AppendSlow((UTF8Char*)buff);
	if (pack->subPackType != 0)
	{
		sb->AppendC(UTF8STRC(", SubType="));
		*(Int32*)buff = pack->subPackType;
		buff[4] = 0;
		sb->AppendSlow((UTF8Char*)buff);
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUOSInt(pack->packSize);

	if (pack->packType == *(Int32*)"avih")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->AppendC(UTF8STRC("\r\nMicroSec Per Frame = "));
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
		sb->AppendU32(ReadUInt32(&packBuff[36]));
	}
	else if (pack->packType == *(Int32*)"strh")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->AppendC(UTF8STRC("\r\nfccType = "));
		*(Int32*)buff = *(Int32*)&packBuff[0];
		sb->AppendSlow((UTF8Char*)buff);
		sb->AppendC(UTF8STRC("\r\nfccHandler = "));
		if (*(Int32*)&packBuff[4] == 0)
		{
			sb->AppendC(UTF8STRC("0"));
		}
		else
		{
			*(Int32*)buff = *(Int32*)&packBuff[4];
			sb->AppendSlow((UTF8Char*)buff);
		}
		sb->AppendC(UTF8STRC("\r\nFlags = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[8]));
		sb->AppendC(UTF8STRC("\r\nPriority = "));
		sb->AppendU32(ReadUInt32(&packBuff[12]));
		sb->AppendC(UTF8STRC("\r\nInitial Frames = "));
		sb->AppendU32(ReadUInt32(&packBuff[16]));
		sb->AppendC(UTF8STRC("\r\nScale = "));
		sb->AppendU32(ReadUInt32(&packBuff[20]));
		sb->AppendC(UTF8STRC("\r\nRate = "));
		sb->AppendU32(ReadUInt32(&packBuff[24]));
		sb->AppendC(UTF8STRC("\r\nStart = "));
		sb->AppendU32(ReadUInt32(&packBuff[28]));
		sb->AppendC(UTF8STRC("\r\nLength = "));
		sb->AppendU32(ReadUInt32(&packBuff[32]));
		sb->AppendC(UTF8STRC("\r\nSuggested Buffer Size = "));
		sb->AppendU32(ReadUInt32(&packBuff[36]));
		sb->AppendC(UTF8STRC("\r\nQuality = "));
		sb->AppendU32(ReadUInt32(&packBuff[40]));
		sb->AppendC(UTF8STRC("\r\nSample Size = "));
		sb->AppendU32(ReadUInt32(&packBuff[44]));
	}
	else if (pack->packType == *(Int32*)"strf")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		if (ReadUInt32(&packBuff[0]) == pack->packSize - 8)
		{
			sb->AppendC(UTF8STRC("\r\nbiSize = "));
			sb->AppendU32(ReadUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nbiWidth = "));
			sb->AppendU32(ReadUInt32(&packBuff[4]));
			sb->AppendC(UTF8STRC("\r\nbiHeight = "));
			sb->AppendU32(ReadUInt32(&packBuff[8]));
			sb->AppendC(UTF8STRC("\r\nbiPlanes = "));
			sb->AppendU16(ReadUInt16(&packBuff[12]));
			sb->AppendC(UTF8STRC("\r\nbiBitCount = "));
			sb->AppendU16(ReadUInt16(&packBuff[14]));
			sb->AppendC(UTF8STRC("\r\nbiCompression = "));
			if (ReadUInt32(&packBuff[16]) == 0)
			{
				sb->AppendC(UTF8STRC("0"));
			}
			else
			{
				*(Int32*)buff = *(Int32*)&packBuff[16];
				sb->AppendSlow((UTF8Char*)buff);
			}
			sb->AppendC(UTF8STRC("\r\nbiSizeImage = "));
			sb->AppendU32(ReadUInt32(&packBuff[20]));
			sb->AppendC(UTF8STRC("\r\nbiXPelsPerMeter = "));
			sb->AppendU32(ReadUInt32(&packBuff[24]));
			sb->AppendC(UTF8STRC("\r\nbiYPelsPerMeter = "));
			sb->AppendU32(ReadUInt32(&packBuff[28]));
			sb->AppendC(UTF8STRC("\r\nbiClrUsed = "));
			sb->AppendU32(ReadUInt32(&packBuff[32]));
			sb->AppendC(UTF8STRC("\r\nbiClrImportant = "));
			sb->AppendU32(ReadUInt32(&packBuff[36]));
		}
		else
		{
			sb->AppendC(UTF8STRC("\r\nwFormatTag = 0x"));
			sb->AppendHex16(ReadUInt16(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nnChannels = "));
			sb->AppendU16(ReadUInt16(&packBuff[2]));
			sb->AppendC(UTF8STRC("\r\nnSamplesPerSecond = "));
			sb->AppendU32(ReadUInt32(&packBuff[4]));
			sb->AppendC(UTF8STRC("\r\nnAvgBytesPerSec = "));
			sb->AppendU32(ReadUInt32(&packBuff[8]));
			sb->AppendC(UTF8STRC("\r\nnBlockAlign = "));
			sb->AppendU16(ReadUInt16(&packBuff[12]));
			sb->AppendC(UTF8STRC("\r\nwBitsPerSample = "));
			sb->AppendU16(ReadUInt16(&packBuff[14]));
		}
	}
	else if (pack->packType == *(Int32*)"JUNK")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendHexBuff(packBuff, ' ', Text::LineBreakType::CRLF);
	}
	else if (pack->packType == *(Int32*)"dmlh")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->AppendC(UTF8STRC("\r\nFrame Count = "));
		sb->AppendU32(ReadUInt32(&packBuff[0]));
	}
	else if (pack->packType == *(Int32*)"00db" || pack->packType == *(Int32*)"00dc" || pack->packType == *(Int32*)"01wb" || pack->packType == *(Int32*)"02wb")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendHexBuff(packBuff, ' ', Text::LineBreakType::CRLF);
	}
	else if (pack->packType == *(Int32*)"idx1")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		i = 0;
		j = (pack->packSize - 8) >> 4;
		k = 0;
		sb->AppendC(UTF8STRC("\r\nIndex List:"));
		while (i < j)
		{
			*(Int32*)buff = *(Int32*)&packBuff[k];
			sb->AppendC(UTF8STRC("\r\nType="));
			sb->AppendSlow((UTF8Char*)buff);
			sb->AppendC(UTF8STRC(", Offset="));
			sb->AppendU32(ReadUInt32(&packBuff[k + 8]));
			sb->AppendC(UTF8STRC(", Size="));
			sb->AppendU32(ReadUInt32(&packBuff[k + 12]));
			if (packBuff[k + 4] != 0)
			{
				sb->AppendC(UTF8STRC(" (key)"));
			}
			k += 16;
			i++;
		}
	}
	else if (pack->packType == *(Int32*)"anih")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->AppendC(UTF8STRC("\r\ncbSize = "));
		sb->AppendU32(ReadUInt32(&packBuff[0]));
		sb->AppendC(UTF8STRC("\r\nNumber of Frames = "));
		sb->AppendU32(ReadUInt32(&packBuff[4]));
		sb->AppendC(UTF8STRC("\r\nNumber of Steps = "));
		sb->AppendU32(ReadUInt32(&packBuff[8]));
		sb->AppendC(UTF8STRC("\r\nWidth"));
		sb->AppendU32(ReadUInt32(&packBuff[12]));
		sb->AppendC(UTF8STRC("\r\nHeight = "));
		sb->AppendU32(ReadUInt32(&packBuff[16]));
		sb->AppendC(UTF8STRC("\r\nBits per Pixel = "));
		sb->AppendU32(ReadUInt32(&packBuff[20]));
		sb->AppendC(UTF8STRC("\r\nNumber of color planes = "));
		sb->AppendU32(ReadUInt32(&packBuff[24]));
		sb->AppendC(UTF8STRC("\r\nDefault frame display rate = "));
		sb->AppendU32(ReadUInt32(&packBuff[28]));
		sb->AppendC(UTF8STRC("\r\nAttributes Flags = 0x"));
		sb->AppendHex32(ReadUInt32(&packBuff[32]));
	}
	else if (pack->packType == *(Int32*)"icon")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		sb->AppendC(UTF8STRC("\r\nFile Header = "));
		sb->AppendU16(ReadUInt16(&packBuff[0]));
		sb->AppendC(UTF8STRC("\r\nFile Format = "));
		sb->AppendU16(ReadUInt16(&packBuff[2]));
		sb->AppendC(UTF8STRC("\r\nNumber of Images = "));
		sb->AppendU16(ReadUInt16(&packBuff[4]));
		OSInt i = 0;
		OSInt j = ReadUInt16(&packBuff[4]);
		while (i < j)
		{
			sb->AppendC(UTF8STRC("\r\nImage "));
			sb->AppendOSInt(i);
			sb->AppendC(UTF8STRC(":"));

			sb->AppendC(UTF8STRC("\r\nWidth = "));
			sb->AppendU16(packBuff[6 + (i << 4)]);
			sb->AppendC(UTF8STRC("\r\nHeight = "));
			sb->AppendU16(packBuff[7 + (i << 4)]);
			sb->AppendC(UTF8STRC("\r\nNumber of Color = "));
			sb->AppendU16(packBuff[8 + (i << 4)]);
			sb->AppendC(UTF8STRC("\r\nReserved = "));
			sb->AppendU16(packBuff[9 + (i << 4)]);
			sb->AppendC(UTF8STRC("\r\nHotSpot X = "));
			sb->AppendU16(ReadUInt16(&packBuff[10 + (i << 4)]));
			sb->AppendC(UTF8STRC("\r\nHotSpot Y = "));
			sb->AppendU16(ReadUInt16(&packBuff[12 + (i << 4)]));
			sb->AppendC(UTF8STRC("\r\nImage Size = "));
			sb->AppendU32(ReadUInt32(&packBuff[14 + (i << 4)]));
			sb->AppendC(UTF8STRC("\r\nOffset = "));
			sb->AppendU32(ReadUInt32(&packBuff[18 + (i << 4)]));
			
			i++;
		}
	}
	return true;
}

UOSInt IO::FileAnalyse::RIFFFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	UOSInt ret;
	UOSInt i = this->maxLev;
	while (true)
	{
		ret = this->GetFrameIndex(i, ofst);
		if (ret != INVALID_INDEX)
		{
			return ret;
		}
		if (i == 0)
		{
			return INVALID_INDEX;
		}
		i--;
	}
}

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::RIFFFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail *frame;
	PackInfo *pack;
	UInt8 buff[5];
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	NotNullPtr<IO::StreamData> fd;
	pack = this->packs.GetItem(index);
	if (pack == 0 || !fd.Set(this->fd))
		return 0;

	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, (UInt32)pack->packSize));
	frame->AddStrS(0, 4, CSTR("Type"), (const UTF8Char*)&pack->packType);
	frame->AddUInt(4, 4, CSTR("Size"), pack->packSize - 8);
	if (pack->subPackType != 0)
	{
		frame->AddStrS(8, 4, CSTR("SubType"), (const UTF8Char*)&pack->subPackType);
	}

	if (pack->packType == *(Int32*)"avih")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);
		frame->AddUInt(8, 4, CSTR("MicroSec Per Frame"), ReadUInt32(&packBuff[0]));
		frame->AddUInt(12, 4, CSTR("Max Bytes Per Second"), ReadUInt32(&packBuff[4]));
		frame->AddUInt(16, 4, CSTR("Padding Granularity"), ReadUInt32(&packBuff[8]));
		frame->AddHex32(20, CSTR("Flags"), ReadUInt32(&packBuff[12]));
		frame->AddUInt(24, 4, CSTR("Total Frames"), ReadUInt32(&packBuff[16]));
		frame->AddUInt(28, 4, CSTR("Initial Frames"), ReadUInt32(&packBuff[20]));
		frame->AddUInt(32, 4, CSTR("Stream Count"), ReadUInt32(&packBuff[24]));
		frame->AddUInt(36, 4, CSTR("Suggested Buffer Size"), ReadUInt32(&packBuff[28]));
		frame->AddUInt(40, 4, CSTR("Width"), ReadUInt32(&packBuff[32]));
		frame->AddUInt(44, 4, CSTR("Height"), ReadUInt32(&packBuff[36]));
	}
	else if (pack->packType == *(Int32*)"strh")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);
		frame->AddStrS(8, 4, CSTR("fccType"), &packBuff[0]);
		if (*(Int32*)&packBuff[4] == 0)
		{
			frame->AddUInt(12, 4, CSTR("fccHandler"), 0);
		}
		else
		{
			frame->AddStrS(12, 4, CSTR("fccHandler"), &packBuff[4]);
		}
		frame->AddHex32(16, CSTR("Flags"), ReadUInt32(&packBuff[8]));
		frame->AddUInt(20, 4, CSTR("Priority"), ReadUInt32(&packBuff[12]));
		frame->AddUInt(24, 4, CSTR("Initial Frames"), ReadUInt32(&packBuff[16]));
		frame->AddUInt(28, 4, CSTR("Scale"), ReadUInt32(&packBuff[20]));
		frame->AddUInt(32, 4, CSTR("Rate"), ReadUInt32(&packBuff[24]));
		frame->AddUInt(36, 4, CSTR("Start"), ReadUInt32(&packBuff[28]));
		frame->AddUInt(40, 4, CSTR("Length"), ReadUInt32(&packBuff[32]));
		frame->AddUInt(44, 4, CSTR("Suggested Buffer Size"), ReadUInt32(&packBuff[36]));
		frame->AddUInt(48, 4, CSTR("Quality"), ReadUInt32(&packBuff[40]));
		frame->AddUInt(52, 4, CSTR("Sample Size"), ReadUInt32(&packBuff[44]));
	}
	else if (pack->packType == *(Int32*)"strf")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		if (ReadUInt32(&packBuff[0]) == pack->packSize - 8)
		{
			frame->AddUInt(8, 4, CSTR("biSize"), ReadUInt32(&packBuff[0]));
			frame->AddUInt(12, 4, CSTR("biWidth"), ReadUInt32(&packBuff[4]));
			frame->AddUInt(16, 4, CSTR("biHeight"), ReadUInt32(&packBuff[8]));
			frame->AddUInt(20, 2, CSTR("biPlanes"), ReadUInt16(&packBuff[12]));
			frame->AddUInt(22, 2, CSTR("biBitCount"), ReadUInt16(&packBuff[14]));
			if (ReadUInt32(&packBuff[16]) == 0)
			{
				frame->AddUInt(24, 4, CSTR("biCompression"), 0);
			}
			else
			{
				frame->AddStrS(24, 4, CSTR("fccHandler"), &packBuff[16]);
			}
			frame->AddUInt(28, 4, CSTR("biSizeImage"), ReadUInt32(&packBuff[20]));
			frame->AddUInt(32, 4, CSTR("biXPelsPerMeter"), ReadUInt32(&packBuff[24]));
			frame->AddUInt(36, 4, CSTR("biYPelsPerMeter"), ReadUInt32(&packBuff[28]));
			frame->AddUInt(40, 4, CSTR("biClrUsed"), ReadUInt32(&packBuff[32]));
			frame->AddUInt(44, 4, CSTR("Suggested biClrImportant Size"), ReadUInt32(&packBuff[36]));
		}
		else
		{
			frame->AddHex16(8, CSTR("wFormatTag"), ReadUInt16(&packBuff[0]));
			frame->AddUInt(10, 2, CSTR("nChannels"), ReadUInt16(&packBuff[2]));
			frame->AddUInt(12, 4, CSTR("nSamplesPerSecond"), ReadUInt32(&packBuff[4]));
			frame->AddUInt(16, 4, CSTR("nAvgBytesPerSec"), ReadUInt32(&packBuff[8]));
			frame->AddUInt(20, 2, CSTR("nBlockAlign"), ReadUInt16(&packBuff[12]));
			frame->AddUInt(22, 2, CSTR("wBitsPerSample"), ReadUInt16(&packBuff[14]));
		}
	}
	else if (pack->packType == *(Int32*)"JUNK")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);
		frame->AddHexBuff(8, CSTR("Unused"), packBuff, true);
	}
	else if (pack->packType == *(Int32*)"dmlh")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);
		frame->AddUInt(8, 4, CSTR("Frame Count"), ReadUInt32(&packBuff[0]));
	}
	else if (pack->packType == *(Int32*)"00db" || pack->packType == *(Int32*)"00dc" || pack->packType == *(Int32*)"01wb" || pack->packType == *(Int32*)"02wb")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);
		frame->AddHexBuff(8, CSTR("Data"), packBuff, true);
	}
	else if (pack->packType == *(Int32*)"idx1")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);

		i = 0;
		j = (pack->packSize - 8) >> 4;
		k = 0;
		frame->AddText(8, CSTR("Index List:"));
		while (i < j)
		{
			frame->AddStrS(k + 8, 4, CSTR("Type"), buff);
			frame->AddUInt(k + 12, 4, CSTR("IsKey"), ReadUInt32(&packBuff[k + 4]));
			frame->AddUInt(k + 16, 4, CSTR("Offset"), ReadUInt32(&packBuff[k + 8]));
			frame->AddUInt(k + 20, 4, CSTR("Size"), ReadUInt32(&packBuff[k + 12]));
			k += 16;
			i++;
		}
	}
	else if (pack->packType == *(Int32*)"anih")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);
		frame->AddUInt(8, 4, CSTR("cbSize"), ReadUInt32(&packBuff[0]));
		frame->AddUInt(12, 4, CSTR("Number of Frames"), ReadUInt32(&packBuff[4]));
		frame->AddUInt(16, 4, CSTR("Number of Steps"), ReadUInt32(&packBuff[8]));
		frame->AddUInt(20, 4, CSTR("Width"), ReadUInt32(&packBuff[12]));
		frame->AddUInt(24, 4, CSTR("Height"), ReadUInt32(&packBuff[16]));
		frame->AddUInt(28, 4, CSTR("Bits per Pixel"), ReadUInt32(&packBuff[20]));
		frame->AddUInt(32, 4, CSTR("Number of color planes"), ReadUInt32(&packBuff[24]));
		frame->AddUInt(36, 4, CSTR("Default frame display rate"), ReadUInt32(&packBuff[28]));
		frame->AddHex32(40, CSTR("Attributes Flags"), ReadUInt32(&packBuff[32]));
	}
	else if (pack->packType == *(Int32*)"icon")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);
		frame->AddUInt(8, 2, CSTR("File Header"), ReadUInt32(&packBuff[0]));
		frame->AddUInt(10, 2, CSTR("File Format"), ReadUInt32(&packBuff[2]));
		frame->AddUInt(12, 2, CSTR("Number of Images"), ReadUInt32(&packBuff[4]));

		UOSInt i = 0;
		UOSInt j = ReadUInt16(&packBuff[4]);
		while (i < j)
		{
			sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Image ")), i), UTF8STRC(":"));
			frame->AddText(14 + (i << 4), CSTRP(sbuff, sptr));
			frame->AddUInt(14 + (i << 4), 1, CSTR("Width"), packBuff[6 + (i << 4)]);
			frame->AddUInt(15 + (i << 4), 1, CSTR("Height"), packBuff[7 + (i << 4)]);
			frame->AddUInt(16 + (i << 4), 1, CSTR("Number of Color"), packBuff[8 + (i << 4)]);
			frame->AddUInt(17 + (i << 4), 1, CSTR("Reserved"), packBuff[9 + (i << 4)]);
			frame->AddUInt(18 + (i << 4), 2, CSTR("HotSpot X"), ReadUInt16(&packBuff[10 + (i << 4)]));
			frame->AddUInt(20 + (i << 4), 2, CSTR("HotSpot Y"), ReadUInt16(&packBuff[12 + (i << 4)]));
			frame->AddUInt(22 + (i << 4), 4, CSTR("Image Size"), ReadUInt16(&packBuff[14 + (i << 4)]));
			frame->AddUInt(26 + (i << 4), 4, CSTR("Offset"), ReadUInt16(&packBuff[18 + (i << 4)]));			
			i++;
		}
	}
	else if (pack->packType == *(Int32*)"VP8X")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);
		frame->AddBool(8, CSTR("ICC profile"), (packBuff[0] & 0x20));
		frame->AddBool(8, CSTR("Alpha"), (packBuff[0] & 0x10));
		frame->AddBool(8, CSTR("Exif metadata"), (packBuff[0] & 0x8));
		frame->AddBool(8, CSTR("XMP metadata"), (packBuff[0] & 0x4));
		frame->AddBool(8, CSTR("Animation"), (packBuff[0] & 0x2));
		frame->AddUInt(9, 3, CSTR("Reserved"), ReadUInt24(&packBuff[1]));
		frame->AddUInt(12, 3, CSTR("Canvas Width Minus One"), ReadUInt24(&packBuff[4]));
		frame->AddUInt(15, 3, CSTR("Canvas Height Minus One"), ReadUInt24(&packBuff[7]));
	}
	else if (pack->packType == *(Int32*)"ICCP")
	{
		Data::ByteBuffer packBuff(pack->packSize - 8);
		this->fd->GetRealData(pack->fileOfst + 8, pack->packSize - 8, packBuff);
		Media::ICCProfile::ParseFrame(frame, 8, packBuff.Ptr(), pack->packSize - 8);
	}
	else if (pack->packType == *(Int32*)"EXIF")
	{
		Media::EXIFData::ParseEXIFFrame(frame, 8, fd, pack->fileOfst + 8);
	}
	else if (pack->subPackType != 0)
	{
		frame->AddSubframe(12, pack->packSize - 12);
	}
	return frame;
}

Bool IO::FileAnalyse::RIFFFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::RIFFFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::RIFFFileAnalyse::TrimPadding(Text::CString outputFile)
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
