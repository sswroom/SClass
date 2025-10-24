#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSYUV16_LRGBC.h"
#include "Media/CS/TransferFunc.h"
#include "Math/Math_C.h"

void Media::CS::CSYUV16_LRGBC::SetupRGB13_LR()
{
	Int32 i;
	Double thisV;
	UInt16 v[4];
	NN<Media::ColorProfile> srcColor;
	NN<Media::ColorManagerSess> nncolorSess;
	if (!this->colorSess.SetTo(nncolorSess))
	{
		srcColor = this->srcProfile;		
		if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN || this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
		{
			this->srcProfile.SetCommonProfile(Media::ColorProfile::CPT_SRGB);
		}
		else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN || this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY)
		{
			this->srcProfile.SetCommonProfile(Media::ColorProfile::CPT_BT709);
		}
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		srcColor = nncolorSess->GetDefVProfile();
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		srcColor = nncolorSess->GetDefPProfile();
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY)
	{
		srcColor = nncolorSess->GetDefVProfile();
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		srcColor = nncolorSess->GetDefPProfile();
	}
	else
	{
		srcColor = this->srcProfile;
	}

	NN<Media::CS::TransferFunc> rtFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetRTranParam());
	NN<Media::CS::TransferFunc> gtFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetGTranParam());
	NN<Media::CS::TransferFunc> btFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetBTranParam());
	Math::Matrix3 mat1;
	if (this->destProfile.GetPrimaries()->colorType == Media::ColorProfile::CT_DISPLAY)
	{
		Media::ColorProfile::GetConvMatrix(mat1, this->srcProfile.GetPrimaries(), this->rgbParam.monProfile.GetPrimaries());
	}
	else
	{
		Media::ColorProfile::GetConvMatrix(mat1, this->srcProfile.GetPrimaries(), this->destProfile.GetPrimaries());
	}

	i = 32768;
	while (i--)
	{
		thisV = rtFunc->InverseTransfer(i / 32767.0 * 4.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[0]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[0]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[0]);
		v[3] = 0;
		this->rgbGammaCorr[i] = *(Int64*)&v[0];

		thisV = gtFunc->InverseTransfer(i / 32767.0 * 4.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[1]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[1]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[1]);
		v[3] = 0;
		this->rgbGammaCorr[i + 65536] = *(Int64*)&v[0];

		thisV = btFunc->InverseTransfer(i / 32767.0 * 4.0);
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
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[0]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[0]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[0]);
		v[3] = 0;
		this->rgbGammaCorr[i] = *(Int64*)&v[0];

		thisV = gtFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[1]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[1]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[1]);
		v[3] = 0;
		this->rgbGammaCorr[i + 65536] = *(Int64*)&v[0];

		thisV = btFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[2]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[2]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[2]);
		v[3] = 0;
		this->rgbGammaCorr[i + 131072] = *(Int64*)&v[0];
	}
	btFunc.Delete();
	gtFunc.Delete();
	rtFunc.Delete();
}

