#ifndef _SM_PARSER_FILEPARSER_ICOPARSER
#define _SM_PARSER_FILEPARSER_ICOPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ICOParser : public IO::FileParser
		{
		public:
			ICOParser();
			virtual ~ICOParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
