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
			static void SetupMatInv(arma::mat& xMat, NN<Data::ArrayListNN<Data::ArrayListDbl>> x);
			static void SetupCubeInv(arma::cube& xCube, NN<Data::ArrayListNN<Data::ArrayListDbl>> x);
			static void SetupLastRow(arma::cube& yCube, NN<Data::ArrayListDbl> y);
			static void SetupRowVec(arma::rowvec& yVec, NN<Data::ArrayListDbl> y);
			static NN<Data::ArrayListDbl> FromRowVec(arma::rowvec& yVec);
			static NN<Data::ArrayListDbl> FromLastRow(arma::cube& yCube);
		};
	}
}
#endif
