#ifndef _SM_PARSER_FILEPARSER_MTKGPSPARSER
#define _SM_PARSER_FILEPARSER_MTKGPSPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MTKGPSParser : public IO::FileParser
		{
		public:
			MTKGPSParser();
			virtual ~MTKGPSParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
