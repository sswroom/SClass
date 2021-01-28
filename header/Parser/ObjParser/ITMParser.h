#ifndef _SM_PARSER_OBJPARSER_ITMPARSER
#define _SM_PARSER_OBJPARSER_ITMPARSER
#include "IO/IObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class ITMParser : public IO::IObjectParser
		{
		private:
			Parser::ParserList *parsers;

		public:
			ITMParser();
			virtual ~ITMParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
