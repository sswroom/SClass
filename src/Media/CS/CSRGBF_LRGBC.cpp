#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/CSRGBF_LRGBC.h"
#include "Media/CS/TransferFunc.h"

extern "C"
{
	void CSRGBF_LRGBC_Convert(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, UOSInt srcNBits, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
}

void Media::CS::CSRGBF_LRGBC::UpdateRGBTable()
{
	if (this->rgbTable == 0)
	{
		this->rgbTable = MemAllocA(UInt8, 1572864);
	}
	OSInt i;
	OSInt iV;
	Double dV;
	Double thisV;
	UInt16 v[4];
	NN<Media::ColorProfile> srcProfile;
	NN<Media::ColorManagerSess> nncolorSess;
	if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN && this->colorSess.SetTo(nncolorSess))
	{
		srcProfile = nncolorSess->GetDefVProfile();
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN && this->colorSess.SetTo(nncolorSess))
	{
		srcProfile = nncolorSess->GetDefPProfile();
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		srcProfile = this->rgbParam.monProfile;
	}
	else
	{
		srcProfile = this->srcProfile;
	}
	NN<Media::CS::TransferFunc> rtFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetRTranParam());
	NN<Media::CS::TransferFunc> gtFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetGTranParam());
	NN<Media::CS::TransferFunc> btFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetBTranParam());
	Math::Matrix3 mat1;
	if (this->destProfile.GetPrimaries()->colorType == Media::ColorProfile::CT_DISPLAY)
	{
		Media::ColorProfile::GetConvMatrix(mat1, this->srcProfile.GetPrimaries(), this->rgbParam.monProfile.GetPrimaries());
	}
	else
	{
		Media::ColorProfile::GetConvMatrix(mat1, this->srcProfile.GetPrimaries(), this->destProfile.GetPrimaries());
	}

	Int64 *rgbGammaCorr = (Int64*)this->rgbTable;
	i = 65536;
	while (i--)
	{
		if (i < 32768)
		{
			iV = i;
		}
		else
		{
			iV = i - 65536;
		}
		dV = OSInt2Double(iV) / 32767.0;
		thisV = rtFunc->InverseTransfer(dV);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[0]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[0]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[0]);
		v[3] = 0x3fff;
		rgbGammaCorr[i] = *(Int64*)&v[0];

		thisV = gtFunc->InverseTransfer(dV);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[1]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[1]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[1]);
		v[3] = 0;
		rgbGammaCorr[i + 65536] = *(Int64*)&v[0];

		thisV = btFunc->InverseTransfer(dV);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[2]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[2]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[2]);
		v[3] = 0;
		rgbGammaCorr[i + 131072] = *(Int64*)&v[0];
	}
	rtFunc.Delete();
	gtFunc.Delete();
	btFunc.Delete();
}

Media::CS::CSRGBF_LRGBC::CSRGBF_LRGBC(UOSInt srcNBits, Media::PixelFormat srcPF, Bool invert, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess) : Media::CS::CSConverter(colorSess), srcProfile(srcProfile), destProfile(destProfile)
{
	this->srcNBits = srcNBits;
	this->srcPF = srcPF;
	this->invert = invert;

	NN<Media::ColorManagerSess> nncolorSess;
	if (colorSess.SetTo(nncolorSess))
	{
		this->rgbParam.Set(nncolorSess->GetRGBParam());
	}
	else
	{
		Media::MonitorColorManager::SetDefaultRGB(this->rgbParam);
	}
	this->rgbTable = 0;
	this->rgbUpdated = true;
}

Media::CS::CSRGBF_LRGBC::~CSRGBF_LRGBC()
{
	if (this->rgbTable)
	{
		MemFreeA(this->rgbTable);
		this->rgbTable = 0;
	}
}

void Media::CS::CSRGBF_LRGBC::ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	if (this->rgbUpdated)
	{
		UpdateRGBTable();
		this->rgbUpdated = false;
	}
	if (invert)
	{
		destPtr = destPtr + (OSInt)(srcStoreHeight - 1) * destRGBBpl;
		destRGBBpl = -destRGBBpl;
	}
	CSRGBF_LRGBC_Convert(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, srcNBits, (OSInt)(srcStoreWidth * srcNBits >> 3), destRGBBpl, this->rgbTable);
}

UOSInt Media::CS::CSRGBF_LRGBC::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return width * height * (this->srcNBits >> 3);
}

UOSInt Media::CS::CSRGBF_LRGBC::GetDestFrameSize(UOSInt width, UOSInt height)
{
	return width * height * 8;
}

void Media::CS::CSRGBF_LRGBC::SetPalette(UInt8 *pal)
{
}

void Media::CS::CSRGBF_LRGBC::YUVParamChanged(NN<const YUVPARAM> yuv)
{
}

void Media::CS::CSRGBF_LRGBC::RGBParamChanged(NN<const RGBPARAM2> rgb)
{
	this->rgbParam.Set(rgb);
	this->rgbUpdated = true;
}
