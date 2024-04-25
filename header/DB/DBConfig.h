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
		static Optional<DB::DBTool> LoadFromConfig(NN<Net::SocketFactory> sockf, NN<IO::ConfigFile> cfg, Text::CString cfgCategory, NN<IO::LogTool> log);
	};
}
#endif
