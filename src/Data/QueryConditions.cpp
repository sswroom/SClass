#include "Stdafx.h"
#include "Data/QueryConditions.h"
#include "DB/SQL/SQLUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

Data::QueryConditions::FieldCondition::FieldCondition(Text::CStringNN fieldName)
{
	this->fieldName = Text::String::New(fieldName);
}

Data::QueryConditions::FieldCondition::FieldCondition(NN<Text::String> fieldName)
{
	this->fieldName = fieldName->Clone();
}

Data::QueryConditions::FieldCondition::~FieldCondition()
{
	this->fieldName->Release();
}

Bool Data::QueryConditions::FieldCondition::IsValid(NN<Data::VariObject> obj)
{
	NN<Data::VariItem> item;
	if (!item.Set(obj->GetItem(this->fieldName->v)))
		return false;
	return this->TestValid(item);
}

Bool Data::QueryConditions::FieldCondition::IsValid(NN<Data::ObjectGetter> getter)
{
	NN<Data::VariItem> item = getter->GetNewItem(this->fieldName->ToCString());
	Bool ret = this->TestValid(item);
	item.Delete();
	return ret;
}

void Data::QueryConditions::FieldCondition::GetFieldList(NN<Data::ArrayListStringNN> fieldList)
{
	fieldList->Add(this->fieldName);
}

Data::QueryConditions::TimeBetweenCondition::TimeBetweenCondition(Text::CStringNN fieldName, const Data::Timestamp &t1, const Data::Timestamp &t2) : FieldCondition(fieldName)
{
	this->t1 = t1;
	this->t2 = t2;
}

Data::QueryConditions::TimeBetweenCondition::~TimeBetweenCondition()
{
}

Data::QueryConditions::ConditionType Data::QueryConditions::TimeBetweenCondition::GetType()
{
	return ConditionType::TimeBetween;
}

Bool Data::QueryConditions::TimeBetweenCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendC(UTF8STRC(" between "));
	sptr = DB::DBUtil::SDBTS(sbuff, this->t1, sqlType, tzQhr);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendC(UTF8STRC(" and "));
	sptr = DB::DBUtil::SDBTS(sbuff, this->t2, sqlType, tzQhr);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

Bool Data::QueryConditions::TimeBetweenCondition::TestValid(NN<Data::VariItem> item)
{
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Timestamp:
		{
			Data::Timestamp t = item->GetItemValue().ts;
			return t >= this->t1 && t <= this->t2;
		}
	case Data::VariItem::ItemType::Date:
		{
			Data::Timestamp t = Data::Timestamp(item->GetItemValue().date.ToTicks(), Data::DateTimeUtil::GetLocalTzQhr());
			return t >= this->t1 && t <= this->t2;
		}
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
	case Data::VariItem::ItemType::I8:
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::I16:
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::U64:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
}

Data::QueryConditions::Int32Condition::Int32Condition(Text::CStringNN fieldName, Int32 val, CompareCondition cond) : FieldCondition(fieldName)
{
	this->val = val;
	this->cond = cond;
}

Data::QueryConditions::Int32Condition::~Int32Condition()
{
}

Data::QueryConditions::ConditionType Data::QueryConditions::Int32Condition::GetType()
{
	return ConditionType::INT32;
}

Bool Data::QueryConditions::Int32Condition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->Append(Data::QueryConditions::CompareConditionGetStr(cond));
	sb->AppendI32(this->val);
	return true;
}

Bool Data::QueryConditions::Int32Condition::TestValid(NN<Data::VariItem> item)
{
	Int64 iVal;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::F32:
		iVal = (Int64)item->GetItemValue().f32;
		break;
	case Data::VariItem::ItemType::F64:
		iVal = (Int64)item->GetItemValue().f64;
		break;
	case Data::VariItem::ItemType::I8:
		iVal = item->GetItemValue().i8;
		break;
	case Data::VariItem::ItemType::U8:
		iVal = item->GetItemValue().u8;
		break;
	case Data::VariItem::ItemType::I16:
		iVal = item->GetItemValue().i16;
		break;
	case Data::VariItem::ItemType::U16:
		iVal = item->GetItemValue().u16;
		break;
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::I32:
		iVal = item->GetItemValue().i32;
		break;
	case Data::VariItem::ItemType::U32:
		iVal = item->GetItemValue().u32;
		break;
	case Data::VariItem::ItemType::I64:
		iVal = item->GetItemValue().i64;
		break;
	case Data::VariItem::ItemType::U64:
		iVal = (Int64)item->GetItemValue().u64;
		break;
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
	switch (this->cond)
	{
	case CompareCondition::Equal:
		return iVal == this->val;
	case CompareCondition::NotEqual:
		return iVal != this->val;
	case CompareCondition::Greater:
		return iVal > this->val;
	case CompareCondition::GreaterOrEqual:
		return iVal >= this->val;
	case CompareCondition::Less:
		return iVal < this->val;
	case CompareCondition::LessOrEqual:
		return iVal <= this->val;
	case CompareCondition::Unknown:
		return false;
	}
	return false;
}

