#include "Stdafx.h"
#include "Python/PythonDict.h"
#include "Python/PythonFloat.h"
#include "Python/PythonFunction.h"
#include "Python/PythonModule.h"
#include "Python/PythonObject.h"
#include "Python/PythonType.h"
#include "Python/PythonUnicode.h"

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

AnyType Python::PythonObject::GetObject() const
{
	return this->clsData->obj;
}

Python::ObjectType Python::PythonObject::GetObjectType() const
{
	return ObjectType::Unknown;
}

void Python::PythonObject::ToPythonString(NN<Text::StringBuilderUTF8> sb) const
{
	PyObject *repr = PyObject_Repr(this->clsData->obj);
	if (repr)
	{
		Py_ssize_t size;
		const char *str = PyUnicode_AsUTF8AndSize(repr, &size);
		if (str)
		{
			sb->AppendC((const UTF8Char*)str, (UIntOS)size);
		}
		Py_DECREF(repr);
	}
}

void Python::PythonObject::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	ToPythonString(sb);
}

Optional<Python::PythonObject> Python::PythonObject::FromPtr(AnyType obj)
{
	NN<Python::PythonObject> ret;
	if (obj.NotNull())
	{
		if (PyUnicode_Check((PyObject*)obj.p))
		{
			NEW_CLASSNN(ret, PythonUnicode(obj));
			return ret;
		}
		else if (PyFloat_Check((PyObject*)obj.p))
		{
			NEW_CLASSNN(ret, PythonFloat(obj));
			return ret;
		}
		else if (PyModule_Check((PyObject*)obj.p))
		{
			NEW_CLASSNN(ret, PythonModule(obj));
			return ret;
		}
		else if (PyDict_Check((PyObject*)obj.p))
		{
			NEW_CLASSNN(ret, PythonDict(obj));
			return ret;
		}
		else if (PyType_Check((PyObject*)obj.p))
		{
			NEW_CLASSNN(ret, PythonType(obj));
			return ret;
		}
		else if (PyFunction_Check((PyObject*)obj.p))
		{
			NEW_CLASSNN(ret, PythonFunction(obj));
			return ret;
		}
		else
		{
			PyObject *type = PyObject_Type((PyObject*)obj.p);
			if (type)
			{
				PyObject *name = PyType_GetName((PyTypeObject*)type);
				printf("Object Type: %s\r\n", PyUnicode_AsUTF8(name));
				Py_DECREF(name);
				Py_DECREF(type);
				NEW_CLASSNN(ret, PythonObject(obj));
				return ret;
			}
			else
			{
				printf("Object Type: NULL\r\n");
				PyErr_Clear();
				NEW_CLASSNN(ret, PythonObject(obj));
				return ret;
			}
		}
	}
	else
	{
		return nullptr;
	}
}

Text::CStringNN Python::ObjectTypeGetName(Python::ObjectType objType)
{
	switch (objType)
	{
	case Python::ObjectType::Unicode:
		return CSTR("Unicode");
	case Python::ObjectType::Float:
		return CSTR("Float");
	case Python::ObjectType::Module:
		return CSTR("Module");
	case Python::ObjectType::Dict:
		return CSTR("Dict");
	case Python::ObjectType::Type:
		return CSTR("Type");
	case Python::ObjectType::Function:
		return CSTR("Function");
	case Python::ObjectType::Unknown:
	default:
		return CSTR("Unknown");
	}
}
