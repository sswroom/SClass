#ifndef _SM_MATH_ML_MLUTIL
#define _SM_MATH_ML_MLUTIL
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListNN.hpp"
#include <mlpack/core/util/log.hpp>
#include <mlpack/methods/linear_regression.hpp>

namespace Math
{
	namespace ML
	{
		class MLUtil
		{
		public:
			static void SetupMat(arma::mat& xMat, NN<Data::ArrayListNN<Data::ArrayListDbl>> x);
			static void SetupRowVec(arma::rowvec& yVec, NN<Data::ArrayListDbl> y);
			static NN<Data::ArrayListDbl> FromRowVec(arma::rowvec& yVec);
		};
	}
}
#endif