Text::String *Data::QueryConditions::Int32Condition::GetFieldName()
{
	return this->fieldName.Ptr();
}

Int32 Data::QueryConditions::Int32Condition::GetVal()
{
	return this->val;
}

Data::CompareCondition Data::QueryConditions::Int32Condition::GetCompareCond()
{
	return this->cond;
}

Data::QueryConditions::Int32InCondition::Int32InCondition(Text::CStringNN fieldName, NN<Data::ArrayList<Int32>> val) : FieldCondition(fieldName)
{
	this->vals.AddAll(val);
}

Data::QueryConditions::Int32InCondition::~Int32InCondition()
{
}

Data::QueryConditions::ConditionType Data::QueryConditions::Int32InCondition::GetType()
{
	return ConditionType::Int32In;
}

Bool Data::QueryConditions::Int32InCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	if (this->vals.GetCount() > maxDBItem)
	{
		return false;
	}
	else
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendC(UTF8STRC(" in ("));
		Text::StringTool::Int32Join(sb, &this->vals, CSTR(", "));
		sb->AppendC(UTF8STRC(")"));
		return true;
	}
}

Bool Data::QueryConditions::Int32InCondition::TestValid(NN<Data::VariItem> item)
{
	Int64 iVal;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::F32:
		iVal = (Int64)item->GetItemValue().f32;
		break;
	case Data::VariItem::ItemType::F64:
		iVal = (Int64)item->GetItemValue().f64;
		break;
	case Data::VariItem::ItemType::I8:
		iVal = item->GetItemValue().i8;
		break;
	case Data::VariItem::ItemType::U8:
		iVal = item->GetItemValue().u8;
		break;
	case Data::VariItem::ItemType::I16:
		iVal = item->GetItemValue().i16;
		break;
	case Data::VariItem::ItemType::U16:
		iVal = item->GetItemValue().u16;
		break;
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::I32:
		iVal = item->GetItemValue().i32;
		break;
	case Data::VariItem::ItemType::U32:
		iVal = item->GetItemValue().u32;
		break;
	case Data::VariItem::ItemType::I64:
		iVal = item->GetItemValue().i64;
		break;
	case Data::VariItem::ItemType::U64:
		iVal = (Int64)item->GetItemValue().u64;
		break;
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
	UOSInt i = this->vals.GetCount();
	while (i-- > 0)
	{
		if (iVal == this->vals.GetItem(i))
		{
			return true;
		}
	}
	return false;
}

Data::QueryConditions::Int64Condition::Int64Condition(Text::CStringNN fieldName, Int64 val, CompareCondition cond) : FieldCondition(fieldName)
{
	this->val = val;
	this->cond = cond;
}

Data::QueryConditions::Int64Condition::~Int64Condition()
{
}

Data::QueryConditions::ConditionType Data::QueryConditions::Int64Condition::GetType()
{
	return ConditionType::INT64;
}

Bool Data::QueryConditions::Int64Condition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->Append(Data::QueryConditions::CompareConditionGetStr(cond));
	sb->AppendI64(this->val);
	return true;
}

Bool Data::QueryConditions::Int64Condition::TestValid(NN<Data::VariItem> item)
{
	Int64 iVal;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::F32:
		iVal = (Int64)item->GetItemValue().f32;
		break;
	case Data::VariItem::ItemType::F64:
		iVal = (Int64)item->GetItemValue().f64;
		break;
	case Data::VariItem::ItemType::I8:
		iVal = item->GetItemValue().i8;
		break;
	case Data::VariItem::ItemType::U8:
		iVal = item->GetItemValue().u8;
		break;
	case Data::VariItem::ItemType::I16:
		iVal = item->GetItemValue().i16;
		break;
	case Data::VariItem::ItemType::U16:
		iVal = item->GetItemValue().u16;
		break;
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::I32:
		iVal = item->GetItemValue().i32;
		break;
	case Data::VariItem::ItemType::U32:
		iVal = item->GetItemValue().u32;
		break;
	case Data::VariItem::ItemType::I64:
		iVal = item->GetItemValue().i64;
		break;
	case Data::VariItem::ItemType::U64:
		iVal = (Int64)item->GetItemValue().u64;
		break;
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
	switch (this->cond)
	{
	case CompareCondition::Equal:
		return iVal == this->val;
	case CompareCondition::NotEqual:
		return iVal != this->val;
	case CompareCondition::Greater:
		return iVal > this->val;
	case CompareCondition::GreaterOrEqual:
		return iVal >= this->val;
	case CompareCondition::Less:
		return iVal < this->val;
	case CompareCondition::LessOrEqual:
		return iVal <= this->val;
	case CompareCondition::Unknown:
		return false;
	}
	return false;
}

Text::String *Data::QueryConditions::Int64Condition::GetFieldName()
{
	return this->fieldName.Ptr();
}

Int64 Data::QueryConditions::Int64Condition::GetVal()
{
	return this->val;
}

Data::CompareCondition Data::QueryConditions::Int64Condition::GetCompareCond()
{
	return this->cond;
}

