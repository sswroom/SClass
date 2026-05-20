#include "Stdafx.h"
#include "Python/PythonObject.h"

Python::PythonObject::PythonObject(AnyType obj)
{
}

Python::PythonObject::~PythonObject()
{
}

IntOS Python::PythonObject::GetRefCount() const
{
	return 0;
}

Bool Python::PythonObject::HasAttr(Text::CStringNN name) const
{
	return false;
}

Optional<Python::PythonObject> Python::PythonObject::GetAttr(Text::CStringNN name) const
{
	return nullptr;
}

Bool Python::PythonObject::SetAttr(Text::CStringNN name, Optional<PythonObject> obj) const
{
	return false;
}

AnyType Python::PythonObject::GetObject() const
{
	return nullptr;
}

Python::ObjectType Python::PythonObject::GetObjectType() const
{
	return ObjectType::Unknown;
}

void Python::PythonObject::ToPythonString(NN<Text::StringBuilderUTF8> sb) const
{
}

void Python::PythonObject::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	ToPythonString(sb);
}

Optional<Python::PythonObject> Python::PythonObject::FromPtr(AnyType obj)
{
	return nullptr;
}

Text::CStringNN Python::ObjectTypeGetName(Python::ObjectType objType)
{
	switch (objType)
	{
	case Python::ObjectType::Unicode:
		return CSTR("Unicode");
	case Python::ObjectType::Float:
		return CSTR("Float");
	case Python::ObjectType::Module:
		return CSTR("Module");
	case Python::ObjectType::Dict:
		return CSTR("Dict");
	case Python::ObjectType::Type:
		return CSTR("Type");
	case Python::ObjectType::Function:
		return CSTR("Function");
	case Python::ObjectType::Boolean:
		return CSTR("Bool");
	case Python::ObjectType::List:
		return CSTR("List");
	case Python::ObjectType::Unknown:
	default:
		return CSTR("Unknown");
	}
}
