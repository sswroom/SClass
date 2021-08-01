#ifndef _SM_PARSER_FILEPARSER_SPKPARSER
#define _SM_PARSER_FILEPARSER_SPKPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SPKParser : public IO::FileParser
		{
		private:
			Parser::ParserList *parsers;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
		public:
			SPKParser();
			virtual ~SPKParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void SetSocketFactory(Net::SocketFactory *sockf);
			virtual void SetSSLEngine(Net::SSLEngine *ssl);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	}
}
#endif
