#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Map/ESRI/FileGDBUtil.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/Point.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#include <stdio.h>

#define HAS_M_FLAG 4
#define HAS_Z_FLAG 2

Optional<Map::ESRI::FileGDBTableInfo> Map::ESRI::FileGDBUtil::ParseFieldDesc(Data::ByteArray fieldDesc, NN<Math::ArcGISPRJParser> prjParser)
{
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	FileGDBFieldInfo *field;
	NN<FileGDBTableInfo> table = MemAllocNN(FileGDBTableInfo);
	MemClear(table.Ptr(), sizeof(FileGDBTableInfo));
	NEW_CLASS(table->fields, Data::ArrayList<FileGDBFieldInfo*>());
	UInt32 descSize = ReadUInt32(&fieldDesc[0]);
	fieldDesc += 4;
	table->geometryType = fieldDesc[4];
	table->tableFlags = fieldDesc[5];
	table->geometryFlags = fieldDesc[7];
	UOSInt fieldCnt = ReadUInt16(&fieldDesc[8]);
	UOSInt ofst = 10;
	Bool valid = true;
	while (fieldCnt-- > 0)
	{
		if (fieldDesc[ofst] == 0 || (ofst + 1 + (UOSInt)fieldDesc[ofst] * 2) > descSize)
		{
			valid = false;
			break;
		}
		field = MemAlloc(FileGDBFieldInfo, 1);
		MemClear(field, sizeof(FileGDBFieldInfo));
		field->name = Text::String::New((const UTF16Char*)&fieldDesc[ofst + 1], fieldDesc[ofst]);
		ofst += 1 + (UOSInt)fieldDesc[ofst] * 2;
		if (fieldDesc[ofst] == 0)
		{
			ofst += 1;
		}
		else if ((ofst + 1 + (UOSInt)fieldDesc[ofst] * 2) > descSize)
		{
			FreeFieldInfo(field);
			valid = false;
			break;
		}
		else
		{
			field->alias = Text::String::New((const UTF16Char*)&fieldDesc[ofst + 1], fieldDesc[ofst]).Ptr();
			ofst += 1 + (UOSInt)fieldDesc[ofst] * 2;
		}
		field->fieldType = fieldDesc[ofst];
		if (field->fieldType == 4)
		{
			if (ofst + 6 > descSize)
			{
				FreeFieldInfo(field);
				valid = false;
				break;
			}
			field->fieldSize = ReadUInt32(&fieldDesc[ofst + 1]);
			field->flags = fieldDesc[ofst + 5];
			ofst += 6;
		}
		else
		{
			if (ofst + 3 > descSize)
			{
				FreeFieldInfo(field);
				valid = false;
				break;
			}
			field->fieldSize = fieldDesc[ofst + 1];
			field->flags = fieldDesc[ofst + 2];
			ofst += 3;
		}
		if (field->flags & 1)
		{
			table->nullableCnt++;
		}
		if (field->fieldType == 7)
		{
			UOSInt srsLen = ReadUInt16(&fieldDesc[ofst]);
			sptr = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&fieldDesc[ofst + 2], srsLen >> 1);
			*sptr = 0;
			field->srsSize = srsLen;
			field->srsValue = MemAlloc(UInt8, srsLen);
			MemCopyNO(field->srsValue, &fieldDesc[ofst + 2], srsLen);
			UOSInt csysLen = (UOSInt)(sptr - sbuff);
			table->csys = prjParser->ParsePRJBuff(CSTR("FileGDB"), sbuff, csysLen, &csysLen);
			ofst += 2 + srsLen;
			UInt8 flags = fieldDesc[ofst];
			ofst += 1;
			table->xOrigin = ReadDouble(&fieldDesc[ofst]);
			table->yOrigin = ReadDouble(&fieldDesc[ofst + 8]);
			table->xyScale = ReadDouble(&fieldDesc[ofst + 16]);
			ofst += 24;
			if (flags & HAS_M_FLAG)
			{
				table->mOrigin = ReadDouble(&fieldDesc[ofst]);
				table->mScale = ReadDouble(&fieldDesc[ofst + 8]);
				ofst += 16;
			}
			if (flags & HAS_Z_FLAG)
			{
				table->zOrigin = ReadDouble(&fieldDesc[ofst]);
				table->zScale = ReadDouble(&fieldDesc[ofst + 8]);
				ofst += 16;
			}
			table->xyTolerance = ReadDouble(&fieldDesc[ofst]);
			ofst += 8;
			if (flags & HAS_M_FLAG)
			{
				table->mTolerance = ReadDouble(&fieldDesc[ofst]);
				ofst += 8;
			}
			if (flags & HAS_Z_FLAG)
			{
				table->zTolerance = ReadDouble(&fieldDesc[ofst]);
				ofst += 8;
			}
			table->xMin = ReadDouble(&fieldDesc[ofst]);
			table->yMin = ReadDouble(&fieldDesc[ofst + 8]);
			table->xMax = ReadDouble(&fieldDesc[ofst + 16]);
			table->yMax = ReadDouble(&fieldDesc[ofst + 24]);
			ofst += 32;
			if (table->geometryFlags & 0x80)
			{
				table->zMin = ReadDouble(&fieldDesc[ofst]);
				table->zMax = ReadDouble(&fieldDesc[ofst + 8]);
				ofst += 16;
			}
			if (table->geometryFlags & 0x40)
			{
				table->mMin = ReadDouble(&fieldDesc[ofst]);
				table->mMax = ReadDouble(&fieldDesc[ofst + 8]);
				ofst += 16;
			}
			UOSInt gridCnt = ReadUInt32(&fieldDesc[ofst + 1]);
			UOSInt i = 0;
			ofst += 5;
			while (i < gridCnt)
			{
				table->spatialGrid[i] = ReadDouble(&fieldDesc[ofst]);
				ofst += 8;
				i++;
			}
		}
		else if (field->fieldType == 9)
		{
			/////////////////////////////////////
			FreeFieldInfo(field);
			valid = false;
			break;
		}

		if ((field->flags & 4) && (field->fieldType < 6)) //has default value
		{
			if (ofst >= descSize || (ofst + 1 + fieldDesc[ofst] > descSize))
			{
				FreeFieldInfo(field);
				valid = false;
				break;
			}
			field->defSize = fieldDesc[ofst];
			field->defValue = MemAlloc(UInt8, field->defSize);
			if (field->defSize)
			{
				MemCopyNO(field->defValue, &fieldDesc[ofst + 1], field->defSize);
			}
			ofst += 1 + (UOSInt)field->defSize;
		}
		table->fields->Add(field);
		if (ofst < descSize && fieldDesc[ofst] == 0)
		{
			ofst++;
		}
	}
	if (!valid || ofst > descSize)
	{
		FreeTableInfo(table);
		return 0;
	}
	return table;
}

