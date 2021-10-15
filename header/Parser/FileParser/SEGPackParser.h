#ifndef _SM_PARSER_FILEPARSER_SEGPACKPARSER
#define _SM_PARSER_FILEPARSER_SEGPACKPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SEGPackParser : public IO::FileParser
		{
		public:
			SEGPackParser();
			virtual ~SEGPackParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
