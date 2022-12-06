#ifndef _SM_PARSER_FILEPARSER_MEDPARSER
#define _SM_PARSER_FILEPARSER_MEDPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MEDParser : public IO::FileParser
		{
		public:
			MEDParser();
			virtual ~MEDParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
