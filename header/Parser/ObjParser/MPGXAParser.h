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
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
