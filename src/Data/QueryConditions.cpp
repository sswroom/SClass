#include "Stdafx.h"
#include "Data/QueryConditions.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

Data::QueryConditions::FieldCondition::FieldCondition(const UTF8Char *fieldName, UOSInt nameLen)
{
	this->fieldName = Text::String::New(fieldName, nameLen);
}

Data::QueryConditions::FieldCondition::FieldCondition(Text::String *fieldName)
{
	this->fieldName = fieldName->Clone();
}

Data::QueryConditions::FieldCondition::~FieldCondition()
{
	this->fieldName->Release();
}

Bool Data::QueryConditions::FieldCondition::IsValid(Data::VariObject *obj)
{
	return this->TestValid(obj->GetItem(this->fieldName->v));
}

Bool Data::QueryConditions::FieldCondition::IsValid(Data::ObjectGetter *getter)
{
	Data::VariItem *item = getter->GetNewItem(this->fieldName->v);
	Bool ret = this->TestValid(item);
	SDEL_CLASS(item);
	return ret;
}

void Data::QueryConditions::FieldCondition::GetFieldList(Data::ArrayList<Text::String*> *fieldList)
{
	fieldList->Add(this->fieldName);
}

Data::QueryConditions::TimeBetweenCondition::TimeBetweenCondition(const UTF8Char *fieldName, UOSInt nameLen, Int64 t1, Int64 t2) : FieldCondition(fieldName, nameLen)
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

Bool Data::QueryConditions::TimeBetweenCondition::ToWhereClause(Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Data::DateTime dt;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, svrType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendC(UTF8STRC(" between "));
	dt.SetTicks(this->t1);
	sptr = DB::DBUtil::SDBDate(sbuff, &dt, svrType, tzQhr);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendC(UTF8STRC(" and "));
	dt.SetTicks(this->t2);
	sptr = DB::DBUtil::SDBDate(sbuff, &dt, svrType, tzQhr);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

Bool Data::QueryConditions::TimeBetweenCondition::TestValid(Data::VariItem *item)
{
	if (item == 0) return false;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Date:
		{
			Int64 t = item->GetItemValue().date->ToTicks();
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
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
}

Data::QueryConditions::Int32Condition::Int32Condition(const UTF8Char *fieldName, UOSInt nameLen, Int32 val, CompareCondition cond) : FieldCondition(fieldName, nameLen)
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

Bool Data::QueryConditions::Int32Condition::ToWhereClause(Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, svrType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->Append(Data::QueryConditions::CompareConditionGetStr(cond));
	sb->AppendI32(this->val);
	return true;
}

Bool Data::QueryConditions::Int32Condition::TestValid(Data::VariItem *item)
{
	if (item == 0) return false;
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
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
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
	}
	return false;
}

Text::String *Data::QueryConditions::Int32Condition::GetFieldName()
{
	return this->fieldName;
}

Int32 Data::QueryConditions::Int32Condition::GetVal()
{
	return this->val;
}

Data::QueryConditions::CompareCondition Data::QueryConditions::Int32Condition::GetCompareCond()
{
	return this->cond;
}

Data::QueryConditions::Int32InCondition::Int32InCondition(const UTF8Char *fieldName, UOSInt nameLen, Data::ArrayList<Int32> *val) : FieldCondition(fieldName, nameLen)
{
	NEW_CLASS(this->vals, Data::ArrayList<Int32>());
	this->vals->AddAll(val);
}

Data::QueryConditions::Int32InCondition::~Int32InCondition()
{
	DEL_CLASS(this->vals);
}

Data::QueryConditions::ConditionType Data::QueryConditions::Int32InCondition::GetType()
{
	return ConditionType::Int32In;
}

Bool Data::QueryConditions::Int32InCondition::ToWhereClause(Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	if (this->vals->GetCount() > maxDBItem)
	{
		return false;
	}
	else
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, svrType);
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendC(UTF8STRC(" in ("));
		Text::StringTool::Int32Join(sb, this->vals, {UTF8STRC(", ")});
		sb->AppendC(UTF8STRC(")"));
		return true;
	}
}

