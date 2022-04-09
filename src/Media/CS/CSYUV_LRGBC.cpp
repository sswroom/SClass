#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSYUV_LRGBC.h"
#include "Math/Math.h"

void Media::CS::CSYUV_LRGBC::SetupRGB13_LR()
{
	Int32 i;
	Double thisV;
	UInt16 v[4];

	Media::ColorProfile *srcProfile;
	if (this->colorSess == 0)
	{
		srcProfile = &this->srcProfile;		
		if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN || this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
		{
			this->srcProfile.SetCommonProfile(Media::ColorProfile::CPT_SRGB);
		}
		else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN || this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY)
		{
			this->srcProfile.SetCommonProfile(Media::ColorProfile::CPT_BT709);
		}
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		srcProfile = this->colorSess->GetDefPProfile();
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		srcProfile = this->colorSess->GetDefVProfile();
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY)
	{
		srcProfile = this->colorSess->GetDefVProfile();
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		srcProfile = this->colorSess->GetDefPProfile();
	}
	else
	{
		srcProfile = &this->srcProfile;
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
	this->srcProfile.GetPrimaries()->GetConvMatrix(&mat1);
	if (this->destProfile.GetPrimaries()->colorType == Media::ColorProfile::CT_DISPLAY)
	{
		this->rgbParam.monProfile.GetPrimaries()->GetConvMatrix(&mat5);
		vec2.Set(this->rgbParam.monProfile.GetPrimaries()->wx, this->rgbParam.monProfile.GetPrimaries()->wy, 1.0);
	}
	else
	{
		this->destProfile.GetPrimaries()->GetConvMatrix(&mat5);
		vec2.Set(this->destProfile.GetPrimaries()->wx, this->destProfile.GetPrimaries()->wy, 1.0);
	}
	mat5.Inverse();

	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(&mat2);
	mat3.Set(&mat2);
	mat4.SetIdentity();
	vec1.Set(this->srcProfile.GetPrimaries()->wx, this->srcProfile.GetPrimaries()->wy, 1.0);
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

	i = 32768;
	while (i--)
	{
		thisV = rtFunc->InverseTransfer(i / 32767.0 * 4.0);
		if (thisV > 2.0)
		{
			thisV = 2.0;
		}
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[0]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[0]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[0]);
		v[3] = 0;
		this->rgbGammaCorr[i] = *(Int64*)&v[0];

		thisV = gtFunc->InverseTransfer(i / 32767.0 * 4.0);
		if (thisV > 2.0)
		{
			thisV = 2.0;
		}
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[1]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[1]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[1]);
		v[3] = 0;
		this->rgbGammaCorr[i + 65536] = *(Int64*)&v[0];

		thisV = btFunc->InverseTransfer(i / 32767.0 * 4.0);
		if (thisV > 2.0)
		{
			thisV = 2.0;
		}
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[2]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[2]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[2]);
		v[3] = 0;
		this->rgbGammaCorr[i + 131072] = *(Int64*)&v[0];
	}
	i = 65536;
	while (i-- > 32768)
	{
		thisV = rtFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0);
		if (thisV < -2.0)
		{
			thisV = -2.0;
		}
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[0]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[0]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[0]);
		v[3] = 0;
		this->rgbGammaCorr[i] = *(Int64*)&v[0];

		thisV = gtFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0);
		if (thisV < -2.0)
		{
			thisV = -2.0;
		}
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[1]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[1]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[1]);
		v[3] = 0;
		this->rgbGammaCorr[i + 65536] = *(Int64*)&v[0];

		thisV = btFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0);
		if (thisV < -2.0)
		{
			thisV = -2.0;
		}
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[2]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[2]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[2]);
		v[3] = 0;
		this->rgbGammaCorr[i + 131072] = *(Int64*)&v[0];
	}
	DEL_CLASS(btFunc);
	DEL_CLASS(gtFunc);
	DEL_CLASS(rtFunc);
}

