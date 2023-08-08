#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "Data/Compress/Inflate.h"
#include "IO/MemoryStream.h"
#include "IO/FileAnalyse/PNGFileAnalyse.h"
#include "Media/ICCProfile.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

UInt32 __stdcall IO::FileAnalyse::PNGFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::PNGFileAnalyse *me = (IO::FileAnalyse::PNGFileAnalyse*)userObj;
	UInt64 dataSize;
	UInt64 ofst;
	UInt32 lastSize;
	UOSInt readSize;
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
		readSize = me->fd->GetRealData(ofst, 12, BYTEARR(tagHdr));
		if (readSize < 4)
			break;
		
		if (tag)
		{
			tag->crc = ReadMUInt32(tagHdr);
		}
		if (readSize < 12)
			break;
		lastSize = ReadMUInt32(&tagHdr[4]);

		tag = MemAlloc(IO::FileAnalyse::PNGFileAnalyse::PNGTag, 1);
		tag->ofst = ofst + 4;
		tag->size = lastSize + 12;
		tag->tagType = ReadInt32(&tagHdr[8]);
		tag->crc = 0;
		me->tags.Add(tag);
		ofst += lastSize + 12;
	}
	
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::PNGFileAnalyse::PNGFileAnalyse(NotNullPtr<IO::StreamData> fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	fd->GetRealData(0, 256, BYTEARR(buff));
	if (buff[0] != 0x89 && buff[1] != 0x50 && buff[2] != 0x4e && buff[3] != 0x47 && buff[4] != 0x0d && buff[5] != 0x0a && buff[6] != 0x1a && buff[7] != 0x0a)
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

IO::FileAnalyse::PNGFileAnalyse::~PNGFileAnalyse()
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
	LIST_FREE_FUNC(&this->tags, MemFree);
}

Text::CStringNN IO::FileAnalyse::PNGFileAnalyse::GetFormatName()
{
	return CSTR("PNG");
}

UOSInt IO::FileAnalyse::PNGFileAnalyse::GetFrameCount()
{
	return this->tags.GetCount();
}

Bool IO::FileAnalyse::PNGFileAnalyse::GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	IO::FileAnalyse::PNGFileAnalyse::PNGTag *tag = this->tags.GetItem(index);
	if (tag == 0)
		return false;
	sb->AppendU64(tag->ofst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->AppendC((UTF8Char*)&tag->tagType, 4);
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUOSInt(tag->size);
	sb->AppendC(UTF8STRC(", CRC=0x"));
	sb->AppendHex32(tag->crc);
	return true;
}

