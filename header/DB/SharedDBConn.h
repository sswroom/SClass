#ifndef _SM_DB_SHAREDDBCONN
#define _SM_DB_SHAREDDBCONN
#include "DB/DBConn.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace DB
{
	class SharedDBConn
	{
	private:
		NN<DB::DBConn> conn;
		Sync::Mutex mutConn;
		Sync::Mutex mutObj;
		OSInt useCnt;

	private:
		~SharedDBConn();
	public:
		SharedDBConn(NN<DB::DBConn> conn);

		DB::SQLType GetSQLType();
		void Reconnect();
		void UseObject();
		void UnuseObject();

		NN<DB::DBConn> UseConn(NN<Sync::MutexUsage> mutUsage);
	};
}
#endif
