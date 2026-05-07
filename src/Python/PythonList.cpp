#include "Stdafx.h"
#include "Python/PythonList.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class Python::PythonObject::ClassData
{
public:
	PyObject *obj;
};

Python::PythonList::PythonList(AnyType listObj) : PythonObject(listObj)
{
}

Python::PythonList::PythonList() : PythonObject(PyList_New(0))
{
}

Python::PythonList::~PythonList()
{
}

UIntOS Python::PythonList::GetCount() const
{
	return (UIntOS)PyList_Size(this->clsData->obj);
}

Optional<Python::PythonObject> Python::PythonList::GetNewItem(UIntOS index) const
{
	PyObject *item = PyList_GetItem(this->clsData->obj, (Py_ssize_t)index);
	if (item)
	{
		Py_INCREF(item);
		return PythonObject::FromPtr(item);
	}
	PyErr_Clear();
	return nullptr;
}

void Python::PythonList::Append(NN<PythonObject> item) const
{
	PyList_Append(this->clsData->obj, (PyObject*)item->GetObject().p);
}

Python::ObjectType Python::PythonList::GetObjectType() const
{
	return ObjectType::List;
}

