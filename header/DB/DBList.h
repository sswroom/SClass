#ifndef _SM_DB_DBLIST
#define _SM_DB_DBLIST
#include "Data/ArrayListNN.hpp"
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
		UIntOS nextIndex;

	public:
		DBList();
		~DBList();

		void Close();
		void AddDB(NN<DB::DBTool> db);
		Optional<DB::DBTool> UseDB();
		void UnuseDB(NN<DB::DBTool> db);
		UIntOS GetCount() const;
	};
}
#endif
