#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSYUV_LRGBHQ.h"
#include "Math/Math.h"

void Media::CS::CSYUV_LRGBHQ::SetupYUV14_RGB13()
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

	i = 65536;
	while (i--)
	{
		if (i >= 32768)
			c = Math_Pow((-i + 8192 + 65536) / 7168.0, this->yuvParam.CGamma) * (-16383);
		else if (i > 8192)
			c = Math_Pow((i - 8192) / 7168.0, this->yuvParam.CGamma) * 16383;
		else if (i == 8192)
			c = 0;
		else
			c = Math_Pow((-i + 8192) / 7168.0, this->yuvParam.CGamma) * (-16383);

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

Media::CS::CSYUV_LRGBHQ::CSYUV_LRGBHQ(NN<const Media::ColorProfile> srcColor, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess) : Media::CS::CSYUV_LRGB(srcColor, yuvType, colorSess)
{
	this->yuv2rgb14 = MemAllocArr(Int64, 256 + 65536 * 2);
}

Media::CS::CSYUV_LRGBHQ::~CSYUV_LRGBHQ()
{
	MemFreeArr(this->yuv2rgb14);
}

void Media::CS::CSYUV_LRGBHQ::UpdateTable()
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
