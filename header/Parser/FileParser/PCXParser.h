#ifndef _SM_PARSER_FILEPARSER_PCXPARSER
#define _SM_PARSER_FILEPARSER_PCXPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class PCXParser : public IO::FileParser
		{
		public:
			PCXParser();
			virtual ~PCXParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
