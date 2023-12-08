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
		static Optional<DB::DBTool> LoadFromConfig(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<IO::ConfigFile> cfg, NotNullPtr<IO::LogTool> log);
	};
}
#endif
