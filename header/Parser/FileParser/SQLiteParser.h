#ifndef _SM_PARSER_FILEPARSER_SQLITEPARSER
#define _SM_PARSER_FILEPARSER_SQLITEPARSER
#include "DB/DBConn.h"
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SQLiteParser : public IO::FileParser
		{
		public:
			SQLiteParser();
			virtual ~SQLiteParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

			static IO::ParsedObject *ParseAsMap(DB::DBConn *conn);
		};
	}
}
#endif
