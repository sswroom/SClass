#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/Path.h"
#include "Parser/FileParser/DShowParser.h"
#ifdef USE_DSHOW
#include "Media/DShow/DShowGraph.h"
#endif

Parser::FileParser::DShowParser::DShowParser()
{
}

Parser::FileParser::DShowParser::~DShowParser()
{
}

Int32 Parser::FileParser::DShowParser::GetName()
{
	return *(Int32*)"DSHO";
}

void Parser::FileParser::DShowParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_VIDEO_PARSER)
	{
	}
}

IO::ParsedObject::ParserType Parser::FileParser::DShowParser::GetParserType()
{
	return IO::ParsedObject::PT_VIDEO_PARSER;
}

IO::ParsedObject *Parser::FileParser::DShowParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
#ifdef USE_DSHOW
	WChar sbuff[256];
	OSInt i;
	if (!fd->IsFullFile())
		return 0;
	Media::DShow::DShowGraph *graph = 0;//Media::DShow::DShowGraph::ParseFile(fd->GetFullName());
	if (graph)
	{

		DEL_CLASS(graph);
	}
	return 0;
#else
	return 0;
#endif
}
