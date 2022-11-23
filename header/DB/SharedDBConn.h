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
		DB::DBConn *conn;
		Sync::Mutex mutConn;
		Sync::Mutex mutObj;
		OSInt useCnt;

	private:
		~SharedDBConn();
	public:
		SharedDBConn(DB::DBConn *conn);

		DB::DBUtil::SQLType GetSQLType();
		void Reconnect();
		void UseObject();
		void UnuseObject();

		DB::DBConn *UseConn(Sync::MutexUsage *mutUsage);
	};
}
#endif
