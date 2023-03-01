#ifndef _SM_PARSER_FILEPARSER_M2VPARSER
#define _SM_PARSER_FILEPARSER_M2VPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class M2VParser : public IO::FileParser
		{
		public:
			M2VParser();
			virtual ~M2VParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
