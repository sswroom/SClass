#ifndef _SM_PARSER_PARSERLIST
#define _SM_PARSER_PARSERLIST
#include "Data/ArrayList.h"
#include "IO/FileParser.h"
#include "IO/IFileSelector.h"
#include "IO/IObjectParser.h"
#include "IO/IStreamData.h"
#include "IO/ParsedObject.h"

namespace Parser
{
	class ParserList
	{
	private:
		Data::ArrayList<IO::FileParser*> *filePArr;
		Data::ArrayList<IO::IObjectParser*> *objPArr;

	protected:
		ParserList();
	public:
		virtual ~ParserList();

		void AddFileParser(IO::FileParser *parser);
		void AddObjectParser(IO::IObjectParser *parser);

		void SetCodePage(UInt32 codePage);
		void SetMapManager(Map::MapManager *mapMgr);
		void SetEncFactory(Text::EncodingFactory *encFact);
		void SetWebBrowser(Net::WebBrowser *browser);
		void SetProgressHandler(IO::IProgressHandler *progHdlr);
		void SetSocketFactory(Net::SocketFactory *sockf);
		void SetSSLEngine(Net::SSLEngine *ssl);
		void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
		IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType *t, IO::ParserType targetType);
		IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType *t);
		IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::ParserType *t);
		IO::ParsedObject *ParseFileType(IO::IStreamData *fd, IO::ParserType t);
		IO::ParsedObject *ParseObject(IO::ParsedObject *pobj, IO::ParserType *t);
	};
}
#endif
