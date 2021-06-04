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
			DB::DBTool *db;
			Bool isUsing;
		} DBInfo;

	private:
		Data::ArrayList<DBInfo *> *dbList;
		Sync::Mutex *dbMut;
		Sync::Event *dbEvt;
		UOSInt nextIndex;

	public:
		DBList();
		~DBList();

		void AddDB(DB::DBTool *db);
		DB::DBTool *UseDB();
		void UnuseDB(DB::DBTool *db);
		UOSInt GetCount();
	};
};
#endif
