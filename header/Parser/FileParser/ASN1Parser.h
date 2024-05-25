#ifndef _SM_PARSER_FILEPARSER_ASN1PARSER
#define _SM_PARSER_FILEPARSER_ASN1PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ASN1Parser : public IO::FileParser
		{
		public:
			ASN1Parser();
			virtual ~ASN1Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
