#include "Stdafx.h"
#include "Python/PythonDict.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class Python::PythonObject::ClassData
{
public:
	PyObject *obj;
};

Python::PythonDict::PythonDict(AnyType dict) : PythonObject(dict)
{
}

Python::PythonDict::~PythonDict()
{
}

UnsafeArrayOpt<const UTF8Char> Python::PythonDict::GetKey(UIntOS index) const
{
	Py_ssize_t pos = (Py_ssize_t)index;
	PyObject *key;
	if (PyDict_Next(this->clsData->obj, &pos, &key, nullptr))
	{
		return (const UTF8Char*)PyUnicode_AsUTF8(key);
	}
	return nullptr;
}

Optional<Python::PythonObject> Python::PythonDict::GetValue(UIntOS index) const
{
	Py_ssize_t pos = (Py_ssize_t)index;
	PyObject *value;
	if (PyDict_Next(this->clsData->obj, &pos, nullptr, &value))
	{
		Py_INCREF(value);
		return PythonObject::FromPtr(value);
	}
	return nullptr;
}

Python::ObjectType Python::PythonDict::GetObjectType() const
{
	return ObjectType::Dict;
}

void Python::PythonDict::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendUTF8Char('{');
	Py_ssize_t pos = 0;
	PyObject *key;
	PyObject *value;
	Bool first = true;
	while (PyDict_Next(this->clsData->obj, &pos, &key, &value))
	{
		if (!first)
		{
			sb->AppendUTF8Char(',');
		}
		first = false;
		Py_ssize_t keySize;
		const char *keyStr = PyUnicode_AsUTF8AndSize(key, &keySize);
		if (keyStr)
		{
			sb->AppendC((const UTF8Char*)keyStr, (UIntOS)keySize);
			sb->AppendUTF8Char(':');
			PyObject *repr = PyObject_Repr(value);
			if (repr)
			{
				Py_ssize_t valueSize;
				const char *valueStr = PyUnicode_AsUTF8AndSize(repr, &valueSize);
				if (valueStr)
				{
					sb->AppendC((const UTF8Char*)valueStr, (UIntOS)valueSize);
				}
				else
				{
					sb->AppendC((const UTF8Char*)"<Error>", 7);
				}
				Py_DECREF(repr);
			}
			else
			{
				sb->AppendC((const UTF8Char*)"<Error>", 7);
			}
		}
		else
		{
			sb->AppendC((const UTF8Char*)"<Non-String Key>", 16);
		}
	}
	sb->AppendUTF8Char('}');
}
