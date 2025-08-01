#include "Stdafx.h"
#include "Data/QueryConditions.h"
#include "DB/SQL/SQLUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

Data::Conditions::BooleanAnd::BooleanAnd()
{
}

Data::Conditions::BooleanAnd::~BooleanAnd()
{
	this->andList.DeleteAll();
}

NN<Data::Conditions::ConditionObject> Data::Conditions::BooleanAnd::Clone() const
{
	NN<BooleanAnd> cond;
	NEW_CLASSNN(cond, BooleanAnd());
	UOSInt i = 0;
	UOSInt j = this->andList.GetCount();
	while (i < j)
	{
		cond->andList.Add(NN<BooleanObject>::ConvertFrom(this->andList.GetItemNoCheck(i)->Clone()));
		i++;
	}
	return cond;
}

Bool Data::Conditions::BooleanAnd::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	UOSInt i = 0;
	UOSInt j = this->andList.GetCount();
	if (j == 0)
		return false;
	while (i < j)
	{
		if (!this->andList.GetItemNoCheck(i)->CanWhereClause(maxDBItem))
			return false;
		i++;
	}
	NN<BooleanObject> cond;
	i = 0;
	while (i < j)
	{
		if (i > 0)
			sb->Append(CSTR(" and "));
		cond = this->andList.GetItemNoCheck(i);
		if (cond->GetType() == ObjectType::BooleanOr && NN<BooleanOr>::ConvertFrom(cond)->GetCount() > 1)
		{
			sb->AppendUTF8Char('(');
			if (!cond->ToWhereClause(sb, sqlType, tzQhr, maxDBItem))
				return false;
			sb->AppendUTF8Char(')');
		}
		else
		{
			if (!cond->ToWhereClause(sb, sqlType, tzQhr, maxDBItem))
				return false;
		}
		i++;
	}
	return true;
}

Bool Data::Conditions::BooleanAnd::ToWhereClauseOrClient(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem, NN<Data::ArrayListNN<BooleanObject>> clientConditions) const
{
	UOSInt i = 0;
	UOSInt j = this->andList.GetCount();
	if (j == 0)
		return false;
	NN<BooleanObject> cond;
	Text::StringBuilderUTF8 sbTmp;
	Bool found = false;
	i = 0;
	while (i < j)
	{
		cond = this->andList.GetItemNoCheck(i);
		sbTmp.ClearStr();
		if (!cond->ToWhereClause(sb, sqlType, tzQhr, maxDBItem))
		{
			clientConditions->Add(cond);
		}
		else
		{
			if (found)
				sb->Append(CSTR(" and "));
			else
				found = true;
			if (cond->GetType() == ObjectType::BooleanOr && NN<BooleanOr>::ConvertFrom(cond)->GetCount() > 1)
			{
				sb->AppendUTF8Char('(');
				sb->Append(sbTmp);
				sb->AppendUTF8Char(')');
			}
			else
			{
				sb->Append(sbTmp);
			}
		}
		i++;
	}
	return true;
}

Bool Data::Conditions::BooleanAnd::CanWhereClause(UOSInt maxDBItem) const
{
	UOSInt i = 0;
	UOSInt j = this->andList.GetCount();
	if (j == 0)
		return false;
	while (i < j)
	{
		if (!this->andList.GetItemNoCheck(i)->CanWhereClause(maxDBItem))
			return false;
		i++;
	}
	return true;
}

void Data::Conditions::BooleanAnd::GetFieldList(NN<Data::ArrayListStringNN> fieldList) const
{
	UOSInt i = 0;
	UOSInt j = this->andList.GetCount();
	while (i < j)
	{
		this->andList.GetItemNoCheck(i)->GetFieldList(fieldList);
		i++;
	}
}

Bool Data::Conditions::BooleanAnd::Eval(NN<Data::VariObject> obj, OutParam<Bool> outVal) const
{
	UOSInt i = 0;
	UOSInt j = this->andList.GetCount();
	if (j == 0)
		return false;
	Bool v;
	while (i < j)
	{
		if (!this->andList.GetItemNoCheck(i)->Eval(obj, v))
			return false;
		if (!v)
		{
			outVal.Set(false);
			return true;
		}
		i++;
	}
	outVal.Set(true);
	return true;
}

Bool Data::Conditions::BooleanAnd::Eval(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal) const
{
	UOSInt i = 0;
	UOSInt j = this->andList.GetCount();
	if (j == 0)
		return false;
	Bool v;
	while (i < j)
	{
		if (!this->andList.GetItemNoCheck(i)->Eval(getter, v))
			return false;
		if (!v)
		{
			outVal.Set(false);
			return true;
		}
		i++;
	}
	outVal.Set(true);
	return true;
}

UOSInt Data::Conditions::BooleanAnd::GetCount() const
{
	return this->andList.GetCount();
}

void Data::Conditions::BooleanAnd::AddAnd(NN<BooleanObject> obj)
{
	this->andList.Add(obj);
}

Data::Conditions::BooleanOr::BooleanOr()
{
}

Data::Conditions::BooleanOr::~BooleanOr()
{
	this->orList.DeleteAll();
}

NN<Data::Conditions::ConditionObject> Data::Conditions::BooleanOr::Clone() const
{
	NN<BooleanOr> cond;
	NEW_CLASSNN(cond, BooleanOr());
	UOSInt i = 0;
	UOSInt j = this->orList.GetCount();
	while (i < j)
	{
		cond->orList.Add(NN<BooleanObject>::ConvertFrom(this->orList.GetItemNoCheck(i)->Clone()));
		i++;
	}
	return cond;
}

