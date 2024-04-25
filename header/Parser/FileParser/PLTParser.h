#ifndef _SM_PARSER_FILEPARSER_PLTPARSER
#define _SM_PARSER_FILEPARSER_PLTPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class PLTParser : public IO::FileParser
		{
		public:
			PLTParser();
			virtual ~PLTParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
