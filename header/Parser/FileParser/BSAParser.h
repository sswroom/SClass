#ifndef _SM_PARSER_FILEPARSER_BSAPARSER
#define _SM_PARSER_FILEPARSER_BSAPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class BSAParser : public IO::FileParser
		{
		public:
			BSAParser();
			virtual ~BSAParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
