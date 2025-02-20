#ifndef _SM_PARSER_OBJPARSER_PKGTIFFPARSER
#define _SM_PARSER_OBJPARSER_PKGTIFFPARSER
#include "Data/ArrayListNN.h"
#include "IO/ObjectParser.h"
#include "Map/MapDrawLayer.h"

namespace Parser
{
	namespace ObjParser
	{
		class PKGTIFFParser : public IO::ObjectParser
		{
		public:
			PKGTIFFParser();
			virtual ~PKGTIFFParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType);
			static void ParsePackage(NN<IO::PackageFile> pkg, NN<Data::ArrayListNN<Map::MapDrawLayer>> layers);
		};
	}
}
#endif