Bool Data::Conditions::BooleanOr::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	NN<BooleanObject> obj;
	UOSInt i = 0;
	UOSInt j = this->orList.GetCount();
	if (j == 0)
		return false;
	while (i < j)
	{
		obj = this->orList.GetItemNoCheck(i);
		if (i > 0)
		{
			sb->Append(CSTR(" or "));
		}
		if (obj->GetType() == ObjectType::BooleanAnd && NN<BooleanAnd>::ConvertFrom(obj)->GetCount() > 1)
		{
			sb->AppendUTF8Char('(');
			if (!obj->ToWhereClause(sb, sqlType, tzQhr, maxDBItem))
				return false;
			sb->AppendUTF8Char(')');
		}
		else
		{
			if (!obj->ToWhereClause(sb, sqlType, tzQhr, maxDBItem))
				return false;
		}
		i++;
	}
	return true;
}

Bool Data::Conditions::BooleanOr::CanWhereClause(UOSInt maxDBItem) const
{
	UOSInt i = this->orList.GetCount();
	if (i == 0)
		return false;
	while (i-- > 0)
	{
		if (!this->orList.GetItemNoCheck(i)->CanWhereClause(maxDBItem))
			return false;
	}
	return true;
}

void Data::Conditions::BooleanOr::GetFieldList(NN<Data::ArrayListStringNN> fieldList) const
{
	UOSInt i = this->orList.GetCount();
	while (i-- > 0)
	{
		this->orList.GetItemNoCheck(i)->GetFieldList(fieldList);
	}
}

Bool Data::Conditions::BooleanOr::Eval(NN<Data::VariObject> obj, OutParam<Bool> outVal) const
{
	Bool v;
	UOSInt i = 0;
	UOSInt j = this->orList.GetCount();
	if (j == 0)
		return false;
	while (i < j)
	{
		if (!this->orList.GetItemNoCheck(i)->Eval(obj, v))
			return false;
		if (v)
		{
			outVal.Set(true);
			return true;
		}
		i++;
	}
	outVal.Set(false);
	return true;
}

Bool Data::Conditions::BooleanOr::Eval(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal) const
{
	Bool v;
	UOSInt i = 0;
	UOSInt j = this->orList.GetCount();
	if (j == 0)
		return false;
	while (i < j)
	{
		if (!this->orList.GetItemNoCheck(i)->Eval(getter, v))
			return false;
		if (v)
		{
			outVal.Set(true);
			return true;
		}
		i++;
	}
	outVal.Set(false);
	return true;
}

UOSInt Data::Conditions::BooleanOr::GetCount() const
{
	return this->orList.GetCount();
}

void Data::Conditions::BooleanOr::AddOr(NN<BooleanObject> obj)
{
	this->orList.Add(obj);
}

Data::Conditions::NumberField::NumberField(Text::CStringNN fieldName)
{
	this->fieldName = Text::String::New(fieldName);
}

Data::Conditions::NumberField::NumberField(NN<Text::String> fieldName)
{
	this->fieldName = fieldName->Clone();
}

Data::Conditions::NumberField::~NumberField()
{
	this->fieldName->Release();
}

Bool Data::Conditions::NumberField::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendP(sbuff, sptr);
	return true;
}

Data::Conditions::NumberType Data::Conditions::NumberField::GetNumberType(NN<Data::VariObject> obj) const
{
	NN<Data::VariItem> item;
	if (obj->GetItem(this->fieldName->v).SetTo(item))
	{
		return ToNumberType(item);
	}
	else
	{
		return NumberType::Null;
	}
}

Data::Conditions::NumberType Data::Conditions::NumberField::GetNumberType(NN<Data::ObjectGetter> getter) const
{
	NN<Data::VariItem> item = getter->GetNewItem(this->fieldName->ToCString());
	NumberType t = ToNumberType(item);
	item.Delete();
	return t;
}

Bool Data::Conditions::NumberField::EvalInt(NN<Data::VariObject> obj, OutParam<Int64> outVal) const
{
	NN<Data::VariItem> item;
	if (obj->GetItem(this->fieldName->v).SetTo(item))
	{
		outVal.Set(item->GetAsI64());
		return true;
	}
	else
	{
		outVal.Set(0);
		return false;
	}
}

Bool Data::Conditions::NumberField::EvalInt(NN<Data::ObjectGetter> getter, OutParam<Int64> outVal) const
{
	NN<Data::VariItem> item = getter->GetNewItem(this->fieldName->ToCString());
	if (item->GetItemType() == Data::VariItem::ItemType::Null)
	{
		item.Delete();
		outVal.Set(0);
		return false;
	}
	outVal.Set(item->GetAsI64());
	item.Delete();
	return true;
}

Bool Data::Conditions::NumberField::EvalDouble(NN<Data::VariObject> obj, OutParam<Double> outVal) const
{
	NN<Data::VariItem> item;
	if (obj->GetItem(this->fieldName->v).SetTo(item))
	{
		outVal.Set(item->GetAsF64());
		return true;
	}
	else
	{
		outVal.Set(0);
		return false;
	}
}

Bool Data::Conditions::NumberField::EvalDouble(NN<Data::ObjectGetter> getter, OutParam<Double> outVal) const
{
	NN<Data::VariItem> item = getter->GetNewItem(this->fieldName->ToCString());
	if (item->GetItemType() == Data::VariItem::ItemType::Null)
	{
		item.Delete();
		outVal.Set(0);
		return false;
	}
	outVal.Set(item->GetAsF64());
	item.Delete();
	return true;
}

Data::Conditions::NumberType Data::Conditions::NumberField::ToNumberType(NN<Data::VariItem> item)
{
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
		return NumberType::F64;
	case Data::VariItem::ItemType::I8:
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::I16:
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::NI32:
		return NumberType::I32;
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::U64:
		return NumberType::I64;
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Timestamp:
	default:
		return NumberType::Null;
	}
}

Data::Conditions::TimeField::TimeField(Text::CStringNN fieldName)
{
	this->fieldName = Text::String::New(fieldName);
}

Data::Conditions::TimeField::TimeField(NN<Text::String> fieldName)
{
	this->fieldName = fieldName->Clone();
}

Data::Conditions::TimeField::~TimeField()
{
	this->fieldName->Release();
}

Bool Data::Conditions::TimeField::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendP(sbuff, sptr);
	return true;
}

