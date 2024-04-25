#ifndef _SM_PARSER_FILEPARSER_WAVPARSER
#define _SM_PARSER_FILEPARSER_WAVPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class WAVParser : public IO::FileParser
		{
		public:
			WAVParser();
			virtual ~WAVParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	};
};
#endif
