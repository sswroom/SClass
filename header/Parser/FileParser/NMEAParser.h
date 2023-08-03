#ifndef _SM_PARSER_FILEPARSER_NMEAPARSER
#define _SM_PARSER_FILEPARSER_NMEAPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class NMEAParser : public IO::FileParser
		{
		public:
			NMEAParser();
			virtual ~NMEAParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
