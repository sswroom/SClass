#ifndef _SM_PARSER_OBJPARSER_DBMAPPARSER
#define _SM_PARSER_OBJPARSER_DBMAPPARSER
#include "IO/ObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class DBMapParser : public IO::ObjectParser
		{
		private:
			typedef struct
			{
				Int32 gpsLogId;
				Int64 time;
				Double speed;
				Int32 wpId;
				Double latitude;
				Double longitude;
				Double altitude;
			} Record;
		public:
			DBMapParser();
			virtual ~DBMapParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
