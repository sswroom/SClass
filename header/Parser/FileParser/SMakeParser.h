#ifndef _SM_PARSER_FILEPARSER_SMAKEPARSER
#define _SM_PARSER_FILEPARSER_SMAKEPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SMakeParser : public IO::FileParser
		{
		public:
			SMakeParser();
			virtual ~SMakeParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
