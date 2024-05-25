#ifndef _SM_PARSER_FILEPARSER_MD5PARSER
#define _SM_PARSER_FILEPARSER_MD5PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MD5Parser : public IO::FileParser
		{
		private:
			UInt32 codePage;
		public:
			MD5Parser();
			virtual ~MD5Parser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
