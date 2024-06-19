#ifndef _SM_PARSER_FILEPARSER_ZIPPARSER
#define _SM_PARSER_FILEPARSER_ZIPPARSER
#include "IO/FileParser.h"
#include "IO/VirtualPackageFile.h"
#include "Text/Encoding.h"

namespace Parser
{
	namespace FileParser
	{
		class ZIPParser : public IO::FileParser
		{
		private:
			UInt32 codePage;
			Optional<Text::EncodingFactory> encFact;
			Optional<Parser::ParserList> parsers;
			Optional<Net::WebBrowser> browser;
		public:
			ZIPParser();
			virtual ~ZIPParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void SetWebBrowser(Optional<Net::WebBrowser> browser);
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void SetEncFactory(Optional<Text::EncodingFactory> encFact);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);

		private:
			static UOSInt ParseCentDir(NN<IO::VirtualPackageFile> pf, Text::Encoding *enc, NN<IO::StreamData> fd, Data::ByteArrayR buff, UInt64 ofst);
		};
	}
}
#endif
