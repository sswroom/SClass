#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/StmData/FileData.h"
#include "Manage/HiResClock.h"
#include "Parser/FullParserList.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN path = CSTR("/home/sswroom/Progs/Temp/Lantau Island.kml");
//	Text::CStringNN path = CSTR("/mnt/raid2_3/GPS/KML/HKPath/Lantau Island.kml");
	Optional<IO::ParsedObject> pobj;
	Manage::HiResClock clk;
	Double t;
	IO::ConsoleWriter console;
	Text::EncodingFactory encFact;
	Parser::FullParserList parsers;
	parsers.SetEncFactory(&encFact);
	{
		IO::StmData::FileData fd(path, false);
		clk.Start();
		pobj = parsers.ParseFile(fd);
		t = clk.GetTimeDiff();
	}
	if (pobj.NotNull())
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Parse success, t = "));
		sb.AppendDouble(t);
		console.WriteLine(sb.ToCString());
		pobj.Delete();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Parse failed, t = "));
		sb.AppendDouble(t);
		console.WriteLine(sb.ToCString());
	}
	return 0;
}
