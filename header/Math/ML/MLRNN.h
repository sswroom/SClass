#ifndef _SM_MATH_ML_MLRNN
#define _SM_MATH_ML_MLRNN
#include "Math/ML/MLModel.h"

namespace Math
{
	namespace ML
	{
		class MLRNN : public MLModel
		{
		private:
			struct ClassData;
			NN<ClassData> data;
		public:
			MLRNN(UIntOS inputSize, UIntOS hiddenSize, UIntOS outputSize, UIntOS batchSize);
			virtual ~MLRNN();

			virtual void Fit(NN<Data::ArrayListNN<Data::ArrayListDbl>> x, NN<Data::ArrayListDbl> y);
			virtual Optional<Data::ArrayListDbl> Predict(NN<Data::ArrayListNN<Data::ArrayListDbl>> x);
		};
	}
}
#endif
