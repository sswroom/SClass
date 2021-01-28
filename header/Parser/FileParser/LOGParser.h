#ifndef _SM_PARSER_FILEPARSER_LOGPARSER
#define _SM_PARSER_FILEPARSER_LOGPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class LOGParser : public IO::IFileParser
		{
		private:
			Int32 codePage;
		public:
			LOGParser();
			virtual ~LOGParser();

			virtual Int32 GetName();
			virtual void SetCodePage(Int32 codePage);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
