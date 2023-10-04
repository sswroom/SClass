#ifndef _SM_PARSER_FILEPARSER_NWAPARSER
#define _SM_PARSER_FILEPARSER_NWAPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class NWAParser : public IO::FileParser
		{
		public:
			NWAParser();
			virtual ~NWAParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
