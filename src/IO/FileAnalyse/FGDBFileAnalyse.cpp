#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "Data/UUID.h"
#include "IO/FileAnalyse/FGDBFileAnalyse.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math_C.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XLSUtil.h"

#define HAS_M_FLAG 4
#define HAS_Z_FLAG 2

void __stdcall IO::FileAnalyse::FGDBFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::FGDBFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::FGDBFileAnalyse>();
	UInt64 dataSize;
	UInt64 ofst;
	UInt32 lastSize;
	Int32 rowSize;
	UInt8 tagHdr[15];
	NN<IO::FileAnalyse::FGDBFileAnalyse::TagInfo> tag;
	NN<IO::StreamData> fd;
	Bool lastIsFree = false;

	tag = MemAllocNN(IO::FileAnalyse::FGDBFileAnalyse::TagInfo);
	tag->ofst = 0;
	tag->size = 40;
	tag->tagType = TagType::Header;
	me->tags.Add(tag);
	if (!me->fd.SetTo(fd))
	{
		return;
	}

	fd->GetRealData(40, 4, BYTEARR(tagHdr));
	lastSize = ReadUInt32(tagHdr);
	tag = MemAllocNN(IO::FileAnalyse::FGDBFileAnalyse::TagInfo);
	tag->ofst = 40;
	tag->size = lastSize + 4;
	tag->tagType = TagType::Field;
	me->tags.Add(tag);

	{
		Data::ByteBuffer fieldBuff(tag->size);
		fd->GetRealData(40, tag->size, fieldBuff);
		me->tableInfo = Map::ESRI::FileGDBUtil::ParseFieldDesc(fieldBuff, me->prjParser);
	}
	NN<Map::ESRI::FileGDBTableInfo> tableInfo;
	if (me->tableInfo.SetTo(tableInfo))
	{
		ofst = 40 + tag->size;
		dataSize = fd->GetDataSize();
		while (ofst < dataSize - 4 && !me->thread.IsStopping())
		{
			if (fd->GetRealData(ofst, 4, BYTEARR(tagHdr)) != 4)
				break;

			TagType tagType = TagType::Row;
			rowSize = ReadInt32(tagHdr);
			if (rowSize < 0)
			{
				rowSize = -rowSize;
				tagType = TagType::FreeSpace;
				lastIsFree = true;
			}
			else if ((UInt32)rowSize > me->maxRowSize)
			{
				if (lastIsFree)
				{
					tag->size += 4;
					ofst += 4;
					continue;
				}
				else
				{
					break;
				}
			}
			else
			{
				lastIsFree = false;
			}
			if (ofst + 4 + (UInt32)rowSize > dataSize)
			{
				break;
			}
			tag = MemAllocNN(IO::FileAnalyse::FGDBFileAnalyse::TagInfo);
			tag->ofst = ofst;
			tag->size = (UInt32)rowSize + 4;
			tag->tagType = tagType;
			me->tags.Add(tag);
			ofst += (UInt32)rowSize + 4;
		}
	}
}

