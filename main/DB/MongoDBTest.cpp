#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "DB/MongoDB.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Text::CString username = CSTR("abc");
	Text::CString password = CSTR("abc");
	Text::CString serverhost = CSTR("abc.com");
	UInt16 serverport = 3717;
	Text::CString database = CSTR("abc");

	DB::MongoDB *mongoDB;
	IO::LogTool *log;
	IO::ConsoleWriter *console;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(log, IO::LogTool());
	sb.ClearStr();
	DB::MongoDB::BuildURL(&sb, username, password, serverhost, serverport);
	NEW_CLASS(mongoDB, DB::MongoDB(sb.ToCString(), database, log));
	
	Data::ArrayListNN<Text::String> tableList;
	Data::ArrayList<Text::String*> dbList;
	UOSInt i;
	UOSInt j;
	j = mongoDB->GetDatabaseNames(&dbList);
	if (j <= 0)
	{
		console->WriteLineC(UTF8STRC("Error in getting Database List:"));
		sb.ClearStr();
		mongoDB->GetLastErrorMsg(&sb);
		console->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		console->WriteLineC(UTF8STRC("Database List:"));
		i = 0;
		j = dbList.GetCount();
		while (i < j)
		{
			console->WriteLineCStr(dbList.GetItem(i)->ToCString());
			i++;
		}
		mongoDB->FreeDatabaseNames(&dbList);
	}
	console->WriteLine();
	console->WriteLineC(UTF8STRC("Table List:"));
	mongoDB->QueryTableNames(CSTR_NULL, &tableList);
	i = 0;
	j = tableList.GetCount();
	while (i < j)
	{
		console->WriteLineCStr(tableList.GetItem(i)->ToCString());
		i++;
	}
	LIST_FREE_STRING(&tableList);

	DEL_CLASS(mongoDB);
	DEL_CLASS(log);
	DEL_CLASS(console);
	return 0;
}
