#include "Stdafx.h"
#include "Core/Core.h"
#include "Parser/FileParser/XMLParser.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Text::CString path = CSTR("/home/sswroom/Progs/Temp/test.jrxml");
	Parser::FileParser::XMLParser parser;
	IO::ParsedObject *pobj = parser.ParseFilePath(path);
	SDEL_CLASS(pobj);
	return 0;
}
