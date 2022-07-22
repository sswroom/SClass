#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/WebPExporter.h"
#include "Parser/FileParser/WebPParser.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::CString fileName = CSTR("/home/sswroom/Progs/Temp/FileTest/1.webp");
	Text::CString outName = CSTR("/home/sswroom/Progs/Temp/FileTest/1.enc.webp");
	Parser::FileParser::WebPParser parser;
	IO::ParsedObject *pobj = parser.ParseFilePath(fileName);
	if (pobj)
	{
		Exporter::WebPExporter exporter;
		exporter.ExportNewFile(outName, pobj, 0);
		DEL_CLASS(pobj);
	}
	return 0;
}