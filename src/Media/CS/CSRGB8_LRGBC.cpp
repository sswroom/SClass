#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/CS/CSRGB8_LRGBC.h"
#include "Media/CS/TransferFunc.h"

extern "C"
{
	void CSRGB8_LRGBC_UpdateRGBTablePal(UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable, UIntOS nColor);
	void CSRGB8_LRGBC_Convert(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UIntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable);
	void CSRGB8_LRGBC_ConvertW8A8(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UIntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable);
	void CSRGB8_LRGBC_ConvertB5G5R5(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UIntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable);
	void CSRGB8_LRGBC_ConvertB5G6R5(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UIntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable);
	void CSRGB8_LRGBC_ConvertR8G8B8(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UIntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable);
	void CSRGB8_LRGBC_ConvertR8G8B8A8(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UIntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable);
	void CSRGB8_LRGBC_ConvertP1_A1(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UIntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable);
	void CSRGB8_LRGBC_ConvertP2_A1(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UIntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable);
	void CSRGB8_LRGBC_ConvertP4_A1(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UIntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable);
	void CSRGB8_LRGBC_ConvertP8_A1(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, UIntOS srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable);
}

void Media::CS::CSRGB8_LRGBC::UpdateRGBTable()
{
	UnsafeArray<UInt8> rgbTable;
	if (!this->rgbTable.SetTo(rgbTable))
	{
		this->rgbTable = rgbTable = MemAllocAArr(UInt8, 6144);
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
		Media::ColorProfile::GetConvMatrix(mat1, srcProfile->GetPrimaries(), this->rgbParam.monProfile.GetPrimaries());
	}
	else
	{
		Media::ColorProfile::GetConvMatrix(mat1, srcProfile->GetPrimaries(), this->destProfile.GetPrimaries());
	}

	UnsafeArray<Int64> rgbGammaCorr = UnsafeArray<Int64>::ConvertFrom(rgbTable);
	i = 256;
	while (i--)
	{
		dV = IntOS2Double(i) / 255.0;
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
		rgbGammaCorr[i + 256] = *(Int64*)&v[0];

		thisV = btFunc->InverseTransfer(dV);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[2]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[2]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[2]);
		v[3] = 0;
		rgbGammaCorr[i + 512] = *(Int64*)&v[0];
	}
	rtFunc.Delete();
	gtFunc.Delete();
	btFunc.Delete();

	if (this->srcPal.NotNull())
	{
		if (this->srcPF == Media::PF_PAL_1_A1 || this->srcPF == Media::PF_PAL_2_A1 || this->srcPF == Media::PF_PAL_4_A1 || this->srcPF == Media::PF_PAL_8_A1)
		{
			CSRGB8_LRGBC_UpdateRGBTablePal(this->srcPal.Ptr(), this->destPal.Ptr(), rgbTable.Ptr(), ((UIntOS)1 << (this->srcNBits - 1)));
		}
		else
		{
			CSRGB8_LRGBC_UpdateRGBTablePal(this->srcPal.Ptr(), this->destPal.Ptr(), rgbTable.Ptr(), ((UIntOS)1 << this->srcNBits));
		}
	}
}

Media::CS::CSRGB8_LRGBC::CSRGB8_LRGBC(UIntOS srcNBits, Media::PixelFormat srcPF, Bool invert, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess) : Media::CS::CSConverter(colorSess), srcProfile(srcProfile), destProfile(destProfile)
{
	this->srcNBits = srcNBits;
	this->srcPF = srcPF;
	this->invert = invert;
	this->srcPal = nullptr;
	this->destPal = nullptr;

	NN<Media::ColorManagerSess> nncolorSess;
	if (colorSess.SetTo(nncolorSess))
	{
		this->rgbParam.Set(nncolorSess->GetRGBParam());
	}
	else
	{
		Media::MonitorColorManager::SetDefaultRGB(this->rgbParam);
	}
	if (this->srcPF == Media::PF_PAL_1 || this->srcPF == Media::PF_PAL_2 || this->srcPF == Media::PF_PAL_4 || this->srcPF == Media::PF_PAL_8 || this->srcPF == Media::PF_PAL_W1 || this->srcPF == Media::PF_PAL_W2 || this->srcPF == Media::PF_PAL_W4 || this->srcPF == Media::PF_PAL_W8)
	{
		UIntOS colorCnt = ((UIntOS)1 << this->srcNBits);
		this->srcPal = MemAlloc(UInt8, colorCnt * 4);
		this->destPal = MemAlloc(UInt8, colorCnt * 8);
	}
	else if (this->srcPF == Media::PF_PAL_1_A1 || this->srcPF == Media::PF_PAL_2_A1 || this->srcPF == Media::PF_PAL_4_A1 || this->srcPF == Media::PF_PAL_8_A1)
	{
		UIntOS colorCnt = ((UIntOS)1 << (this->srcNBits - 1));
		this->srcPal = MemAlloc(UInt8, colorCnt * 4);
		this->destPal = MemAlloc(UInt8, colorCnt * 8);
	}
	this->rgbTable = nullptr;
	this->rgbUpdated = true;
}

