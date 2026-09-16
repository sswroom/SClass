#ifndef _SM_MATH_ML_MACHINELEARNING
#define _SM_MATH_ML_MACHINELEARNING
#include "Math/ML/MLModel.h"

namespace Math
{
	namespace ML
	{
		class MachineLearning
		{
		public:
			static NN<MLModel> CreateLinearRegression();
		};
	}
}
#endif