Data::QueryConditions::DoubleCondition::DoubleCondition(Text::CStringNN fieldName, Double val, CompareCondition cond) : FieldCondition(fieldName)
{
	this->val = val;
	this->cond = cond;
}

Data::QueryConditions::DoubleCondition::~DoubleCondition()
{
}

Data::QueryConditions::ConditionType Data::QueryConditions::DoubleCondition::GetType()
{
	return ConditionType::DOUBLE;
}

Bool Data::QueryConditions::DoubleCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->Append(Data::QueryConditions::CompareConditionGetStr(cond));
	Text::SBAppendF64(sb, this->val);
	return true;
}

Bool Data::QueryConditions::DoubleCondition::TestValid(NN<Data::VariItem> item)
{
	Double dVal;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::F32:
		dVal = item->GetItemValue().f32;
		break;
	case Data::VariItem::ItemType::F64:
		dVal = item->GetItemValue().f64;
		break;
	case Data::VariItem::ItemType::I8:
		dVal = item->GetItemValue().i8;
		break;
	case Data::VariItem::ItemType::U8:
		dVal = item->GetItemValue().u8;
		break;
	case Data::VariItem::ItemType::I16:
		dVal = item->GetItemValue().i16;
		break;
	case Data::VariItem::ItemType::U16:
		dVal = item->GetItemValue().u16;
		break;
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::I32:
		dVal = item->GetItemValue().i32;
		break;
	case Data::VariItem::ItemType::U32:
		dVal = item->GetItemValue().u32;
		break;
	case Data::VariItem::ItemType::I64:
		dVal = (Double)item->GetItemValue().i64;
		break;
	case Data::VariItem::ItemType::U64:
		dVal = (Double)item->GetItemValue().u64;
		break;
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
	switch (this->cond)
	{
	case CompareCondition::Equal:
		return dVal == this->val;
	case CompareCondition::NotEqual:
		return dVal != this->val;
	case CompareCondition::Greater:
		return dVal > this->val;
	case CompareCondition::GreaterOrEqual:
		return dVal >= this->val;
	case CompareCondition::Less:
		return dVal < this->val;
	case CompareCondition::LessOrEqual:
		return dVal <= this->val;
	case CompareCondition::Unknown:
		return false;
	}
	return false;
}

Data::QueryConditions::StringInCondition::StringInCondition(Text::CStringNN fieldName, NN<Data::ArrayList<const UTF8Char*>> val) : FieldCondition(fieldName)
{
	UOSInt i = 0;
	UOSInt j = val->GetCount();
	while (i < j)
	{
		this->vals.Add(Text::StrCopyNew(val->GetItem(i)).Ptr());
		i++;
	}
}

Data::QueryConditions::StringInCondition::~StringInCondition()
{
	LIST_FREE_FUNC(&this->vals, Text::StrDelNew);
}

Data::QueryConditions::ConditionType Data::QueryConditions::StringInCondition::GetType()
{
	return ConditionType::StringIn;
}

Bool Data::QueryConditions::StringInCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	if (this->vals.GetCount() > maxDBItem || this->vals.GetCount() == 0)
	{
		return false;
	}
	else
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		UTF8Char *sptrTmp = 0;
		UOSInt sptrSize = 0;
		UOSInt thisSize;
		UOSInt i;
		UOSInt j;
		sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendC(UTF8STRC(" in ("));
		i = 0;
		j = this->vals.GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sb->AppendC(UTF8STRC(", "));
			}
			thisSize = DB::DBUtil::SDBStrUTF8Leng(this->vals.GetItem(i), sqlType);
			if (thisSize < 512)
			{
				sptr = DB::DBUtil::SDBStrUTF8(sbuff, this->vals.GetItem(i), sqlType);
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			}
			else
			{
				if (thisSize > sptrSize)
				{
					sptrSize = thisSize;
					if (sptrTmp)
					{
						MemFree(sptrTmp);
					}
					sptrTmp = MemAlloc(UTF8Char, thisSize + 1);
				}
				sptr = DB::DBUtil::SDBStrUTF8(sptrTmp, this->vals.GetItem(i), sqlType);
				sb->AppendC(sptrTmp, (UOSInt)(sptr - sptrTmp));
			}
			i++;
		}
		if (sptrTmp)
		{
			MemFree(sptrTmp);
		}
		sb->AppendUTF8Char(')');
		return true;
	}
}

Bool Data::QueryConditions::StringInCondition::TestValid(NN<Data::VariItem> item)
{
	const UTF8Char *csptr;
	UOSInt i;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Str:
		csptr = item->GetItemValue().str->v;
		i = this->vals.GetCount();
		while (i-- > 0)
		{
			if (Text::StrEquals(csptr, this->vals.GetItem(i)))
			{
				return true;
			}
		}
		return false;
	case Data::VariItem::ItemType::CStr:
		csptr = item->GetItemValue().cstr.v;
		i = this->vals.GetCount();
		while (i-- > 0)
		{
			if (Text::StrEquals(csptr, this->vals.GetItem(i)))
			{
				return true;
			}
		}
		return false;
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
	case Data::VariItem::ItemType::I8:
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::I16:
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::U64:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
}

