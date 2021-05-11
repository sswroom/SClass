#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/StreamReader.h"
#include "IO/UDPLog.h"
#include "Parser/FileParser/LOGParser.h"
#include "Text/MyString.h"

Parser::FileParser::LOGParser::LOGParser()
{
	this->codePage = 0;
}

Parser::FileParser::LOGParser::~LOGParser()
{
}

Int32 Parser::FileParser::LOGParser::GetName()
{
	return *(Int32*)"LOGP";
}

void Parser::FileParser::LOGParser::SetCodePage(Int32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::LOGParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_LOG_FILE)
	{
		selector->AddFilter((const UTF8Char*)"*.log", (const UTF8Char*)"Log File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::LOGParser::GetParserType()
{
	return IO::ParsedObject::PT_LOG_FILE;
}

IO::ParsedObject *Parser::FileParser::LOGParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UTF8Char sbuff[512];
//	WChar baseDir[256];
//	WChar sbuff2[256];
//	WChar sbuff3[256];
//	WChar *filePath;
//	WChar *fileName;
//	WChar *sarr[10];
	OSInt i;
//	OSInt j;
//	OSInt k;
	Text::StrConcat(sbuff, fd->GetFullName());
	i = Text::StrLastIndexOf(sbuff, '.');
	if (Text::StrCompareICase(&sbuff[i], (const UTF8Char*)".LOG") != 0)
	{
		return 0;
	}
	IO::StreamDataStream *stm;
	IO::StreamReader *reader;
	NEW_CLASS(stm, IO::StreamDataStream(fd));
	NEW_CLASS(reader, IO::StreamReader(stm, this->codePage));
	if (reader->ReadLine(sbuff, 255) == 0)
	{
		DEL_CLASS(reader);
		DEL_CLASS(stm);
		return 0;
	}
	UOSInt strLen = Text::StrCharCnt(sbuff);
	if (strLen >= 43 && sbuff[2] == ':' && sbuff[5] == ':' && sbuff[8] == ' ' && Text::StrEquals(&sbuff[9], (const UTF8Char*)"MSG start UDP server successfully!"))
	{
		IO::UDPLog *log = 0;
/*		NEW_CLASS(log, IO::LogFile(fd->GetFullFileName()));
		log->*/
		//////////////////////////////
		DEL_CLASS(reader);
		DEL_CLASS(stm);
		return log;
	}
	DEL_CLASS(reader);
	DEL_CLASS(stm);
	return 0;
}
