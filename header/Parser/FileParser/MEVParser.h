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
			Parser::ParserList *parsers;
			Map::MapManager *mapMgr;
		public:
			MEVParser();
			virtual ~MEVParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void SetMapManager(Map::MapManager *mapMgr);
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

		private:
			void ReadItems(IO::StreamData *fd, Map::MapEnv *env, UInt32 itemCnt, UInt32 *currPos, Map::MapEnv::GroupItem *group, const WChar **dirArr, MEVImageInfo *imgInfos);
		};
	}
}
#endif
