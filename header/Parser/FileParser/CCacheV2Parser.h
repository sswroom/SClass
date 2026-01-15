#ifndef _SM_PARSER_FILEPARSER_CCACHEV2PARSER
#define _SM_PARSER_FILEPARSER_CCACHEV2PARSER
#include "IO/FileParser.h"
#include "IO/VirtualPackageFile.h"

namespace Parser
{
	namespace FileParser
	{
		class CCacheV2Parser : public IO::FileParser
		{
		public:
			CCacheV2Parser();
			virtual ~CCacheV2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
			Bool ParseAppend(NN<IO::StreamData> fd, Data::ByteArrayR hdr, NN<IO::VirtualPackageFile> pkgFile, UIntOS rOfst, UIntOS cOfst);
		};
	}
}
#endif
