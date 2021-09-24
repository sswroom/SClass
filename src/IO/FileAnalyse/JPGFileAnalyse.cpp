#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/JPGFileAnalyse.h"
#include "Media/EXIFData.h"
#include "Media/ICCProfile.h"
#include "Parser/FileParser/TIFFParser.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/StringBuilderUTF8.h"

const UTF8Char *IO::FileAnalyse::JPGFileAnalyse::GetTagName(UInt8 tagType)
{
	switch (tagType)
	{
	case 0x0:
		return (const UTF8Char*)"Data";
	case 0x01:
		return (const UTF8Char*)"TMP"; //For temporary use in arithmetic coding
	case 0xc0:
		return (const UTF8Char*)"SOF0"; //Baseline DCT
	case 0xc1:
		return (const UTF8Char*)"SOF1"; //Extended sequential DCT
	case 0xc2:
		return (const UTF8Char*)"SOF2"; //Progressive DCT
	case 0xc3:
		return (const UTF8Char*)"SOF3"; //Lossless (sequential)
	case 0xc4:
		return (const UTF8Char*)"DHT"; //Define Huffman Table
	case 0xc5:
		return (const UTF8Char*)"SOF5"; //Differential sequential DCT
	case 0xc6:
		return (const UTF8Char*)"SOF6"; //Differential progressive DCT
	case 0xc7:
		return (const UTF8Char*)"SOF7"; //Differential Lossless
	case 0xc8:
		return (const UTF8Char*)"JPG"; //JPG Extension
	case 0xc9:
		return (const UTF8Char*)"SOF9"; //Extended sequantial DCT
	case 0xca:
		return (const UTF8Char*)"SOF10"; //Progressive DCT
	case 0xcb:
		return (const UTF8Char*)"SOF11"; //Lossless (sequential)
	case 0xcc:
		return (const UTF8Char*)"DAC"; //Define arithmetic conditioning table
	case 0xcd:
		return (const UTF8Char*)"SOF13"; //Differential sequential DCT
	case 0xce:
		return (const UTF8Char*)"SOF14"; //Differential progressive DCT
	case 0xcf:
		return (const UTF8Char*)"SOF15"; //Differential Lossless
	case 0xd8:
		return (const UTF8Char*)"SOI"; //Start of Image
	case 0xd9:
		return (const UTF8Char*)"EOI"; //End of Image
	case 0xda:
		return (const UTF8Char*)"SOS"; //Start of Scan
	case 0xdb:
		return (const UTF8Char*)"DQT"; //Define quantization table
	case 0xdc:
		return (const UTF8Char*)"DNL"; //Define number of lines
	case 0xdd:
		return (const UTF8Char*)"DRI"; //Define restart interval
	case 0xde:
		return (const UTF8Char*)"DHP"; //Define hierarchial progression
	case 0xdf:
		return (const UTF8Char*)"EXP"; //Expand reference image(s)
	case 0xe0:
		return (const UTF8Char*)"APP0";
	case 0xe1:
		return (const UTF8Char*)"APP1";
	case 0xe2:
		return (const UTF8Char*)"APP2";
	case 0xe3:
		return (const UTF8Char*)"APP3";
	case 0xe4:
		return (const UTF8Char*)"APP4";
	case 0xe5:
		return (const UTF8Char*)"APP5";
	case 0xe6:
		return (const UTF8Char*)"APP6";
	case 0xe7:
		return (const UTF8Char*)"APP7";
	case 0xe8:
		return (const UTF8Char*)"APP8";
	case 0xe9:
		return (const UTF8Char*)"APP9";
	case 0xea:
		return (const UTF8Char*)"APP10";
	case 0xeb:
		return (const UTF8Char*)"APP11";
	case 0xec:
		return (const UTF8Char*)"APP12";
	case 0xed:
		return (const UTF8Char*)"APP13";
	case 0xee:
		return (const UTF8Char*)"APP14";
	case 0xef:
		return (const UTF8Char*)"APP15";
	case 0xfe:
		return (const UTF8Char*)"COM"; //comment
	}
	return 0;
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

const UTF8Char *IO::FileAnalyse::JPGFileAnalyse::GetFormatName()
{
	return (const UTF8Char*)"JPEG";
}

UOSInt IO::FileAnalyse::JPGFileAnalyse::GetFrameCount()
{
	return this->tags->GetCount();
}

Bool IO::FileAnalyse::JPGFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::JPGFileAnalyse::JPGTag *tag = this->tags->GetItem(index);
	const UTF8Char *name;
	if (tag == 0)
		return false;
	sb->AppendU64(tag->ofst);
	sb->Append((const UTF8Char*)": Type=0x");
	sb->AppendHex8(tag->tagType);
	name = GetTagName(tag->tagType);
	if (name)
	{
		sb->Append((const UTF8Char*)" (");
		sb->Append(name);
		sb->Append((const UTF8Char*)")");
	}
	sb->Append((const UTF8Char*)", size=");
	sb->AppendUOSInt(tag->size);
	return true;
}

