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
	UIntOS i = 2;
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

IntOS Data::FieldComparator::Compare(NN<VariObject> a, NN<VariObject> b) const
{
	Data::ArrayIterator<NN<Text::String>> it = this->fieldNames.Iterator();
	IntOS k;
	UIntOS i = 0;
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
	UIntOS i = 0;
	while (it.HasNext())
	{
		if (found)
			sb->AppendC(UTF8STRC(", "));
		sptr = DB::DBUtil::SDBColUTF8(sbuff, it.Next()->v, sqlType);
		sb->AppendC(sbuff, (UIntOS)(sptr - sbuff));
		if (this->dirs.GetItem(i) == -1)
		{
			sb->AppendC(UTF8STRC(" desc"));
		}
		found = true;
		i++;
	}
	return true;
}

IntOS Data::FieldComparator::Compare(Optional<VariItem> a, Optional<VariItem> b)
{
	NN<VariItem> nna;
	NN<VariItem> nnb;
	if (a == b)
	{
		return 0;
	}
	if (!a.SetTo(nna))
	{
		return -1;
	}
	if (!b.SetTo(nnb))
	{
		return 1;
	}
	if (nna->GetItemType() == nnb->GetItemType())
	{
		switch (nna->GetItemType())
		{
		case Data::VariItem::ItemType::Unknown:
		case Data::VariItem::ItemType::Null:
			return 0;
		case Data::VariItem::ItemType::Str:
			return Text::StrCompare(nna->GetItemValue().str->v, nnb->GetItemValue().str->v);
		case Data::VariItem::ItemType::CStr:
			return Text::StrCompare(nna->GetItemValue().cstr.v, nnb->GetItemValue().cstr.v);
		case Data::VariItem::ItemType::Timestamp:
			return nna->GetItemValue().ts.CompareTo(nnb->GetItemValue().ts);
		case Data::VariItem::ItemType::Date:
			return nna->GetItemValue().date.CompareTo(nnb->GetItemValue().date);
		case Data::VariItem::ItemType::F32:
			return Data::DataComparer::Compare(nna->GetItemValue().f32, nnb->GetItemValue().f32);
		case Data::VariItem::ItemType::F64:
			return Data::DataComparer::Compare(nna->GetItemValue().f64, nnb->GetItemValue().f64);
		case Data::VariItem::ItemType::I8:
			return Data::DataComparer::Compare(nna->GetItemValue().i8, nnb->GetItemValue().i8);
		case Data::VariItem::ItemType::U8:
			return Data::DataComparer::Compare(nna->GetItemValue().u8, nnb->GetItemValue().u8);
		case Data::VariItem::ItemType::I16:
			return Data::DataComparer::Compare(nna->GetItemValue().i16, nnb->GetItemValue().i16);
		case Data::VariItem::ItemType::U16:
			return Data::DataComparer::Compare(nna->GetItemValue().u16, nnb->GetItemValue().u16);
		case Data::VariItem::ItemType::NI32:
		case Data::VariItem::ItemType::I32:
			return Data::DataComparer::Compare(nna->GetItemValue().i32, nnb->GetItemValue().i32);
		case Data::VariItem::ItemType::U32:
			return Data::DataComparer::Compare(nna->GetItemValue().u32, nnb->GetItemValue().u32);
		case Data::VariItem::ItemType::I64:
			return Data::DataComparer::Compare(nna->GetItemValue().i64, nnb->GetItemValue().i64);
		case Data::VariItem::ItemType::U64:
			return Data::DataComparer::Compare(nna->GetItemValue().u64, nnb->GetItemValue().u64);
		case Data::VariItem::ItemType::BOOL:
			return Data::DataComparer::Compare(nna->GetItemValue().boolean, nnb->GetItemValue().boolean);
		case Data::VariItem::ItemType::ByteArr:
		{
			Data::ReadonlyArray<UInt8> *arrA = nna->GetItemValue().byteArr;
			Data::ReadonlyArray<UInt8> *arrB = nnb->GetItemValue().byteArr;
			return Data::DataComparer::Compare(arrA->GetArray(), arrA->GetCount(), arrB->GetArray(), arrB->GetCount());
		}
		case Data::VariItem::ItemType::Vector:
			return Data::DataComparer::Compare(nna->GetItemValue().vector.Ptr(), nnb->GetItemValue().vector.Ptr());
		case Data::VariItem::ItemType::UUID:
			return Data::DataComparer::Compare(nna->GetItemValue().uuid.Ptr(), nnb->GetItemValue().uuid.Ptr());
		}
		return 0;
	}
	else if (nna->GetItemType() == Data::VariItem::ItemType::Null)
	{
		return -1;
	}
	else if (nnb->GetItemType() == Data::VariItem::ItemType::Null)
	{
		return 1;
	}
	else
	{
		Text::StringBuilderUTF8 sbA;
		Text::StringBuilderUTF8 sbB;
		nna->ToString(sbA);
		nnb->ToString(sbB);
		return Text::StrCompare(sbA.ToString(), sbB.ToString());
	}
}
