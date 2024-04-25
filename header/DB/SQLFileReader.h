#ifndef _SM_DB_SQLFILEREADER
#define _SM_DB_SQLFILEREADER
#include "DB/DBUtil.h"
#include "IO/Stream.h"
#include "Text/UTF8Reader.h"

namespace DB
{
	class SQLFileReader
	{
	private:
		Text::UTF8Reader reader;
		DB::SQLType sqlType;
		Bool mergeInsert;
		Text::StringBuilderUTF8 sbLastLine;
	public:
		SQLFileReader(NN<IO::Stream> stm, DB::SQLType sqlType, Bool mergeInsert);
		~SQLFileReader();

		Bool NextSQL(NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
