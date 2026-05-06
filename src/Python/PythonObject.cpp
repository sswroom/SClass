#include "Stdafx.h"
#include "Python/PythonObject.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class Python::PythonObject::ClassData
{
public:
	PyObject *obj;
};

Python::PythonObject::PythonObject(AnyType obj)
{
	this->clsData = MemAllocNN(ClassData);
	this->clsData->obj = (PyObject*)obj.p;
}

Python::PythonObject::~PythonObject()
{
	Py_DECREF(this->clsData->obj);
	MemFreeNN(this->clsData);
}

IntOS Python::PythonObject::GetRefCount() const
{
	return Py_REFCNT(this->clsData->obj);
}

Bool Python::PythonObject::HasAttr(Text::CStringNN name) const
{
	return PyObject_HasAttrString(this->clsData->obj, (const Char*)name.v.Ptr()) != 0;
}

Optional<Python::PythonObject> Python::PythonObject::GetAttr(Text::CStringNN name) const
{
	PyObject *attr = PyObject_GetAttrString(this->clsData->obj, (const Char*)name.v.Ptr());
	if (attr == nullptr)
	{
		PyErr_Clear();
		return nullptr;
	}
	return PythonObject::FromPtr(AnyType(attr));
}

Bool Python::PythonObject::SetAttr(Text::CStringNN name, Optional<PythonObject> obj) const
{
	NN<PythonObject> nnobj;
	PyObject *value = obj.SetTo(nnobj) ? nnobj->clsData->obj : nullptr;
	if (PyObject_SetAttrString(this->clsData->obj, (const Char*)name.v.Ptr(), value) != 0)
	{
		PyErr_Clear();
		return false;
	}
	return true;
}

Optional<Python::PythonObject> Python::PythonObject::FromPtr(AnyType obj)
{
	NN<Python::PythonObject> ret;
	if (obj.NotNull())
	{
		NEW_CLASSNN(ret, PythonObject(obj));
		return ret;
	}
	else
	{
		return nullptr;
	}
}
