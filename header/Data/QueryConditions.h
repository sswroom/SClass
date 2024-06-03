#ifndef _SM_DATA_QUERYCONDITIONS
#define _SM_DATA_QUERYCONDITIONS
#include "Data/ArrayList.h"
#include "Data/ArrayListArr.h"
#include "Data/ArrayListStringNN.h"
#include "Data/Condition.h"
#include "Data/ObjectGetter.h"
#include "Data/VariObject.h"
#include "DB/DBUtil.h"
#include "Text/CString.h"

namespace Data
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
			StringNotIn,
			StringContains,
			StringEquals,
			Boolean,
			NotNull,
			Inner,
			Or,
			INT64
		};

		class Condition
		{
		public:
			virtual ~Condition(){};

			virtual ConditionType GetType() = 0;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) = 0;
			virtual Bool IsValid(NN<Data::VariObject> obj) = 0;
			virtual Bool IsValid(NN<Data::ObjectGetter> getter) = 0;
			virtual void GetFieldList(NN<Data::ArrayListStringNN> fieldList) = 0;
		};

		class FieldCondition : public Condition
		{
		protected:
			NN<Text::String> fieldName;
		public:
			FieldCondition(Text::CStringNN fieldName);
			FieldCondition(NN<Text::String> fieldName);
			virtual ~FieldCondition();

			virtual Bool IsValid(NN<Data::VariObject> obj);
			virtual Bool IsValid(NN<Data::ObjectGetter> getter);
			virtual void GetFieldList(NN<Data::ArrayListStringNN> fieldList);

			virtual Bool TestValid(NN<Data::VariItem> item) = 0;
		};

		class TimeBetweenCondition : public FieldCondition
		{
		private:
			Data::Timestamp t1;
			Data::Timestamp t2;

		public:
			TimeBetweenCondition(Text::CStringNN fieldName, const Data::Timestamp &t1, const Data::Timestamp &t2);
			virtual ~TimeBetweenCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NN<Data::VariItem> item);
		};

		class Int32Condition : public FieldCondition
		{
		private:
			Int32 val;
			CompareCondition cond;

		public:
			Int32Condition(Text::CStringNN fieldName, Int32 val, CompareCondition cond);
			virtual ~Int32Condition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NN<Data::VariItem> item);

			Text::String *GetFieldName();
			Int32 GetVal();
			CompareCondition GetCompareCond();
		};

		class Int32InCondition : public FieldCondition
		{
		private:
			Data::ArrayList<Int32> vals;

		public:
			Int32InCondition(Text::CStringNN fieldName, NN<Data::ArrayList<Int32>> val);
			virtual ~Int32InCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NN<Data::VariItem> item);
		};

		class Int64Condition : public FieldCondition
		{
		private:
			Int64 val;
			CompareCondition cond;

		public:
			Int64Condition(Text::CStringNN fieldName, Int64 val, CompareCondition cond);
			virtual ~Int64Condition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NN<Data::VariItem> item);

			Text::String *GetFieldName();
			Int64 GetVal();
			CompareCondition GetCompareCond();
		};

		class DoubleCondition : public FieldCondition
		{
		private:
			Double val;
			CompareCondition cond;

		public:
			DoubleCondition(Text::CStringNN fieldName, Double val, CompareCondition cond);
			virtual ~DoubleCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NN<Data::VariItem> item);
		};

		class StringInCondition : public FieldCondition
		{
		private:
			Data::ArrayListArr<const UTF8Char> vals;

		public:
			StringInCondition(Text::CStringNN fieldName, NN<Data::ArrayListArr<const UTF8Char>> val);
			virtual ~StringInCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NN<Data::VariItem> item);
		};

		class StringNotInCondition : public FieldCondition
		{
		private:
			Data::ArrayListArr<const UTF8Char> vals;

		public:
			StringNotInCondition(Text::CStringNN fieldName, NN<Data::ArrayListArr<const UTF8Char>> val);
			virtual ~StringNotInCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NN<Data::VariItem> item);
		};

		class StringContainsCondition : public FieldCondition
		{
		private:
			NN<Text::String> val;

		public:
			StringContainsCondition(Text::CStringNN fieldName, UnsafeArray<const UTF8Char> val);
			virtual ~StringContainsCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NN<Data::VariItem> item);
		};

		class StringEqualsCondition : public FieldCondition
		{
		private:
			NN<Text::String> val;

		public:
			StringEqualsCondition(Text::CStringNN fieldName, Text::CStringNN val);
			virtual ~StringEqualsCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NN<Data::VariItem> item);
		};

		class BooleanCondition : public FieldCondition
		{
		private:
			Bool val;

		public:
			BooleanCondition(Text::CStringNN fieldName, Bool val);
			virtual ~BooleanCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NN<Data::VariItem> item);
		};

		class NotNullCondition : public FieldCondition
		{
		public:
			NotNullCondition(Text::CStringNN fieldName);
			virtual ~NotNullCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NN<Data::VariItem> item);
		};

		class InnerCondition : public Condition
		{
		private:
			NN<QueryConditions> innerCond;

		public:
			InnerCondition(NN<QueryConditions> innerCond);
			virtual ~InnerCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool IsValid(NN<Data::VariObject> obj);
			virtual Bool IsValid(NN<Data::ObjectGetter> getter);
			virtual void GetFieldList(NN<Data::ArrayListStringNN> fieldList);

			NN<QueryConditions> GetConditions();
		};

		class OrCondition : public Condition
		{
		public:
			OrCondition();
			virtual ~OrCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool IsValid(NN<Data::VariObject> obj);
			virtual Bool IsValid(NN<Data::ObjectGetter> getter);
			virtual void GetFieldList(NN<Data::ArrayListStringNN> fieldList);
		};

	private:
		Data::ArrayListNN<Condition> conditionList;
	public:
		QueryConditions();
		~QueryConditions();

		Bool IsValid(NN<Data::VariObject> obj);
		Bool IsValid(NN<Data::ObjectGetter> getter);
		Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem, NN<Data::ArrayListNN<Condition>> clientConditions);
		UOSInt GetCount();
		Optional<Condition> GetItem(UOSInt index);
		NN<Data::ArrayListNN<Condition>> GetList();
		void GetFieldList(NN<Data::ArrayListStringNN> fieldList);

		NN<QueryConditions> TimeBetween(Text::CStringNN fieldName, const Data::Timestamp &t1, const Data::Timestamp &t2);
		NN<QueryConditions> Or();
		NN<QueryConditions> InnerCond(NN<QueryConditions> innerCond);
		NN<QueryConditions> Int32Equals(Text::CStringNN fieldName, Int32 val);
		NN<QueryConditions> Int64Equals(Text::CStringNN fieldName, Int64 val);
		NN<QueryConditions> Int32In(Text::CStringNN fieldName, NN<Data::ArrayList<Int32>> val);
		NN<QueryConditions> DoubleGE(Text::CStringNN fieldName, Double val);
		NN<QueryConditions> DoubleLE(Text::CStringNN fieldName, Double val);
		NN<QueryConditions> StrIn(Text::CStringNN fieldName, NN<Data::ArrayListArr<const UTF8Char>> vals);
		NN<QueryConditions> StrNotIn(Text::CStringNN fieldName, NN<Data::ArrayListArr<const UTF8Char>> vals);
		NN<QueryConditions> StrContains(Text::CStringNN fieldName, UnsafeArray<const UTF8Char> val);
		NN<QueryConditions> StrEquals(Text::CStringNN fieldName, Text::CStringNN val);
		NN<QueryConditions> BoolEquals(Text::CStringNN fieldName, Bool val);
		NN<QueryConditions> NotNull(Text::CStringNN fieldName);

		static Text::CStringNN CompareConditionGetStr(CompareCondition cond);
		static Bool ObjectValid(NN<Data::VariObject> obj, NN<Data::ArrayListNN<Condition>> conditionList);
		static Bool ObjectValid(NN<Data::ObjectGetter> getter, NN<Data::ArrayListNN<Condition>> conditionList);
		static QueryConditions *ParseStr(Text::CStringNN s, DB::SQLType sqlType);
	};
}
#endif
