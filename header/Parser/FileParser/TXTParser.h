#ifndef _SM_PARSER_FILEPARSER_TXTPARSER
#define _SM_PARSER_FILEPARSER_TXTPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class TXTParser : public IO::IFileParser
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
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);

			static UInt32 ToColor(UInt32 val);
		};
	}
}
#endif
