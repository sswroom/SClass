#include "Stdafx.h"
#include "DB/QueryConditions.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

DB::QueryConditions::FieldCondition::FieldCondition(const UTF8Char *fieldName)
{
	this->fieldName = Text::StrCopyNew(fieldName);
}

DB::QueryConditions::FieldCondition::~FieldCondition()
{
	Text::StrDelNew(this->fieldName);
}

Bool DB::QueryConditions::FieldCondition::IsValid(Data::VariObject *obj)
{
	return this->TestValid(obj->GetItem(this->fieldName));
}

Bool DB::QueryConditions::FieldCondition::IsValid(Data::ObjectGetter *getter)
{
	Data::VariItem *item = getter->GetNewItem(this->fieldName);
	Bool ret = this->TestValid(item);
	SDEL_CLASS(item);
	return ret;
}

void DB::QueryConditions::FieldCondition::GetFieldList(Data::ArrayList<const UTF8Char*> *fieldList)
{
	fieldList->Add(this->fieldName);
}

DB::QueryConditions::TimeBetweenCondition::TimeBetweenCondition(const UTF8Char *fieldName, Int64 t1, Int64 t2) : FieldCondition(fieldName)
{
	this->t1 = t1;
	this->t2 = t2;
}

DB::QueryConditions::TimeBetweenCondition::~TimeBetweenCondition()
{
}

DB::QueryConditions::ConditionType DB::QueryConditions::TimeBetweenCondition::GetType()
{
	return ConditionType::TimeBetween;
}

Bool DB::QueryConditions::TimeBetweenCondition::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	Data::DateTime dt;
	DB::DBUtil::SDBColUTF8(sbuff, this->fieldName, svrType);
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)" between ");
	dt.SetTicks(this->t1);
	DB::DBUtil::SDBDate(sbuff, &dt, svrType, tzQhr);
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)" and ");
	dt.SetTicks(this->t2);
	DB::DBUtil::SDBDate(sbuff, &dt, svrType, tzQhr);
	sb->Append(sbuff);
	return true;
}

Bool DB::QueryConditions::TimeBetweenCondition::TestValid(Data::VariItem *item)
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

DB::QueryConditions::Int32Condition::Int32Condition(const UTF8Char *fieldName, Int32 val, CompareCondition cond) : FieldCondition(fieldName)
{
	this->val = val;
	this->cond = cond;
}

DB::QueryConditions::Int32Condition::~Int32Condition()
{
}

DB::QueryConditions::ConditionType DB::QueryConditions::Int32Condition::GetType()
{
	return ConditionType::INT32;
}

Bool DB::QueryConditions::Int32Condition::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	DBUtil::SDBColUTF8(sbuff, this->fieldName, svrType);
	sb->Append(sbuff);
	sb->Append(DB::QueryConditions::CompareConditionGetStr(cond));
	sb->AppendI32(this->val);
	return true;
}

Bool DB::QueryConditions::Int32Condition::TestValid(Data::VariItem *item)
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

const UTF8Char *DB::QueryConditions::Int32Condition::GetFieldName()
{
	return this->fieldName;
}

Int32 DB::QueryConditions::Int32Condition::GetVal()
{
	return this->val;
}

DB::QueryConditions::CompareCondition DB::QueryConditions::Int32Condition::GetCompareCond()
{
	return this->cond;
}

DB::QueryConditions::Int32InCondition::Int32InCondition(const UTF8Char *fieldName, Data::ArrayList<Int32> *val) : FieldCondition(fieldName)
{
	NEW_CLASS(this->vals, Data::ArrayList<Int32>());
	this->vals->AddAll(val);
}

DB::QueryConditions::Int32InCondition::~Int32InCondition()
{
	DEL_CLASS(this->vals);
}

DB::QueryConditions::ConditionType DB::QueryConditions::Int32InCondition::GetType()
{
	return ConditionType::Int32In;
}

