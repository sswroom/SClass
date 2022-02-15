#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/StmData/FileData.h"
#include "Manage/HiResClock.h"
#include "Parser/FullParserList.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Parser::FullParserList *parsers;
	IO::StmData::FileData *fd;
	IO::ParsedObject *pobj;
	Manage::HiResClock clk;
	IO::ParserType pt;
	Double t;
	IO::ConsoleWriter console;
	Text::EncodingFactory *encFact;
	NEW_CLASS(encFact, Text::EncodingFactory());
	NEW_CLASS(parsers, Parser::FullParserList());
	parsers->SetEncFactory(encFact);
	NEW_CLASS(fd, IO::StmData::FileData(CSTR("/mnt/raid2_3/GPS/KML/HKPath/Lantau Island.kml"), false));

	clk.Start();
	pobj = parsers->ParseFile(fd, &pt);
	t = clk.GetTimeDiff();
	DEL_CLASS(fd);
	if (pobj)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Parse success, t = "));
		sb.AppendDouble(t);
		console.WriteLineC(sb.ToString(), sb.GetLength());
		DEL_CLASS(pobj);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Parse failed, t = "));
		sb.AppendDouble(t);
		console.WriteLineC(sb.ToString(), sb.GetLength());
	}
	DEL_CLASS(parsers);
	DEL_CLASS(encFact);
	return 0;
}
