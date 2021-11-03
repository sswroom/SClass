#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Map/ESRI/FileGDBUtil.h"
#include "Math/CoordinateSystemManager.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#define HAS_M_FLAG 4
#define HAS_Z_FLAG 2

Map::ESRI::FileGDBTableInfo *Map::ESRI::FileGDBUtil::ParseFieldDesc(const UInt8 *fieldDesc)
{
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	FileGDBFieldInfo *field;
	FileGDBTableInfo *table = MemAlloc(FileGDBTableInfo, 1);
	MemClear(table, sizeof(FileGDBTableInfo));
	NEW_CLASS(table->fields, Data::ArrayList<FileGDBFieldInfo*>());
	UInt32 descSize = ReadUInt32(fieldDesc);
	fieldDesc += 4;
	table->geometryType = fieldDesc[4];
	table->tableFlags = fieldDesc[5];
	table->geometryFlags = fieldDesc[7];
	UOSInt fieldCnt = ReadUInt16(&fieldDesc[8]);
	UOSInt ofst = 10;
	Bool valid = true;
	while (fieldCnt-- > 0)
	{
		if (fieldDesc[ofst] == 0 || (ofst + 1 + fieldDesc[ofst] * 2) > descSize)
		{
			valid = false;
			break;
		}
		field = MemAlloc(FileGDBFieldInfo, 1);
		MemClear(field, sizeof(FileGDBFieldInfo));
		*Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&fieldDesc[ofst + 1], fieldDesc[ofst]) = 0;
		field->name = Text::StrCopyNew(sbuff);
		ofst += 1 + (UOSInt)fieldDesc[ofst] * 2;
		if (fieldDesc[ofst] == 0)
		{
			ofst += 1;
		}
		else if ((ofst + 1 + fieldDesc[ofst] * 2) > descSize)
		{
			FreeFieldInfo(field);
			valid = false;
			break;
		}
		else
		{
			*Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&fieldDesc[ofst + 1], fieldDesc[ofst]) = 0;
			field->alias = Text::StrCopyNew(sbuff);
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
			UOSInt csysLen = (UOSInt)(sptr - sbuff);
			table->csys = Math::CoordinateSystemManager::ParsePRJBuff((const UTF8Char *)"FileGDB", (Char*)sbuff, &csysLen);
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

		if ((field->flags & 4) && (field->fieldType < 7)) //has default value
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
			ofst += 1 + field->defSize;
		}
		table->fields->Add(field);
		if (fieldDesc[ofst] == 0 && ofst < descSize)
		{
			ofst++;
		}
	}
	if (!valid || ofst != descSize)
	{
		FreeTableInfo(table);
		return 0;
	}
	return table;
}

void Map::ESRI::FileGDBUtil::FreeFieldInfo(FileGDBFieldInfo *fieldInfo)
{
	SDEL_TEXT(fieldInfo->name);
	SDEL_TEXT(fieldInfo->alias);
	if (fieldInfo->defValue)
	{
		MemFree(fieldInfo->defValue);
	}
	MemFree(fieldInfo);
}

void Map::ESRI::FileGDBUtil::FreeTableInfo(FileGDBTableInfo *tableInfo)
{
	LIST_FREE_FUNC(tableInfo->fields, FreeFieldInfo);
	DEL_CLASS(tableInfo->fields);
	MemFree(tableInfo);
}

UOSInt Map::ESRI::FileGDBUtil::ReadVarUInt(const UInt8 *buff, UOSInt ofst, UOSInt *val)
{
	UOSInt v = 0;
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
	*val = v;
	return ofst;
}

const UTF8Char *Map::ESRI::FileGDBUtil::GeometryTypeGetName(UInt8 t)
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

const UTF8Char *Map::ESRI::FileGDBUtil::FieldTypeGetName(UInt8 t)
{
	switch (t)
	{
	case 0:
		return (const UTF8Char*)"Int16";
	case 1:
		return (const UTF8Char*)"Int32";
	case 2:
		return (const UTF8Char*)"Float32";
	case 3:
		return (const UTF8Char*)"Float64";
	case 4:
		return (const UTF8Char*)"String";
	case 5:
		return (const UTF8Char*)"Datetime";
	case 6:
		return (const UTF8Char*)"ObjectId";
	case 7:
		return (const UTF8Char*)"Geometry";
	case 8:
		return (const UTF8Char*)"Binary";
	case 9:
		return (const UTF8Char*)"Raster";
	case 10:
		return (const UTF8Char*)"UUID";
	case 11:
		return (const UTF8Char*)"UUID";
	case 12:
		return (const UTF8Char*)"XML";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

