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
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);

			static Optional<IO::ParsedObject> ParseAsMap(DB::DBConn *conn);
		};
	}
}
#endif