Data::QueryConditions::StringNotInCondition::StringNotInCondition(Text::CStringNN fieldName, NN<Data::ArrayList<const UTF8Char*>> val) : FieldCondition(fieldName)
{
	UOSInt i = 0;
	UOSInt j = val->GetCount();
	while (i < j)
	{
		this->vals.Add(Text::StrCopyNew(val->GetItem(i)).Ptr());
		i++;
	}
}

Data::QueryConditions::StringNotInCondition::~StringNotInCondition()
{
	LIST_FREE_FUNC(&this->vals, Text::StrDelNew);
}

Data::QueryConditions::ConditionType Data::QueryConditions::StringNotInCondition::GetType()
{
	return ConditionType::StringNotIn;
}

Bool Data::QueryConditions::StringNotInCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	if (this->vals.GetCount() > maxDBItem || this->vals.GetCount() == 0)
	{
		return false;
	}
	else
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		UTF8Char *sptrTmp = 0;
		UOSInt sptrSize = 0;
		UOSInt thisSize;
		UOSInt i;
		UOSInt j;
		sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendC(UTF8STRC(" not in ("));
		i = 0;
		j = this->vals.GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sb->AppendC(UTF8STRC(", "));
			}
			thisSize = DB::DBUtil::SDBStrUTF8Leng(this->vals.GetItem(i), sqlType);
			if (thisSize < 512)
			{
				sptr = DB::DBUtil::SDBStrUTF8(sbuff, this->vals.GetItem(i), sqlType);
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			}
			else
			{
				if (thisSize > sptrSize)
				{
					sptrSize = thisSize;
					if (sptrTmp)
					{
						MemFree(sptrTmp);
					}
					sptrTmp = MemAlloc(UTF8Char, thisSize + 1);
				}
				sptr = DB::DBUtil::SDBStrUTF8(sptrTmp, this->vals.GetItem(i), sqlType);
				sb->AppendC(sptrTmp, (UOSInt)(sptr - sptrTmp));
			}
			i++;
		}
		if (sptrTmp)
		{
			MemFree(sptrTmp);
		}
		sb->AppendUTF8Char(')');
		return true;
	}
}

Bool Data::QueryConditions::StringNotInCondition::TestValid(NN<Data::VariItem> item)
{
	const UTF8Char *csptr;
	UOSInt i;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Str:
		csptr = item->GetItemValue().str->v;
		i = this->vals.GetCount();
		while (i-- > 0)
		{
			if (Text::StrEquals(csptr, this->vals.GetItem(i)))
			{
				return false;
			}
		}
		return true;
	case Data::VariItem::ItemType::CStr:
		csptr = item->GetItemValue().cstr.v;
		i = this->vals.GetCount();
		while (i-- > 0)
		{
			if (Text::StrEquals(csptr, this->vals.GetItem(i)))
			{
				return false;
			}
		}
		return true;
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
	case Data::VariItem::ItemType::I8:
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::I16:
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::U64:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
}

Data::QueryConditions::StringContainsCondition::StringContainsCondition(Text::CStringNN fieldName, const UTF8Char *val) : FieldCondition(fieldName)
{
	this->val = Text::String::NewNotNullSlow(val);
}

Data::QueryConditions::StringContainsCondition::~StringContainsCondition()
{
	this->val->Release();
}

Data::QueryConditions::ConditionType Data::QueryConditions::StringContainsCondition::GetType()
{
	return ConditionType::StringContains;
}

Bool Data::QueryConditions::StringContainsCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr2;
	sptr2 = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr2 - sbuff));
	sb->AppendC(UTF8STRC(" like "));
	Text::StringBuilderUTF8 sb2;
	sb2.AppendUTF8Char('%');
	sb2.Append(this->val);
	sb2.AppendUTF8Char('%');
	UOSInt size = DB::DBUtil::SDBStrUTF8Leng(sb2.ToString(), sqlType);
	if (size < 512)
	{
		sptr2 = DB::DBUtil::SDBStrUTF8(sbuff, sb2.ToString(), sqlType);
		sb->AppendC(sbuff, (UOSInt)(sptr2 - sbuff));
	}
	else
	{
		UTF8Char *sptr = MemAlloc(UTF8Char, size + 1);
		sptr2 = DB::DBUtil::SDBStrUTF8(sptr, sb2.ToString(), sqlType);
		sb->AppendC(sptr, (UOSInt)(sptr2 - sptr));
		MemFree(sptr);
	}
	return true;
}

Bool Data::QueryConditions::StringContainsCondition::TestValid(NN<Data::VariItem> item)
{
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Str:
		return item->GetItemValue().str->IndexOf(this->val->v, this->val->leng) != INVALID_INDEX;
	case Data::VariItem::ItemType::CStr:
	{
		Data::VariItem::ItemValue ival = item->GetItemValue();
		return Text::StrIndexOfC(ival.cstr.v, ival.cstr.leng, this->val->v, this->val->leng) != INVALID_INDEX;
	}
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
	case Data::VariItem::ItemType::I8:
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::I16:
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::U64:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
}

