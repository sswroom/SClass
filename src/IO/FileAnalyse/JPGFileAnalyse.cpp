#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/FileAnalyse/JPGFileAnalyse.h"
#include "Media/EXIFData.h"
#include "Media/ICCProfile.h"
#include "Parser/FileParser/TIFFParser.h"
#include "Text/Encoding.h"
#include "Text/StringBuilderUTF8.h"

Text::CString IO::FileAnalyse::JPGFileAnalyse::GetTagName(UInt8 tagType)
{
	switch (tagType)
	{
	case 0x0:
		return CSTR("Data");
	case 0x01:
		return CSTR("TMP"); //For temporary use in arithmetic coding
	case 0xc0:
		return CSTR("SOF0"); //Baseline DCT
	case 0xc1:
		return CSTR("SOF1"); //Extended sequential DCT
	case 0xc2:
		return CSTR("SOF2"); //Progressive DCT
	case 0xc3:
		return CSTR("SOF3"); //Lossless (sequential)
	case 0xc4:
		return CSTR("DHT"); //Define Huffman Table
	case 0xc5:
		return CSTR("SOF5"); //Differential sequential DCT
	case 0xc6:
		return CSTR("SOF6"); //Differential progressive DCT
	case 0xc7:
		return CSTR("SOF7"); //Differential Lossless
	case 0xc8:
		return CSTR("JPG"); //JPG Extension
	case 0xc9:
		return CSTR("SOF9"); //Extended sequantial DCT
	case 0xca:
		return CSTR("SOF10"); //Progressive DCT
	case 0xcb:
		return CSTR("SOF11"); //Lossless (sequential)
	case 0xcc:
		return CSTR("DAC"); //Define arithmetic conditioning table
	case 0xcd:
		return CSTR("SOF13"); //Differential sequential DCT
	case 0xce:
		return CSTR("SOF14"); //Differential progressive DCT
	case 0xcf:
		return CSTR("SOF15"); //Differential Lossless
	case 0xd8:
		return CSTR("SOI"); //Start of Image
	case 0xd9:
		return CSTR("EOI"); //End of Image
	case 0xda:
		return CSTR("SOS"); //Start of Scan
	case 0xdb:
		return CSTR("DQT"); //Define quantization table
	case 0xdc:
		return CSTR("DNL"); //Define number of lines
	case 0xdd:
		return CSTR("DRI"); //Define restart interval
	case 0xde:
		return CSTR("DHP"); //Define hierarchial progression
	case 0xdf:
		return CSTR("EXP"); //Expand reference image(s)
	case 0xe0:
		return CSTR("APP0");
	case 0xe1:
		return CSTR("APP1");
	case 0xe2:
		return CSTR("APP2");
	case 0xe3:
		return CSTR("APP3");
	case 0xe4:
		return CSTR("APP4");
	case 0xe5:
		return CSTR("APP5");
	case 0xe6:
		return CSTR("APP6");
	case 0xe7:
		return CSTR("APP7");
	case 0xe8:
		return CSTR("APP8");
	case 0xe9:
		return CSTR("APP9");
	case 0xea:
		return CSTR("APP10");
	case 0xeb:
		return CSTR("APP11");
	case 0xec:
		return CSTR("APP12");
	case 0xed:
		return CSTR("APP13");
	case 0xee:
		return CSTR("APP14");
	case 0xef:
		return CSTR("APP15");
	case 0xfe:
		return CSTR("COM"); //comment
	}
	return nullptr;
}

