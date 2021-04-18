#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/TransferFuncLUT.h"

#if _OSINT_SIZE != 16
Media::CS::TransferFuncLUT::TransferFuncLUT(Media::LUT *lut) : Media::CS::TransferFunc(lut)
{
	this->srcCnt = lut->GetInputLevel();
	this->fwdLUT = MemAlloc(Double, 65536);
	this->invLUT = MemAlloc(Double, this->srcCnt);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	if (lut->GetFormat() == Media::LUT::DF_UINT8)
	{
		Double srcMul = 1 / 255.0;
		Double currV;
		Double valAdd;
		UInt8 lastV = 0;
		UInt8 thisV;
		UInt8 *srcTab = lut->GetTablePtr();
		Double destMul = 1 / (Double)(this->srcCnt - 1);
		currV = 0;
		valAdd = 1 / 65535.0;
		i = 0;
		while (i < this->srcCnt)
		{
			this->invLUT[i] = srcTab[i] * srcMul;
			thisV = srcTab[i];
			j = (UInt16)(lastV | (lastV << 8));
			k = (UInt16)(thisV | (thisV << 8));
			if (k > j)
			{
				currV = Math::UOSInt2Double(i - 1) * destMul;
				valAdd = 1 / (Double)(k - j) * destMul;
				while (j < k)
				{
					this->fwdLUT[j] = currV;

					currV += valAdd;
					j++;
				}
			}

			lastV = thisV;
			i++;
		}
		k = 65536;
		j = (UInt16)(lastV | (lastV << 8));
		while (j < k)
		{
			this->fwdLUT[j] = currV;

			currV += valAdd;
			j++;
		}
	}
	else if (lut->GetFormat() == Media::LUT::DF_UINT16)
	{
		Double srcMul = 1 / 65535.0;
		Double currV;
		Double valAdd;
		UInt16 lastV = 0;
		UInt16 thisV;
		UInt16 *srcTab = (UInt16*)lut->GetTablePtr();
		Double destMul = 1 / (Double)(this->srcCnt - 1);
		currV = 0;
		valAdd = 1 / 65535.0;
		i = 0;
		while (i < this->srcCnt)
		{
			this->invLUT[i] = srcTab[i] * srcMul;
			thisV = srcTab[i];
			j = lastV;
			k = thisV;
			if (k > j)
			{
				currV = Math::UOSInt2Double(i - 1) * destMul;
				valAdd = 1 / (Double)(k - j) * destMul;
				while (j < k)
				{
					this->fwdLUT[j] = currV;

					currV += valAdd;
					j++;
				}
			}

			lastV = thisV;
			i++;
		}
		k = 65536;
		j = lastV;
		while (j < k)
		{
			this->fwdLUT[j] = currV;

			currV += valAdd;
			j++;
		}
	}
	else if (lut->GetFormat() == Media::LUT::DF_SINGLE)
	{
		Double currV;
		Double valAdd;
		Double lastV = 0;
		Double thisV;
		Single *srcTab = (Single*)lut->GetTablePtr();
		Double destMul = 1 / (Double)(this->srcCnt - 1);
		currV = 0;
		valAdd = 1 / 65535.0;
		i = 0;
		while (i < this->srcCnt)
		{
			this->invLUT[i] = srcTab[i];
			thisV = srcTab[i];
			j = (UInt32)(lastV * 65535.0);
			k = (UInt32)(thisV * 65535.0);
			if (k > j)
			{
				currV = Math::UOSInt2Double(i - 1) * destMul;
				valAdd = 1 / (thisV - lastV) * destMul;
				while (j < k)
				{
					this->fwdLUT[j] = currV;

					currV += valAdd;
					j++;
				}
			}

			lastV = thisV;
			i++;
		}
		k = 65536;
		j = (UInt32)(lastV * 65535.0);
		while (j < k)
		{
			this->fwdLUT[j] = currV;

			currV += valAdd;
			j++;
		}
	}
}

Media::CS::TransferFuncLUT::~TransferFuncLUT()
{
	MemFree(this->fwdLUT);
	MemFree(this->invLUT);
}

Double Media::CS::TransferFuncLUT::ForwardTransfer(Double linearVal)
{
	if (linearVal < 0.0)
	{
		Double v1 = this->fwdLUT[0];
		Double v2 = this->fwdLUT[1];
		return (v2 - v1) * linearVal * 65535.0;
	}
	else if (linearVal >= 1.0)
	{
		Double v1 = this->fwdLUT[65534];
		Double v2 = this->fwdLUT[65535];
		return v2 + (v2 - v1) * (linearVal - 1.0) * 65535.0;
	}
	else
	{
		Double v = linearVal * 65535.0;
		Int32 iv = (Int32)v;
		Double v1 = this->fwdLUT[iv];
		Double v2 = this->fwdLUT[iv + 1];
		return v1 + (v2 - v1) * (v - iv);
	}
}

Double Media::CS::TransferFuncLUT::InverseTransfer(Double gammaVal)
{
	if (gammaVal < 0.0)
	{
		Double v1 = this->invLUT[0];
		Double v2 = this->invLUT[1];
		return (v2 - v1) * gammaVal * Math::UOSInt2Double(this->srcCnt - 1);
	}
	else if (gammaVal >= 1.0)
	{
		Double v1 = this->invLUT[this->srcCnt - 2];
		Double v2 = this->invLUT[this->srcCnt - 1];
		return v2 + (v2 - v1) * (gammaVal - 1.0) * Math::UOSInt2Double(this->srcCnt - 1);
	}
	else
	{
		Double v = gammaVal * Math::UOSInt2Double(this->srcCnt - 1);
		Int32 iv = (Int32)v;
		Double v1 = this->invLUT[iv];
		Double v2 = this->invLUT[iv + 1];
		return v1 + (v2 - v1) * (v - iv);
	}
}
#endif