Media::CS::CSRGB8_LRGBC::~CSRGB8_LRGBC()
{
	UnsafeArray<UInt8> rgbTable;
	UnsafeArray<UInt8> srcPal;
	UnsafeArray<UInt8> destPal;
	if (this->rgbTable.SetTo(rgbTable))
	{
		MemFreeAArr(rgbTable);
		this->rgbTable = nullptr;
	}
	if (this->srcPal.SetTo(srcPal))
	{
		MemFreeArr(srcPal);
		this->srcPal = nullptr;
	}
	if (this->destPal.SetTo(destPal))
	{
		MemFreeArr(destPal);
		this->destPal = nullptr;
	}
}

void Media::CS::CSRGB8_LRGBC::ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
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
	if (this->srcPF == Media::PF_W8A8)
	{
		CSRGB8_LRGBC_ConvertW8A8(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, srcNBits, this->srcPal.Ptr(), this->destPal.Ptr(), this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_LE_R5G5B5)
	{
		CSRGB8_LRGBC_ConvertB5G5R5(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, srcNBits, this->srcPal.Ptr(), this->destPal.Ptr(), this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_LE_R5G6B5)
	{
		CSRGB8_LRGBC_ConvertB5G6R5(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, srcNBits, this->srcPal.Ptr(), this->destPal.Ptr(), this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_R8G8B8)
	{
		CSRGB8_LRGBC_ConvertR8G8B8(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, srcNBits, this->srcPal.Ptr(), this->destPal.Ptr(), this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_R8G8B8A8)
	{
		CSRGB8_LRGBC_ConvertR8G8B8A8(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, srcNBits, this->srcPal.Ptr(), this->destPal.Ptr(), this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_LE_R5G6B5)
	{
		CSRGB8_LRGBC_ConvertB5G6R5(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, srcNBits, this->srcPal.Ptr(), this->destPal.Ptr(), this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_PAL_1_A1)
	{
		CSRGB8_LRGBC_ConvertP1_A1(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, srcNBits, this->srcPal.Ptr(), this->destPal.Ptr(), this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_PAL_2_A1)
	{
		CSRGB8_LRGBC_ConvertP2_A1(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, srcNBits, this->srcPal.Ptr(), this->destPal.Ptr(), this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_PAL_4_A1)
	{
		CSRGB8_LRGBC_ConvertP4_A1(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, srcNBits, this->srcPal.Ptr(), this->destPal.Ptr(), this->rgbTable.Ptr());
	}
	else if (this->srcPF == Media::PF_PAL_8_A1)
	{
		CSRGB8_LRGBC_ConvertP8_A1(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, srcNBits, this->srcPal.Ptr(), this->destPal.Ptr(), this->rgbTable.Ptr());
	}
	else
	{
		CSRGB8_LRGBC_Convert(srcPtr[0].Ptr(), destPtr.Ptr(), dispWidth, dispHeight, (IntOS)(srcStoreWidth * srcNBits >> 3), destRGBBpl, srcNBits, this->srcPal.Ptr(), this->destPal.Ptr(), this->rgbTable.Ptr());
	}
}

UIntOS Media::CS::CSRGB8_LRGBC::GetSrcFrameSize(UIntOS width, UIntOS height)
{
	return width * height * (this->srcNBits >> 3);
}

UIntOS Media::CS::CSRGB8_LRGBC::GetDestFrameSize(UIntOS width, UIntOS height)
{
	return width * height * 8;
}

void Media::CS::CSRGB8_LRGBC::SetPalette(UnsafeArray<UInt8> pal)
{
	UnsafeArray<UInt8> srcPal;
	if (this->srcPal.SetTo(srcPal))
	{
		if (this->srcPF == Media::PF_PAL_1_A1 || this->srcPF == Media::PF_PAL_2_A1 || this->srcPF == Media::PF_PAL_4_A1 || this->srcPF == Media::PF_PAL_8_A1)
		{
			UIntOS nColor = (UIntOS)(4 << (this->srcNBits - 1));
			MemCopyNO(srcPal.Ptr(), pal.Ptr(), nColor);
		}
		else
		{
			UIntOS nColor = (UIntOS)(4 << this->srcNBits);
			MemCopyNO(srcPal.Ptr(), pal.Ptr(), nColor);
		}
		this->rgbUpdated = true;
	}
}

void Media::CS::CSRGB8_LRGBC::YUVParamChanged(NN<const YUVPARAM> yuv)
{
}

void Media::CS::CSRGB8_LRGBC::RGBParamChanged(NN<const RGBPARAM2> rgb)
{
	this->rgbParam.Set(rgb);
	this->rgbUpdated = true;
}

