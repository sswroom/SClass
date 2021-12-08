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
	NEW_CLASS(this->fields, Data::StringMap<FieldInfo*>());
}

Data::Class::~Class()
{
	Data::ArrayList<FieldInfo*> *fieldList = this->fields->GetValues();
	LIST_FREE_FUNC(fieldList, FreeFieldInfo);
	DEL_CLASS(this->fields);
}

UOSInt Data::Class::AddField(const UTF8Char *name, OSInt ofst, Data::VariItem::ItemType itemType)
{
	FieldInfo *field = this->fields->Get(name);
	if (field == 0)
	{
		field = MemAlloc(FieldInfo, 1);
		field->name = Text::String::NewNotNull(name);
		field->ofst = ofst;
		field->itemType = itemType;
		this->fields->Put(field->name, field);
		return Data::VariItem::GetItemSize(itemType);
	}
	else
	{
		return 0;
	}
}

Bool Data::Class::AddField(const UTF8Char *name, UInt8 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::U8) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, Int8 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::I8) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, UInt16 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::U16) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, Int16 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::I16) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, UInt32 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::U32) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, Int32 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::I32) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, UInt64 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::U64) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, Int64 *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::I64) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, Single *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::F32) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, Double *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::F64) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, Text::String **val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::Str) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, Data::DateTime **val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::Date) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, Bool *val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::BOOL) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, Data::ReadonlyArray<UInt8> **val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::ByteArr) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, Math::Vector2D **val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::Vector) != 0;
}

Bool Data::Class::AddField(const UTF8Char *name, Data::UUID **val)
{
	return this->AddField(name, ((UInt8*)val) - (UInt8*)this->refObj, Data::VariItem::ItemType::UUID) != 0;
}

UOSInt Data::Class::GetFieldCount()
{
	return this->fields->GetCount();
}

Text::String *Data::Class::GetFieldName(UOSInt index)
{
	FieldInfo *field = this->fields->GetValues()->GetItem(index);
	if (field)
	{
		return field->name;
	}
	return 0;
}

Data::VariItem::ItemType Data::Class::GetFieldType(UOSInt index)
{
	FieldInfo *field = this->fields->GetValues()->GetItem(index);
	if (field)
	{
		return field->itemType;
	}
	return Data::VariItem::ItemType::Unknown;
}

Data::VariItem *Data::Class::GetNewValue(UOSInt index, void *obj)
{
	FieldInfo *field = this->fields->GetValues()->GetItem(index);
	if (field == 0)
	{
		return 0;
	}
	void *valPtr = (void*)(field->ofst + (UInt8*)obj);
	return Data::VariItem::NewFromPtr(valPtr, field->itemType);
}

Bool Data::Class::GetValue(Data::VariItem *itm, UOSInt index, void *obj)
{
	FieldInfo *field = this->fields->GetValues()->GetItem(index);
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
	FieldInfo *field = this->fields->GetValues()->GetItem(index);
	if (field == 0 || item == 0)
	{
		return false;
	}
	void *valPtr = (void*)(field->ofst + (OSInt)obj);
	Data::VariItem::SetPtr(valPtr, field->itemType, item);
	return true;
}

Bool Data::Class::Equals(void *obj1, void *obj2)
{
	Data::ArrayList<FieldInfo*> *fieldList = this->fields->GetValues();
	UOSInt i = fieldList->GetCount();
	FieldInfo *field;
	while (i-- > 0)
	{
		field = fieldList->GetItem(i);
		void *valPtr1 = (void*)(field->ofst + (OSInt)obj1);
		void *valPtr2 = (void*)(field->ofst + (OSInt)obj2);
		if (!Data::VariItem::PtrEquals(valPtr1, valPtr2, field->itemType))
		{
			return false;
		}
	}
	return true;
}

