#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/FGDBFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall IO::FileAnalyse::FGDBFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::FGDBFileAnalyse *me = (IO::FileAnalyse::FGDBFileAnalyse*)userObj;
	UInt64 dataSize;
	UInt64 ofst;
	UInt32 lastSize;
	UInt8 tagHdr[15];
	IO::FileAnalyse::FGDBFileAnalyse::TagInfo *tag;
	me->threadRunning = true;
	me->threadStarted = true;

	tag = MemAlloc(IO::FileAnalyse::FGDBFileAnalyse::TagInfo, 1);
	tag->ofst = 0;
	tag->size = 40;
	tag->tagType = TagType::Header;
	me->tags->Add(tag);

	me->fd->GetRealData(40, 4, tagHdr);
	lastSize = ReadUInt32(tagHdr);
	tag = MemAlloc(IO::FileAnalyse::FGDBFileAnalyse::TagInfo, 1);
	tag->ofst = 40;
	tag->size = lastSize + 4;
	tag->tagType = TagType::Field;
	me->tags->Add(tag);

//	UInt8 *fieldBuff = MemAlloc(UInt8, lastSize);

/*	ofst = me->hdrSize + 4;
	dataSize = me->fd->GetDataSize();
	lastSize = 0;
	while (ofst < dataSize - 11 && !me->threadToStop)
	{
		if (me->fd->GetRealData(ofst - 4, 15, tagHdr) != 15)
			break;
		
		if (ReadMUInt32(tagHdr) != lastSize)
			break;

		lastSize = ReadMUInt24(&tagHdr[5]) + 11;
		if (lastSize <= 11)
		{
			break;
		}
		tag = MemAlloc(IO::FileAnalyse::FGDBFileAnalyse::TagInfo, 1);
		tag->ofst = ofst;
		tag->size = lastSize;
		tag->tagType = tagHdr[4] & 0x1f;
		me->tags->Add(tag);
		ofst += lastSize + 4;
	}*/
	
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::FGDBFileAnalyse::FGDBFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[40];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	NEW_CLASS(this->tags, Data::SyncArrayList<IO::FileAnalyse::FGDBFileAnalyse::TagInfo*>());
	fd->GetRealData(0, 40, buff);
	if (ReadUInt64(&buff[24]) != fd->GetDataSize())
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

IO::FileAnalyse::FGDBFileAnalyse::~FGDBFileAnalyse()
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

const UTF8Char *IO::FileAnalyse::FGDBFileAnalyse::GetFormatName()
{
	return (const UTF8Char*)"FGDB";
}

UOSInt IO::FileAnalyse::FGDBFileAnalyse::GetFrameCount()
{
	return this->tags->GetCount();
}

Bool IO::FileAnalyse::FGDBFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::FGDBFileAnalyse::TagInfo *tag = this->tags->GetItem(index);
	if (tag == 0)
		return false;
	sb->AppendU64(tag->ofst);
	sb->Append((const UTF8Char*)": Type=");
	sb->Append(TagTypeGetName(tag->tagType));
	sb->Append((const UTF8Char*)", size=");
	sb->AppendUOSInt(tag->size);
	return true;
}

