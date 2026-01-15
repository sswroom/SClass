#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/CS/CSRGB16_LRGBC.h"
#include "Media/CS/TransferFunc.h"

extern "C"
{
	void CSRGB16_LRGBC_ConvertB16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UInt8 *rgbTable);
	void CSRGB16_LRGBC_ConvertR16G16B16A16(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UInt8 *rgbTable);
	void CSRGB16_LRGBC_ConvertB16G16R16(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UInt8 *rgbTable);
	void CSRGB16_LRGBC_ConvertR16G16B16(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UInt8 *rgbTable);
	void CSRGB16_LRGBC_ConvertW16A16(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UInt8 *rgbTable);
	void CSRGB16_LRGBC_ConvertW16(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UInt8 *rgbTable);
	void CSRGB16_LRGBC_ConvertA2B10G10R10(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UInt8 *rgbTable);
	void CSRGB16_LRGBC_ConvertFloat(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, UIntOS srcNBits, IntOS srcRGBBpl, IntOS destRGBBpl, UInt8 *rgbTable);
}

void Media::CS::CSRGB16_LRGBC::UpdateRGBTable()
{
	UnsafeArray<UInt8> rgbTable;
	if (!this->rgbTable.SetTo(rgbTable))
	{
		this->rgbTable = rgbTable = MemAllocAArr(UInt8, 1572864);
	}
	IntOS i;
	Double thisV;
	Double dV;
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

	UnsafeArray<Int64> rgbGammaCorr = UnsafeArray<Int64>::ConvertFrom(rgbTable);
	i = 65536;
	while (i--)
	{
		dV = IntOS2Double(i) / 65535.0;
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

Media::CS::CSRGB16_LRGBC::CSRGB16_LRGBC(UIntOS srcNBits, Media::PixelFormat srcPF, Bool invert, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess) : Media::CS::CSConverter(colorSess), srcProfile(srcProfile), destProfile(destProfile)
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
	this->rgbTable = nullptr;
	this->rgbUpdated = true;
}

Media::CS::CSRGB16_LRGBC::~CSRGB16_LRGBC()
{
	UnsafeArray<UInt8> rgbTable;
	if (this->rgbTable.SetTo(rgbTable))
	{
		MemFreeAArr(rgbTable);
		this->rgbTable = nullptr;
	}
}

void Media::CS::CSRGB16_LRGBC::ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	if (this->rgbUpdated)
	{
		UpdateRGBTable();
		this->rgbUpdated = false;
	}
	if (invert)
	{
		destPtr = destPtr + (IntOS)(srcStoreHeight - 1) * destRGBBpl;
		destRGBBpl = -destRGBBpl;
	}
	if (this->srcPF == Media::PF_LE_A2B10G10R10)
	{
		CSRGB16_LRGBC_ConvertA2B10G10R10(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_LE_R16G16B16A16)
	{
		CSRGB16_LRGBC_ConvertR16G16B16A16(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_LE_B16G16R16)
	{
		CSRGB16_LRGBC_ConvertB16G16R16(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_LE_R16G16B16)
	{
		CSRGB16_LRGBC_ConvertR16G16B16(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_LE_W16A16)
	{
		CSRGB16_LRGBC_ConvertW16A16(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_LE_W16)
	{
		CSRGB16_LRGBC_ConvertW16(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, this->rgbTable.Ptr());
	}
	else
	{
		CSRGB16_LRGBC_ConvertB16G16R16A16(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, this->rgbTable.Ptr());
	}
}

UIntOS Media::CS::CSRGB16_LRGBC::GetSrcFrameSize(UIntOS width, UIntOS height)
{
	return width * height * (this->srcNBits >> 3);
}

UIntOS Media::CS::CSRGB16_LRGBC::GetDestFrameSize(UIntOS width, UIntOS height)
{
	return width * height * 8;
}

void Media::CS::CSRGB16_LRGBC::SetPalette(UnsafeArray<UInt8> pal)
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
