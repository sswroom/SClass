#ifndef _SM_PARSER_OBJPARSER_MPGXAPARSER
#define _SM_PARSER_OBJPARSER_MPGXAPARSER
#include "IO/ObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class MPGXAParser : public IO::ObjectParser
		{
		public:
			MPGXAParser();
			virtual ~MPGXAParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(NotNullPtr<IO::ParsedObject> pobj, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
