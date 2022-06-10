#include "Stdafx.h"
#include "DB/SortableDBReader.h"
#include "Map/ESRI/FileGDBDir.h"
#include "Map/ESRI/FileGDBReader.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

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

Map::ESRI::FileGDBDir::FileGDBDir(Text::String *sourceName) : DB::ReadingDB(sourceName)
{
}

Map::ESRI::FileGDBDir::~FileGDBDir()
{
	LIST_FREE_FUNC(&this->tables, DEL_CLASS);
}

UOSInt Map::ESRI::FileGDBDir::GetTableNames(Data::ArrayList<Text::CString> *names)
{
	UOSInt i = 0;
	UOSInt j = this->tables.GetCount();
	while (i < j)
	{
		names->Add(this->tables.GetItem(i)->GetName()->ToCString());
		i++;
	}
	return j;
}

DB::DBReader *Map::ESRI::FileGDBDir::QueryTableData(Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
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
		return NEW_CLASS_D(DB::SortableDBReader(this, tableName, columnNames, ofst, maxCnt, ordering, condition));
	}
}

void Map::ESRI::FileGDBDir::CloseReader(DB::DBReader *r)
{
	Map::ESRI::FileGDBReader *reader = (Map::ESRI::FileGDBReader*)r;
	DEL_CLASS(reader);
}

void Map::ESRI::FileGDBDir::GetErrorMsg(Text::StringBuilderUTF8 *str)
{
}

void Map::ESRI::FileGDBDir::Reconnect()
{
}

void Map::ESRI::FileGDBDir::AddTable(FileGDBTable *table)
{
	this->tables.Add(table);
}

Map::ESRI::FileGDBDir *Map::ESRI::FileGDBDir::OpenDir(IO::PackageFile *pkg)
{
	IO::IStreamData *tableFD = pkg->GetItemStmData(UTF8STRC("a00000001.gdbtable"));
	IO::IStreamData *indexFD = pkg->GetItemStmData(UTF8STRC("a00000001.gdbtablx"));
	FileGDBTable *table;
	if (tableFD == 0)
	{
		SDEL_CLASS(indexFD);
		return 0;
	}
	NEW_CLASS(table, FileGDBTable(CSTR("GDB_SystemCatalog"), tableFD, indexFD));
	DEL_CLASS(tableFD);
	SDEL_CLASS(indexFD);
	if (table->IsError())
	{
		DEL_CLASS(table);
		return 0;
	}
	FileGDBReader *reader = (FileGDBReader*)table->OpenReader(0, 0, 0, CSTR_NULL, 0);
	if (reader == 0)
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
		reader->GetStr(1, &sb);
		if (id > 1 && sb.GetLength() > 0 && fmt == 0)
		{
			FileGDBTable *innerTable;
			sptr = Text::StrConcatC(Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("a")), (UInt32)id), UTF8STRC(".gdbtable"));
			sptr = Text::StrToLowerC(sbuff, sbuff, (UOSInt)(sptr - sbuff));
			tableFD = pkg->GetItemStmData(sbuff, (UOSInt)(sptr - sbuff));
			sptr = Text::StrConcatC(Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("a")), (UInt32)id), UTF8STRC(".gdbtablx"));
			sptr = Text::StrToLowerC(sbuff, sbuff, (UOSInt)(sptr - sbuff));
			indexFD = pkg->GetItemStmData(sbuff, (UOSInt)(sptr - sbuff));
			if (tableFD)
			{
				NEW_CLASS(innerTable, FileGDBTable(sb.ToCString(), tableFD, indexFD));
				DEL_CLASS(tableFD);
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
	DEL_CLASS(reader);
	return dir;
}
