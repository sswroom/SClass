#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/Compress/Inflate.h"
#include "IO/MemoryStream.h"
#include "IO/FileAnalyse/PNGFileAnalyse.h"
#include "Media/ICCProfile.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

UInt32 __stdcall IO::FileAnalyse::PNGFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::PNGFileAnalyse *me = (IO::FileAnalyse::PNGFileAnalyse*)userObj;
	Int64 dataSize;
	Int64 ofst;
	Int32 lastSize;
	OSInt readSize;
	UInt8 tagHdr[12];
	IO::FileAnalyse::PNGFileAnalyse::PNGTag *tag;
	me->threadRunning = true;
	me->threadStarted = true;
	ofst = 4;
	dataSize = me->fd->GetDataSize();
	lastSize = 0;
	
	tag = 0;

	while (ofst < dataSize && !me->threadToStop)
	{
		readSize = me->fd->GetRealData(ofst, 12, tagHdr);
		if (readSize < 4)
			break;
		
		if (tag)
		{
			tag->crc = ReadMInt32(tagHdr);
		}
		if (readSize < 12)
			break;
		lastSize = ReadMInt32(&tagHdr[4]);

		tag = MemAlloc(IO::FileAnalyse::PNGFileAnalyse::PNGTag, 1);
		tag->ofst = ofst + 4;
		tag->size = lastSize + 12;
		tag->tagType = ReadInt32(&tagHdr[8]);
		tag->crc = 0;
		me->tagsMut->Lock();
		me->tags->Add(tag);
		me->tagsMut->Unlock();
		ofst += lastSize + 12;
	}
	
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::PNGFileAnalyse::PNGFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	NEW_CLASS(this->tags, Data::ArrayList<IO::FileAnalyse::PNGFileAnalyse::PNGTag*>());
	NEW_CLASS(this->tagsMut, Sync::Mutex());
	fd->GetRealData(0, 256, buff);
	if (buff[0] != 0x89 && buff[1] != 0x50 && buff[2] != 0x4e && buff[3] != 0x47 && buff[4] != 0x0d && buff[5] != 0x0a && buff[6] != 0x1a && buff[7] != 0x0a)
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

IO::FileAnalyse::PNGFileAnalyse::~PNGFileAnalyse()
{
	OSInt i;
	IO::FileAnalyse::PNGFileAnalyse::PNGTag *tag;
	if (this->threadRunning)
	{
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}

	SDEL_CLASS(this->fd);
	i = this->tags->GetCount();
	while (i-- > 0)
	{
		tag = this->tags->GetItem(i);
		MemFree(tag);
	}
	DEL_CLASS(this->tags);
	DEL_CLASS(this->tagsMut);
}

UOSInt IO::FileAnalyse::PNGFileAnalyse::GetFrameCount()
{
	return this->tags->GetCount();
}

Bool IO::FileAnalyse::PNGFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::PNGFileAnalyse::PNGTag *tag = this->tags->GetItem(index);
	if (tag == 0)
		return false;
	sb->AppendI64(tag->ofst);
	sb->Append((const UTF8Char*)": Type=");
	sb->AppendC((UTF8Char*)&tag->tagType, 4);
	sb->Append((const UTF8Char*)", size=");
	sb->AppendOSInt(tag->size);
	sb->Append((const UTF8Char*)", CRC=0x");
	sb->AppendHex32(tag->crc);
	return true;
}