void Map::ESRI::FileGDBUtil::FreeFieldInfo(FileGDBFieldInfo *fieldInfo)
{
	fieldInfo->name->Release();
	SDEL_STRING(fieldInfo->alias);
	if (fieldInfo->defValue)
	{
		MemFree(fieldInfo->defValue);
	}
	if (fieldInfo->srsValue)
	{
		MemFree(fieldInfo->srsValue);
	}
	MemFree(fieldInfo);
}

void Map::ESRI::FileGDBUtil::FreeTableInfo(NN<FileGDBTableInfo> tableInfo)
{
	LIST_FREE_FUNC(tableInfo->fields, FreeFieldInfo);
	DEL_CLASS(tableInfo->fields);
	SDEL_CLASS(tableInfo->csys);
	MemFreeNN(tableInfo);
}

Map::ESRI::FileGDBFieldInfo *Map::ESRI::FileGDBUtil::FieldInfoClone(FileGDBFieldInfo *field)
{
	FileGDBFieldInfo *newField = MemAlloc(FileGDBFieldInfo, 1);
	newField->name = field->name->Clone();
	newField->alias = SCOPY_STRING(field->alias);
	newField->fieldType = field->fieldType;
	newField->fieldSize = field->fieldSize;
	newField->flags = field->flags;
	newField->defSize = field->defSize;
	newField->defValue = 0;
	if (field->defValue)
	{
		newField->defValue = MemAlloc(UInt8, field->defSize);
		MemCopyNO(newField->defValue, field->defValue, field->defSize);
	}
	newField->srsSize = field->srsSize;
	newField->srsValue = 0;
	if (field->srsValue)
	{
		newField->srsValue = MemAlloc(UInt8, field->srsSize);
		MemCopyNO(newField->srsValue, field->srsValue, field->srsSize);
	}
	return newField;
}