void Media::CS::CSYUV_LRGBC::SetupYUV_RGB13()
{
	Double c;

	int i;

	Int32 y;

	Double Kr;
	Double Kb;
	Double Kg;
	Double Kc1;
	Double Kc2;
	Double Kc3;
	Double Kc4;

	Media::ColorProfile::YUVType yuvType;
	Bool fullRange = (this->yuvType & Media::ColorProfile::YUVT_FLAG_YUV_0_255) != 0;
	if ((this->yuvType & Media::ColorProfile::YUVT_MASK) == Media::ColorProfile::YUVT_UNKNOWN)
	{
		yuvType = this->colorSess->GetDefYUVType();
	}
	else
	{
		yuvType = (Media::ColorProfile::YUVType)(this->yuvType & Media::ColorProfile::YUVT_MASK);
	}
	Media::ColorProfile::GetYUVConstants(yuvType, &Kr, &Kb);
	Kg = 1 - Kr - Kb;
	Kc1 = (1 - Kr) / 0.5;
	Kc2 = -(2 * Kr - 2 * Kr * Kr) / Kg;
	Kc3 = -(2 * Kb - 2 * Kb * Kb) / Kg;
	Kc4 = (1 - Kb) / 0.5;

	if (fullRange)
	{
		i = 256;
		while (i--)
		{
			y = Double2Int32(0x7fff * (Math_Pow(i / 255.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
			if (y < -0x8000)
				y = 0x8000;
			else if (y > 0x7fff)
				y = 0x7fff;
			else
				y = y & 0xffff;
			yuv2rgb[i] = (((Int64)y) << 32) | (((Int64)y) << 16) | y;
		}
	}
	else
	{
		i = 256;
		while (i--)
		{
			y = Double2Int32(0x7fff * (Math_Pow((i - 16) / 219.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
			if (y < -0x8000)
				y = 0x8000;
			else if (y > 0x7fff)
				y = 0x7fff;
			else
				y = y & 0xffff;
			yuv2rgb[i] = (((Int64)y) << 32) | (((Int64)y) << 16) | y;
		}
	}

	i = 256;
	while (i--)
	{
		if (i == 0)
			i = 0;

		if (fullRange)
		{
			if (i > 128)
				c = Math_Pow((i - 128) / 127.0, this->yuvParam.CGamma) * 16383;
			else if (i == 128)
				c = 0;
			else
				c = Math_Pow((-i + 128) / 127.0, this->yuvParam.CGamma) * (-16383);
		}
		else
		{
			if (i > 128)
				c = Math_Pow((i - 128) / 112.0, this->yuvParam.CGamma) * 16383;
			else if (i == 128)
				c = 0;
			else
				c = Math_Pow((-i + 128) / 112.0, this->yuvParam.CGamma) * (-16383);
		}

		Int32 v2r8;
		Int32 u2g8;
		Int32 v2g8;
		Int32 u2b8;
		v2r8 = Double2Int32((Kc1 * c * this->yuvParam.Saturation * this->yuvParam.RMul + this->yuvParam.RAdd) / 4.0);
		u2g8 = Double2Int32((Kc3 * c * this->yuvParam.Saturation * this->yuvParam.GMul + this->yuvParam.GAdd) / 4.0);
		v2g8 = Double2Int32((Kc2 * c * this->yuvParam.Saturation * this->yuvParam.GMul) / 4.0);
		u2b8 = Double2Int32((Kc4 * c * this->yuvParam.Saturation * this->yuvParam.BMul + this->yuvParam.BAdd) / 4.0);
		if (v2r8 < -0x8000)
			v2r8 = 0x8000;
		else if (v2r8 > 0x7fff)
			v2r8 = 0x7fff;
		else
			v2r8 = v2r8 & 0xffff;
		if (u2g8 < -0x8000)
			u2g8 = 0x8000;
		else if (u2g8 > 0x7fff)
			u2g8 = 0x7fff;
		else
			u2g8 = u2g8 & 0xffff;
		if (v2g8 < -0x8000)
			v2g8 = 0x8000;
		else if (v2g8 > 0x7fff)
			v2g8 = 0x7fff;
		else
			v2g8 = v2g8 & 0xffff;
		if (u2b8 < -0x8000)
			u2b8 = 0x8000;
		else if (u2b8 > 0x7fff)
			u2b8 = 0x7fff;
		else
			u2b8 = u2b8 & 0xffff;
		yuv2rgb[i + 256] = (((Int64)u2g8) << 16) | u2b8;
		yuv2rgb[i + 512] = (((Int64)v2g8) << 16) | (((Int64)v2r8) << 32);
	}
}

void Media::CS::CSYUV_LRGBC::SetupYUV14_RGB13()
{
	Double c;

	Int32 i;

	Int32 y;

	Double Kr;
	Double Kb;
	Double Kg;
	Double Kc1;
	Double Kc2;
	Double Kc3;
	Double Kc4;
	Media::ColorProfile::YUVType yuvType;
	Bool fullRange = (this->yuvType & Media::ColorProfile::YUVT_FLAG_YUV_0_255) != 0;
	if ((this->yuvType & Media::ColorProfile::YUVT_MASK) == Media::ColorProfile::YUVT_UNKNOWN)
	{
		yuvType = this->colorSess->GetDefYUVType();
	}
	else
	{
		yuvType = (Media::ColorProfile::YUVType)(this->yuvType & Media::ColorProfile::YUVT_MASK);
	}

	Media::ColorProfile::GetYUVConstants(yuvType, &Kr, &Kb);
	Kg = 1 - Kr - Kb;
	Kc1 = (1 - Kr) / 0.5;
	Kc2 = -(2 * Kr - 2 * Kr * Kr) / Kg;
	Kc3 = -(2 * Kb - 2 * Kb * Kb) / Kg;
	Kc4 = (1 - Kb) / 0.5;

	if (fullRange)
	{
		i = 256;
		while (i--)
		{
			y = Double2Int32(0x7fff * (Math_Pow(i / 255.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
			if (y < -0x8000)
				y = 0x8000;
			else if (y > 0x7fff)
				y = 0x7fff;
			else
				y = y & 0xffff;
			yuv2rgb14[i] = (((Int64)y) << 32) | (((Int64)y) << 16) | y;
		}
	}
	else
	{
		i = 256;
		while (i--)
		{
			y = Double2Int32(0x7fff * (Math_Pow((i - 16) / 219.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
			if (y < -0x8000)
				y = 0x8000;
			else if (y > 0x7fff)
				y = 0x7fff;
			else
				y = y & 0xffff;
			yuv2rgb14[i] = (((Int64)y) << 32) | (((Int64)y) << 16) | y;
		}
	}

	i = 65536;
	while (i--)
	{
		if (fullRange)
		{
			if (i >= 32768)
				c = Math_Pow((-i + 8192 + 65536) / 8160.0, this->yuvParam.CGamma) * (-16383);
			else if (i > 8192)
				c = Math_Pow((i - 8192) / 8160.0, this->yuvParam.CGamma) * 16383;
			else if (i == 8192)
				c = 0;
			else
				c = Math_Pow((-i + 8192) / 8160.0, this->yuvParam.CGamma) * (-16383);
		}
		else
		{
			if (i >= 32768)
				c = Math_Pow((-i + 8192 + 65536) / 7168.0, this->yuvParam.CGamma) * (-16383);
			else if (i > 8192)
				c = Math_Pow((i - 8192) / 7168.0, this->yuvParam.CGamma) * 16383;
			else if (i == 8192)
				c = 0;
			else
				c = Math_Pow((-i + 8192) / 7168.0, this->yuvParam.CGamma) * (-16383);
		}

		Int32 v2r8;
		Int32 u2g8;
		Int32 v2g8;
		Int32 u2b8;
		v2r8 = Double2Int32((Kc1 * c * this->yuvParam.Saturation * this->yuvParam.RMul + this->yuvParam.RAdd) / 4.0);
		u2g8 = Double2Int32((Kc3 * c * this->yuvParam.Saturation * this->yuvParam.GMul + this->yuvParam.GAdd) / 4.0);
		v2g8 = Double2Int32((Kc2 * c * this->yuvParam.Saturation * this->yuvParam.GMul) / 4.0);
		u2b8 = Double2Int32((Kc4 * c * this->yuvParam.Saturation * this->yuvParam.BMul + this->yuvParam.BAdd) / 4.0);
		if (v2r8 < -0x8000)
			v2r8 = 0x8000;
		else if (v2r8 > 0x7fff)
			v2r8 = 0x7fff;
		else
			v2r8 = v2r8 & 0xffff;
		if (u2g8 < -0x8000)
			u2g8 = 0x8000;
		else if (u2g8 > 0x7fff)
			u2g8 = 0x7fff;
		else
			u2g8 = u2g8 & 0xffff;
		if (v2g8 < -0x8000)
			v2g8 = 0x8000;
		else if (v2g8 > 0x7fff)
			v2g8 = 0x7fff;
		else
			v2g8 = v2g8 & 0xffff;
		if (u2b8 < -0x8000)
			u2b8 = 0x8000;
		else if (u2b8 > 0x7fff)
			u2b8 = 0x7fff;
		else
			u2b8 = u2b8 & 0xffff;
		yuv2rgb14[i + 256] = (((Int64)u2g8) << 16) | u2b8;
		yuv2rgb14[i + 256 + 65536] = (((Int64)v2g8) << 16) | (((Int64)v2r8) << 32);
	}
}

Media::CS::CSYUV_LRGBC::CSYUV_LRGBC(const Media::ColorProfile *srcProfile, const Media::ColorProfile *destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess) : Media::CS::CSConverter(colorSess), srcProfile(srcProfile), destProfile(destProfile)
{
	this->yuvType = yuvType;
	this->rgbGammaCorr = MemAlloc(Int64, 65536 * 3);
	this->yuv2rgb = MemAlloc(Int64, 768);
	this->yuv2rgb14 = MemAlloc(Int64, 256 + 65536 * 2);
	this->rgbUpdated = true;
	this->yuvUpdated = true;

	if (colorSess)
	{
		MemCopyNO(&this->yuvParam, colorSess->GetYUVParam(), sizeof(YUVPARAM));
		this->rgbParam.Set(colorSess->GetRGBParam());
	}
	else
	{
		this->yuvParam.SetDefault();
		this->rgbParam.SetDefault();
	}	
}

Media::CS::CSYUV_LRGBC::~CSYUV_LRGBC()
{
	MemFree(this->rgbGammaCorr);
	MemFree(this->yuv2rgb14);
	MemFree(this->yuv2rgb);
}

void Media::CS::CSYUV_LRGBC::UpdateTable()
{
	if (this->rgbUpdated)
	{
		this->rgbUpdated = false;
		this->SetupRGB13_LR();
	}
	if (this->yuvUpdated)
	{
		this->yuvUpdated = false;
		this->SetupYUV_RGB13();
		this->SetupYUV14_RGB13();
	}
}

void Media::CS::CSYUV_LRGBC::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuv)
{
	MemCopyNO(&this->yuvParam, yuv, sizeof(YUVPARAM));
	this->yuvUpdated = true;
}

void Media::CS::CSYUV_LRGBC::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgb)
{
	this->rgbParam.Set(rgb);
	this->rgbUpdated = true;
}

UOSInt Media::CS::CSYUV_LRGBC::GetDestFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 3;
}
