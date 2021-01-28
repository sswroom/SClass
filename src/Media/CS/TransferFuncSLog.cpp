#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncSLog.h"

//https://pro.sony.com/bbsccms/assets/files/mkt/cinema/solutions/slog_manual.pdf

Media::CS::TransferFuncSLog::TransferFuncSLog() : Media::CS::TransferFunc(Media::CS::TRANT_SLOG, 2.2)
{
}

Media::CS::TransferFuncSLog::~TransferFuncSLog()
{
}

Double Media::CS::TransferFuncSLog::ForwardTransfer(Double linearVal)
{
	return (0.432699 * Math::Log10(linearVal + 0.037584) + 0.616596) + 0.03;
}

Double Media::CS::TransferFuncSLog::InverseTransfer(Double gammaVal)
{
	return Math::Pow(10, ((gammaVal - 0.616596 - 0.03) / 0.432699)) - 0.037584;
}
