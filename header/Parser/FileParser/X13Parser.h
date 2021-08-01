#ifndef _SM_PARSER_FILEPARSER_X13PARSER
#define _SM_PARSER_FILEPARSER_X13PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class X13Parser : public IO::FileParser
		{
		public:
			X13Parser();
			virtual ~X13Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	}
}
#endif
