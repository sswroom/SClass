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
			Optional<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
		public:
			SPKParser();
			virtual ~SPKParser();

			virtual Int32 GetName();
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void SetTCPClientFactory(NN<Net::TCPClientFactory> clif);
			virtual void SetSSLEngine(Optional<Net::SSLEngine> ssl);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
