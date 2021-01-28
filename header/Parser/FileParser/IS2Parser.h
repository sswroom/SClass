#ifndef _SM_PARSER_FILEPARSER_IS2PARSER
#define _SM_PARSER_FILEPARSER_IS2PARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class IS2Parser : public IO::IFileParser
		{
		public:
			IS2Parser();
			virtual ~IS2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