Data::Timestamp Data::Conditions::TimeField::Eval(NN<Data::VariObject> obj) const
{
	NN<Data::VariItem> item;
	if (obj->GetItem(this->fieldName->v).SetTo(item))
	{
		return item->GetAsTimestamp();
	}
	else
	{
		return 0;
	}
}

Data::Timestamp Data::Conditions::TimeField::Eval(NN<Data::ObjectGetter> getter) const
{
	NN<Data::VariItem> item = getter->GetNewItem(this->fieldName->ToCString());
	if (item->GetItemType() == Data::VariItem::ItemType::Null)
	{
		item.Delete();
		return 0;
	}
	Data::Timestamp ts = item->GetAsTimestamp();
	item.Delete();
	return ts;

}

Bool Data::Conditions::NumberCondition::IntCompare(Int64 left, Int64 right, CompareCondition cond)
{
	switch (cond)
	{
	case CompareCondition::Equal:
		return left == right;
	case CompareCondition::NotEqual:
		return left != right;
	case CompareCondition::Greater:
		return left > right;
	case CompareCondition::GreaterOrEqual:
		return left >= right;
	case CompareCondition::Less:
		return left < right;
	case CompareCondition::LessOrEqual:
		return left <= right;
	case CompareCondition::Unknown:
	default:
		return false;
	}
}

Bool Data::Conditions::NumberCondition::FloatCompare(Double left, Double right, CompareCondition cond)
{
	switch (cond)
	{
	case CompareCondition::Equal:
		return left == right;
	case CompareCondition::NotEqual:
		return left != right;
	case CompareCondition::Greater:
		return left > right;
	case CompareCondition::GreaterOrEqual:
		return left >= right;
	case CompareCondition::Less:
		return left < right;
	case CompareCondition::LessOrEqual:
		return left <= right;
	case CompareCondition::Unknown:
	default:
		return false;
	}
}

Bool Data::Conditions::NumberCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	if (!this->left->ToWhereClause(sb, sqlType, tzQhr, maxDBItem))
		return false;
	switch (this->cond)
	{
	case CompareCondition::Equal:
		sb->Append(CSTR(" = "));
		break;
	case CompareCondition::NotEqual:
		sb->Append(CSTR(" <> "));
		break;
	case CompareCondition::Greater:
		sb->Append(CSTR(" > "));
		break;
	case CompareCondition::GreaterOrEqual:
		sb->Append(CSTR(" >= "));
		break;
	case CompareCondition::Less:
		sb->Append(CSTR(" < "));
		break;
	case CompareCondition::LessOrEqual:
		sb->Append(CSTR(" <= "));
		break;
	case CompareCondition::Unknown:
	default:
		return false;
	}
	if (!this->right->ToWhereClause(sb, sqlType, tzQhr, maxDBItem))
		return false;
	return true;
}

void Data::Conditions::NumberCondition::GetFieldList(NN<Data::ArrayListStringNN> fieldList) const
{
	this->left->GetFieldList(fieldList);
	this->right->GetFieldList(fieldList);
}

Bool Data::Conditions::NumberCondition::Eval(NN<Data::VariObject> obj, OutParam<Bool> outVal) const
{
	if (this->left->GetNumberType(obj) == NumberType::F64 || this->right->GetNumberType(obj) == NumberType::F64)
	{
		Double leftVal;
		Double rightVal;
		if (!this->left->EvalDouble(obj, leftVal) || !this->right->EvalDouble(obj, rightVal))
			return false;
		outVal.Set(FloatCompare(leftVal, rightVal, this->cond));
		return true;
	}
	else
	{
		Int64 leftVal;
		Int64 rightVal;
		if (!this->left->EvalInt(obj, leftVal) || !this->right->EvalInt(obj, rightVal))
			return false;
		outVal.Set(IntCompare(leftVal, rightVal, this->cond));
		return true;
	}
}

Bool Data::Conditions::NumberCondition::Eval(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal) const
{
	if (this->left->GetNumberType(getter) == NumberType::F64 || this->right->GetNumberType(getter) == NumberType::F64)
	{
		Double leftVal;
		Double rightVal;
		if (!this->left->EvalDouble(getter, leftVal) || !this->right->EvalDouble(getter, rightVal))
			return false;
		outVal.Set(FloatCompare(leftVal, rightVal, this->cond));
		return true;
	}
	else
	{
		Int64 leftVal;
		Int64 rightVal;
		if (!this->left->EvalInt(getter, leftVal) || !this->right->EvalInt(getter, rightVal))
			return false;
		outVal.Set(IntCompare(leftVal, rightVal, this->cond));
		return true;
	}
}

Bool Data::Conditions::TimeCondition::TSCompare(Data::Timestamp left, Data::Timestamp right, CompareCondition cond)
{
	switch (cond)
	{
	case CompareCondition::Equal:
		return left == right;
	case CompareCondition::NotEqual:
		return left != right;
	case CompareCondition::Greater:
		return left > right;
	case CompareCondition::GreaterOrEqual:
		return left >= right;
	case CompareCondition::Less:
		return left < right;
	case CompareCondition::LessOrEqual:
		return left <= right;
	case CompareCondition::Unknown:
	default:
		return false;
	}
}

Bool Data::Conditions::TimeCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	if (!this->left->ToWhereClause(sb, sqlType, tzQhr, maxDBItem))
		return false;
	switch (this->cond)
	{
	case CompareCondition::Equal:
		sb->Append(CSTR(" = "));
		break;
	case CompareCondition::NotEqual:
		sb->Append(CSTR(" <> "));
		break;
	case CompareCondition::Greater:
		sb->Append(CSTR(" > "));
		break;
	case CompareCondition::GreaterOrEqual:
		sb->Append(CSTR(" >= "));
		break;
	case CompareCondition::Less:
		sb->Append(CSTR(" < "));
		break;
	case CompareCondition::LessOrEqual:
		sb->Append(CSTR(" <= "));
		break;
	case CompareCondition::Unknown:
	default:
		return false;
	}
	if (!this->right->ToWhereClause(sb, sqlType, tzQhr, maxDBItem))
		return false;
	return true;
}

