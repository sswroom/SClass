#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncParam1.h"

Media::CS::TransferFuncParam1::TransferFuncParam1(Double *params) : Media::CS::TransferFunc(Media::CS::TRANT_PARAM1, 2.2)
{
	this->param->Set(Media::CS::TRANT_PARAM1, params, 7);
	this->param2 = this->param->params[4] * this->param->params[3] + this->param->params[6];
}

Media::CS::TransferFuncParam1::~TransferFuncParam1()
{
}

Double Media::CS::TransferFuncParam1::ForwardTransfer(Double linearVal)
{
	if (linearVal < this->param2)
	{
		return (linearVal - this->param->params[6]) / this->param->params[3];
	}
	else
	{
		return (Math::Pow(linearVal - this->param->params[5], 1 / this->param->params[0]) - this->param->params[2]) / this->param->params[1];
	}
}

Double Media::CS::TransferFuncParam1::InverseTransfer(Double gammaVal)
{
	if (gammaVal < this->param->params[4])
	{
		return gammaVal * this->param->params[3] + this->param->params[6];
	}
	else
	{
		return Math::Pow(gammaVal * this->param->params[1] + this->param->params[2], this->param->params[0]) + this->param->params[5];
	}
}
