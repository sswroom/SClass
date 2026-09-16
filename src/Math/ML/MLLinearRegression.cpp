#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/ML/MLLinearRegression.h"
#include "Math/ML/MLUtil.h"
#include <mlpack/methods/linear_regression.hpp>

struct Math::ML::MLLinearRegression::ClassData
{
	mlpack::LinearRegression<arma::mat> model;
};

Math::ML::MLLinearRegression::MLLinearRegression()
{
	NEW_CLASSNN(this->data, ClassData());
}

Math::ML::MLLinearRegression::~MLLinearRegression()
{
	this->data.Delete();
}

void Math::ML::MLLinearRegression::Fit(NN<Data::ArrayListNN<Data::ArrayListDbl>> x, NN<Data::ArrayListDbl> y)
{
	arma::mat xMat;
	arma::rowvec yVec;
	Math::ML::MLUtil::SetupMat(xMat, x);
	Math::ML::MLUtil::SetupRowVec(yVec, y);
	this->data->model.Train(xMat, yVec);
}

Optional<Data::ArrayListDbl> Math::ML::MLLinearRegression::Predict(NN<Data::ArrayListNN<Data::ArrayListDbl>> x)
{
	arma::mat xMat;
	Math::ML::MLUtil::SetupMat(xMat, x);
	arma::rowvec yVec;
	this->data->model.Predict(xMat, yVec);
	return Math::ML::MLUtil::FromRowVec(yVec);
}
