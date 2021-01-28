#ifndef _SM_PARSER_OBJPARSER_DBMAPPARSER
#define _SM_PARSER_OBJPARSER_DBMAPPARSER
#include "IO/IObjectParser.h"

namespace Parser
{
	namespace ObjParser
	{
		class DBMapParser : public IO::IObjectParser
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
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
