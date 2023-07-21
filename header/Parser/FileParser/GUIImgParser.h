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
			Parser::ParserList *parsers;
		public:
			GUIImgParser();
			virtual ~GUIImgParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}

#endif
