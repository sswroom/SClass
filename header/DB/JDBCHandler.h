#ifndef _SM_DB_JDBCHANDLER
#define _SM_DB_JDBCHANDLER
#include "DB/DBTool.h"
#include "Net/SocketFactory.h"

namespace DB
{
	class JDBCHandler
	{
	public:
		static DB::DBTool *OpenConn(Text::String *url, Text::String *username, Text::String *password, IO::LogTool *log, Net::SocketFactory *sockf);
	};
}
#endif
