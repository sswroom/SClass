#ifndef _SM_PARSER_FILEPARSER_GZIPPARSER
#define _SM_PARSER_FILEPARSER_GZIPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class GZIPParser : public IO::FileParser
		{
		public:
			GZIPParser();
			virtual ~GZIPParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
