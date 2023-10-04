#ifndef _SM_PARSER_FILEPARSER_M2VSTMPARSER
#define _SM_PARSER_FILEPARSER_M2VSTMPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class M2VStmParser : public IO::FileParser
		{
		public:
			M2VStmParser();
			virtual ~M2VStmParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
