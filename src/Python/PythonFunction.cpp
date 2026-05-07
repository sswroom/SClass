#include "Stdafx.h"
#include "Python/PythonFunction.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class Python::PythonObject::ClassData
{
public:
	PyObject *obj;
};

Python::PythonFunction::PythonFunction(AnyType function) : PythonObject(function)
{
}

Python::PythonFunction::~PythonFunction()
{
}

Python::ObjectType Python::PythonFunction::GetObjectType() const
{
	return ObjectType::Function;
}

void Python::PythonFunction::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	ToPythonString(sb);
	PyObject *dict = PyFunction_GetDefaults(this->clsData->obj);
	if (dict)
	{
		sb->AppendUTF8Char('{');
		Py_ssize_t pos = 0;
		PyObject *key, *value;
		while (PyDict_Next(dict, &pos, &key, &value))
		{
			if (key && value)
			{
				Py_ssize_t keySize;
				const char *keyStr = PyUnicode_AsUTF8AndSize(key, &keySize);
				if (keyStr)
				{
					sb->AppendC((const UTF8Char*)keyStr, (UIntOS)keySize);
					sb->AppendUTF8Char(':');
					PyObject *repr = PyObject_Repr(value);
					if (repr)
					{
						Py_ssize_t valSize;
						const char *valStr = PyUnicode_AsUTF8AndSize(repr, &valSize);
						if (valStr)
						{
							sb->AppendC((const UTF8Char*)valStr, (UIntOS)valSize);
						}
						Py_DECREF(repr);
					}
					sb->AppendUTF8Char(',');
				}
			}
		}
		sb->AppendUTF8Char('}');
		Py_DECREF(dict);
	}
}
