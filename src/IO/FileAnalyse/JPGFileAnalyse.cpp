#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/JPGFileAnalyse.h"
#include "Media/EXIFData.h"
#include "Media/ICCProfile.h"
#include "Parser/FileParser/TIFFParser.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/StringBuilderUTF8.h"

Text::CString IO::FileAnalyse::JPGFileAnalyse::GetTagName(UInt8 tagType)
{
	switch (tagType)
	{
	case 0x0:
		return {UTF8STRC("Data")};
	case 0x01:
		return {UTF8STRC("TMP")}; //For temporary use in arithmetic coding
	case 0xc0:
		return {UTF8STRC("SOF0")}; //Baseline DCT
	case 0xc1:
		return {UTF8STRC("SOF1")}; //Extended sequential DCT
	case 0xc2:
		return {UTF8STRC("SOF2")}; //Progressive DCT
	case 0xc3:
		return {UTF8STRC("SOF3")}; //Lossless (sequential)
	case 0xc4:
		return {UTF8STRC("DHT")}; //Define Huffman Table
	case 0xc5:
		return {UTF8STRC("SOF5")}; //Differential sequential DCT
	case 0xc6:
		return {UTF8STRC("SOF6")}; //Differential progressive DCT
	case 0xc7:
		return {UTF8STRC("SOF7")}; //Differential Lossless
	case 0xc8:
		return {UTF8STRC("JPG")}; //JPG Extension
	case 0xc9:
		return {UTF8STRC("SOF9")}; //Extended sequantial DCT
	case 0xca:
		return {UTF8STRC("SOF10")}; //Progressive DCT
	case 0xcb:
		return {UTF8STRC("SOF11")}; //Lossless (sequential)
	case 0xcc:
		return {UTF8STRC("DAC")}; //Define arithmetic conditioning table
	case 0xcd:
		return {UTF8STRC("SOF13")}; //Differential sequential DCT
	case 0xce:
		return {UTF8STRC("SOF14")}; //Differential progressive DCT
	case 0xcf:
		return {UTF8STRC("SOF15")}; //Differential Lossless
	case 0xd8:
		return {UTF8STRC("SOI")}; //Start of Image
	case 0xd9:
		return {UTF8STRC("EOI")}; //End of Image
	case 0xda:
		return {UTF8STRC("SOS")}; //Start of Scan
	case 0xdb:
		return {UTF8STRC("DQT")}; //Define quantization table
	case 0xdc:
		return {UTF8STRC("DNL")}; //Define number of lines
	case 0xdd:
		return {UTF8STRC("DRI")}; //Define restart interval
	case 0xde:
		return {UTF8STRC("DHP")}; //Define hierarchial progression
	case 0xdf:
		return {UTF8STRC("EXP")}; //Expand reference image(s)
	case 0xe0:
		return {UTF8STRC("APP0")};
	case 0xe1:
		return {UTF8STRC("APP1")};
	case 0xe2:
		return {UTF8STRC("APP2")};
	case 0xe3:
		return {UTF8STRC("APP3")};
	case 0xe4:
		return {UTF8STRC("APP4")};
	case 0xe5:
		return {UTF8STRC("APP5")};
	case 0xe6:
		return {UTF8STRC("APP6")};
	case 0xe7:
		return {UTF8STRC("APP7")};
	case 0xe8:
		return {UTF8STRC("APP8")};
	case 0xe9:
		return {UTF8STRC("APP9")};
	case 0xea:
		return {UTF8STRC("APP10")};
	case 0xeb:
		return {UTF8STRC("APP11")};
	case 0xec:
		return {UTF8STRC("APP12")};
	case 0xed:
		return {UTF8STRC("APP13")};
	case 0xee:
		return {UTF8STRC("APP14")};
	case 0xef:
		return {UTF8STRC("APP15")};
	case 0xfe:
		return {UTF8STRC("COM")}; //comment
	}
	return {0, 0};
}

