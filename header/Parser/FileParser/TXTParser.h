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
			Optional<Parser::ParserList> parsers;
			Optional<Map::MapManager> mapMgr;
		public:
			TXTParser();
			virtual ~TXTParser();

			virtual Int32 GetName();
			virtual void SetCodePage(UInt32 codePage);
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void SetMapManager(Optional<Map::MapManager> mapMgr);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);

			static UInt32 ToColor(UInt32 val);
		};
	}
}
#endif
