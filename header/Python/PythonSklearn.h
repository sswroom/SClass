#ifndef _SM_PYTHON_PYTHONSKLEARN
#define _SM_PYTHON_PYTHONSKLEARN
#include "Python/PythonCore.h"
#include "Python/PythonSklearnLinearModel.h"

namespace Python
{
	class PythonSklearn
	{
	private:
		NN<PythonCore> core;
		Optional<PythonModule> sklearn;
		Optional<PythonModule> sklearnLinearModel;
	public:
		PythonSklearn(NN<PythonCore> core);
		~PythonSklearn();

		Optional<PythonSklearnLinearModel> NewLinearRegression() const;
		Optional<PythonModule> GetSklearnModule() const;
	};
}
#endif