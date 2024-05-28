#ifndef _SM_PARSER_OBJPARSER_KMZPARSER
#define _SM_PARSER_OBJPARSER_KMZPARSER
#include "IO/ObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class KMZParser : public IO::ObjectParser
		{
		private:
			Optional<Parser::ParserList> parsers;

		public:
			KMZParser();
			virtual ~KMZParser();

			virtual Int32 GetName();
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
