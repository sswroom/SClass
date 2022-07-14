#ifndef _SM_DB_SHAREDREADINGDB
#define _SM_DB_SHAREDREADINGDB
#include "DB/ReadingDB.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace DB
{
	class SharedReadingDB
	{
	private:
		DB::ReadingDB *db;
		Sync::Mutex mutDB;
		Sync::Mutex mutObj;
		OSInt useCnt;

	private:
		~SharedReadingDB();
	public:
		SharedReadingDB(DB::ReadingDB *conn);

		void Reconnect();
		void UseObject();
		void UnuseObject();

		DB::ReadingDB *UseDB(Sync::MutexUsage *mutUsage);
	};
}
#endif
