#ifndef _SM_PARSER_OBJPARSER_MPGXAPARSER
#define _SM_PARSER_OBJPARSER_MPGXAPARSER
#include "IO/IObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class MPGXAParser : public IO::IObjectParser
		{
		public:
			MPGXAParser();
			virtual ~MPGXAParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