void Media::CS::CSYUV16_LRGBC::SetupYUV_RGB13()
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
	NN<Media::ColorManagerSess> nncolorSess;
	Bool fullRange = (this->yuvType & Media::ColorProfile::YUVT_FLAG_YUV_0_255) != 0;
	if ((this->yuvType & Media::ColorProfile::YUVT_MASK) == Media::ColorProfile::YUVT_UNKNOWN && this->colorSess.SetTo(nncolorSess))
	{
		yuvType = nncolorSess->GetDefYUVType();
	}
	else
	{
		yuvType = (Media::ColorProfile::YUVType)(this->yuvType & Media::ColorProfile::YUVT_MASK);
	}
	Media::ColorProfile::GetYUVConstants(yuvType, Kr, Kb);
	Kg = 1 - Kr - Kb;
	Kc1 = (1 - Kr) / 0.5;
	Kc2 = -(2 * Kr - 2 * Kr * Kr) / Kg;
	Kc3 = -(2 * Kb - 2 * Kb * Kb) / Kg;
	Kc4 = (1 - Kb) / 0.5;

	if (fullRange)
	{
		i = 65536;
		while (i--)
		{
			y = Double2Int32(0x7fff * (Math_Pow(i / 65535.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
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
		i = 65536;
		while (i--)
		{
			y = Double2Int32(0x7fff * (Math_Pow((i - 4096) / 56064.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
			if (y < -0x8000)
				y = 0x8000;
			else if (y > 0x7fff)
				y = 0x7fff;
			else
				y = y & 0xffff;
			yuv2rgb[i] = (((Int64)y) << 32) | (((Int64)y) << 16) | y;
		}
	}

	i = 65536;
	while (i--)
	{
		if (i == 0)
			i = 0;

		if (fullRange)
		{
			if (i > 32768)
				c = Math_Pow((i - 32768) / 32767.0, this->yuvParam.CGamma) * 16383;
			else if (i == 32768)
				c = 0;
			else
				c = Math_Pow((-i + 32768) / 32767.0, this->yuvParam.CGamma) * (-16383);
		}
		else
		{
			if (i > 32768)
				c = Math_Pow((i - 32768) / 28672.0, this->yuvParam.CGamma) * 16383;
			else if (i == 32768)
				c = 0;
			else
				c = Math_Pow((-i + 32768) / 28672.0, this->yuvParam.CGamma) * (-16383);
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
		yuv2rgb[i + 65536] = (((Int64)u2g8) << 16) | u2b8;
		yuv2rgb[i + 131072] = (((Int64)v2g8) << 16) | (((Int64)v2r8) << 32);
	}
}

void Media::CS::CSYUV16_LRGBC::SetupYUV14_RGB13()
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
	NN<Media::ColorManagerSess> nncolorSess;
	Bool fullRange = (this->yuvType & Media::ColorProfile::YUVT_FLAG_YUV_0_255) != 0;
	if ((this->yuvType & Media::ColorProfile::YUVT_MASK) == Media::ColorProfile::YUVT_UNKNOWN && this->colorSess.SetTo(nncolorSess))
	{
		yuvType = nncolorSess->GetDefYUVType();
	}
	else
	{
		yuvType = (Media::ColorProfile::YUVType)(this->yuvType & Media::ColorProfile::YUVT_MASK);
	}

	Media::ColorProfile::GetYUVConstants(yuvType, Kr, Kb);
	Kg = 1 - Kr - Kb;
	Kc1 = (1 - Kr) / 0.5;
	Kc2 = -(2 * Kr - 2 * Kr * Kr) / Kg;
	Kc3 = -(2 * Kb - 2 * Kb * Kb) / Kg;
	Kc4 = (1 - Kb) / 0.5;

	if (fullRange)
	{
		i = 65536;
		while (i--)
		{
			y = Double2Int32(0x7fff * (Math_Pow(i / 65535.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
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
		i = 65536;
		while (i--)
		{
			y = Double2Int32(0x7fff * (Math_Pow((i - 4096) / 56064.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
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
				c = Math_Pow((-i + 8192 + 65536) / 8191.75, this->yuvParam.CGamma) * (-16383);
			else if (i > 8192)
				c = Math_Pow((i - 8192) / 8191.75, this->yuvParam.CGamma) * 16383;
			else if (i == 8192)
				c = 0;
			else
				c = Math_Pow((-i + 8192) / 8191.75, this->yuvParam.CGamma) * (-16383);
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
		yuv2rgb14[i + 65536] = (((Int64)u2g8) << 16) | u2b8;
		yuv2rgb14[i + 65536 + 65536] = (((Int64)v2g8) << 16) | (((Int64)v2r8) << 32);
	}
}

Media::CS::CSYUV16_LRGBC::CSYUV16_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess) : Media::CS::CSConverter(colorSess), srcProfile(srcProfile), destProfile(destProfile)
{
	this->yuvType = yuvType;
	this->rgbGammaCorr = MemAllocArr(Int64, 65536 * 3);
	this->yuv2rgb = MemAllocArr(Int64, 65536 * 3);
	this->yuv2rgb14 = MemAllocArr(Int64, 65536 * 3);
	this->rgbUpdated = true;
	this->yuvUpdated = true;

	NN<Media::ColorManagerSess> nncolorSess;
	if (colorSess.SetTo(nncolorSess))
	{
		MemCopyNO(&this->yuvParam, nncolorSess->GetYUVParam().Ptr(), sizeof(YUVPARAM));
		this->rgbParam.Set(nncolorSess->GetRGBParam());
	}
	else
	{
		this->yuvParam.SetDefault();
		this->rgbParam.SetDefault();
	}	
}

Media::CS::CSYUV16_LRGBC::~CSYUV16_LRGBC()
{
	MemFreeArr(this->rgbGammaCorr);
	MemFreeArr(this->yuv2rgb);
	MemFreeArr(this->yuv2rgb14);
}

void Media::CS::CSYUV16_LRGBC::UpdateTable()
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

void Media::CS::CSYUV16_LRGBC::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuv)
{
	MemCopyNO(&this->yuvParam, yuv.Ptr(), sizeof(YUVPARAM));
	this->yuvUpdated = true;
}

void Media::CS::CSYUV16_LRGBC::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgb)
{
	this->rgbParam.Set(rgb);
	this->rgbUpdated = true;
}

UOSInt Media::CS::CSYUV16_LRGBC::GetDestFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 3;
}