UOSInt IO::FileAnalyse::FGDBFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->tags->GetCount() - 1;
	OSInt k;
	TagInfo *pack;
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

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::FGDBFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail *frame;
	UTF8Char sbuff[128];
	UInt8 *tagData;
	IO::FileAnalyse::FGDBFileAnalyse::TagInfo *tag = this->tags->GetItem(index);
	if (tag == 0)
		return 0;
	
	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(tag->ofst, (UInt32)tag->size));
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"Type="), TagTypeGetName(tag->tagType));
	frame->AddHeader(sbuff);

	tagData = MemAlloc(UInt8, tag->size);
	this->fd->GetRealData(tag->ofst, tag->size, tagData);
	if (tag->tagType == TagType::Header)
	{
		frame->AddUInt(0, 4, "Signature", ReadUInt32(&tagData[0]));
		frame->AddUInt(4, 4, "Number of Valid Rows", ReadUInt32(&tagData[4]));
		frame->AddUInt(8, 4, "Max Row Size", ReadUInt32(&tagData[8]));
		frame->AddUInt(12, 4, "Unknown", ReadUInt32(&tagData[12]));
		frame->AddUInt(16, 4, "Unknown2", ReadUInt32(&tagData[16]));
		frame->AddUInt(20, 4, "Reserved", ReadUInt32(&tagData[20]));
		frame->AddUInt64(24, "File Size", ReadUInt64(&tagData[24]));
		frame->AddUInt64(24, "FieldDesc Offset", ReadUInt64(&tagData[32]));
	}
	else if (tag->tagType == TagType::Field)
	{
		frame->AddUInt(0, 4, "Field Desc Size", ReadUInt32(&tagData[0]));
		frame->AddUInt(4, 4, "Version", ReadUInt32(&tagData[4]));
		frame->AddHex8Name(8, "Geometry Type", tagData[8], GeometryTypeGetName(tagData[8]));
		UInt32 geoFlags = ReadUInt24(&tagData[9]);
		frame->AddHex64V(9, 3, "Flags", geoFlags);
		UOSInt nFields = ReadUInt16(&tagData[12]);
		frame->AddUInt(12, 2, "Number of fields", nFields);
		UOSInt i = 0;
		UOSInt ofst = 14;
		while (ofst + 5 <= tag->size && i < nFields)
		{
			if (ofst + 1 + (UOSInt)tagData[ofst] * 2 > tag->size)
			{
				break;
			}
			frame->AddUInt(ofst, 1, "Char Count of Name", tagData[ofst]);
			if (tagData[ofst] > 0)
			{
				*Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&tagData[ofst + 1], tagData[ofst]) = 0;
				frame->AddField(ofst + 1, tagData[ofst] * 2, (const UTF8Char*)"Field Name", sbuff);
				ofst += 1 + (UOSInt)tagData[ofst] * 2;
			}
			else
			{
				ofst += 1;
			}
			if (ofst + 1 + (UOSInt)tagData[ofst] * 2 > tag->size)
			{
				break;
			}
			frame->AddUInt(ofst, 1, "Char Count of Alias", tagData[ofst]);
			if (tagData[ofst] > 0)
			{
				*Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&tagData[ofst + 1], tagData[ofst]) = 0;
				frame->AddField(ofst + 1, tagData[ofst] * 2, (const UTF8Char*)"Alias Name", sbuff);
				ofst += 1 + (UOSInt)tagData[ofst] * 2;
			}
			else
			{
				ofst += 1;
			}
			if (ofst + 3 <= tag->size)
			{
				UInt8 fieldType = tagData[ofst];
				UInt32 fieldSize;
				UInt8 fieldFlags; //bit0 = nullable, bit2 = has_default
				frame->AddUIntName(ofst, 1, "Field Type", tagData[ofst], FieldTypeGetName(fieldType));
				if (fieldType == 4)
				{
					fieldSize = ReadUInt32(&tagData[ofst + 1]);
					fieldFlags = tagData[ofst + 5];
					frame->AddUInt(ofst + 1, 4, "Field Size", fieldSize);
					frame->AddUInt(ofst + 5, 1, "Field Flags", fieldFlags);
					ofst += 6;
				}
				else
				{
					fieldSize = tagData[ofst + 1];
				 	fieldFlags = tagData[ofst + 2];
					frame->AddUInt(ofst + 1, 1, "Field Size", fieldSize);
					frame->AddUInt(ofst + 2, 1, "Field Flags", fieldFlags);
					ofst += 3;
				}

				switch (tagData[ofst])
				{
				case 7: //Geometry
					frame->AddUInt(ofst, 2, "SRS", ReadUInt16(&tagData[ofst]));
					break;
				case 9: //Raster
					frame->AddUInt(ofst, 1, "Name Length", tagData[ofst]);
					break;
				}
				if ((fieldFlags & 4) && ofst + 1 + tagData[ofst] <= tag->size)
				{
					UInt8 ldf = tagData[ofst];
					frame->AddUInt(ofst, 1, "Default Value Len", ldf);
					if (fieldType == 4)
					{
						frame->AddStrC(ofst + 1, ldf, "Default Value", &tagData[ofst + 1]);
					}
					else
					{
						frame->AddHexBuff(ofst + 1, ldf, "Default Value", &tagData[ofst + 1], false);
					}
					ofst += 1 + ldf;
				}
				

			}
			i++;
		}
	}
	MemFree(tagData);
	return frame;
}

Bool IO::FileAnalyse::FGDBFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::FGDBFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::FGDBFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
	return false;
}

const UTF8Char *IO::FileAnalyse::FGDBFileAnalyse::GeometryTypeGetName(UInt8 t)
{
	switch (t)
	{
	case 0:
		return (const UTF8Char*)"None";
	case 1:
		return (const UTF8Char*)"Point";
	case 2:
		return (const UTF8Char*)"Multipoint";
	case 3:
		return (const UTF8Char*)"Polyline";
	case 4:
		return (const UTF8Char*)"Polygon";
	case 5:
		return (const UTF8Char*)"Rectangle";
	case 6:
		return (const UTF8Char*)"Path";
	case 7:
		return (const UTF8Char*)"Mixed";
	case 9:
		return (const UTF8Char*)"Multipath";
	case 11:
		return (const UTF8Char*)"Ring";
	case 13:
		return (const UTF8Char*)"Line";
	case 14:
		return (const UTF8Char*)"Circular Arc";
	case 15:
		return (const UTF8Char*)"Bezier Curves";
	case 16:
		return (const UTF8Char*)"Elliptic Curves";
	case 17:
		return (const UTF8Char*)"Geometry Collection";
	case 18:
		return (const UTF8Char*)"Triangle Strip";
	case 19:
		return (const UTF8Char*)"Triangle Fan";
	case 20:
		return (const UTF8Char*)"Ray";
	case 21:
		return (const UTF8Char*)"Sphere";
	case 22:
		return (const UTF8Char*)"TIN";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *IO::FileAnalyse::FGDBFileAnalyse::FieldTypeGetName(UInt8 t)
{
	switch (t)
	{
	case 0:
		return (const UTF8Char*)"Int16";
	case 1:
		return (const UTF8Char*)"Int16";
	case 2:
		return (const UTF8Char*)"Int16";
	case 3:
		return (const UTF8Char*)"Int16";
	case 4:
		return (const UTF8Char*)"Int16";
	case 5:
		return (const UTF8Char*)"Int16";
	case 6:
		return (const UTF8Char*)"Int16";
	case 7:
		return (const UTF8Char*)"Int16";
	case 8:
		return (const UTF8Char*)"Int16";
	case 9:
		return (const UTF8Char*)"Int16";
	case 10:
		return (const UTF8Char*)"Int16";
	case 11:
		return (const UTF8Char*)"Int16";
	case 12:
		return (const UTF8Char*)"Int16";
	default:
		return (const UTF8Char*)"Int16";
	}
}

const UTF8Char *IO::FileAnalyse::FGDBFileAnalyse::TagTypeGetName(TagType tagType)
{
	switch (tagType)
	{
	case TagType::Header:
		return (const UTF8Char*)"Header";
	case TagType::Field:
		return (const UTF8Char*)"Field descriptor";
	case TagType::Row:
		return (const UTF8Char*)"Row";
	default:
		return 0;
	}
}
