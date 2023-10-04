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
			Math::ArcGISPRJParser *prjParser;
		public:
			SHPParser();
			virtual ~SHPParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void SetArcGISPRJParser(Math::ArcGISPRJParser *prjParser);
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
