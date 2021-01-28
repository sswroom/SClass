#ifndef _SM_PARSER_FILEPARSER_MEDPARSER
#define _SM_PARSER_FILEPARSER_MEDPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MEDParser : public IO::IFileParser
		{
		public:
			MEDParser();
			virtual ~MEDParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
