#ifndef _SM_DB_DBLIST
#define _SM_DB_DBLIST
#include "Data/ArrayList.h"
#include "DB/DBTool.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace DB
{
	class DBList
	{
	private:
		typedef struct
		{
			NN<DB::DBTool> db;
			Bool isUsing;
		} DBInfo;

	private:
		Data::ArrayListNN<DBInfo> dbList;
		Sync::Mutex dbMut;
		Sync::Event dbEvt;
		UOSInt nextIndex;

	public:
		DBList();
		~DBList();

		void Close();
		void AddDB(NN<DB::DBTool> db);
		Optional<DB::DBTool> UseDB();
		void UnuseDB(NN<DB::DBTool> db);
		UOSInt GetCount() const;
	};
}
#endif
