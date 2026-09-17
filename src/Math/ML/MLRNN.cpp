#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/ML/MLRNN.h"
#include "Math/ML/MLUtil.h"
#include <mlpack/core.hpp>
#include <mlpack/methods/ann/rnn.hpp>
#include <mlpack/methods/ann/layer/layer.hpp>
#include <mlpack/methods/ann/loss_functions/mean_squared_error.hpp> 

struct Math::ML::MLRNN::ClassData
{
	mlpack::RNN<mlpack::MeanSquaredError> model;
	ens::Adam optimizer;
	UIntOS inputSize;
	UIntOS hiddenSize;
	UIntOS outputSize;
	UIntOS batchSize;
};

Math::ML::MLRNN::MLRNN(UIntOS inputSize, UIntOS hiddenSize, UIntOS outputSize, UIntOS batchSize)
{
	UIntOS effectiveHidden = hiddenSize;
	if (effectiveHidden < 8)
	{
		effectiveHidden = 8;
	}
	if (effectiveHidden > 512)
	{
		effectiveHidden = 512;
	}
	NEW_CLASSNN(this->data, ClassData());
	this->data->model = mlpack::RNN<mlpack::MeanSquaredError>(batchSize, true);
	this->data->optimizer = ens::Adam(0.001, 1, 1e-8, 10);
	this->data->inputSize = inputSize;
	this->data->hiddenSize = effectiveHidden;
	this->data->outputSize = outputSize;
	this->data->batchSize = batchSize;
	this->data->model.Add<mlpack::Linear<>>(effectiveHidden);
	this->data->model.Add<mlpack::LSTM<>>(effectiveHidden);
	this->data->model.Add<mlpack::Linear<>>(outputSize);
}

Math::ML::MLRNN::~MLRNN()
{
	this->data.Delete();
}

void Math::ML::MLRNN::Fit(NN<Data::ArrayListNN<Data::ArrayListDbl>> x, NN<Data::ArrayListDbl> y)
{
	arma::cube xData;
	arma::cube yData;
	Math::ML::MLUtil::SetupCubeInv(xData, x);
	Math::ML::MLUtil::SetupLastRow(yData, y);
	this->data->model.Train(xData, yData, this->data->optimizer);
}

Optional<Data::ArrayListDbl> Math::ML::MLRNN::Predict(NN<Data::ArrayListNN<Data::ArrayListDbl>> x)
{
	arma::cube xData;
	Math::ML::MLUtil::SetupCubeInv(xData, x);
	arma::cube yData(this->data->outputSize, 1, 1);
	this->data->model.Predict(xData, yData);
	return Math::ML::MLUtil::FromLastRow(yData);
}
