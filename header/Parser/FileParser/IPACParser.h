#ifndef _SM_PARSER_FILEPARSER_IPACPARSER
#define _SM_PARSER_FILEPARSER_IPACPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class IPACParser : public IO::FileParser
		{
		public:
			IPACParser();
			virtual ~IPACParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
