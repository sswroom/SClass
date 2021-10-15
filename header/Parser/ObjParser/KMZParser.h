#ifndef _SM_PARSER_OBJPARSER_KMZPARSER
#define _SM_PARSER_OBJPARSER_KMZPARSER
#include "IO/IObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class KMZParser : public IO::IObjectParser
		{
		private:
			Parser::ParserList *parsers;

		public:
			KMZParser();
			virtual ~KMZParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
