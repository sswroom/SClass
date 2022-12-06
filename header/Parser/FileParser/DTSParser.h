#ifndef _SM_PARSER_FILEPARSER_DTSPARSER
#define _SM_PARSER_FILEPARSER_DTSPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class DTSParser : public IO::FileParser
		{
		public:
			DTSParser();
			virtual ~DTSParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