void __stdcall IO::FileAnalyse::JPGFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::JPGFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::JPGFileAnalyse>();
	NN<IO::StreamData> fd;
	UInt64 dataSize;
	UInt64 ofst;
	UInt32 lastSize;
	UInt8 tagHdr[4];
	NN<IO::FileAnalyse::JPGFileAnalyse::JPGTag> tag;
	if (!me->fd.SetTo(fd))
		return;
	ofst = 2;
	dataSize = fd->GetDataSize();
	lastSize = 0;
	
	tag = MemAllocNN(IO::FileAnalyse::JPGFileAnalyse::JPGTag);
	tag->ofst = 0;
	tag->size = 2;
	tag->tagType = 0xd8; //SOI
	me->tags.Add(tag);

	while (ofst < dataSize - 11 && !thread->IsStopping())
	{
		if (fd->GetRealData(ofst, 4, BYTEARR(tagHdr)) != 4)
			break;
		
		lastSize = ReadMUInt16(&tagHdr[2]);
		if (tagHdr[0] != 0xff)
			break;

		if (tagHdr[1] == 0xda) //SOS
		{
			if (lastSize < 2)
				break;
			tag = MemAllocNN(IO::FileAnalyse::JPGFileAnalyse::JPGTag);
			tag->ofst = ofst;
			tag->size = lastSize + 2;
			tag->tagType = tagHdr[1];
			me->tags.Add(tag);
			ofst += lastSize + 2;

			fd->GetRealData(dataSize - 2, 2, BYTEARR(tagHdr));
			if (ReadMUInt16(tagHdr) == 0xffd9) //EOI
			{
				tag = MemAllocNN(IO::FileAnalyse::JPGFileAnalyse::JPGTag);
				tag->ofst = ofst;
				tag->size = (UIntOS)(dataSize - ofst - 2);
				tag->tagType = 0;
				me->tags.Add(tag);
				tag = MemAllocNN(IO::FileAnalyse::JPGFileAnalyse::JPGTag);
				tag->ofst = dataSize - 2;
				tag->size = 2;
				tag->tagType = 0xd9;
				me->tags.Add(tag);
			}
			else
			{
				tag = MemAllocNN(IO::FileAnalyse::JPGFileAnalyse::JPGTag);
				tag->ofst = ofst;
				tag->size = (UIntOS)(dataSize - ofst);
				tag->tagType = 0;
				me->tags.Add(tag);
			}
			break;
		}
		else
		{
			if (lastSize < 2)
				break;
			tag = MemAllocNN(IO::FileAnalyse::JPGFileAnalyse::JPGTag);
			tag->ofst = ofst;
			tag->size = lastSize + 2;
			tag->tagType = tagHdr[1];
			me->tags.Add(tag);
			ofst += lastSize + 2;
		}
	}
}

