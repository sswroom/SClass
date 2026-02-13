#include "Stdafx.h"
#include "Python/PythonCore.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <pyconfig.h>

Python::PythonCore::PythonCore()
{
	Py_InitializeEx(0);
}

Python::PythonCore::~PythonCore()
{
	int ret = Py_FinalizeEx();
	if (ret != 0)
	{
		printf("PythonCore: Finalize return error\r\n");
	}
}


UnsafeArrayOpt<const WChar> Python::PythonCore::GetProgramName() const
{
	return Py_GetProgramName();
}
