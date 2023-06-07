#ifndef _SM_DB_DBCONFIG
#define _SM_DB_DBCONFIG
#include "DB/DBTool.h"
#include "IO/ConfigFile.h"
#include "Net/SocketFactory.h"

namespace DB
{
	class DBConfig
	{
	public:
		static DB::DBTool *LoadFromConfig(Net::SocketFactory *sockf, IO::ConfigFile *cfg, IO::LogTool *log);
	};
}
#endif
