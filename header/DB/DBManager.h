#ifndef _SM_DB_DBMANAGER
#define _SM_DB_DBMANAGER
#include "Data/ArrayListNN.h"
#include "DB/DBManagerCtrl.h"
#include "Parser/ParserList.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class DBManager
	{
	public:
		static Bool GetConnStr(NN<DB::DBTool> db, NN<Text::StringBuilderUTF8> connStr);
		static Optional<DB::ReadingDB> OpenConn(NN<Text::String> connStr, NN<IO::LogTool> log, NN<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
		static Optional<DB::ReadingDB> OpenConn(Text::CStringNN connStr, NN<IO::LogTool> log, NN<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
		static void GetConnName(Text::CString connStr, NN<Text::StringBuilderUTF8> sb);
		static Bool StoreConn(Text::CStringNN fileName, NN<Data::ArrayListNN<DB::DBManagerCtrl>> ctrlList);
		static Bool RestoreConn(Text::CStringNN fileName, NN<Data::ArrayListNN<DB::DBManagerCtrl>> ctrlList, NN<IO::LogTool> log, NN<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
	};
}
#endif
