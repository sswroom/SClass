#ifndef _SM_PARSER_FILEPARSER_LOGPARSER
#define _SM_PARSER_FILEPARSER_LOGPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class LOGParser : public IO::FileParser
		{
		private:
			UInt32 codePage;
		public:
			LOGParser();
			virtual ~LOGParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
