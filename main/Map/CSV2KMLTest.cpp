#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Exporter/KMLExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Math/Math_C.h"
#include "Manage/HiResClock.h"
#include "Parser/FileParser/CSVParser.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<IO::StmData::FileData> fd;
	Parser::FileParser::CSVParser *parser;
	Exporter::KMLExporter *exporter;
	Manage::HiResClock *clk;
	IO::Path::PathType pt;
	IO::ConsoleWriter *console;
	NN<IO::Path::FindFileSession> sess;

	NEW_CLASS(parser, Parser::FileParser::CSVParser());
	NEW_CLASS(exporter, Exporter::KMLExporter());
	NEW_CLASS(clk, Manage::HiResClock());
	NEW_CLASS(console, IO::ConsoleWriter());
	
	exporter->SetCodePage(65001);
	sptr = Text::StrConcatC(sbuff, UTF8STRC("GPS/"));
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("temp*.csv"));
	clk->Start();
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(sptr, sess, nullptr, pt, nullptr).SetTo(sptr2))
		{
			NEW_CLASSNN(fd, IO::StmData::FileData(CSTRP(sbuff, sptr2), false));
			NN<IO::ParsedObject> pobj;
			if (parser->ParseFile(fd, nullptr, IO::ParserType::Unknown).SetTo(pobj))
			{
				NN<IO::FileStream> fs;
				sptr2 = IO::Path::ReplaceExt(sptr, UTF8STRC("kml"));
				NEW_CLASSNN(fs, IO::FileStream(CSTRP(sbuff, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				exporter->ExportFile(fs, CSTRP(sbuff, sptr2), pobj, nullptr);
				fs.Delete();
				pobj.Delete();
			}
			fd.Delete();
		}
		IO::Path::FindFileClose(sess);
	}
	Double t1 = clk->GetTimeDiff();

	sptr = Text::StrConcatC(Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("Time used: ")), Double2Int32(t1 * 1000)), UTF8STRC(" ms"));
	console->WriteLine(CSTRP(sbuff, sptr));

	DEL_CLASS(console);
	DEL_CLASS(exporter);
	DEL_CLASS(parser);
	DEL_CLASS(clk);
	return 0;
}
