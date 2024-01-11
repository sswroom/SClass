#include "Stdafx.h"
#include "DB/SortableDBReader.h"
#include "DB/TableDef.h"
#include "Map/ESRI/FileGDBDir.h"
#include "Map/ESRI/FileGDBReader.h"
#include "Text/StringBuilderUTF8.h"

Map::ESRI::FileGDBTable *Map::ESRI::FileGDBDir::GetTable(Text::CString name)
{
	UOSInt i = this->tables.GetCount();
	while (i-- > 0)
	{
		FileGDBTable *table = this->tables.GetItem(i);
		if (table->GetName()->EqualsICase(name.v, name.leng))
		{
			return table;
		}
	}
	return 0;
}

Map::ESRI::FileGDBDir::FileGDBDir(NotNullPtr<Text::String> sourceName) : DB::ReadingDB(sourceName)
{
}

Map::ESRI::FileGDBDir::~FileGDBDir()
{
	LIST_FREE_FUNC(&this->tables, DEL_CLASS);
}

UOSInt Map::ESRI::FileGDBDir::QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListStringNN> names)
{
	UOSInt i = 0;
	UOSInt j = this->tables.GetCount();
	while (i < j)
	{
		names->Add(this->tables.GetItem(i)->GetName()->Clone());
		i++;
	}
	return j;
}

Optional<DB::DBReader> Map::ESRI::FileGDBDir::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	FileGDBTable *table = this->GetTable(tableName);
	if (table == 0)
	{
		return 0;
	}
	if (ordering.leng == 0)
	{
		return table->OpenReader(columnNames, ofst, maxCnt, ordering, condition);
	}
	else
	{
		return NEW_CLASS_D(DB::SortableDBReader(this, schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition));
	}
}

DB::TableDef *Map::ESRI::FileGDBDir::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	FileGDBTable *table = this->GetTable(tableName);
	if (table == 0)
	{
		return 0;
	}
	DB::TableDef *tab;
	NotNullPtr<DB::DBReader> r;
	NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
	if (table->OpenReader(0, 0, 0, CSTR_NULL, 0).SetTo(r))
	{
		tab->ColFromReader(r);
		this->CloseReader(r);
		return tab;
	}
	else
	{
		return 0;
	}
}

void Map::ESRI::FileGDBDir::CloseReader(NotNullPtr<DB::DBReader> r)
{
	Map::ESRI::FileGDBReader *reader = (Map::ESRI::FileGDBReader*)r.Ptr();
	DEL_CLASS(reader);
}

void Map::ESRI::FileGDBDir::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
{
}

void Map::ESRI::FileGDBDir::Reconnect()
{
}

void Map::ESRI::FileGDBDir::AddTable(FileGDBTable *table)
{
	this->tables.Add(table);
}

Map::ESRI::FileGDBDir *Map::ESRI::FileGDBDir::OpenDir(NotNullPtr<IO::PackageFile> pkg, NotNullPtr<Math::ArcGISPRJParser> prjParser)
{
	FileGDBTable *table;
	IO::StreamData *indexFD = pkg->GetItemStmDataNew(CSTR("a00000001.gdbtablx")).OrNull();
	NotNullPtr<IO::StreamData> tableFD;;
	if (!pkg->GetItemStmDataNew(CSTR("a00000001.gdbtable")).SetTo(tableFD))
	{
		SDEL_CLASS(indexFD);
		return 0;
	}
	NEW_CLASS(table, FileGDBTable(CSTR("GDB_SystemCatalog"), tableFD, indexFD, prjParser));
	tableFD.Delete();
	SDEL_CLASS(indexFD);
	if (table->IsError())
	{
		DEL_CLASS(table);
		return 0;
	}
	NotNullPtr<FileGDBReader> reader;
	if (!Optional<FileGDBReader>::ConvertFrom(table->OpenReader(0, 0, 0, CSTR_NULL, 0)).SetTo(reader))
	{
		DEL_CLASS(table);
		return 0;
	}
	FileGDBDir *dir;
	NEW_CLASS(dir, FileGDBDir(pkg->GetSourceNameObj()));
	dir->AddTable(table);
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	while (reader->ReadNext())
	{
		Int32 id = reader->GetInt32(0);
		Int32 fmt = reader->GetInt32(2);
		sb.ClearStr();
		reader->GetStr(1, sb);
		if (id > 1 && sb.GetLength() > 0 && fmt == 0)
		{
			FileGDBTable *innerTable;
			sptr = Text::StrConcatC(Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("a")), (UInt32)id), UTF8STRC(".gdbtablx"));
			sptr = Text::StrToLowerC(sbuff, sbuff, (UOSInt)(sptr - sbuff));
			indexFD = pkg->GetItemStmDataNew(CSTRP(sbuff, sptr)).OrNull();
			sptr = Text::StrConcatC(Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("a")), (UInt32)id), UTF8STRC(".gdbtable"));
			sptr = Text::StrToLowerC(sbuff, sbuff, (UOSInt)(sptr - sbuff));
			if (pkg->GetItemStmDataNew(CSTRP(sbuff, sptr)).SetTo(tableFD))
			{
				NEW_CLASS(innerTable, FileGDBTable(sb.ToCString(), tableFD, indexFD, prjParser));
				tableFD.Delete();
				if (innerTable->IsError())
				{
					DEL_CLASS(innerTable);
				}
				else
				{
					dir->AddTable(innerTable);
				}
			}
			SDEL_CLASS(indexFD);
		}
	}
	reader.Delete();
	return dir;
}
