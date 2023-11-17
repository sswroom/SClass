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
			Parser::ParserList *parsers;
		public:
			OSMMapParser();
			virtual ~OSMMapParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(NotNullPtr<IO::ParsedObject> pobj, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
