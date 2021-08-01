#ifndef _SM_PARSER_FILEPARSER_PCAPNGPARSER
#define _SM_PARSER_FILEPARSER_PCAPNGPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class PCAPNGParser : public IO::FileParser
		{
		public:
			PCAPNGParser();
			virtual ~PCAPNGParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
