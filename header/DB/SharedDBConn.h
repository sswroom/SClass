#ifndef _SM_DB_SHAREDDBCONN
#define _SM_DB_SHAREDDBCONN
#include "DB/DBConn.h"
#include "Sync/Mutex.h"

namespace DB
{
	class SharedDBConn
	{
	private:
		DB::DBConn *conn;
		Sync::Mutex *mutConn;
		Sync::Mutex *mutObj;
		OSInt useCnt;

	private:
		~SharedDBConn();
	public:
		SharedDBConn(DB::DBConn *conn);

		DB::DBUtil::ServerType GetSvrType();
		void Reconnect();
		void UseObject();
		void UnuseObject();

		DB::DBConn *BeginUseConn();
		void EndUseConn();
	};
};
#endif
