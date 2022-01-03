#include "Stdafx.h"
#include "Data/DataComparer.h"
#include "Data/FieldComparator.h"
#include "Text/StringBuilderUTF8.h"

Data::FieldComparator::FieldComparator(const UTF8Char *compareConds)
{
	NEW_CLASS(this->fieldNames, Data::ArrayList<const UTF8Char*>());
	NEW_CLASS(this->dirs, Data::ArrayList<Int8>());
	if (compareConds == 0 || compareConds[0] == 0)
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	UTF8Char *sarr[2];
	UOSInt i = 2;
	Int8 dir;
	UOSInt len;
	sb.Append(compareConds);
	sarr[1] = sb.ToString();
	while (i == 2)
	{
		i = Text::StrSplitTrim(sarr, 2, sarr[1], ',');
		dir = 1;
		len = Text::StrCharCnt(sarr[0]);
		if (Text::StrEqualsICase(&sarr[0][len - 4], (const UTF8Char*)" ASC"))
		{
			sarr[0][len - 4] = 0;
		}
		else if (Text::StrEqualsICase(&sarr[0][len - 5], (const UTF8Char*)" DESC"))
		{
			sarr[0][len - 5] = 0;
			dir = (Int8)-1;
		}
		this->fieldNames->Add(Text::StrCopyNew(sarr[0]));
		this->dirs->Add(dir);
	}
}

Data::FieldComparator::~FieldComparator()
{
	LIST_FREE_FUNC(this->fieldNames, Text::StrDelNew);
	DEL_CLASS(this->fieldNames);
	DEL_CLASS(this->dirs);
}

OSInt Data::FieldComparator::Compare(VariObject *a, VariObject *b)
{
	UOSInt i = 0;
	UOSInt j = this->fieldNames->GetCount();
	OSInt k;
	while (i < j)
	{
		const UTF8Char *fieldName = this->fieldNames->GetItem(i);
		k = Compare(a->GetItem(fieldName), b->GetItem(fieldName)) * this->dirs->GetItem(i);
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
	return this->fieldNames->GetCount() > 0;
}

Bool Data::FieldComparator::ToOrderClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType)
{
	if (this->fieldNames->GetCount() == 0)
	{
		return false;
	}
	UTF8Char sbuff[512];
	UOSInt i = 0;
	UOSInt j = this->fieldNames->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendC(UTF8STRC(", "));
		}
		DB::DBUtil::SDBColUTF8(sbuff, this->fieldNames->GetItem(i), svrType);
		sb->Append(sbuff);
		if (this->dirs->GetItem(i) == -1)
		{
			sb->AppendC(UTF8STRC(" desc"));
		}
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
		case Data::VariItem::ItemType::Date:
			return a->GetItemValue().date->CompareTo(b->GetItemValue().date);
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
			return Data::DataComparer::Compare(a->GetItemValue().vector, b->GetItemValue().vector);
		case Data::VariItem::ItemType::UUID:
			return Data::DataComparer::Compare(a->GetItemValue().uuid, b->GetItemValue().uuid);
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
		a->ToString(&sbA);
		b->ToString(&sbB);
		return Text::StrCompare(sbA.ToString(), sbB.ToString());
	}
}
