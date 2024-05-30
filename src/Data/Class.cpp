#include "Stdafx.h"
#include "Data/Class.h"
#include "Data/StringMap.h"
#include "Text/CharUtil.h"
#include "Text/CPPText.h"
#include "Text/JavaText.h"
#include "Text/JSText.h"
#include "Text/StringBuilderUTF8.h"

void Data::Class::FreeFieldInfo(NN<FieldInfo> field)
{
	field->name->Release();
	OPTSTR_DEL(field->typeName);
	MemFreeNN(field);
}

Data::Class::Class(const void *refObj)
{
	this->refObj = refObj;
}

Data::Class::~Class()
{
	this->fields.FreeAll(FreeFieldInfo);
}

UOSInt Data::Class::AddField(Text::CStringNN name, OSInt ofst, Data::VariItem::ItemType itemType, Bool notNull)
{
	NN<FieldInfo> field = MemAllocNN(FieldInfo);
	field->name = Text::String::New(name);
	field->ofst = ofst;
	field->itemType = itemType;
	field->notNull = notNull;
	field->typeName = 0;
	field->byNameFunc = 0;
	this->fields.Add(field);
	return Data::VariItem::GetItemSize(itemType);
}

UOSInt Data::Class::AddFieldEnum(Text::CStringNN name, OSInt ofst, Text::CStringNN typeName, UOSInt fieldSize, ByNameFunc byNameFunc)
{
	NN<FieldInfo> field = MemAllocNN(FieldInfo);
	field->name = Text::String::New(name);
	field->ofst = ofst;
	if (byNameFunc != 0)
	{
		field->itemType = Data::VariItem::ItemType::Str;
	}
	else if (fieldSize == 8)
	{
		field->itemType = Data::VariItem::ItemType::I64;
	}
	else
	{
		field->itemType = Data::VariItem::ItemType::I32;
	}
	field->byNameFunc = byNameFunc;
	field->notNull = false;
	field->typeName = Text::String::New(typeName);
	this->fields.Add(field);
	return Data::VariItem::GetItemSize(field->itemType);
}

Bool Data::Class::AddField(Text::CStringNN name, const UInt8 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::U8, true) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const Int8 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::I8, true) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const UInt16 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::U16, true) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const Int16 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::I16, true) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const UInt32 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::U32, true) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const Int32 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::I32, true) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const NInt32 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::NI32, false) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const UInt64 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::U64, true) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const Int64 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::I64, true) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const Single *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::F32, true) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const Double *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::F64, true) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, Text::String *const *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::Str, false) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const Optional<Text::String> *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::Str, false) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const NN<Text::String> *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::Str, true) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const Data::Timestamp *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::Timestamp, false) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const Data::Date *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::Date, false) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const Bool *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::BOOL, true) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, Data::ReadonlyArray<UInt8> *const *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::ByteArr, false) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, const Optional<Math::Geometry::Vector2D> *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::Vector, false) != 0;
}

Bool Data::Class::AddField(Text::CStringNN name, Data::UUID *const *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::UUID, false) != 0;
}

UOSInt Data::Class::GetFieldCount()
{
	return this->fields.GetCount();
}

Optional<Text::String> Data::Class::GetFieldName(UOSInt index)
{
	NN<FieldInfo> field;
	if (this->fields.GetItem(index).SetTo(field))
	{
		return field->name;
	}
	return 0;
}

Data::VariItem::ItemType Data::Class::GetFieldType(UOSInt index)
{
	NN<FieldInfo> field;
	if (this->fields.GetItem(index).SetTo(field))
	{
		return field->itemType;
	}
	return Data::VariItem::ItemType::Unknown;
}

Optional<Data::Class::FieldInfo> Data::Class::GetFieldInfo(UOSInt index)
{
	return this->fields.GetItem(index);
}

Optional<Data::VariItem> Data::Class::GetNewValue(UOSInt index, AnyType obj)
{
	NN<FieldInfo> field;
	if (!this->fields.GetItem(index).SetTo(field))
	{
		return 0;
	}
	void *valPtr = (void*)(field->ofst + (UInt8*)obj.p);
	return Data::VariItem::NewFromPtr(valPtr, field->itemType).Ptr();
}

Bool Data::Class::IsNotNull(UOSInt index)
{
	NN<FieldInfo> field;
	if (this->fields.GetItem(index).SetTo(field))
	{
		return field->notNull;
	}
	return false;
}

Bool Data::Class::GetValue(NN<Data::VariItem> itm, UOSInt index, AnyType obj)
{
	NN<FieldInfo> field;
	if (!this->fields.GetItem(index).SetTo(field))
	{
		return false;
	}
	void *valPtr = (void*)(field->ofst + (UInt8*)obj.p);
	if (field->typeName.NotNull())
	{
		Data::VariItem::SetFromPtr(itm, valPtr, Data::VariItem::ItemType::I32);
	}
	else
	{
		Data::VariItem::SetFromPtr(itm, valPtr, field->itemType);
	}
	return true;
}