IO::FileAnalyse::FGDBFileAnalyse::FGDBFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("FGDBFileAnalyse"))
{
	UInt8 buff[40];
	this->fd = nullptr;
	this->pauseParsing = false;
	this->tableInfo = nullptr;
	fd->GetRealData(0, 40, BYTEARR(buff));
	if (ReadUInt64(&buff[24]) != fd->GetDataSize())
	{
		return;
	}
	this->maxRowSize = ReadUInt32(&buff[4]);
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::FGDBFileAnalyse::~FGDBFileAnalyse()
{
	this->thread.Stop();
	NN<Map::ESRI::FileGDBTableInfo> tableInfo;
	if (this->tableInfo.SetTo(tableInfo))
	{
		Map::ESRI::FileGDBUtil::FreeTableInfo(tableInfo);
		this->tableInfo = nullptr;
	}
	this->fd.Delete();
	this->tags.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::FGDBFileAnalyse::GetFormatName()
{
	return CSTR("FGDB Table");
}

UOSInt IO::FileAnalyse::FGDBFileAnalyse::GetFrameCount()
{
	return this->tags.GetCount();
}

Bool IO::FileAnalyse::FGDBFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::FGDBFileAnalyse::TagInfo> tag;
	if (!this->tags.GetItem(index).SetTo(tag))
		return false;
	sb->AppendU64(tag->ofst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->Append(TagTypeGetName(tag->tagType));
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendUOSInt(tag->size);
	return true;
}

UOSInt IO::FileAnalyse::FGDBFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->tags.GetCount() - 1;
	OSInt k;
	NN<TagInfo> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->tags.GetItemNoCheck((UOSInt)k);
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::FGDBFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	NN<IO::StreamData> fd;
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::FileAnalyse::FGDBFileAnalyse::TagInfo> tag;
	if (!this->tags.GetItem(index).SetTo(tag))
		return nullptr;
	if (!this->fd.SetTo(fd))
		return nullptr;
	NN<Map::ESRI::FileGDBTableInfo> tableInfo;
	
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(tag->ofst, tag->size));
	sptr = TagTypeGetName(tag->tagType).ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Type=")));
	frame->AddHeader(CSTRP(sbuff, sptr));

	Data::ByteBuffer tagData(tag->size);
	fd->GetRealData(tag->ofst, tag->size, tagData);
	if (tag->tagType == TagType::Header)
	{
		frame->AddUInt(0, 4, CSTR("Signature"), ReadUInt32(&tagData[0]));
		frame->AddUInt(4, 4, CSTR("Number of Valid Rows"), ReadUInt32(&tagData[4]));
		frame->AddUInt(8, 4, CSTR("Max Row Size"), ReadUInt32(&tagData[8]));
		frame->AddUInt(12, 4, CSTR("Unknown"), ReadUInt32(&tagData[12]));
		frame->AddUInt(16, 4, CSTR("Unknown2"), ReadUInt32(&tagData[16]));
		frame->AddUInt(20, 4, CSTR("Reserved"), ReadUInt32(&tagData[20]));
		frame->AddUInt64(24, CSTR("File Size"), ReadUInt64(&tagData[24]));
		frame->AddUInt64(32, CSTR("FieldDesc Offset"), ReadUInt64(&tagData[32]));
	}
	else if (tag->tagType == TagType::Field)
	{
		frame->AddUInt(0, 4, CSTR("Field Desc Size"), ReadUInt32(&tagData[0]));
		frame->AddUInt(4, 4, CSTR("Version"), ReadUInt32(&tagData[4]));
		frame->AddHex8Name(8, CSTR("Geometry Type"), tagData[8], Map::ESRI::FileGDBUtil::GeometryTypeGetName(tagData[8]));
		UInt32 geoFlags = ReadUInt24(&tagData[9]);
		frame->AddHex64V(9, 3, CSTR("Flags"), geoFlags);
		UOSInt nFields = ReadUInt16(&tagData[12]);
		frame->AddUInt(12, 2, CSTR("Number of fields"), nFields);
		UOSInt i = 0;
		UOSInt ofst = 14;
		while (ofst + 5 <= tag->size && i < nFields)
		{
			if (ofst + 1 + (UOSInt)tagData[ofst] * 2 > tag->size)
			{
				break;
			}
			frame->AddUInt(ofst, 1, CSTR("Char Count of Name"), tagData[ofst]);
			if (tagData[ofst] > 0)
			{
				sptr = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&tagData[ofst + 1], tagData[ofst]);
				sptr[0] = 0;
				frame->AddField(ofst + 1, (UOSInt)tagData[ofst] * 2, CSTR("Field Name"), CSTRP(sbuff, sptr));
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
			frame->AddUInt(ofst, 1, CSTR("Char Count of Alias"), tagData[ofst]);
			if (tagData[ofst] > 0)
			{
				sptr = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&tagData[ofst + 1], tagData[ofst]);
				sptr[0] = 0;
				frame->AddField(ofst + 1, (UOSInt)tagData[ofst] * 2, CSTR("Alias Name"), CSTRP(sbuff, sptr));
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
				frame->AddUIntName(ofst, 1, CSTR("Field Type"), fieldType, Map::ESRI::FileGDBUtil::FieldTypeGetName(fieldType));
				if (fieldType == 4)
				{
					fieldSize = ReadUInt32(&tagData[ofst + 1]);
					fieldFlags = tagData[ofst + 5];
					frame->AddUInt(ofst + 1, 4, CSTR("Field Size"), fieldSize);
					frame->AddUInt(ofst + 5, 1, CSTR("Field Flags"), fieldFlags);
					ofst += 6;
				}
				else
				{
					fieldSize = tagData[ofst + 1];
				 	fieldFlags = tagData[ofst + 2];
					frame->AddUInt(ofst + 1, 1, CSTR("Field Size"), fieldSize);
					frame->AddUInt(ofst + 2, 1, CSTR("Field Flags"), fieldFlags);
					ofst += 3;
				}

				if (fieldType == 7) //Geometry
				{
					UOSInt srsLen = ReadUInt16(&tagData[ofst]);
					frame->AddUInt(ofst, 2, CSTR("SRS Length"), srsLen);
					sptr = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&tagData[ofst + 2], srsLen >> 1);
					*sptr = 0;
					frame->AddField(ofst + 2, srsLen, CSTR("SRS"), CSTRP(sbuff, sptr));
					UOSInt csysLen = (UOSInt)(sptr - sbuff);
					NN<Math::CoordinateSystem> csys;
					if (this->prjParser.ParsePRJBuff(fd->GetFullName()->ToCString(), sbuff, csysLen, csysLen).SetTo(csys))
					{
						Text::StringBuilderUTF8 sb;
						csys->ToString(sb);
						frame->AddField(ofst + 2, srsLen, CSTR("Coordinate System"), sb.ToCString());
						csys.Delete();
					}
					ofst += 2 + srsLen;
					UInt8 flags = tagData[ofst];
					frame->AddHex8(ofst, CSTR("Flags"), flags);
					ofst += 1;
					frame->AddFloat(ofst, 8, CSTR("XOrigin"), ReadDouble(&tagData[ofst]));
					frame->AddFloat(ofst + 8, 8, CSTR("YOrigin"), ReadDouble(&tagData[ofst + 8]));
					frame->AddFloat(ofst + 16, 8, CSTR("XYScale"), ReadDouble(&tagData[ofst + 16]));
					ofst += 24;
					if (flags & HAS_M_FLAG)
					{
						frame->AddFloat(ofst, 8, CSTR("MOrigin"), ReadDouble(&tagData[ofst]));
						frame->AddFloat(ofst + 8, 8, CSTR("MScale"), ReadDouble(&tagData[ofst + 8]));
						ofst += 16;
					}
					if (flags & HAS_Z_FLAG)
					{
						frame->AddFloat(ofst, 8, CSTR("ZOrigin"), ReadDouble(&tagData[ofst]));
						frame->AddFloat(ofst + 8, 8, CSTR("ZScale"), ReadDouble(&tagData[ofst + 8]));
						ofst += 16;
					}
					frame->AddFloat(ofst, 8, CSTR("XYTolerance"), ReadDouble(&tagData[ofst]));
					ofst += 8;
					if (flags & HAS_M_FLAG)
					{
						frame->AddFloat(ofst, 8, CSTR("MTolerance"), ReadDouble(&tagData[ofst]));
						ofst += 8;
					}
					if (flags & HAS_Z_FLAG)
					{
						frame->AddFloat(ofst, 8, CSTR("ZTolerance"), ReadDouble(&tagData[ofst]));
						ofst += 8;
					}
					frame->AddFloat(ofst, 8, CSTR("XMin"), ReadDouble(&tagData[ofst]));
					frame->AddFloat(ofst + 8, 8, CSTR("YMin"), ReadDouble(&tagData[ofst + 8]));
					frame->AddFloat(ofst + 16, 8, CSTR("XMax"), ReadDouble(&tagData[ofst + 16]));
					frame->AddFloat(ofst + 24, 8, CSTR("YMax"), ReadDouble(&tagData[ofst + 24]));
					ofst += 32;
					if (this->tableInfo.SetTo(tableInfo) && tableInfo->geometryFlags & 0x80)
					{
						frame->AddFloat(ofst, 8, CSTR("ZMin"), ReadDouble(&tagData[ofst]));
						frame->AddFloat(ofst + 8, 8, CSTR("ZMax"), ReadDouble(&tagData[ofst + 8]));
						ofst += 16;
					}
					if (this->tableInfo.SetTo(tableInfo) && tableInfo->geometryFlags & 0x40)
					{
						frame->AddFloat(ofst, 8, CSTR("MMin"), ReadDouble(&tagData[ofst]));
						frame->AddFloat(ofst + 8, 8, CSTR("MMax"), ReadDouble(&tagData[ofst + 8]));
						ofst += 16;
					}
					frame->AddUInt(ofst, 1, CSTR("Unknown"), tagData[ofst]);
					UOSInt gridCnt = ReadUInt32(&tagData[ofst + 1]);
					frame->AddUInt(ofst + 1, 4, CSTR("Spatial Grid Count"), gridCnt);
					ofst += 5;
					while (gridCnt-- > 0)
					{
						frame->AddFloat(ofst, 8, CSTR("Spatial Grid"), ReadDouble(&tagData[ofst]));
						ofst += 8;
					}
				}
				else if (fieldType == 9) //Raster
				{
					frame->AddUInt(ofst, 1, CSTR("Name Length"), tagData[ofst]);
					break;
				}

				if ((fieldFlags & 4) && (fieldType < 6) && ofst + 1 + tagData[ofst] <= tag->size)
				{
					UInt8 ldf = tagData[ofst];
					frame->AddUInt(ofst, 1, CSTR("Default Value Len"), ldf);
					if (fieldType == 4)
					{
						frame->AddStrC(ofst + 1, ldf, CSTR("Default Value"), &tagData[ofst + 1]);
					}
					else
					{
						frame->AddHexBuff(ofst + 1, ldf, CSTR("Default Value"), &tagData[ofst + 1], false);
					}
					ofst += 1 + (UOSInt)ldf;
				}
				if (ofst < tag->size && tagData[ofst] == 0)
				{
					frame->AddUInt(ofst, 1, CSTR("Padding"), 0);
					ofst++;
				}
			}
			i++;
		}
	}
	else if (tag->tagType == TagType::Row)
	{
		frame->AddUInt(0, 4, CSTR("Row Size"), ReadUInt32(&tagData[0]));
		if (this->tableInfo.SetTo(tableInfo))
		{
			UOSInt ofst = 4;
			UOSInt ofst2;
			UOSInt nullIndex = 0;
			UOSInt i;
			UOSInt j;
			UInt64 v;
			OSInt diffMul = 1;
			NN<Map::ESRI::FileGDBFieldInfo> field;
			if (tableInfo->nullableCnt > 0)
			{
				frame->AddHexBuff(4, (tableInfo->nullableCnt + 7) >> 3, CSTR("Null Status"), &tagData[4], false);
				ofst += (tableInfo->nullableCnt + 7) >> 3;
			}
			i = 0;
			j = tableInfo->fields->GetCount();
			while (i < j)
			{
				field = tableInfo->fields->GetItemNoCheck(i);
				Bool isNull = false;
				if (field->flags & 1)
				{
					isNull = ((tagData[4 + (nullIndex >> 3)] & (1 << (nullIndex & 7))) != 0);
					sptr = Text::StrConcatC(field->name->ConcatTo(sbuff), UTF8STRC(" isNull"));
					frame->AddUInt(4 + (nullIndex >> 3), 1, CSTRP(sbuff, sptr), isNull?1:0);
					nullIndex++;
				}
				if (!isNull && ofst < tag->size)
				{
					if (field->fieldType == 0) //int16
					{
						frame->AddInt(ofst, 2, field->name->ToCString(), ReadInt16(&tagData[ofst]));
						ofst += 2;
					}
					else if (field->fieldType == 1) //int32
					{
						frame->AddInt(ofst, 4, field->name->ToCString(), ReadInt32(&tagData[ofst]));
						ofst += 4;
					}
					else if (field->fieldType == 2) //float32
					{
						frame->AddFloat(ofst, 4, field->name->ToCString(), ReadFloat(&tagData[ofst]));
						ofst += 4;
					}
					else if (field->fieldType == 3) //float64
					{
						frame->AddFloat(ofst, 8, field->name->ToCString(), ReadDouble(&tagData[ofst]));
						ofst += 8;
					}
					else if (field->fieldType == 4) //String
					{
						ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
						frame->AddUInt(ofst, ofst2 - ofst, CSTR("String Length"), (UOSInt)v);
						ofst = ofst2;
						if (ofst + v > tag->size)
						{
							break;
						}
						frame->AddStrC(ofst, (UOSInt)v, field->name->ToCString(), &tagData[ofst]);
						ofst += (UOSInt)v;
					}
					else if (field->fieldType == 5) //datetime
					{
						Double t = ReadDouble(&tagData[ofst]);
						sptr = field->name->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("RAW ")));
						frame->AddFloat(ofst, 8, CSTRP(sbuff, sptr), t);
						Data::DateTime dt;
						Text::XLSUtil::Number2Date(dt, t);
						sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
						frame->AddField(ofst, 8, field->name->ToCString(), CSTRP(sbuff, sptr));
						ofst += 8;
					}
					else if (field->fieldType == 6) //ObjectId
					{
						frame->AddUInt(ofst, 0, field->name->ToCString(), index - 1);
					}
					else if (field->fieldType == 7) //Geometry
					{
						UInt64 geometryLen;
						ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, geometryLen);
						UOSInt endOfst = ofst2 + (UOSInt)geometryLen;
						frame->AddUInt(ofst, ofst2 - ofst, CSTR("Len"), (UOSInt)geometryLen);
						ofst = ofst2;
						UInt64 geometryType;
						ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, geometryType);
						frame->AddHex64V(ofst, ofst2 - ofst, CSTR("GeometryType"), geometryType);
						ofst = ofst2;

						switch (geometryType & 0xff)
						{
						case 1:
						case 9:
						case 11:
						case 21:
							ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
							frame->AddUInt(ofst, ofst2 - ofst, CSTR("X_Raw"), (UOSInt)v);
							frame->AddFloat(ofst, ofst2 - ofst, CSTR("X"), UOSInt2Double(v - 1) / tableInfo->xyScale + tableInfo->xOrigin);
							ofst = ofst2;
							ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
							frame->AddUInt(ofst, ofst2 - ofst, CSTR("Y_Raw"), (UOSInt)v);
							frame->AddFloat(ofst, ofst2 - ofst, CSTR("Y"), UOSInt2Double(v - 1) / tableInfo->xyScale + tableInfo->yOrigin);
							ofst = ofst2;
							if (tableInfo->geometryFlags & 0x80)
							{
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("Z"), UOSInt2Double(v - 1) / tableInfo->zScale + tableInfo->zOrigin);
								ofst = ofst2;
							}
							if (tableInfo->geometryFlags & 0x40)
							{
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("M"), UOSInt2Double(v - 1) / tableInfo->mScale + tableInfo->mOrigin);
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
								frame->AddUInt(ofst, 1, CSTR("Number of Points"), 0);
								ofst++;
							}
							else
							{
								UInt64 nPoints;
								UInt64 nParts;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, nPoints);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("Number of Points"), (UOSInt)nPoints);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, nParts);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("Number of Parts"), (UOSInt)nParts);
								ofst = ofst2;
								Double xMin;
								Double yMin;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("XMin_RAW"), (UOSInt)v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("XMin"), xMin = UOSInt2Double(v) / tableInfo->xyScale + tableInfo->xOrigin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("YMin_RAW"), (UOSInt)v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("YMin"), yMin = UOSInt2Double(v) / tableInfo->xyScale + tableInfo->yOrigin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("XMax_RAW"), (UOSInt)v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("XMax"), UOSInt2Double(v) / tableInfo->xyScale + xMin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("YMax_RAW"), (UOSInt)v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("YMax"), UOSInt2Double(v) / tableInfo->xyScale + yMin);
								ofst = ofst2;
								UOSInt tmpI = 1;
								while (tmpI < nParts)
								{
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
									frame->AddUInt(ofst, ofst2 - ofst, CSTR("Number of points in part"), (UOSInt)v);
									ofst = ofst2;
									tmpI++;
								}
								Int64 iv;
								OSInt dx = 0;
								OSInt dy = 0;
								tmpI = 0;
								while (tmpI < nPoints)
								{
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, iv);
									dx += diffMul * (OSInt)iv;
									frame->AddFloat(ofst, ofst2 - ofst, CSTR("X"), OSInt2Double(dx) / tableInfo->xyScale + tableInfo->xOrigin);
									ofst = ofst2;
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, iv);
									dy += diffMul * (OSInt)iv;
									frame->AddFloat(ofst, ofst2 - ofst, CSTR("Y"), OSInt2Double(dy) / tableInfo->xyScale + tableInfo->yOrigin);
									ofst = ofst2;
									tmpI++;
								}
								if (tableInfo->geometryFlags & 0x80)
								{
									dx = 0;
									tmpI = 0;
									while (tmpI < nPoints)
									{
										ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, iv);
										dx += diffMul * (OSInt)iv;
										frame->AddFloat(ofst, ofst2 - ofst, CSTR("Z"), OSInt2Double(dx) / tableInfo->zScale + tableInfo->zOrigin);
										ofst = ofst2;
										tmpI++;
									}
								}
								if (tableInfo->geometryFlags & 0x40)
								{
									dx = 0;
									tmpI = 0;
									while (tmpI < nPoints)
									{
										ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, iv);
										dx += diffMul * (OSInt)iv;
										frame->AddFloat(ofst, ofst2 - ofst, CSTR("M"), OSInt2Double(dx) / tableInfo->mScale + tableInfo->mOrigin);
										ofst = ofst2;
										tmpI++;
									}
								}
							}
							break;
						case 50:
							if (tagData[ofst] == 0)
							{
								frame->AddUInt(ofst, 1, CSTR("Number of Points"), 0);
								ofst++;
							}
							else
							{
								UInt64 nPoints;
								UInt64 nParts;
								UInt64 nCurves = 0;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, nPoints);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("Number of Points"), (UOSInt)nPoints);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, nParts);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("Number of Parts"), (UOSInt)nParts);
								ofst = ofst2;
								if (geometryType & 0x20000000)
								{
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, nCurves);
									frame->AddUInt(ofst, ofst2 - ofst, CSTR("Number of Curve"), (UOSInt)nCurves);
									ofst = ofst2;
								}
								Double xMin;
								Double yMin;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("XMin_RAW"), (UOSInt)v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("XMin"), xMin = UOSInt2Double(v) / tableInfo->xyScale + tableInfo->xOrigin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("YMin_RAW"), (UOSInt)v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("YMin"), yMin = UOSInt2Double(v) / tableInfo->xyScale + tableInfo->yOrigin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("XMax_RAW"), (UOSInt)v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("XMax"), UOSInt2Double(v) / tableInfo->xyScale + xMin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("YMax_RAW"), (UOSInt)v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("YMax"), UOSInt2Double(v) / tableInfo->xyScale + yMin);
								ofst = ofst2;
								UInt64 *parts = MemAlloc(UInt64, (UOSInt)nParts);
								parts[nParts - 1] = nPoints;
								UOSInt tmpI = 0;
								while (tmpI < nParts - 1)
								{
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, parts[tmpI]);
									frame->AddUInt(ofst, ofst2 - ofst, CSTR("Number of points in part"), (UOSInt)parts[tmpI]);
									parts[nParts - 1] -= parts[tmpI];
									ofst = ofst2;
									tmpI++;
								}
								Int64 iv;
								OSInt dx = 0;
								OSInt dy = 0;
								tmpI = nPoints;
								while (tmpI-- > 0)
								{
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, iv);
									frame->AddInt(ofst, ofst2 - ofst, CSTR("X_RAW"), (OSInt)iv);
									dx -= (OSInt)iv;
									frame->AddFloat(ofst, ofst2 - ofst, CSTR("X"), OSInt2Double(dx) / tableInfo->xyScale + tableInfo->xOrigin);
									ofst = ofst2;
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, iv);
									frame->AddInt(ofst, ofst2 - ofst, CSTR("Y_RAW"), (OSInt)iv);
									dy -= (OSInt)iv;
									frame->AddFloat(ofst, ofst2 - ofst, CSTR("Y"), OSInt2Double(dy) / tableInfo->xyScale + tableInfo->yOrigin);
									ofst = ofst2;
								}
								if (geometryType & 0x80000000)
								{
									dx = 0;
									tmpI = nPoints;
									while (tmpI-- > 0)
									{
										ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, iv);
										dx -= (OSInt)iv;
										frame->AddFloat(ofst, ofst2 - ofst, CSTR("Z"), OSInt2Double(dx) / tableInfo->zScale + tableInfo->zOrigin);
										ofst = ofst2;
									}
								}
								if (geometryType & 0x40000000)
								{
									dx = 0;
									tmpI = nPoints;
									while (tmpI-- > 0)
									{
										ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, iv);
										dx -= (OSInt)iv;
										frame->AddFloat(ofst, ofst2 - ofst, CSTR("M"), OSInt2Double(dx) / tableInfo->mScale + tableInfo->mOrigin);
										ofst = ofst2;
									}
								}
								MemFree(parts);
							}
							break;
						case 51:
							if (tagData[ofst] == 0)
							{
								frame->AddUInt(ofst, 1, CSTR("Number of Points"), 0);
								ofst++;
							}
							else
							{
								UInt64 nPoints;
								UInt64 nParts;
								UInt64 nCurves = 0;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, nPoints);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("Number of Points"), (UOSInt)nPoints);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, nParts);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("Number of Parts"), (UOSInt)nParts);
								ofst = ofst2;
								if (geometryType & 0x20000000)
								{
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, nCurves);
									frame->AddUInt(ofst, ofst2 - ofst, CSTR("Number of Curve"), (UOSInt)nCurves);
									ofst = ofst2;
								}
								Double xMin;
								Double yMin;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("XMin_RAW"), (UOSInt)v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("XMin"), xMin = UOSInt2Double(v) / tableInfo->xyScale + tableInfo->xOrigin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("YMin_RAW"), (UOSInt)v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("YMin"), yMin = UOSInt2Double(v) / tableInfo->xyScale + tableInfo->yOrigin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("XMax_RAW"), (UOSInt)v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("XMax"), UOSInt2Double(v) / tableInfo->xyScale + xMin);
								ofst = ofst2;
								ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
								frame->AddUInt(ofst, ofst2 - ofst, CSTR("YMax_RAW"), (UOSInt)v);
								frame->AddFloat(ofst, ofst2 - ofst, CSTR("YMax"), UOSInt2Double(v) / tableInfo->xyScale + yMin);
								ofst = ofst2;
								UInt64 *parts = MemAlloc(UInt64, (UOSInt)nParts);
								parts[nParts - 1] = nPoints;
								UOSInt tmpI = 0;
								while (tmpI < nParts - 1)
								{
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, parts[tmpI]);
									frame->AddUInt(ofst, ofst2 - ofst, CSTR("Number of points in part"), (UOSInt)parts[tmpI]);
									parts[nParts - 1] -= parts[tmpI];
									ofst = ofst2;
									tmpI++;
								}
								Int64 iv;
								OSInt dx = 0;
								OSInt dy = 0;
								OSInt dz = 0;
								OSInt dm = 0;
								tmpI = nPoints;
								while (tmpI-- > 0)
								{
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, iv);
									frame->AddInt(ofst, ofst2 - ofst, CSTR("X_RAW"), (OSInt)iv);
									dx += (OSInt)iv;
									frame->AddFloat(ofst, ofst2 - ofst, CSTR("X"), OSInt2Double(dx) / tableInfo->xyScale + tableInfo->xOrigin);
									ofst = ofst2;
									ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, iv);
									frame->AddInt(ofst, ofst2 - ofst, CSTR("Y_RAW"), (OSInt)iv);
									dy += (OSInt)iv;
									frame->AddFloat(ofst, ofst2 - ofst, CSTR("Y"), OSInt2Double(dy) / tableInfo->xyScale + tableInfo->yOrigin);
									ofst = ofst2;
								}
								if (geometryType & 0x80000000)
								{
									tmpI = nPoints;
									while (tmpI-- > 0)
									{
										ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, iv);
										frame->AddInt(ofst, ofst2 - ofst, CSTR("Z_RAW"), (OSInt)iv);
										dz += (OSInt)iv;
										frame->AddFloat(ofst, ofst2 - ofst, CSTR("Z"), OSInt2Double(dz) / tableInfo->zScale + tableInfo->zOrigin);
										ofst = ofst2;
									}
								}
								if (geometryType & 0x40000000)
								{
									tmpI = nPoints;
									while (tmpI-- > 0)
									{
										ofst2 = Map::ESRI::FileGDBUtil::ReadVarInt(tagData, ofst, iv);
										frame->AddInt(ofst, ofst2 - ofst, CSTR("M_RAW"), (OSInt)iv);
										dm -= (OSInt)iv;
										frame->AddFloat(ofst, ofst2 - ofst, CSTR("M"), OSInt2Double(dm) / tableInfo->mScale + tableInfo->mOrigin);
										ofst = ofst2;
									}
								}
								if (nCurves > 0)
								{
									tmpI = 0;
									while (tmpI < nCurves)
									{
										UInt32 bits;
										UInt64 uv;
										ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, uv);
										frame->AddUInt(ofst, ofst2 - ofst, CSTR("startPointIndex"), uv);
										ofst = ofst2;
										frame->AddInt(ofst, 1, CSTR("segmentType"), tagData[ofst]);
										if (tagData[ofst] == 1) //esriSegmentArc
										{
											bits = ReadUInt32(&tagData[ofst + 17]);
											if (bits & 0x40)
											{
												frame->AddFloat(ofst + 1, 8, CSTR("startAngle"), ReadDouble(&tagData[ofst + 1]));
												frame->AddFloat(ofst + 9, 8, CSTR("centralAngle"), ReadDouble(&tagData[ofst + 9]));
											}
											else
											{
												frame->AddFloat(ofst + 1, 8, CSTR("centerPoint.x"), ReadDouble(&tagData[ofst + 1]));
												frame->AddFloat(ofst + 9, 8, CSTR("centerPoint.y"), ReadDouble(&tagData[ofst + 9]));
											}
											frame->AddHex32(ofst + 17, CSTR("Bits"), bits);
											frame->AddBit(ofst + 17, CSTR("IsEmpty"), (UInt8)(bits & 0xff), 0);
											frame->AddBit(ofst + 17, CSTR("(reserved)"), (UInt8)(bits & 0xff), 1);
											frame->AddBit(ofst + 17, CSTR("(reserved)"), (UInt8)(bits & 0xff), 2);
											frame->AddBit(ofst + 17, CSTR("IsCCW"), (UInt8)(bits & 0xff), 3);
											frame->AddBit(ofst + 17, CSTR("IsMinor"), (UInt8)(bits & 0xff), 4);
											frame->AddBit(ofst + 17, CSTR("IsLine"), (UInt8)(bits & 0xff), 5);
											frame->AddBit(ofst + 17, CSTR("IsPoint"), (UInt8)(bits & 0xff), 6);
											frame->AddBit(ofst + 17, CSTR("DefinedIP"), (UInt8)(bits & 0xff), 7);
											ofst += 21;
										}
										else if (tagData[ofst] == 2) //esriSegmentLine
										{
											// Should not exist
											break;
										}
										else if (tagData[ofst] == 3) //esriSegmentSpiral
										{
											// Unknown
											break;
										}
										else if (tagData[ofst] == 4) //esriSegmentBezier3Curve
										{
											frame->AddFloat(ofst + 1, 8, CSTR("centerPoint0.x"), ReadDouble(&tagData[ofst + 1]));
											frame->AddFloat(ofst + 9, 8, CSTR("centerPoint0.y"), ReadDouble(&tagData[ofst + 9]));
											frame->AddFloat(ofst + 17, 8, CSTR("centerPoint1.x"), ReadDouble(&tagData[ofst + 17]));
											frame->AddFloat(ofst + 25, 8, CSTR("centerPoint1.y"), ReadDouble(&tagData[ofst + 25]));
											ofst += 33;
										}
										else if (tagData[ofst] == 5) //esriSegmentEllipticArc
										{
											bits = ReadUInt32(&tagData[ofst + 41]);
											if ((bits & 0x600) == 0)
											{
												frame->AddFloat(ofst + 1, 8, CSTR("center.x"), ReadDouble(&tagData[ofst + 1]));
												frame->AddFloat(ofst + 9, 8, CSTR("center.y"), ReadDouble(&tagData[ofst + 9]));
											}
											else
											{
												frame->AddFloat(ofst + 1, 8, CSTR("fromVs"), ReadDouble(&tagData[ofst + 1]));
												frame->AddFloat(ofst + 9, 8, CSTR("deltaVs"), ReadDouble(&tagData[ofst + 9]));
											}
											if ((bits & 0x640) == 0x40)
											{
												frame->AddFloat(ofst + 17, 8, CSTR("fromV"), ReadDouble(&tagData[ofst + 17]));
											}
											else
											{
												frame->AddFloat(ofst + 17, 8, CSTR("rotation"), ReadDouble(&tagData[ofst + 17]));
											}
											frame->AddFloat(ofst + 25, 8, CSTR("semiMajor"), ReadDouble(&tagData[ofst + 25]));
											if ((bits & 0x640) == 0x40)
											{
												frame->AddFloat(ofst + 33, 8, CSTR("deltaV"), ReadDouble(&tagData[ofst + 33]));
											}
											else
											{
												frame->AddFloat(ofst + 33, 8, CSTR("minorMajorRatio"), ReadDouble(&tagData[ofst + 33]));
											}
											frame->AddHex32(ofst + 41, CSTR("Bits"), bits);
											frame->AddBit(ofst + 41, CSTR("IsEmpty"), (UInt8)(bits & 0xff), 0);
											frame->AddBit(ofst + 41, CSTR("(reserved)"), (UInt8)(bits & 0xff), 1);
											frame->AddBit(ofst + 41, CSTR("(reserved)"), (UInt8)(bits & 0xff), 2);
											frame->AddBit(ofst + 41, CSTR("(reserved)"), (UInt8)(bits & 0xff), 3);
											frame->AddBit(ofst + 41, CSTR("(reserved)"), (UInt8)(bits & 0xff), 4);
											frame->AddBit(ofst + 41, CSTR("(reserved)"), (UInt8)(bits & 0xff), 5);
											frame->AddBit(ofst + 41, CSTR("IsLine"), (UInt8)(bits & 0xff), 6);
											frame->AddBit(ofst + 41, CSTR("IsPoint"), (UInt8)(bits & 0xff), 7);
											frame->AddBit(ofst + 42, CSTR("IsCircular"), (UInt8)((bits >> 8) & 0xff), 0);
											frame->AddBit(ofst + 42, CSTR("CenterTo"), (UInt8)((bits >> 8) & 0xff), 1);
											frame->AddBit(ofst + 42, CSTR("CenterFrom"), (UInt8)((bits >> 8) & 0xff), 2);
											frame->AddBit(ofst + 42, CSTR("IsCCW"), (UInt8)((bits >> 8) & 0xff), 3);
											frame->AddBit(ofst + 42, CSTR("IsMinor"), (UInt8)((bits >> 8) & 0xff), 4);
											frame->AddBit(ofst + 42, CSTR("IsComplete"), (UInt8)((bits >> 8) & 0xff), 5);
											ofst += 45;
										}
										else
										{
											break;
										}
										tmpI++;
									}
								}
								MemFree(parts);
							}
							break;
						}
						ofst = endOfst;
					}
					else if (field->fieldType == 8) //Binary
					{
						ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
						frame->AddUInt(ofst, ofst2 - ofst, CSTR("Size"), (UOSInt)v);
						ofst = ofst2;
						frame->AddHexBuff(ofst, (UOSInt)v, field->name->ToCString(), &tagData[ofst], true);
						ofst += (UOSInt)v;						
					}
					else if (field->fieldType == 10 || field->fieldType == 11) //UUID
					{
						Data::UUID uuid(&tagData[ofst]);
						sptr = uuid.ToString(sbuff);
						frame->AddField(ofst, 16, field->name->ToCString(), CSTRP(sbuff, sptr));
						ofst += 16;
					}
					else if (field->fieldType == 12) //XML
					{
						ofst2 = Map::ESRI::FileGDBUtil::ReadVarUInt(tagData, ofst, v);
						frame->AddUInt(ofst, ofst2 - ofst, CSTR("Size"), (UOSInt)v);
						ofst = ofst2;
						frame->AddStrC(ofst, (UOSInt)v, field->name->ToCString(), &tagData[ofst]);
						ofst += (UOSInt)v;						
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
		frame->AddInt(0, 4, CSTR("Size"), ReadInt32(&tagData[0]));
		frame->AddHexBuff(4, tag->size - 4, CSTR("Deleted content"), &tagData[4], true);
	}
	return frame;
}

Bool IO::FileAnalyse::FGDBFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::FGDBFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::FGDBFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}

Text::CStringNN IO::FileAnalyse::FGDBFileAnalyse::TagTypeGetName(TagType tagType)
{
	switch (tagType)
	{
	case TagType::Header:
		return CSTR("Header");
	case TagType::Field:
		return CSTR("Field descriptor");
	case TagType::Row:
		return CSTR("Row");
	case TagType::FreeSpace:
		return CSTR("FreeSpace");
	default:
		return CSTR("Unknown");
	}
}
