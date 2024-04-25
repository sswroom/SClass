#ifndef _SM_PARSER_FILEPARSER_SFVPARSER
#define _SM_PARSER_FILEPARSER_SFVPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SFVParser : public IO::FileParser
		{
		public:
			SFVParser();
			virtual ~SFVParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
