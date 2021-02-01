#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "Map/FileGDB.h"
#include "Map/SHPData.h"
#include "Math/Point3D.h"
#include "Text/MyString.h"

#if defined(__APPLE__)

Map::FileGDB::FileGDB(const UTF8Char *filePath) : DB::ReadingDB(filePath)
{
}

Map::FileGDB::~FileGDB()
{
}

Bool Map::FileGDB::IsError()
{
	return true;
}

UOSInt Map::FileGDB::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	return 0;
}

DB::DBReader *Map::FileGDB::GetTableData(const UTF8Char *name)
{
	return 0;
}

void Map::FileGDB::CloseReader(DB::DBReader *r)
{
}

void Map::FileGDB::GetErrorMsg(Text::StringBuilderUTF *str)
{
}

void Map::FileGDB::Reconnect()
{
}

#else
#if defined(__MINGW32__)
#define __linux__
#endif
#include <FileGDBAPI.h>

Map::FileGDB::FileGDB(const UTF8Char *filePath) : DB::ReadingDB(filePath)
{
	FileGDBAPI::Geodatabase *geodatabase;
	fgdbError hr;
	NEW_CLASS(this->tables, Data::ArrayListStrUTF8());
	NEW_CLASS(this->features, Data::ArrayListStrUTF8());
	this->wPath = Text::StrToWCharNew(filePath);

	geodatabase = new FileGDBAPI::Geodatabase();
	hr = FileGDBAPI::OpenGeodatabase(this->wPath, *geodatabase);
	if (hr != S_OK)
	{
		delete geodatabase;
		this->gdb = 0;
	}
	else
	{
		OSInt i;
		OSInt j;
		std::wstring wptr;
		this->gdb = geodatabase;
		std::vector<std::wstring> dsets;
		dsets.clear();
		geodatabase->GetChildDatasets(L"\\", L"Table", dsets);
		i = 0;
		j = dsets.size();
		while (i < j)
		{
			wptr = dsets.at(i);
			if (wptr[0] == '\\')
			{
				this->tables->SortedInsert(Text::StrToUTF8New(&wptr[1]));
			}

			i++;
		}

		dsets.clear();
		geodatabase->GetChildDatasets(L"\\", L"Feature Class", dsets);
		i = 0;
		j = dsets.size();
		while (i < j)
		{
			wptr = dsets.at(i);
			if (wptr[0] == '\\')
			{
				this->features->SortedInsert(Text::StrToUTF8New(&wptr[1]));
			}

			i++;
		}
	}
}

Map::FileGDB::~FileGDB()
{
	if (this->gdb)
	{
		FileGDBAPI::Geodatabase *geodatabase = (FileGDBAPI::Geodatabase*)this->gdb;
		FileGDBAPI::CloseGeodatabase(*geodatabase);
		delete geodatabase;
		this->gdb = 0;
	}
	OSInt i;
	i = this->tables->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->tables->GetItem(i));
	}
	DEL_CLASS(this->tables);
	Text::StrDelNew(this->wPath);
}

Bool Map::FileGDB::IsError()
{
	return this->gdb == 0;
}

UOSInt Map::FileGDB::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	names->AddRange(this->tables);
	names->AddRange(this->features);
	return this->tables->GetCount() + this->features->GetCount();
}

DB::DBReader *Map::FileGDB::GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition)
{
	WChar sbuff[256];
	fgdbError hr;
	FileGDBAPI::Geodatabase *geodatabase;
	geodatabase = (FileGDBAPI::Geodatabase*)this->gdb;
	sbuff[0] = '\\';
	Text::StrUTF8_WChar(&sbuff[1], name, -1, 0);

	FileGDBAPI::Table *table;
	table = new FileGDBAPI::Table();
	hr = geodatabase->OpenTable(sbuff, *table);
	if (hr != S_OK)
	{
		delete table;
		return 0;
	}

	FileGDBAPI::EnumRows *rows = new FileGDBAPI::EnumRows();
	hr = table->Search(L"*", L"", true, *rows);
	if (hr != S_OK)
	{
		geodatabase->CloseTable(*table);
		delete table;
		return 0;
	}

	Map::FileGDBReader *reader;
	NEW_CLASS(reader, Map::FileGDBReader(geodatabase, table, rows));
	return reader;
}