Bool Data::QueryConditions::Int32InCondition::TestValid(Data::VariItem *item)
{
	if (item == 0) return false;
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
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
	UOSInt i = this->vals->GetCount();
	while (i-- > 0)
	{
		if (iVal == this->vals->GetItem(i))
		{
			return true;
		}
	}
	return false;
}

Data::QueryConditions::DoubleCondition::DoubleCondition(const UTF8Char *fieldName, UOSInt nameLen, Double val, CompareCondition cond) : FieldCondition(fieldName, nameLen)
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

Bool Data::QueryConditions::DoubleCondition::ToWhereClause(Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, svrType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->Append(Data::QueryConditions::CompareConditionGetStr(cond));
	Text::SBAppendF64(sb, this->val);
	return true;
}

Bool Data::QueryConditions::DoubleCondition::TestValid(Data::VariItem *item)
{
	if (item == 0) return false;
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
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
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
	}
	return false;
}

Data::QueryConditions::StringInCondition::StringInCondition(const UTF8Char *fieldName, UOSInt nameLen, Data::ArrayList<const UTF8Char*> *val) : FieldCondition(fieldName, nameLen)
{
	NEW_CLASS(this->vals, Data::ArrayList<const UTF8Char*>());
	UOSInt i = 0;
	UOSInt j = val->GetCount();
	while (i < j)
	{
		this->vals->Add(Text::StrCopyNew(val->GetItem(i)));
		i++;
	}
}

Data::QueryConditions::StringInCondition::~StringInCondition()
{
	LIST_FREE_FUNC(this->vals, Text::StrDelNew);
	DEL_CLASS(this->vals);
}

Data::QueryConditions::ConditionType Data::QueryConditions::StringInCondition::GetType()
{
	return ConditionType::StringIn;
}

