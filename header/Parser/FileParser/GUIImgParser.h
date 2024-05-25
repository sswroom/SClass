#ifndef _SM_PARSER_FILEPARSER_GUIIMGPARSER
#define _SM_PARSER_FILEPARSER_GUIIMGPARSER
#include "IO/FileParser.h"
#include "Sync/Mutex.h"

namespace Parser
{
	namespace FileParser
	{
		class GUIImgParser : public IO::FileParser
		{
		private:
			struct ClassData;
			ClassData *clsData;
			Optional<Parser::ParserList> parsers;
		public:
			GUIImgParser();
			virtual ~GUIImgParser();

			virtual Int32 GetName();
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}

#endif
