#ifndef _SM_PARSER_FILEPARSER_SAKUOTOARCPARSER
#define _SM_PARSER_FILEPARSER_SAKUOTOARCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SakuotoArcParser : public IO::FileParser
		{
		public:
			SakuotoArcParser();
			virtual ~SakuotoArcParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
