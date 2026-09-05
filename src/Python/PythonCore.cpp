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
#if PY_VERSION_HEX >= 0x030D0000
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_NONE));
#else
	return PythonObject::FromPtr(Py_None);
#endif
}

Optional<Python::PythonObject> Python::PythonCore::GetConstFalse() const
{
#if PY_VERSION_HEX >= 0x030D0000
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_FALSE));
#else
	return PythonObject::FromPtr(Py_False);
#endif
}

Optional<Python::PythonObject> Python::PythonCore::GetConstTrue() const
{
#if PY_VERSION_HEX >= 0x030D0000
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_TRUE));
#else
	return PythonObject::FromPtr(Py_True);
#endif
}

Optional<Python::PythonObject> Python::PythonCore::GetConstEllipsis() const
{
#if PY_VERSION_HEX >= 0x030D0000
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_ELLIPSIS));
#else
	return PythonObject::FromPtr(Py_Ellipsis);
#endif
}

Optional<Python::PythonObject> Python::PythonCore::GetConstNotImplemented() const
{
#if PY_VERSION_HEX >= 0x030D0000
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_NOT_IMPLEMENTED));
#else
	return PythonObject::FromPtr(Py_NotImplemented);
#endif
}

Optional<Python::PythonObject> Python::PythonCore::GetConstZero() const
{
#if PY_VERSION_HEX >= 0x030D0000
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_ZERO));
#else
	return PythonObject::FromPtr(PyLong_FromLong(0));
#endif
}

Optional<Python::PythonObject> Python::PythonCore::GetConstOne() const
{
#if PY_VERSION_HEX >= 0x030D0000
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_ONE));
#else
	return PythonObject::FromPtr(PyLong_FromLong(1));
#endif
}

Optional<Python::PythonObject> Python::PythonCore::GetConstEmptyStr() const
{
#if PY_VERSION_HEX >= 0x030D0000
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_EMPTY_STR));
#else
	return PythonObject::FromPtr(PyUnicode_FromString(""));
#endif
}

Optional<Python::PythonObject> Python::PythonCore::GetConstEmptyBytes() const
{
#if PY_VERSION_HEX >= 0x030D0000
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_EMPTY_BYTES));
#else
	return PythonObject::FromPtr(PyBytes_FromString(""));
#endif
}

Optional<Python::PythonObject> Python::PythonCore::GetConstEmptyTuple() const
{
#if PY_VERSION_HEX >= 0x030D0000
	return PythonObject::FromPtr(Py_GetConstant(Py_CONSTANT_EMPTY_TUPLE));
#else
	return PythonObject::FromPtr(PyTuple_New(0));
#endif
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