Bool Data::Class::SetField(AnyType obj, UOSInt index, NN<Data::VariItem> item)
{
	NN<FieldInfo> field;
	if (!this->fields.GetItem(index).SetTo(field))
	{
		return false;
	}
	void *valPtr = (void*)(field->ofst + (OSInt)obj.p);
	Data::VariItem::SetPtr(valPtr, field->itemType, item);
	return true;
}

Bool Data::Class::SetFieldClearItem(AnyType obj, UOSInt index, NN<Data::VariItem> item)
{
	NN<FieldInfo> field;
	if (!this->fields.GetItem(index).SetTo(field))
	{
		return false;
	}
	void *valPtr = (void*)(field->ofst + (OSInt)obj.p);
	Data::VariItem::SetPtrAndNotKeep(valPtr, field->itemType, item);
	return true;
}

Bool Data::Class::Equals(AnyType obj1, AnyType obj2)
{
	UOSInt i = this->fields.GetCount();
	NN<FieldInfo> field;
	while (i-- > 0)
	{
		field = this->fields.GetItemNoCheck(i);
		void *valPtr1 = (void*)(field->ofst + (OSInt)obj1.p);
		void *valPtr2 = (void*)(field->ofst + (OSInt)obj2.p);
		if (!Data::VariItem::PtrEquals(valPtr1, valPtr2, field->itemType))
		{
			return false;
		}
	}
	return true;
}

