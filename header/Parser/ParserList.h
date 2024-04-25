#ifndef _SM_PARSER_PARSERLIST
#define _SM_PARSER_PARSERLIST
#include "Data/ArrayList.h"
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
		Data::ArrayList<IO::FileParser*> filePArr;
		Data::ArrayList<IO::ObjectParser*> objPArr;

	protected:
		ParserList();
	public:
		virtual ~ParserList();

		void AddFileParser(IO::FileParser *parser);
		void AddObjectParser(IO::ObjectParser *parser);

		void SetCodePage(UInt32 codePage);
		void SetMapManager(Map::MapManager *mapMgr);
		void SetEncFactory(Optional<Text::EncodingFactory> encFact);
		void SetWebBrowser(Net::WebBrowser *browser);
		void SetProgressHandler(IO::ProgressHandler *progHdlr);
		void SetSocketFactory(NN<Net::SocketFactory> sockf);
		void SetSSLEngine(Optional<Net::SSLEngine> ssl);
		void SetArcGISPRJParser(Math::ArcGISPRJParser *prjParser);
		void SetLogTool(IO::LogTool *log);
		void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
		IO::ParsedObject *ParseFile(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		IO::ParsedObject *ParseFile(NN<IO::StreamData> fd, IO::PackageFile *pkgFile);
		IO::ParsedObject *ParseFile(NN<IO::StreamData> fd);
		IO::ParsedObject *ParseFileType(NN<IO::StreamData> fd, IO::ParserType t);
		IO::ParsedObject *ParseObject(NN<IO::ParsedObject> pobj);
		IO::ParsedObject *ParseObjectType(NN<IO::ParsedObject> pobj, IO::ParserType targetType);
	};
}
#endif
