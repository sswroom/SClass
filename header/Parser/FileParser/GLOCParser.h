#ifndef _SM_PARSER_FILEPARSER_GLOCPARSER
#define _SM_PARSER_FILEPARSER_GLOCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class GLOCParser : public IO::FileParser
		{
		public:
			GLOCParser();
			virtual ~GLOCParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