Data::QueryConditions::StringEqualsCondition::StringEqualsCondition(Text::CStringNN fieldName, Text::CString val) : FieldCondition(fieldName)
{
	this->val = Text::String::New(val);
}

Data::QueryConditions::StringEqualsCondition::~StringEqualsCondition()
{
	this->val->Release();
}

Data::QueryConditions::ConditionType Data::QueryConditions::StringEqualsCondition::GetType()
{
	return ConditionType::StringEquals;
}

Bool Data::QueryConditions::StringEqualsCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr2;
	sptr2 = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr2 - sbuff));
	sb->AppendC(UTF8STRC(" = "));
	UOSInt size = DB::DBUtil::SDBStrUTF8Leng(this->val->v, sqlType);
	if (size < 512)
	{
		sptr2 = DB::DBUtil::SDBStrUTF8(sbuff, this->val->v, sqlType);
		sb->AppendC(sbuff, (UOSInt)(sptr2 - sbuff));
	}
	else
	{
		UTF8Char *sptr = MemAlloc(UTF8Char, size + 1);
		sptr2 = DB::DBUtil::SDBStrUTF8(sptr, this->val->v, sqlType);
		sb->AppendC(sptr, (UOSInt)(sptr2 - sptr));
		MemFree(sptr);
	}
	return true;
}

Bool Data::QueryConditions::StringEqualsCondition::TestValid(NN<Data::VariItem> item)
{
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Str:
		return item->GetItemValue().str->Equals(this->val);
	case Data::VariItem::ItemType::CStr:
	{
		Data::VariItem::ItemValue ival = item->GetItemValue();
		return this->val->Equals(ival.cstr.v, ival.cstr.leng);		
	}
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
	case Data::VariItem::ItemType::I8:
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::I16:
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::U64:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
}

Data::QueryConditions::BooleanCondition::BooleanCondition(Text::CStringNN fieldName, Bool val) : FieldCondition(fieldName)
{
	this->val = val;
}

Data::QueryConditions::BooleanCondition::~BooleanCondition()
{
}

Data::QueryConditions::ConditionType Data::QueryConditions::BooleanCondition::GetType()
{
	return ConditionType::Boolean;
}

Bool Data::QueryConditions::BooleanCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	if (!this->val)
	{
		sb->AppendC(UTF8STRC("NOT "));
	}
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

Bool Data::QueryConditions::BooleanCondition::TestValid(NN<Data::VariItem> item)
{
	Bool bVal;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::F32:
		bVal = (item->GetItemValue().f32 != 0);
		break;
	case Data::VariItem::ItemType::F64:
		bVal = (item->GetItemValue().f64 != 0);
		break;
	case Data::VariItem::ItemType::I8:
		bVal = (item->GetItemValue().i8 != 0);
		break;
	case Data::VariItem::ItemType::U8:
		bVal = (item->GetItemValue().u8 != 0);
		break;
	case Data::VariItem::ItemType::I16:
		bVal = (item->GetItemValue().i16 != 0);
		break;
	case Data::VariItem::ItemType::U16:
		bVal = (item->GetItemValue().u16 != 0);
		break;
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::I32:
		bVal = (item->GetItemValue().i32 != 0);
		break;
	case Data::VariItem::ItemType::U32:
		bVal = (item->GetItemValue().u32 != 0);
		break;
	case Data::VariItem::ItemType::I64:
		bVal = (item->GetItemValue().i64 != 0);
		break;
	case Data::VariItem::ItemType::U64:
		bVal = (item->GetItemValue().u64 != 0);
		break;
	case Data::VariItem::ItemType::BOOL:
		bVal = item->GetItemValue().boolean;
		break;
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
	return bVal == this->val;
}

Data::QueryConditions::NotNullCondition::NotNullCondition(Text::CStringNN fieldName) : FieldCondition(fieldName)
{
}

Data::QueryConditions::NotNullCondition::~NotNullCondition()
{
}

Data::QueryConditions::ConditionType Data::QueryConditions::NotNullCondition::GetType()
{
	return ConditionType::NotNull;
}

Bool Data::QueryConditions::NotNullCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendC(UTF8STRC(" is not null"));
	return true;
}

Bool Data::QueryConditions::NotNullCondition::TestValid(NN<Data::VariItem> item)
{
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
	case Data::VariItem::ItemType::I8:
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::I16:
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::U64:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
		return true;
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	default:
		return false;
	}
}

Data::QueryConditions::InnerCondition::InnerCondition(NN<QueryConditions> innerCond)
{
	this->innerCond = innerCond;
}

Data::QueryConditions::InnerCondition::~InnerCondition()
{
	this->innerCond.Delete();
}

Data::QueryConditions::ConditionType Data::QueryConditions::InnerCondition::GetType()
{
	return ConditionType::Inner;
}

