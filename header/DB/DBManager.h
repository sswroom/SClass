#ifndef _SM_DB_DBMANAGER
#define _SM_DB_DBMANAGER
#include "Data/ArrayList.h"
#include "DB/DBManagerCtrl.h"
#include "Parser/ParserList.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class DBManager
	{
	public:
		static Bool GetConnStr(DB::DBTool *db, NotNullPtr<Text::StringBuilderUTF8> connStr);
		static DB::ReadingDB *OpenConn(Text::String *connStr, IO::LogTool *log, NotNullPtr<Net::SocketFactory> sockf, Parser::ParserList *parsers);
		static DB::ReadingDB *OpenConn(Text::CString connStr, IO::LogTool *log, NotNullPtr<Net::SocketFactory> sockf, Parser::ParserList *parsers);
		static void GetConnName(Text::CString connStr, NotNullPtr<Text::StringBuilderUTF8> sb);
		static Bool StoreConn(Text::CString fileName, Data::ArrayList<DB::DBManagerCtrl*> *ctrlList);
		static Bool RestoreConn(Text::CString fileName, Data::ArrayList<DB::DBManagerCtrl*> *ctrlList, IO::LogTool *log, NotNullPtr<Net::SocketFactory> sockf, Parser::ParserList *parsers);
	};
}
#endif
