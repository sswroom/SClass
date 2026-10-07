#include "Stdafx.h"
#include "Python/PythonModule.h"

Python::PythonModule::PythonModule(AnyType module) : PythonObject(module)
{
}

Python::PythonModule::~PythonModule()
{
}

UnsafeArrayOpt<const UTF8Char> Python::PythonModule::GetName() const
{
	return nullptr;
}

Optional<Text::String> Python::PythonModule::GetFileNameNew() const
{
	return nullptr;
}

Optional<Python::PythonDict> Python::PythonModule::GetDict() const
{
	return nullptr;
}

Python::ObjectType Python::PythonModule::GetObjectType() const
{
	return ObjectType::Module;
}

void Python::PythonModule::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	ToPythonString(sb);
}
