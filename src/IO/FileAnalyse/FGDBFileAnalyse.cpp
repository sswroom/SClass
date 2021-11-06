#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/UUID.h"
#include "IO/FileAnalyse/FGDBFileAnalyse.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

#define HAS_M_FLAG 4
#define HAS_Z_FLAG 2

UInt32 __stdcall IO::FileAnalyse::FGDBFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::FGDBFileAnalyse *me = (IO::FileAnalyse::FGDBFileAnalyse*)userObj;
	UInt64 dataSize;
	UInt64 ofst;
	UInt32 lastSize;
	Int32 rowSize;
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

	UInt8 *fieldBuff = MemAlloc(UInt8, tag->size);
	me->fd->GetRealData(40, tag->size, fieldBuff);
	me->tableInfo = Map::ESRI::FileGDBUtil::ParseFieldDesc(fieldBuff);
	MemFree(fieldBuff);

	ofst = 40 + tag->size;
	dataSize = me->fd->GetDataSize();
	while (ofst < dataSize - 4 && !me->threadToStop)
	{
		if (me->fd->GetRealData(ofst, 4, tagHdr) != 4)
			break;

		TagType tagType = TagType::Row;
		rowSize = ReadInt32(tagHdr);
		if (rowSize < 0)
		{
			rowSize = -rowSize;
			tagType = TagType::FreeSpace;
		}
		if (ofst + 4 + (UInt32)rowSize > dataSize)
		{
			break;
		}
		tag = MemAlloc(IO::FileAnalyse::FGDBFileAnalyse::TagInfo, 1);
		tag->ofst = ofst;
		tag->size = (UInt32)rowSize + 4;
		tag->tagType = tagType;
		me->tags->Add(tag);
		ofst += (UInt32)rowSize + 4;
	}
	
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
	this->tableInfo = 0;
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
	if (this->tableInfo)
	{
		Map::ESRI::FileGDBUtil::FreeTableInfo(this->tableInfo);
		this->tableInfo = 0;
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
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
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
		frame->AddUInt64(32, "FieldDesc Offset", ReadUInt64(&tagData[32]));
	}
	else if (tag->tagType == TagType::Field)
	{
		frame->AddUInt(0, 4, "Field Desc Size", ReadUInt32(&tagData[0]));
		frame->AddUInt(4, 4, "Version", ReadUInt32(&tagData[4]));
		frame->AddHex8Name(8, "Geometry Type", tagData[8], Map::ESRI::FileGDBUtil::GeometryTypeGetName(tagData[8]));
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
				frame->AddField(ofst + 1, (UOSInt)tagData[ofst] * 2, (const UTF8Char*)"Field Name", sbuff);
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
				frame->AddField(ofst + 1, (UOSInt)tagData[ofst] * 2, (const UTF8Char*)"Alias Name", sbuff);
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
				frame->AddUIntName(ofst, 1, "Field Type", fieldType, Map::ESRI::FileGDBUtil::FieldTypeGetName(fieldType));
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

				if (fieldType == 7) //Geometry
				{
					UOSInt srsLen = ReadUInt16(&tagData[ofst]);
					frame->AddUInt(ofst, 2, "SRS Length", srsLen);
					sptr = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&tagData[ofst + 2], srsLen >> 1);
					*sptr = 0;
					frame->AddField(ofst + 2, srsLen, (const UTF8Char*)"SRS", sbuff);
					UOSInt csysLen = (UOSInt)(sptr - sbuff);
					Math::CoordinateSystem *csys = Math::CoordinateSystemManager::ParsePRJBuff(this->fd->GetFullName(), (Char*)sbuff, &csysLen);
					if (csys)
					{
						Text::StringBuilderUTF8 sb;
						csys->ToString(&sb);
						frame->AddField(ofst + 2, srsLen, (const UTF8Char*)"Coordinate System", sb.ToString());
						DEL_CLASS(csys);
					}
					ofst += 2 + srsLen;
					UInt8 flags = tagData[ofst];
					frame->AddHex8(ofst, "Flags", flags);
					ofst += 1;
					frame->AddFloat(ofst, 8, "XOrigin", ReadDouble(&tagData[ofst]));
					frame->AddFloat(ofst + 8, 8, "YOrigin", ReadDouble(&tagData[ofst + 8]));
					frame->AddFloat(ofst + 16, 8, "XYScale", ReadDouble(&tagData[ofst + 16]));
					ofst += 24;
					if (flags & HAS_M_FLAG)
					{
						frame->AddFloat(ofst, 8, "MOrigin", ReadDouble(&tagData[ofst]));
						frame->AddFloat(ofst + 8, 8, "MScale", ReadDouble(&tagData[ofst + 8]));
						ofst += 16;
					}
					if (flags & HAS_Z_FLAG)
					{
						frame->AddFloat(ofst, 8, "ZOrigin", ReadDouble(&tagData[ofst]));
						frame->AddFloat(ofst + 8, 8, "ZScale", ReadDouble(&tagData[ofst + 8]));
						ofst += 16;
					}
					frame->AddFloat(ofst, 8, "XYTolerance", ReadDouble(&tagData[ofst]));
					ofst += 8;
					if (flags & HAS_M_FLAG)
					{
						frame->AddFloat(ofst, 8, "MTolerance", ReadDouble(&tagData[ofst]));
						ofst += 8;
					}
					if (flags & HAS_Z_FLAG)
					{
						frame->AddFloat(ofst, 8, "ZTolerance", ReadDouble(&tagData[ofst]));
						ofst += 8;
					}
					frame->AddFloat(ofst, 8, "XMin", ReadDouble(&tagData[ofst]));
					frame->AddFloat(ofst + 8, 8, "YMin", ReadDouble(&tagData[ofst + 8]));
					frame->AddFloat(ofst + 16, 8, "XMax", ReadDouble(&tagData[ofst + 16]));
					frame->AddFloat(ofst + 24, 8, "YMax", ReadDouble(&tagData[ofst + 24]));
					ofst += 32;
					if (this->tableInfo && this->tableInfo->geometryFlags & 0x80)
					{
						frame->AddFloat(ofst, 8, "ZMin", ReadDouble(&tagData[ofst]));
						frame->AddFloat(ofst + 8, 8, "ZMax", ReadDouble(&tagData[ofst + 8]));
						ofst += 16;
					}
					if (this->tableInfo && this->tableInfo->geometryFlags & 0x40)
					{
						frame->AddFloat(ofst, 8, "MMin", ReadDouble(&tagData[ofst]));
						frame->AddFloat(ofst + 8, 8, "MMax", ReadDouble(&tagData[ofst + 8]));
						ofst += 16;
					}
					frame->AddUInt(ofst, 1, "Unknown", tagData[ofst]);
					UOSInt gridCnt = ReadUInt32(&tagData[ofst + 1]);
					frame->AddUInt(ofst + 1, 4, "Spatial Grid Count", gridCnt);
					ofst += 5;
					while (gridCnt-- > 0)
					{
						frame->AddFloat(ofst, 8, "Spatial Grid", ReadDouble(&tagData[ofst]));
						ofst += 8;
					}
				}
				else if (fieldType == 9) //Raster
				{
					frame->AddUInt(ofst, 1, "Name Length", tagData[ofst]);
					break;
				}

				if ((fieldFlags & 4) && (fieldType < 6) && ofst + 1 + tagData[ofst] <= tag->size)
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
					ofst += 1 + (UOSInt)ldf;
				}
				if (ofst < tag->size && tagData[ofst] == 0)
				{
					frame->AddUInt(ofst, 1, "Padding", 0);
					ofst++;
				}
			}
			i++;
		}
	}
	else if (tag->tagType == TagType::Row)
	{
		frame->AddUInt(0, 4, "Row Size", ReadUInt32(&tagData[0]));
		if (this->tableInfo)
		{
			UOSInt ofst = 4;
			UOSInt ofst2;
			UOSInt nullIndex = 0;
			UOSInt i;
			UOSInt j;
			UOSInt v;
			OSInt diffMul = 1;
			Map::ESRI::FileGDBFieldInfo *field;
			if (this->tableInfo->nullableCnt > 0)
			{
				frame->AddHexBuff(4, (this->tableInfo->nullableCnt + 7) >> 3, "Null Status", &tagData[4], false);
				ofst += (this->tableInfo->nullableCnt + 7) >> 3;
			}
			i = 0;
			j = this->tableInfo->fields->GetCount();
			while (i < j)
			{
				field = this->tableInfo->fields->GetItem(i);
				Bool isNull = false;
				if (field->flags & 1)
				{
					isNull = ((tagData[4 + (nullIndex >> 3)] & (1 << (nullIndex & 7))) != 0);
					Text::StrConcat(Text::StrConcat(sbuff, field->name), (const UTF8Char*)" isNull");
					frame->AddUInt(4 + (nullIndex >> 3), 1, (const Char*)sbuff, isNull?1:0);
					nullIndex++;
				}
				if (!isNull && ofst < tag->size)
				{
					if (field->fieldType == 0) //int16
					{
						frame->AddInt(ofst, 2, (const Char*)field->name, ReadInt16(&tagData[ofst]));
						ofst += 2;
					}
					else if (field->fieldType == 1) //int32
					{
						frame->AddInt(ofst, 4, (const Char*)field->name, ReadInt32(&tagData[ofst]));
						ofst += 4;
					}
					else if (field->fieldType == 2) //float32
					{
						frame->AddFloat(ofst, 4, (const Char*)field->name, ReadFloat(&tagData[ofst]));
						ofst += 4;
					}
					else if (field->fieldType == 3) //float64
					{
						frame->AddFloat(ofst, 8, (const Char*)field->name, ReadDouble(&tagData[ofst]));
						ofst += 8;
					}
					else if (field->fieldType == 4) //String
					{
						ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
						frame->AddUInt(ofst, ofst2 - ofst, "String Length", v);
						ofst = ofst2;
						frame->AddStrC(ofst, v, (const Char*)field->name, &tagData[ofst]);
						ofst += v;
					}
					else if (field->fieldType == 5) //datetime
					{
						Double t = ReadDouble(&tagData[ofst]);
						Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"RAW "), field->name);
						frame->AddFloat(ofst, 8, (const Char*)sbuff, t);
						Data::DateTime dt;
						Map::ESRI::FileGDBUtil::ToDateTime(&dt, t);
						dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
						frame->AddField(ofst, 8, field->name, sbuff);
						ofst += 8;
					}
					else if (field->fieldType == 6) //ObjectId
					{
						frame->AddUInt(ofst, 0, (const Char*)field->name, index - 1);
					}
					else if (field->fieldType == 7) //Geometry
					{
						UOSInt geometryLen;
						ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &geometryLen);
						UOSInt endOfst = ofst2 + geometryLen;
						frame->AddUInt(ofst, ofst2 - ofst, "Len", geometryLen);
						ofst = ofst2;
						UOSInt geometryType;
						ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &geometryType);
						frame->AddHex64V(ofst, ofst2 - ofst, "GeometryType", geometryType);
						ofst = ofst2;

						switch (geometryType & 0xff)
						{
						case 1:
						case 9:
						case 11:
						case 21:
							ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
							frame->AddUInt(ofst, ofst2 - ofst, "X_Raw", v);
							frame->AddFloat(ofst, ofst2 - ofst, "X", Math::UOSInt2Double(v - 1) / this->tableInfo->xyScale + this->tableInfo->xOrigin);
							ofst = ofst2;
							ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
							frame->AddUInt(ofst, ofst2 - ofst, "Y_Raw", v);
							frame->AddFloat(ofst, ofst2 - ofst, "Y", Math::UOSInt2Double(v - 1) / this->tableInfo->xyScale + this->tableInfo->yOrigin);
							ofst = ofst2;
							if (this->tableInfo->geometryFlags & 0x80)
							{
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
								frame->AddFloat(ofst, ofst2 - ofst, "Z", Math::UOSInt2Double(v - 1) / this->tableInfo->zScale + this->tableInfo->zOrigin);
								ofst = ofst2;
							}
							if (this->tableInfo->geometryFlags & 0x40)
							{
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
								frame->AddFloat(ofst, ofst2 - ofst, "M", Math::UOSInt2Double(v - 1) / this->tableInfo->mScale + this->tableInfo->mOrigin);
								ofst = ofst2;
							}
							break;
						case 3:
						case 10:
						case 13:
						case 23:
							diffMul = -1;
						case 5:
						case 15:
						case 19:
						case 25:
							if (tagData[ofst] == 0)
							{
								frame->AddUInt(ofst, 1, "Number of Points", 0);
								ofst++;
							}
							else
							{
								UOSInt nPoints;
								UOSInt nParts;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &nPoints);
								frame->AddUInt(ofst, ofst2 - ofst, "Number of Points", nPoints);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &nParts);
								frame->AddUInt(ofst, ofst2 - ofst, "Number of Parts", nParts);
								ofst = ofst2;
								Double xMin;
								Double yMin;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
								frame->AddUInt(ofst, ofst2 - ofst, "XMin_RAW", v);
								frame->AddFloat(ofst, ofst2 - ofst, "XMin", xMin = Math::UOSInt2Double(v) / this->tableInfo->xyScale + this->tableInfo->xOrigin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
								frame->AddUInt(ofst, ofst2 - ofst, "YMin_RAW", v);
								frame->AddFloat(ofst, ofst2 - ofst, "YMin", yMin = Math::UOSInt2Double(v) / this->tableInfo->xyScale + this->tableInfo->yOrigin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
								frame->AddUInt(ofst, ofst2 - ofst, "XMax_RAW", v);
								frame->AddFloat(ofst, ofst2 - ofst, "XMax", Math::UOSInt2Double(v) / this->tableInfo->xyScale + xMin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
								frame->AddUInt(ofst, ofst2 - ofst, "YMax_RAW", v);
								frame->AddFloat(ofst, ofst2 - ofst, "YMax", Math::UOSInt2Double(v) / this->tableInfo->xyScale + yMin);
								ofst = ofst2;
								UOSInt tmpI = 1;
								while (tmpI < nParts)
								{
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
									frame->AddUInt(ofst, ofst2 - ofst, "Number of points in part", v);
									ofst = ofst2;
									tmpI++;
								}
								OSInt iv;
								OSInt dx = 0;
								OSInt dy = 0;
								tmpI = 0;
								while (tmpI < nPoints)
								{
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, &iv);
									dx += diffMul * iv;
									frame->AddFloat(ofst, ofst2 - ofst, "X", Math::OSInt2Double(dx) / this->tableInfo->xyScale + this->tableInfo->xOrigin);
									ofst = ofst2;
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, &iv);
									dy += diffMul * iv;
									frame->AddFloat(ofst, ofst2 - ofst, "Y", Math::OSInt2Double(dy) / this->tableInfo->xyScale + this->tableInfo->yOrigin);
									ofst = ofst2;
									tmpI++;
								}
								if (this->tableInfo->geometryFlags & 0x80)
								{
									dx = 0;
									tmpI = 0;
									while (tmpI < nPoints)
									{
										ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, &iv);
										dx += diffMul * iv;
										frame->AddFloat(ofst, ofst2 - ofst, "Z", Math::OSInt2Double(dx) / this->tableInfo->zScale + this->tableInfo->zOrigin);
										ofst = ofst2;
										tmpI++;
									}
								}
								if (this->tableInfo->geometryFlags & 0x40)
								{
									dx = 0;
									tmpI = 0;
									while (tmpI < nPoints)
									{
										ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, &iv);
										dx += diffMul * iv;
										frame->AddFloat(ofst, ofst2 - ofst, "M", Math::OSInt2Double(dx) / this->tableInfo->mScale + this->tableInfo->mOrigin);
										ofst = ofst2;
										tmpI++;
									}
								}
							}
							break;
						case 50:
							if (tagData[ofst] == 0)
							{
								frame->AddUInt(ofst, 1, "Number of Points", 0);
								ofst++;
							}
							else
							{
								UOSInt nPoints;
								UOSInt nParts;
								UOSInt nCurves = 0;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &nPoints);
								frame->AddUInt(ofst, ofst2 - ofst, "Number of Points", nPoints);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &nParts);
								frame->AddUInt(ofst, ofst2 - ofst, "Number of Parts", nParts);
								ofst = ofst2;
								if (geometryType & 0x20000000)
								{
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &nCurves);
									frame->AddUInt(ofst, ofst2 - ofst, "Number of Curve", nCurves);
									ofst = ofst2;
								}
								Double xMin;
								Double yMin;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
								frame->AddUInt(ofst, ofst2 - ofst, "XMin_RAW", v);
								frame->AddFloat(ofst, ofst2 - ofst, "XMin", xMin = Math::UOSInt2Double(v) / this->tableInfo->xyScale + this->tableInfo->xOrigin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
								frame->AddUInt(ofst, ofst2 - ofst, "YMin_RAW", v);
								frame->AddFloat(ofst, ofst2 - ofst, "YMin", yMin = Math::UOSInt2Double(v) / this->tableInfo->xyScale + this->tableInfo->yOrigin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
								frame->AddUInt(ofst, ofst2 - ofst, "XMax_RAW", v);
								frame->AddFloat(ofst, ofst2 - ofst, "XMax", Math::UOSInt2Double(v) / this->tableInfo->xyScale + xMin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
								frame->AddUInt(ofst, ofst2 - ofst, "YMax_RAW", v);
								frame->AddFloat(ofst, ofst2 - ofst, "YMax", Math::UOSInt2Double(v) / this->tableInfo->xyScale + yMin);
								ofst = ofst2;
								UOSInt *parts = MemAlloc(UOSInt, nParts);
								parts[nParts - 1] = nPoints;
								UOSInt tmpI = 0;
								while (tmpI < nParts - 1)
								{
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &parts[tmpI]);
									frame->AddUInt(ofst, ofst2 - ofst, "Number of points in part", parts[tmpI]);
									parts[nParts - 1] -= parts[tmpI];
									ofst = ofst2;
									tmpI++;
								}
								UOSInt tmpJ;
								tmpI = 0;
								while (tmpI < nParts)
								{
									OSInt iv;
									OSInt dx = 0;
									OSInt dy = 0;
									tmpJ = parts[tmpI];
									while (tmpJ-- > 0)
									{
										ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, &iv);
										frame->AddInt(ofst, ofst2 - ofst, "X_RAW", iv);
										dx -= iv;
										frame->AddFloat(ofst, ofst2 - ofst, "X", Math::OSInt2Double(dx) / this->tableInfo->xyScale + this->tableInfo->xOrigin);
										ofst = ofst2;
										ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, &iv);
										frame->AddInt(ofst, ofst2 - ofst, "Y_RAW", iv);
										dy -= iv;
										frame->AddFloat(ofst, ofst2 - ofst, "Y", Math::OSInt2Double(dy) / this->tableInfo->xyScale + this->tableInfo->yOrigin);
										ofst = ofst2;
									}
									if (geometryType & 0x80000000)
									{
										dx = 0;
										tmpJ = parts[tmpI];
										while (tmpJ-- > 0)
										{
											ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, &iv);
											dx -= iv;
											frame->AddFloat(ofst, ofst2 - ofst, "Z", Math::OSInt2Double(dx) / this->tableInfo->zScale + this->tableInfo->zOrigin);
											ofst = ofst2;
										}
									}
									if (geometryType & 0x40000000)
									{
										dx = 0;
										tmpJ = parts[tmpI];
										while (tmpJ-- > 0)
										{
											ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, &iv);
											dx -= iv;
											frame->AddFloat(ofst, ofst2 - ofst, "M", Math::OSInt2Double(dx) / this->tableInfo->mScale + this->tableInfo->mOrigin);
											ofst = ofst2;
										}
									}
									tmpI++;
								}
								MemFree(parts);
							}
							break;
						}
						ofst = endOfst;
					}
					else if (field->fieldType == 8) //Binary
					{
						ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
						frame->AddUInt(ofst, ofst2 - ofst, "Size", v);
						ofst = ofst2;
						frame->AddHexBuff(ofst, v, (const Char*)field->name, &tagData[ofst], true);
						ofst += v;						
					}
					else if (field->fieldType == 10 || field->fieldType == 11) //UUID
					{
						Data::UUID uuid(&tagData[ofst]);
						uuid.ToString(sbuff);
						frame->AddField(ofst, 16, field->name, sbuff);
						ofst += 16;
					}
					else if (field->fieldType == 12) //XML
					{
						ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, &v);
						frame->AddUInt(ofst, ofst2 - ofst, "Size", v);
						ofst = ofst2;
						frame->AddStrC(ofst, v, (const Char*)field->name, &tagData[ofst]);
						ofst += v;						
					}
					else
					{
						break;
					}
				}
				i++;
			}
		}
	}
	else if (tag->tagType == TagType::FreeSpace)
	{
		frame->AddInt(0, 4, "Size", ReadInt32(tagData));
		frame->AddHexBuff(4, tag->size - 4, "Deleted content", &tagData[4], true);
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
	case TagType::FreeSpace:
		return (const UTF8Char*)"FreeSpace";
	default:
		return 0;
	}
}
