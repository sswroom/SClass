#ifndef _SM_PARSER_FILEPARSER_HTRECPARSER
#define _SM_PARSER_FILEPARSER_HTRECPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class HTRecParser : public IO::FileParser
		{
		public:
			HTRecParser();
			virtual ~HTRecParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
