#ifndef _SM_DATA_QUERYCONDITIONS
#define _SM_DATA_QUERYCONDITIONS
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
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
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) = 0;
			virtual Bool IsValid(NotNullPtr<Data::VariObject> obj) = 0;
			virtual Bool IsValid(NotNullPtr<Data::ObjectGetter> getter) = 0;
			virtual void GetFieldList(NotNullPtr<Data::ArrayListNN<Text::String>> fieldList) = 0;
		};

		class FieldCondition : public Condition
		{
		protected:
			NotNullPtr<Text::String> fieldName;
		public:
			FieldCondition(Text::CStringNN fieldName);
			FieldCondition(NotNullPtr<Text::String> fieldName);
			virtual ~FieldCondition();

			virtual Bool IsValid(NotNullPtr<Data::VariObject> obj);
			virtual Bool IsValid(NotNullPtr<Data::ObjectGetter> getter);
			virtual void GetFieldList(NotNullPtr<Data::ArrayListNN<Text::String>> fieldList);

			virtual Bool TestValid(NotNullPtr<Data::VariItem> item) = 0;
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
			Data::Timestamp t1;
			Data::Timestamp t2;

		public:
			TimeBetweenCondition(Text::CStringNN fieldName, const Data::Timestamp &t1, const Data::Timestamp &t2);
			virtual ~TimeBetweenCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NotNullPtr<Data::VariItem> item);
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
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NotNullPtr<Data::VariItem> item);

			Text::String *GetFieldName();
			Int32 GetVal();
			CompareCondition GetCompareCond();
		};

		class Int32InCondition : public FieldCondition
		{
		private:
			Data::ArrayList<Int32> vals;

		public:
			Int32InCondition(Text::CStringNN fieldName, NotNullPtr<Data::ArrayList<Int32>> val);
			virtual ~Int32InCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NotNullPtr<Data::VariItem> item);
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
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NotNullPtr<Data::VariItem> item);

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
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NotNullPtr<Data::VariItem> item);
		};

		class StringInCondition : public FieldCondition
		{
		private:
			Data::ArrayList<const UTF8Char*> vals;

		public:
			StringInCondition(Text::CStringNN fieldName, NotNullPtr<Data::ArrayList<const UTF8Char*>> val);
			virtual ~StringInCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NotNullPtr<Data::VariItem> item);
		};

		class StringNotInCondition : public FieldCondition
		{
		private:
			Data::ArrayList<const UTF8Char*> vals;

		public:
			StringNotInCondition(Text::CStringNN fieldName, NotNullPtr<Data::ArrayList<const UTF8Char*>> val);
			virtual ~StringNotInCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NotNullPtr<Data::VariItem> item);
		};

		class StringContainsCondition : public FieldCondition
		{
		private:
			NotNullPtr<Text::String> val;

		public:
			StringContainsCondition(Text::CStringNN fieldName, const UTF8Char *val);
			virtual ~StringContainsCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NotNullPtr<Data::VariItem> item);
		};

		class StringEqualsCondition : public FieldCondition
		{
		private:
			NotNullPtr<Text::String> val;

		public:
			StringEqualsCondition(Text::CStringNN fieldName, Text::CString val);
			virtual ~StringEqualsCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NotNullPtr<Data::VariItem> item);
		};

		class BooleanCondition : public FieldCondition
		{
		private:
			Bool val;

		public:
			BooleanCondition(Text::CStringNN fieldName, Bool val);
			virtual ~BooleanCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NotNullPtr<Data::VariItem> item);
		};

		class NotNullCondition : public FieldCondition
		{
		public:
			NotNullCondition(Text::CStringNN fieldName);
			virtual ~NotNullCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool TestValid(NotNullPtr<Data::VariItem> item);
		};

		class InnerCondition : public Condition
		{
		private:
			NotNullPtr<QueryConditions> innerCond;

		public:
			InnerCondition(NotNullPtr<QueryConditions> innerCond);
			virtual ~InnerCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool IsValid(NotNullPtr<Data::VariObject> obj);
			virtual Bool IsValid(NotNullPtr<Data::ObjectGetter> getter);
			virtual void GetFieldList(NotNullPtr<Data::ArrayListNN<Text::String>> fieldList);

			NotNullPtr<QueryConditions> GetConditions();
		};

		class OrCondition : public Condition
		{
		public:
			OrCondition();
			virtual ~OrCondition();

			virtual ConditionType GetType();
			virtual Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem);
			virtual Bool IsValid(NotNullPtr<Data::VariObject> obj);
			virtual Bool IsValid(NotNullPtr<Data::ObjectGetter> getter);
			virtual void GetFieldList(NotNullPtr<Data::ArrayListNN<Text::String>> fieldList);
		};

	private:
		Data::ArrayListNN<Condition> conditionList;
	public:
		QueryConditions();
		~QueryConditions();

		Bool IsValid(NotNullPtr<Data::VariObject> obj);
		Bool IsValid(NotNullPtr<Data::ObjectGetter> getter);
		Bool ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem, Data::ArrayList<Condition*> *clientConditions);
		UOSInt GetCount();
		Condition *GetItem(UOSInt index);
		NotNullPtr<Data::ArrayListNN<Condition>> GetList();
		void GetFieldList(NotNullPtr<Data::ArrayListNN<Text::String>> fieldList);

		NotNullPtr<QueryConditions> TimeBetween(Text::CStringNN fieldName, const Data::Timestamp &t1, const Data::Timestamp &t2);
		NotNullPtr<QueryConditions> Or();
		NotNullPtr<QueryConditions> InnerCond(NotNullPtr<QueryConditions> innerCond);
		NotNullPtr<QueryConditions> Int32Equals(Text::CStringNN fieldName, Int32 val);
		NotNullPtr<QueryConditions> Int64Equals(Text::CStringNN fieldName, Int64 val);
		NotNullPtr<QueryConditions> Int32In(Text::CStringNN fieldName, NotNullPtr<Data::ArrayList<Int32>> val);
		NotNullPtr<QueryConditions> DoubleGE(Text::CStringNN fieldName, Double val);
		NotNullPtr<QueryConditions> DoubleLE(Text::CStringNN fieldName, Double val);
		NotNullPtr<QueryConditions> StrIn(Text::CStringNN fieldName, NotNullPtr<Data::ArrayList<const UTF8Char*>> vals);
		NotNullPtr<QueryConditions> StrNotIn(Text::CStringNN fieldName, NotNullPtr<Data::ArrayList<const UTF8Char*>> vals);
		NotNullPtr<QueryConditions> StrContains(Text::CStringNN fieldName, const UTF8Char *val);
		NotNullPtr<QueryConditions> StrEquals(Text::CStringNN fieldName, Text::CString val);
		NotNullPtr<QueryConditions> BoolEquals(Text::CStringNN fieldName, Bool val);
		NotNullPtr<QueryConditions> NotNull(Text::CStringNN fieldName);

		static Text::CStringNN CompareConditionGetStr(CompareCondition cond);
		static Bool ObjectValid(NotNullPtr<Data::VariObject> obj, NotNullPtr<Data::ArrayListNN<Condition>> conditionList);
		static Bool ObjectValid(NotNullPtr<Data::ObjectGetter> getter, NotNullPtr<Data::ArrayListNN<Condition>> conditionList);
		static QueryConditions *ParseStr(Text::CStringNN s, DB::SQLType sqlType);
	};
}
#endif