NN<Map::ESRI::FileGDBTableInfo> Map::ESRI::FileGDBUtil::TableInfoClone(NN<FileGDBTableInfo> tableInfo)
{
	NN<FileGDBTableInfo> newTable = MemAllocNN(FileGDBTableInfo);
	MemCopyNO(newTable.Ptr(), tableInfo.Ptr(), sizeof(FileGDBTableInfo));
	if (tableInfo->csys)
	{
		newTable->csys = tableInfo->csys->Clone().Ptr();
	}
	NEW_CLASS(newTable->fields, Data::ArrayList<FileGDBFieldInfo*>());
	UOSInt i = 0;
	UOSInt j = tableInfo->fields->GetCount();
	while (i < j)
	{
		newTable->fields->Add(FieldInfoClone(tableInfo->fields->GetItem(i)));
		i++;
	}
	return newTable;
}

UOSInt Map::ESRI::FileGDBUtil::ReadVarUInt(const UInt8 *buff, UOSInt ofst, OutParam<UInt64> val)
{
	UInt64 v = 0;
	UOSInt i = 0;
	UOSInt currV;
	while (true)
	{
		currV = buff[ofst];
		ofst++;
		v = v | ((currV & 0x7F) << i);
		if ((currV & 0x80) == 0)
		{
			break;
		}
		i += 7;
	}
	val.Set(v);
	return ofst;
}

UOSInt Map::ESRI::FileGDBUtil::ReadVarInt(const UInt8 *buff, UOSInt ofst, OutParam<Int64> val)
{
	Bool sign = (buff[ofst] & 0x40) != 0;
	Int64 v = 0;
	UOSInt i = 0;
	Int64 currV;
	currV = buff[ofst];
	ofst++;
	i = 6;
	v = currV & 0x3f;
	while (currV & 0x80)
	{
		currV = buff[ofst];
		ofst++;
		v = v | ((currV & 0x7F) << i);
		i += 7;
	}
	if (sign)
	{
		val.Set(-v);
	}
	else
	{
		val.Set(v);
	}
	return ofst;
}

UOSInt Map::ESRI::FileGDBUtil::ReadVarUInt(Data::ByteArrayR buff, UOSInt ofst, OutParam<UInt64> val)
{
	UInt64 v = 0;
	UOSInt i = 0;
	UOSInt currV;
	while (true)
	{
		currV = buff[ofst];
		ofst++;
		v = v | ((currV & 0x7F) << i);
		if ((currV & 0x80) == 0)
		{
			break;
		}
		i += 7;
	}
	val.Set(v);
	return ofst;
}

UOSInt Map::ESRI::FileGDBUtil::ReadVarInt(Data::ByteArrayR buff, UOSInt ofst, OutParam<Int64> val)
{
	Bool sign = (buff[ofst] & 0x40) != 0;
	Int64 v = 0;
	UOSInt i = 0;
	Int64 currV;
	currV = buff[ofst];
	ofst++;
	i = 6;
	v = currV & 0x3f;
	while (currV & 0x80)
	{
		currV = buff[ofst];
		ofst++;
		v = v | ((currV & 0x7F) << i);
		i += 7;
	}
	if (sign)
	{
		val.Set(-v);
	}
	else
	{
		val.Set(v);
	}
	return ofst;
}

