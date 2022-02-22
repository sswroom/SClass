#include "Stdafx.h"
#include "Data/Class.h"
#include "Text/CharUtil.h"
#include "Text/CPPText.h"

void Data::Class::FreeFieldInfo(FieldInfo *field)
{
	field->name->Release();
	MemFree(field);
}

Data::Class::Class(void *refObj)
{
	this->refObj = refObj;
	NEW_CLASS(this->fields, Data::ArrayList<FieldInfo*>());
}

Data::Class::~Class()
{
	LIST_FREE_FUNC(this->fields, FreeFieldInfo);
	DEL_CLASS(this->fields);
}

UOSInt Data::Class::AddField(Text::CString name, OSInt ofst, Data::VariItem::ItemType itemType)
{
	FieldInfo *field = MemAlloc(FieldInfo, 1);
	field->name = Text::String::New(name);
	field->ofst = ofst;
	field->itemType = itemType;
	this->fields->Add(field);
	return Data::VariItem::GetItemSize(itemType);
}

Bool Data::Class::AddField(Text::CString name, UInt8 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::U8) != 0;
}

Bool Data::Class::AddField(Text::CString name, Int8 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::I8) != 0;
}

Bool Data::Class::AddField(Text::CString name, UInt16 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::U16) != 0;
}

Bool Data::Class::AddField(Text::CString name, Int16 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::I16) != 0;
}

Bool Data::Class::AddField(Text::CString name, UInt32 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::U32) != 0;
}

Bool Data::Class::AddField(Text::CString name, Int32 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::I32) != 0;
}

Bool Data::Class::AddField(Text::CString name, UInt64 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::U64) != 0;
}

Bool Data::Class::AddField(Text::CString name, Int64 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::I64) != 0;
}

Bool Data::Class::AddField(Text::CString name, Single *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::F32) != 0;
}

Bool Data::Class::AddField(Text::CString name, Double *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::F64) != 0;
}

Bool Data::Class::AddField(Text::CString name, Text::String **val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::Str) != 0;
}

Bool Data::Class::AddField(Text::CString name, Data::DateTime **val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::Date) != 0;
}

Bool Data::Class::AddField(Text::CString name, Bool *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::BOOL) != 0;
}

Bool Data::Class::AddField(Text::CString name, Data::ReadonlyArray<UInt8> **val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::ByteArr) != 0;
}

Bool Data::Class::AddField(Text::CString name, Math::Vector2D **val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::Vector) != 0;
}

Bool Data::Class::AddField(Text::CString name, Data::UUID **val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::UUID) != 0;
}

UOSInt Data::Class::GetFieldCount()
{
	return this->fields->GetCount();
}

Text::String *Data::Class::GetFieldName(UOSInt index)
{
	FieldInfo *field = this->fields->GetItem(index);
	if (field)
	{
		return field->name;
	}
	return 0;
}

Data::VariItem::ItemType Data::Class::GetFieldType(UOSInt index)
{
	FieldInfo *field = this->fields->GetItem(index);
	if (field)
	{
		return field->itemType;
	}
	return Data::VariItem::ItemType::Unknown;
}

Data::VariItem *Data::Class::GetNewValue(UOSInt index, void *obj)
{
	FieldInfo *field = this->fields->GetItem(index);
	if (field == 0)
	{
		return 0;
	}
	void *valPtr = (void*)(field->ofst + (UInt8*)obj);
	return Data::VariItem::NewFromPtr(valPtr, field->itemType);
}

Bool Data::Class::GetValue(Data::VariItem *itm, UOSInt index, void *obj)
{
	FieldInfo *field = this->fields->GetItem(index);
	if (field == 0)
	{
		return false;
	}
	void *valPtr = (void*)(field->ofst + (UInt8*)obj);
	Data::VariItem::SetFromPtr(itm, valPtr, field->itemType);
	return true;
}

Bool Data::Class::SetField(void *obj, UOSInt index, Data::VariItem *item)
{
	FieldInfo *field = this->fields->GetItem(index);
	if (field == 0 || item == 0)
	{
		return false;
	}
	void *valPtr = (void*)(field->ofst + (OSInt)obj);
	Data::VariItem::SetPtr(valPtr, field->itemType, item);
	return true;
}

Bool Data::Class::SetFieldClearItem(void *obj, UOSInt index, Data::VariItem *item)
{
	FieldInfo *field = this->fields->GetItem(index);
	if (field == 0 || item == 0)
	{
		return false;
	}
	void *valPtr = (void*)(field->ofst + (OSInt)obj);
	Data::VariItem::SetPtrAndNotKeep(valPtr, field->itemType, item);
	return true;
}

