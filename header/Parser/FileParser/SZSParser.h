#ifndef _SM_PARSER_FILEPARSER_SZSPARSER
#define _SM_PARSER_FILEPARSER_SZSPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SZSParser : public IO::FileParser
		{
		public:
			SZSParser();
			virtual ~SZSParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
