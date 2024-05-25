#ifndef _SM_PARSER_FILEPARSER_MEVPARSER
#define _SM_PARSER_FILEPARSER_MEVPARSER
#include "IO/FileParser.h"
#include "Map/MapEnv.h"
#include "Text/Encoding.h"

namespace Parser
{
	namespace FileParser
	{
		class MEVParser : public IO::FileParser
		{
		private:
			typedef struct
			{
				Int32 fileIndex;
				OSInt envIndex;
			} MEVImageInfo;
		private:
			Optional<Parser::ParserList> parsers;
			Optional<Map::MapManager> mapMgr;
		public:
			MEVParser();
			virtual ~MEVParser();

			virtual Int32 GetName();
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void SetMapManager(Optional<Map::MapManager> mapMgr);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

		private:
			static void ReadItems(NN<IO::StreamData> fd, NN<Map::MapEnv> env, UInt32 itemCnt, InOutParam<UInt32> currPos, Optional<Map::MapEnv::GroupItem> group, const WChar **dirArr, MEVImageInfo *imgInfos, NN<Parser::ParserList> parsers, NN<Map::MapManager> mapMgr);
		};
	}
}
#endif
