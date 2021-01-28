#ifndef _SM_DB_DBMANAGER
#define _SM_DB_DBMANAGER
#include "Data/ArrayList.h"
#include "DB/DBTool.h"
#include "Net/SocketFactory.h"
#include "Text/StringBuilderUTF.h"

namespace DB
{
	class DBManager
	{
	public:
		static Bool GetConnStr(DB::DBTool *db, Text::StringBuilderUTF *connStr);
		static DB::DBTool *OpenConn(const UTF8Char *connStr, IO::LogTool *log, Net::SocketFactory *sockf);
		static Bool StoreConn(const UTF8Char *fileName, Data::ArrayList<DB::DBTool*> *dbList);
		static Bool RestoreConn(const UTF8Char *fileName, Data::ArrayList<DB::DBTool*> *dbList, IO::LogTool *log, Net::SocketFactory *sockf);
	};
}
#endif