UInt32 __stdcall IO::FileAnalyse::JPGFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::JPGFileAnalyse *me = (IO::FileAnalyse::JPGFileAnalyse*)userObj;
	UInt64 dataSize;
	UInt64 ofst;
	UInt32 lastSize;
	UInt8 tagHdr[4];
	IO::FileAnalyse::JPGFileAnalyse::JPGTag *tag;
	me->threadRunning = true;
	me->threadStarted = true;
	ofst = 2;
	dataSize = me->fd->GetDataSize();
	lastSize = 0;
	
	tag = MemAlloc(IO::FileAnalyse::JPGFileAnalyse::JPGTag, 1);
	tag->ofst = 0;
	tag->size = 2;
	tag->tagType = 0xd8; //SOI
	me->tags->Add(tag);

	while (ofst < dataSize - 11 && !me->threadToStop)
	{
		if (me->fd->GetRealData(ofst, 4, tagHdr) != 4)
			break;
		
		lastSize = ReadMUInt16(&tagHdr[2]);
		if (tagHdr[0] != 0xff)
			break;

		if (tagHdr[1] == 0xda) //SOS
		{
			if (lastSize < 2)
				break;
			tag = MemAlloc(IO::FileAnalyse::JPGFileAnalyse::JPGTag, 1);
			tag->ofst = ofst;
			tag->size = lastSize + 2;
			tag->tagType = tagHdr[1];
			me->tags->Add(tag);
			ofst += lastSize + 2;

			me->fd->GetRealData(dataSize - 2, 2, tagHdr);
			if (ReadMUInt16(tagHdr) == 0xffd9) //EOI
			{
				tag = MemAlloc(IO::FileAnalyse::JPGFileAnalyse::JPGTag, 1);
				tag->ofst = ofst;
				tag->size = (UOSInt)(dataSize - ofst - 2);
				tag->tagType = 0;
				me->tags->Add(tag);
				tag = MemAlloc(IO::FileAnalyse::JPGFileAnalyse::JPGTag, 1);
				tag->ofst = dataSize - 2;
				tag->size = 2;
				tag->tagType = 0xd9;
				me->tags->Add(tag);
			}
			else
			{
				tag = MemAlloc(IO::FileAnalyse::JPGFileAnalyse::JPGTag, 1);
				tag->ofst = ofst;
				tag->size = (UOSInt)(dataSize - ofst);
				tag->tagType = 0;
				me->tags->Add(tag);
			}
			break;
		}
		else
		{
			if (lastSize < 2)
				break;
			tag = MemAlloc(IO::FileAnalyse::JPGFileAnalyse::JPGTag, 1);
			tag->ofst = ofst;
			tag->size = lastSize + 2;
			tag->tagType = tagHdr[1];
			me->tags->Add(tag);
			ofst += lastSize + 2;
		}
	}
	
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::JPGFileAnalyse::JPGFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[256];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	NEW_CLASS(this->tags, Data::SyncArrayList<IO::FileAnalyse::JPGFileAnalyse::JPGTag*>());
	fd->GetRealData(0, 256, buff);
	if (buff[0] != 0xff || buff[1] != 0xd8)
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

IO::FileAnalyse::JPGFileAnalyse::~JPGFileAnalyse()
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
	LIST_FREE_FUNC(this->tags, MemFree);
	DEL_CLASS(this->tags);
}

Text::CString IO::FileAnalyse::JPGFileAnalyse::GetFormatName()
{
	return {UTF8STRC("JPEG")};
}

UOSInt IO::FileAnalyse::JPGFileAnalyse::GetFrameCount()
{
	return this->tags->GetCount();
}

Bool IO::FileAnalyse::JPGFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::JPGFileAnalyse::JPGTag *tag = this->tags->GetItem(index);
	Text::CString name;
	if (tag == 0)
		return false;
	sb->AppendU64(tag->ofst);
	sb->AppendC(UTF8STRC(": Type=0x"));
	sb->AppendHex8(tag->tagType);
	name = GetTagName(tag->tagType);
	if (name.v)
	{
		sb->AppendC(UTF8STRC(" ("));
		sb->Append(name);
		sb->AppendC(UTF8STRC(")"));
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUOSInt(tag->size);
	return true;
}

