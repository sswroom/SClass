#ifndef _SM_DB_QUERYCONDITIONS
#define _SM_DB_QUERYCONDITIONS
#include "Data/ArrayList.h"
#include "Data/VariObject.h"
#include "DB/DBUtil.h"

namespace DB
{
	class QueryConditions
	{
	public:
		enum class ConditionType
		{
			Or
		};

		class Condition
		{
		public:
			virtual ~Condition(){};

			virtual ConditionType GetType() = 0;
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, UOSInt maxDBItem) = 0;
			virtual Bool IsValid(Data::VariObject *obj) = 0;
		};

		enum class CompareCondition
		{
			Equal,
			GreaterOrEqual,
			LessOrEqual,
			Greater,
			Less,
			NotEqual
		};
	private:
		Data::ArrayList<Condition*> *conditionList;
	public:
		QueryConditions();
		~QueryConditions();

		Bool IsValid(Data::VariObject *obj);
		Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, UOSInt maxDBItem, Data::ArrayList<Condition*> *clientConditions);
		UOSInt GetCount();
		Condition *GetItem(UOSInt index);
		Data::ArrayList<Condition*> *GetList();

		static const UTF8Char *CompareConditionGetStr(CompareCondition cond);
		static Bool ObjectValid(Data::VariObject *obj, Data::ArrayList<Condition*> *conditionList);
	};
}
#endif
