#include "Stdafx.h"
#include "Python/PythonFloat.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class Python::PythonObject::ClassData
{
public:
	PyObject *obj;
};

Python::PythonFloat::PythonFloat(AnyType floatObj) : PythonObject(floatObj)
{
}

Python::PythonFloat::PythonFloat(Double value) : PythonObject(PyFloat_FromDouble(value))
{
}

Python::PythonFloat::~PythonFloat()
{
}

Double Python::PythonFloat::GetValue() const
{
	return PyFloat_AsDouble(this->clsData->obj);
}

Python::ObjectType Python::PythonFloat::GetObjectType() const
{
	return ObjectType::Float;
}

