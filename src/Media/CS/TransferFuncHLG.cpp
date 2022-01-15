#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncHLG.h"

#define csHLGa (0.17883277)
#define csHLGb (0.28466892)
#define csHLGc (0.55991073)

Media::CS::TransferFuncHLG::TransferFuncHLG() : Media::CS::TransferFunc(Media::CS::TRANT_HLG, 2.2)
{
}

Media::CS::TransferFuncHLG::~TransferFuncHLG()
{
}

Double Media::CS::TransferFuncHLG::ForwardTransfer(Double linearVal)
{
	if (linearVal <= (1 / 12.0))
	{
		return Math_Sqrt(3 * linearVal);
	}
	else
	{
		return csHLGc + csHLGa * Math_Ln(12 * linearVal - csHLGb);
	}
}

Double Media::CS::TransferFuncHLG::InverseTransfer(Double gammaVal)
{
	if (gammaVal <= 0.5)
	{
		return (gammaVal * gammaVal) / 3.0;
	}
	else
	{
		return (Math_Exp((gammaVal - csHLGc) / csHLGa) + csHLGb) / 12.0;
	}
}
