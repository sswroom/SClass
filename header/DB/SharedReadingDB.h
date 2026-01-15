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
		NN<DB::ReadingDB> db;
		Sync::Mutex mutDB;
		Sync::Mutex mutObj;
		IntOS useCnt;

	private:
		~SharedReadingDB();
	public:
		SharedReadingDB(NN<DB::ReadingDB> conn);

		void Reconnect();
		void UseObject();
		void UnuseObject();

		NN<DB::ReadingDB> UseDB(NN<Sync::MutexUsage> mutUsage);
	};
}
#endif
