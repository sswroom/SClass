#include "Stdafx.h"
#include "Map/ESRI/FileGDBDir.h"
#include "Map/ESRI/FileGDBReader.h"

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

DB::DBReader *Map::ESRI::FileGDBDir::GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition)
{
	FileGDBTable *table = this->GetTable(name);
	if (table == 0)
	{
		return 0;
	}
	return table->OpenReader();
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
	////////////////////////////////////////
	return 0;
}
