#include "Stdafx.h"
#include "Python/PythonDict.h"
#include "Python/PythonModule.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class Python::PythonObject::ClassData
{
public:
	PyObject *obj;
};

Python::PythonModule::PythonModule(AnyType module) : PythonObject(module)
{
}

Python::PythonModule::~PythonModule()
{
}

UnsafeArrayOpt<const UTF8Char> Python::PythonModule::GetName() const
{
	return (const UTF8Char*)PyModule_GetName(this->clsData->obj);
}

Optional<Text::String> Python::PythonModule::GetFileNameNew() const
{
	PyObject* filename_obj = PyModule_GetFilenameObject(this->clsData->obj);
	if (!filename_obj)
	{
		PyErr_Clear();
		return nullptr;
	}
	const char* filename_cstr = PyUnicode_AsUTF8(filename_obj);
	if (!filename_cstr)
	{
		PyErr_Clear();
		Py_DECREF(filename_obj);
		return nullptr;
	}
	NN<Text::String> ret = Text::String::NewNotNullSlow((const UTF8Char*)filename_cstr);
	Py_DECREF(filename_obj);
	return ret;
}

Optional<Python::PythonDict> Python::PythonModule::GetDict() const
{
	PyObject *dict = PyModule_GetDict(this->clsData->obj);
	if (dict)
	{
		Py_INCREF(dict);
		NN<PythonDict> nndict;
		NEW_CLASSNN(nndict, PythonDict(dict));
		return nndict;
	}
	return nullptr;
}

Optional<Python::PythonObject> Python::PythonModule::GetVariable(Text::CStringNN name) const
{
	PyObject *value = PyObject_GetAttrString(this->clsData->obj, (const Char*)name.v.Ptr());
	if (value)
	{
		NN<PythonObject> nnvalue;
		if (PythonObject::FromPtr(value).SetTo(nnvalue))
		{
			return nnvalue;
		}
		Py_DECREF(value);
		return nullptr;
	}
	return nullptr;
}

Python::ObjectType Python::PythonModule::GetObjectType() const
{
	return ObjectType::Module;
}

void Python::PythonModule::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	ToPythonString(sb);
	PyObject *dict = PyModule_GetDict(this->clsData->obj);
	if (dict)
	{
		sb->AppendUTF8Char('{');
		Py_ssize_t pos = 0;
		PyObject *key;
		PyObject *value;
		Bool first = true;
		while (PyDict_Next(dict, &pos, &key, &value))
		{
			if (!first)
			{
				sb->Append(CSTR(",\r\n"));
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
}