void Data::Conditions::TimeCondition::GetFieldList(NN<Data::ArrayListStringNN> fieldList) const
{
	this->left->GetFieldList(fieldList);
	this->right->GetFieldList(fieldList);
}

Bool Data::Conditions::TimeCondition::Eval(NN<Data::VariObject> obj, OutParam<Bool> outVal) const
{
	Data::Timestamp leftVal = this->left->Eval(obj);
	Data::Timestamp rightVal = this->right->Eval(obj);
	if (leftVal.IsNull() || rightVal.IsNull())
		return false;
	outVal.Set(TSCompare(leftVal, rightVal, this->cond));
	return true;
}

Bool Data::Conditions::TimeCondition::Eval(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal) const
{
	Data::Timestamp leftVal = this->left->Eval(getter);
	Data::Timestamp rightVal = this->right->Eval(getter);
	if (leftVal.IsNull() || rightVal.IsNull())
		return false;
	outVal.Set(TSCompare(leftVal, rightVal, this->cond));
	return true;
}

Data::Conditions::FieldCondition::FieldCondition(Text::CStringNN fieldName)
{
	this->fieldName = Text::String::New(fieldName);
}

Data::Conditions::FieldCondition::FieldCondition(NN<Text::String> fieldName)
{
	this->fieldName = fieldName->Clone();
}

Data::Conditions::FieldCondition::~FieldCondition()
{
	this->fieldName->Release();
}

Bool Data::Conditions::FieldCondition::Eval(NN<Data::VariObject> obj, OutParam<Bool> outVal) const
{
	NN<Data::VariItem> item;
	if (!obj->GetItem(this->fieldName->v).SetTo(item))
		return false;
	outVal.Set(this->TestValid(item));
	return true;
}

Bool Data::Conditions::FieldCondition::Eval(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal) const
{
	NN<Data::VariItem> item = getter->GetNewItem(this->fieldName->ToCString());
	Bool ret = this->TestValid(item);
	item.Delete();
	outVal.Set(ret);
	return true;
}

void Data::Conditions::FieldCondition::GetFieldList(NN<Data::ArrayListStringNN> fieldList) const
{
	fieldList->Add(this->fieldName);
}

Data::Conditions::TimeBetweenCondition::TimeBetweenCondition(Text::CStringNN fieldName, const Data::Timestamp &t1, const Data::Timestamp &t2) : FieldCondition(fieldName)
{
	this->t1 = t1;
	this->t2 = t2;
}

Data::Conditions::TimeBetweenCondition::~TimeBetweenCondition()
{
}

Data::Conditions::ObjectType Data::Conditions::TimeBetweenCondition::GetType() const
{
	return ObjectType::TimeBetween;
}

Bool Data::Conditions::TimeBetweenCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
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

Bool Data::Conditions::TimeBetweenCondition::CanWhereClause(UOSInt maxDBItem) const
{
	return true;
}

Bool Data::Conditions::TimeBetweenCondition::TestValid(NN<Data::VariItem> item) const
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

Data::Conditions::Int32InCondition::Int32InCondition(Text::CStringNN fieldName, NN<const Data::ArrayList<Int32>> val) : FieldCondition(fieldName)
{
	this->vals.AddAll(val);
}

Data::Conditions::Int32InCondition::~Int32InCondition()
{
}

Data::Conditions::ObjectType Data::Conditions::Int32InCondition::GetType() const
{
	return ObjectType::Int32In;
}

Bool Data::Conditions::Int32InCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	if (this->vals.GetCount() > maxDBItem)
	{
		return false;
	}
	else
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendC(UTF8STRC(" in ("));
		Text::StringTool::Int32Join(sb, this->vals, CSTR(", "));
		sb->AppendC(UTF8STRC(")"));
		return true;
	}
}

Bool Data::Conditions::Int32InCondition::CanWhereClause(UOSInt maxDBItem) const
{
	return this->vals.GetCount() <= maxDBItem;
}

Bool Data::Conditions::Int32InCondition::TestValid(NN<Data::VariItem> item) const
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

Data::Conditions::StringInCondition::StringInCondition(Text::CStringNN fieldName, NN<const Data::ArrayListArr<const UTF8Char>> val) : FieldCondition(fieldName)
{
	UOSInt i = 0;
	UOSInt j = val->GetCount();
	while (i < j)
	{
		this->vals.Add(Text::StrCopyNew(val->GetItemNoCheck(i)));
		i++;
	}
}

Data::Conditions::StringInCondition::~StringInCondition()
{
	UOSInt i = 0;
	UOSInt j = this->vals.GetCount();
	while (i < j)
	{
		Text::StrDelNew(this->vals.GetItemNoCheck(i));
		i++;
	}
}

Data::Conditions::ObjectType Data::Conditions::StringInCondition::GetType() const
{
	return ObjectType::StringIn;
}

Bool Data::Conditions::StringInCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	if (this->vals.GetCount() > maxDBItem || this->vals.GetCount() == 0)
	{
		return false;
	}
	else
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UnsafeArrayOpt<UTF8Char> sptrTmp = 0;
		UnsafeArray<UTF8Char> nnsptrTmp;
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
					if (sptrTmp.SetTo(nnsptrTmp))
					{
						MemFreeArr(nnsptrTmp);
					}
					nnsptrTmp = MemAllocArr(UTF8Char, thisSize + 1);
					sptrTmp = nnsptrTmp;
				}
				if (sptrTmp.SetTo(nnsptrTmp))
				{
					sptr = DB::DBUtil::SDBStrUTF8(nnsptrTmp, this->vals.GetItem(i), sqlType);
					sb->AppendC(nnsptrTmp, (UOSInt)(sptr - nnsptrTmp));
				}
			}
			i++;
		}
		if (sptrTmp.SetTo(nnsptrTmp))
		{
			MemFreeArr(nnsptrTmp);
		}
		sb->AppendUTF8Char(')');
		return true;
	}
}