Bool Data::QueryConditions::StringInCondition::ToWhereClause(Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	if (this->vals->GetCount() > maxDBItem || this->vals->GetCount() == 0)
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
		sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, svrType);
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendC(UTF8STRC(" in ("));
		i = 0;
		j = this->vals->GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sb->AppendC(UTF8STRC(", "));
			}
			thisSize = DB::DBUtil::SDBStrUTF8Leng(this->vals->GetItem(i), svrType);
			if (thisSize < 512)
			{
				sptr = DB::DBUtil::SDBStrUTF8(sbuff, this->vals->GetItem(i), svrType);
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
				sptr = DB::DBUtil::SDBStrUTF8(sptrTmp, this->vals->GetItem(i), svrType);
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

Bool Data::QueryConditions::StringInCondition::TestValid(Data::VariItem *item)
{
	if (item == 0) return false;
	const UTF8Char *csptr;
	UOSInt i;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Str:
		csptr = item->GetItemValue().str->v;
		i = this->vals->GetCount();
		while (i-- > 0)
		{
			if (Text::StrEquals(csptr, this->vals->GetItem(i)))
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
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
}

Data::QueryConditions::StringContainsCondition::StringContainsCondition(const UTF8Char *fieldName, UOSInt nameLen, const UTF8Char *val) : FieldCondition(fieldName, nameLen)
{
	this->val = Text::String::NewNotNull(val);
}

Data::QueryConditions::StringContainsCondition::~StringContainsCondition()
{
	this->val->Release();
}

Data::QueryConditions::ConditionType Data::QueryConditions::StringContainsCondition::GetType()
{
	return ConditionType::StringContains;
}

Bool Data::QueryConditions::StringContainsCondition::ToWhereClause(Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr2;
	sptr2 = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, svrType);
	sb->AppendC(sbuff, (UOSInt)(sptr2 - sbuff));
	sb->AppendC(UTF8STRC(" like "));
	Text::StringBuilderUTF8 sb2;
	sb2.AppendUTF8Char('%');
	sb2.Append(this->val);
	sb2.AppendUTF8Char('%');
	UOSInt size = DB::DBUtil::SDBStrUTF8Leng(sb2.ToString(), svrType);
	if (size < 512)
	{
		sptr2 = DB::DBUtil::SDBStrUTF8(sbuff, sb2.ToString(), svrType);
		sb->AppendC(sbuff, (UOSInt)(sptr2 - sbuff));
	}
	else
	{
		UTF8Char *sptr = MemAlloc(UTF8Char, size + 1);
		sptr2 = DB::DBUtil::SDBStrUTF8(sptr, sb2.ToString(), svrType);
		sb->AppendC(sptr, (UOSInt)(sptr2 - sptr));
		MemFree(sptr);
	}
	return true;
}

Bool Data::QueryConditions::StringContainsCondition::TestValid(Data::VariItem *item)
{
	if (item == 0) return false;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Str:
		return item->GetItemValue().str->IndexOf(this->val->v, this->val->leng) != INVALID_INDEX;
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
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
}

Data::QueryConditions::StringEqualsCondition::StringEqualsCondition(const UTF8Char *fieldName, UOSInt nameLen, const UTF8Char *val) : FieldCondition(fieldName, nameLen)
{
	this->val = Text::StrCopyNew(val);
}

Data::QueryConditions::StringEqualsCondition::~StringEqualsCondition()
{
	Text::StrDelNew(this->val);
}

Data::QueryConditions::ConditionType Data::QueryConditions::StringEqualsCondition::GetType()
{
	return ConditionType::StringEquals;
}

Bool Data::QueryConditions::StringEqualsCondition::ToWhereClause(Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr2;
	sptr2 = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, svrType);
	sb->AppendC(sbuff, (UOSInt)(sptr2 - sbuff));
	sb->AppendC(UTF8STRC(" = "));
	UOSInt size = DB::DBUtil::SDBStrUTF8Leng(this->val, svrType);
	if (size < 512)
	{
		sptr2 = DB::DBUtil::SDBStrUTF8(sbuff, this->val, svrType);
		sb->AppendC(sbuff, (UOSInt)(sptr2 - sbuff));
	}
	else
	{
		UTF8Char *sptr = MemAlloc(UTF8Char, size + 1);
		sptr2 = DB::DBUtil::SDBStrUTF8(sptr, this->val, svrType);
		sb->AppendC(sptr, (UOSInt)(sptr2 - sptr));
		MemFree(sptr);
	}
	return true;
}

Bool Data::QueryConditions::StringEqualsCondition::TestValid(Data::VariItem *item)
{
	if (item == 0) return false;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Str:
		return Text::StrEquals(item->GetItemValue().str->v, this->val);
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
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
}

Data::QueryConditions::BooleanCondition::BooleanCondition(const UTF8Char *fieldName, UOSInt nameLen, Bool val) : FieldCondition(fieldName, nameLen)
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

Bool Data::QueryConditions::BooleanCondition::ToWhereClause(Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	if (!this->val)
	{
		sb->AppendC(UTF8STRC("NOT "));
	}
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, svrType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

Bool Data::QueryConditions::BooleanCondition::TestValid(Data::VariItem *item)
{
	if (item == 0) return false;
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
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}
	return bVal == this->val;
}

Data::QueryConditions::NotNullCondition::NotNullCondition(const UTF8Char *fieldName, UOSInt nameLen) : FieldCondition(fieldName, nameLen)
{
}

Data::QueryConditions::NotNullCondition::~NotNullCondition()
{
}

Data::QueryConditions::ConditionType Data::QueryConditions::NotNullCondition::GetType()
{
	return ConditionType::NotNull;
}

Bool Data::QueryConditions::NotNullCondition::ToWhereClause(Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, svrType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendC(UTF8STRC(" is not null"));
	return true;
}

