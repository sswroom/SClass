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
		enum class ObjectType
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
			INT64,
			BooleanAnd,
			BooleanOr,
			NumberField
		};

		enum class DataType
		{
			Unknown,
			Boolean,
			Number,
			String,
			Time
		};

		enum class NumberType
		{
			F64,
			I64,
			I32,
			Null
		};

		class ConditionObject
		{
		public:
			virtual ~ConditionObject() {};

			virtual ObjectType GetType() const = 0;
			virtual DataType GetReturnType() const = 0;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const = 0;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const = 0;
			virtual void GetFieldList(NN<Data::ArrayListStringNN> fieldList) const = 0;
		};

		class BooleanObject : public ConditionObject
		{
		public:
			virtual ~BooleanObject() {};

			virtual DataType GetReturnType() const { return DataType::Boolean; };
			virtual Bool Eval(NN<Data::VariObject> obj, OutParam<Bool> outVal) const = 0;
			virtual Bool Eval(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal) const = 0;
		};

		class NumberObject : public ConditionObject
		{
		public:
			virtual ~NumberObject() {};

			virtual DataType GetReturnType() const { return DataType::Number; };
			virtual NumberType GetNumberType(NN<Data::VariObject> obj) const = 0;
			virtual NumberType GetNumberType(NN<Data::ObjectGetter> getter) const = 0;
			virtual Bool EvalInt(NN<Data::VariObject> obj, OutParam<Int64> outVal) const = 0;
			virtual Bool EvalInt(NN<Data::ObjectGetter> getter, OutParam<Int64> outVal) const = 0;
			virtual Bool EvalDouble(NN<Data::VariObject> obj, OutParam<Double> outVal) const = 0;
			virtual Bool EvalDouble(NN<Data::ObjectGetter> getter, OutParam<Double> outVal) const = 0;
		};

		class StringObject : public ConditionObject
		{
		public:
			virtual ~StringObject() {};

			virtual DataType GetReturnType() const { return DataType::String; };
			virtual Optional<Text::String> Eval(NN<Data::VariObject> obj) const = 0;
			virtual Optional<Text::String> Eval(NN<Data::ObjectGetter> getter) const = 0;
		};

		class TimeObject : public ConditionObject
		{
		public:
			virtual ~TimeObject() {};

			virtual DataType GetReturnType() const { return DataType::Time; };
			virtual Data::Timestamp Eval(NN<Data::VariObject> obj) const = 0;
			virtual Data::Timestamp Eval(NN<Data::ObjectGetter> getter) const = 0;
		};

		class BooleanAnd : public BooleanObject
		{
		private:
			Data::ArrayListNN<BooleanObject> andList;

		public:
			BooleanAnd();
			virtual ~BooleanAnd();

			virtual ObjectType GetType() const { return ObjectType::BooleanAnd; }
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			Bool ToWhereClauseOrClient(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem, NN<Data::ArrayListNN<BooleanObject>> clientConditions) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual void GetFieldList(NN<Data::ArrayListStringNN> fieldList) const;
			virtual Bool Eval(NN<Data::VariObject> obj, OutParam<Bool> outVal) const;
			virtual Bool Eval(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal) const;

			UOSInt GetCount() const;
			void AddAnd(NN<BooleanObject> obj);
		};

		class BooleanOr : public BooleanObject
		{
		private:
			Data::ArrayListNN<BooleanObject> orList;

		public:
			BooleanOr();
			virtual ~BooleanOr();

			virtual ObjectType GetType() const { return ObjectType::BooleanOr; }
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual void GetFieldList(NN<Data::ArrayListStringNN> fieldList) const;
			virtual Bool Eval(NN<Data::VariObject> obj, OutParam<Bool> outVal) const;
			virtual Bool Eval(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal) const;

			UOSInt GetCount() const;
			void AddOr(NN<BooleanObject> obj);
		};

		class NumberField : public NumberObject
		{
		private:
			NN<Text::String> fieldName;
		public:
			NumberField(Text::CStringNN fieldName);
			NumberField(NN<Text::String> fieldName);
			virtual ~NumberField();

			virtual ObjectType GetType() const { return ObjectType::NumberField; }
			virtual NumberType GetNumberType(NN<Data::VariObject> obj) const;
			virtual NumberType GetNumberType(NN<Data::ObjectGetter> getter) const;
			virtual Bool EvalInt(NN<Data::VariObject> obj, OutParam<Int64> outVal) const;
			virtual Bool EvalInt(NN<Data::ObjectGetter> getter, OutParam<Int64> outVal) const;
			virtual Bool EvalDouble(NN<Data::VariObject> obj, OutParam<Double> outVal) const;
			virtual Bool EvalDouble(NN<Data::ObjectGetter> getter, OutParam<Double> outVal) const;

			static NumberType ToNumberType(NN<Data::VariItem> item);
		};

		class FieldCondition : public BooleanObject
		{
		protected:
			NN<Text::String> fieldName;
		public:
			FieldCondition(Text::CStringNN fieldName);
			FieldCondition(NN<Text::String> fieldName);
			virtual ~FieldCondition();

			virtual Bool Eval(NN<Data::VariObject> obj, OutParam<Bool> outVal) const;
			virtual Bool Eval(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal) const;
			virtual void GetFieldList(NN<Data::ArrayListStringNN> fieldList) const;

			virtual Bool TestValid(NN<Data::VariItem> item) const = 0;
		};

		class TimeBetweenCondition : public FieldCondition
		{
		private:
			Data::Timestamp t1;
			Data::Timestamp t2;

		public:
			TimeBetweenCondition(Text::CStringNN fieldName, const Data::Timestamp &t1, const Data::Timestamp &t2);
			virtual ~TimeBetweenCondition();

			virtual ObjectType GetType() const;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual Bool TestValid(NN<Data::VariItem> item) const;
		};

		class Int32Condition : public FieldCondition
		{
		private:
			Int32 val;
			CompareCondition cond;

		public:
			Int32Condition(Text::CStringNN fieldName, Int32 val, CompareCondition cond);
			virtual ~Int32Condition();

			virtual ObjectType GetType() const;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual Bool TestValid(NN<Data::VariItem> item) const;

			NN<Text::String> GetFieldName();
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

			virtual ObjectType GetType() const;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual Bool TestValid(NN<Data::VariItem> item) const;
		};

		class Int64Condition : public FieldCondition
		{
		private:
			Int64 val;
			CompareCondition cond;

		public:
			Int64Condition(Text::CStringNN fieldName, Int64 val, CompareCondition cond);
			virtual ~Int64Condition();

			virtual ObjectType GetType() const;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual Bool TestValid(NN<Data::VariItem> item) const;

			NN<Text::String> GetFieldName();
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

			virtual ObjectType GetType() const;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual Bool TestValid(NN<Data::VariItem> item) const;
		};

		class StringInCondition : public FieldCondition
		{
		private:
			Data::ArrayListArr<const UTF8Char> vals;

		public:
			StringInCondition(Text::CStringNN fieldName, NN<Data::ArrayListArr<const UTF8Char>> val);
			virtual ~StringInCondition();

			virtual ObjectType GetType() const;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual Bool TestValid(NN<Data::VariItem> item) const;
		};

		class StringNotInCondition : public FieldCondition
		{
		private:
			Data::ArrayListArr<const UTF8Char> vals;

		public:
			StringNotInCondition(Text::CStringNN fieldName, NN<Data::ArrayListArr<const UTF8Char>> val);
			virtual ~StringNotInCondition();

			virtual ObjectType GetType() const;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual Bool TestValid(NN<Data::VariItem> item) const;
		};

		class StringContainsCondition : public FieldCondition
		{
		private:
			NN<Text::String> val;

		public:
			StringContainsCondition(Text::CStringNN fieldName, UnsafeArray<const UTF8Char> val);
			virtual ~StringContainsCondition();

			virtual ObjectType GetType() const;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual Bool TestValid(NN<Data::VariItem> item) const;
		};

		class StringEqualsCondition : public FieldCondition
		{
		private:
			NN<Text::String> val;

		public:
			StringEqualsCondition(Text::CStringNN fieldName, Text::CStringNN val);
			virtual ~StringEqualsCondition();

			virtual ObjectType GetType() const;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual Bool TestValid(NN<Data::VariItem> item) const;
		};

		class BooleanCondition : public FieldCondition
		{
		private:
			Bool val;

		public:
			BooleanCondition(Text::CStringNN fieldName, Bool val);
			virtual ~BooleanCondition();

			virtual ObjectType GetType() const;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual Bool TestValid(NN<Data::VariItem> item) const;
		};

		class NotNullCondition : public FieldCondition
		{
		public:
			NotNullCondition(Text::CStringNN fieldName);
			virtual ~NotNullCondition();

			virtual ObjectType GetType() const;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual Bool TestValid(NN<Data::VariItem> item) const;
		};

		class InnerCondition : public BooleanObject
		{
		private:
			NN<QueryConditions> innerCond;

		public:
			InnerCondition(NN<QueryConditions> innerCond);
			virtual ~InnerCondition();

			virtual ObjectType GetType() const;
			virtual Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const;
			virtual Bool CanWhereClause(UOSInt maxDBItem) const;
			virtual Bool Eval(NN<Data::VariObject> obj, OutParam<Bool> outVal) const;
			virtual Bool Eval(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal) const;
			virtual void GetFieldList(NN<Data::ArrayListStringNN> fieldList) const;

			NN<QueryConditions> GetConditions();
		};

	private:
		NN<BooleanObject> cond;
		NN<BooleanAnd> andCond;
		Optional<BooleanOr> orCond;
	public:
		QueryConditions();
		~QueryConditions();

		Bool IsValid(NN<Data::VariObject> obj, OutParam<Bool> outVal);
		Bool IsValid(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal);
		Bool ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem, NN<Data::ArrayListNN<BooleanObject>> clientConditions);
		Bool CanWhereClause(UOSInt maxDBItem) const;
		void GetFieldList(NN<Data::ArrayListStringNN> fieldList) const;

		NN<QueryConditions> TimeBetween(Text::CStringNN fieldName, const Data::Timestamp &t1, const Data::Timestamp &t2);
		NN<QueryConditions> Or();
		NN<QueryConditions> InnerCond(NN<QueryConditions> innerCond);
		NN<QueryConditions> Int32Equals(Text::CStringNN fieldName, Int32 val);
		NN<QueryConditions> Int32GE(Text::CStringNN fieldName, Int32 val);
		NN<QueryConditions> Int32LE(Text::CStringNN fieldName, Int32 val);
		NN<QueryConditions> Int32GT(Text::CStringNN fieldName, Int32 val);
		NN<QueryConditions> Int32LT(Text::CStringNN fieldName, Int32 val);
		NN<QueryConditions> Int32In(Text::CStringNN fieldName, NN<Data::ArrayList<Int32>> val);
		NN<QueryConditions> Int64Equals(Text::CStringNN fieldName, Int64 val);
		NN<QueryConditions> Int64GE(Text::CStringNN fieldName, Int64 val);
		NN<QueryConditions> Int64LE(Text::CStringNN fieldName, Int64 val);
		NN<QueryConditions> Int64GT(Text::CStringNN fieldName, Int64 val);
		NN<QueryConditions> Int64LT(Text::CStringNN fieldName, Int64 val);
		NN<QueryConditions> DoubleEquals(Text::CStringNN fieldName, Double val);
		NN<QueryConditions> DoubleGE(Text::CStringNN fieldName, Double val);
		NN<QueryConditions> DoubleLE(Text::CStringNN fieldName, Double val);
		NN<QueryConditions> DoubleGT(Text::CStringNN fieldName, Double val);
		NN<QueryConditions> DoubleLT(Text::CStringNN fieldName, Double val);
		NN<QueryConditions> StrIn(Text::CStringNN fieldName, NN<Data::ArrayListArr<const UTF8Char>> vals);
		NN<QueryConditions> StrNotIn(Text::CStringNN fieldName, NN<Data::ArrayListArr<const UTF8Char>> vals);
		NN<QueryConditions> StrContains(Text::CStringNN fieldName, UnsafeArray<const UTF8Char> val);
		NN<QueryConditions> StrEquals(Text::CStringNN fieldName, Text::CStringNN val);
		NN<QueryConditions> BoolEquals(Text::CStringNN fieldName, Bool val);
		NN<QueryConditions> NotNull(Text::CStringNN fieldName);

		static Text::CStringNN CompareConditionGetStr(CompareCondition cond);
		static Bool ObjectValid(NN<Data::VariObject> obj, NN<Data::ArrayListNN<BooleanObject>> conditionList, OutParam<Bool> outVal);
		static Bool ObjectValid(NN<Data::ObjectGetter> getter, NN<Data::ArrayListNN<BooleanObject>> conditionList, OutParam<Bool> outVal);
		static Optional<QueryConditions> ParseStr(Text::CStringNN s, DB::SQLType sqlType);
	};
}
#endif
