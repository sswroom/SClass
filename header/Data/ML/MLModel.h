#ifndef _SM_DATA_ML_MLMODEL
#define _SM_DATA_ML_MLMODEL
#include "Data/ArrayList.hpp"
#include "Data/ArrayListArr.hpp"
#include "Text/StringBuilderUTF8.h"
namespace Data
{
	namespace ML
	{
		class MLModel
		{
		public:
			virtual ~MLModel(){}

			virtual void PredictMulti(NN<Data::ArrayListArr<UnsafeArray<Double>>> historyData, UOSInt historySize, NN<Data::ArrayListArr<Double>> predictData) = 0;
			virtual void Predict(UnsafeArray<UnsafeArray<Double>> historyData, UOSInt historySize, NN<Data::ArrayList<Double>> predictData) = 0;
		};
	}
}
#endif
