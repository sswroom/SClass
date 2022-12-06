#ifndef _SM_PARSER_FILEPARSER_AOSPARSER
#define _SM_PARSER_FILEPARSER_AOSPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class AOSParser : public IO::FileParser
		{
		public:
			AOSParser();
			virtual ~AOSParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
