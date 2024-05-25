#ifndef _SM_PARSER_FILEPARSER_JSONPARSER
#define _SM_PARSER_FILEPARSER_JSONPARSER
#include "IO/FileParser.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/JSON.h"

namespace Parser
{
	namespace FileParser
	{
		class JSONParser : public IO::FileParser
		{
		private:
			Optional<Parser::ParserList> parsers;
		public:
			JSONParser();
			virtual ~JSONParser();

			virtual Int32 GetName();
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

			static IO::ParsedObject *ParseJSON(Text::JSONBase *fileJSON, NN<Text::String> sourceName, Text::CStringNN layerName, IO::ParserType targetType, Optional<IO::PackageFile> pkgFile, Optional<Parser::ParserList> parsers);
			static Math::Geometry::Vector2D *ParseGeomJSON(Text::JSONObject *obj, UInt32 srid);
			static Text::JSONArray *GetDataArray(Text::JSONBase *fileJSON);
		};
	}
}
#endif
