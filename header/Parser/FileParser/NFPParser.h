#ifndef _SM_PARSER_FILEPARSER_NFPPARSER
#define _SM_PARSER_FILEPARSER_NFPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class NFPParser : public IO::FileParser
		{
		public:
			NFPParser();
			virtual ~NFPParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
