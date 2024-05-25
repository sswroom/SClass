#include "Stdafx.h"
#include "Core/Core.h"
//#include "Exporter/WebPExporter.h"
#include "Parser/FileParser/HEIFParser.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Text::CStringNN fileName = CSTR("/home/sswroom/Progs/Temp/FileTest/grid_960x640.heic");
//	Text::CString outName = CSTR("/home/sswroom/Progs/Temp/FileTest/1.enc.webp");
	Parser::FileParser::HEIFParser parser;
	NN<IO::ParsedObject> pobj;
	if (parser.ParseFilePath(fileName).SetTo(pobj))
	{
//		Exporter::WebPExporter exporter;
//		exporter.ExportNewFile(outName, pobj, 0);
		pobj.Delete();
	}
	return 0;
}