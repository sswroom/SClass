#ifndef _SM_PARSER_FILEPARSER_JSONPARSER
#define _SM_PARSER_FILEPARSER_JSONPARSER
#include "IO/IFileParser.h"
#include "Math/Vector2D.h"
#include "Text/JSON.h"

namespace Parser
{
	namespace FileParser
	{
		class JSONParser : public IO::IFileParser
		{
		public:
			JSONParser();
			virtual ~JSONParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);

			static Math::Vector2D *ParseGeomJSON(Text::JSONObject *obj, Int32 srid);
		};
	};
};
#endif
