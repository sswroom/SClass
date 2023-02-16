#ifndef _SM_DB_DBMANAGER
#define _SM_DB_DBMANAGER
#include "Data/ArrayList.h"
#include "DB/DBTool.h"
#include "Net/SocketFactory.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class DBManager
	{
	public:
		static Bool GetConnStr(DB::DBTool *db, Text::StringBuilderUTF8 *connStr);
		static DB::DBTool *OpenConn(Text::String *connStr, IO::LogTool *log, Net::SocketFactory *sockf);
		static DB::DBTool *OpenConn(Text::CString connStr, IO::LogTool *log, Net::SocketFactory *sockf);
		static Bool StoreConn(Text::CString fileName, Data::ArrayList<DB::DBTool*> *dbList);
		static Bool RestoreConn(Text::CString fileName, Data::ArrayList<DB::DBTool*> *dbList, IO::LogTool *log, Net::SocketFactory *sockf);
	};
}
#endif
