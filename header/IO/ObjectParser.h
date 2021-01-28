#ifndef _SM_IO_OBJECTPARSER
#define _SM_IO_OBJECTPARSER
#include "IO/IStreamData.h"
#include "IO/ParsedObject.h"
#include "IO/IProgressHandler.h"
#include "Text/EncodingFactory.h"
#include "Net/WebBrowser.h"

namespace IO
{
	class FileParser
	{
	public:
		virtual ~FileParser(){};

		virtual Int32 GetName() = 0;
		virtual void SetCodePage(Int32 codePage) {};
		virtual void SetParserList(Parser::ParserList *parsers) {};
		virtual void SetWebBrowser(Net::WebBrowser *browser) {};
		virtual void SetMapManager(Map::MapManager *mapMgr) {};
		virtual void SetEncFactory(Text::EncodingFactory *encFact) {};
		virtual void SetProgressHandler(IO::IProgressHandler *progHdlr) {};
		virtual IO::ParsedObject::ParserType GetParserType() = 0;
		virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd) = 0;
	};
};
#endif

