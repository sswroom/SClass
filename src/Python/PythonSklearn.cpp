#include "Stdafx.h"
#include "Python/PythonModule.h"
#include "Python/PythonSklearn.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

Python::PythonSklearn::PythonSklearn(NN<PythonCore> core) : core(core)
{
	this->sklearn = core->ImportModule(U8STR("sklearn"));
	this->sklearnLinearModel = core->ImportModule(U8STR("sklearn.linear_model"));
}

Python::PythonSklearn::~PythonSklearn()
{
	this->sklearn.Delete();
	this->sklearnLinearModel.Delete();
}

Optional<Python::PythonSklearnLinearModel> Python::PythonSklearn::NewLinearRegression() const
{
	NN<PythonModule> linearModelModule;
	if (!this->sklearnLinearModel.SetTo(linearModelModule))
	{
		return nullptr;
	}

	Optional<PythonObject> linearRegression = linearModelModule->GetVariable(CSTR("LinearRegression"));
	NN<PythonObject> linearRegressionObj;
	if (!linearRegression.SetTo(linearRegressionObj))
	{
		return nullptr;
	}
	PyObject *model = PyObject_CallObject((PyObject*)linearRegressionObj->GetObject().p, nullptr);
	if (model == nullptr)
	{
		PyErr_Print();
		return nullptr;
	}

	NN<PythonModule> modelModule;
	NEW_CLASSNN(modelModule, PythonModule(model));
	NN<PythonSklearnLinearModel> ret;
	NEW_CLASSNN(ret, PythonSklearnLinearModel(modelModule));
	return ret;
}

Optional<Python::PythonModule> Python::PythonSklearn::GetSklearnModule() const
{
	return this->sklearn;
}
