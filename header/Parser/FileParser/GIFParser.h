#ifndef _SM_PARSER_FILEPARSER_GIFPARSER
#define _SM_PARSER_FILEPARSER_GIFPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class GIFParser : public IO::IFileParser
		{
		public:
			GIFParser();
			virtual ~GIFParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