Bool Data::QueryConditions::InnerCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	Data::ArrayListNN<Condition> clientConditions;
	Text::StringBuilderUTF8 sbTmp;
	this->innerCond->ToWhereClause(sbTmp, sqlType, tzQhr, maxDBItem, clientConditions);
	if (clientConditions.GetCount() > 0)
	{
		return false;
	}
	else
	{
		sb->AppendUTF8Char('(');
		sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
		sb->AppendUTF8Char(')');
		return true;
	}
}

Bool Data::QueryConditions::InnerCondition::IsValid(NN<Data::VariObject> obj)
{
	return this->innerCond->IsValid(obj);
}

Bool Data::QueryConditions::InnerCondition::IsValid(NN<Data::ObjectGetter> getter)
{
	return this->innerCond->IsValid(getter);
}

void Data::QueryConditions::InnerCondition::GetFieldList(NN<Data::ArrayListStringNN> fieldList)
{
	this->innerCond->GetFieldList(fieldList);	
}

NN<Data::QueryConditions> Data::QueryConditions::InnerCondition::GetConditions()
{
	return this->innerCond;
}

Data::QueryConditions::OrCondition::OrCondition()
{
}

Data::QueryConditions::OrCondition::~OrCondition()
{
}

Data::QueryConditions::ConditionType Data::QueryConditions::OrCondition::GetType()
{
	return ConditionType::Or;
}

Bool Data::QueryConditions::OrCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	sb->AppendC(UTF8STRC(" or "));
	return true;
}

Bool Data::QueryConditions::OrCondition::IsValid(NN<Data::VariObject> obj)
{
	return true;
}

Bool Data::QueryConditions::OrCondition::IsValid(NN<Data::ObjectGetter> getter)
{
	return true;
}

void Data::QueryConditions::OrCondition::GetFieldList(NN<Data::ArrayListStringNN> fieldList)
{
}

Data::QueryConditions::QueryConditions()
{
}

Data::QueryConditions::~QueryConditions()
{
	this->conditionList.DeleteAll();
}

Bool Data::QueryConditions::IsValid(NN<Data::VariObject> obj)
{
	return ObjectValid(obj, this->conditionList);
}

Bool Data::QueryConditions::IsValid(NN<Data::ObjectGetter> getter)
{
	return ObjectValid(getter, this->conditionList);
}

Bool Data::QueryConditions::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem, NN<Data::ArrayListNN<Condition>> clientConditions)
{
	Text::StringBuilderUTF8 sbTmp;
	Bool hasOr = false;
	Data::ArrayIterator<NN<Condition>> it = this->conditionList.Iterator();
	NN<Condition> condition;
	while (it.HasNext())
	{
		condition = it.Next();
		if (condition->GetType() == ConditionType::Or)
		{
			hasOr = true;
			break;
		}
	}

	if (hasOr)
	{
		UOSInt splitType = 1;
		it = this->conditionList.Iterator();
		while (it.HasNext())
		{
			condition = it.Next();
			if (condition->GetType() == ConditionType::Or)
			{
				if (splitType != 0)
				{

				}
				else
				{
					sb->AppendUTF8Char(')');
					splitType = 2;
				}
			}
			else
			{
				sbTmp.ClearStr();
				if (!condition->ToWhereClause(sbTmp, sqlType, tzQhr, maxDBItem))
				{
					clientConditions->Add(condition);
				}
				else
				{
					if (splitType == 2)
					{
						sb->AppendC(UTF8STRC(" or ("));
						splitType = 0;
					}
					else if (splitType != 0)
					{
						sb->AppendUTF8Char('(');
						splitType = 0;
					}
					else
					{
						sb->AppendC(UTF8STRC(" and "));
					}
					sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
				}
			}
		}
		if (splitType == 0)
		{
			sb->AppendUTF8Char(')');
		}
	}
	else
	{
		Bool hasCond = false;
		it = this->conditionList.Iterator();
		while (it.HasNext())
		{
			condition = it.Next();
			sbTmp.ClearStr();
			if (!condition->ToWhereClause(sbTmp, sqlType, tzQhr, maxDBItem))
			{
				clientConditions->Add(condition);
			}
			else
			{
				if (hasCond)
				{
					sb->AppendC(UTF8STRC(" and "));
				}
				sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
				hasCond = true;
			}
		}
	}
	return true;
}

UOSInt Data::QueryConditions::GetCount()
{
	return this->conditionList.GetCount();
}

Optional<Data::QueryConditions::Condition> Data::QueryConditions::GetItem(UOSInt index)
{
	return this->conditionList.GetItem(index);
}

NN<Data::ArrayListNN<Data::QueryConditions::Condition>> Data::QueryConditions::GetList()
{
	return this->conditionList;
}

void Data::QueryConditions::GetFieldList(NN<Data::ArrayListStringNN> fieldList)
{
	Data::ArrayIterator<NN<Condition>> it = this->conditionList.Iterator();
	while (it.HasNext())
	{
		it.Next()->GetFieldList(fieldList);
	}
}

