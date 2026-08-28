
#ifndef _SM_PARSER_FILEPARSER_SQLPARSER
#define _SM_PARSER_FILEPARSER_SQLPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SQLParser : public IO::FileParser
		{
		private:
			DB::SQLType defSQLType;
		public:
			SQLParser();
			virtual ~SQLParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
			void SetDefaultSQLType(DB::SQLType sqlType);
		};
	}
}
#endif