void Data::Class::ToCppClassHeader(Text::StringBase<UTF8Char> *clsName, UOSInt tabLev, NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("class "));
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("{\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("private:\r\n"));
	NN<Data::ArrayListNN<FieldInfo>> fieldList = this->fields;
	NN<FieldInfo> field;
	NN<Text::String> typeName;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItemNoCheck(i);
		sb->AppendChar('\t', tabLev + 1);
		if (field->typeName.SetTo(typeName))
		{
			sb->Append(typeName);
		}
		else
		{
			sb->Append(Text::CPPText::GetCppType(field->itemType, field->notNull));
		}
		sb->AppendUTF8Char(' ');
		sb->Append(field->name);
		sb->AppendC(UTF8STRC(";\r\n"));
		i++;
	}
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("public:\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("();\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendUTF8Char('~');
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("();\r\n"));
	sb->AppendC(UTF8STRC("\r\n"));
	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItemNoCheck(i);
		if (field->typeName.SetTo(typeName))
		{
			sb->AppendChar('\t', tabLev + 1);
			sb->Append(typeName);
			sb->AppendUTF8Char(' ');
			sb->AppendC(UTF8STRC("Get"));
			sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
			sb->AppendC(field->name->v + 1, field->name->leng - 1);
			sb->AppendC(UTF8STRC("() const;\r\n"));

			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("void Set"));
			sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
			sb->AppendC(field->name->v + 1, field->name->leng - 1);
			sb->AppendUTF8Char('(');
			sb->Append(typeName);
			sb->AppendUTF8Char(' ');
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(");\r\n"));

			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("void Set"));
			sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
			sb->AppendC(field->name->v + 1, field->name->leng - 1);
			sb->AppendC(UTF8STRC("FromName(Text::CStringNN "));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(");\r\n"));

			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("void Set"));
			sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
			sb->AppendC(field->name->v + 1, field->name->leng - 1);
			sb->AppendC(UTF8STRC("FromIndex(UOSInt "));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(");\r\n"));
		}
		else
		{
			Text::CStringNN cppType = Text::CPPText::GetCppType(field->itemType, field->notNull);
			sb->AppendChar('\t', tabLev + 1);
			sb->Append(cppType);
			sb->AppendUTF8Char(' ');
			sb->AppendC(UTF8STRC("Get"));
			sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
			sb->AppendC(field->name->v + 1, field->name->leng - 1);
			sb->AppendC(UTF8STRC("() const;\r\n"));

			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("void Set"));
			sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
			sb->AppendC(field->name->v + 1, field->name->leng - 1);
			sb->AppendUTF8Char('(');
			sb->Append(cppType);
			sb->AppendUTF8Char(' ');
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(");\r\n"));
			
			if (field->itemType == Data::VariItem::ItemType::Str)
			{
				sb->AppendChar('\t', tabLev + 1);
				sb->AppendC(UTF8STRC("void Set"));
				sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
				sb->AppendC(field->name->v + 1, field->name->leng - 1);
				if (field->notNull)
				{
					sb->AppendC(UTF8STRC("(Text::CStringNN "));
				}
				else
				{
					sb->AppendC(UTF8STRC("(Text::CString "));
				}
				sb->Append(field->name);
				sb->AppendC(UTF8STRC(");\r\n"));
			}
		}
		i++;
	}

	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("NN<Data::NamedClass<"));
	sb->Append(clsName);
	sb->AppendC(UTF8STRC(">> CreateClass() const;\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("Bool FillFromDBReader(NN<DB::DBReader> r);\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("static void DBColList(NN<Data::ArrayListNN<Text::String>> colList);\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("};\r\n"));
}

void Data::Class::ToCppClassSource(Text::StringBase<UTF8Char> *clsPrefix, Text::StringBase<UTF8Char> *clsName, UOSInt tabLev, NN<Text::StringBuilderUTF8> sb)
{
	if (clsPrefix == 0)
	{
		clsPrefix = Text::String::NewEmpty().Ptr();
	}
	NN<Data::ArrayListNN<FieldInfo>> fieldList = this->fields;
	NN<FieldInfo> field;
	NN<Text::String> typeName;
	UOSInt i;
	UOSInt j;

	sb->AppendChar('\t', tabLev);
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("::"));
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("()\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("{\r\n"));
	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItemNoCheck(i);
		if (field->typeName.SetTo(typeName))
		{
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("this->"));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(" = ("));
			sb->Append(typeName);
			sb->AppendC(UTF8STRC(")0;\r\n"));
		}
		else
		{
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("this->"));
			sb->Append(field->name);
			if (field->itemType == Data::VariItem::ItemType::Str && field->notNull)
			{
				sb->AppendC(UTF8STRC(" = Text::String::NewEmpty();\r\n"));
			}
			else if (field->itemType == Data::VariItem::ItemType::I32 && !field->notNull)
			{
				sb->AppendC(UTF8STRC(" = nullptr;\r\n"));
			}
			else
			{
				sb->AppendC(UTF8STRC(" = 0;\r\n"));
			}
		}
		i++;
	}
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("}\r\n"));
	sb->AppendC(UTF8STRC("\r\n"));

	sb->AppendChar('\t', tabLev);
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("::~"));
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("()\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("{\r\n"));
	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItemNoCheck(i);
		if (field->typeName.NotNull())
		{

		}
		else
		{
			switch (field->itemType)
			{
			case Data::VariItem::ItemType::Str:
				sb->AppendChar('\t', tabLev + 1);
				if (field->notNull)
				{
					sb->AppendC(UTF8STRC("this->"));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC("->Release();\r\n"));
				}
				else
				{
					sb->AppendC(UTF8STRC("OPTSTR_DEL(this->"));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC(");\r\n"));
				}
				break;
			case Data::VariItem::ItemType::Vector:
				sb->AppendChar('\t', tabLev + 1);
				sb->AppendC(UTF8STRC("this->"));
				sb->Append(field->name);
				sb->AppendC(UTF8STRC(".Delete();\r\n"));
				break;
			case Data::VariItem::ItemType::ByteArr:
			case Data::VariItem::ItemType::UUID:
				sb->AppendChar('\t', tabLev + 1);
				sb->AppendC(UTF8STRC("SDEL_CLASS(this->"));
				sb->Append(field->name);
				sb->AppendC(UTF8STRC(");\r\n"));
				break;
			case Data::VariItem::ItemType::Timestamp:
			case Data::VariItem::ItemType::Date:
			case Data::VariItem::ItemType::CStr:
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
			case Data::VariItem::ItemType::Unknown:
			case Data::VariItem::ItemType::Null:
			default:
				break;
			}
		}
		i++;
	}
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("}\r\n"));
	sb->AppendC(UTF8STRC("\r\n"));

	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItemNoCheck(i);
		if (field->typeName.SetTo(typeName))
		{
			sb->AppendChar('\t', tabLev);
			sb->Append(typeName);
			sb->AppendUTF8Char(' ');
			sb->Append(clsPrefix);
			sb->Append(clsName);
			sb->AppendC(UTF8STRC("::Get"));
			sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
			sb->AppendC(field->name->v + 1, field->name->leng - 1);
			sb->AppendC(UTF8STRC("() const\r\n"));
			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("{\r\n"));
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("return this->"));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(";\r\n"));
			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("}\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));

			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("void "));
			sb->Append(clsPrefix);
			sb->Append(clsName);
			sb->AppendC(UTF8STRC("::Set"));
			sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
			sb->AppendC(field->name->v + 1, field->name->leng - 1);
			sb->AppendUTF8Char('(');
			sb->Append(typeName);
			sb->AppendUTF8Char(' ');
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(")\r\n"));
			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("{\r\n"));
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("this->"));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(" = "));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(";\r\n"));
			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("}\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));

			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("void "));
			sb->Append(clsPrefix);
			sb->Append(clsName);
			sb->AppendC(UTF8STRC("::Set"));
			sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
			sb->AppendC(field->name->v + 1, field->name->leng - 1);
			sb->AppendC(UTF8STRC("FromName(Text::CStringNN "));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(")\r\n"));
			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("{\r\n"));
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("this->"));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(" = "));
			sb->Append(typeName);
			sb->AppendC(UTF8STRC("FromName("));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(");\r\n"));
			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("}\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));

			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("void "));
			sb->Append(clsPrefix);
			sb->Append(clsName);
			sb->AppendC(UTF8STRC("::Set"));
			sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
			sb->AppendC(field->name->v + 1, field->name->leng - 1);
			sb->AppendC(UTF8STRC("FromIndex(UOSInt "));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(")\r\n"));
			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("{\r\n"));
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("this->"));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(" = ("));
			sb->Append(typeName);
			sb->AppendUTF8Char(')');
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(";\r\n"));
			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("}\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		else
		{
			Text::CStringNN cppType = Text::CPPText::GetCppType(field->itemType, field->notNull);
			sb->AppendChar('\t', tabLev);
			sb->Append(cppType);
			sb->AppendUTF8Char(' ');
			sb->Append(clsPrefix);
			sb->Append(clsName);
			sb->AppendC(UTF8STRC("::Get"));
			sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
			sb->AppendC(field->name->v + 1, field->name->leng - 1);
			sb->AppendC(UTF8STRC("() const\r\n"));
			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("{\r\n"));
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("return this->"));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(";\r\n"));
			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("}\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));

			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("void "));
			sb->Append(clsPrefix);
			sb->Append(clsName);
			sb->AppendC(UTF8STRC("::Set"));
			sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
			sb->AppendC(field->name->v + 1, field->name->leng - 1);
			sb->AppendUTF8Char('(');
			sb->Append(cppType);
			sb->AppendUTF8Char(' ');
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(")\r\n"));
			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("{\r\n"));
			switch (field->itemType)
			{
			case Data::VariItem::ItemType::Str:
				if (field->notNull)
				{
					sb->AppendChar('\t', tabLev + 1);
					sb->AppendC(UTF8STRC("this->"));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC("->Release();\r\n"));
					sb->AppendChar('\t', tabLev + 1);
					sb->AppendC(UTF8STRC("this->"));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC(" = "));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC("->Clone();\r\n"));
					sb->AppendChar('\t', tabLev);
					sb->AppendC(UTF8STRC("}\r\n"));
					sb->AppendC(UTF8STRC("\r\n"));

					sb->AppendChar('\t', tabLev);
					sb->AppendC(UTF8STRC("void "));
					sb->Append(clsPrefix);
					sb->Append(clsName);
					sb->AppendC(UTF8STRC("::Set"));
					sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
					sb->AppendC(field->name->v + 1, field->name->leng - 1);
					sb->AppendC(UTF8STRC("(Text::CStringNN "));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC(")\r\n"));
					sb->AppendChar('\t', tabLev);
					sb->AppendC(UTF8STRC("{\r\n"));
					sb->AppendChar('\t', tabLev + 1);
					sb->AppendC(UTF8STRC("this->"));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC("->Release();\r\n"));
					sb->AppendChar('\t', tabLev + 1);
					sb->AppendC(UTF8STRC("this->"));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC(" = Text::String::New("));
					sb->Append(field->name);
					sb->AppendUTF8Char(')');
				}
				else
				{
					sb->AppendChar('\t', tabLev + 1);
					sb->AppendC(UTF8STRC("OPTSTR_DEL(this->"));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC(");\r\n"));
					sb->AppendChar('\t', tabLev + 1);
					sb->AppendC(UTF8STRC("this->"));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC(" = !"));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC(".IsNull()?"));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC(".OrNull()->Clone():Optional<Text::String>(0);\r\n"));
					sb->AppendChar('\t', tabLev);
					sb->AppendC(UTF8STRC("}\r\n"));
					sb->AppendC(UTF8STRC("\r\n"));

					sb->AppendChar('\t', tabLev);
					sb->AppendC(UTF8STRC("void "));
					sb->Append(clsPrefix);
					sb->Append(clsName);
					sb->AppendC(UTF8STRC("::Set"));
					sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
					sb->AppendC(field->name->v + 1, field->name->leng - 1);
					sb->AppendC(UTF8STRC("(Text::CString "));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC(")\r\n"));
					sb->AppendChar('\t', tabLev);
					sb->AppendC(UTF8STRC("{\r\n"));
					sb->AppendChar('\t', tabLev + 1);
					sb->AppendC(UTF8STRC("OPTSTR_DEL(this->"));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC(");\r\n"));
					sb->AppendChar('\t', tabLev + 1);
					sb->AppendC(UTF8STRC("this->"));
					sb->Append(field->name);
					sb->AppendC(UTF8STRC(" = Text::String::NewOrNull("));
					sb->Append(field->name);
					sb->AppendUTF8Char(')');
				}
				break;
			case Data::VariItem::ItemType::Date:
			case Data::VariItem::ItemType::Timestamp:
				sb->AppendChar('\t', tabLev + 1);
				sb->AppendC(UTF8STRC("this->"));
				sb->Append(field->name);
				sb->AppendC(UTF8STRC(" = "));
				sb->Append(field->name);
				break;
			case Data::VariItem::ItemType::Vector:
				sb->AppendChar('\t', tabLev + 1);
				sb->AppendC(UTF8STRC("this->"));
				sb->Append(field->name);
				sb->AppendC(UTF8STRC(".Delete();\r\n"));
				sb->AppendChar('\t', tabLev + 1);
				sb->AppendC(UTF8STRC("this->"));
				sb->Append(field->name);
				sb->AppendC(UTF8STRC(" = "));
				sb->Append(field->name);
				sb->AppendC(UTF8STRC(".IsNull()?Optional<Math::Geometry::Vector2D>(0):"));
				sb->Append(field->name);
				sb->AppendC(UTF8STRC(".OrNull()->Clone()"));
				break;
			case Data::VariItem::ItemType::ByteArr:
			case Data::VariItem::ItemType::UUID:
				sb->AppendChar('\t', tabLev + 1);
				sb->AppendC(UTF8STRC("SDEL_CLASS(this->"));
				sb->Append(field->name);
				sb->AppendC(UTF8STRC(");\r\n"));
				sb->AppendChar('\t', tabLev + 1);
				sb->AppendC(UTF8STRC("this->"));
				sb->Append(field->name);
				sb->AppendC(UTF8STRC(" = "));
				sb->Append(field->name);
				sb->AppendC(UTF8STRC("?"));
				sb->Append(field->name);
				sb->AppendC(UTF8STRC("->Clone():0"));
				break;
			case Data::VariItem::ItemType::CStr:
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
			case Data::VariItem::ItemType::Unknown:
			case Data::VariItem::ItemType::Null:
			default:
				sb->AppendChar('\t', tabLev + 1);
				sb->AppendC(UTF8STRC("this->"));
				sb->Append(field->name);
				sb->AppendC(UTF8STRC(" = "));
				sb->Append(field->name);
				break;
			}
			sb->AppendC(UTF8STRC(";\r\n"));
			sb->AppendChar('\t', tabLev);
			sb->AppendC(UTF8STRC("}\r\n"));
			sb->AppendC(UTF8STRC("\r\n"));
		}
		i++;
	}

	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("NN<Data::NamedClass<"));
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC(">> "));
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("::CreateClass() const\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("{\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("NN<Data::NamedClass<"));
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC(">> cls;\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("NEW_CLASSNN(cls, Data::NamedClass<"));
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC(">(this));\r\n"));
	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItemNoCheck(i);
		if (field->typeName.SetTo(typeName))
		{
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("cls->AddFieldEnum(CSTR(\""));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC("\"), (OSInt)&this->"));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(" - (OSInt)this, CSTR(\""));
			sb->Append(typeName);
			sb->AppendC(UTF8STRC("\"), sizeof("));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC("), "));
			if (field->itemType == Data::VariItem::ItemType::Str)
			{
				sb->AppendC(UTF8STRC("(Data::Class::ByNameFunc)"));
				sb->Append(typeName);
				sb->AppendC(UTF8STRC("FromName);\r\n"));
			}
			else
				sb->AppendC(UTF8STRC("0);\r\n"));
		}
		else
		{
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("CLASS_ADD(cls, "));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(");\r\n"));
		}
		i++;
	}
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("return cls;\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("}\r\n"));
	sb->AppendC(UTF8STRC("\r\n"));

	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("Bool "));
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("::FillFromDBReader(NN<DB::DBReader> r)\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("{\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("NN<Data::NamedClass<"));
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC(">> cls = this->CreateClass();\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("Bool ret = Data::DataModelUtil::FillFromDBReader(cls, this, r);\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("cls.Delete();\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("return ret;\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("}\r\n"));
	sb->AppendC(UTF8STRC("\r\n"));

	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("void "));
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("::DBColList(NN<Data::ArrayListNN<Text::String>> colList)\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("{\r\n"));
	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItemNoCheck(i);
		sb->AppendChar('\t', tabLev + 1);
		sb->AppendC(UTF8STRC("colList->Add(Text::String::New(CSTR(\""));
		Text::JavaText::ToDBName(sb, field->name->v);
		sb->AppendC(UTF8STRC("\")));\r\n"));
		i++;
	}
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("}\r\n"));
}

