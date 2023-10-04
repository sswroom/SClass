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

void Parser::FileParser::DShowParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
	}
}

IO::ParserType Parser::FileParser::DShowParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::DShowParser::ParseFile(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
#ifdef USE_DSHOW
	WChar wbuff[256];
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
