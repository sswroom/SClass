#include "Stdafx.h"
#include "Python/PythonSklearn.h"

Python::PythonSklearn::PythonSklearn(NN<PythonCore> core) : core(core)
{
	this->sklearn = nullptr;
	this->sklearnLinearModel = nullptr;
}

Python::PythonSklearn::~PythonSklearn()
{
	this->sklearn.Delete();
	this->sklearnLinearModel.Delete();
}

Optional<Python::PythonSklearnLinearModel> Python::PythonSklearn::NewLinearRegression() const
{
	return nullptr;
}

Optional<Python::PythonModule> Python::PythonSklearn::GetSklearnModule() const
{
	return nullptr;
}
