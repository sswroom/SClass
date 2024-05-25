#ifndef _SM_PARSER_FILEPARSER_SHPPARSER
#define _SM_PARSER_FILEPARSER_SHPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SHPParser : public IO::FileParser
		{
		private:
			UInt32 codePage;
			Optional<Math::ArcGISPRJParser> prjParser;
		public:
			SHPParser();
			virtual ~SHPParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void SetArcGISPRJParser(Optional<Math::ArcGISPRJParser> prjParser);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
