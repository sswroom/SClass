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
		void SetEncFactory(Text::EncodingFactory *encFact);
		void SetWebBrowser(Net::WebBrowser *browser);
		void SetProgressHandler(IO::ProgressHandler *progHdlr);
		void SetSocketFactory(NotNullPtr<Net::SocketFactory> sockf);
		void SetSSLEngine(Net::SSLEngine *ssl);
		void SetArcGISPRJParser(Math::ArcGISPRJParser *prjParser);
		void SetLogTool(IO::LogTool *log);
		void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
		IO::ParsedObject *ParseFile(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType *t, IO::ParserType targetType);
		IO::ParsedObject *ParseFile(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType *t);
		IO::ParsedObject *ParseFile(NotNullPtr<IO::StreamData> fd, IO::ParserType *t);
		IO::ParsedObject *ParseFileType(NotNullPtr<IO::StreamData> fd, IO::ParserType t);
		IO::ParsedObject *ParseObject(IO::ParsedObject *pobj, IO::ParserType *t);
		IO::ParsedObject *ParseObjectType(IO::ParsedObject *pobj, IO::ParserType *t, IO::ParserType targetType);
	};
}
#endif
