#include "Stdafx.h"
#include "Python/PythonCore.h"
#include "Python/PythonModule.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <pyconfig.h>

Python::PythonCore::PythonCore()
{
	Py_InitializeEx(0);
}

Python::PythonCore::~PythonCore()
{
	int ret = Py_FinalizeEx();
	if (ret != 0)
	{
		printf("PythonCore: Finalize return error\r\n");
	}
}

Optional<Python::PythonObject> Python::PythonCore::GetConstNone() const
{
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_NONE));
}

Optional<Python::PythonObject> Python::PythonCore::GetConstFalse() const
{
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_FALSE));
}

Optional<Python::PythonObject> Python::PythonCore::GetConstTrue() const
{
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_TRUE));
}

Optional<Python::PythonObject> Python::PythonCore::GetConstEllipsis() const
{
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_ELLIPSIS));
}

Optional<Python::PythonObject> Python::PythonCore::GetConstNotImplemented() const
{
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_NOT_IMPLEMENTED));
}

Optional<Python::PythonObject> Python::PythonCore::GetConstZero() const
{
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_ZERO));
}

Optional<Python::PythonObject> Python::PythonCore::GetConstOne() const
{
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_ONE));
}

Optional<Python::PythonObject> Python::PythonCore::GetConstEmptyStr() const
{
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_EMPTY_STR));
}

Optional<Python::PythonObject> Python::PythonCore::GetConstEmptyBytes() const
{
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_EMPTY_BYTES));
}

Optional<Python::PythonObject> Python::PythonCore::GetConstEmptyTuple() const
{
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_EMPTY_TUPLE));
}

Int32 Python::PythonCore::RunString(UnsafeArray<const UTF8Char> s)
{
	return PyRun_SimpleStringFlags((const char*)s.Ptr(), nullptr);
}

UnsafeArrayOpt<const UTF8Char> Python::PythonCore::GetVersion() const
{
	return (const UTF8Char*)Py_GetVersion();
}

UnsafeArrayOpt<const UTF8Char> Python::PythonCore::GetPlatform() const
{
	return (const UTF8Char*)Py_GetPlatform();
}
UnsafeArrayOpt<const UTF8Char> Python::PythonCore::GetCopyright() const
{
	return (const UTF8Char*)Py_GetCopyright();
}
UnsafeArrayOpt<const UTF8Char> Python::PythonCore::GetCompiler() const
{
	return (const UTF8Char*)Py_GetCompiler();
}
UnsafeArrayOpt<const UTF8Char> Python::PythonCore::GetBuildInfo() const
{
	return (const UTF8Char*)Py_GetBuildInfo();
}

Optional<Python::PythonModule> Python::PythonCore::ImportModule(UnsafeArray<const UTF8Char> name)
{
	PyObject *mod = PyImport_ImportModule((const char*)name.Ptr());
	if (mod == nullptr)
	{
		PyErr_Print();
		return nullptr;
	}
	NN<PythonModule> ret;
	NEW_CLASSNN(ret, PythonModule(mod));
	return ret;
}
