#ifndef _SM_PARSER_FILEPARSER_TSUYOSHIARCPARSER
#define _SM_PARSER_FILEPARSER_TSUYOSHIARCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class TsuyoshiArcParser : public IO::FileParser
		{
		public:
			TsuyoshiArcParser();
			virtual ~TsuyoshiArcParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
