#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/StmData/FileData.h"
#include "Parser/FileParser/X509Parser.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Parser::FileParser::X509Parser parser;
	Text::CString fileName = CSTR("/home/sswroom/Progs/Temp/keys/server.crt");

	IO::StmData::FileData *fd;
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	parser.ParseFile(fd, 0, IO::ParserType::ASN1Data);
	DEL_CLASS(fd);
	return 0;
}
