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
		static Bool GetConnStr(NotNullPtr<DB::DBTool> db, NotNullPtr<Text::StringBuilderUTF8> connStr);
		static Optional<DB::ReadingDB> OpenConn(NotNullPtr<Text::String> connStr, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
		static Optional<DB::ReadingDB> OpenConn(Text::CStringNN connStr, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
		static void GetConnName(Text::CString connStr, NotNullPtr<Text::StringBuilderUTF8> sb);
		static Bool StoreConn(Text::CStringNN fileName, NotNullPtr<Data::ArrayListNN<DB::DBManagerCtrl>> ctrlList);
		static Bool RestoreConn(Text::CStringNN fileName, NotNullPtr<Data::ArrayListNN<DB::DBManagerCtrl>> ctrlList, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
	};
}
#endif
