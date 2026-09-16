#ifndef _SM_MATH_ML_MLMODEL
#define _SM_MATH_ML_MLMODEL
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListNN.hpp"

namespace Math
{
	namespace ML
	{
		class MLModel
		{
		public:
			virtual ~MLModel() {}

			virtual void Fit(NN<Data::ArrayListNN<Data::ArrayListDbl>> x, NN<Data::ArrayListDbl> y) = 0;
			virtual Optional<Data::ArrayListDbl> Predict(NN<Data::ArrayListNN<Data::ArrayListDbl>> x) = 0;
		};
	}
}
#endif
