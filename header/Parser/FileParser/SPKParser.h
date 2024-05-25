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
			Optional<Parser::ParserList> parsers;
			Optional<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
		public:
			SPKParser();
			virtual ~SPKParser();

			virtual Int32 GetName();
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void SetSocketFactory(NN<Net::SocketFactory> sockf);
			virtual void SetSSLEngine(Optional<Net::SSLEngine> ssl);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
