#ifndef _SM_PARSER_FILEPARSER_PCXPARSER
#define _SM_PARSER_FILEPARSER_PCXPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class PCXParser : public IO::FileParser
		{
		public:
			PCXParser();
			virtual ~PCXParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
