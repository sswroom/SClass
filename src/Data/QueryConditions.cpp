#include "Stdafx.h"
#include "Data/QueryConditions.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

Data::QueryConditions::FieldCondition::FieldCondition(Text::CStringNN fieldName)
{
	this->fieldName = Text::String::New(fieldName);
}

Data::QueryConditions::FieldCondition::FieldCondition(NotNullPtr<Text::String> fieldName)
{
	this->fieldName = fieldName->Clone();
}

Data::QueryConditions::FieldCondition::~FieldCondition()
{
	this->fieldName->Release();
}

Bool Data::QueryConditions::FieldCondition::IsValid(NotNullPtr<Data::VariObject> obj)
{
	NotNullPtr<Data::VariItem> item;
	if (!item.Set(obj->GetItem(this->fieldName->v)))
		return false;
	return this->TestValid(item);
}

Bool Data::QueryConditions::FieldCondition::IsValid(NotNullPtr<Data::ObjectGetter> getter)
{
	NotNullPtr<Data::VariItem> item = getter->GetNewItem(this->fieldName->v);
	Bool ret = this->TestValid(item);
	item.Delete();
	return ret;
}

void Data::QueryConditions::FieldCondition::GetFieldList(NotNullPtr<Data::ArrayListNN<Text::String>> fieldList)
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

Bool Data::QueryConditions::TimeBetweenCondition::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
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

Bool Data::QueryConditions::TimeBetweenCondition::TestValid(NotNullPtr<Data::VariItem> item)
{
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Timestamp:
		{
			Data::Timestamp t = item->GetItemValue().ts;
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

Bool Data::QueryConditions::Int32Condition::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->Append(Data::QueryConditions::CompareConditionGetStr(cond));
	sb->AppendI32(this->val);
	return true;
}

Bool Data::QueryConditions::Int32Condition::TestValid(NotNullPtr<Data::VariItem> item)
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

Data::QueryConditions::CompareCondition Data::QueryConditions::Int32Condition::GetCompareCond()
{
	return this->cond;
}

Data::QueryConditions::Int32InCondition::Int32InCondition(Text::CStringNN fieldName, NotNullPtr<Data::ArrayList<Int32>> val) : FieldCondition(fieldName)
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

Bool Data::QueryConditions::Int32InCondition::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
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

Bool Data::QueryConditions::Int32InCondition::TestValid(NotNullPtr<Data::VariItem> item)
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

Bool Data::QueryConditions::Int64Condition::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->Append(Data::QueryConditions::CompareConditionGetStr(cond));
	sb->AppendI64(this->val);
	return true;
}

Bool Data::QueryConditions::Int64Condition::TestValid(NotNullPtr<Data::VariItem> item)
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

Data::QueryConditions::CompareCondition Data::QueryConditions::Int64Condition::GetCompareCond()
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

Bool Data::QueryConditions::DoubleCondition::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->Append(Data::QueryConditions::CompareConditionGetStr(cond));
	Text::SBAppendF64(sb, this->val);
	return true;
}

Bool Data::QueryConditions::DoubleCondition::TestValid(NotNullPtr<Data::VariItem> item)
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
	}
	return false;
}

Data::QueryConditions::StringInCondition::StringInCondition(Text::CStringNN fieldName, Data::ArrayList<const UTF8Char*> *val) : FieldCondition(fieldName)
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

Bool Data::QueryConditions::StringInCondition::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
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

Bool Data::QueryConditions::StringInCondition::TestValid(NotNullPtr<Data::VariItem> item)
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
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::U64:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Timestamp:
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

Bool Data::QueryConditions::StringContainsCondition::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
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

Bool Data::QueryConditions::StringContainsCondition::TestValid(NotNullPtr<Data::VariItem> item)
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
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::U64:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Timestamp:
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

Bool Data::QueryConditions::StringEqualsCondition::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
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

Bool Data::QueryConditions::StringEqualsCondition::TestValid(NotNullPtr<Data::VariItem> item)
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
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::U64:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Timestamp:
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

Bool Data::QueryConditions::BooleanCondition::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
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

Bool Data::QueryConditions::BooleanCondition::TestValid(NotNullPtr<Data::VariItem> item)
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

