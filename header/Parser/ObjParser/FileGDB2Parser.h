#ifndef _SM_PARSER_OBJPARSER_FILEGDB2PARSER
#define _SM_PARSER_OBJPARSER_FILEGDB2PARSER
#include "IO/IObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class FileGDB2Parser : public IO::IObjectParser
		{
		public:
			FileGDB2Parser();
			virtual ~FileGDB2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	}
}
#endif