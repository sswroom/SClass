#ifndef _SM_PARSER_OBJPARSER_PKGMAPPARSER
#define _SM_PARSER_OBJPARSER_PKGMAPPARSER
#include "Data/ArrayListNN.h"
#include "IO/ObjectParser.h"
#include "Map/MapDrawLayer.h"

namespace Parser
{
	namespace ObjParser
	{
		class PKGMapParser : public IO::ObjectParser
		{
		private:
			Optional<Text::EncodingFactory> encFact;
		public:
			PKGMapParser();
			virtual ~PKGMapParser();

			virtual Int32 GetName();
			virtual void SetEncFactory(Optional<Text::EncodingFactory> encFact) { this->encFact = encFact; }
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType);
			static void ParsePackage(NN<IO::PackageFile> pkg, NN<Data::ArrayListNN<Map::MapDrawLayer>> layers, Optional<Text::EncodingFactory> encFact);
		};
	}
}
#endif
