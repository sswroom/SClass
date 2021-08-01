#ifndef _SM_PARSER_FILEPARSER_LINKARCPARSER
#define _SM_PARSER_FILEPARSER_LINKARCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class LinkArcParser : public IO::FileParser
		{
		public:
			LinkArcParser();
			virtual ~LinkArcParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
