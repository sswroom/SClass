#ifndef _SM_MATH_ML_MLLINEARREGRESSION
#define _SM_MATH_ML_MLLINEARREGRESSION
#include "Math/ML/MLModel.h"

namespace Math
{
	namespace ML
	{
		class MLLinearRegression : public MLModel
		{
		private:
			struct ClassData;
			NN<ClassData> data;
		public:
			MLLinearRegression();
			virtual ~MLLinearRegression();

			virtual void Fit(NN<Data::ArrayListNN<Data::ArrayListDbl>> x, NN<Data::ArrayListDbl> y);
			virtual Optional<Data::ArrayListDbl> Predict(NN<Data::ArrayListNN<Data::ArrayListDbl>> x);
		};
	}
}
#endif