void Data::Class::ToCppClassHeader(const UTF8Char *clsName, UOSInt tabLev, Text::StringBuilderUTF *sb)
{
	sb->AppendChar('\t', tabLev);
	sb->Append((const UTF8Char*)"class ");
	sb->Append(clsName);
	sb->Append((const UTF8Char*)"\r\n");
	sb->AppendChar('\t', tabLev);
	sb->Append((const UTF8Char*)"{\r\n");
	sb->AppendChar('\t', tabLev);
	sb->Append((const UTF8Char*)"private:\r\n");
	Data::ArrayList<FieldInfo *> *fieldList = this->fields->GetValues();
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
		sb->AppendChar(' ', 1);
		sb->Append(field->name);
		sb->Append((const UTF8Char*)";\r\n");
		i++;
	}
	sb->Append((const UTF8Char*)"\r\n");
	sb->AppendChar('\t', tabLev);
	sb->Append((const UTF8Char*)"public:\r\n");
	sb->AppendChar('\t', tabLev + 1);
	sb->Append(clsName);
	sb->Append((const UTF8Char*)"();\r\n");
	sb->AppendChar('\t', tabLev + 1);
	sb->AppendChar('~', 1);
	sb->Append(clsName);
	sb->Append((const UTF8Char*)"();\r\n");
	sb->Append((const UTF8Char*)"\r\n");
	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItem(i);
		const UTF8Char *cppType = Text::CPPText::GetCppType(field->itemType);
		sb->AppendChar('\t', tabLev + 1);
		sb->Append(cppType);
		sb->AppendChar(' ', 1);
		sb->Append((const UTF8Char*)"Get");
		sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
		sb->Append(field->name->v + 1);
		sb->Append((const UTF8Char*)"();\r\n");

		sb->AppendChar('\t', tabLev + 1);
		sb->Append((const UTF8Char*)"void Set");
		sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
		sb->Append(field->name->v + 1);
		sb->AppendChar('(', 1);
		sb->Append(cppType);
		sb->AppendChar(' ', 1);
		sb->Append(field->name);
		sb->Append((const UTF8Char*)");\r\n");
		i++;
	}

	sb->Append((const UTF8Char*)"\r\n");
	sb->AppendChar('\t', tabLev + 1);
	sb->Append((const UTF8Char*)"Data::NamedClass<");
	sb->Append(clsName);
	sb->Append((const UTF8Char*)"> *CreateClass();\r\n");
	sb->AppendChar('\t', tabLev);
	sb->Append((const UTF8Char*)"};\r\n");
}

