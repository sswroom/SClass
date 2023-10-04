#ifndef _SM_PARSER_FILEPARSER_SPDPARSER
#define _SM_PARSER_FILEPARSER_SPDPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SPDParser : public IO::FileParser
		{
		public:
			SPDParser();
			virtual ~SPDParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
