#ifndef _SM_PYTHON_PYTHONSKLEARNLINEARMODEL
#define _SM_PYTHON_PYTHONSKLEARNLINEARMODEL
#include "Python/PythonCore.h"
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListNN.hpp"

namespace Python
{
	class PythonModule;
	class PythonSklearnLinearModel
	{
	private:
		NN<PythonModule> linearModel;
	public:
		PythonSklearnLinearModel(NN<PythonModule> linearModel);
		~PythonSklearnLinearModel();

		void Fit(NN<Data::ArrayListNN<Data::ArrayListDbl>> x, NN<Data::ArrayListDbl> y);
		Optional<Data::ArrayListDbl> Predict(NN<Data::ArrayListNN<Data::ArrayListDbl>> x);
	};
}
#endif