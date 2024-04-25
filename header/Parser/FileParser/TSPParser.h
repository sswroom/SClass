#ifndef _SM_PARSER_FILEPARSER_TSPPARSER
#define _SM_PARSER_FILEPARSER_TSPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class TSPParser : public IO::FileParser
		{
		public:
			TSPParser();
			virtual ~TSPParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
