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
	class TCPClientFactory;
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
		virtual void SetParserList(Optional<Parser::ParserList> parsers) {};
		virtual void SetWebBrowser(Optional<Net::WebBrowser> browser) {};
		virtual void SetMapManager(Optional<Map::MapManager> mapMgr) {};
		virtual void SetEncFactory(Optional<Text::EncodingFactory> encFact) {};
		virtual void SetProgressHandler(Optional<IO::ProgressHandler> progHdlr) {};
		virtual void SetTCPClientFactory(NN<Net::TCPClientFactory> clif) {};
		virtual void SetSSLEngine(Optional<Net::SSLEngine> ssl) {};
		virtual void SetArcGISPRJParser(Optional<Math::ArcGISPRJParser> prjParser) {};
		virtual void SetLogTool(Optional<IO::LogTool> log) {};
		virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t) = 0;
		virtual IO::ParserType GetParserType() = 0;
	};
}
#endif

