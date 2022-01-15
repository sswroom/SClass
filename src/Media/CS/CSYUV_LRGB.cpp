#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSYUV_LRGB.h"
#include "Math/Math.h"

void Media::CS::CSYUV_LRGB::SetupRGB13_LR()
{
	Int32 i;
	Double thisV;

	i = 32768;
	while (i--)
	{
		thisV = this->irFunc->InverseTransfer(i / 32767.0 * 4.0) * 16383.0;
		if (thisV < 0)
			*(UInt16*)&rgbGammaCorr[i << 1] = 0;
		else if (thisV > 32767.0)
			*(UInt16*)&rgbGammaCorr[i << 1] = 32767;
		else
			*(UInt16*)&rgbGammaCorr[i << 1] = (UInt16)Math::Double2Int32(thisV);
	}
	i = 65536;
	while (i-- > 32768)
	{
		thisV = this->irFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0) * 16383.0;
		if (thisV < 0)
			*(UInt16*)&rgbGammaCorr[i << 1] = 0;
		else if (thisV > 32767)
			*(UInt16*)&rgbGammaCorr[i << 1] = 32767;
		else
			*(UInt16*)&rgbGammaCorr[i << 1] = (UInt16)Math::Double2Int32(thisV);
	}
}

void Media::CS::CSYUV_LRGB::SetupYUV_RGB13()
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
//	Bool fullRange = (this->yuvType & Media::ColorProfile::YUVT_FLAG_YUV_0_255) != 0;
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

	i = 256;
	while (i--)
	{
		y = Math::Double2Int32(0x7fff * (Math_Pow((i - 16) / 219.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
		if (y < -0x8000)
			y = 0x8000;
		else if (y > 0x7fff)
			y = 0x7fff;
		else
			y = y & 0xffff;
		yuv2rgb[i] = (((Int64)y) << 32) | (((Int64)y) << 16) | y;
	}

	i = 256;
	while (i--)
	{
		if (i == 0)
			i = 0;

		if (i > 128)
			c = Math_Pow((i - 128) / 112.0, this->yuvParam.CGamma) * 16383;
		else if (i == 128)
			c = 0;
		else
			c = Math_Pow((-i + 128) / 112.0, this->yuvParam.CGamma) * (-16383);

		Int32 v2r8;
		Int32 u2g8;
		Int32 v2g8;
		Int32 u2b8;
		v2r8 = Math::Double2Int32((Kc1 * c * this->yuvParam.Saturation * this->yuvParam.RMul + this->yuvParam.RAdd) / 4.0);
		u2g8 = Math::Double2Int32((Kc3 * c * this->yuvParam.Saturation * this->yuvParam.GMul + this->yuvParam.GAdd) / 4.0);
		v2g8 = Math::Double2Int32((Kc2 * c * this->yuvParam.Saturation * this->yuvParam.GMul) / 4.0);
		u2b8 = Math::Double2Int32((Kc4 * c * this->yuvParam.Saturation * this->yuvParam.BMul + this->yuvParam.BAdd) / 4.0);
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

Media::CS::CSYUV_LRGB::CSYUV_LRGB(const Media::ColorProfile *srcColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess) : Media::CS::CSConverter(colorSess)
{
	NEW_CLASS(this->srcColor, Media::ColorProfile(srcColor));
	NEW_CLASS(this->rgbParam, Media::IColorHandler::RGBPARAM2());
	this->yuvType = yuvType;
	this->rgbGammaCorr = MemAlloc(UInt8, 65536 * 2);
	this->yuv2rgb = MemAlloc(Int64, 768);

	this->rgbUpdated = true;
	this->yuvUpdated = true;

	MemCopyNO(&this->yuvParam, colorSess->GetYUVParam(), sizeof(YUVPARAM));
	this->rgbParam->Set(colorSess->GetRGBParam());

	Media::ColorProfile *tranColor;
	if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		tranColor = this->colorSess->GetDefVProfile();
	}
	else if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		tranColor = this->colorSess->GetDefPProfile();
	}
	else if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		tranColor = this->rgbParam->monProfile;
	}
	else
	{
		tranColor = this->srcColor;
	}
	this->irFunc = Media::CS::TransferFunc::CreateFunc(tranColor->GetRTranParam());
	this->igFunc = Media::CS::TransferFunc::CreateFunc(tranColor->GetGTranParam());
	this->ibFunc = Media::CS::TransferFunc::CreateFunc(tranColor->GetBTranParam());
}

Media::CS::CSYUV_LRGB::~CSYUV_LRGB()
{
	MemFree(this->rgbGammaCorr);
	MemFree(this->yuv2rgb);
	DEL_CLASS(this->srcColor);
	DEL_CLASS(this->rgbParam);
	DEL_CLASS(this->irFunc);
	DEL_CLASS(this->igFunc);
	DEL_CLASS(this->ibFunc);
}

void Media::CS::CSYUV_LRGB::UpdateTable()
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
	}
}

void Media::CS::CSYUV_LRGB::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuv)
{
	MemCopyNO(&this->yuvParam, yuv, sizeof(YUVPARAM));
	this->yuvUpdated = true;
}

void Media::CS::CSYUV_LRGB::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgb)
{
	Media::ColorProfile *srcColor;
	if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		srcColor = this->colorSess->GetDefVProfile();
	}
	else if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		srcColor = this->colorSess->GetDefPProfile();
	}
	else if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY)
	{
		srcColor = this->colorSess->GetDefVProfile();
	}
	else if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		srcColor = this->colorSess->GetDefPProfile();
 	}
	else
	{
		srcColor = this->srcColor;
	}

	DEL_CLASS(this->irFunc);
	DEL_CLASS(this->igFunc);
	DEL_CLASS(this->ibFunc);
	this->irFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetRTranParam());
	this->igFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetGTranParam());
	this->ibFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetBTranParam());
	this->rgbUpdated = true;
	this->rgbParam->Set(rgb);
}

UOSInt Media::CS::CSYUV_LRGB::GetDestFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 3;
}