Bool Data::Conditions::StringInCondition::CanWhereClause(UOSInt maxDBItem) const
{
	return this->vals.GetCount() > 0 && this->vals.GetCount() <= maxDBItem;
}

Bool Data::Conditions::StringInCondition::TestValid(NN<Data::VariItem> item) const
{
	UnsafeArray<const UTF8Char> csptr;
	UOSInt i;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Str:
		csptr = item->GetItemValue().str->v;
		i = this->vals.GetCount();
		while (i-- > 0)
		{
			if (Text::StrEquals(csptr, this->vals.GetItemNoCheck(i)))
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
			if (Text::StrEquals(csptr, this->vals.GetItemNoCheck(i)))
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

Data::Conditions::StringNotInCondition::StringNotInCondition(Text::CStringNN fieldName, NN<const Data::ArrayListArr<const UTF8Char>> val) : FieldCondition(fieldName)
{
	UOSInt i = 0;
	UOSInt j = val->GetCount();
	while (i < j)
	{
		this->vals.Add(Text::StrCopyNew(val->GetItemNoCheck(i)));
		i++;
	}
}

Data::Conditions::StringNotInCondition::~StringNotInCondition()
{
	UOSInt i = 0;
	UOSInt j = this->vals.GetCount();
	while (i < j)
	{
		Text::StrDelNew(this->vals.GetItemNoCheck(i));
		i++;
	}
}

Data::Conditions::ObjectType Data::Conditions::StringNotInCondition::GetType() const
{
	return ObjectType::StringNotIn;
}

Bool Data::Conditions::StringNotInCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	if (this->vals.GetCount() > maxDBItem || this->vals.GetCount() == 0)
	{
		return false;
	}
	else
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UnsafeArrayOpt<UTF8Char> sptrTmp = 0;
		UnsafeArray<UTF8Char> nnsptrTmp;
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
					if (sptrTmp.SetTo(nnsptrTmp))
					{
						MemFreeArr(nnsptrTmp);
					}
					nnsptrTmp = MemAllocArr(UTF8Char, thisSize + 1);
					sptrTmp = nnsptrTmp;
				}
				if (sptrTmp.SetTo(nnsptrTmp))
				{
					sptr = DB::DBUtil::SDBStrUTF8(nnsptrTmp, this->vals.GetItem(i), sqlType);
					sb->AppendC(nnsptrTmp, (UOSInt)(sptr - nnsptrTmp));
				}
			}
			i++;
		}
		if (sptrTmp.SetTo(nnsptrTmp))
		{
			MemFreeArr(nnsptrTmp);
		}
		sb->AppendUTF8Char(')');
		return true;
	}
}

Bool Data::Conditions::StringNotInCondition::CanWhereClause(UOSInt maxDBItem) const
{
	return this->vals.GetCount() > 0 && this->vals.GetCount() <= maxDBItem;
}

Bool Data::Conditions::StringNotInCondition::TestValid(NN<Data::VariItem> item) const
{
	UnsafeArray<const UTF8Char> csptr;
	UOSInt i;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::Str:
		csptr = item->GetItemValue().str->v;
		i = this->vals.GetCount();
		while (i-- > 0)
		{
			if (Text::StrEquals(csptr, this->vals.GetItemNoCheck(i)))
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
			if (Text::StrEquals(csptr, this->vals.GetItemNoCheck(i)))
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

Data::Conditions::StringContainsCondition::StringContainsCondition(Text::CStringNN fieldName, Text::CStringNN val) : FieldCondition(fieldName)
{
	this->val = Text::String::New(val);
}

Data::Conditions::StringContainsCondition::~StringContainsCondition()
{
	this->val->Release();
}

Data::Conditions::ObjectType Data::Conditions::StringContainsCondition::GetType() const
{
	return ObjectType::StringContains;
}

Bool Data::Conditions::StringContainsCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr2;
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
		UnsafeArray<UTF8Char> sptr = MemAllocArr(UTF8Char, size + 1);
		sptr2 = DB::DBUtil::SDBStrUTF8(sptr, sb2.ToString(), sqlType);
		sb->AppendC(sptr, (UOSInt)(sptr2 - sptr));
		MemFreeArr(sptr);
	}
	return true;
}

Bool Data::Conditions::StringContainsCondition::CanWhereClause(UOSInt maxDBItem) const
{
	return true;
}

Bool Data::Conditions::StringContainsCondition::TestValid(NN<Data::VariItem> item) const
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

Data::Conditions::StringEqualsCondition::StringEqualsCondition(Text::CStringNN fieldName, Text::CStringNN val) : FieldCondition(fieldName)
{
	this->val = Text::String::New(val);
}

Data::Conditions::StringEqualsCondition::~StringEqualsCondition()
{
	this->val->Release();
}

Data::Conditions::ObjectType Data::Conditions::StringEqualsCondition::GetType() const
{
	return ObjectType::StringEquals;
}

Bool Data::Conditions::StringEqualsCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr2;
	sptr2 = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr2 - sbuff));
	sb->AppendC(UTF8STRC(" = "));
	UOSInt size = DB::DBUtil::SDBStrUTF8Leng(UnsafeArray<const UTF8Char>(this->val->v), sqlType);
	if (size < 512)
	{
		sptr2 = DB::DBUtil::SDBStrUTF8(sbuff, UnsafeArray<const UTF8Char>(this->val->v), sqlType);
		sb->AppendC(sbuff, (UOSInt)(sptr2 - sbuff));
	}
	else
	{
		UnsafeArray<UTF8Char> sptr = MemAllocArr(UTF8Char, size + 1);
		sptr2 = DB::DBUtil::SDBStrUTF8(sptr, UnsafeArray<const UTF8Char>(this->val->v), sqlType);
		sb->AppendC(sptr, (UOSInt)(sptr2 - sptr));
		MemFreeArr(sptr);
	}
	return true;
}

Bool Data::Conditions::StringEqualsCondition::CanWhereClause(UOSInt maxDBItem) const
{
	return true;
}