void Data::Class::ToJavaClass(Text::StringBase<UTF8Char> *clsName, UOSInt tabLev, NN<Text::StringBuilderUTF8> sb)
{
	Data::StringMap<Bool> importMap;
	Text::StringBuilderUTF8 sbCode;
	Text::StringBuilderUTF8 sbConstrHdr;
	Text::StringBuilderUTF8 sbConstrItem;
	Text::StringBuilderUTF8 sbGetterSetter;
	Text::StringBuilderUTF8 sbEquals;
	Text::StringBuilderUTF8 sbHashCode;
	Text::StringBuilderUTF8 sbFieldOrder;
	Text::StringBuilderUTF8 sbTmp;
	importMap.Put(CSTR("javax.persistence.Entity"), true);
	importMap.Put(CSTR("javax.persistence.Table"), true);

	sbCode.AppendC(UTF8STRC("@Entity\r\n"));
	sbCode.AppendC(UTF8STRC("@Table(name="));
	Text::JavaText::ToDBName(sbTmp, clsName->v);
	Text::JSText::ToJSTextDQuote(sbCode, sbTmp.v);
	sbCode.AppendC(UTF8STRC(", schema="));
	Text::JSText::ToJSTextDQuote(sbCode, (const UTF8Char*)"dbo");
	sbCode.AppendC(UTF8STRC(")\r\n"));
	sbCode.AppendC(UTF8STRC("public class "));
	Text::JavaText::ToJavaName(sbCode, clsName->v, true);
	sbCode.AppendC(UTF8STRC("\r\n"));

	sbConstrHdr.AppendC(UTF8STRC("\r\n"));
	sbConstrHdr.AppendC(UTF8STRC("\tpublic "));
	Text::JavaText::ToJavaName(sbConstrHdr, clsName->v, true);
	sbConstrHdr.AppendC(UTF8STRC("() {\r\n"));
	sbConstrHdr.AppendC(UTF8STRC("\t}\r\n"));
	sbConstrHdr.AppendC(UTF8STRC("\r\n"));
	sbConstrHdr.AppendC(UTF8STRC("\tpublic "));
	Text::JavaText::ToJavaName(sbConstrHdr, clsName->v, true);
	sbConstrHdr.AppendUTF8Char('(');

	sbEquals.AppendC(UTF8STRC("\r\n"));
	sbEquals.AppendC(UTF8STRC("\t@Override\r\n"));
	sbEquals.AppendC(UTF8STRC("\tpublic boolean equals(Object o) {\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\tif (o == this)\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\t\treturn true;\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\tif (!(o instanceof "));
	Text::JavaText::ToJavaName(sbEquals, clsName->v, true);
	sbEquals.AppendC(UTF8STRC(")) {\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\t\treturn false;\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\t}\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\t"));
	Text::JavaText::ToJavaName(sbEquals, clsName->v, true);
	sbEquals.AppendUTF8Char(' ');
	Text::JavaText::ToJavaName(sbEquals, clsName->v, false);
	sbEquals.AppendC(UTF8STRC(" = ("));
	Text::JavaText::ToJavaName(sbEquals, clsName->v, true);
	sbEquals.AppendC(UTF8STRC(") o;\r\n"));
	sbEquals.AppendC(UTF8STRC("\t\treturn "));

	sbHashCode.AppendC(UTF8STRC("\r\n"));
	sbHashCode.AppendC(UTF8STRC("\t@Override\r\n"));
	sbHashCode.AppendC(UTF8STRC("\tpublic int hashCode() {\r\n"));
	sbHashCode.AppendC(UTF8STRC("\t\treturn Objects.hash("));

	sbFieldOrder.AppendC(UTF8STRC("\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\t@Override\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\tpublic String toString() {\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\t\treturn DataTools.toObjectString(this);\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\t}\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\tpublic static String[] getFieldOrder() {\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\t\treturn new String[] {\r\n"));

	sbCode.AppendC(UTF8STRC("{\r\n"));
	NN<FieldInfo> field;
	NN<Text::String> typeName;
	Text::CStringNN javaType;
	UOSInt i = 0;
	UOSInt j = this->fields.GetCount();
	while (i < j)
	{
		field = this->fields.GetItemNoCheck(i);
		if (field->typeName.SetTo(typeName))
		{
			if (field->itemType == Data::VariItem::ItemType::Str)
			{
				sbCode.AppendC(UTF8STRC("\t@Enumerated(value = EnumType.STRING)\r\n"));
			}
			else
			{
				sbCode.AppendC(UTF8STRC("\t@Enumerated(value = EnumType.ORDINAL)\r\n"));
			}
			sbCode.AppendC(UTF8STRC("\tprivate "));
			sbCode.Append(typeName);
			javaType = typeName->ToCString();

		}
		else
		{
			sbCode.AppendC(UTF8STRC("\tprivate "));
			if (field->itemType == Data::VariItem::ItemType::Vector)
			{
				importMap.Put(CSTR("org.locationtech.jts.geom.Geometry"), true);
			}
			else if (field->itemType == Data::VariItem::ItemType::Timestamp)
			{
				importMap.Put(CSTR("java.sql.Timestamp"), true);
			}
			else if (field->itemType == Data::VariItem::ItemType::Date)
			{
				importMap.Put(CSTR("java.sql.Date"), true);
			}
			javaType = Text::JavaText::GetJavaTypeName(field->itemType, field->notNull);
			sbCode.Append(javaType);
		}
		sbCode.AppendUTF8Char(' ');
		Text::JavaText::ToJavaName(sbCode, field->name->v, false);
		sbCode.AppendC(UTF8STRC(";\r\n"));

		sbConstrHdr.Append(javaType);
		sbConstrHdr.AppendUTF8Char(' ');
		Text::JavaText::ToJavaName(sbConstrHdr, field->name->v, false);
		if (i + 1 < j)
		{
			sbConstrHdr.AppendC(UTF8STRC(", "));
		}

		sbConstrItem.AppendC(UTF8STRC("\t\tthis."));
		Text::JavaText::ToJavaName(sbConstrItem, field->name->v, false);
		sbConstrItem.AppendC(UTF8STRC(" = "));
		Text::JavaText::ToJavaName(sbConstrItem, field->name->v, false);
		sbConstrItem.AppendC(UTF8STRC(";\r\n"));

		sbGetterSetter.AppendC(UTF8STRC("\r\n"));
		sbGetterSetter.AppendC(UTF8STRC("\tpublic "));
		sbGetterSetter.Append(javaType);
		if (field->itemType == Data::VariItem::ItemType::BOOL)
		{
			sbGetterSetter.AppendC(UTF8STRC(" is"));
		}
		else
		{
			sbGetterSetter.AppendC(UTF8STRC(" get"));
		}
		Text::JavaText::ToJavaName(sbGetterSetter, field->name->v, true);
		sbGetterSetter.AppendC(UTF8STRC("() {\r\n"));
		sbGetterSetter.AppendC(UTF8STRC("\t\treturn this."));
		Text::JavaText::ToJavaName(sbGetterSetter, field->name->v, false);
		sbGetterSetter.AppendC(UTF8STRC(";\r\n"));
		sbGetterSetter.AppendC(UTF8STRC("\t}\r\n"));
		sbGetterSetter.AppendC(UTF8STRC("\r\n"));
		sbGetterSetter.AppendC(UTF8STRC("\tpublic void set"));
		Text::JavaText::ToJavaName(sbGetterSetter, field->name->v, true);
		sbGetterSetter.AppendUTF8Char('(');
		sbGetterSetter.Append(javaType);
		sbGetterSetter.AppendUTF8Char(' ');
		Text::JavaText::ToJavaName(sbGetterSetter, field->name->v, false);
		sbGetterSetter.AppendC(UTF8STRC(") {\r\n"));
		sbGetterSetter.AppendC(UTF8STRC("\t\tthis."));
		Text::JavaText::ToJavaName(sbGetterSetter, field->name->v, false);
		sbGetterSetter.AppendC(UTF8STRC(" = "));
		Text::JavaText::ToJavaName(sbGetterSetter, field->name->v, false);
		sbGetterSetter.AppendC(UTF8STRC(";\r\n"));
		sbGetterSetter.AppendC(UTF8STRC("\t}\r\n"));

		Bool isObj = true;
		if (field->notNull)
		{
			switch (field->itemType)
			{
			case Data::VariItem::ItemType::BOOL:
			case Data::VariItem::ItemType::U8:
			case Data::VariItem::ItemType::I8:
			case Data::VariItem::ItemType::I16:
			case Data::VariItem::ItemType::U16:
			case Data::VariItem::ItemType::U32:
			case Data::VariItem::ItemType::I32:
			case Data::VariItem::ItemType::NI32:
			case Data::VariItem::ItemType::I64:
			case Data::VariItem::ItemType::U64:
			case Data::VariItem::ItemType::F64:
			case Data::VariItem::ItemType::F32:
				isObj = false;
				break;
			case Data::VariItem::ItemType::Str:
			case Data::VariItem::ItemType::CStr:
			case Data::VariItem::ItemType::UUID:
			case Data::VariItem::ItemType::Date:
			case Data::VariItem::ItemType::Timestamp:
			case Data::VariItem::ItemType::ByteArr:
			case Data::VariItem::ItemType::Vector:
			case Data::VariItem::ItemType::Null:
			case Data::VariItem::ItemType::Unknown:
			default:
				break;
			}
		}
		if (isObj)
		{
			sbEquals.AppendC(UTF8STRC("Objects.equals("));
			Text::JavaText::ToJavaName(sbEquals, field->name->v, false);
			sbEquals.AppendC(UTF8STRC(", "));
			Text::JavaText::ToJavaName(sbEquals, clsName->v, false);
			sbEquals.AppendUTF8Char('.');
			Text::JavaText::ToJavaName(sbEquals, field->name->v, false);
			sbEquals.AppendUTF8Char(')');
		}
		else
		{
			Text::JavaText::ToJavaName(sbEquals, field->name->v, false);
			sbEquals.AppendC(UTF8STRC(" == "));
			Text::JavaText::ToJavaName(sbEquals, clsName->v, false);
			sbEquals.AppendUTF8Char('.');
			Text::JavaText::ToJavaName(sbEquals, field->name->v, false);
		}
		if (i + 1 < j)
		{
			sbEquals.AppendC(UTF8STRC(" && "));
		}

		Text::JavaText::ToJavaName(sbHashCode, field->name->v, false);
		if (i + 1 < j)
		{
			sbHashCode.AppendC(UTF8STRC(", "));
		}

		sbFieldOrder.AppendC(UTF8STRC("\t\t\""));
		Text::JavaText::ToJavaName(sbFieldOrder, field->name->v, false);
		if (i + 1 >= j)
		{
			sbFieldOrder.AppendC(UTF8STRC("\"\r\n"));
		}
		else
		{
			sbFieldOrder.AppendC(UTF8STRC("\",\r\n"));
		}
		i++;
	}


	sbConstrHdr.AppendC(UTF8STRC(") {\r\n"));
	sbConstrItem.AppendC(UTF8STRC("\t}\r\n"));
	sbEquals.AppendC(UTF8STRC(";\r\n"));
	sbEquals.AppendC(UTF8STRC("\t}\r\n"));
	sbHashCode.AppendC(UTF8STRC(");\r\n"));
	sbHashCode.AppendC(UTF8STRC("\t}\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\t\t};\r\n"));
	sbFieldOrder.AppendC(UTF8STRC("\t}\r\n"));
	importMap.Put(CSTR("org.sswr.util.data.DataTools"), true);
	importMap.Put(CSTR("java.util.Objects"), true);

	i = 0;
	j = importMap.GetCount();
	while (i < j)
	{
		sb->AppendC(UTF8STRC("import "));
		sb->Append(importMap.GetKey(i));
		sb->AppendC(UTF8STRC(";\r\n"));
		i++;
	}
	sb->AllocLeng(sbCode.GetLength() + sbConstrHdr.GetLength() + sbConstrItem.GetLength() + sbGetterSetter.GetLength() + sbEquals.GetLength() + sbHashCode.GetLength() + sbFieldOrder.GetLength() + 2);
	sb->AppendC(UTF8STRC("\r\n"));
	sb->Append(sbCode.ToCString());
	sb->Append(sbConstrHdr.ToCString());
	sb->Append(sbConstrItem.ToCString());
	sb->Append(sbGetterSetter.ToCString());
	sb->Append(sbEquals.ToCString());
	sb->Append(sbHashCode.ToCString());
	sb->Append(sbFieldOrder.ToCString());
	sb->AppendC(UTF8STRC("}\r\n"));
}

