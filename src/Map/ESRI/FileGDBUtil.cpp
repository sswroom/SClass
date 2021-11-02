#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Map/ESRI/FileGDBUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Map::ESRI::FileGDBTableInfo *Map::ESRI::FileGDBUtil::ParseFieldDesc(const UInt8 *fieldDesc)
{
	UTF8Char sbuff[256];
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
			/////////////////////////////////////
			FreeFieldInfo(field);
			valid = false;
			break;
		}
		else if (field->fieldType == 9)
		{
			/////////////////////////////////////
			FreeFieldInfo(field);
			valid = false;
			break;
		}

		if (field->flags & 4) //has default value
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
	*val = buff[ofst];
	return ofst + 1;
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

