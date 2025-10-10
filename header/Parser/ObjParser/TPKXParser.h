#ifndef _SM_PARSER_OBJPARSER_TPKXPARSER
#define _SM_PARSER_OBJPARSER_TPKXPARSER
#include "IO/ObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class TPKXParser : public IO::ObjectParser
		{
		public:
			TPKXParser();
			virtual ~TPKXParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
