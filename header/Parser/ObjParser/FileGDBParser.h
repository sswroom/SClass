#ifndef _SM_PARSER_OBJPARSER_FILEGDBPARSER
#define _SM_PARSER_OBJPARSER_FILEGDBPARSER
#include "IO/IObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class FileGDBParser : public IO::IObjectParser
		{
		public:
			FileGDBParser();
			virtual ~FileGDBParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
