#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "DB/MongoDB.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Text::CString username = CSTR("abc");
	Text::CString password = CSTR("abc");
	Text::CStringNN serverhost = CSTR("abc.com");
	UInt16 serverport = 3717;
	Text::CString database = CSTR("abc");

	DB::MongoDB *mongoDB;
	IO::LogTool *log;
	IO::ConsoleWriter *console;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(log, IO::LogTool());
	sb.ClearStr();
	DB::MongoDB::BuildURL(sb, username, password, serverhost, serverport);
	NEW_CLASS(mongoDB, DB::MongoDB(sb.ToCString(), database, log));
	
	Data::ArrayListStringNN tableList;
	Data::ArrayListStringNN dbList;
	Data::ArrayIterator<NN<Text::String>> it;
	UOSInt j;
	j = mongoDB->GetDatabaseNames(dbList);
	if (j <= 0)
	{
		console->WriteLine(CSTR("Error in getting Database List:"));
		sb.ClearStr();
		mongoDB->GetLastErrorMsg(sb);
		console->WriteLine(sb.ToCString());
	}
	else
	{
		console->WriteLine(CSTR("Database List:"));
		it = dbList.Iterator();
		while (it.HasNext())
		{
			console->WriteLine(it.Next()->ToCString());
		}
		mongoDB->FreeDatabaseNames(dbList);
	}
	console->WriteLine();
	console->WriteLine(CSTR("Table List:"));
	mongoDB->QueryTableNames(CSTR_NULL, tableList);
	it = tableList.Iterator();
	while (it.HasNext())
	{
		console->WriteLine(it.Next()->ToCString());
	}
	tableList.FreeAll();

	DEL_CLASS(mongoDB);
	DEL_CLASS(log);
	DEL_CLASS(console);
	return 0;
}
