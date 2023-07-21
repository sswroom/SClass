#ifndef _SM_PARSER_FILEPARSER_BURIKOARCPARSER
#define _SM_PARSER_FILEPARSER_BURIKOARCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class BurikoArcParser : public IO::FileParser
		{
		public:
			BurikoArcParser();
			virtual ~BurikoArcParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
