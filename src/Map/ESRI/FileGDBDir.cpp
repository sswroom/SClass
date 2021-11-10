#include "Stdafx.h"
#include "DB/SortableDBReader.h"
#include "Map/ESRI/FileGDBDir.h"
#include "Map/ESRI/FileGDBReader.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

Map::ESRI::FileGDBTable *Map::ESRI::FileGDBDir::GetTable(const UTF8Char *name)
{
	UOSInt i = this->tables->GetCount();
	while (i-- > 0)
	{
		FileGDBTable *table = this->tables->GetItem(i);
		if (Text::StrEqualsICase(table->GetName(), name))
		{
			return table;
		}
	}
	return 0;
}

Map::ESRI::FileGDBDir::FileGDBDir(const UTF8Char *sourceName) : DB::ReadingDB(sourceName)
{
	NEW_CLASS(this->tables, Data::ArrayList<FileGDBTable*>());
}

Map::ESRI::FileGDBDir::~FileGDBDir()
{
	LIST_FREE_FUNC(this->tables, DEL_CLASS);
	DEL_CLASS(this->tables);
}

UOSInt Map::ESRI::FileGDBDir::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	UOSInt i = 0;
	UOSInt j = this->tables->GetCount();
	while (i < j)
	{
		names->Add(this->tables->GetItem(i)->GetName());
		i++;
	}
	return j;
}

DB::DBReader *Map::ESRI::FileGDBDir::GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, DB::QueryConditions *condition)
{
	FileGDBTable *table = this->GetTable(tableName);
	if (table == 0)
	{
		return 0;
	}
	if (ordering == 0)
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

void Map::ESRI::FileGDBDir::GetErrorMsg(Text::StringBuilderUTF *str)
{
}

void Map::ESRI::FileGDBDir::Reconnect()
{
}

void Map::ESRI::FileGDBDir::AddTable(FileGDBTable *table)
{
	this->tables->Add(table);
}

Map::ESRI::FileGDBDir *Map::ESRI::FileGDBDir::OpenDir(IO::PackageFile *pkg)
{
	IO::IStreamData *fd = pkg->GetItemStmData((const UTF8Char*)"a00000001.gdbtable");
	FileGDBTable *table;
	if (fd == 0)
	{
		return 0;
	}
	NEW_CLASS(table, FileGDBTable((const UTF8Char*)"GDB_SystemCatalog", fd));
	DEL_CLASS(fd);
	if (table->IsError())
	{
		DEL_CLASS(table);
		return 0;
	}
	FileGDBReader *reader = (FileGDBReader*)table->OpenReader(0, 0, 0, 0, 0);
	if (reader == 0)
	{
		DEL_CLASS(table);
		return 0;
	}
	FileGDBDir *dir;
	NEW_CLASS(dir, FileGDBDir(pkg->GetSourceNameObj()));
	dir->AddTable(table);
	UTF8Char sbuff[128];
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
			Text::StrConcat(Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"a"), (UInt32)id), (const UTF8Char*)".gdbtable");
			Text::StrToLower(sbuff, sbuff);
			fd = pkg->GetItemStmData(sbuff);
			if (fd)
			{
				NEW_CLASS(innerTable, FileGDBTable(sb.ToString(), fd));
				DEL_CLASS(fd);
				if (innerTable->IsError())
				{
					DEL_CLASS(innerTable);
				}
				else
				{
					dir->AddTable(innerTable);
				}
			}
		}
	}
	DEL_CLASS(reader);
	return dir;
}
