#include "Stdafx.h"
#include "Data/DataComparer.h"
#include "Data/FieldComparator.h"
#include "Text/StringBuilderUTF8.h"

Data::FieldComparator::FieldComparator(Text::CString compareConds)
{
	Text::CStringNN nncompareConds;
	if (!compareConds.SetTo(nncompareConds) || nncompareConds.leng == 0)
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	Text::PString sarr[2];
	UOSInt i = 2;
	Int8 dir;
	sb.Append(nncompareConds);
	sarr[1] = sb;
	while (i == 2)
	{
		i = Text::StrSplitTrimP(sarr, 2, sarr[1], ',');
		dir = 1;
		if (sarr[0].EndsWithICase(UTF8STRC(" ASC")))
		{
			sarr[0].RemoveChars(4);
		}
		else if (sarr[0].EndsWithICase(UTF8STRC(" DESC")))
		{
			sarr[0].RemoveChars(5);
			dir = (Int8)-1;
		}
		this->fieldNames.Add(Text::String::New(sarr[0].v, sarr[0].leng));
		this->dirs.Add(dir);
	}
}

Data::FieldComparator::~FieldComparator()
{
	this->fieldNames.FreeAll();
}

OSInt Data::FieldComparator::Compare(NN<VariObject> a, NN<VariObject> b) const
{
	Data::ArrayIterator<NN<Text::String>> it = this->fieldNames.Iterator();
	OSInt k;
	UOSInt i = 0;
	while (it.HasNext())
	{
		NN<Text::String> fieldName = it.Next();
		k = Compare(a->GetItem(fieldName->v), b->GetItem(fieldName->v)) * this->dirs.GetItem(i);
		if (k != 0)
		{
			return k;
		}
		i++;
	}
	return 0;
}

Bool Data::FieldComparator::IsValid()
{
	return this->fieldNames.GetCount() > 0;
}

Bool Data::FieldComparator::ToOrderClause(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType)
{
	if (this->fieldNames.GetCount() == 0)
	{
		return false;
	}
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Data::ArrayIterator<NN<Text::String>> it = this->fieldNames.Iterator();
	Bool found = false;
	UOSInt i = 0;
	while (it.HasNext())
	{
		if (found)
			sb->AppendC(UTF8STRC(", "));
		sptr = DB::DBUtil::SDBColUTF8(sbuff, it.Next()->v, sqlType);
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		if (this->dirs.GetItem(i) == -1)
		{
			sb->AppendC(UTF8STRC(" desc"));
		}
		found = true;
		i++;
	}
	return true;
}

OSInt Data::FieldComparator::Compare(VariItem *a, VariItem *b)
{
	if (a == b)
	{
		return 0;
	}
	if (a == 0)
	{
		return -1;
	}
	if (b == 0)
	{
		return 1;
	}
	if (a->GetItemType() == b->GetItemType())
	{
		switch (a->GetItemType())
		{
		case Data::VariItem::ItemType::Unknown:
		case Data::VariItem::ItemType::Null:
			return 0;
		case Data::VariItem::ItemType::Str:
			return Text::StrCompare(a->GetItemValue().str->v, b->GetItemValue().str->v);
		case Data::VariItem::ItemType::CStr:
			return Text::StrCompare(a->GetItemValue().cstr.v, b->GetItemValue().cstr.v);
		case Data::VariItem::ItemType::Timestamp:
			return a->GetItemValue().ts.CompareTo(b->GetItemValue().ts);
		case Data::VariItem::ItemType::Date:
			return a->GetItemValue().date.CompareTo(b->GetItemValue().date);
		case Data::VariItem::ItemType::F32:
			return Data::DataComparer::Compare(a->GetItemValue().f32, b->GetItemValue().f32);
		case Data::VariItem::ItemType::F64:
			return Data::DataComparer::Compare(a->GetItemValue().f64, b->GetItemValue().f64);
		case Data::VariItem::ItemType::I8:
			return Data::DataComparer::Compare(a->GetItemValue().i8, b->GetItemValue().i8);
		case Data::VariItem::ItemType::U8:
			return Data::DataComparer::Compare(a->GetItemValue().u8, b->GetItemValue().u8);
		case Data::VariItem::ItemType::I16:
			return Data::DataComparer::Compare(a->GetItemValue().i16, b->GetItemValue().i16);
		case Data::VariItem::ItemType::U16:
			return Data::DataComparer::Compare(a->GetItemValue().u16, b->GetItemValue().u16);
		case Data::VariItem::ItemType::NI32:
		case Data::VariItem::ItemType::I32:
			return Data::DataComparer::Compare(a->GetItemValue().i32, b->GetItemValue().i32);
		case Data::VariItem::ItemType::U32:
			return Data::DataComparer::Compare(a->GetItemValue().u32, b->GetItemValue().u32);
		case Data::VariItem::ItemType::I64:
			return Data::DataComparer::Compare(a->GetItemValue().i64, b->GetItemValue().i64);
		case Data::VariItem::ItemType::U64:
			return Data::DataComparer::Compare(a->GetItemValue().u64, b->GetItemValue().u64);
		case Data::VariItem::ItemType::BOOL:
			return Data::DataComparer::Compare(a->GetItemValue().boolean, b->GetItemValue().boolean);
		case Data::VariItem::ItemType::ByteArr:
		{
			Data::ReadonlyArray<UInt8> *arrA = a->GetItemValue().byteArr;
			Data::ReadonlyArray<UInt8> *arrB = b->GetItemValue().byteArr;
			return Data::DataComparer::Compare(arrA->GetArray(), arrA->GetCount(), arrB->GetArray(), arrB->GetCount());
		}
		case Data::VariItem::ItemType::Vector:
			return Data::DataComparer::Compare(a->GetItemValue().vector.Ptr(), b->GetItemValue().vector.Ptr());
		case Data::VariItem::ItemType::UUID:
			return Data::DataComparer::Compare(a->GetItemValue().uuid.Ptr(), b->GetItemValue().uuid.Ptr());
		}
		return 0;
	}
	else if (a->GetItemType() == Data::VariItem::ItemType::Null)
	{
		return -1;
	}
	else if (b->GetItemType() == Data::VariItem::ItemType::Null)
	{
		return 1;
	}
	else
	{
		Text::StringBuilderUTF8 sbA;
		Text::StringBuilderUTF8 sbB;
		a->ToString(sbA);
		b->ToString(sbB);
		return Text::StrCompare(sbA.ToString(), sbB.ToString());
	}
}