Bool IO::FileAnalyse::PNGFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
{
	UInt8 *tagData;
	IO::FileAnalyse::PNGFileAnalyse::PNGTag *tag = this->tags->GetItem(index);
	if (tag == 0)
		return false;
	sb->Append((const UTF8Char*)"Tag");
	sb->AppendOSInt(index);
	sb->Append((const UTF8Char*)"\r\nTagType = ");
	sb->AppendC((UTF8Char*)&tag->tagType, 4);
	sb->Append((const UTF8Char*)"\r\nSize = ");
	sb->AppendOSInt(tag->size);
	sb->Append((const UTF8Char*)"\r\nCRC = 0x");
	sb->AppendHex32(tag->crc);
	if (tag->tagType == *(Int32*)"IHDR")
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nWidth = ");
		sb->AppendI32(ReadMInt32(&tagData[8]));
		sb->Append((const UTF8Char*)"\r\nHeight = ");
		sb->AppendI32(ReadMInt32(&tagData[12]));
		sb->Append((const UTF8Char*)"\r\nBit depth = ");
		sb->AppendU16(tagData[16]);
		sb->Append((const UTF8Char*)"\r\nColor type = ");
		sb->AppendU16(tagData[17]);
		sb->Append((const UTF8Char*)"\r\nCompression method = ");
		sb->AppendU16(tagData[18]);
		sb->Append((const UTF8Char*)"\r\nFilter method = ");
		sb->AppendU16(tagData[19]);
		sb->Append((const UTF8Char*)"\r\nInterlace method = ");
		sb->AppendU16(tagData[20]);

		MemFree(tagData);
	}
	else if (tag->tagType == *(Int32*)"gAMA")
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nGamma = ");
		sb->AppendI32(ReadMInt32(&tagData[8]));
		sb->Append((const UTF8Char*)" (");
		Text::SBAppendF64(sb, 100000.0 / ReadMInt32(&tagData[8]));
		sb->Append((const UTF8Char*)")");

		MemFree(tagData);
	}
	else if (tag->tagType == *(Int32*)"sRGB")
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nRendering intent = ");
		sb->AppendU16(tagData[8]);

		MemFree(tagData);
	}
	else if (tag->tagType == *(Int32*)"cHRM")
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nWhite Point x = ");
		Text::SBAppendF64(sb, ReadMInt32(&tagData[8]) / 100000.0);
		sb->Append((const UTF8Char*)"\r\nWhite Point y = ");
		Text::SBAppendF64(sb, ReadMInt32(&tagData[12]) / 100000.0);

		sb->Append((const UTF8Char*)"\r\nRed x = ");
		Text::SBAppendF64(sb, ReadMInt32(&tagData[16]) / 100000.0);
		sb->Append((const UTF8Char*)"\r\nRed y = ");
		Text::SBAppendF64(sb, ReadMInt32(&tagData[20]) / 100000.0);

		sb->Append((const UTF8Char*)"\r\nGreen x = ");
		Text::SBAppendF64(sb, ReadMInt32(&tagData[24]) / 100000.0);
		sb->Append((const UTF8Char*)"\r\nGreen y = ");
		Text::SBAppendF64(sb, ReadMInt32(&tagData[28]) / 100000.0);

		sb->Append((const UTF8Char*)"\r\nBlue x = ");
		Text::SBAppendF64(sb, ReadMInt32(&tagData[32]) / 100000.0);
		sb->Append((const UTF8Char*)"\r\nBlue y = ");
		Text::SBAppendF64(sb, ReadMInt32(&tagData[36]) / 100000.0);

		MemFree(tagData);
	}
	else if (tag->tagType == *(Int32*)"acTL")
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nNumber of frames = ");
		sb->AppendI32(ReadMInt32(&tagData[8]));
		sb->Append((const UTF8Char*)"\r\nNumber of time to loop = ");
		sb->AppendI32(ReadMInt32(&tagData[12]));

		MemFree(tagData);
	}
	else if (tag->tagType == *(Int32*)"fcTL")
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nSequence number = ");
		sb->AppendI32(ReadMInt32(&tagData[8]));
		sb->Append((const UTF8Char*)"\r\nWidth = ");
		sb->AppendI32(ReadMInt32(&tagData[12]));
		sb->Append((const UTF8Char*)"\r\nHeight = ");
		sb->AppendI32(ReadMInt32(&tagData[16]));
		sb->Append((const UTF8Char*)"\r\nX Offset = ");
		sb->AppendI32(ReadMInt32(&tagData[20]));
		sb->Append((const UTF8Char*)"\r\nY Offset = ");
		sb->AppendI32(ReadMInt32(&tagData[24]));
		sb->Append((const UTF8Char*)"\r\nDelay numerator = ");
		sb->AppendI16(ReadMInt16(&tagData[28]));
		sb->Append((const UTF8Char*)"\r\nDelay denominator = ");
		sb->AppendI16(ReadMInt16(&tagData[30]));
		sb->Append((const UTF8Char*)"\r\nDisposal operation = ");
		sb->AppendU16(tagData[32]);
		sb->Append((const UTF8Char*)"\r\nBlend operation = ");
		sb->AppendU16(tagData[33]);

		MemFree(tagData);
	}
	else if (tag->tagType == *(Int32*)"fdAT")
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nSequence number = ");
		sb->AppendI32(ReadMInt32(&tagData[8]));

		MemFree(tagData);
	}
	else if (tag->tagType == *(Int32*)"pHYs")
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nH Pixel per unit = ");
		sb->AppendI32(ReadMInt32(&tagData[8]));
		sb->Append((const UTF8Char*)"\r\nV Pixel per unit = ");
		sb->AppendI32(ReadMInt32(&tagData[12]));
		sb->Append((const UTF8Char*)"\r\nUnit type = ");
		sb->AppendU16(tagData[16]);

		MemFree(tagData);
	}
	else if (tag->tagType == *(Int32*)"iCCP")
	{
		OSInt i;
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		i = Text::StrCharCnt((Char*)&tagData[8]) + 9;
		sb->Append((const UTF8Char*)"\r\nProfile name = ");
		sb->Append((UTF8Char*)&tagData[8]);
		sb->Append((const UTF8Char*)"\r\nCompression Method = ");
		sb->AppendU16(tagData[i]);

		if (tagData[i] == 0)
		{
			sb->Append((const UTF8Char*)"\r\nzlib Compression Method = ");
			sb->AppendU16(tagData[i + 1]);
			sb->Append((const UTF8Char*)"\r\nAdditional flags = ");
			sb->AppendU16(tagData[i + 2]);
			sb->Append((const UTF8Char*)"\r\nCheck value = 0x");
			sb->AppendHex32(ReadMInt32(&tagData[tag->size - 8]));

			IO::MemoryStream *mstm;
			IO::IStreamData *stmData = this->fd->GetPartialData(tag->ofst + i + 3, tag->size - i - 12);
			Data::Compress::Inflate comp;
			NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"IO.FileAnalyse.PNGFileAnalyse"));
			if (!comp.Decompress(mstm, stmData))
			{
				UOSInt iccSize;
				UInt8 *iccBuff = mstm->GetBuff(&iccSize);
				Media::ICCProfile *icc = Media::ICCProfile::Parse(iccBuff, iccSize);
				if (icc)
				{
					sb->Append((const UTF8Char*)"\r\n\r\n");
					icc->ToString(sb);
					DEL_CLASS(icc);
				}
			}
			DEL_CLASS(mstm);
			DEL_CLASS(stmData);
		}
		MemFree(tagData);
	}
	else if (tag->tagType == *(Int32*)"PLTE")
	{
		if (tag->size <= 768 + 12 && (tag->size % 3) == 0)
		{
			OSInt i;
			OSInt j;
			tagData = MemAlloc(UInt8, tag->size - 12);
			this->fd->GetRealData(tag->ofst + 8, tag->size - 12, tagData);
			i = 0;
			j = 0;
			while (j < tag->size - 12)
			{
				sb->Append((const UTF8Char*)"\r\nEntry ");
				sb->AppendOSInt(i);
				sb->AppendChar(' ', 1);
				sb->AppendChar('R', 1);
				sb->AppendU16(tagData[j + 0]);
				sb->AppendChar('G', 1);
				sb->AppendU16(tagData[j + 1]);
				sb->AppendChar('B', 1);
				sb->AppendU16(tagData[j + 2]);
				i++;
				j += 3;
			}
			MemFree(tagData);
		}
	}
	return true;
}

Bool IO::FileAnalyse::PNGFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::PNGFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::PNGFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
	return false;
}