Bool IO::FileAnalyse::PNGFileAnalyse::GetFrameDetail(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	IO::FileAnalyse::PNGFileAnalyse::PNGTag *tag = this->tags.GetItem(index);
	if (tag == 0)
		return false;
	sb->AppendC(UTF8STRC("Tag"));
	sb->AppendUOSInt(index);
	sb->AppendC(UTF8STRC("\r\nTagType = "));
	sb->AppendC((UTF8Char*)&tag->tagType, 4);
	sb->AppendC(UTF8STRC("\r\nSize = "));
	sb->AppendUOSInt(tag->size);
	sb->AppendC(UTF8STRC("\r\nCRC = 0x"));
	sb->AppendHex32(tag->crc);
	if (tag->tagType == *(Int32*)"IHDR")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nWidth = "));
		sb->AppendI32(ReadMInt32(&tagData[8]));
		sb->AppendC(UTF8STRC("\r\nHeight = "));
		sb->AppendI32(ReadMInt32(&tagData[12]));
		sb->AppendC(UTF8STRC("\r\nBit depth = "));
		sb->AppendU16(tagData[16]);
		sb->AppendC(UTF8STRC("\r\nColor type = "));
		sb->AppendU16(tagData[17]);
		sb->AppendC(UTF8STRC("\r\nCompression method = "));
		sb->AppendU16(tagData[18]);
		sb->AppendC(UTF8STRC("\r\nFilter method = "));
		sb->AppendU16(tagData[19]);
		sb->AppendC(UTF8STRC("\r\nInterlace method = "));
		sb->AppendU16(tagData[20]);
	}
	else if (tag->tagType == *(Int32*)"gAMA")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nGamma = "));
		sb->AppendI32(ReadMInt32(&tagData[8]));
		sb->AppendC(UTF8STRC(" ("));
		Text::SBAppendF64(sb, 100000.0 / ReadMInt32(&tagData[8]));
		sb->AppendC(UTF8STRC(")"));
	}
	else if (tag->tagType == *(Int32*)"sRGB")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nRendering intent = "));
		sb->AppendU16(tagData[8]);
	}
	else if (tag->tagType == *(Int32*)"cHRM")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nWhite Point x = "));
		Text::SBAppendF64(sb, ReadMInt32(&tagData[8]) / 100000.0);
		sb->AppendC(UTF8STRC("\r\nWhite Point y = "));
		Text::SBAppendF64(sb, ReadMInt32(&tagData[12]) / 100000.0);

		sb->AppendC(UTF8STRC("\r\nRed x = "));
		Text::SBAppendF64(sb, ReadMInt32(&tagData[16]) / 100000.0);
		sb->AppendC(UTF8STRC("\r\nRed y = "));
		Text::SBAppendF64(sb, ReadMInt32(&tagData[20]) / 100000.0);

		sb->AppendC(UTF8STRC("\r\nGreen x = "));
		Text::SBAppendF64(sb, ReadMInt32(&tagData[24]) / 100000.0);
		sb->AppendC(UTF8STRC("\r\nGreen y = "));
		Text::SBAppendF64(sb, ReadMInt32(&tagData[28]) / 100000.0);

		sb->AppendC(UTF8STRC("\r\nBlue x = "));
		Text::SBAppendF64(sb, ReadMInt32(&tagData[32]) / 100000.0);
		sb->AppendC(UTF8STRC("\r\nBlue y = "));
		Text::SBAppendF64(sb, ReadMInt32(&tagData[36]) / 100000.0);
	}
	else if (tag->tagType == *(Int32*)"acTL")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nNumber of frames = "));
		sb->AppendI32(ReadMInt32(&tagData[8]));
		sb->AppendC(UTF8STRC("\r\nNumber of time to loop = "));
		sb->AppendI32(ReadMInt32(&tagData[12]));
	}
	else if (tag->tagType == *(Int32*)"fcTL")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nSequence number = "));
		sb->AppendI32(ReadMInt32(&tagData[8]));
		sb->AppendC(UTF8STRC("\r\nWidth = "));
		sb->AppendI32(ReadMInt32(&tagData[12]));
		sb->AppendC(UTF8STRC("\r\nHeight = "));
		sb->AppendI32(ReadMInt32(&tagData[16]));
		sb->AppendC(UTF8STRC("\r\nX Offset = "));
		sb->AppendI32(ReadMInt32(&tagData[20]));
		sb->AppendC(UTF8STRC("\r\nY Offset = "));
		sb->AppendI32(ReadMInt32(&tagData[24]));
		sb->AppendC(UTF8STRC("\r\nDelay numerator = "));
		sb->AppendI16(ReadMInt16(&tagData[28]));
		sb->AppendC(UTF8STRC("\r\nDelay denominator = "));
		sb->AppendI16(ReadMInt16(&tagData[30]));
		sb->AppendC(UTF8STRC("\r\nDisposal operation = "));
		sb->AppendU16(tagData[32]);
		sb->AppendC(UTF8STRC("\r\nBlend operation = "));
		sb->AppendU16(tagData[33]);
	}
	else if (tag->tagType == *(Int32*)"fdAT")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nSequence number = "));
		sb->AppendI32(ReadMInt32(&tagData[8]));
	}
	else if (tag->tagType == *(Int32*)"pHYs")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nH Pixel per unit = "));
		sb->AppendI32(ReadMInt32(&tagData[8]));
		sb->AppendC(UTF8STRC("\r\nV Pixel per unit = "));
		sb->AppendI32(ReadMInt32(&tagData[12]));
		sb->AppendC(UTF8STRC("\r\nUnit type = "));
		sb->AppendU16(tagData[16]);
	}
	else if (tag->tagType == *(Int32*)"iCCP")
	{
		UOSInt i;
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		i = Text::StrCharCnt((Char*)&tagData[8]) + 9;
		sb->AppendC(UTF8STRC("\r\nProfile name = "));
		sb->AppendSlow((UTF8Char*)&tagData[8]);
		sb->AppendC(UTF8STRC("\r\nCompression Method = "));
		sb->AppendU16(tagData[i]);

		if (tagData[i] == 0)
		{
			sb->AppendC(UTF8STRC("\r\nzlib Compression Method = "));
			sb->AppendU16(tagData[i + 1]);
			sb->AppendC(UTF8STRC("\r\nAdditional flags = "));
			sb->AppendU16(tagData[i + 2]);
			sb->AppendC(UTF8STRC("\r\nCheck value = 0x"));
			sb->AppendHex32(ReadMUInt32(&tagData[tag->size - 8]));

			NotNullPtr<IO::StreamData> stmData = this->fd->GetPartialData(tag->ofst + i + 3, tag->size - i - 12);
			Data::Compress::Inflate comp(false);
			IO::MemoryStream mstm;
			if (!comp.Decompress(&mstm, stmData))
			{
				Media::ICCProfile *icc = Media::ICCProfile::Parse(mstm.GetArray());
				if (icc)
				{
					sb->AppendC(UTF8STRC("\r\n\r\n"));
					icc->ToString(sb);
					DEL_CLASS(icc);
				}
			}
			stmData.Delete();
		}
	}
	else if (tag->tagType == *(Int32*)"PLTE")
	{
		if (tag->size <= 768 + 12 && (tag->size % 3) == 0)
		{
			UOSInt i;
			UOSInt j;
			Data::ByteBuffer tagData(tag->size - 12);
			this->fd->GetRealData(tag->ofst + 8, tag->size - 12, tagData);
			i = 0;
			j = 0;
			while (j < tag->size - 12)
			{
				sb->AppendC(UTF8STRC("\r\nEntry "));
				sb->AppendUOSInt(i);
				sb->AppendUTF8Char(' ');
				sb->AppendUTF8Char('R');
				sb->AppendU16(tagData[j + 0]);
				sb->AppendUTF8Char('G');
				sb->AppendU16(tagData[j + 1]);
				sb->AppendUTF8Char('B');
				sb->AppendU16(tagData[j + 2]);
				i++;
				j += 3;
			}
		}
	}
	return true;
}

