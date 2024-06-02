#ifndef _SM_DB_SDFFILE
#define _SM_DB_SDFFILE
#include "DB/OLEDBConn.h"

namespace DB
{
	class SDFFile : public DB::OLEDBConn
	{
	public:
		typedef enum
		{
			VER_30,
			VER_35,
			VER_40
		} Version;
	public:
		SDFFile(UnsafeArray<const UTF8Char> fileName, Version ver, NN<IO::LogTool> log, UnsafeArrayOpt<const UTF8Char> passowrd);
	};
}
#endif