NN<Data::QueryConditions> Data::QueryConditions::TimeBetween(Text::CStringNN fieldName, const Data::Timestamp &t1, const Data::Timestamp &t2)
{
	NN<TimeBetweenCondition> cond;
	NEW_CLASSNN(cond, TimeBetweenCondition(fieldName, t1, t2));
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Or()
{
	NN<OrCondition> cond;
	NEW_CLASSNN(cond, OrCondition());
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::InnerCond(NN<QueryConditions> innerCond)
{
	NN<InnerCondition> cond;
	NEW_CLASSNN(cond, InnerCondition(innerCond));
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int32Equals(Text::CStringNN fieldName, Int32 val)
{
	NN<Int32Condition> cond;
	NEW_CLASSNN(cond, Int32Condition(fieldName, val, CompareCondition::Equal));
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int32In(Text::CStringNN fieldName, NN<Data::ArrayList<Int32>> vals)
{
	NN<Int32InCondition> cond;
	NEW_CLASSNN(cond, Int32InCondition(fieldName, vals));
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int64Equals(Text::CStringNN fieldName, Int64 val)
{
	NN<Int64Condition> cond;
	NEW_CLASSNN(cond, Int64Condition(fieldName, val, CompareCondition::Equal));
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::DoubleGE(Text::CStringNN fieldName, Double val)
{
	NN<DoubleCondition> cond;
	NEW_CLASSNN(cond, DoubleCondition(fieldName, val, CompareCondition::GreaterOrEqual));
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::DoubleLE(Text::CStringNN fieldName, Double val)
{
	NN<DoubleCondition> cond;
	NEW_CLASSNN(cond, DoubleCondition(fieldName, val, CompareCondition::GreaterOrEqual));
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::StrIn(Text::CStringNN fieldName, NN<Data::ArrayList<const UTF8Char*>> vals)
{
	NN<StringInCondition> cond;
	NEW_CLASSNN(cond, StringInCondition(fieldName, vals));
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::StrNotIn(Text::CStringNN fieldName, NN<Data::ArrayList<const UTF8Char*>> vals)
{
	NN<StringNotInCondition> cond;
	NEW_CLASSNN(cond, StringNotInCondition(fieldName, vals));
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::StrContains(Text::CStringNN fieldName, const UTF8Char *val)
{
	NN<StringContainsCondition> cond;
	NEW_CLASSNN(cond, StringContainsCondition(fieldName, val));
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::StrEquals(Text::CStringNN fieldName, Text::CString val)
{
	NN<StringEqualsCondition> cond;
	NEW_CLASSNN(cond, StringEqualsCondition(fieldName, val));
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::BoolEquals(Text::CStringNN fieldName, Bool val)
{
	NN<BooleanCondition> cond;
	NEW_CLASSNN(cond, BooleanCondition(fieldName, val));
	this->conditionList.Add(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::NotNull(Text::CStringNN fieldName)
{
	NN<NotNullCondition> cond;
	NEW_CLASSNN(cond, NotNullCondition(fieldName));
	this->conditionList.Add(cond);
	return *this;
}

Text::CStringNN Data::QueryConditions::CompareConditionGetStr(CompareCondition cond)
{
	switch (cond)
	{
	case CompareCondition::Equal:
		return CSTR(" = ");
	case CompareCondition::Greater:
		return CSTR(" > ");
	case CompareCondition::Less:
		return CSTR(" < ");
	case CompareCondition::GreaterOrEqual:
		return CSTR(" >= ");
	case CompareCondition::LessOrEqual:
		return CSTR(" <= ");
	case CompareCondition::NotEqual:
		return CSTR(" <> ");
	case CompareCondition::Unknown:
		return CSTR("");
	}
	return CSTR("");
}

Bool Data::QueryConditions::ObjectValid(NN<Data::VariObject> obj, NN<Data::ArrayListNN<Condition>> conditionList)
{
	Bool ret = true;
	NN<Condition> cond;
	Data::ArrayIterator<NN<Condition>> it = conditionList->Iterator();
	while (it.HasNext())
	{
		cond = it.Next();
		if (cond->GetType() == ConditionType::Or)
		{
			if (ret)
				return true;
			ret = true;
		}
		else
		{
			ret = ret && cond->IsValid(obj);
		}
	}
	return ret;
}

Bool Data::QueryConditions::ObjectValid(NN<Data::ObjectGetter> getter, NN<Data::ArrayListNN<Condition>> conditionList)
{
	Bool ret = true;
	NN<Condition> cond;
	Data::ArrayIterator<NN<Condition>> it = conditionList->Iterator();
	while (it.HasNext())
	{
		cond = it.Next();
		if (cond->GetType() == ConditionType::Or)
		{
			if (ret)
				return true;
			ret = true;
		}
		else
		{
			ret = ret && cond->IsValid(getter);
		}
	}
	return ret;
}

Data::QueryConditions *Data::QueryConditions::ParseStr(Text::CStringNN s, DB::SQLType sqlType)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbField;
	const UTF8Char *sql = s.v;
	NN<Data::VariItem> item;
	Data::QueryConditions *cond;
	UOSInt i;
	NEW_CLASS(cond, Data::QueryConditions());
	while (true)
	{
		sql = DB::SQL::SQLUtil::ParseNextWord(sql, sbField, sqlType);
		if (sbField.leng == 0)
		{
			DEL_CLASS(cond);
			return 0;
		}
		sql = DB::SQL::SQLUtil::ParseNextWord(sql, sb, sqlType);
		if (sb.EqualsICase(UTF8STRC("in")))
		{
			sql = DB::SQL::SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (!sb.Equals(UTF8STRC("(")))
			{
				DEL_CLASS(cond);
				return 0;
			}
			Data::ArrayListNN<Data::VariItem> items;
			while (true)
			{
				sql = DB::SQL::SQLUtil::ParseNextWord(sql, sb, sqlType);
				if (!item.Set(DB::SQL::SQLUtil::ParseValue(sb.ToCString(), sqlType)))
				{
					items.DeleteAll();
					DEL_CLASS(cond);
					return 0;
				}
				items.Add(item);
				sql = DB::SQL::SQLUtil::ParseNextWord(sql, sb, sqlType);
				if (sb.Equals(UTF8STRC(")")))
				{
					Data::VariItem::ItemType itemType = item->GetItemType();
					if (itemType == Data::VariItem::ItemType::Str)
					{
						Data::ArrayList<const UTF8Char*> vals;
						Data::ArrayIterator<NN<Data::VariItem>> it = items.Iterator();
						NN<Data::VariItem> item;
						while (it.HasNext())
						{
							item = it.Next();
							if (item->GetItemType() != itemType)
							{
								items.DeleteAll();
								DEL_CLASS(cond);
								return 0;
							}
							vals.Add(item->GetItemValue().str->v);
							i++;
						}
						cond->StrIn(sbField.ToCString(), vals);
						items.DeleteAll();
						break;
					}
					else
					{
						items.DeleteAll();
						DEL_CLASS(cond);
						return 0;
					}
				}
				else if (sb.Equals(UTF8STRC(",")))
				{
				}
				else
				{
					items.DeleteAll();
					DEL_CLASS(cond);
					return 0;
				}
			}
		}
		else if (sb.EqualsICase(UTF8STRC("not")))
		{
			sql = DB::SQL::SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (sb.EqualsICase(UTF8STRC("in")))
			{
				sql = DB::SQL::SQLUtil::ParseNextWord(sql, sb, sqlType);
				if (!sb.Equals(UTF8STRC("(")))
				{
					DEL_CLASS(cond);
					return 0;
				}
				Data::ArrayListNN<Data::VariItem> items;
				while (true)
				{
					sql = DB::SQL::SQLUtil::ParseNextWord(sql, sb, sqlType);
					if (!item.Set(DB::SQL::SQLUtil::ParseValue(sb.ToCString(), sqlType)))
					{
						items.DeleteAll();
						DEL_CLASS(cond);
						return 0;
					}
					items.Add(item);
					sql = DB::SQL::SQLUtil::ParseNextWord(sql, sb, sqlType);
					if (sb.Equals(UTF8STRC(")")))
					{
						Data::VariItem::ItemType itemType = item->GetItemType();
						if (itemType == Data::VariItem::ItemType::Str)
						{
							Data::ArrayList<const UTF8Char*> vals;
							Data::ArrayIterator<NN<Data::VariItem>> it = items.Iterator();
							NN<Data::VariItem> item;
							while (it.HasNext())
							{
								item = it.Next();
								if (item->GetItemType() != itemType)
								{
									items.DeleteAll();
									DEL_CLASS(cond);
									return 0;
								}
								vals.Add(item->GetItemValue().str->v);
								i++;
							}
							cond->StrNotIn(sbField.ToCString(), vals);
							items.DeleteAll();
							break;
						}
						else
						{
							items.DeleteAll();
							DEL_CLASS(cond);
							return 0;
						}
					}
					else if (sb.Equals(UTF8STRC(",")))
					{
					}
					else
					{
						items.DeleteAll();
						DEL_CLASS(cond);
						return 0;
					}
				}
			}
			else
			{
				DEL_CLASS(cond);
				return 0;
			}
		}
		else if (sb.Equals(UTF8STRC("=")))
		{
			sql = DB::SQL::SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (!item.Set(DB::SQL::SQLUtil::ParseValue(sb.ToCString(), sqlType)))
			{
				DEL_CLASS(cond);
				return 0;
			}
			Data::VariItem::ItemType itemType = item->GetItemType();
			if (itemType == Data::VariItem::ItemType::Str)
			{
				cond->StrEquals(sbField.ToCString(), item->GetItemValue().str->ToCString());
			}
			item.Delete();
		}
		else
		{
			DEL_CLASS(cond);
			return 0;
		}
		sql = DB::SQL::SQLUtil::ParseNextWord(sql, sb, sqlType);
		if (sb.GetLength() == 0)
		{
			return cond;
		}
		else
		{
			DEL_CLASS(cond);
			return 0;
		}
	}
	return 0;
}
