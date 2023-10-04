#ifndef _SM_PARSER_FILEPARSER_PACPARSER
#define _SM_PARSER_FILEPARSER_PACPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class PACParser : public IO::FileParser
		{
		public:
			PACParser();
			virtual ~PACParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