Bool DB::QueryConditions::Int32InCondition::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	if (this->vals->GetCount() > maxDBItem)
	{
		return false;
	}
	else
	{
		UTF8Char sbuff[512];
		DB::DBUtil::SDBColUTF8(sbuff, this->fieldName, svrType);
		sb->Append(sbuff);
		sb->Append((const UTF8Char*)" in (");
		Text::StringTool::Int32Join(sb, this->vals, (const UTF8Char*)", ");
		sb->Append((const UTF8Char*)")");
		return true;
	}
}

Bool DB::QueryConditions::Int32InCondition::TestValid(Data::VariItem *item)
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

DB::QueryConditions::DoubleCondition::DoubleCondition(const UTF8Char *fieldName, Double val, CompareCondition cond) : FieldCondition(fieldName)
{
	this->val = val;
	this->cond = cond;
}

DB::QueryConditions::DoubleCondition::~DoubleCondition()
{
}

DB::QueryConditions::ConditionType DB::QueryConditions::DoubleCondition::GetType()
{
	return ConditionType::DOUBLE;
}

Bool DB::QueryConditions::DoubleCondition::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	DBUtil::SDBColUTF8(sbuff, this->fieldName, svrType);
	sb->Append(sbuff);
	sb->Append(DB::QueryConditions::CompareConditionGetStr(cond));
	Text::SBAppendF64(sb, this->val);
	return true;
}

Bool DB::QueryConditions::DoubleCondition::TestValid(Data::VariItem *item)
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

DB::QueryConditions::StringInCondition::StringInCondition(const UTF8Char *fieldName, Data::ArrayList<const UTF8Char*> *val) : FieldCondition(fieldName)
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

DB::QueryConditions::StringInCondition::~StringInCondition()
{
	LIST_FREE_FUNC(this->vals, Text::StrDelNew);
	DEL_CLASS(this->vals);
}

DB::QueryConditions::ConditionType DB::QueryConditions::StringInCondition::GetType()
{
	return ConditionType::StringIn;
}

Bool DB::QueryConditions::StringInCondition::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	if (this->vals->GetCount() > maxDBItem || this->vals->GetCount() == 0)
	{
		return false;
	}
	else
	{
		UTF8Char sbuff[512];
		UTF8Char *sptrTmp = 0;
		UOSInt sptrSize = 0;
		UOSInt thisSize;
		UOSInt i;
		UOSInt j;
		DB::DBUtil::SDBColUTF8(sbuff, this->fieldName, svrType);
		sb->Append(sbuff);
		sb->Append((const UTF8Char*)" in (");
		i = 0;
		j = this->vals->GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sb->Append((const UTF8Char*)", ");
			}
			thisSize = DBUtil::SDBStrUTF8Leng(this->vals->GetItem(i), svrType);
			if (thisSize < 512)
			{
				DB::DBUtil::SDBStrUTF8(sbuff, this->vals->GetItem(i), svrType);
				sb->Append(sbuff);
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
				DB::DBUtil::SDBStrUTF8(sptrTmp, this->vals->GetItem(i), svrType);
				sb->Append(sptrTmp);
			}
			i++;
		}
		if (sptrTmp)
		{
			MemFree(sptrTmp);
		}
		sb->AppendChar(')', 1);
		return true;
	}
}

Bool DB::QueryConditions::StringInCondition::TestValid(Data::VariItem *item)
{
	if (item == 0) return false;
	const UTF8Char *csptr;
	UOSInt i;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Str:
		csptr = item->GetItemValue().str;
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

DB::QueryConditions::StringContainsCondition::StringContainsCondition(const UTF8Char *fieldName, const UTF8Char *val) : FieldCondition(fieldName)
{
	this->val = Text::StrCopyNew(val);
}

DB::QueryConditions::StringContainsCondition::~StringContainsCondition()
{
	Text::StrDelNew(this->val);
}

DB::QueryConditions::ConditionType DB::QueryConditions::StringContainsCondition::GetType()
{
	return ConditionType::StringContains;
}

Bool DB::QueryConditions::StringContainsCondition::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	DB::DBUtil::SDBColUTF8(sbuff, this->fieldName, svrType);
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)" like ");
	Text::StringBuilderUTF8 sb2;
	sb2.AppendChar('%', 1);
	sb2.Append(this->val);
	sb2.AppendChar('%', 1);
	UOSInt size = DB::DBUtil::SDBStrUTF8Leng(sb2.ToString(), svrType);
	if (size < 512)
	{
		DB::DBUtil::SDBStrUTF8(sbuff, sb2.ToString(), svrType);
		sb->Append(sbuff);
	}
	else
	{
		UTF8Char *sptr = MemAlloc(UTF8Char, size + 1);
		DB::DBUtil::SDBStrUTF8(sptr, sb2.ToString(), svrType);
		sb->Append(sptr);
		MemFree(sptr);
	}
	return true;
}

