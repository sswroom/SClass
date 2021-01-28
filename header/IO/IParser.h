#ifndef _SM_IO_IPARSER
#define _SM_IO_IPARSER
#include "IO/IFileSelector.h"
#include "IO/IProgressHandler.h"
#include "IO/IStreamData.h"
#include "IO/ParsedObject.h"

namespace Map
{
	class MapManager;
}
namespace Net
{
	class SocketFactory;
	class WebBrowser;
}
namespace Parser
{
	class ParserList;
}
namespace Text
{
	class EncodingFactory;
}
namespace IO
{
	class IParser
	{
	public:
		virtual ~IParser(){};

		virtual Int32 GetName() = 0;
		virtual void SetCodePage(Int32 codePage) {};
		virtual void SetParserList(Parser::ParserList *parsers) {};
		virtual void SetWebBrowser(Net::WebBrowser *browser) {};
		virtual void SetMapManager(Map::MapManager *mapMgr) {};
		virtual void SetEncFactory(Text::EncodingFactory *encFact) {};
		virtual void SetProgressHandler(IO::IProgressHandler *progHdlr) {};
		virtual void SetSocketFactory(Net::SocketFactory *sockf) {};
		virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t) = 0;
		virtual IO::ParsedObject::ParserType GetParserType() = 0;
	};
};
#endif

