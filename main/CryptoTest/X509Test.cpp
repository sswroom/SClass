#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/StmData/FileData.h"
#include "Parser/FileParser/X509Parser.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Parser::FileParser::X509Parser parser;
	const UTF8Char *fileName = (const UTF8Char*)"/home/sswroom/Progs/Stoneroad/keys/server.crt";

	IO::StmData::FileData *fd;
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	parser.ParseFile(fd, 0, IO::ParsedObject::PT_ASN1_DATA);
	DEL_CLASS(fd);
	return 0;
}