Bool DB::QueryConditions::StringContainsCondition::TestValid(Data::VariItem *item)
{
	if (item == 0) return false;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Str:
		return Text::StrIndexOf(item->GetItemValue().str, this->val) != INVALID_INDEX;
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

DB::QueryConditions::StringEqualsCondition::StringEqualsCondition(const UTF8Char *fieldName, const UTF8Char *val) : FieldCondition(fieldName)
{
	this->val = Text::StrCopyNew(val);
}

DB::QueryConditions::StringEqualsCondition::~StringEqualsCondition()
{
	Text::StrDelNew(this->val);
}

DB::QueryConditions::ConditionType DB::QueryConditions::StringEqualsCondition::GetType()
{
	return ConditionType::StringEquals;
}

Bool DB::QueryConditions::StringEqualsCondition::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	DB::DBUtil::SDBColUTF8(sbuff, this->fieldName, svrType);
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)" = ");
	UOSInt size = DB::DBUtil::SDBStrUTF8Leng(this->val, svrType);
	if (size < 512)
	{
		DB::DBUtil::SDBStrUTF8(sbuff, this->val, svrType);
		sb->Append(sbuff);
	}
	else
	{
		UTF8Char *sptr = MemAlloc(UTF8Char, size + 1);
		DB::DBUtil::SDBStrUTF8(sptr, this->val, svrType);
		sb->Append(sptr);
		MemFree(sptr);
	}
	return true;
}

Bool DB::QueryConditions::StringEqualsCondition::TestValid(Data::VariItem *item)
{
	if (item == 0) return false;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Str:
		return Text::StrEquals(item->GetItemValue().str, this->val);
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

DB::QueryConditions::BooleanCondition::BooleanCondition(const UTF8Char *fieldName, Bool val) : FieldCondition(fieldName)
{
	this->val = val;
}

DB::QueryConditions::BooleanCondition::~BooleanCondition()
{
}

DB::QueryConditions::ConditionType DB::QueryConditions::BooleanCondition::GetType()
{
	return ConditionType::Boolean;
}

Bool DB::QueryConditions::BooleanCondition::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	if (!this->val)
	{
		sb->Append((const UTF8Char*)"NOT ");
	}
	UTF8Char sbuff[512];
	DB::DBUtil::SDBColUTF8(sbuff, this->fieldName, svrType);
	sb->Append(sbuff);
	return true;
}

Bool DB::QueryConditions::BooleanCondition::TestValid(Data::VariItem *item)
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

DB::QueryConditions::NotNullCondition::NotNullCondition(const UTF8Char *fieldName) : FieldCondition(fieldName)
{
}

DB::QueryConditions::NotNullCondition::~NotNullCondition()
{
}

DB::QueryConditions::ConditionType DB::QueryConditions::NotNullCondition::GetType()
{
	return ConditionType::NotNull;
}

Bool DB::QueryConditions::NotNullCondition::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	UTF8Char sbuff[512];
	DB::DBUtil::SDBColUTF8(sbuff, this->fieldName, svrType);
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)" is not null");
	return true;
}

Bool DB::QueryConditions::NotNullCondition::TestValid(Data::VariItem *item)
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

