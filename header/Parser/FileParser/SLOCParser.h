#ifndef _SM_PARSER_FILEPARSER_SLOCPARSER
#define _SM_PARSER_FILEPARSER_SLOCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SLOCParser : public IO::FileParser
		{
		public:
			SLOCParser();
			virtual ~SLOCParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