Bool Data::QueryConditions::NotNullCondition::TestValid(Data::VariItem *item)
{
	if (item == 0) return false;
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

Data::QueryConditions::InnerCondition::InnerCondition(QueryConditions *innerCond)
{
	this->innerCond = innerCond;
}

Data::QueryConditions::InnerCondition::~InnerCondition()
{
	DEL_CLASS(this->innerCond);
}

Data::QueryConditions::ConditionType Data::QueryConditions::InnerCondition::GetType()
{
	return ConditionType::Inner;
}

Bool Data::QueryConditions::InnerCondition::ToWhereClause(Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	Data::ArrayList<Condition*> clientConditions;
	Text::StringBuilderUTF8 sbTmp;
	this->innerCond->ToWhereClause(&sbTmp, svrType, tzQhr, maxDBItem, &clientConditions);
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

Bool Data::QueryConditions::InnerCondition::IsValid(Data::VariObject *obj)
{
	return this->innerCond->IsValid(obj);
}

Bool Data::QueryConditions::InnerCondition::IsValid(Data::ObjectGetter *getter)
{
	return this->innerCond->IsValid(getter);
}

void Data::QueryConditions::InnerCondition::GetFieldList(Data::ArrayList<Text::String*> *fieldList)
{
	this->innerCond->GetFieldList(fieldList);	
}

Data::QueryConditions *Data::QueryConditions::InnerCondition::GetConditions()
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

Bool Data::QueryConditions::OrCondition::ToWhereClause(Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	sb->AppendC(UTF8STRC(" or "));
	return true;
}

Bool Data::QueryConditions::OrCondition::IsValid(Data::VariObject *obj)
{
	return true;
}

Bool Data::QueryConditions::OrCondition::IsValid(Data::ObjectGetter *getter)
{
	return true;
}

void Data::QueryConditions::OrCondition::GetFieldList(Data::ArrayList<Text::String*> *fieldList)
{
}

Data::QueryConditions::QueryConditions()
{
	NEW_CLASS(this->conditionList, Data::ArrayList<Condition*>());
}

Data::QueryConditions::~QueryConditions()
{
	LIST_FREE_FUNC(this->conditionList, DEL_CLASS);
	DEL_CLASS(this->conditionList);
}

Bool Data::QueryConditions::IsValid(Data::VariObject *obj)
{
	return ObjectValid(obj, this->conditionList);
}

Bool Data::QueryConditions::IsValid(Data::ObjectGetter *getter)
{
	return ObjectValid(getter, this->conditionList);
}

Bool Data::QueryConditions::ToWhereClause(Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem, Data::ArrayList<Condition*> *clientConditions)
{
	Text::StringBuilderUTF8 sbTmp;
	Bool hasOr = false;
	UOSInt i = 0;
	UOSInt j = this->conditionList->GetCount();
	while (i < j)
	{
		Condition *condition = this->conditionList->GetItem(i);
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
		j = this->conditionList->GetCount();
		while (i < j)
		{
			Condition *condition = this->conditionList->GetItem(i);
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
				if (!condition->ToWhereClause(&sbTmp, svrType, tzQhr, maxDBItem))
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
		j = this->conditionList->GetCount();
		while (i < j)
		{
			Condition *condition = this->conditionList->GetItem(i);
			sbTmp.ClearStr();
			if (!condition->ToWhereClause(&sbTmp, svrType, tzQhr, maxDBItem))
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
	return this->conditionList->GetCount();
}

Data::QueryConditions::Condition *Data::QueryConditions::GetItem(UOSInt index)
{
	return this->conditionList->GetItem(index);
}

Data::ArrayList<Data::QueryConditions::Condition*> *Data::QueryConditions::GetList()
{
	return this->conditionList;
}

void Data::QueryConditions::GetFieldList(Data::ArrayList<Text::String*> *fieldList)
{
	UOSInt i = 0;
	UOSInt j = this->conditionList->GetCount();
	while (i < j)
	{
		this->conditionList->GetItem(i)->GetFieldList(fieldList);
		i++;
	}
}

Data::QueryConditions *Data::QueryConditions::TimeBetween(const UTF8Char *fieldName, UOSInt nameLen, Int64 t1, Int64 t2)
{
	this->conditionList->Add(NEW_CLASS_D(TimeBetweenCondition(fieldName, nameLen, t1, t2)));
	return this;
}

Data::QueryConditions *Data::QueryConditions::Or()
{
	this->conditionList->Add(NEW_CLASS_D(OrCondition()));
	return this;
}

Data::QueryConditions *Data::QueryConditions::InnerCond(QueryConditions *cond)
{
	this->conditionList->Add(NEW_CLASS_D(InnerCondition(cond)));
	return this;
}

Data::QueryConditions *Data::QueryConditions::Int32Equals(const UTF8Char *fieldName, UOSInt nameLen, Int32 val)
{
	this->conditionList->Add(NEW_CLASS_D(Int32Condition(fieldName, nameLen, val, CompareCondition::Equal)));
	return this;
}

Data::QueryConditions *Data::QueryConditions::Int32In(const UTF8Char *fieldName, UOSInt nameLen, Data::ArrayList<Int32> *vals)
{
	this->conditionList->Add(NEW_CLASS_D(Int32InCondition(fieldName, nameLen, vals)));
	return this;
}

Data::QueryConditions *Data::QueryConditions::DoubleGE(const UTF8Char *fieldName, UOSInt nameLen, Double val)
{
	this->conditionList->Add(NEW_CLASS_D(DoubleCondition(fieldName, nameLen, val, CompareCondition::GreaterOrEqual)));
	return this;
}

Data::QueryConditions *Data::QueryConditions::DoubleLE(const UTF8Char *fieldName, UOSInt nameLen, Double val)
{
	this->conditionList->Add(NEW_CLASS_D(DoubleCondition(fieldName, nameLen, val, CompareCondition::GreaterOrEqual)));
	return this;
}

Data::QueryConditions *Data::QueryConditions::StrIn(const UTF8Char *fieldName, UOSInt nameLen, Data::ArrayList<const UTF8Char*> *vals)
{
	this->conditionList->Add(NEW_CLASS_D(StringInCondition(fieldName, nameLen, vals)));
	return this;
}

Data::QueryConditions *Data::QueryConditions::StrContains(const UTF8Char *fieldName, UOSInt nameLen, const UTF8Char *val)
{
	this->conditionList->Add(NEW_CLASS_D(StringContainsCondition(fieldName, nameLen, val)));
	return this;
}

Data::QueryConditions *Data::QueryConditions::StrEquals(const UTF8Char *fieldName, UOSInt nameLen, const UTF8Char *val)
{
	this->conditionList->Add(NEW_CLASS_D(StringEqualsCondition(fieldName, nameLen, val)));
	return this;
}

Data::QueryConditions *Data::QueryConditions::BoolEquals(const UTF8Char *fieldName, UOSInt nameLen, Bool val)
{
	this->conditionList->Add(NEW_CLASS_D(BooleanCondition(fieldName, nameLen, val)));
	return this;
}

Data::QueryConditions *Data::QueryConditions::NotNull(const UTF8Char* fieldName, UOSInt nameLen)
{
	this->conditionList->Add(NEW_CLASS_D(NotNullCondition(fieldName, nameLen)));
	return this;
}

Text::CString Data::QueryConditions::CompareConditionGetStr(CompareCondition cond)
{
	switch (cond)
	{
	case CompareCondition::Equal:
		return {UTF8STRC(" = ")};
	case CompareCondition::Greater:
		return {UTF8STRC(" > ")};
	case CompareCondition::Less:
		return {UTF8STRC(" < ")};
	case CompareCondition::GreaterOrEqual:
		return {UTF8STRC(" >= ")};
	case CompareCondition::LessOrEqual:
		return {UTF8STRC(" <= ")};
	case CompareCondition::NotEqual:
		return {UTF8STRC(" <> ")};
	}
	return {UTF8STRC("")};
}

Bool Data::QueryConditions::ObjectValid(Data::VariObject *obj, Data::ArrayList<Condition*> *conditionList)
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

Bool Data::QueryConditions::ObjectValid(Data::ObjectGetter *getter, Data::ArrayList<Condition*> *conditionList)
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
			ret = ret && cond->IsValid(getter);
		}
		i++;
	}
	return ret;
}