UOSInt IO::FileAnalyse::PNGFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->tags.GetCount() - 1;
	OSInt k;
	PNGTag *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->tags.GetItem((UOSInt)k);
		if (ofst < pack->ofst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->ofst + pack->size)
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

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::PNGFileAnalyse::GetFrameDetail(UOSInt index)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr2;
	UTF8Char *sptr;
	IO::FileAnalyse::FrameDetail *frame;
	IO::FileAnalyse::PNGFileAnalyse::PNGTag *tag = this->tags.GetItem(index);
	if (tag == 0)
		return 0;
	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(tag->ofst, (UInt32)tag->size));
	sptr = Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Tag"), index);
	frame->AddHeader(CSTRP(sbuff, sptr));
	frame->AddUInt(0, 4, CSTR("Size"), tag->size - 12);
	frame->AddStrC(4, 4, CSTR("TagType"), (const UTF8Char*)&tag->tagType);
	if (tag->tagType == *(Int32*)"IHDR")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(8, 4, CSTR("Width"), ReadMUInt32(&tagData[8]));
		frame->AddUInt(12, 4, CSTR("Height"), ReadMUInt32(&tagData[12]));
		frame->AddUInt(16, 1, CSTR("Bit depth"), tagData[16]);
		frame->AddUInt(17, 1, CSTR("Color type"), tagData[17]);
		frame->AddUInt(18, 1, CSTR("Compression method"), tagData[18]);
		frame->AddUInt(19, 1, CSTR("Filter method"), tagData[19]);
		frame->AddUInt(20, 1, CSTR("Interlace method"), tagData[20]);
	}
	else if (tag->tagType == *(Int32*)"gAMA")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddInt(8, 4, CSTR("Gamma"), ReadMInt32(&tagData[8]));
		frame->AddFloat(8, 4, CSTR("Gamma"), 100000.0 / ReadMInt32(&tagData[8]));
	}
	else if (tag->tagType == *(Int32*)"sRGB")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(8, 1, CSTR("Rendering intent"), tagData[8]);
	}
	else if (tag->tagType == *(Int32*)"cHRM")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddFloat(8, 4, CSTR("White Point x"), ReadMInt32(&tagData[8]) / 100000.0);
		frame->AddFloat(12, 4, CSTR("White Point y"), ReadMInt32(&tagData[12]) / 100000.0);
		frame->AddFloat(16, 4, CSTR("Red x"), ReadMInt32(&tagData[16]) / 100000.0);
		frame->AddFloat(20, 4, CSTR("Red y"), ReadMInt32(&tagData[20]) / 100000.0);
		frame->AddFloat(24, 4, CSTR("Green x"), ReadMInt32(&tagData[24]) / 100000.0);
		frame->AddFloat(28, 4, CSTR("Green y"), ReadMInt32(&tagData[28]) / 100000.0);
		frame->AddFloat(32, 4, CSTR("Blue x"), ReadMInt32(&tagData[32]) / 100000.0);
		frame->AddFloat(36, 4, CSTR("Blue y"), ReadMInt32(&tagData[36]) / 100000.0);
	}
	else if (tag->tagType == *(Int32*)"acTL")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(8, 4, CSTR("Number of frames"), ReadMUInt32(&tagData[8]));
		frame->AddUInt(12, 4, CSTR("Number of time to loop"), ReadMUInt32(&tagData[12]));
	}
	else if (tag->tagType == *(Int32*)"fcTL")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(8, 4, CSTR("Sequence number"), ReadMUInt32(&tagData[8]));
		frame->AddUInt(12, 4, CSTR("Width"), ReadMUInt32(&tagData[12]));
		frame->AddUInt(16, 4, CSTR("Height"), ReadMUInt32(&tagData[16]));
		frame->AddInt(20, 4, CSTR("X Offset"), ReadMInt32(&tagData[20]));
		frame->AddInt(24, 4, CSTR("Y Offset"), ReadMInt32(&tagData[24]));
		frame->AddInt(28, 2, CSTR("Delay numerator"), ReadMInt16(&tagData[28]));
		frame->AddInt(30, 2, CSTR("Delay denominator"), ReadMInt16(&tagData[30]));
		frame->AddInt(32, 1, CSTR("Disposal operation"), tagData[32]);
		frame->AddInt(33, 1, CSTR("Blend operation"), tagData[33]);
	}
	else if (tag->tagType == *(Int32*)"fdAT")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(8, 4, CSTR("Sequence number"), ReadMUInt32(&tagData[8]));
	}
	else if (tag->tagType == *(Int32*)"pHYs")
	{
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddInt(8, 4, CSTR("H Pixel per unit"), ReadMInt32(&tagData[8]));
		frame->AddInt(12, 4, CSTR("V Pixel per unit"), ReadMInt32(&tagData[12]));
		frame->AddUInt(16, 1, CSTR("Unit type"), tagData[16]);
	}
	else if (tag->tagType == *(Int32*)"iCCP")
	{
		UOSInt i;
		Data::ByteBuffer tagData(tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		i = Text::StrCharCnt((Char*)&tagData[8]);
		frame->AddStrS(8, i + 1, CSTR("Profile name"), &tagData[8]);
		i += 9;
		frame->AddUInt(i, 1, CSTR("Compression Method"), tagData[i]);

		if (tagData[i] == 0)
		{
			frame->AddUInt(i + 1, 1, CSTR("zlib Compression Method"), tagData[i + 1]);
			frame->AddUInt(i + 2, 1, CSTR("Additional flags"), tagData[i + 2]);
			frame->AddHex32(tag->size - 8, CSTR("Check value"), ReadMUInt32(&tagData[tag->size - 8]));

/*			IO::MemoryStream *mstm;
			NotNullPtr<IO::StreamData> stmData = this->fd->GetPartialData(tag->ofst + i + 3, tag->size - i - 12);
			Data::Compress::Inflate comp;
			NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"IO.FileAnalyse.PNGFileAnalyse"));
			if (!comp.Decompress(mstm, stmData))
			{
				UOSInt iccSize;
				UInt8 *iccBuff = mstm->GetBuff(&iccSize);
				Media::ICCProfile *icc = Media::ICCProfile::Parse(iccBuff, iccSize);
				if (icc)
				{
					sb->AppendC(UTF8STRC("\r\n\r\n"));
					icc->ToString(sb);
					DEL_CLASS(icc);
				}
			}
			DEL_CLASS(mstm);
			DEL_CLASS(stmData);*/
		}
	}
	else if (tag->tagType == *(Int32*)"PLTE")
	{
		if (tag->size <= 768 + 12 && (tag->size % 3) == 0)
		{
			UOSInt i;
			UOSInt j;
			Data::ByteBuffer tagData(tag->size - 12);
			this->fd->GetRealData(tag->ofst + 8, tag->size - 12, tagData);
			i = 0;
			j = 0;
			while (j < tag->size - 12)
			{
				sptr2 = Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Entry "), i) + 1;
				sptr = sptr2;
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"R");
				sptr = Text::StrUInt16(sptr, tagData[j + 0]);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"G");
				sptr = Text::StrUInt16(sptr, tagData[j + 1]);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"B");
				sptr = Text::StrUInt16(sptr, tagData[j + 2]);
				frame->AddField(8 + j, 3, CSTRP(sbuff, sptr), {sptr2, (UOSInt)(sptr - sptr2)});
				
				i++;
				j += 3;
			}
		}
	}
	frame->AddHex32(tag->size - 4, CSTR("CRC"), tag->crc);
	return frame;
}

Bool IO::FileAnalyse::PNGFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::PNGFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::PNGFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
