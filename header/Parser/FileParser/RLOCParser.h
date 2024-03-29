#ifndef _SM_PARSER_FILEPARSER_RLOCPARSER
#define _SM_PARSER_FILEPARSER_RLOCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class RLOCParser : public IO::FileParser
		{
		public:
			RLOCParser();
			virtual ~RLOCParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
