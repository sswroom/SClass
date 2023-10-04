#ifndef _SM_PARSER_FILEPARSER_AC3PARSER
#define _SM_PARSER_FILEPARSER_AC3PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class AC3Parser : public IO::FileParser
		{
		public:
			AC3Parser();
			virtual ~AC3Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