Bool Data::QueryConditions::NotNullCondition::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendC(UTF8STRC(" is not null"));
	return true;
}

Bool Data::QueryConditions::NotNullCondition::TestValid(NotNullPtr<Data::VariItem> item)
{
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
	case Data::VariItem::ItemType::I8:
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::I16:
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::U64:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Timestamp:
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

Data::QueryConditions::InnerCondition::InnerCondition(NotNullPtr<QueryConditions> innerCond)
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

Bool Data::QueryConditions::InnerCondition::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	Data::ArrayList<Condition*> clientConditions;
	Text::StringBuilderUTF8 sbTmp;
	this->innerCond->ToWhereClause(sbTmp, sqlType, tzQhr, maxDBItem, &clientConditions);
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

Bool Data::QueryConditions::InnerCondition::IsValid(NotNullPtr<Data::VariObject> obj)
{
	return this->innerCond->IsValid(obj);
}

Bool Data::QueryConditions::InnerCondition::IsValid(NotNullPtr<Data::ObjectGetter> getter)
{
	return this->innerCond->IsValid(getter);
}

void Data::QueryConditions::InnerCondition::GetFieldList(NotNullPtr<Data::ArrayListNN<Text::String>> fieldList)
{
	this->innerCond->GetFieldList(fieldList);	
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::InnerCondition::GetConditions()
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

Bool Data::QueryConditions::OrCondition::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem)
{
	sb->AppendC(UTF8STRC(" or "));
	return true;
}

Bool Data::QueryConditions::OrCondition::IsValid(NotNullPtr<Data::VariObject> obj)
{
	return true;
}

Bool Data::QueryConditions::OrCondition::IsValid(NotNullPtr<Data::ObjectGetter> getter)
{
	return true;
}

void Data::QueryConditions::OrCondition::GetFieldList(NotNullPtr<Data::ArrayListNN<Text::String>> fieldList)
{
}

Data::QueryConditions::QueryConditions()
{
}

Data::QueryConditions::~QueryConditions()
{
	LIST_FREE_FUNC(&this->conditionList, DEL_CLASS);
}

Bool Data::QueryConditions::IsValid(NotNullPtr<Data::VariObject> obj)
{
	return ObjectValid(obj, this->conditionList);
}

Bool Data::QueryConditions::IsValid(NotNullPtr<Data::ObjectGetter> getter)
{
	return ObjectValid(getter, this->conditionList);
}

Bool Data::QueryConditions::ToWhereClause(NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem, Data::ArrayList<Condition*> *clientConditions)
{
	Text::StringBuilderUTF8 sbTmp;
	Bool hasOr = false;
	UOSInt i = 0;
	UOSInt j = this->conditionList.GetCount();
	while (i < j)
	{
		Condition *condition = this->conditionList.GetItem(i);
		if (condition->GetType() == ConditionType::Or)
		{
			hasOr = true;
			break;
		}
		i++;
	}

	if (hasOr)
	{
		UOSInt splitType = 1;
		i = 0;
		j = this->conditionList.GetCount();
		while (i < j)
		{
			Condition *condition = this->conditionList.GetItem(i);
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
			i++;
		}
		if (splitType == 0)
		{
			sb->AppendUTF8Char(')');
		}
	}
	else
	{
		Bool hasCond = false;
		i = 0;
		j = this->conditionList.GetCount();
		while (i < j)
		{
			Condition *condition = this->conditionList.GetItem(i);
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
			i++;
		}
	}
	return true;
}

UOSInt Data::QueryConditions::GetCount()
{
	return this->conditionList.GetCount();
}

Data::QueryConditions::Condition *Data::QueryConditions::GetItem(UOSInt index)
{
	return this->conditionList.GetItem(index);
}

NotNullPtr<Data::ArrayListNN<Data::QueryConditions::Condition>> Data::QueryConditions::GetList()
{
	return this->conditionList;
}

void Data::QueryConditions::GetFieldList(NotNullPtr<Data::ArrayListNN<Text::String>> fieldList)
{
	UOSInt i = 0;
	UOSInt j = this->conditionList.GetCount();
	while (i < j)
	{
		this->conditionList.GetItem(i)->GetFieldList(fieldList);
		i++;
	}
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::TimeBetween(Text::CStringNN fieldName, const Data::Timestamp &t1, const Data::Timestamp &t2)
{
	NotNullPtr<TimeBetweenCondition> cond;
	NEW_CLASSNN(cond, TimeBetweenCondition(fieldName, t1, t2));
	this->conditionList.Add(cond);
	return *this;
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::Or()
{
	NotNullPtr<OrCondition> cond;
	NEW_CLASSNN(cond, OrCondition());
	this->conditionList.Add(cond);
	return *this;
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::InnerCond(NotNullPtr<QueryConditions> innerCond)
{
	NotNullPtr<InnerCondition> cond;
	NEW_CLASSNN(cond, InnerCondition(innerCond));
	this->conditionList.Add(cond);
	return *this;
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::Int32Equals(Text::CStringNN fieldName, Int32 val)
{
	NotNullPtr<Int32Condition> cond;
	NEW_CLASSNN(cond, Int32Condition(fieldName, val, CompareCondition::Equal));
	this->conditionList.Add(cond);
	return *this;
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::Int32In(Text::CStringNN fieldName, NotNullPtr<Data::ArrayList<Int32>> vals)
{
	NotNullPtr<Int32InCondition> cond;
	NEW_CLASSNN(cond, Int32InCondition(fieldName, vals));
	this->conditionList.Add(cond);
	return *this;
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::Int64Equals(Text::CStringNN fieldName, Int64 val)
{
	NotNullPtr<Int64Condition> cond;
	NEW_CLASSNN(cond, Int64Condition(fieldName, val, CompareCondition::Equal));
	this->conditionList.Add(cond);
	return *this;
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::DoubleGE(Text::CStringNN fieldName, Double val)
{
	NotNullPtr<DoubleCondition> cond;
	NEW_CLASSNN(cond, DoubleCondition(fieldName, val, CompareCondition::GreaterOrEqual));
	this->conditionList.Add(cond);
	return *this;
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::DoubleLE(Text::CStringNN fieldName, Double val)
{
	NotNullPtr<DoubleCondition> cond;
	NEW_CLASSNN(cond, DoubleCondition(fieldName, val, CompareCondition::GreaterOrEqual));
	this->conditionList.Add(cond);
	return *this;
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::StrIn(Text::CStringNN fieldName, Data::ArrayList<const UTF8Char*> *vals)
{
	NotNullPtr<StringInCondition> cond;
	NEW_CLASSNN(cond, StringInCondition(fieldName, vals));
	this->conditionList.Add(cond);
	return *this;
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::StrContains(Text::CStringNN fieldName, const UTF8Char *val)
{
	NotNullPtr<StringContainsCondition> cond;
	NEW_CLASSNN(cond, StringContainsCondition(fieldName, val));
	this->conditionList.Add(cond);
	return *this;
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::StrEquals(Text::CStringNN fieldName, Text::CString val)
{
	NotNullPtr<StringEqualsCondition> cond;
	NEW_CLASSNN(cond, StringEqualsCondition(fieldName, val));
	this->conditionList.Add(cond);
	return *this;
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::BoolEquals(Text::CStringNN fieldName, Bool val)
{
	NotNullPtr<BooleanCondition> cond;
	NEW_CLASSNN(cond, BooleanCondition(fieldName, val));
	this->conditionList.Add(cond);
	return *this;
}

NotNullPtr<Data::QueryConditions> Data::QueryConditions::NotNull(Text::CStringNN fieldName)
{
	NotNullPtr<NotNullCondition> cond;
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
	}
	return CSTR("");
}

Bool Data::QueryConditions::ObjectValid(NotNullPtr<Data::VariObject> obj, NotNullPtr<Data::ArrayListNN<Condition>> conditionList)
{
	Bool ret = true;
	Condition *cond;
	UOSInt i = 0;
	UOSInt j = conditionList->GetCount();
	while (i < j)
	{
		cond = conditionList->GetItem(i);
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
		i++;
	}
	return ret;
}

Bool Data::QueryConditions::ObjectValid(NotNullPtr<Data::ObjectGetter> getter, NotNullPtr<Data::ArrayListNN<Condition>> conditionList)
{
	Bool ret = true;
	NotNullPtr<Condition> cond;
	Data::ArrayIterator<NotNullPtr<Condition>> it = conditionList->Iterator();
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
