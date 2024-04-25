#ifndef _SM_PARSER_FILEPARSER_IS2PARSER
#define _SM_PARSER_FILEPARSER_IS2PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class IS2Parser : public IO::FileParser
		{
		public:
			IS2Parser();
			virtual ~IS2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
