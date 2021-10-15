#ifndef _SM_PARSER_FILEPARSER_MRGPARSER
#define _SM_PARSER_FILEPARSER_MRGPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MRGParser : public IO::FileParser
		{
		public:
			MRGParser();
			virtual ~MRGParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
