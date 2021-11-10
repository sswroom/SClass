#ifndef _SM_DB_QUERYCONDITIONS
#define _SM_DB_QUERYCONDITIONS
#include "Data/ArrayList.h"
#include "Data/ObjectGetter.h"
#include "Data/VariObject.h"
#include "DB/DBUtil.h"

namespace DB
{
	class QueryConditions
	{
	public:
		enum class ConditionType
		{
			TimeBetween,
			INT32,
			Int32In,
			DOUBLE,
			StringIn,
			StringContains,
			StringEquals,
			Boolean,
			NotNull,
			Inner,
			Or
		};

		class Condition
		{
		public:
			virtual ~Condition(){};

			virtual ConditionType GetType() = 0;
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem) = 0;
			virtual Bool IsValid(Data::VariObject *obj) = 0;
			virtual Bool IsValid(Data::ObjectGetter *getter) = 0;
			virtual void GetFieldList(Data::ArrayList<const UTF8Char*> *fieldList) = 0;
		};

		class FieldCondition : public Condition
		{
		protected:
			const UTF8Char *fieldName;
		public:
			FieldCondition(const UTF8Char *fieldName);
			virtual ~FieldCondition();

			virtual Bool IsValid(Data::VariObject *obj);
			virtual Bool IsValid(Data::ObjectGetter *getter);
			virtual void GetFieldList(Data::ArrayList<const UTF8Char*> *fieldList);

			virtual Bool TestValid(Data::VariItem *item) = 0;
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

		class TimeBetweenCondition : public FieldCondition
		{
		private:
			Int64 t1;
			Int64 t2;

		public:
			TimeBetweenCondition(const UTF8Char *fieldName, Int64 t1, Int64 t2);
			virtual ~TimeBetweenCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(Data::VariItem *item);
		};

		class Int32Condition : public FieldCondition
		{
		private:
			Int32 val;
			CompareCondition cond;

		public:
			Int32Condition(const UTF8Char *fieldName, Int32 val, CompareCondition cond);
			virtual ~Int32Condition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(Data::VariItem *item);

			const UTF8Char *GetFieldName();
			Int32 GetVal();
			CompareCondition GetCompareCond();
		};

		class Int32InCondition : public FieldCondition
		{
		private:
			Data::ArrayList<Int32> *vals;

		public:
			Int32InCondition(const UTF8Char *fieldName, Data::ArrayList<Int32> *val);
			virtual ~Int32InCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(Data::VariItem *item);
		};

		class DoubleCondition : public FieldCondition
		{
		private:
			Double val;
			CompareCondition cond;

		public:
			DoubleCondition(const UTF8Char *fieldName, Double val, CompareCondition cond);
			virtual ~DoubleCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(Data::VariItem *item);
		};

		class StringInCondition : public FieldCondition
		{
		private:
			Data::ArrayList<const UTF8Char*> *vals;

		public:
			StringInCondition(const UTF8Char *fieldName, Data::ArrayList<const UTF8Char*> *val);
			virtual ~StringInCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(Data::VariItem *item);
		};

		class StringContainsCondition : public FieldCondition
		{
		private:
			const UTF8Char *val;

		public:
			StringContainsCondition(const UTF8Char *fieldName, const UTF8Char *val);
			virtual ~StringContainsCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(Data::VariItem *item);
		};

		class StringEqualsCondition : public FieldCondition
		{
		private:
			const UTF8Char *val;

		public:
			StringEqualsCondition(const UTF8Char *fieldName, const UTF8Char *val);
			virtual ~StringEqualsCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(Data::VariItem *item);
		};

		class BooleanCondition : public FieldCondition
		{
		private:
			Bool val;

		public:
			BooleanCondition(const UTF8Char *fieldName, Bool val);
			virtual ~BooleanCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(Data::VariItem *item);
		};

		class NotNullCondition : public FieldCondition
		{
		public:
			NotNullCondition(const UTF8Char *fieldName);
			virtual ~NotNullCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(Data::VariItem *item);
		};

		class InnerCondition : public Condition
		{
		private:
			QueryConditions *innerCond;

		public:
			InnerCondition(QueryConditions *innerCond);
			virtual ~InnerCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool IsValid(Data::VariObject *obj);
			virtual Bool IsValid(Data::ObjectGetter *getter);
			virtual void GetFieldList(Data::ArrayList<const UTF8Char*> *fieldList);

			QueryConditions *GetConditions();
		};

		class OrCondition : public Condition
		{
		public:
			OrCondition();
			virtual ~OrCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool IsValid(Data::VariObject *obj);
			virtual Bool IsValid(Data::ObjectGetter *getter);
			virtual void GetFieldList(Data::ArrayList<const UTF8Char*> *fieldList);
		};

	private:
		Data::ArrayList<Condition*> *conditionList;
	public:
		QueryConditions();
		~QueryConditions();

		Bool IsValid(Data::VariObject *obj);
		Bool IsValid(Data::ObjectGetter *getter);
		Bool ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem, Data::ArrayList<Condition*> *clientConditions);
		UOSInt GetCount();
		Condition *GetItem(UOSInt index);
		Data::ArrayList<Condition*> *GetList();
		void GetFieldList(Data::ArrayList<const UTF8Char*> *fieldList);

		QueryConditions *TimeBetween(const UTF8Char *fieldName, Int64 t1, Int64 t2);
		QueryConditions *Or();
		QueryConditions *InnerCond(QueryConditions *cond);
		QueryConditions *Int32Equals(const UTF8Char *fieldName, Int32 val);
		QueryConditions *Int32In(const UTF8Char *fieldName, Data::ArrayList<Int32> *val);
		QueryConditions *DoubleGE(const UTF8Char *fieldName, Double val);
		QueryConditions *DoubleLE(const UTF8Char *fieldName, Double val);
		QueryConditions *StrIn(const UTF8Char *fieldName, Data::ArrayList<const UTF8Char*> *vals);
		QueryConditions *StrContains(const UTF8Char *fieldName, const UTF8Char *val);
		QueryConditions *StrEquals(const UTF8Char *fieldName, const UTF8Char *val);
		QueryConditions *BoolEquals(const UTF8Char *fieldName, Bool val);
		QueryConditions *NotNull(const UTF8Char* fieldName);

		static const UTF8Char *CompareConditionGetStr(CompareCondition cond);
		static Bool ObjectValid(Data::VariObject *obj, Data::ArrayList<Condition*> *conditionList);
		static Bool ObjectValid(Data::ObjectGetter *getter, Data::ArrayList<Condition*> *conditionList);
	};
}
#endif
