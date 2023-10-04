#ifndef _SM_PARSER_OBJPARSER_ITMPARSER
#define _SM_PARSER_OBJPARSER_ITMPARSER
#include "IO/ObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class ITMParser : public IO::ObjectParser
		{
		private:
			Parser::ParserList *parsers;

		public:
			ITMParser();
			virtual ~ITMParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
