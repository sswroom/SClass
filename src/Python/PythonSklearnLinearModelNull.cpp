#include "Stdafx.h"
#include "Python/PythonSklearnLinearModel.h"

Python::PythonSklearnLinearModel::PythonSklearnLinearModel(NN<PythonModule> linearModel) : linearModel(linearModel)
{
}

Python::PythonSklearnLinearModel::~PythonSklearnLinearModel()
{
	this->linearModel.Delete();
}

void Python::PythonSklearnLinearModel::Fit(NN<Data::ArrayListNN<Data::ArrayListDbl>> x, NN<Data::ArrayListDbl> y)
{
}

Optional<Data::ArrayListDbl> Python::PythonSklearnLinearModel::Predict(NN<Data::ArrayListNN<Data::ArrayListDbl>> x)
{
	return nullptr;
}