DB::QueryConditions::InnerCondition::InnerCondition(QueryConditions *innerCond)
{
	this->innerCond = innerCond;
}

DB::QueryConditions::InnerCondition::~InnerCondition()
{
	DEL_CLASS(this->innerCond);
}

DB::QueryConditions::ConditionType DB::QueryConditions::InnerCondition::GetType()
{
	return ConditionType::Inner;
}

Bool DB::QueryConditions::InnerCondition::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
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
		sb->AppendChar('(', 1);
		sb->Append(sbTmp.ToString());
		sb->AppendChar(')', 1);
		return true;
	}
}

Bool DB::QueryConditions::InnerCondition::IsValid(Data::VariObject *obj)
{
	return this->innerCond->IsValid(obj);
}

Bool DB::QueryConditions::InnerCondition::IsValid(Data::ObjectGetter *getter)
{
	return this->innerCond->IsValid(getter);
}

void DB::QueryConditions::InnerCondition::GetFieldList(Data::ArrayList<const UTF8Char*> *fieldList)
{
	this->innerCond->GetFieldList(fieldList);	
}

DB::QueryConditions *DB::QueryConditions::InnerCondition::GetConditions()
{
	return this->innerCond;
}

DB::QueryConditions::OrCondition::OrCondition()
{
}

DB::QueryConditions::OrCondition::~OrCondition()
{
}

DB::QueryConditions::ConditionType DB::QueryConditions::OrCondition::GetType()
{
	return ConditionType::Or;
}

Bool DB::QueryConditions::OrCondition::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem)
{
	sb->Append((const UTF8Char*)" or ");
	return true;
}

Bool DB::QueryConditions::OrCondition::IsValid(Data::VariObject *obj)
{
	return true;
}

Bool DB::QueryConditions::OrCondition::IsValid(Data::ObjectGetter *getter)
{
	return true;
}

void DB::QueryConditions::OrCondition::GetFieldList(Data::ArrayList<const UTF8Char*> *fieldList)
{
}

DB::QueryConditions::QueryConditions()
{
	NEW_CLASS(this->conditionList, Data::ArrayList<Condition*>());
}

DB::QueryConditions::~QueryConditions()
{
	LIST_FREE_FUNC(this->conditionList, DEL_CLASS);
	DEL_CLASS(this->conditionList);
}

Bool DB::QueryConditions::IsValid(Data::VariObject *obj)
{
	return ObjectValid(obj, this->conditionList);
}

Bool DB::QueryConditions::IsValid(Data::ObjectGetter *getter)
{
	return ObjectValid(getter, this->conditionList);
}

Bool DB::QueryConditions::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, Int8 tzQhr, UOSInt maxDBItem, Data::ArrayList<Condition*> *clientConditions)
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
					sb->AppendChar(')', 1);
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
						sb->Append((const UTF8Char*)" or (");
						splitType = 0;
					}
					else if (splitType != 0)
					{
						sb->AppendChar('(', 1);
						splitType = 0;
					}
					else
					{
						sb->Append((const UTF8Char*)" and ");
					}
					sb->Append(sbTmp.ToString());
				}
			}
			i++;
		}
		if (splitType == 0)
		{
			sb->AppendChar(')', 1);
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
					sb->Append((const UTF8Char*)" and ");
				}
				sb->Append(sbTmp.ToString());
				hasCond = true;
			}
			i++;
		}
	}
	return true;
}

UOSInt DB::QueryConditions::GetCount()
{
	return this->conditionList->GetCount();
}

DB::QueryConditions::Condition *DB::QueryConditions::GetItem(UOSInt index)
{
	return this->conditionList->GetItem(index);
}

Data::ArrayList<DB::QueryConditions::Condition*> *DB::QueryConditions::GetList()
{
	return this->conditionList;
}

void DB::QueryConditions::GetFieldList(Data::ArrayList<const UTF8Char*> *fieldList)
{
	UOSInt i = 0;
	UOSInt j = this->conditionList->GetCount();
	while (i < j)
	{
		this->conditionList->GetItem(i)->GetFieldList(fieldList);
		i++;
	}
}

