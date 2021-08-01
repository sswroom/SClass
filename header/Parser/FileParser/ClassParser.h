#ifndef _SM_PARSER_FILEPARSER_CLASSPARSER
#define _SM_PARSER_FILEPARSER_CLASSPARSER
#include "IO/FileParser.h"
#include "IO/JavaClass.h"

namespace Parser
{
	namespace FileParser
	{
		class ClassParser : public IO::FileParser
		{
		public:
			ClassParser();
			virtual ~ClassParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
