#include "Stdafx.h"
#include "Python/PythonModule.h"
#include "Python/PythonSklearnLinearModel.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

Python::PythonSklearnLinearModel::PythonSklearnLinearModel(NN<PythonModule> linearModel) : linearModel(linearModel)
{
}

Python::PythonSklearnLinearModel::~PythonSklearnLinearModel()
{
	this->linearModel.Delete();
}

static PyObject *PythonSklearnLinearModel_CreateFloatMatrixInv(NN<Data::ArrayListNN<Data::ArrayListDbl>> values)
{
	if (values->GetCount() == 0)
	{
		return nullptr;
	}
	PyObject *matrix = PyList_New((Py_ssize_t)values->GetItemNoCheck(0)->GetCount());
	if (matrix == nullptr)
	{
		return nullptr;
	}
	UIntOS i = 0;
	UIntOS i2 = values->GetItemNoCheck(0)->GetCount();
	while (i < i2)
	{
		PyObject *pyRow = PyList_New((Py_ssize_t)values->GetCount());
		if (pyRow == nullptr)
		{
			Py_DECREF(matrix);
			return nullptr;
		}
		UIntOS j = 0;
		UIntOS j2 = values->GetCount();
		while (j < j2)
		{
			PyList_SET_ITEM(pyRow, (Py_ssize_t)j, PyFloat_FromDouble(values->GetItemNoCheck(j)->GetItem(i)));
			j++;
		}
		PyList_SET_ITEM(matrix, (Py_ssize_t)i, pyRow);
		i++;
	}
	return matrix;
}

void Python::PythonSklearnLinearModel::Fit(NN<Data::ArrayListNN<Data::ArrayListDbl>> x, NN<Data::ArrayListDbl> y)
{
	PyObject *pyX = PythonSklearnLinearModel_CreateFloatMatrixInv(x);
	PyObject *pyY = PyList_New((Py_ssize_t)y->GetCount());
	if (pyX == nullptr || pyY == nullptr)
	{
		Py_XDECREF(pyX);
		Py_XDECREF(pyY);
		return;
	}
	UIntOS i = 0;
	while (i < y->GetCount())
	{
		PyList_SET_ITEM(pyY, (Py_ssize_t)i, PyFloat_FromDouble(y->GetItem(i)));
		i++;
	}

	Optional<PythonObject> fit = this->linearModel->GetVariable(CSTR("fit"));
	NN<PythonObject> fitFunc;
	if (fit.SetTo(fitFunc))
	{
		PyObject *ret = PyObject_CallFunctionObjArgs((PyObject*)fitFunc->GetObject().p, pyX, pyY, nullptr);
		if (ret == nullptr)
		{
			PyErr_Print();
		}
		else
		{
			Py_DECREF(ret);
		}
		fitFunc.Delete();
	}
	Py_DECREF(pyX);
	Py_DECREF(pyY);
}

Optional<Data::ArrayListDbl> Python::PythonSklearnLinearModel::Predict(NN<Data::ArrayListNN<Data::ArrayListDbl>> x)
{
	PyObject *pyX = PythonSklearnLinearModel_CreateFloatMatrixInv(x);
	if (pyX == nullptr)
	{
		return nullptr;
	}
	Optional<PythonObject> predict = this->linearModel->GetVariable(CSTR("predict"));
	NN<PythonObject> predictFunc;
	if (!predict.SetTo(predictFunc))
	{
		Py_DECREF(pyX);
		return nullptr;
	}
	PyObject *result = PyObject_CallFunctionObjArgs((PyObject*)predictFunc->GetObject().p, pyX, nullptr);
	Py_DECREF(pyX);
	if (result == nullptr)
	{
		PyErr_Print();
		return nullptr;
	}
	PyObject *sequence = PySequence_Fast(result, "LinearRegression.predict did not return a sequence");
	Py_DECREF(result);
	if (sequence == nullptr)
	{
		PyErr_Print();
		return nullptr;
	}

	NN<Data::ArrayListDbl> ret;
	NEW_CLASSNN(ret, Data::ArrayListDbl());
	Py_ssize_t i = 0;
	Py_ssize_t count = PySequence_Fast_GET_SIZE(sequence);
	while (i < count)
	{
		PyObject *value = PySequence_Fast_GET_ITEM(sequence, i);
		Double dbl = PyFloat_AsDouble(value);
		if (PyErr_Occurred())
		{
			PyErr_Print();
			Py_DECREF(sequence);
			return nullptr;
		}
		ret->Add(dbl);
		i++;
	}
	Py_DECREF(sequence);
	return ret;
}
