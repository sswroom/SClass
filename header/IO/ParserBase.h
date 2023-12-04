#ifndef _SM_IO_PARSERBASE
#define _SM_IO_PARSERBASE
#include "IO/FileSelector.h"
#include "IO/ProgressHandler.h"
#include "IO/StreamData.h"
#include "IO/ParsedObject.h"

namespace Map
{
	class MapManager;
}

namespace Math
{
	class ArcGISPRJParser;
}
namespace Net
{
	class SocketFactory;
	class SSLEngine;
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
	class LogTool;
	class ParserBase
	{
	public:
		virtual ~ParserBase(){};

		virtual Int32 GetName() = 0;
		virtual void SetCodePage(UInt32 codePage) {};
		virtual void SetParserList(Parser::ParserList *parsers) {};
		virtual void SetWebBrowser(Net::WebBrowser *browser) {};
		virtual void SetMapManager(Map::MapManager *mapMgr) {};
		virtual void SetEncFactory(Text::EncodingFactory *encFact) {};
		virtual void SetProgressHandler(IO::ProgressHandler *progHdlr) {};
		virtual void SetSocketFactory(NotNullPtr<Net::SocketFactory> sockf) {};
		virtual void SetSSLEngine(Optional<Net::SSLEngine> ssl) {};
		virtual void SetArcGISPRJParser(Math::ArcGISPRJParser *prjParser) {};
		virtual void SetLogTool(IO::LogTool *log) {};
		virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t) = 0;
		virtual IO::ParserType GetParserType() = 0;
	};
}
#endif

