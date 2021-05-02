#ifndef _SM_PARSER_FILEPARSER_MEVPARSER
#define _SM_PARSER_FILEPARSER_MEVPARSER
#include "IO/IFileParser.h"
#include "Map/MapEnv.h"
#include "Text/Encoding.h"

namespace Parser
{
	namespace FileParser
	{
		class MEVParser : public IO::IFileParser
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
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);

		private:
			void ReadItems(IO::IStreamData *fd, Map::MapEnv *env, UInt32 itemCnt, Int32 *currPos, Map::MapEnv::GroupItem *group, const WChar **dirArr, MEVImageInfo *imgInfos);
		};
	}
}
#endif
