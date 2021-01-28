#ifndef _SM_PARSER_FILEPARSER_GUIIMGPARSER
#define _SM_PARSER_FILEPARSER_GUIIMGPARSER
#include "IO/IFileParser.h"
#include "Sync/Mutex.h"

namespace Parser
{
	namespace FileParser
	{
		class GUIImgParser : public IO::IFileParser
		{
		private:
			void *clsData;
			Parser::ParserList *parsers;
		public:
			GUIImgParser();
			virtual ~GUIImgParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};

#endif
