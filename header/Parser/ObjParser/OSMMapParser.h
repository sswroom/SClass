#ifndef _SM_PARSER_OBJPARSER_OSMMAPPARSER
#define _SM_PARSER_OBJPARSER_OSMMAPPARSER
#include "IO/ObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class OSMMapParser : public IO::ObjectParser
		{
		private:
			Optional<Parser::ParserList> parsers;
		public:
			OSMMapParser();
			virtual ~OSMMapParser();

			virtual Int32 GetName();
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
