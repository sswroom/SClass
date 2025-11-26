#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/FileAnalyse/PSTFileAnalyse.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall IO::FileAnalyse::PSTFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::PSTFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::PSTFileAnalyse>();
	UInt64 dataSize;
	UInt64 ofst;
	UInt32 lastSize;
	UOSInt readSize;
	UInt8 hdr[12];
	Optional<IO::FileAnalyse::PSTFileAnalyse::PackItem> item;
	NN<PackItem> nnitem;
	ofst = 4;
	dataSize = me->fd->GetDataSize();
	lastSize = 0;
	
	item = 0;

	while (ofst < dataSize && !thread->IsStopping())
	{
		readSize = me->fd->GetRealData(ofst, 12, BYTEARR(hdr));
		if (readSize < 4)
			break;
		
		if (item.SetTo(nnitem))
		{
			nnitem->crc = ReadMUInt32(&hdr[0]);
		}
		if (readSize < 12)
			break;
		lastSize = ReadMUInt32(&hdr[4]);

		nnitem = MemAllocNN(IO::FileAnalyse::PSTFileAnalyse::PackItem);
		nnitem->ofst = ofst + 4;
		nnitem->size = lastSize + 12;
		nnitem->packType = (PackType)ReadInt32(&hdr[8]);
		me->tags.Add(nnitem);
		tag = nntag;
		ofst += lastSize + 12;
	}
}

IO::FileAnalyse::PSTFileAnalyse::PSTFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("PSTFileAnalyse"))
{
	UInt8 buff[256];
	this->fd = 0;
	this->pauseParsing = false;
	fd->GetRealData(0, 256, BYTEARR(buff));
	if (buff[0] != '!' || buff[1] != 'B' || buff[2] != 'D' || buff[3] != 'N' || buff[8] != 0x53 && buff[9] != 0x4d)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->thread.Start();
}

IO::FileAnalyse::PSTFileAnalyse::~PSTFileAnalyse()
{
	this->thread.Stop();
	this->fd.Delete();
	this->items.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::PSTFileAnalyse::GetFormatName()
{
	return CSTR("PST");
}

UOSInt IO::FileAnalyse::PSTFileAnalyse::GetFrameCount()
{
	return this->items.GetCount();
}

Bool IO::FileAnalyse::PSTFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::PSTFileAnalyse::PackItem> item;
	if (!this->items.GetItem(index).SetTo(item))
		return false;
	sb->AppendU64(item->ofst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->Append(PackTypeGetName(item->packType));
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUOSInt(item->size);
	return true;
}


UOSInt IO::FileAnalyse::PSTFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->items.GetCount() - 1;
	OSInt k;
	NN<PackItem> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->items.GetItemNoCheck((UOSInt)k);
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::PSTFileAnalyse::GetFrameDetail(UOSInt index)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr;
	NN<IO::FileAnalyse::FrameDetail> frame;
	NN<IO::FileAnalyse::PSTFileAnalyse::PackItem> tag;
	if (!this->items.GetItem(index).SetTo(tag))
		return 0;
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(tag->ofst, tag->size));
	sptr = Text::StrUOSInt(Text::StrConcat(sbuff, U8STR("Tag")), index);
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
		i = Text::StrCharCnt((UTF8Char*)&tagData[8]);
		frame->AddStrS(8, i + 1, CSTR("Profile name"), &tagData[8]);
		i += 9;
		frame->AddUInt(i, 1, CSTR("Compression Method"), tagData[i]);

		if (tagData[i] == 0)
		{
			frame->AddUInt(i + 1, 1, CSTR("zlib Compression Method"), tagData[i + 1]);
			frame->AddUInt(i + 2, 1, CSTR("Additional flags"), tagData[i + 2]);
			frame->AddHex32(tag->size - 8, CSTR("Check value"), ReadMUInt32(&tagData[tag->size - 8]));

/*			IO::MemoryStream *mstm;
			NN<IO::StreamData> stmData = this->fd->GetPartialData(tag->ofst + i + 3, tag->size - i - 12);
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
				frame->AddField(8 + j, 3, CSTRP(sbuff, sptr), CSTRP(sptr2, sptr));
				
				i++;
				j += 3;
			}
		}
	}
	frame->AddHex32(tag->size - 4, CSTR("CRC"), tag->crc);
	return frame;
}

Bool IO::FileAnalyse::PSTFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::PSTFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::PSTFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
