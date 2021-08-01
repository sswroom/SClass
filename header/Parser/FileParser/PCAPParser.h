#ifndef _SM_PARSER_FILEPARSER_PCAPPARSER
#define _SM_PARSER_FILEPARSER_PCAPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class PCAPParser : public IO::FileParser
		{
		public:
			PCAPParser();
			virtual ~PCAPParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
