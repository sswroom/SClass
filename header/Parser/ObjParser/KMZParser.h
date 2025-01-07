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
			Optional<Text::EncodingFactory> encFact;
			Optional<Net::WebBrowser> browser;

		public:
			KMZParser();
			virtual ~KMZParser();

			virtual Int32 GetName();
			virtual void SetWebBrowser(Optional<Net::WebBrowser> browser);
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void SetEncFactory(Optional<Text::EncodingFactory> encFact);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