IO::FileAnalyse::JPGFileAnalyse::JPGFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("JPGFileAnalyse"))
{
	UInt8 buff[256];
	this->fd = nullptr;
	this->pauseParsing = false;
	fd->GetRealData(0, 256, BYTEARR(buff));
	if (buff[0] != 0xff || buff[1] != 0xd8)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::JPGFileAnalyse::~JPGFileAnalyse()
{
	this->thread.Stop();
	this->fd.Delete();
	this->tags.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::JPGFileAnalyse::GetFormatName()
{
	return CSTR("JPEG");
}

UIntOS IO::FileAnalyse::JPGFileAnalyse::GetFrameCount()
{
	return this->tags.GetCount();
}

Bool IO::FileAnalyse::JPGFileAnalyse::GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::JPGFileAnalyse::JPGTag> tag;
	Text::CStringNN name;
	if (!this->tags.GetItem(index).SetTo(tag))
		return false;
	sb->AppendU64(tag->ofst);
	sb->AppendC(UTF8STRC(": Type=0x"));
	sb->AppendHex8(tag->tagType);
	if (GetTagName(tag->tagType).SetTo(name))
	{
		sb->AppendC(UTF8STRC(" ("));
		sb->Append(name);
		sb->AppendC(UTF8STRC(")"));
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUIntOS(tag->size);
	return true;
}

Bool IO::FileAnalyse::JPGFileAnalyse::GetFrameDetail(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	UIntOS i;
	UIntOS j;
	UIntOS k;
	Int32 v;
	Text::CStringNN name;
	NN<IO::FileAnalyse::JPGFileAnalyse::JPGTag> tag;
	NN<IO::StreamData> fd;
	if (!this->tags.GetItem(index).SetTo(tag))
		return false;
	sb->AppendC(UTF8STRC("Tag "));
	sb->AppendUIntOS(index);
	sb->AppendC(UTF8STRC("\r\nTagType = 0x"));
	sb->AppendHex8(tag->tagType);
	if (GetTagName(tag->tagType).SetTo(name))
	{
		sb->AppendC(UTF8STRC(" ("));
		sb->Append(name);
		sb->AppendC(UTF8STRC(")"));
	}
	sb->AppendC(UTF8STRC("\r\nSize = "));
	sb->AppendUIntOS(tag->size);
	if (!this->fd.SetTo(fd))
		return true;
	if (tag->tagType == 0xc4)
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nTable class = "));
		sb->AppendU16((UInt16)(tagData[4] >> 4));
		sb->AppendC(UTF8STRC("\r\nTable identifier = "));
		sb->AppendU16(tagData[4] & 15);
		i = 0;
		while (i < 16)
		{
			sb->AppendC(UTF8STRC("\r\nCode length "));
			sb->AppendUIntOS(i + 1);
			sb->AppendC(UTF8STRC(" count = "));
			sb->AppendU16(tagData[5 + i]);

			i++;
		}
	}
	else if (tag->tagType == 0xc8)
	{
	}
	else if (tag->tagType == 0xcc)
	{
	}
	else if (tag->tagType >= 0xc0 && tag->tagType <= 0xcf) //SOFn
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
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
			sb->AppendUIntOS(i);
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
	}
	else if (tag->tagType == 0xda) //SOS
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nNumber of components in scan = "));
		sb->AppendU16(tagData[4]);
		j = 5;
		i = 0;
		while (i < tagData[4])
		{
			sb->AppendC(UTF8STRC("\r\nComponent "));
			sb->AppendUIntOS(i);
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
	}
	else if (tag->tagType == 0xdb) //DQT
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
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
	}
	else if (tag->tagType == 0xe0) //APP0
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
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
	}
	else if (tag->tagType == 0xe1) //APP1
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nIdentifier = "));
		sb->AppendSlow((UTF8Char*)&tagData[4]);
		if (tagData[4] == 'E' && tagData[5] == 'x' && tagData[6] == 'i' && tagData[7] == 'f' && tagData[8] == 0)
		{
			NN<Media::EXIFData> exif;
			if (Media::EXIFData::ParseExifDirect(tagData.Arr() + 10, tag->size - 10).SetTo(exif))
			{
				sb->AppendC(UTF8STRC("\r\n"));
				exif->ToString(sb, nullptr);
				exif.Delete();
			}
		}
		else if (Text::StrStartsWithC(&tagData[4], tag->size - 4, UTF8STRC("http://ns.adobe.com/xap/1.0/")))
		{
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC((const UTF8Char*)&tagData[33], tag->size - 33);
		}
	}
	else if (tag->tagType == 0xe2) //APP2
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nIdentifier = "));
		sb->AppendSlow((UTF8Char*)&tagData[4]);
		if (Text::StrStartsWithC(&tagData[4], tag->size, UTF8STRC("ICC_PROFILE")))
		{
			NN<Media::ICCProfile> icc;
			if (Media::ICCProfile::Parse(tagData.SubArray(18, tag->size - 18)).SetTo(icc))
			{
				sb->AppendC(UTF8STRC("\r\n\r\n"));
				icc->ToString(sb);
				icc.Delete();
			}
		}
	}
	else if (tag->tagType == 0xed) //APP13
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nIdentifier = "));
		sb->AppendSlow((UTF8Char*)&tagData[4]);
	}
	else if (tag->tagType == 0xee) //APP14
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->AppendC(UTF8STRC("\r\nIdentifier = "));
		sb->AppendSlow((UTF8Char*)&tagData[4]);
	}
	return true;
}