Bool Data::Class::Equals(void *obj1, void *obj2)
{
	UOSInt i = this->fields->GetCount();
	FieldInfo *field;
	while (i-- > 0)
	{
		field = this->fields->GetItem(i);
		void *valPtr1 = (void*)(field->ofst + (OSInt)obj1);
		void *valPtr2 = (void*)(field->ofst + (OSInt)obj2);
		if (!Data::VariItem::PtrEquals(valPtr1, valPtr2, field->itemType))
		{
			return false;
		}
	}
	return true;
}

void Data::Class::ToCppClassHeader(Text::StringBase<UTF8Char> *clsName, UOSInt tabLev, Text::StringBuilderUTF8 *sb)
{
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("class "));
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("{\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("private:\r\n"));
	Data::ArrayList<FieldInfo *> *fieldList = this->fields;
	FieldInfo *field;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItem(i);
		sb->AppendChar('\t', tabLev + 1);
		sb->Append(Text::CPPText::GetCppType(field->itemType));
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
		field = fieldList->GetItem(i);
		Text::CString cppType = Text::CPPText::GetCppType(field->itemType);
		sb->AppendChar('\t', tabLev + 1);
		sb->Append(cppType);
		sb->AppendUTF8Char(' ');
		sb->AppendC(UTF8STRC("Get"));
		sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
		sb->AppendC(field->name->v + 1, field->name->leng - 1);
		sb->AppendC(UTF8STRC("();\r\n"));

		sb->AppendChar('\t', tabLev + 1);
		sb->AppendC(UTF8STRC("void Set"));
		sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
		sb->AppendC(field->name->v + 1, field->name->leng - 1);
		sb->AppendUTF8Char('(');
		sb->Append(cppType);
		sb->AppendUTF8Char(' ');
		sb->Append(field->name);
		sb->AppendC(UTF8STRC(");\r\n"));
		i++;
	}

	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("Data::NamedClass<"));
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("> *CreateClass();\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("};\r\n"));
}

void Data::Class::ToCppClassSource(Text::StringBase<UTF8Char> *clsPrefix, Text::StringBase<UTF8Char> *clsName, UOSInt tabLev, Text::StringBuilderUTF8 *sb)
{
	if (clsPrefix == 0)
	{
		clsPrefix = Text::String::NewEmpty();
	}
	Data::ArrayList<FieldInfo *> *fieldList = this->fields;
	FieldInfo *field;
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
		field = fieldList->GetItem(i);
		sb->AppendChar('\t', tabLev + 1);
		sb->AppendC(UTF8STRC("this->"));
		sb->Append(field->name);
		sb->AppendC(UTF8STRC(" = 0;\r\n"));
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
		field = fieldList->GetItem(i);
		switch (field->itemType)
		{
		case Data::VariItem::ItemType::Str:
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("SDEL_STRING(this->"));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(");\r\n"));
			break;
		case Data::VariItem::ItemType::Date:
		case Data::VariItem::ItemType::ByteArr:
		case Data::VariItem::ItemType::Vector:
		case Data::VariItem::ItemType::UUID:
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("SDEL_CLASS(this->"));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(");\r\n"));
			break;
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
		case Data::VariItem::ItemType::Unknown:
		case Data::VariItem::ItemType::Null:
		default:
			break;
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
		field = fieldList->GetItem(i);
		Text::CString cppType = Text::CPPText::GetCppType(field->itemType);
		sb->AppendChar('\t', tabLev);
		sb->Append(cppType);
		sb->AppendUTF8Char(' ');
		sb->Append(clsPrefix);
		sb->Append(clsName);
		sb->AppendC(UTF8STRC("::Get"));
		sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
		sb->AppendC(field->name->v + 1, field->name->leng);
		sb->AppendC(UTF8STRC("()\r\n"));
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
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("SDEL_STRING(this->"));
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
		case Data::VariItem::ItemType::Date:
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("SDEL_CLASS(this->"));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(");\r\n"));
			sb->AppendChar('\t', tabLev + 1);
			sb->AppendC(UTF8STRC("this->"));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC(" = "));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC("?(NEW_CLASS_D(Data::DateTime("));
			sb->Append(field->name);
			sb->AppendC(UTF8STRC("))):0"));
			break;
		case Data::VariItem::ItemType::ByteArr:
		case Data::VariItem::ItemType::Vector:
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
		i++;
	}

	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("Data::NamedClass<"));
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("> *"));
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("::CreateClass()\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("{\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("Data::NamedClass<"));
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC("> *cls;\r\n"));
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("NEW_CLASS(cls, Data::NamedClass<"));
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->AppendC(UTF8STRC(">(this));\r\n"));
	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItem(i);
		sb->AppendChar('\t', tabLev + 1);
		sb->AppendC(UTF8STRC("CLASS_ADD(cls, "));
		sb->Append(field->name);
		sb->AppendC(UTF8STRC(");\r\n"));
		i++;
	}
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendC(UTF8STRC("return cls;\r\n"));
	sb->AppendChar('\t', tabLev);
	sb->AppendC(UTF8STRC("}\r\n"));
}
