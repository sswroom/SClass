#ifndef _SM_DB_DBCLASSBUILDER
#define _SM_DB_DBCLASSBUILDER
#include "Data/Class.h"
#include "DB/DBUtil.h"

namespace DB
{
	class DBClassBuilder
	{
	private:
		Data::Class *cls;
		OSInt currPos;
	public:
		DBClassBuilder();
		~DBClassBuilder();

		void AddItem(const UTF8Char *colName, DB::DBUtil::ColType colType);

		Data::Class *GetResultClass();
	};
}
#endif