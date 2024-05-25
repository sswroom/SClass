#ifndef _SM_PARSER_FILEPARSER_SM2MPXPARSER
#define _SM_PARSER_FILEPARSER_SM2MPXPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SM2MPXParser : public IO::FileParser
		{
		public:
			SM2MPXParser();
			virtual ~SM2MPXParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
