#include "Stdafx.h"
#include "Python/PythonUnicode.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class Python::PythonObject::ClassData
{
public:
	PyObject *obj;
};

Python::PythonUnicode::PythonUnicode(AnyType unicode) : PythonObject(unicode)
{
}

Python::PythonUnicode::PythonUnicode(Text::CStringNN str) : PythonObject(PyUnicode_FromStringAndSize((const Char*)str.v.Ptr(), (Py_ssize_t)str.leng))
{
}

Python::PythonUnicode::~PythonUnicode()
{
}

Python::ObjectType Python::PythonUnicode::GetObjectType() const
{
	return ObjectType::Unicode;
}