void Data::Class::ToCppClassSource(const UTF8Char *clsPrefix, const UTF8Char *clsName, UOSInt tabLev, Text::StringBuilderUTF *sb)
{
	if (clsPrefix == 0)
	{
		clsPrefix = (const UTF8Char*)"";
	}
	Data::ArrayList<FieldInfo *> *fieldList = this->fields->GetValues();
	FieldInfo *field;
	UOSInt i;
	UOSInt j;

	sb->AppendChar('\t', tabLev);
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->Append((const UTF8Char*)"::");
	sb->Append(clsName);
	sb->Append((const UTF8Char*)"()\r\n");
	sb->AppendChar('\t', tabLev);
	sb->Append((const UTF8Char*)"{\r\n");
	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItem(i);
		sb->AppendChar('\t', tabLev + 1);
		sb->Append((const UTF8Char*)"this->");
		sb->Append(field->name);
		sb->Append((const UTF8Char*)" = 0;\r\n");
		i++;
	}
	sb->AppendChar('\t', tabLev);
	sb->Append((const UTF8Char*)"}\r\n");
	sb->Append((const UTF8Char*)"\r\n");

	sb->AppendChar('\t', tabLev);
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->Append((const UTF8Char*)"::~");
	sb->Append(clsName);
	sb->Append((const UTF8Char*)"()\r\n");
	sb->AppendChar('\t', tabLev);
	sb->Append((const UTF8Char*)"{\r\n");
	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItem(i);
		switch (field->itemType)
		{
		case Data::VariItem::ItemType::Str:
			sb->AppendChar('\t', tabLev + 1);
			sb->Append((const UTF8Char*)"SDEL_STRING(this->");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)");\r\n");
			break;
		case Data::VariItem::ItemType::Date:
		case Data::VariItem::ItemType::ByteArr:
		case Data::VariItem::ItemType::Vector:
		case Data::VariItem::ItemType::UUID:
			sb->AppendChar('\t', tabLev + 1);
			sb->Append((const UTF8Char*)"SDEL_CLASS(this->");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)");\r\n");
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
	sb->Append((const UTF8Char*)"}\r\n");
	sb->Append((const UTF8Char*)"\r\n");

	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItem(i);
		const UTF8Char *cppType = Text::CPPText::GetCppType(field->itemType);
		sb->AppendChar('\t', tabLev);
		sb->Append(cppType);
		sb->AppendChar(' ', 1);
		sb->Append(clsPrefix);
		sb->Append(clsName);
		sb->Append((const UTF8Char*)"::Get");
		sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
		sb->Append(field->name->v + 1);
		sb->Append((const UTF8Char*)"()\r\n");
		sb->AppendChar('\t', tabLev);
		sb->Append((const UTF8Char*)"{\r\n");
		sb->AppendChar('\t', tabLev + 1);
		sb->Append((const UTF8Char*)"return this->");
		sb->Append(field->name);
		sb->Append((const UTF8Char*)";\r\n");
		sb->AppendChar('\t', tabLev);
		sb->Append((const UTF8Char*)"}\r\n");
		sb->Append((const UTF8Char*)"\r\n");

		sb->AppendChar('\t', tabLev);
		sb->Append((const UTF8Char*)"void ");
		sb->Append(clsPrefix);
		sb->Append(clsName);
		sb->Append((const UTF8Char*)"::Set");
		sb->AppendChar(Text::CharUtil::ToUpper(field->name->v[0]), 1);
		sb->Append(field->name->v + 1);
		sb->AppendChar('(', 1);
		sb->Append(cppType);
		sb->AppendChar(' ', 1);
		sb->Append(field->name);
		sb->Append((const UTF8Char*)")\r\n");
		sb->AppendChar('\t', tabLev);
		sb->Append((const UTF8Char*)"{\r\n");
		switch (field->itemType)
		{
		case Data::VariItem::ItemType::Str:
			sb->AppendChar('\t', tabLev + 1);
			sb->Append((const UTF8Char*)"SDEL_STRING(this->");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)");\r\n");
			sb->AppendChar('\t', tabLev + 1);
			sb->Append((const UTF8Char*)"this->");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)" = ");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)"?");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)"->Clone():0");
			break;
		case Data::VariItem::ItemType::Date:
			sb->AppendChar('\t', tabLev + 1);
			sb->Append((const UTF8Char*)"SDEL_CLASS(this->");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)");\r\n");
			sb->AppendChar('\t', tabLev + 1);
			sb->Append((const UTF8Char*)"this->");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)" = ");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)"?(NEW_CLASS_D(Data::DateTime(");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)"))):0");
			break;
		case Data::VariItem::ItemType::ByteArr:
		case Data::VariItem::ItemType::Vector:
		case Data::VariItem::ItemType::UUID:
			sb->AppendChar('\t', tabLev + 1);
			sb->Append((const UTF8Char*)"SDEL_CLASS(this->");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)");\r\n");
			sb->AppendChar('\t', tabLev + 1);
			sb->Append((const UTF8Char*)"this->");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)" = ");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)"?");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)"->Clone():0");
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
			sb->Append((const UTF8Char*)"this->");
			sb->Append(field->name);
			sb->Append((const UTF8Char*)" = ");
			sb->Append(field->name);
			break;
		}
		sb->Append((const UTF8Char*)";\r\n");
		sb->AppendChar('\t', tabLev);
		sb->Append((const UTF8Char*)"}\r\n");
		sb->Append((const UTF8Char*)"\r\n");
		i++;
	}

	sb->AppendChar('\t', tabLev);
	sb->Append((const UTF8Char*)"Data::NamedClass<");
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->Append((const UTF8Char*)"> *");
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->Append((const UTF8Char*)"::CreateClass()\r\n");
	sb->AppendChar('\t', tabLev);
	sb->Append((const UTF8Char*)"{\r\n");
	sb->AppendChar('\t', tabLev + 1);
	sb->Append((const UTF8Char*)"Data::NamedClass<");
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->Append((const UTF8Char*)"> *cls;\r\n");
	sb->AppendChar('\t', tabLev + 1);
	sb->Append((const UTF8Char*)"NEW_CLASS(cls, Data::NamedClass<");
	sb->Append(clsPrefix);
	sb->Append(clsName);
	sb->Append((const UTF8Char*)">(this));\r\n");
	i = 0;
	j = fieldList->GetCount();
	while (i < j)
	{
		field = fieldList->GetItem(i);
		sb->AppendChar('\t', tabLev + 1);
		sb->Append((const UTF8Char*)"CLASS_ADD(cls, ");
		sb->Append(field->name);
		sb->Append((const UTF8Char*)");\r\n");
		i++;
	}
	sb->AppendChar('\t', tabLev + 1);
	sb->Append((const UTF8Char*)"return cls;\r\n");
	sb->AppendChar('\t', tabLev);
	sb->Append((const UTF8Char*)"}\r\n");
}
