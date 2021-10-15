#ifndef _SM_PARSER_FILEPARSER_IS2PARSER
#define _SM_PARSER_FILEPARSER_IS2PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class IS2Parser : public IO::FileParser
		{
		public:
			IS2Parser();
			virtual ~IS2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
