#include "Stdafx.h"
#include "Python/PythonDict.h"

Python::PythonDict::PythonDict(AnyType dict) : PythonObject(dict)
{
}

Python::PythonDict::~PythonDict()
{
}

UnsafeArrayOpt<const UTF8Char> Python::PythonDict::GetKey(UIntOS index) const
{
	return nullptr;
}

Optional<Python::PythonObject> Python::PythonDict::GetValue(UIntOS index) const
{
	return nullptr;
}

Python::ObjectType Python::PythonDict::GetObjectType() const
{
	return ObjectType::Dict;
}

void Python::PythonDict::ToString(NN<Text::StringBuilderUTF8> sb) const
{
}
