#include "Stdafx.h"
#include "Python/PythonCore.h"

Python::PythonCore::PythonCore()
{
}

Python::PythonCore::~PythonCore()
{
}

Optional<Python::PythonObject> Python::PythonCore::GetConstNone() const
{
	return nullptr;
}

Optional<Python::PythonObject> Python::PythonCore::GetConstFalse() const
{
	return nullptr;
}

Optional<Python::PythonObject> Python::PythonCore::GetConstTrue() const
{
	return nullptr;
}

Optional<Python::PythonObject> Python::PythonCore::GetConstEllipsis() const
{
	return nullptr;
}

Optional<Python::PythonObject> Python::PythonCore::GetConstNotImplemented() const
{
	return nullptr;
}

Optional<Python::PythonObject> Python::PythonCore::GetConstZero() const
{
	return nullptr;
}

Optional<Python::PythonObject> Python::PythonCore::GetConstOne() const
{
	return nullptr;
}

Optional<Python::PythonObject> Python::PythonCore::GetConstEmptyStr() const
{
	return nullptr;
}

Optional<Python::PythonObject> Python::PythonCore::GetConstEmptyBytes() const
{
	return nullptr;
}

Optional<Python::PythonObject> Python::PythonCore::GetConstEmptyTuple() const
{
	return nullptr;
}

Int32 Python::PythonCore::RunString(UnsafeArray<const UTF8Char> s)
{
	return 0;
}

UnsafeArrayOpt<const UTF8Char> Python::PythonCore::GetVersion() const
{
	return nullptr;
}

UnsafeArrayOpt<const UTF8Char> Python::PythonCore::GetPlatform() const
{
	return nullptr;
}
UnsafeArrayOpt<const UTF8Char> Python::PythonCore::GetCopyright() const
{
	return nullptr;
}
UnsafeArrayOpt<const UTF8Char> Python::PythonCore::GetCompiler() const
{
	return nullptr;
}
UnsafeArrayOpt<const UTF8Char> Python::PythonCore::GetBuildInfo() const
{
	return nullptr;
}

Optional<Python::PythonModule> Python::PythonCore::ImportModule(UnsafeArray<const UTF8Char> name)
{
	return nullptr;
}
