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
	Double thisV;
	Double cV;
	UInt16 v[4];
	Media::ColorProfile *srcProfile;
	if (this->srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		srcProfile = this->colorSess->GetDefVProfile();
	}
	else if (this->srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		srcProfile = this->colorSess->GetDefPProfile();
	}
	else if (this->srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || this->srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		srcProfile = this->rgbParam->monProfile;
	}
	else
	{
		srcProfile = this->srcProfile;
	}
	Media::CS::TransferFunc *rtFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetRTranParam());
	Media::CS::TransferFunc *gtFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetGTranParam());
	Media::CS::TransferFunc *btFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetBTranParam());
	Math::Matrix3 mat1;
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Matrix3 mat4;
	Math::Matrix3 mat5;
	Math::Vector3 vec1;
	Math::Vector3 vec2;
	Math::Vector3 vec3;
	this->srcProfile->GetPrimaries()->GetConvMatrix(&mat1);
	if (this->destProfile->GetPrimaries()->colorType == Media::ColorProfile::CT_DISPLAY)
	{
		this->rgbParam->monProfile->GetPrimaries()->GetConvMatrix(&mat5);
		vec2.Set(this->rgbParam->monProfile->GetPrimaries()->wx, this->rgbParam->monProfile->GetPrimaries()->wy, 1.0);
	}
	else
	{
		this->destProfile->GetPrimaries()->GetConvMatrix(&mat5);
		vec2.Set(this->destProfile->GetPrimaries()->wx, this->destProfile->GetPrimaries()->wy, 1.0);
	}
	mat5.Inverse();

	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(&mat2);
	mat3.Set(&mat2);
	mat4.SetIdentity();
	vec1.Set(this->srcProfile->GetPrimaries()->wx, this->srcProfile->GetPrimaries()->wy, 1.0);
	Media::ColorProfile::ColorPrimaries::xyYToXYZ(&vec2, &vec3);
	Media::ColorProfile::ColorPrimaries::xyYToXYZ(&vec1, &vec2);
	mat2.Multiply(&vec2, &vec1);
	mat2.Multiply(&vec3, &vec2);
	mat2.Inverse();
	mat4.vec[0].val[0] = vec2.val[0] / vec1.val[0];
	mat4.vec[1].val[1] = vec2.val[1] / vec1.val[1];
	mat4.vec[2].val[2] = vec2.val[2] / vec1.val[2];
	mat2.Multiply(&mat4);
	mat2.Multiply(&mat3);
	mat1.MyMultiply(&mat2);

	mat1.MyMultiply(&mat5);

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
		thisV = rtFunc->InverseTransfer(iV / 32767.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[0]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[0]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[0]);
		v[3] = 0x3fff;
		rgbGammaCorr[i] = *(Int64*)&v[0];

		thisV = gtFunc->InverseTransfer(iV / 32767.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[1]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[1]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[1]);
		v[3] = 0;
		rgbGammaCorr[i + 65536] = *(Int64*)&v[0];

		thisV = btFunc->InverseTransfer(iV / 32767.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[2]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[2]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[2]);
		v[3] = 0;
		rgbGammaCorr[i + 131072] = *(Int64*)&v[0];
	}
	DEL_CLASS(rtFunc);
	DEL_CLASS(gtFunc);
	DEL_CLASS(btFunc);
}

Media::CS::CSRGBF_LRGBC::CSRGBF_LRGBC(UOSInt srcNBits, Media::PixelFormat srcPF, Bool invert, const Media::ColorProfile *srcProfile, const Media::ColorProfile *destProfile, Media::ColorManagerSess *colorSess) : Media::CS::CSConverter(colorSess)
{
	this->srcNBits = srcNBits;
	this->srcPF = srcPF;
	NEW_CLASS(this->rgbParam, Media::IColorHandler::RGBPARAM2());
	NEW_CLASS(this->srcProfile, Media::ColorProfile(srcProfile));
	NEW_CLASS(this->destProfile, Media::ColorProfile(destProfile));
	this->invert = invert;

	if (colorSess)
	{
		this->rgbParam->Set(colorSess->GetRGBParam());
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
	DEL_CLASS(this->srcProfile);
	DEL_CLASS(this->destProfile);
	DEL_CLASS(this->rgbParam);
}

void Media::CS::CSRGBF_LRGBC::ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	if (this->rgbUpdated)
	{
		UpdateRGBTable();
		this->rgbUpdated = false;
	}
	if (invert)
	{
		destPtr = ((UInt8*)destPtr) + (srcStoreHeight - 1) * destRGBBpl;
		destRGBBpl = -destRGBBpl;
	}
	CSRGBF_LRGBC_Convert(srcPtr[0], destPtr, dispWidth, dispHeight, srcNBits, srcStoreWidth * srcNBits >> 3, destRGBBpl, this->rgbTable);
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

void Media::CS::CSRGBF_LRGBC::YUVParamChanged(const YUVPARAM *yuv)
{
}

void Media::CS::CSRGBF_LRGBC::RGBParamChanged(const RGBPARAM2 *rgb)
{
	this->rgbParam->Set(rgb);
	this->rgbUpdated = true;
}
