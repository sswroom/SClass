#ifndef _SM_DB_DBCLASSBUILDER
#define _SM_DB_DBCLASSBUILDER
#include "Data/Class.h"
#include "DB/DBUtil.h"

namespace DB
{
	class DBClassBuilder
	{
	private:
		NN<Data::Class> cls;
		OSInt currPos;
	public:
		DBClassBuilder();
		~DBClassBuilder();

		void AddItem(UnsafeArray<const UTF8Char> colName, DB::DBUtil::ColType colType, Bool notNull);

		NN<Data::Class> GetResultClass();
	};
}
#endif