Bool Data::Conditions::StringEqualsCondition::TestValid(NN<Data::VariItem> item) const
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

Data::Conditions::BooleanCondition::BooleanCondition(Text::CStringNN fieldName, Bool val) : FieldCondition(fieldName)
{
	this->val = val;
}

Data::Conditions::BooleanCondition::~BooleanCondition()
{
}

Data::Conditions::ObjectType Data::Conditions::BooleanCondition::GetType() const
{
	return ObjectType::Boolean;
}

Bool Data::Conditions::BooleanCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	if (!this->val)
	{
		sb->AppendC(UTF8STRC("NOT "));
	}
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

Bool Data::Conditions::BooleanCondition::CanWhereClause(UOSInt maxDBItem) const
{
	return true;
}

Bool Data::Conditions::BooleanCondition::TestValid(NN<Data::VariItem> item) const
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

Bool Data::Conditions::NullCondition::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem) const
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = DB::DBUtil::SDBColUTF8(sbuff, this->fieldName->v, sqlType);
	sb->AppendP(sbuff, sptr);
	sb->Append(CSTR(" is "));
	if (this->notNull)
		sb->Append(CSTR("not "));
	sb->Append(CSTR("null"));
	return true;
}

Bool Data::Conditions::NullCondition::CanWhereClause(UOSInt maxDBItem) const
{
	return true;
}

Bool Data::Conditions::NullCondition::Eval(NN<Data::VariObject> obj, OutParam<Bool> outVal) const
{
	NN<Data::VariItem> item;
	if (obj->GetItem(this->fieldName->v).SetTo(item))
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
			outVal.Set(this->notNull);
			return true;
		case Data::VariItem::ItemType::Null:
			outVal.Set(!this->notNull);
			return true;
		case Data::VariItem::ItemType::Unknown:
		default:
			return false;
		}
	}
	else
	{
		return false;
	}
}

Bool Data::Conditions::NullCondition::Eval(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal) const
{
	NN<Data::VariItem> item = getter->GetNewItem(this->fieldName->ToCString());
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
		outVal.Set(this->notNull);
		item.Delete();
		return true;
	case Data::VariItem::ItemType::Null:
		outVal.Set(!this->notNull);
		item.Delete();
		return true;
	case Data::VariItem::ItemType::Unknown:
	default:
		item.Delete();
		return false;
	}
}

void Data::Conditions::NullCondition::GetFieldList(NN<Data::ArrayListStringNN> fieldList) const
{
	fieldList->Add(this->fieldName);
}

Data::QueryConditions::QueryConditions()
{
	NEW_CLASSNN(this->andCond, Conditions::BooleanAnd());
	this->cond = this->andCond;
	this->orCond = 0;
}

Data::QueryConditions::~QueryConditions()
{
	this->cond.Delete();
}

Bool Data::QueryConditions::IsValid(NN<Data::VariObject> obj, OutParam<Bool> outVal)
{
	return this->cond->Eval(obj, outVal);
}

Bool Data::QueryConditions::IsValid(NN<Data::ObjectGetter> getter, OutParam<Bool> outVal)
{
	return this->cond->Eval(getter, outVal);
}

Bool Data::QueryConditions::ToWhereClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType, Int8 tzQhr, UOSInt maxDBItem, NN<Data::ArrayListNN<Conditions::BooleanObject>> clientConditions)
{
	if (this->andCond.Ptr() == this->cond.Ptr())
	{
		return this->andCond->ToWhereClauseOrClient(sb, sqlType, tzQhr, maxDBItem, clientConditions);
	}
	else
	{
		if (!this->cond->ToWhereClause(sb, sqlType, tzQhr, maxDBItem))
		{
			clientConditions->Add(this->cond);
		}
		return true;
	}
}

Bool Data::QueryConditions::CanWhereClause(UOSInt maxDBItem) const
{
	return this->cond->CanWhereClause(maxDBItem);
}

void Data::QueryConditions::GetFieldList(NN<Data::ArrayListStringNN> fieldList) const
{
	this->cond->GetFieldList(fieldList);
}