void Map::FileGDB::CloseReader(DB::DBReader *r)
{
	if (r)
	{
		Map::FileGDBReader *reader = (Map::FileGDBReader*)r;
		DEL_CLASS(reader);
	}
}

void Map::FileGDB::GetErrorMsg(Text::StringBuilderUTF *str)
{
}

void Map::FileGDB::Reconnect()
{
	FileGDBAPI::Geodatabase *geodatabase;
	fgdbError hr;

	if (this->gdb)
	{
		geodatabase = (FileGDBAPI::Geodatabase*)this->gdb;
		FileGDBAPI::CloseGeodatabase(*geodatabase);
		delete geodatabase;
		this->gdb = 0;
	}

	geodatabase = new FileGDBAPI::Geodatabase();
	hr = FileGDBAPI::OpenGeodatabase(this->wPath, *geodatabase);
	if (hr != S_OK)
	{
		delete geodatabase;
		this->gdb = 0;
	}
	else
	{
		this->gdb = geodatabase;
	}
}

Map::FileGDBReader::FileGDBReader(void *gdb, void *table, void *rows)
{
	this->gdb = gdb;
	this->table = table;
	this->rows = rows;
	FileGDBAPI::Row *row = new FileGDBAPI::Row();
	this->currRow = row;

	FileGDBAPI::Table *tab = (FileGDBAPI::Table*)this->table;
	fgdbError hr;
	std::vector<FileGDBAPI::FieldDef> *fields = new std::vector<FileGDBAPI::FieldDef>();
	hr = tab->GetFields(*fields);
	this->fieldDefs = fields;
}

Map::FileGDBReader::~FileGDBReader()
{
	FileGDBAPI::Geodatabase *geodatabase = (FileGDBAPI::Geodatabase*)this->gdb;
	FileGDBAPI::Table *tab = (FileGDBAPI::Table*)this->table;
	FileGDBAPI::EnumRows *erows = (FileGDBAPI::EnumRows*)this->rows;
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	std::vector<FileGDBAPI::FieldDef> *fields = (std::vector<FileGDBAPI::FieldDef> *)this->fieldDefs;

	delete fields;
	erows->Close();
	delete erows;
	delete row;
	geodatabase->CloseTable(*tab);
	delete tab;
}

Bool Map::FileGDBReader::ReadNext()
{
	FileGDBAPI::EnumRows *erows = (FileGDBAPI::EnumRows*)this->rows;
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	fgdbError hr;
	hr = erows->Next(*row);
	return hr == S_OK;
}

OSInt Map::FileGDBReader::ColCount()
{
	std::vector<FileGDBAPI::FieldDef> *fields = (std::vector<FileGDBAPI::FieldDef> *)this->fieldDefs;
	return fields->size();
}

Int32 Map::FileGDBReader::GetInt32(OSInt colIndex)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	std::vector<FileGDBAPI::FieldDef> *fields = (std::vector<FileGDBAPI::FieldDef> *)this->fieldDefs;
	fgdbError hr;
	if (colIndex < 0 || (UOSInt)colIndex >= fields->size())
		return 0;
	FileGDBAPI::FieldDef field = fields->at(colIndex);
	FileGDBAPI::FieldType ftype;
	hr = field.GetType(ftype);
	if (hr != S_OK)
	{
		return 0;
	}
	if (ftype == FileGDBAPI::fieldTypeOID)
	{
		Int32 v;
		hr = row->GetOID(v);
		if (hr != S_OK)
		{
			return 0;
		}
		return v;
	}
	else
	{
		Int32 v;
		hr = row->GetInteger((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return v;
	}
}

Int64 Map::FileGDBReader::GetInt64(OSInt colIndex)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	Int32 v;
	fgdbError hr = row->GetInteger((int)colIndex, v);
	if (hr != S_OK)
	{
		return 0;
	}
	return v;
}

