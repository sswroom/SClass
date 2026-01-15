#include "Stdafx.h"
#include "DB/SortableDBReader.h"
#include "DB/TableDef.h"
#include "Map/ESRI/FileGDBDir.h"
#include "Map/ESRI/FileGDBReader.h"
#include "Text/StringBuilderUTF8.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Map::ESRI::FileGDBDir::FileGDBDir(NN<IO::PackageFile> pkg, NN<FileGDBTable> systemCatalog, NN<Math::ArcGISPRJParser> prjParser) : DB::ReadingDB(pkg->GetSourceNameObj())
{
	this->pkg = pkg->Clone();
	this->prjParser = prjParser;
	NN<FileGDBReader> reader;
	if (!Optional<FileGDBReader>::ConvertFrom(systemCatalog->OpenReader(nullptr, 0, 0, nullptr, nullptr)).SetTo(reader))
	{
		systemCatalog.Delete();
		return;
	}
	this->tableMap.PutNN(systemCatalog->GetName(), 1);
	this->tables.PutNN(systemCatalog->GetName(), systemCatalog);
	this->tableNames.Add(systemCatalog->GetName()->Clone());
	NN<Text::String> s;
	Text::StringBuilderUTF8 sb;
	while (reader->ReadNext())
	{
		Int32 id = reader->GetInt32(0);
		Int32 fmt = reader->GetInt32(2);
		sb.ClearStr();
		reader->GetStr(1, sb);
		if (id > 1 && sb.GetLength() > 0 && fmt == 0)
		{
			s = Text::String::New(sb.ToCString());
			this->tableNames.Add(s);
			this->tableMap.PutNN(s, id);
		}
	}
	reader.Delete();
}

Map::ESRI::FileGDBDir::~FileGDBDir()
{
	this->tables.DeleteAll();
	this->prjParser.Delete();
	this->pkg.Delete();
	this->tableNames.FreeAll();
}

UIntOS Map::ESRI::FileGDBDir::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	UIntOS i = 0;
	UIntOS j = this->tableNames.GetCount();
	while (i < j)
	{
		names->Add(this->tableNames.GetItemNoCheck(i)->Clone());
		i++;
	}
	return j;
}

Optional<DB::DBReader> Map::ESRI::FileGDBDir::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<FileGDBTable> table;
	if (!this->GetTable(tableName).SetTo(table))
	{
#if defined(VERBOSE)
		printf("FileGDBDir: QueryTableData failed in getting table: %s\r\n", tableName.v.Ptr());
#endif
		return nullptr;
	}
	if (ordering.leng == 0)
	{
		return table->OpenReader(columnNames, ofst, maxCnt, ordering, condition);
	}
	else
	{
		return NEW_CLASS_D(DB::SortableDBReader(*this, schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition));
	}
}

Optional<DB::TableDef> Map::ESRI::FileGDBDir::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<FileGDBTable> table;
	if (!this->GetTable(tableName).SetTo(table))
	{
		return nullptr;
	}
	DB::TableDef *tab;
	NN<DB::DBReader> r;
	NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
	if (table->OpenReader(nullptr, 0, 0, nullptr, nullptr).SetTo(r))
	{
		tab->ColFromReader(r);
		this->CloseReader(r);
		return tab;
	}
	else
	{
		DEL_CLASS(tab);
		return nullptr;
	}
}

void Map::ESRI::FileGDBDir::CloseReader(NN<DB::DBReader> r)
{
	Map::ESRI::FileGDBReader *reader = (Map::ESRI::FileGDBReader*)r.Ptr();
	DEL_CLASS(reader);
}

void Map::ESRI::FileGDBDir::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
}

void Map::ESRI::FileGDBDir::Reconnect()
{
}

Bool Map::ESRI::FileGDBDir::IsError() const
{
	return this->tableMap.GetCount() == 0;
}

Optional<Map::ESRI::FileGDBTable> Map::ESRI::FileGDBDir::GetTable(Text::CStringNN name)
{
	Int32 id = this->tableMap.GetC(name);
	if (id == 0)
	{
		return nullptr;
	}
	NN<FileGDBTable> table;
	if (this->tables.GetC(name).SetTo(table))
	{
		return table;
	}
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Optional<IO::StreamData> indexFD;
	NN<IO::StreamData> tableFD;
	NN<FileGDBTable> innerTable;
	sptr = Text::StrConcatC(Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("a")), (UInt32)id), UTF8STRC(".gdbtablx"));
	sptr = Text::StrToLowerC(sbuff, sbuff, (UIntOS)(sptr - sbuff));
	indexFD = this->pkg->GetItemStmDataNew(CSTRP(sbuff, sptr));
	sptr = Text::StrConcatC(Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("a")), (UInt32)id), UTF8STRC(".gdbtable"));
	sptr = Text::StrToLowerC(sbuff, sbuff, (UIntOS)(sptr - sbuff));
	if (pkg->GetItemStmDataNew(CSTRP(sbuff, sptr)).SetTo(tableFD))
	{
		NEW_CLASSNN(innerTable, FileGDBTable(name, tableFD, indexFD, prjParser));
		tableFD.Delete();
		if (innerTable->IsError())
		{
			innerTable.Delete();
#if defined(VERBOSE)
			printf("FileGDBTable: Table %s has error\r\n", sbuff);
#endif
		}
		else
		{
			indexFD.Delete();
			this->tables.PutNN(innerTable->GetName(), innerTable);
			return innerTable;
		}
	}
#if defined(VERBOSE)
	else
	{
		printf("FileGDBTable: Cannot get item %s in package file\r\n", sbuff);
	}
#endif
	indexFD.Delete();
	return nullptr;
}

Optional<Map::ESRI::FileGDBDir> Map::ESRI::FileGDBDir::OpenDir(NN<IO::PackageFile> pkg)
{
	NN<FileGDBTable> table;
	IO::StreamData *indexFD = pkg->GetItemStmDataNew(CSTR("a00000001.gdbtablx")).OrNull();
	NN<IO::StreamData> tableFD;
	if (!pkg->GetItemStmDataNew(CSTR("a00000001.gdbtable")).SetTo(tableFD))
	{
		SDEL_CLASS(indexFD);
		return nullptr;
	}
	NN<Math::ArcGISPRJParser> prjParser;
	NEW_CLASSNN(prjParser, Math::ArcGISPRJParser());
	NEW_CLASSNN(table, FileGDBTable(CSTR("GDB_SystemCatalog"), tableFD, indexFD, prjParser));
	tableFD.Delete();
	SDEL_CLASS(indexFD);
	if (table->IsError())
	{
		table.Delete();
		prjParser.Delete();
		return nullptr;
	}
	NN<FileGDBDir> dir;
	NEW_CLASSNN(dir, FileGDBDir(pkg, table, prjParser));
	if (dir->IsError())
	{
		dir.Delete();
		return nullptr;
	}
	return dir;
}