Optional<Math::Geometry::Vector2D> Map::ESRI::FileGDBUtil::ParseSDERecord(Data::ByteArrayR buff)
{
	UInt32 recSize = buff.ReadU32(0);
	if (recSize != buff.GetSize())
		return 0;
	UInt32 nPoints = buff.ReadU32(4);
	UInt16 type1 = buff.ReadU16(8);
	UInt16 type2 = buff.ReadU16(10);
	UInt32 srid = buff.ReadU32(12);
	UInt32 pointLen = buff.ReadU32(16);
	UInt32 nParts = buff.ReadU32(20);
	UOSInt ofst = 24;
	Int64 iv;
	Double ratio = 1 / 1000000000.0;
	Double dx = -400;
	Double dy = -400;
	if (ReadVarInt(buff, 24 + pointLen, iv) != INVALID_INDEX)
	{
		if (iv != 0)
		{
			if (iv >= 1000 || iv <= -1000)
			{
				ratio = 1 / (Double)iv;
			}
		}
	}
	if (srid == 2326)
	{
		dx = 0;
		dy = 0;
		printf("FileGDBUtil.ParseSDERecord: ratio = %lf\r\n", 1 / ratio);
	}
	
	if (nParts != 1)
	{
		printf("FileGDBUtil.ParseSDERecord: unknown nParts value: %d\r\n", (UInt32)nParts);
	}
	if (type1 == 1 && type2 == 4 && nPoints == 1)
	{
		NN<Math::Geometry::Point> pt;
		ofst = ReadVarInt(buff, ofst, iv);
		dx += (Double)iv * ratio;
		ofst = ReadVarInt(buff, ofst, iv);
		dy += (Double)iv * ratio;
		NEW_CLASSNN(pt, Math::Geometry::Point(srid, Math::Coord2DDbl(dx, dy)));
		if (ofst != pointLen + 24)
		{
			pt.Delete();
			printf("FileGDBUtil.ParseSDERecord: pointLen not valid: %d, actual = %d\r\n", (UInt32)pointLen, (UInt32)ofst - 24);
			return 0;
		}
		return pt;
	}
	else if (type1 == 4 && type2 == 12)
	{
		UOSInt tmpV;
		NN<Math::Geometry::LineString> lineString;
		NEW_CLASSNN(lineString, Math::Geometry::LineString(srid, nPoints, false, false));
		Math::Coord2DDbl *ptList = lineString->GetPointList(tmpV);
		UOSInt i = 0;
		while (i < nPoints)
		{
			ofst = ReadVarInt(buff, ofst, iv);
			dx += (Double)iv * ratio;
			ofst = ReadVarInt(buff, ofst, iv);
			dy += (Double)iv * ratio;
			ptList[i] = Math::Coord2DDbl(dx, dy);
			i++;
		}
		if (ofst != pointLen + 24)
		{
			lineString.Delete();
			printf("FileGDBUtil.ParseSDERecord: pointLen not valid: %d, actual = %d\r\n", (UInt32)pointLen, (UInt32)ofst - 24);
			return 0;
		}
		return lineString;
	}
	else
	{
		printf("FileGDBUtil.ParseSDERecord: Record type is not supported: %d, %d\r\n", type1, type2);
		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(buff, ' ', Text::LineBreakType::CRLF);
		printf("%s\r\n", sb.v);
		return 0;
	}
}

Text::CStringNN Map::ESRI::FileGDBUtil::GeometryTypeGetName(UInt8 t)
{
	switch (t)
	{
	case 0:
		return CSTR("None");
	case 1:
		return CSTR("Point");
	case 2:
		return CSTR("Multipoint");
	case 3:
		return CSTR("Polyline");
	case 4:
		return CSTR("Polygon");
	case 5:
		return CSTR("Rectangle");
	case 6:
		return CSTR("Path");
	case 7:
		return CSTR("Mixed");
	case 9:
		return CSTR("Multipath");
	case 11:
		return CSTR("Ring");
	case 13:
		return CSTR("Line");
	case 14:
		return CSTR("Circular Arc");
	case 15:
		return CSTR("Bezier Curves");
	case 16:
		return CSTR("Elliptic Curves");
	case 17:
		return CSTR("Geometry Collection");
	case 18:
		return CSTR("Triangle Strip");
	case 19:
		return CSTR("Triangle Fan");
	case 20:
		return CSTR("Ray");
	case 21:
		return CSTR("Sphere");
	case 22:
		return CSTR("TIN");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Map::ESRI::FileGDBUtil::FieldTypeGetName(UInt8 t)
{
	switch (t)
	{
	case 0:
		return CSTR("Int16");
	case 1:
		return CSTR("Int32");
	case 2:
		return CSTR("Float32");
	case 3:
		return CSTR("Float64");
	case 4:
		return CSTR("String");
	case 5:
		return CSTR("Datetime");
	case 6:
		return CSTR("ObjectId");
	case 7:
		return CSTR("Geometry");
	case 8:
		return CSTR("Binary");
	case 9:
		return CSTR("Raster");
	case 10:
		return CSTR("UUID");
	case 11:
		return CSTR("UUID");
	case 12:
		return CSTR("XML");
	default:
		return CSTR("Unknown");
	}
}