Bool IO::FileAnalyse::JPGFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	UInt8 *tagData;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int32 v;
	Text::CString name;
	IO::FileAnalyse::JPGFileAnalyse::JPGTag *tag = this->tags->GetItem(index);
	if (tag == 0)
		return false;
	sb->AppendC(UTF8STRC("Tag "));
	sb->AppendUOSInt(index);
	sb->AppendC(UTF8STRC("\r\nTagType = 0x"));
	sb->AppendHex8(tag->tagType);
	name = GetTagName(tag->tagType);
	if (name.v)
	{
		sb->AppendC(UTF8STRC(" ("));
		sb->Append(name);
		sb->AppendC(UTF8STRC(")"));
	}
	sb->AppendC(UTF8STRC("\r\nSize = "));
	sb->AppendUOSInt(tag->size);
	if (tag->tagType == 0xc4)
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nTable class = "));
		sb->AppendU16((UInt16)(tagData[4] >> 4));
		sb->AppendC(UTF8STRC("\r\nTable identifier = "));
		sb->AppendU16(tagData[4] & 15);
		i = 0;
		while (i < 16)
		{
			sb->AppendC(UTF8STRC("\r\nCode length "));
			sb->AppendUOSInt(i + 1);
			sb->AppendC(UTF8STRC(" count = "));
			sb->AppendU16(tagData[5 + i]);

			i++;
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xc8)
	{
	}
	else if (tag->tagType == 0xcc)
	{
	}
	else if (tag->tagType >= 0xc0 && tag->tagType <= 0xcf) //SOFn
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nSample precision = "));
		sb->AppendU16(tagData[4]);
		sb->AppendC(UTF8STRC("\r\nNumber of lines = "));
		sb->AppendI16(ReadMInt16(&tagData[5]));
		sb->AppendC(UTF8STRC("\r\nNumber of samples/line = "));
		sb->AppendI16(ReadMInt16(&tagData[7]));
		sb->AppendC(UTF8STRC("\r\nNumber of components in frame = "));
		sb->AppendU16(tagData[9]);
		i = 0;
		j = 10;
		while (i < tagData[9])
		{
			sb->AppendC(UTF8STRC("\r\nComponent "));
			sb->AppendUOSInt(i);
			sb->AppendC(UTF8STRC(":\r\n"));
			sb->AppendC(UTF8STRC(" Component identifier = "));
			sb->AppendU16(tagData[j]);
			sb->AppendC(UTF8STRC("\r\n Horizontal sampling factor = "));
			sb->AppendU16((UInt16)(tagData[j + 1] >> 4));
			sb->AppendC(UTF8STRC("\r\n Vertical sampling factor = "));
			sb->AppendU16(tagData[j + 1] & 15);
			sb->AppendC(UTF8STRC("\r\n Quantization table destination selector = "));
			sb->AppendU16(tagData[j + 2]);
			sb->AppendC(UTF8STRC("\r\n"));
			j += 3;

			i++;
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xda) //SOS
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nNumber of components in scan = "));
		sb->AppendU16(tagData[4]);
		j = 5;
		i = 0;
		while (i < tagData[4])
		{
			sb->AppendC(UTF8STRC("\r\nComponent "));
			sb->AppendUOSInt(i);
			sb->AppendC(UTF8STRC("\r\n Scan component selector = "));
			sb->AppendU16(tagData[j]);
			sb->AppendC(UTF8STRC("\r\n DC entropy coding selector = "));
			sb->AppendU16((UInt16)(tagData[j + 1] >> 4));
			sb->AppendC(UTF8STRC("\r\n AC entropy coding selector = "));
			sb->AppendU16(tagData[j + 1] & 15);
			j += 2;
			i++;
		}
		sb->AppendC(UTF8STRC("\r\nStart of spectral selection = "));
		sb->AppendU16(tagData[j]);
		sb->AppendC(UTF8STRC("\r\nEnd of spectral selection = "));
		sb->AppendU16(tagData[j + 1]);
		sb->AppendC(UTF8STRC("\r\nSuccessive approximation bit position high = "));
		sb->AppendU16((UInt16)(tagData[j + 2] >> 4));
		sb->AppendC(UTF8STRC("\r\nSuccessive approximation bit position low = "));
		sb->AppendU16(tagData[j + 2] & 15);
		MemFree(tagData);
	}
	else if (tag->tagType == 0xdb) //DQT
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nElement precision = "));
		sb->AppendU16((UInt16)(tagData[4] >> 4));
		sb->AppendC(UTF8STRC("\r\nTable identifier = "));
		sb->AppendU16(tagData[4] & 15);
		if (tagData[4] * 0xf0 == 0x10)
		{
			i = 8;
			j = 5;
			while (i-- > 0)
			{
				sb->AppendC(UTF8STRC("\r\n"));
				k = 8;
				while (k-- > 0)
				{
					v = ReadMUInt16(&tagData[j]);
					if (v < 10)
					{
						sb->AppendC(UTF8STRC("     "));
					}
					else if (v < 100)
					{
						sb->AppendC(UTF8STRC("    "));
					}
					else if (v < 1000)
					{
						sb->AppendC(UTF8STRC("   "));
					}
					else if (v < 10000)
					{
						sb->AppendC(UTF8STRC("  "));
					}
					else
					{
						sb->AppendC(UTF8STRC(" "));
					}
					sb->AppendI32(v);

					j += 2;
				}
			}
		}
		else
		{
			i = 8;
			j = 5;
			while (i-- > 0)
			{
				sb->AppendC(UTF8STRC("\r\n"));
				k = 8;
				while (k-- > 0)
				{
					if (tagData[j] < 10)
					{
						sb->AppendC(UTF8STRC("   "));
					}
					else if (tagData[j] < 100)
					{
						sb->AppendC(UTF8STRC("  "));
					}
					else
					{
						sb->AppendC(UTF8STRC(" "));
					}
					sb->AppendU16(tagData[j]);

					j++;
				}
			}
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xe0) //APP0
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nIdentifier = "));
		sb->AppendSlow((const UTF8Char*)&tagData[4]);
		if (tagData[4] == 'J' && tagData[5] == 'F' && tagData[6] == 'I' && tagData[7] == 'F' && tagData[8] == 0)
		{
			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(tagData[9]);
			sb->AppendC(UTF8STRC("."));
			sb->AppendU16(tagData[10]);
			sb->AppendC(UTF8STRC("\r\nDensity unit = "));
			sb->AppendU16(tagData[11]);
			sb->AppendC(UTF8STRC("\r\nHorizontal pixel density = "));
			sb->AppendI16(ReadMInt16(&tagData[12]));
			sb->AppendC(UTF8STRC("\r\nVertical pixel density = "));
			sb->AppendI16(ReadMInt16(&tagData[14]));
			sb->AppendC(UTF8STRC("\r\nX Thumbnail = "));
			sb->AppendU16(tagData[16]);
			sb->AppendC(UTF8STRC("\r\nY Thumbnail = "));
			sb->AppendU16(tagData[17]);
		}
		else if (tagData[4] == 'J' && tagData[5] == 'F' && tagData[6] == 'X' && tagData[7] == 'X' && tagData[8] == 0)
		{
			sb->AppendC(UTF8STRC("\r\nThumbnail format = "));
			sb->AppendU16(tagData[9]);
			if (tagData[9] == 10)
			{
				sb->AppendC(UTF8STRC(" (JPEG format)"));
			}
			else if (tagData[9] == 11)
			{
				sb->AppendC(UTF8STRC(" (8-bit palettized format)"));
			}
			else if (tagData[9] == 13)
			{
				sb->AppendC(UTF8STRC(" (24-bit RGB format)"));
			}
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xe1) //APP1
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nIdentifier = "));
		sb->AppendSlow((UTF8Char*)&tagData[4]);
		if (tagData[4] == 'E' && tagData[5] == 'x' && tagData[6] == 'i' && tagData[7] == 'f' && tagData[8] == 0)
		{
			Media::EXIFData::RInt32Func readInt32;
			Media::EXIFData::RInt16Func readInt16;
			Bool valid = true;
			if (*(Int16*)&tagData[10] == *(Int16*)"II")
			{
				readInt32 = Media::EXIFData::TReadInt32;
				readInt16 = Media::EXIFData::TReadInt16;
			}
			else if (*(Int16*)&tagData[10] == *(Int16*)"MM")
			{
				readInt32 = Media::EXIFData::TReadMInt32;
				readInt16 = Media::EXIFData::TReadMInt16;
			}
			else
			{
				valid = false;
			}
			if (valid)
			{
				if (readInt16(&tagData[12]) != 42)
				{
					valid = false;
				}
				if (readInt32(&tagData[14]) != 8)
				{
					valid = false;
				}
			}
			if (valid)
			{
				UInt32 nextOfst;
				Media::EXIFData *exif = Media::EXIFData::ParseIFD(fd, tag->ofst + 18, readInt32, readInt16, &nextOfst, tag->ofst + 10);
				if (exif)
				{
					sb->AppendC(UTF8STRC("\r\n"));
					exif->ToString(sb, {0, 0});
					DEL_CLASS(exif);
				}
			}
		}
		else if (Text::StrStartsWithC(&tagData[4], tag->size - 4, UTF8STRC("http://ns.adobe.com/xap/1.0/")))
		{
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC((const UTF8Char*)&tagData[33], tag->size - 33);
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xe2) //APP2
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nIdentifier = "));
		sb->AppendSlow((UTF8Char*)&tagData[4]);
		if (Text::StrStartsWithC(&tagData[4], tag->size, UTF8STRC("ICC_PROFILE")))
		{
			Media::ICCProfile *icc = Media::ICCProfile::Parse(&tagData[18], tag->size - 18);
			if (icc)
			{
				sb->AppendC(UTF8STRC("\r\n\r\n"));
				icc->ToString(sb);
				DEL_CLASS(icc);
			}
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xed) //APP13
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nIdentifier = "));
		sb->AppendSlow((UTF8Char*)&tagData[4]);
		MemFree(tagData);
	}
	else if (tag->tagType == 0xee) //APP14
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nIdentifier = "));
		sb->AppendSlow((UTF8Char*)&tagData[4]);
		MemFree(tagData);
	}
	return true;
}

