#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/StmData/FileData.h"
#include "Parser/FileParser/X509Parser.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Parser::FileParser::X509Parser parser;
	Text::CStringNN fileName = CSTR("/home/sswroom/Progs/Temp/keys/server.crt");

	IO::StmData::FileData fd(fileName, false);
	parser.ParseFile(fd, nullptr, IO::ParserType::ASN1Data);
	return 0;
}
