#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/WebPExporter.h"
#include "Parser/FileParser/WebPParser.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN fileName = CSTR("/home/sswroom/Progs/Temp/FileTest/1.webp");
	Text::CStringNN outName = CSTR("/home/sswroom/Progs/Temp/FileTest/1.enc.webp");
	Parser::FileParser::WebPParser parser;
	NN<IO::ParsedObject> pobj;
	if (parser.ParseFilePath(fileName).SetTo(pobj))
	{
		Exporter::WebPExporter exporter;
		exporter.ExportNewFile(outName, pobj, 0);
		pobj.Delete();
	}
	return 0;
}