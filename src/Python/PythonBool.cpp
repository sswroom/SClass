#include "Stdafx.h"
#include "Python/PythonBool.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class Python::PythonObject::ClassData
{
public:
	PyObject *obj;
};

Python::PythonBool::PythonBool(AnyType boolObj) : PythonObject(boolObj)
{
}

Python::PythonBool::PythonBool(Bool value) : PythonObject(PyBool_FromLong(value ? 1 : 0))
{
}

Python::PythonBool::~PythonBool()
{
}

Bool Python::PythonBool::GetValue() const
{
	return PyObject_IsTrue(this->clsData->obj) != 0;
}

Python::ObjectType Python::PythonBool::GetObjectType() const
{
	return ObjectType::Boolean;
}

