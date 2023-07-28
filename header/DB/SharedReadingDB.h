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
		NotNullPtr<DB::ReadingDB> db;
		Sync::Mutex mutDB;
		Sync::Mutex mutObj;
		OSInt useCnt;

	private:
		~SharedReadingDB();
	public:
		SharedReadingDB(NotNullPtr<DB::ReadingDB> conn);

		void Reconnect();
		void UseObject();
		void UnuseObject();

		NotNullPtr<DB::ReadingDB> UseDB(NotNullPtr<Sync::MutexUsage> mutUsage);
	};
}
#endif
