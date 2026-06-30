#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "DB/MongoDB.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CString username = CSTR("abc");
	Text::CString password = CSTR("abc");
	Text::CStringNN serverhost = CSTR("abc.com");
	UInt16 serverport = 3717;
	Text::CString database = CSTR("abc");

	NN<DB::MongoDB> mongoDB;
	NN<IO::LogTool> log;
	NN<IO::ConsoleWriter> console;
	Text::StringBuilderUTF8 sb;
	NEW_CLASSNN(console, IO::ConsoleWriter());
	NEW_CLASSNN(log, IO::LogTool());
	sb.ClearStr();
	DB::MongoDB::BuildURL(sb, username, password, serverhost, serverport);
	NEW_CLASSNN(mongoDB, DB::MongoDB(sb.ToCString(), database, log));
	
	Data::ArrayListStringNN tableList;
	Data::ArrayListStringNN dbList;
	Data::ArrayIterator<NN<Text::String>> it;
	UIntOS j;
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
	mongoDB->QueryTableNames(nullptr, tableList);
	it = tableList.Iterator();
	while (it.HasNext())
	{
		console->WriteLine(it.Next()->ToCString());
	}
	tableList.FreeAll();

	mongoDB.Delete();
	log.Delete();
	console.Delete();
	return 0;
}