UIntOS IO::FileAnalyse::JPGFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	IntOS i = 0;
	IntOS j = (IntOS)this->tags.GetCount() - 1;
	IntOS k;
	NN<JPGTag> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->tags.GetItemNoCheck((UIntOS)k);
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
			return (UIntOS)k;
		}
	}
	return INVALID_INDEX;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::JPGFileAnalyse::GetFrameDetail(UIntOS index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	Int32 v;
	NN<IO::FileAnalyse::JPGFileAnalyse::JPGTag> tag;
	NN<IO::StreamData> fd;
	if (!this->tags.GetItem(index).SetTo(tag) || !this->fd.SetTo(fd))
		return nullptr;
	
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(tag->ofst, tag->size));
	sptr = Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("Tag")), index);
	frame->AddHeader(CSTRP(sbuff, sptr));
	if (tag->tagType != 0)
	{
		frame->AddHex8(0, CSTR("Start of Tag"), 0xFF);
		frame->AddHex8Name(1, CSTR("TagType"), tag->tagType, GetTagName(tag->tagType));
	}
	sptr = Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("Size=")), tag->size);
	frame->AddText(2, CSTRP(sbuff, sptr));
	if (tag->tagType == 0xc4)
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		frame->AddUInt(4, 1, CSTR("Table class"), (UInt16)(tagData[4] >> 4));
		frame->AddUInt(4, 1, CSTR("Table identifier"), tagData[4] & 15);
		i = 0;
		while (i < 16)
		{
			sptr = Text::StrConcatC(Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("Code length ")), i + 1), UTF8STRC(" count"));
			frame->AddUInt(5 + i, 1, CSTRP(sbuff, sptr), tagData[5 + i]);

			i++;
		}
	}
	else if (tag->tagType == 0xc8)
	{
	}
	else if (tag->tagType == 0xcc)
	{
	}
	else if (tag->tagType >= 0xc0 && tag->tagType <= 0xcf) //SOFn
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		frame->AddUInt(4, 1, CSTR("Sample precision"), tagData[4]);
		frame->AddInt(5, 2, CSTR("Number of lines"), ReadMInt16(&tagData[5]));
		frame->AddInt(7, 2, CSTR("Number of samples/line"), ReadMInt16(&tagData[7]));
		frame->AddUInt(9, 1, CSTR("Number of components in frame"), tagData[9]);
		i = 0;
		j = 10;
		while (i < tagData[9])
		{
			sptr = Text::StrConcatC(Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("Component ")), i), UTF8STRC(":"));
			frame->AddText((UInt32)j, CSTRP(sbuff, sptr));
			frame->AddUInt(j, 1, CSTR("Component identifier"), tagData[j]);
			frame->AddUInt(j + 1, 1, CSTR("Horizontal sampling factor"), (UInt16)(tagData[j + 1] >> 4));
			frame->AddUInt(j + 1, 1, CSTR("Vertical sampling factor"), tagData[j + 1] & 15);
			frame->AddUInt(j + 2, 1, CSTR("Quantization table destination selector"), tagData[j + 2]);
			j += 3;

			i++;
		}
	}
	else if (tag->tagType == 0xda) //SOS
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		frame->AddUInt(4, 1, CSTR("Number of components in scan"), tagData[4]);
		j = 5;
		i = 0;
		while (i < tagData[4])
		{
			sptr = Text::StrConcatC(Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("Component ")), i), UTF8STRC(":"));
			frame->AddText((UInt32)j, CSTRP(sbuff, sptr));
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
	}
	else if (tag->tagType == 0xdb) //DQT
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
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
			frame->AddField(5, 128, CSTR("Table"), sb.ToCString());
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
			frame->AddField(5, 64, CSTR("Table"), sb.ToCString());
		}
	}
	else if (tag->tagType == 0xe0) //APP0
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		frame->AddStrZ(4, CSTR("Identifier"), &tagData[4]);
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
			Text::CString vName = nullptr;
			switch (tagData[9])
			{
				case 10:
					vName = CSTR("JPEG format");
					break;
				case 11:
					vName = CSTR("8-bit palettized format");
					break;
				case 13:
					vName = CSTR("24-bit RGB format");
					break;
			}
			frame->AddUIntName(9, 1, CSTR("Thumbnail format"), tagData[9], vName);
		}
	}
	else if (tag->tagType == 0xe1) //APP1
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		frame->AddStrZ(4, CSTR("Identifier"), &tagData[4]);
		if (tagData[4] == 'E' && tagData[5] == 'x' && tagData[6] == 'i' && tagData[7] == 'f' && tagData[8] == 0)
		{
			Media::EXIFData::ParseEXIFFrame(frame, 10, fd, tag->ofst + 10);
		}
		else if (Text::StrStartsWithC(&tagData[4], tag->size - 4, UTF8STRC("http://ns.adobe.com/xap/1.0/")))
		{
			frame->AddStrC(33, tag->size - 33, CSTR("Data"), &tagData[33]);
		}
	}
	else if (tag->tagType == 0xe2) //APP2
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		frame->AddStrZ(4, CSTR("Identifier"), &tagData[4]);
		if (Text::StrStartsWithC(&tagData[4], tag->size - 4, UTF8STRC("ICC_PROFILE")))
		{
			Media::ICCProfile::ParseFrame(frame, 18, &tagData[18], tag->size - 18);
		}
	}
	else if (tag->tagType == 0xed) //APP13
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		frame->AddStrZ(4, CSTR("Identifier"), &tagData[4]);
	}
	else if (tag->tagType == 0xee) //APP14
	{
		Data::ByteBuffer tagData(tag->size);
		fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, CSTR("Tag Length"), ReadMUInt16(&tagData[2]));
		frame->AddStrZ(4,CSTR("Identifier"), &tagData[4]);
	}
	return frame;
}

Bool IO::FileAnalyse::JPGFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::JPGFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::JPGFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
