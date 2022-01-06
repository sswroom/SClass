#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "DB/MongoDB.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	const UTF8Char *username = (const UTF8Char*)"abc";
	const UTF8Char *password = (const UTF8Char*)"abc";
	const UTF8Char *serverhost = (const UTF8Char*)"abc.com";
	UInt16 serverport = 3717;
	const UTF8Char *database = (const UTF8Char*)"abc";

	DB::MongoDB *mongoDB;
	IO::LogTool *log;
	IO::ConsoleWriter *console;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(log, IO::LogTool());
	sb.ClearStr();
	DB::MongoDB::BuildURL(&sb, username, password, serverhost, serverport);
	NEW_CLASS(mongoDB, DB::MongoDB(sb.ToString(), database, log));
	
	Data::ArrayList<const UTF8Char*> nameList;
	UOSInt i;
	UOSInt j;
	j = mongoDB->GetDatabaseNames(&nameList);
	if (j <= 0)
	{
		console->WriteLineC(UTF8STRC("Error in getting Database List:"));
		sb.ClearStr();
		mongoDB->GetErrorMsg(&sb);
		console->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		console->WriteLineC(UTF8STRC("Database List:"));
		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			console->WriteLine(nameList.GetItem(i));
			i++;
		}
		mongoDB->FreeDatabaseNames(&nameList);
	}
	console->WriteLine();
	console->WriteLineC(UTF8STRC("Table List:"));
	nameList.Clear();
	mongoDB->GetTableNames(&nameList);
	i = 0;
	j = nameList.GetCount();
	while (i < j)
	{
		console->WriteLine(nameList.GetItem(i));
		i++;
	}

	DEL_CLASS(mongoDB);
	DEL_CLASS(log);
	DEL_CLASS(console);
	return 0;
}