DB::QueryConditions *DB::QueryConditions::TimeBetween(const UTF8Char *fieldName, Int64 t1, Int64 t2)
{
	this->conditionList->Add(NEW_CLASS_D(TimeBetweenCondition(fieldName, t1, t2)));
	return this;
}

DB::QueryConditions *DB::QueryConditions::Or()
{
	this->conditionList->Add(NEW_CLASS_D(OrCondition()));
	return this;
}

DB::QueryConditions *DB::QueryConditions::InnerCond(QueryConditions *cond)
{
	this->conditionList->Add(NEW_CLASS_D(InnerCondition(cond)));
	return this;
}

DB::QueryConditions *DB::QueryConditions::Int32Equals(const UTF8Char *fieldName, Int32 val)
{
	this->conditionList->Add(NEW_CLASS_D(Int32Condition(fieldName, val, CompareCondition::Equal)));
	return this;
}

DB::QueryConditions *DB::QueryConditions::Int32In(const UTF8Char *fieldName, Data::ArrayList<Int32> *vals)
{
	this->conditionList->Add(NEW_CLASS_D(Int32InCondition(fieldName, vals)));
	return this;
}

DB::QueryConditions *DB::QueryConditions::DoubleGE(const UTF8Char *fieldName, Double val)
{
	this->conditionList->Add(NEW_CLASS_D(DoubleCondition(fieldName, val, CompareCondition::GreaterOrEqual)));
	return this;
}

DB::QueryConditions *DB::QueryConditions::DoubleLE(const UTF8Char *fieldName, Double val)
{
	this->conditionList->Add(NEW_CLASS_D(DoubleCondition(fieldName, val, CompareCondition::GreaterOrEqual)));
	return this;
}

DB::QueryConditions *DB::QueryConditions::StrIn(const UTF8Char *fieldName, Data::ArrayList<const UTF8Char*> *vals)
{
	this->conditionList->Add(NEW_CLASS_D(StringInCondition(fieldName, vals)));
	return this;
}

DB::QueryConditions *DB::QueryConditions::StrContains(const UTF8Char *fieldName, const UTF8Char *val)
{
	this->conditionList->Add(NEW_CLASS_D(StringContainsCondition(fieldName, val)));
	return this;
}

DB::QueryConditions *DB::QueryConditions::StrEquals(const UTF8Char *fieldName, const UTF8Char *val)
{
	this->conditionList->Add(NEW_CLASS_D(StringEqualsCondition(fieldName, val)));
	return this;
}

DB::QueryConditions *DB::QueryConditions::BoolEquals(const UTF8Char *fieldName, Bool val)
{
	this->conditionList->Add(NEW_CLASS_D(BooleanCondition(fieldName, val)));
	return this;
}

DB::QueryConditions *DB::QueryConditions::NotNull(const UTF8Char* fieldName)
{
	this->conditionList->Add(NEW_CLASS_D(NotNullCondition(fieldName)));
	return this;
}

const UTF8Char *DB::QueryConditions::CompareConditionGetStr(CompareCondition cond)
{
	switch (cond)
	{
	case CompareCondition::Equal:
		return (const UTF8Char*)" = ";
	case CompareCondition::Greater:
		return (const UTF8Char*)" > ";
	case CompareCondition::Less:
		return (const UTF8Char*)" < ";
	case CompareCondition::GreaterOrEqual:
		return (const UTF8Char*)" >= ";
	case CompareCondition::LessOrEqual:
		return (const UTF8Char*)" <= ";
	case CompareCondition::NotEqual:
		return (const UTF8Char*)" <> ";
	}
	return (const UTF8Char*)"";
}

Bool DB::QueryConditions::ObjectValid(Data::VariObject *obj, Data::ArrayList<Condition*> *conditionList)
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

Bool DB::QueryConditions::ObjectValid(Data::ObjectGetter *getter, Data::ArrayList<Condition*> *conditionList)
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