Bool IO::FileAnalyse::JPGFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
{
	UInt8 *tagData;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int32 v;
	const UTF8Char *name;
	IO::FileAnalyse::JPGFileAnalyse::JPGTag *tag = this->tags->GetItem(index);
	if (tag == 0)
		return false;
	sb->Append((const UTF8Char*)"Tag ");
	sb->AppendUOSInt(index);
	sb->Append((const UTF8Char*)"\r\nTagType = 0x");
	sb->AppendHex8(tag->tagType);
	name = GetTagName(tag->tagType);
	if (name)
	{
		sb->Append((const UTF8Char*)" (");
		sb->Append(name);
		sb->Append((const UTF8Char*)")");
	}
	sb->Append((const UTF8Char*)"\r\nSize = ");
	sb->AppendUOSInt(tag->size);
	if (tag->tagType == 0xc4)
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nTable class = ");
		sb->AppendU16((UInt16)(tagData[4] >> 4));
		sb->Append((const UTF8Char*)"\r\nTable identifier = ");
		sb->AppendU16(tagData[4] & 15);
		i = 0;
		while (i < 16)
		{
			sb->Append((const UTF8Char*)"\r\nCode length ");
			sb->AppendUOSInt(i + 1);
			sb->Append((const UTF8Char*)" count = ");
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
		sb->Append((const UTF8Char*)"\r\nSample precision = ");
		sb->AppendU16(tagData[4]);
		sb->Append((const UTF8Char*)"\r\nNumber of lines = ");
		sb->AppendI16(ReadMInt16(&tagData[5]));
		sb->Append((const UTF8Char*)"\r\nNumber of samples/line = ");
		sb->AppendI16(ReadMInt16(&tagData[7]));
		sb->Append((const UTF8Char*)"\r\nNumber of components in frame = ");
		sb->AppendU16(tagData[9]);
		i = 0;
		j = 10;
		while (i < tagData[9])
		{
			sb->Append((const UTF8Char*)"\r\nComponent ");
			sb->AppendUOSInt(i);
			sb->Append((const UTF8Char*)":\r\n");
			sb->Append((const UTF8Char*)" Component identifier = ");
			sb->AppendU16(tagData[j]);
			sb->Append((const UTF8Char*)"\r\n Horizontal sampling factor = ");
			sb->AppendU16((UInt16)(tagData[j + 1] >> 4));
			sb->Append((const UTF8Char*)"\r\n Vertical sampling factor = ");
			sb->AppendU16(tagData[j + 1] & 15);
			sb->Append((const UTF8Char*)"\r\n Quantization table destination selector = ");
			sb->AppendU16(tagData[j + 2]);
			sb->Append((const UTF8Char*)"\r\n");
			j += 3;

			i++;
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xda) //SOS
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nNumber of components in scan = ");
		sb->AppendU16(tagData[4]);
		j = 5;
		i = 0;
		while (i < tagData[4])
		{
			sb->Append((const UTF8Char*)"\r\nComponent ");
			sb->AppendUOSInt(i);
			sb->Append((const UTF8Char*)"\r\n Scan component selector = ");
			sb->AppendU16(tagData[j]);
			sb->Append((const UTF8Char*)"\r\n DC entropy coding selector = ");
			sb->AppendU16((UInt16)(tagData[j + 1] >> 4));
			sb->Append((const UTF8Char*)"\r\n AC entropy coding selector = ");
			sb->AppendU16(tagData[j + 1] & 15);
			j += 2;
			i++;
		}
		sb->Append((const UTF8Char*)"\r\nStart of spectral selection = ");
		sb->AppendU16(tagData[j]);
		sb->Append((const UTF8Char*)"\r\nEnd of spectral selection = ");
		sb->AppendU16(tagData[j + 1]);
		sb->Append((const UTF8Char*)"\r\nSuccessive approximation bit position high = ");
		sb->AppendU16((UInt16)(tagData[j + 2] >> 4));
		sb->Append((const UTF8Char*)"\r\nSuccessive approximation bit position low = ");
		sb->AppendU16(tagData[j + 2] & 15);
		MemFree(tagData);
	}
	else if (tag->tagType == 0xdb) //DQT
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nElement precision = ");
		sb->AppendU16((UInt16)(tagData[4] >> 4));
		sb->Append((const UTF8Char*)"\r\nTable identifier = ");
		sb->AppendU16(tagData[4] & 15);
		if (tagData[4] * 0xf0 == 0x10)
		{
			i = 8;
			j = 5;
			while (i-- > 0)
			{
				sb->Append((const UTF8Char*)"\r\n");
				k = 8;
				while (k-- > 0)
				{
					v = ReadMUInt16(&tagData[j]);
					if (v < 10)
					{
						sb->Append((const UTF8Char*)"     ");
					}
					else if (v < 100)
					{
						sb->Append((const UTF8Char*)"    ");
					}
					else if (v < 1000)
					{
						sb->Append((const UTF8Char*)"   ");
					}
					else if (v < 10000)
					{
						sb->Append((const UTF8Char*)"  ");
					}
					else
					{
						sb->Append((const UTF8Char*)" ");
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
				sb->Append((const UTF8Char*)"\r\n");
				k = 8;
				while (k-- > 0)
				{
					if (tagData[j] < 10)
					{
						sb->Append((const UTF8Char*)"   ");
					}
					else if (tagData[j] < 100)
					{
						sb->Append((const UTF8Char*)"  ");
					}
					else
					{
						sb->Append((const UTF8Char*)" ");
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
		sb->Append((const UTF8Char*)"\r\nIdentifier = ");
		sb->Append((const UTF8Char*)&tagData[4]);
		if (tagData[4] == 'J' && tagData[5] == 'F' && tagData[6] == 'I' && tagData[7] == 'F' && tagData[8] == 0)
		{
			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(tagData[9]);
			sb->Append((const UTF8Char*)".");
			sb->AppendU16(tagData[10]);
			sb->Append((const UTF8Char*)"\r\nDensity unit = ");
			sb->AppendU16(tagData[11]);
			sb->Append((const UTF8Char*)"\r\nHorizontal pixel density = ");
			sb->AppendI16(ReadMInt16(&tagData[12]));
			sb->Append((const UTF8Char*)"\r\nVertical pixel density = ");
			sb->AppendI16(ReadMInt16(&tagData[14]));
			sb->Append((const UTF8Char*)"\r\nX Thumbnail = ");
			sb->AppendU16(tagData[16]);
			sb->Append((const UTF8Char*)"\r\nY Thumbnail = ");
			sb->AppendU16(tagData[17]);
		}
		else if (tagData[4] == 'J' && tagData[5] == 'F' && tagData[6] == 'X' && tagData[7] == 'X' && tagData[8] == 0)
		{
			sb->Append((const UTF8Char*)"\r\nThumbnail format = ");
			sb->AppendU16(tagData[9]);
			if (tagData[9] == 10)
			{
				sb->Append((const UTF8Char*)" (JPEG format)");
			}
			else if (tagData[9] == 11)
			{
				sb->Append((const UTF8Char*)" (8-bit palettized format)");
			}
			else if (tagData[9] == 13)
			{
				sb->Append((const UTF8Char*)" (24-bit RGB format)");
			}
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xe1) //APP1
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nIdentifier = ");
		sb->Append((UTF8Char*)&tagData[4]);
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
					sb->Append((const UTF8Char *)"\r\n");
					exif->ToString(sb, 0);
					DEL_CLASS(exif);
				}
			}
		}
		else if (Text::StrEquals((Char*)&tagData[4], "http://ns.adobe.com/xap/1.0/"))
		{
			sb->Append((const UTF8Char *)"\r\n");
			sb->AppendC((const UTF8Char *)&tagData[33], tag->size - 33);
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xe2) //APP2
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nIdentifier = ");
		sb->Append((UTF8Char*)&tagData[4]);
		if (Text::StrEquals((Char*)&tagData[4], "ICC_PROFILE"))
		{
			Media::ICCProfile *icc = Media::ICCProfile::Parse(&tagData[18], tag->size - 18);
			if (icc)
			{
				sb->Append((const UTF8Char*)"\r\n\r\n");
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
		sb->Append((const UTF8Char*)"\r\nIdentifier = ");
		sb->Append((UTF8Char*)&tagData[4]);
		MemFree(tagData);
	}
	else if (tag->tagType == 0xee) //APP14
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		sb->Append((const UTF8Char*)"\r\nIdentifier = ");
		sb->Append((UTF8Char*)&tagData[4]);
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
	UInt8 *tagData;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int32 v;
	IO::FileAnalyse::JPGFileAnalyse::JPGTag *tag = this->tags->GetItem(index);
	if (tag == 0)
		return 0;
	
	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(tag->ofst, (UInt32)tag->size));
	Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Tag"), index);
	frame->AddHeader(sbuff);
	if (tag->tagType != 0)
	{
		frame->AddHex8(0, "Start of Tag", 0xFF);
		frame->AddHex8Name(1, "TagType", tag->tagType, GetTagName(tag->tagType));
	}
	Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Size="), tag->size);
	frame->AddText(2, sbuff);
	if (tag->tagType == 0xc4)
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, "Tag Length", ReadMUInt16(&tagData[2]));
		frame->AddUInt(4, 1, "Table class", (UInt16)(tagData[4] >> 4));
		frame->AddUInt(4, 1, "Table identifier", tagData[4] & 15);
		i = 0;
		while (i < 16)
		{
			Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Code length "), i + 1), (const UTF8Char*)" count");
			frame->AddUInt(5 + i, 1, (const Char*)sbuff, tagData[5 + i]);

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
		frame->AddUInt(2, 2, "Tag Length", ReadMUInt16(&tagData[2]));
		frame->AddUInt(4, 1, "Sample precision", tagData[4]);
		frame->AddInt(5, 2, "Number of lines", ReadMInt16(&tagData[5]));
		frame->AddInt(7, 2, "Number of samples/line", ReadMInt16(&tagData[7]));
		frame->AddUInt(9, 1, "Number of components in frame", tagData[9]);
		i = 0;
		j = 10;
		while (i < tagData[9])
		{
			Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Component "), i), (const UTF8Char*)":");
			frame->AddText((UInt32)j, sbuff);
			frame->AddUInt(j, 1, "Component identifier", tagData[j]);
			frame->AddUInt(j + 1, 1, "Horizontal sampling factor", (UInt16)(tagData[j + 1] >> 4));
			frame->AddUInt(j + 1, 1, "Vertical sampling factor", tagData[j + 1] & 15);
			frame->AddUInt(j + 2, 1, "Quantization table destination selector", tagData[j + 2]);
			j += 3;

			i++;
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xda) //SOS
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, "Tag Length", ReadMUInt16(&tagData[2]));
		frame->AddUInt(4, 1, "Number of components in scan", tagData[4]);
		j = 5;
		i = 0;
		while (i < tagData[4])
		{
			Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Component "), i), (const UTF8Char*)":");
			frame->AddText((UInt32)j, sbuff);
			frame->AddUInt(j, 1, "Scan component selector", tagData[j]);
			frame->AddUInt(j + 1, 1, "DC entropy coding selector", (UInt16)(tagData[j + 1] >> 4));
			frame->AddUInt(j + 1, 1, "AC entropy coding selector", tagData[j + 1] & 15);
			j += 2;
			i++;
		}
		frame->AddUInt(j, 1, "Start of spectral selection", tagData[j]);
		frame->AddUInt(j + 1, 1, "End of spectral selection", tagData[j + 1]);
		frame->AddUInt(j + 2, 1, "Successive approximation bit position high", (UInt16)(tagData[j + 2] >> 4));
		frame->AddUInt(j + 2, 1, "Successive approximation bit position low", tagData[j + 2] & 15);
		MemFree(tagData);
	}
	else if (tag->tagType == 0xdb) //DQT
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, "Tag Length", ReadMUInt16(&tagData[2]));
		frame->AddUInt(4, 1, "Element precision", (UInt16)(tagData[4] >> 4));
		frame->AddUInt(4, 1, "Table identifier", tagData[4] & 15);
		if (tagData[4] * 0xf0 == 0x10)
		{
			Text::StringBuilderUTF8 sb;
			i = 8;
			j = 5;
			while (i-- > 0)
			{
				sb.Append((const UTF8Char*)"\r\n");
				k = 8;
				while (k-- > 0)
				{
					v = ReadMUInt16(&tagData[j]);
					if (v < 10)
					{
						sb.Append((const UTF8Char*)"     ");
					}
					else if (v < 100)
					{
						sb.Append((const UTF8Char*)"    ");
					}
					else if (v < 1000)
					{
						sb.Append((const UTF8Char*)"   ");
					}
					else if (v < 10000)
					{
						sb.Append((const UTF8Char*)"  ");
					}
					else
					{
						sb.Append((const UTF8Char*)" ");
					}
					sb.AppendI32(v);

					j += 2;
				}
			}
			frame->AddField(5, 128, (const UTF8Char*)"Table", sb.ToString());
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			i = 8;
			j = 5;
			while (i-- > 0)
			{
				sb.Append((const UTF8Char*)"\r\n");
				k = 8;
				while (k-- > 0)
				{
					if (tagData[j] < 10)
					{
						sb.Append((const UTF8Char*)"   ");
					}
					else if (tagData[j] < 100)
					{
						sb.Append((const UTF8Char*)"  ");
					}
					else
					{
						sb.Append((const UTF8Char*)" ");
					}
					sb.AppendU16(tagData[j]);

					j++;
				}
			}
			frame->AddField(5, 64, (const UTF8Char*)"Table", sb.ToString());
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xe0) //APP0
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, "Tag Length", ReadMUInt16(&tagData[2]));
		i = Text::StrCharCnt(&tagData[4]);
		frame->AddStrC(4, i + 1, "Identifier", &tagData[4]);
		if (tagData[4] == 'J' && tagData[5] == 'F' && tagData[6] == 'I' && tagData[7] == 'F' && tagData[8] == 0)
		{
			frame->AddUInt(9, 1, "Major Version", tagData[9]);
			frame->AddUInt(10, 1, "Minor Version", tagData[10]);
			frame->AddUInt(11, 1, "Density unit", tagData[11]);
			frame->AddInt(12, 2, "Horizontal pixel density", ReadMInt16(&tagData[12]));
			frame->AddInt(14, 2, "Vertical pixel density", ReadMInt16(&tagData[14]));
			frame->AddUInt(16, 1, "X Thumbnail", tagData[16]);
			frame->AddUInt(17, 1, "Y Thumbnail", tagData[17]);
		}
		else if (tagData[4] == 'J' && tagData[5] == 'F' && tagData[6] == 'X' && tagData[7] == 'X' && tagData[8] == 0)
		{
			const Char *vName = 0;
			switch (tagData[9])
			{
				case 10:
					vName = "JPEG format";
					break;
				case 11:
					vName = "8-bit palettized format";
					break;
				case 13:
					vName = "24-bit RGB format";
					break;
			}
			frame->AddUIntName(9, 1, "Thumbnail format", tagData[9], (const UTF8Char*)vName);
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xe1) //APP1
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, "Tag Length", ReadMUInt16(&tagData[2]));
		i = Text::StrCharCnt(&tagData[4]);
		frame->AddStrC(4, i + 1, "Identifier", &tagData[4]);
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
					exif->ToString(&sb, 0);
					frame->AddText(18, sb.ToString());
					DEL_CLASS(exif);
				}
			}
		}
		else if (Text::StrEquals((Char*)&tagData[4], "http://ns.adobe.com/xap/1.0/"))
		{
			frame->AddStrC(33, tag->size - 33, "Data", &tagData[33]);
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xe2) //APP2
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, "Tag Length", ReadMUInt16(&tagData[2]));
		i = Text::StrCharCnt(&tagData[4]);
		frame->AddStrC(4, i + 1, "Identifier", &tagData[4]);
		if (Text::StrEquals((Char*)&tagData[4], "ICC_PROFILE"))
		{
			Media::ICCProfile *icc = Media::ICCProfile::Parse(&tagData[18], tag->size - 18);
			if (icc)
			{
				Text::StringBuilderUTF8 sb;
				icc->ToString(&sb);
				frame->AddText(18, sb.ToString());
				DEL_CLASS(icc);
			}
		}
		MemFree(tagData);
	}
	else if (tag->tagType == 0xed) //APP13
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, "Tag Length", ReadMUInt16(&tagData[2]));
		i = Text::StrCharCnt(&tagData[4]);
		frame->AddStrC(4, i + 1, "Identifier", &tagData[4]);
		MemFree(tagData);
	}
	else if (tag->tagType == 0xee) //APP14
	{
		tagData = MemAlloc(UInt8, tag->size);
		this->fd->GetRealData(tag->ofst, tag->size, tagData);
		frame->AddUInt(2, 2, "Tag Length", ReadMUInt16(&tagData[2]));
		i = Text::StrCharCnt(&tagData[4]);
		frame->AddStrC(4, i + 1, "Identifier", &tagData[4]);
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
