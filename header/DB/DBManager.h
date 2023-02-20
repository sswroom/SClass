#ifndef _SM_DB_DBMANAGER
#define _SM_DB_DBMANAGER
#include "Data/ArrayList.h"
#include "DB/DBManagerCtrl.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class DBManager
	{
	public:
		static Bool GetConnStr(DB::DBTool *db, Text::StringBuilderUTF8 *connStr);
		static DB::DBTool *OpenConn(Text::String *connStr, IO::LogTool *log, Net::SocketFactory *sockf);
		static DB::DBTool *OpenConn(Text::CString connStr, IO::LogTool *log, Net::SocketFactory *sockf);
		static void GetConnName(Text::CString connStr, Text::StringBuilderUTF8 *sb);
		static Bool StoreConn(Text::CString fileName, Data::ArrayList<DB::DBManagerCtrl*> *ctrlList);
		static Bool RestoreConn(Text::CString fileName, Data::ArrayList<DB::DBManagerCtrl*> *ctrlList, IO::LogTool *log, Net::SocketFactory *sockf);
	};
}
#endif
