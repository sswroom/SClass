#include "Stdafx.h"
#include "Python/PythonType.h"
#include "Python/PythonUnicode.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class Python::PythonObject::ClassData
{
public:
	PyObject *obj;
};

Python::PythonType::PythonType(AnyType t) : PythonObject(t)
{
}

Python::PythonType::~PythonType()
{
}

Optional<Python::PythonUnicode> Python::PythonType::GetName() const
{
	PyObject *o = PyType_GetName((PyTypeObject*)this->clsData->obj);
	if (o == nullptr)
	{
		PyErr_Clear();
		return nullptr;
	}
	NN<PythonUnicode> ret;
	NEW_CLASSNN(ret, PythonUnicode(o));
	return ret;
}

Python::ObjectType Python::PythonType::GetObjectType() const
{
	return Python::ObjectType::Type;
}

void Python::PythonType::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	ToPythonString(sb);
}
