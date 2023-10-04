#ifndef _SM_PARSER_FILEPARSER_ADXPARSER
#define _SM_PARSER_FILEPARSER_ADXPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ADXParser : public IO::FileParser
		{
		public:
			ADXParser();
			virtual ~ADXParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
