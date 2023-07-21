#ifndef _SM_PARSER_FILEPARSER_TXTPARSER
#define _SM_PARSER_FILEPARSER_TXTPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class TXTParser : public IO::FileParser
		{
		private:
			UInt32 codePage;
			Parser::ParserList *parsers;
			Map::MapManager *mapMgr;
		public:
			TXTParser();
			virtual ~TXTParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void SetMapManager(Map::MapManager *mapMgr);
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

			static UInt32 ToColor(UInt32 val);
		};
	}
}
#endif