UOSInt IO::FileAnalyse::JPGFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->tags->GetCount() - 1;
	OSInt k;
	JPGTag *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->tags->GetItem((UOSInt)k);
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

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::JPGFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail *frame;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	UInt8 *tagData;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int32 v;
	IO::FileAnalyse::JPGFileAnalyse::JPGTag *tag = this->tags->GetItem(index);
	if (tag == 0)
		return 0;
	
	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(tag->ofst, (UInt32)tag->size));
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Tag")), index);
	frame->AddHeader({sbuff, (UOSInt)(sptr - sbuff)});
	if (tag->tagType != 0)
	{
		frame->AddHex8(0, CSTR("Start of Tag"), 0xFF);
		frame->AddHex8Name(1, CSTR("TagType"), tag->tagType, GetTagName(tag->tagType));
	}
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Size=")), tag->size);
	frame->AddText(2, {sbuff, (UOSInt)(sptr - sbuff)});
	if (tag->tagType == 0xc4)
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		frame->AddUInt(4, 1, CSTR("Table class"), (UInt16)(tagData[4] >> 4));
		frame->AddUInt(4, 1, CSTR("Table identifier"), tagData[4] & 15);
		i = 0;
		while (i < 16)
		{
			sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Code length ")), i + 1), UTF8STRC(" count"));
			frame->AddUInt(5 + i, 1, {sbuff, (UOSInt)(sptr - sbuff)}, tagData[5 + i]);

			i++;
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xc8)
	{
	}
	else if (tag->tagType == 0xcc)
	{
	}
	else if (tag->tagType >= 0xc0 && tag->tagType <= 0xcf) //SOFn
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		frame->AddUInt(4, 1, CSTR("Sample precision"), tagData[4]);
		frame->AddInt(5, 2, CSTR("Number of lines"), ReadMInt16(&tagData[5]));
		frame->AddInt(7, 2, CSTR("Number of samples/line"), ReadMInt16(&tagData[7]));
		frame->AddUInt(9, 1, CSTR("Number of components in frame"), tagData[9]);
		i = 0;
		j = 10;
		while (i < tagData[9])
		{
			sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Component ")), i), UTF8STRC(":"));
			frame->AddText((UInt32)j, {sbuff, (UOSInt)(sptr - sbuff)});
			frame->AddUInt(j, 1, CSTR("Component identifier"), tagData[j]);
			frame->AddUInt(j + 1, 1, CSTR("Horizontal sampling factor"), (UInt16)(tagData[j + 1] >> 4));
			frame->AddUInt(j + 1, 1, CSTR("Vertical sampling factor"), tagData[j + 1] & 15);
			frame->AddUInt(j + 2, 1, CSTR("Quantization table destination selector"), tagData[j + 2]);
			j += 3;

			i++;
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xda) //SOS
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		frame->AddUInt(4, 1, CSTR("Number of components in scan"), tagData[4]);
		j = 5;
		i = 0;
		while (i < tagData[4])
		{
			sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Component ")), i), UTF8STRC(":"));
			frame->AddText((UInt32)j, {sbuff, (UOSInt)(sptr - sbuff)});
			frame->AddUInt(j, 1, CSTR("Scan component selector"), tagData[j]);
			frame->AddUInt(j + 1, 1, CSTR("DC entropy coding selector"), (UInt16)(tagData[j + 1] >> 4));
			frame->AddUInt(j + 1, 1, CSTR("AC entropy coding selector"), tagData[j + 1] & 15);
			j += 2;
			i++;
		}
		frame->AddUInt(j, 1, CSTR("Start of spectral selection"), tagData[j]);
		frame->AddUInt(j + 1, 1, CSTR("End of spectral selection"), tagData[j + 1]);
		frame->AddUInt(j + 2, 1, CSTR("Successive approximation bit position high"), (UInt16)(tagData[j + 2] >> 4));
		frame->AddUInt(j + 2, 1, CSTR("Successive approximation bit position low"), tagData[j + 2] & 15);
		MemFree(tagData);
	}
	else if (tag->tagType == 0xdb) //DQT
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		frame->AddUInt(4, 1, CSTR("Element precision"), (UInt16)(tagData[4] >> 4));
		frame->AddUInt(4, 1, CSTR("Table identifier"), tagData[4] & 15);
		if (tagData[4] * 0xf0 == 0x10)
		{
			Text::StringBuilderUTF8 sb;
			i = 8;
			j = 5;
			while (i-- > 0)
			{
				sb.AppendC(UTF8STRC("\r\n"));
				k = 8;
				while (k-- > 0)
				{
					v = ReadMUInt16(&tagData[j]);
					if (v < 10)
					{
						sb.AppendC(UTF8STRC("     "));
					}
					else if (v < 100)
					{
						sb.AppendC(UTF8STRC("    "));
					}
					else if (v < 1000)
					{
						sb.AppendC(UTF8STRC("   "));
					}
					else if (v < 10000)
					{
						sb.AppendC(UTF8STRC("  "));
					}
					else
					{
						sb.AppendC(UTF8STRC(" "));
					}
					sb.AppendI32(v);

					j += 2;
				}
			}
			frame->AddField(5, 128, {UTF8STRC("Table")}, sb.ToCString());
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			i = 8;
			j = 5;
			while (i-- > 0)
			{
				sb.AppendC(UTF8STRC("\r\n"));
				k = 8;
				while (k-- > 0)
				{
					if (tagData[j] < 10)
					{
						sb.AppendC(UTF8STRC("   "));
					}
					else if (tagData[j] < 100)
					{
						sb.AppendC(UTF8STRC("  "));
					}
					else
					{
						sb.AppendC(UTF8STRC(" "));
					}
					sb.AppendU16(tagData[j]);

					j++;
				}
			}
			frame->AddField(5, 64, {UTF8STRC("Table")}, sb.ToCString());
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xe0) //APP0
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		i = Text::StrCharCnt(&tagData[4]);
		frame->AddStrC(4, i + 1, CSTR("Identifier"), &tagData[4]);
		if (tagData[4] == 'J' && tagData[5] == 'F' && tagData[6] == 'I' && tagData[7] == 'F' && tagData[8] == 0)
		{
			frame->AddUInt(9, 1, CSTR("Major Version"), tagData[9]);
			frame->AddUInt(10, 1, CSTR("Minor Version"), tagData[10]);
			frame->AddUInt(11, 1, CSTR("Density unit"), tagData[11]);
			frame->AddInt(12, 2, CSTR("Horizontal pixel density"), ReadMInt16(&tagData[12]));
			frame->AddInt(14, 2, CSTR("Vertical pixel density"), ReadMInt16(&tagData[14]));
			frame->AddUInt(16, 1, CSTR("X Thumbnail"), tagData[16]);
			frame->AddUInt(17, 1, CSTR("Y Thumbnail"), tagData[17]);
		}
		else if (tagData[4] == 'J' && tagData[5] == 'F' && tagData[6] == 'X' && tagData[7] == 'X' && tagData[8] == 0)
		{
			Text::CString vName = {0, 0};
			switch (tagData[9])
			{
				case 10:
					vName = {UTF8STRC("JPEG format")};
					break;
				case 11:
					vName = {UTF8STRC("8-bit palettized format")};
					break;
				case 13:
					vName = {UTF8STRC("24-bit RGB format")};
					break;
			}
			frame->AddUIntName(9, 1, CSTR("Thumbnail format"), tagData[9], vName);
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xe1) //APP1
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		i = Text::StrCharCnt(&tagData[4]);
		frame->AddStrC(4, i + 1, CSTR("Identifier"), &tagData[4]);
		if (tagData[4] == 'E' && tagData[5] == 'x' && tagData[6] == 'i' && tagData[7] == 'f' && tagData[8] == 0)
		{
			Media::EXIFData::RInt32Func readInt32;
			Media::EXIFData::RInt16Func readInt16;
			Bool valid = true;
			if (*(Int16*)&tagData[10] == *(Int16*)"II")
			{
				readInt32 = Media::EXIFData::TReadInt32;
				readInt16 = Media::EXIFData::TReadInt16;
			}
			else if (*(Int16*)&tagData[10] == *(Int16*)"MM")
			{
				readInt32 = Media::EXIFData::TReadMInt32;
				readInt16 = Media::EXIFData::TReadMInt16;
			}
			else
			{
				valid = false;
			}
			if (valid)
			{
				if (readInt16(&tagData[12]) != 42)
				{
					valid = false;
				}
				if (readInt32(&tagData[14]) != 8)
				{
					valid = false;
				}
			}
			if (valid)
			{
				UInt32 nextOfst;
				Media::EXIFData *exif = Media::EXIFData::ParseIFD(fd, tag->ofst + 18, readInt32, readInt16, &nextOfst, tag->ofst + 10);
				if (exif)
				{
					Text::StringBuilderUTF8 sb;
					exif->ToString(&sb, {0, 0});
					frame->AddText(18, sb.ToCString());
					DEL_CLASS(exif);
				}
			}
		}
		else if (Text::StrStartsWithC(&tagData[4], tag->size - 4, UTF8STRC("http://ns.adobe.com/xap/1.0/")))
		{
			frame->AddStrC(33, tag->size - 33, CSTR("Data"), &tagData[33]);
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xe2) //APP2
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		i = Text::StrCharCnt(&tagData[4]);
		frame->AddStrC(4, i + 1, CSTR("Identifier"), &tagData[4]);
		if (Text::StrStartsWithC(&tagData[4], tag->size - 4, UTF8STRC("ICC_PROFILE")))
		{
			Media::ICCProfile *icc = Media::ICCProfile::Parse(&tagData[18], tag->size - 18);
			if (icc)
			{
				Text::StringBuilderUTF8 sb;
				icc->ToString(&sb);
				frame->AddText(18, sb.ToCString());
				DEL_CLASS(icc);
			}
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xed) //APP13
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		i = Text::StrCharCnt(&tagData[4]);
		frame->AddStrC(4, i + 1, CSTR("Identifier"), &tagData[4]);
		MemFree(tagData);
	}
	else if (tag->tagType == 0xee) //APP14
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		i = Text::StrCharCnt(&tagData[4]);
		frame->AddStrC(4, i + 1, CSTR("Identifier"), &tagData[4]);
		MemFree(tagData);
	}
	return frame;
}

Bool IO::FileAnalyse::JPGFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::JPGFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::JPGFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
	return false;
}
