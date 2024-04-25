#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/CSRGB16_LRGBC.h"
#include "Media/CS/TransferFunc.h"

extern "C"
{
	void CSRGB16_LRGBC_Convert(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, UOSInt srcNBits, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
	void CSRGB16_LRGBC_ConvertA2B10G10R10(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, UOSInt srcNBits, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
	void CSRGB16_LRGBC_ConvertFloat(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, UOSInt srcNBits, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
}

void Media::CS::CSRGB16_LRGBC::UpdateRGBTable()
{
	if (this->rgbTable == 0)
	{
		this->rgbTable = MemAllocA(UInt8, 1572864);
	}
	OSInt i;
	Double thisV;
	Double dV;
	UInt16 v[4];
	NN<Media::ColorProfile> srcProfile;
	if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		srcProfile = this->colorSess->GetDefVProfile();
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		srcProfile = this->colorSess->GetDefPProfile();
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
		dV = OSInt2Double(i) / 65535.0;
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

Media::CS::CSRGB16_LRGBC::CSRGB16_LRGBC(UOSInt srcNBits, Media::PixelFormat srcPF, Bool invert, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorManagerSess *colorSess) : Media::CS::CSConverter(colorSess), srcProfile(srcProfile), destProfile(destProfile)
{
	this->srcNBits = srcNBits;
	this->srcPF = srcPF;
	this->invert = invert;

	if (colorSess)
	{
		this->rgbParam.Set(colorSess->GetRGBParam());
	}
	else
	{
		Media::MonitorColorManager::SetDefaultRGB(this->rgbParam);
	}
	this->rgbTable = 0;
	this->rgbUpdated = true;
}

Media::CS::CSRGB16_LRGBC::~CSRGB16_LRGBC()
{
	if (this->rgbTable)
	{
		MemFreeA(this->rgbTable);
		this->rgbTable = 0;
	}
}

void Media::CS::CSRGB16_LRGBC::ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	if (this->rgbUpdated)
	{
		UpdateRGBTable();
		this->rgbUpdated = false;
	}
	if (invert)
	{
		destPtr = ((UInt8*)destPtr) + (OSInt)(srcStoreHeight - 1) * destRGBBpl;
		destRGBBpl = -destRGBBpl;
	}
	if (this->srcPF == Media::PF_LE_A2B10G10R10)
	{
		CSRGB16_LRGBC_ConvertA2B10G10R10(srcPtr[0], destPtr, dispWidth, dispHeight, srcNBits, (OSInt)(srcStoreWidth * srcNBits >> 3), destRGBBpl, this->rgbTable);
	}
	else
	{
		CSRGB16_LRGBC_Convert(srcPtr[0], destPtr, dispWidth, dispHeight, srcNBits, (OSInt)(srcStoreWidth * srcNBits >> 3), destRGBBpl, this->rgbTable);
	}
}

UOSInt Media::CS::CSRGB16_LRGBC::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return width * height * (this->srcNBits >> 3);
}

UOSInt Media::CS::CSRGB16_LRGBC::GetDestFrameSize(UOSInt width, UOSInt height)
{
	return width * height * 8;
}

void Media::CS::CSRGB16_LRGBC::SetPalette(UInt8 *pal)
{
}

void Media::CS::CSRGB16_LRGBC::YUVParamChanged(NN<const YUVPARAM> yuv)
{
}

void Media::CS::CSRGB16_LRGBC::RGBParamChanged(NN<const RGBPARAM2> rgb)
{
	this->rgbParam.Set(rgb);
	this->rgbUpdated = true;
}