Optional<Data::Class> Data::Class::ParseFromStr(Text::CStringNN str)
{
	Optional<Data::Class> cls = ParseFromCpp(str);
	if (cls.NotNull())
		return cls;
	return 0;
}

Optional<Data::Class> Data::Class::ParseFromCpp(Text::CStringNN str)
{
	NN<Data::Class> cls;
	OSInt ofst = 0;
	NEW_CLASSNN(cls, Data::Class(0))
	Text::StringBuilderUTF8 sb;
	sb.Append(str);
	Text::PString sarr[2];
	sarr[1] = sb;
	Text::PString strType;
	Text::PString strName;
	UOSInt i;
	UOSInt j;
	while (true)
	{
		i = Text::StrSplitLineP(sarr, 2, sarr[1]);
		sarr[0].Trim();
		if (sarr[0].leng > 0)
		{
			if (!sarr[0].EndsWith(';'))
			{
				printf("Not end with ';': %s\r\n", sarr[0].v.Ptr());
				cls.Delete();
				return 0;
			}
			j = sarr[0].IndexOf(' ');
			if (j == INVALID_INDEX)
			{
				printf("Space not found: %s\r\n", sarr[0].v.Ptr());
				cls.Delete();
				return 0;
			}
			strName = sarr[0].SubstrTrim(j + 1);
			strType = sarr[0].SubstrTrim(0, j);
			strName.RemoveChars(1);
			if (strType.Equals(CSTR("Int8")))
			{
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::I8, true);
				ofst += 1;
			}
			else if (strType.Equals(CSTR("UInt8")))
			{
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::U8, true);
				ofst += 1;
			}
			else if (strType.Equals(CSTR("Int16")))
			{
				if (ofst & 1)
				{
					ofst++;
				}
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::I16, true);
				ofst += 2;
			}
			else if (strType.Equals(CSTR("UInt16")))
			{
				if (ofst & 1)
				{
					ofst++;
				}
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::U16, true);
				ofst += 2;
			}
			else if (strType.Equals(CSTR("Int32")))
			{
				if (ofst & 3)
				{
					ofst += 4 - (ofst & 3);
				}
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::I32, true);
				ofst += 4;
			}
			else if (strType.Equals(CSTR("UInt32")))
			{
				if (ofst & 3)
				{
					ofst += 4 - (ofst & 3);
				}
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::U32, true);
				ofst += 4;
			}
			else if (strType.Equals(CSTR("Int64")))
			{
				if (ofst & 7)
				{
					ofst += 8 - (ofst & 7);
				}
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::I64, true);
				ofst += 8;
			}
			else if (strType.Equals(CSTR("UInt64")))
			{
				if (ofst & 7)
				{
					ofst += 8 - (ofst & 7);
				}
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::U64, true);
				ofst += 8;
			}
			else if (strType.Equals(CSTR("Single")))
			{
				if (ofst & 3)
				{
					ofst += 4 - (ofst & 3);
				}
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::F32, true);
				ofst += 4;
			}
			else if (strType.Equals(CSTR("Double")))
			{
				if (ofst & 7)
				{
					ofst += 8 - (ofst & 7);
				}
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::F64, true);
				ofst += 8;
			}
			else if (strType.Equals(CSTR("NN<Text::String>")))
			{
				if (ofst & (_OSINT_SIZE / 8 - 1))
				{
					ofst += (_OSINT_SIZE / 8) - (ofst & (_OSINT_SIZE / 8 - 1));
				}
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::Str, true);
				ofst += (_OSINT_SIZE / 8);
			}
			else if (strType.Equals(CSTR("Optional<Text::String>")))
			{
				if (ofst & (_OSINT_SIZE / 8 - 1))
				{
					ofst += (_OSINT_SIZE / 8) - (ofst & (_OSINT_SIZE / 8 - 1));
				}
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::Str, false);
				ofst += (_OSINT_SIZE / 8);
			}
			else if (strType.Equals(CSTR("Data::Timestamp")))
			{
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::Timestamp, false);
				ofst += 13;
			}
			else if (strType.Equals(CSTR("Data::Date")))
			{
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::Date, false);
				ofst += 8;
			}
			else if (strType.Equals(CSTR("NN<Math::Geometry::Vector2D>")))
			{
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::Vector, true);
				ofst += (_OSINT_SIZE / 8);
			}
			else if (strType.Equals(CSTR("Optional<Math::Geometry::Vector2D>")))
			{
				cls->AddField(strName.ToCString(), ofst, Data::VariItem::ItemType::Vector, false);
				ofst += (_OSINT_SIZE / 8);
			}
			else
			{
				cls->AddFieldEnum(strName.ToCString(), ofst, strType.ToCString(), _OSINT_SIZE / 8, 0);
				ofst += (_OSINT_SIZE / 8);
			}
		}

		if (i != 2)
		{
			break;
		}
	}
	return cls;
}