NN<Data::QueryConditions> Data::QueryConditions::TimeBetween(Text::CStringNN fieldName, const Data::Timestamp &t1, const Data::Timestamp &t2)
{
	NN<Conditions::TimeBetweenCondition> cond;
	NEW_CLASSNN(cond, Conditions::TimeBetweenCondition(fieldName, t1, t2));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::TimeBefore(Text::CStringNN fieldName, const Data::Timestamp &val)
{
	NN<Conditions::TimeCondition> cond;
	NEW_CLASSNN(cond, Conditions::TimeCondition(TSField(fieldName), TimeObj(val), Data::CompareCondition::Less));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::TimeOnOrAfter(Text::CStringNN fieldName, const Data::Timestamp &val)
{
	NN<Conditions::TimeCondition> cond;
	NEW_CLASSNN(cond, Conditions::TimeCondition(TSField(fieldName), TimeObj(val), Data::CompareCondition::GreaterOrEqual));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Or()
{
	NN<Conditions::BooleanOr> cond;
	if (!this->orCond.SetTo(cond))
	{
		NEW_CLASSNN(cond, Conditions::BooleanOr());
		this->orCond = cond;
		this->cond = cond;
		cond->AddOr(this->andCond);
	}
	NEW_CLASSNN(this->andCond, Conditions::BooleanAnd());
	cond->AddOr(this->andCond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::And(NN<Conditions::BooleanObject> innerCond)
{
	this->andCond->AddAnd(innerCond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int32Equals(Text::CStringNN fieldName, Int32 val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), Int32Obj(val), CompareCondition::Equal));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int32GE(Text::CStringNN fieldName, Int32 val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), Int32Obj(val), CompareCondition::GreaterOrEqual));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int32LE(Text::CStringNN fieldName, Int32 val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), Int32Obj(val), CompareCondition::LessOrEqual));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int32GT(Text::CStringNN fieldName, Int32 val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), Int32Obj(val), CompareCondition::Greater));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int32LT(Text::CStringNN fieldName, Int32 val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), Int32Obj(val), CompareCondition::Less));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int32In(Text::CStringNN fieldName, NN<Data::ArrayList<Int32>> vals)
{
	NN<Conditions::Int32InCondition> cond;
	NEW_CLASSNN(cond, Conditions::Int32InCondition(fieldName, vals));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int64Equals(Text::CStringNN fieldName, Int64 val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), Int64Obj(val), CompareCondition::Equal));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int64GE(Text::CStringNN fieldName, Int64 val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), Int64Obj(val), CompareCondition::GreaterOrEqual));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int64LE(Text::CStringNN fieldName, Int64 val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), Int64Obj(val), CompareCondition::LessOrEqual));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int64GT(Text::CStringNN fieldName, Int64 val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), Int64Obj(val), CompareCondition::Greater));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::Int64LT(Text::CStringNN fieldName, Int64 val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), Int64Obj(val), CompareCondition::Less));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::DoubleEquals(Text::CStringNN fieldName, Double val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), DoubleObj(val), CompareCondition::Equal));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::DoubleGE(Text::CStringNN fieldName, Double val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), DoubleObj(val), CompareCondition::GreaterOrEqual));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::DoubleLE(Text::CStringNN fieldName, Double val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), DoubleObj(val), CompareCondition::LessOrEqual));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::DoubleGT(Text::CStringNN fieldName, Double val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), DoubleObj(val), CompareCondition::Greater));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::DoubleLT(Text::CStringNN fieldName, Double val)
{
	NN<Conditions::NumberCondition> cond;
	NEW_CLASSNN(cond, Conditions::NumberCondition(NumField(fieldName), DoubleObj(val), CompareCondition::Less));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::StrIn(Text::CStringNN fieldName, NN<Data::ArrayListArr<const UTF8Char>> vals)
{
	NN<Conditions::StringInCondition> cond;
	NEW_CLASSNN(cond, Conditions::StringInCondition(fieldName, vals));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::StrNotIn(Text::CStringNN fieldName, NN<Data::ArrayListArr<const UTF8Char>> vals)
{
	NN<Conditions::StringNotInCondition> cond;
	NEW_CLASSNN(cond, Conditions::StringNotInCondition(fieldName, vals));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::StrContains(Text::CStringNN fieldName, Text::CStringNN val)
{
	NN<Conditions::StringContainsCondition> cond;
	NEW_CLASSNN(cond, Conditions::StringContainsCondition(fieldName, val));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::StrEquals(Text::CStringNN fieldName, Text::CStringNN val)
{
	NN<Conditions::StringEqualsCondition> cond;
	NEW_CLASSNN(cond, Conditions::StringEqualsCondition(fieldName, val));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::BoolEquals(Text::CStringNN fieldName, Bool val)
{
	NN<Conditions::BooleanCondition> cond;
	NEW_CLASSNN(cond, Conditions::BooleanCondition(fieldName, val));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::IsNull(Text::CStringNN fieldName)
{
	NN<Conditions::NullCondition> cond;
	NEW_CLASSNN(cond, Conditions::NullCondition(fieldName, false));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::NotNull(Text::CStringNN fieldName)
{
	NN<Conditions::NullCondition> cond;
	NEW_CLASSNN(cond, Conditions::NullCondition(fieldName, true));
	this->andCond->AddAnd(cond);
	return *this;
}

NN<Data::QueryConditions> Data::QueryConditions::New()
{
	NN<Data::QueryConditions> cond;
	NEW_CLASSNN(cond, QueryConditions());
	return cond;
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

Bool Data::QueryConditions::ObjectValid(NN<Data::VariObject> obj, NN<Data::ArrayListNN<Conditions::BooleanObject>> conditionList, OutParam<Bool> outVal)
{
	Bool ret = true;
	Bool v;
	NN<Conditions::BooleanObject> cond;
	Data::ArrayIterator<NN<Conditions::BooleanObject>> it = conditionList->Iterator();
	while (it.HasNext())
	{
		cond = it.Next();
		if (!cond->Eval(obj, v))
			return false;
		ret = ret && v;
	}
	outVal.Set(ret);
	return true;
}

Bool Data::QueryConditions::ObjectValid(NN<Data::ObjectGetter> getter, NN<Data::ArrayListNN<Conditions::BooleanObject>> conditionList, OutParam<Bool> outVal)
{
	Bool ret = true;
	Bool v;
	NN<Conditions::BooleanObject> cond;
	Data::ArrayIterator<NN<Conditions::BooleanObject>> it = conditionList->Iterator();
	while (it.HasNext())
	{
		cond = it.Next();
		if (!cond->Eval(getter, v))
			return false;
		ret = ret && v;
	}
	outVal.Set(ret);
	return true;
}

Optional<Data::QueryConditions> Data::QueryConditions::ParseStr(Text::CStringNN s, DB::SQLType sqlType)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbField;
	UnsafeArray<const UTF8Char> sql = s.v;
	NN<Data::VariItem> item;
	Data::QueryConditions *cond;
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
				if (!DB::SQL::SQLUtil::ParseValue(sb.ToCString(), sqlType).SetTo(item))
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
						Data::ArrayListArr<const UTF8Char> vals;
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
						}
						cond->StrIn(sbField.ToCString(), vals);
						items.DeleteAll();
						break;
					}
					else if (itemType == Data::VariItem::ItemType::I32 || itemType == Data::VariItem::ItemType::I64 || itemType == Data::VariItem::ItemType::F64)
					{
						Data::ArrayList<Int32> i32List;
						Data::ArrayList<Int64> i64List;
						Data::ArrayList<Double> dblList;
						Bool hasI64 = false;
						Bool hasF64 = false;
						Data::ArrayIterator<NN<Data::VariItem>> it = items.Iterator();
						NN<Data::VariItem> item;
						while (it.HasNext())
						{
							item = it.Next();
							if (item->GetItemType() == Data::VariItem::ItemType::I32)
							{
								i32List.Add(item->GetAsI32());
								i64List.Add(item->GetAsI32());
								dblList.Add(item->GetAsF64());
							}
							else if (item->GetItemType() == Data::VariItem::ItemType::I64)
							{
								i32List.Add(item->GetAsI32());
								i64List.Add(item->GetAsI64());
								dblList.Add(item->GetAsF64());
								hasI64 = true;
							}
							else if (item->GetItemType() == Data::VariItem::ItemType::F64)
							{
								i32List.Add(item->GetAsI32());
								i64List.Add(item->GetAsI64());
								dblList.Add(item->GetAsF64());
								hasF64 = true;
							}
							else
							{
								items.DeleteAll();
								DEL_CLASS(cond);
								return 0;
							}
						}
						if (hasF64)
						{
//							cond->DoubleIn(sbField.ToCString(), dblList);
							items.DeleteAll();
							DEL_CLASS(cond);
							return 0;
						}
						else if (hasI64)
						{
//							cond->Int64In(sbField.ToCString(), i64List);
							items.DeleteAll();
							DEL_CLASS(cond);
							return 0;
						}
						else
						{
							cond->Int32In(sbField.ToCString(), i32List);
						}
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
					if (!DB::SQL::SQLUtil::ParseValue(sb.ToCString(), sqlType).SetTo(item))
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
							Data::ArrayListArr<const UTF8Char> vals;
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
			if (!DB::SQL::SQLUtil::ParseValue(sb.ToCString(), sqlType).SetTo(item))
			{
				DEL_CLASS(cond);
				return 0;
			}
			Data::VariItem::ItemType itemType = item->GetItemType();
			if (itemType == Data::VariItem::ItemType::Str)
			{
				cond->StrEquals(sbField.ToCString(), item->GetItemValue().str->ToCString());
			}
			else if (itemType == Data::VariItem::ItemType::I32)
			{
				cond->Int32Equals(sbField.ToCString(), item->GetItemValue().i32);
			}
			else if (itemType == Data::VariItem::ItemType::I64)
			{
				cond->Int64Equals(sbField.ToCString(), item->GetItemValue().i64);
			}
			else if (itemType == Data::VariItem::ItemType::F64)
			{
				cond->DoubleEquals(sbField.ToCString(), item->GetItemValue().f64);
			}
			else
			{
				item.Delete();
				DEL_CLASS(cond);
				return 0;
			}
			item.Delete();
		}
		else if (sb.Equals(UTF8STRC(">")))
		{
			sql = DB::SQL::SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (!DB::SQL::SQLUtil::ParseValue(sb.ToCString(), sqlType).SetTo(item))
			{
				DEL_CLASS(cond);
				return 0;
			}
			Data::VariItem::ItemType itemType = item->GetItemType();
			if (itemType == Data::VariItem::ItemType::I32)
			{
				cond->Int32GT(sbField.ToCString(), item->GetItemValue().i32);
			}
			else if (itemType == Data::VariItem::ItemType::I64)
			{
				cond->Int64GT(sbField.ToCString(), item->GetItemValue().i64);
			}
			else if (itemType == Data::VariItem::ItemType::F64)
			{
				cond->DoubleGT(sbField.ToCString(), item->GetItemValue().f64);
			}
			else
			{
				item.Delete();
				DEL_CLASS(cond);
				return 0;
			}
			item.Delete();
		}
		else if (sb.Equals(UTF8STRC("<")))
		{
			sql = DB::SQL::SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (!DB::SQL::SQLUtil::ParseValue(sb.ToCString(), sqlType).SetTo(item))
			{
				DEL_CLASS(cond);
				return 0;
			}
			Data::VariItem::ItemType itemType = item->GetItemType();
			if (itemType == Data::VariItem::ItemType::I32)
			{
				cond->Int32LT(sbField.ToCString(), item->GetItemValue().i32);
			}
			else if (itemType == Data::VariItem::ItemType::I64)
			{
				cond->Int64LT(sbField.ToCString(), item->GetItemValue().i64);
			}
			else if (itemType == Data::VariItem::ItemType::F64)
			{
				cond->DoubleLT(sbField.ToCString(), item->GetItemValue().f64);
			}
			else
			{
				item.Delete();
				DEL_CLASS(cond);
				return 0;
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
		else if (sb.EqualsICase(CSTR("AND")))
		{
		}
		else
		{
			DEL_CLASS(cond);
			return 0;
		}
	}
	return 0;
}

NN<Data::Conditions::TimeObject> Data::QueryConditions::TSField(Text::CStringNN fieldName)
{
	NN<Conditions::TimeField> cond;
	NEW_CLASSNN(cond, Conditions::TimeField(fieldName));
	return cond;
}

NN<Data::Conditions::NumberObject> Data::QueryConditions::NumField(Text::CStringNN fieldName)
{
	NN<Conditions::NumberField> cond;
	NEW_CLASSNN(cond, Conditions::NumberField(fieldName));
	return cond;
}

NN<Data::Conditions::TimeObject> Data::QueryConditions::TimeObj(const Data::Timestamp& val)
{
	NN<Conditions::TimeObject> cond;
	NEW_CLASSNN(cond, Conditions::TimestampObject(val));
	return cond;
}

NN<Data::Conditions::NumberObject> Data::QueryConditions::Int32Obj(Int32 val)
{
	NN<Conditions::Int32Object> cond;
	NEW_CLASSNN(cond, Conditions::Int32Object(val));
	return cond;
}

NN<Data::Conditions::NumberObject> Data::QueryConditions::Int64Obj(Int64 val)
{
	NN<Conditions::Int64Object> cond;
	NEW_CLASSNN(cond, Conditions::Int64Object(val));
	return cond;
}

NN<Data::Conditions::NumberObject> Data::QueryConditions::DoubleObj(Double val)
{
	NN<Conditions::Float64Object> cond;
	NEW_CLASSNN(cond, Conditions::Float64Object(val));
	return cond;
}
