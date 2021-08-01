#ifndef _SM_PARSER_FILEPARSER_LUTPARSER
#define _SM_PARSER_FILEPARSER_LUTPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class LUTParser : public IO::FileParser
		{
		public:
			LUTParser();
			virtual ~LUTParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
