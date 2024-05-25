#ifndef _SM_PARSER_PARSERLIST
#define _SM_PARSER_PARSERLIST
#include "Data/ArrayListNN.h"
#include "IO/FileParser.h"
#include "IO/FileSelector.h"
#include "IO/ObjectParser.h"
#include "IO/StreamData.h"
#include "IO/ParsedObject.h"

namespace Parser
{
	class ParserList
	{
	private:
		Data::ArrayListNN<IO::FileParser> filePArr;
		Data::ArrayListNN<IO::ObjectParser> objPArr;

	protected:
		ParserList();
	public:
		virtual ~ParserList();

		void AddFileParser(NN<IO::FileParser> parser);
		void AddObjectParser(NN<IO::ObjectParser> parser);

		void SetCodePage(UInt32 codePage);
		void SetMapManager(Optional<Map::MapManager> mapMgr);
		void SetEncFactory(Optional<Text::EncodingFactory> encFact);
		void SetWebBrowser(Optional<Net::WebBrowser> browser);
		void SetProgressHandler(Optional<IO::ProgressHandler> progHdlr);
		void SetSocketFactory(NN<Net::SocketFactory> sockf);
		void SetSSLEngine(Optional<Net::SSLEngine> ssl);
		void SetArcGISPRJParser(Optional<Math::ArcGISPRJParser> prjParser);
		void SetLogTool(Optional<IO::LogTool> log);
		void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
		Optional<IO::ParsedObject> ParseFile(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		Optional<IO::ParsedObject> ParseFile(NN<IO::StreamData> fd, IO::PackageFile *pkgFile);
		Optional<IO::ParsedObject> ParseFile(NN<IO::StreamData> fd);
		Optional<IO::ParsedObject> ParseFileType(NN<IO::StreamData> fd, IO::ParserType t);
		Optional<IO::ParsedObject> ParseObject(NN<IO::ParsedObject> pobj);
		Optional<IO::ParsedObject> ParseObjectType(NN<IO::ParsedObject> pobj, IO::ParserType targetType);
	};
}
#endif