WChar *Map::FileGDBReader::GetStr(OSInt colIndex, WChar *buff)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	std::vector<FileGDBAPI::FieldDef> *fields = (std::vector<FileGDBAPI::FieldDef> *)this->fieldDefs;
	fgdbError hr;
	if (colIndex < 0 || (UOSInt)colIndex >= fields->size())
		return 0;
	FileGDBAPI::FieldDef field = fields->at(colIndex);
	FileGDBAPI::FieldType ftype;
	hr = field.GetType(ftype);
	if (hr != S_OK)
	{
		return 0;
	}
	switch (ftype)
	{
	case FileGDBAPI::fieldTypeSmallInteger:
	{
		Int16 v;
		hr = row->GetShort((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrInt16(buff, v);
	}
	case FileGDBAPI::fieldTypeInteger:
	{
		Int32 v;
		hr = row->GetInteger((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrInt32(buff, v);
	}
	case FileGDBAPI::fieldTypeSingle:
	{
		Single v;
		hr = row->GetFloat((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrDouble(buff, v);
	}
	case FileGDBAPI::fieldTypeDouble:
	{
		Double v;
		hr = row->GetDouble((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrDouble(buff, v);
	}
	case FileGDBAPI::fieldTypeString:
	{
		std::wstring v;
		hr = row->GetString((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrConcat(buff, v.data());
	}
	case FileGDBAPI::fieldTypeDate:
	{
		tm v;
		hr = row->GetDate((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		Data::DateTime dt;
		dt.SetValue(v.tm_year, v.tm_mon, v.tm_mday, v.tm_hour, v.tm_min, v.tm_sec, 0);
		return dt.ToString(buff);
	}
	case FileGDBAPI::fieldTypeOID:
	{
		Int32 v;
		hr = row->GetOID(v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrInt32(buff, v);
	}
	case FileGDBAPI::fieldTypeGeometry:
	{
		FileGDBAPI::ShapeBuffer v;
		hr = row->GetGeometry(v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrHexBytes(buff, v.shapeBuffer, v.inUseLength, 0);
	}
	case FileGDBAPI::fieldTypeBlob:
	{
		FileGDBAPI::ByteArray v;
		hr = row->GetBinary((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrHexBytes(buff, v.byteArray, v.inUseLength, 0);
	}
	case FileGDBAPI::fieldTypeRaster:
		return 0;
	case FileGDBAPI::fieldTypeGUID:
		return 0;
	case FileGDBAPI::fieldTypeGlobalID:
		return 0;
	case FileGDBAPI::fieldTypeXML:
	{
		std::string v;
		hr = row->GetXML((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrUTF8_WChar(buff, (const UTF8Char*)v.data(), -1, 0);
	}

	}
	return 0;
}

Bool Map::FileGDBReader::GetStr(OSInt colIndex, Text::StringBuilderUTF *sb)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	std::vector<FileGDBAPI::FieldDef> *fields = (std::vector<FileGDBAPI::FieldDef> *)this->fieldDefs;
	fgdbError hr;
	if (colIndex < 0 || (UOSInt)colIndex >= fields->size())
		return false;
	FileGDBAPI::FieldDef field = fields->at(colIndex);
	FileGDBAPI::FieldType ftype;
	hr = field.GetType(ftype);
	if (hr != S_OK)
	{
		return false;
	}
	switch (ftype)
	{
	case FileGDBAPI::fieldTypeSmallInteger:
	{
		Int16 v;
		hr = row->GetShort((int)colIndex, v);
		if (hr != S_OK)
		{
			return false;
		}
		sb->Append(v);
		return true;
	}
	case FileGDBAPI::fieldTypeInteger:
	{
		Int32 v;
		hr = row->GetInteger((int)colIndex, v);
		if (hr != S_OK)
		{
			return false;
		}
		sb->Append(v);
		return true;
	}
	case FileGDBAPI::fieldTypeSingle:
	{
		Single v;
		hr = row->GetFloat((int)colIndex, v);
		if (hr != S_OK)
		{
			return false;
		}
		sb->Append(v);
		return true;
	}
	case FileGDBAPI::fieldTypeDouble:
	{
		Double v;
		hr = row->GetDouble((int)colIndex, v);
		if (hr != S_OK)
		{
			return false;
		}
		sb->Append(v);
		return true;
	}
	case FileGDBAPI::fieldTypeString:
	{
		std::wstring v;
		hr = row->GetString((int)colIndex, v);
		if (hr != S_OK)
		{
			return false;
		}
		sb->AppendW(v.data());
		return true;
	}
	case FileGDBAPI::fieldTypeDate:
	{
		tm v;
		hr = row->GetDate((int)colIndex, v);
		if (hr != S_OK)
		{
			return false;
		}
		Data::DateTime dt;
		dt.SetValue(v.tm_year, v.tm_mon, v.tm_mday, v.tm_hour, v.tm_min, v.tm_sec, 0);
		sb->Append(&dt);
		return true;
	}
	case FileGDBAPI::fieldTypeOID:
	{
		Int32 v;
		hr = row->GetOID(v);
		if (hr != S_OK)
		{
			return false;
		}
		sb->Append(v);
		return true;
	}
	case FileGDBAPI::fieldTypeGeometry:
	{
		FileGDBAPI::ShapeBuffer v;
		hr = row->GetGeometry(v);
		if (hr != S_OK)
		{
			return false;
		}
//		sb->AppendHexBuff(v.shapeBuffer, v.inUseLength, 0, Text::LBT_NONE);
		return true;
	}
	case FileGDBAPI::fieldTypeBlob:
	{
		FileGDBAPI::ByteArray v;
		hr = row->GetBinary((int)colIndex, v);
		if (hr != S_OK)
		{
			return false;
		}
		sb->AppendHexBuff(v.byteArray, v.inUseLength, 0, Text::LBT_NONE);
		return true;
	}
	case FileGDBAPI::fieldTypeRaster:
		return false;
	case FileGDBAPI::fieldTypeGUID:
		return false;
	case FileGDBAPI::fieldTypeGlobalID:
		return false;
	case FileGDBAPI::fieldTypeXML:
	{
		std::string v;
		hr = row->GetXML((int)colIndex, v);
		if (hr != S_OK)
		{
			return false;
		}
		sb->Append((const UTF8Char*)v.data());
		return true;
	}

	}
	return false;
}

const UTF8Char *Map::FileGDBReader::GetNewStr(OSInt colIndex)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	std::vector<FileGDBAPI::FieldDef> *fields = (std::vector<FileGDBAPI::FieldDef> *)this->fieldDefs;
	fgdbError hr;
	UTF8Char sbuff[40];
	if (colIndex < 0 || (UOSInt)colIndex >= fields->size())
		return 0;
	FileGDBAPI::FieldDef field = fields->at(colIndex);
	FileGDBAPI::FieldType ftype;
	hr = field.GetType(ftype);
	if (hr != S_OK)
	{
		return 0;
	}
	switch (ftype)
	{
	case FileGDBAPI::fieldTypeSmallInteger:
	{
		Int16 v;
		hr = row->GetShort((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		Text::StrInt16(sbuff, v);
		return Text::StrCopyNew(sbuff);
	}
	case FileGDBAPI::fieldTypeInteger:
	{
		Int32 v;
		hr = row->GetInteger((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		Text::StrInt32(sbuff, v);
		return Text::StrCopyNew(sbuff);
	}
	case FileGDBAPI::fieldTypeSingle:
	{
		Single v;
		hr = row->GetFloat((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		Text::StrDouble(sbuff, v);
		return Text::StrCopyNew(sbuff);
	}
	case FileGDBAPI::fieldTypeDouble:
	{
		Double v;
		hr = row->GetDouble((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		Text::StrDouble(sbuff, v);
		return Text::StrCopyNew(sbuff);
	}
	case FileGDBAPI::fieldTypeString:
	{
		std::wstring v;
		hr = row->GetString((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrCopyNew(v.data());
	}
	case FileGDBAPI::fieldTypeDate:
	{
		tm v;
		hr = row->GetDate((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		Data::DateTime dt;
		dt.SetValue(v.tm_year, v.tm_mon, v.tm_mday, v.tm_hour, v.tm_min, v.tm_sec, 0);
		dt.ToString(sbuff);
		return Text::StrCopyNew(sbuff);
	}
	case FileGDBAPI::fieldTypeOID:
	{
		Int32 v;
		hr = row->GetOID(v);
		if (hr != S_OK)
		{
			return 0;
		}
		Text::StrInt32(sbuff, v);
		return Text::StrCopyNew(sbuff);
	}
	case FileGDBAPI::fieldTypeGeometry:
		return 0;
	case FileGDBAPI::fieldTypeBlob:
		return 0;
	case FileGDBAPI::fieldTypeRaster:
		return 0;
	case FileGDBAPI::fieldTypeGUID:
		return 0;
	case FileGDBAPI::fieldTypeGlobalID:
		return 0;
	case FileGDBAPI::fieldTypeXML:
	{
		std::string v;
		hr = row->GetXML((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrCopyNew((const UTF8Char*)v.data());
	}

	}
	return 0;
}

UTF8Char *Map::FileGDBReader::GetStr(OSInt colIndex, UTF8Char *buff)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	std::vector<FileGDBAPI::FieldDef> *fields = (std::vector<FileGDBAPI::FieldDef> *)this->fieldDefs;
	fgdbError hr;
	if (colIndex < 0 || (UOSInt)colIndex >= fields->size())
		return 0;
	FileGDBAPI::FieldDef field = fields->at(colIndex);
	FileGDBAPI::FieldType ftype;
	hr = field.GetType(ftype);
	if (hr != S_OK)
	{
		return 0;
	}
	switch (ftype)
	{
	case FileGDBAPI::fieldTypeSmallInteger:
	{
		Int16 v;
		hr = row->GetShort((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrInt16(buff, v);
	}
	case FileGDBAPI::fieldTypeInteger:
	{
		Int32 v;
		hr = row->GetInteger((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrInt32(buff, v);
	}
	case FileGDBAPI::fieldTypeSingle:
	{
		Single v;
		hr = row->GetFloat((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrDouble(buff, v);
	}
	case FileGDBAPI::fieldTypeDouble:
	{
		Double v;
		hr = row->GetDouble((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrDouble(buff, v);
	}
	case FileGDBAPI::fieldTypeString:
	{
		std::wstring v;
		hr = row->GetString((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrWChar_UTF8(buff, v.data(), -1);
	}
	case FileGDBAPI::fieldTypeDate:
	{
		tm v;
		hr = row->GetDate((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		Data::DateTime dt;
		dt.SetValue(v.tm_year, v.tm_mon, v.tm_mday, v.tm_hour, v.tm_min, v.tm_sec, 0);
		return dt.ToString(buff);
	}
	case FileGDBAPI::fieldTypeOID:
	{
		Int32 v;
		hr = row->GetOID(v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrInt32(buff, v);
	}
	case FileGDBAPI::fieldTypeGeometry:
	{
		FileGDBAPI::ShapeBuffer v;
		hr = row->GetGeometry(v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrHexBytes(buff, v.shapeBuffer, v.inUseLength, 0);
	}
	case FileGDBAPI::fieldTypeBlob:
	{
		FileGDBAPI::ByteArray v;
		hr = row->GetBinary((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrHexBytes(buff, v.byteArray, v.inUseLength, 0);
	}
	case FileGDBAPI::fieldTypeRaster:
		return 0;
	case FileGDBAPI::fieldTypeGUID:
		return 0;
	case FileGDBAPI::fieldTypeGlobalID:
		return 0;
	case FileGDBAPI::fieldTypeXML:
	{
		std::string v;
		hr = row->GetXML((int)colIndex, v);
		if (hr != S_OK)
		{
			return 0;
		}
		return Text::StrConcat(buff, (const UTF8Char*)v.data());
	}

	}
	return 0;
}

DB::DBReader::DateErrType Map::FileGDBReader::GetDate(OSInt colIndex, Data::DateTime *outVal)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	fgdbError hr;
	Bool isNull;
	hr = row->IsNull((int)colIndex, isNull);
	if (hr != S_OK)
		return DB::DBReader::DET_ERROR;
	if (isNull)
		return DB::DBReader::DET_NULL;
	tm v;
	hr = row->GetDate((int)colIndex, v);
	if (hr != S_OK)
	{
		return DB::DBReader::DET_ERROR;
	}
	outVal->SetValue(v.tm_year, v.tm_mon, v.tm_mday, v.tm_hour, v.tm_min, v.tm_sec, 0);
	return DB::DBReader::DET_OK;
}

Double Map::FileGDBReader::GetDbl(OSInt colIndex)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	Double v;
	fgdbError hr = row->GetDouble((int)colIndex, v);
	if (hr != S_OK)
	{
		return 0;
	}
	return v;
}

Bool Map::FileGDBReader::GetBool(OSInt colIndex)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	Int32 v;
	fgdbError hr = row->GetInteger((int)colIndex, v);
	if (hr != S_OK)
	{
		return 0;
	}
	return v != 0;
}

OSInt Map::FileGDBReader::GetBinarySize(OSInt colIndex)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	FileGDBAPI::ByteArray v;
	fgdbError hr = row->GetBinary((int)colIndex, v);
	if (hr != S_OK)
	{
		return 0;
	}
	return v.inUseLength;
}

OSInt Map::FileGDBReader::GetBinary(OSInt colIndex, UInt8 *buff)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	FileGDBAPI::ByteArray v;
	fgdbError hr = row->GetBinary((int)colIndex, v);
	if (hr != S_OK)
	{
		return 0;
	}
	MemCopyNO(buff, v.byteArray, v.inUseLength);
	return v.inUseLength;
}

Math::Vector2D *Map::FileGDBReader::GetVector(OSInt colIndex)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	std::vector<FileGDBAPI::FieldDef> *fields = (std::vector<FileGDBAPI::FieldDef> *)this->fieldDefs;
	fgdbError hr;
	if (colIndex < 0 || (UOSInt)colIndex >= fields->size())
		return 0;
	FileGDBAPI::FieldDef field = fields->at(colIndex);
	FileGDBAPI::FieldType ftype;
	hr = field.GetType(ftype);
	if (hr != S_OK)
	{
		return 0;
	}
	if (ftype != FileGDBAPI::fieldTypeGeometry)
	{
		return 0;
	}
	FileGDBAPI::GeometryDef geomDef;
	hr = field.GetGeometryDef(geomDef);
	if (hr != S_OK)
	{
		return 0;
	}

	FileGDBAPI::GeometryType geomType;
	hr = geomDef.GetGeometryType(geomType);
	if (hr != S_OK)
	{
		return 0;
	}

	switch (geomType)
	{
	case FileGDBAPI::geometryPoint:
	{
		FileGDBAPI::PointShapeBuffer v;
		fgdbError hr = row->GetGeometry(v);
		if (hr != S_OK)
		{
			return 0;
		}
		FileGDBAPI::ShapeType shpType;
		hr = v.GetShapeType(shpType);
		if (hr != S_OK)
		{
			return 0;
		}
		if (v.HasZs(shpType))
		{
			FileGDBAPI::Point *p;
			Math::Point3D *pt;
			Double *z;
			if (v.GetPoint(p) == S_OK && v.GetZ(z) == S_OK)
			{
				NEW_CLASS(pt, Math::Point3D(p->x, p->y, z[0]));
				return pt;
			}
		}
		else
		{
			FileGDBAPI::Point *p;
			Math::Point *pt;
			if (v.GetPoint(p) == S_OK)
			{
				NEW_CLASS(pt, Math::Point(p->x, p->y));
				return pt;
			}
		}
		return 0;
	}
	case FileGDBAPI::geometryPolygon:
	{
		FileGDBAPI::MultiPartShapeBuffer v;
		fgdbError hr = row->GetGeometry(v);
		if (hr != S_OK)
		{
			return 0;
		}
		FileGDBAPI::ShapeType shpType;
		Int32 nParts;
		Int32 nPoints;
		Int32 *parts;
		FileGDBAPI::Point *points;
		if (v.GetShapeType(shpType) != S_OK)
			return 0;
		if (v.GetNumParts(nParts) != S_OK)
			return 0;
		if (v.GetNumPoints(nPoints) != S_OK)
			return 0;
		if (v.GetParts(parts) != S_OK)
			return 0;
		if (v.GetPoints(points) != S_OK)
			return 0;

		Math::Polygon *pg;
		Int32 *parts2;
		Double *points2;
		OSInt tmpV;
		NEW_CLASS(pg, Math::Polygon(nParts, nPoints));
		parts2 = pg->GetPartList(&tmpV);
		tmpV = 0;
		while (tmpV < nParts)
		{
			parts2[tmpV] = parts[tmpV];
			tmpV++;
		}
		points2 = pg->GetPointList(&tmpV);
		tmpV = 0;
		while (tmpV < nPoints)
		{
			points2[tmpV * 2 + 0] = points[tmpV].x;
			points2[tmpV * 2 + 1] = points[tmpV].y;
			tmpV++;
		}
		return pg;
	}
	case FileGDBAPI::geometryPolyline:
	{
		FileGDBAPI::MultiPartShapeBuffer v;
		fgdbError hr = row->GetGeometry(v);
		if (hr != S_OK)
		{
			return 0;
		}
		FileGDBAPI::ShapeType shpType;
		Int32 nParts;
		Int32 nPoints;
		Int32 *parts;
		FileGDBAPI::Point *points;
		if (v.GetShapeType(shpType) != S_OK)
			return 0;
		if (v.GetNumParts(nParts) != S_OK)
			return 0;
		if (v.GetNumPoints(nPoints) != S_OK)
			return 0;
		if (v.GetParts(parts) != S_OK)
			return 0;
		if (v.GetPoints(points) != S_OK)
			return 0;

		Math::Polyline *pl;
		Int32 *parts2;
		Double *points2;
		OSInt tmpV;
		NEW_CLASS(pl, Math::Polyline(nParts, nPoints));
		parts2 = pl->GetPartList(&tmpV);
		tmpV = 0;
		while (tmpV < nParts)
		{
			parts2[tmpV] = parts[tmpV];
			tmpV++;
		}
		points2 = pl->GetPointList(&tmpV);
		tmpV = 0;
		while (tmpV < nPoints)
		{
			points2[tmpV * 2 + 0] = points[tmpV].x;
			points2[tmpV * 2 + 1] = points[tmpV].y;
			tmpV++;
		}
		return pl;
	}
	default:
		return 0;
	}
}

Bool Map::FileGDBReader::IsNull(OSInt colIndex)
{
	FileGDBAPI::Row *row = (FileGDBAPI::Row *)this->currRow;
	Bool v;
	fgdbError hr = row->IsNull((int)colIndex, v);
	if (hr != S_OK)
	{
		return true;
	}
	return v;
}

UTF8Char *Map::FileGDBReader::GetName(OSInt colIndex, UTF8Char *buff)
{
	FileGDBAPI::Table *tab = (FileGDBAPI::Table *)this->table;
	fgdbError hr;
	std::vector<FileGDBAPI::FieldDef> fields;
	if (colIndex < 0)
		return 0;
	hr = tab->GetFields(fields);
	if (hr == S_OK)
	{
		if ((UOSInt)colIndex >= fields.size())
			return 0;
		FileGDBAPI::FieldDef field = fields.at(colIndex);
		std::wstring wstr;
		hr = field.GetName(wstr);
		if (hr == S_OK)
		{
			return Text::StrWChar_UTF8(buff, wstr.data(), -1);
		}
	}
	return 0;
}

DB::DBUtil::ColType Map::FileGDBReader::GetColType(OSInt colIndex, OSInt *colSize)
{
	FileGDBAPI::Table *tab = (FileGDBAPI::Table *)this->table;
	fgdbError hr;
	std::vector<FileGDBAPI::FieldDef> fields;
	if (colIndex < 0)
		return DB::DBUtil::CT_Unknown;
	hr = tab->GetFields(fields);
	if (hr == S_OK)
	{
		FileGDBAPI::FieldType fType;
		if ((UOSInt)colIndex >= fields.size())
			return DB::DBUtil::CT_Unknown;
		FileGDBAPI::FieldDef field = fields.at(colIndex);
		hr = field.GetType(fType);
		if (hr == S_OK)
		{
			Int32 cs = 0;
			hr = field.GetLength(cs);
			*colSize = cs;
			switch (fType)
			{
			case FileGDBAPI::fieldTypeSmallInteger:
				return DB::DBUtil::CT_Int16;
			case FileGDBAPI::fieldTypeInteger:
				return DB::DBUtil::CT_Int32;
			case FileGDBAPI::fieldTypeSingle:
				return DB::DBUtil::CT_Float;
			case FileGDBAPI::fieldTypeDouble:
				return DB::DBUtil::CT_Double;
			case FileGDBAPI::fieldTypeString:
				return DB::DBUtil::CT_VarChar;
			case FileGDBAPI::fieldTypeDate:
				return DB::DBUtil::CT_DateTime;
			case FileGDBAPI::fieldTypeOID:
				return DB::DBUtil::CT_Int32;
			case FileGDBAPI::fieldTypeGeometry:
				return DB::DBUtil::CT_Vector;
			case FileGDBAPI::fieldTypeBlob:
				return DB::DBUtil::CT_Binary;
			case FileGDBAPI::fieldTypeRaster:
				return DB::DBUtil::CT_Unknown;
			case FileGDBAPI::fieldTypeGUID:
				return DB::DBUtil::CT_Unknown;
			case FileGDBAPI::fieldTypeGlobalID:
				return DB::DBUtil::CT_Unknown;
			case FileGDBAPI::fieldTypeXML:
				return DB::DBUtil::CT_VarChar;
			}
		}
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Map::FileGDBReader::GetColDef(OSInt colIndex, DB::ColDef *colDef)
{
	FileGDBAPI::Table *tab = (FileGDBAPI::Table *)this->table;
	fgdbError hr;
	UTF8Char sbuff[32];
	std::vector<FileGDBAPI::FieldDef> fields;
	if (colIndex < 0)
		return false;
	hr = tab->GetFields(fields);
	if (hr == S_OK)
	{
		FileGDBAPI::FieldType fType;
		if ((UOSInt)colIndex >= fields.size())
			return false;
		FileGDBAPI::FieldDef field = fields.at(colIndex);
		hr = field.GetType(fType);
		if (hr != S_OK)
		{
			return false;
		}
		Int32 cs = 0;
		hr = field.GetLength(cs);
		colDef->SetColSize(cs);
		switch (fType)
		{
		case FileGDBAPI::fieldTypeSmallInteger:
			colDef->SetColType(DB::DBUtil::CT_Int16);
			break;
		case FileGDBAPI::fieldTypeInteger:
			colDef->SetColType(DB::DBUtil::CT_Int32);
			break;
		case FileGDBAPI::fieldTypeSingle:
			colDef->SetColType(DB::DBUtil::CT_Float);
			break;
		case FileGDBAPI::fieldTypeDouble:
			colDef->SetColType(DB::DBUtil::CT_Double);
			break;
		case FileGDBAPI::fieldTypeString:
			colDef->SetColType(DB::DBUtil::CT_VarChar);
			break;
		case FileGDBAPI::fieldTypeDate:
			colDef->SetColType(DB::DBUtil::CT_DateTime);
			break;
		case FileGDBAPI::fieldTypeOID:
			colDef->SetColType(DB::DBUtil::CT_Int32);
			colDef->SetIsPK(true);
			break;
		case FileGDBAPI::fieldTypeGeometry:
			colDef->SetColType(DB::DBUtil::CT_Vector);
			{
				FileGDBAPI::GeometryDef geomDef;
				hr = field.GetGeometryDef(geomDef);
				if (hr == S_OK)
				{
					FileGDBAPI::SpatialReference spRef;
					hr = geomDef.GetSpatialReference(spRef);
					if (hr == S_OK)
					{
						std::wstring wstr;
						hr = spRef.SetSpatialReferenceText(wstr);
						if (hr == S_OK)
						{
							if (wstr.length() > 0)
							{
								const UTF8Char *csptr = Text::StrToUTF8New(wstr.data());
								colDef->SetAttr(csptr);
								Text::StrDelNew(csptr);
							}
							else
							{
								Int32 spId;
								hr = spRef.GetSpatialReferenceID(spId);
								if (hr == S_OK)
								{
									Text::StrInt32(Text::StrConcat(sbuff, (const UTF8Char*)"EPSG:"), spId);
									colDef->SetAttr(sbuff);
								}
							}
						}
					}
				}
			}
			break;
		case FileGDBAPI::fieldTypeBlob:
			colDef->SetColType(DB::DBUtil::CT_Binary);
			break;
		case FileGDBAPI::fieldTypeRaster:
			colDef->SetColType(DB::DBUtil::CT_Unknown);
			break;
		case FileGDBAPI::fieldTypeGUID:
			colDef->SetColType(DB::DBUtil::CT_Unknown);
			break;
		case FileGDBAPI::fieldTypeGlobalID:
			colDef->SetColType(DB::DBUtil::CT_Unknown);
			break;
		case FileGDBAPI::fieldTypeXML:
			colDef->SetColType(DB::DBUtil::CT_VarChar);
			break;
		default:
			colDef->SetColType(DB::DBUtil::CT_Unknown);
			break;
		}
		Bool allowNull;
		hr = field.GetIsNullable(allowNull);
		if (hr != S_OK)
			return false;
		colDef->SetIsNotNull(!allowNull);
		std::wstring wstr;
		hr = field.GetName(wstr);
		if (hr != S_OK)
			return false;
		colDef->SetColNameW(wstr.data());
		return true;
	}
	return false;
}

void Map::FileGDBReader::DelNewStr(const WChar *s)
{
	Text::StrDelNew(s);
}
#endif
