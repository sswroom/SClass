#ifndef _SM_PARSER_OBJPARSER_DBITPARSER
#define _SM_PARSER_OBJPARSER_DBITPARSER
#include "IO/ObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class DBITParser : public IO::ObjectParser
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
			DBITParser();
			virtual ~DBITParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(NN<IO::ParsedObject> pobj, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
