#ifndef _SM_PARSER_OBJPARSER_C3DPARSER
#define _SM_PARSER_OBJPARSER_C3DPARSER
#include "IO/ObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class C3DParser : public IO::ObjectParser
		{
		private:
			Optional<Text::EncodingFactory> encFact;
		public:
			C3DParser();
			virtual ~C3DParser();

			virtual Int32 GetName();
			virtual void SetEncFactory(Optional<Text::EncodingFactory> encFact);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
