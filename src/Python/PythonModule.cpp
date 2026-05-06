#include "Stdafx.h"
#include "Python/PythonModule.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

Python::PythonModule::PythonModule(AnyType module) : PythonObject(module)
{
}

Python::PythonModule::~PythonModule()
{
}
