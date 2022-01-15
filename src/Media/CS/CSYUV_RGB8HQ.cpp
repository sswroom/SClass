#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSYUV_RGB8HQ.h"
#include "Math/Math.h"

void Media::CS::CSYUV_RGB8HQ::SetupYUV14_RGB13()
{
/*	Double sumy[256];
	Double sumc[256];
	Double currSum;
	Double ynor;
	Double cnor;
	Double c;

	__int64 j;
	int i;
	int k;

	int updateY = 1;
	int updateC = 1;
	if (updateY)
	{
		i = 16;
		j = 0;
		k = 0;
		ynor = 16;
		currSum = 0;
		i = 17;
		while (i <= 235)
		{
			currSum += ynor;
			sumy[i++] = currSum;
		}
		while (i < 256)
			sumy[i++] = currSum;

		i = 17;
		while (i--)
			sumy[i] = 0;

		currSum = 1 / sumy[255];
		long y;
		i = 256;
		while (i--)
		{
			y = (long)(0x7fff * (Math_Pow(sumy[i] * currSum, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
			if (y < -0x8000)
				y = 0x8000;
			else if (y > 0x7fff)
				y = 0x7fff;
			else
				y = y & 0xffff;
			yuv2rgb[i] = (((Int64)y) << 32) | (((Int64)y) << 16) | y;
		}
	}

	if (updateC)
	{
		Double Kr;
		Double Kb;
		Double Kg;
		Double Kc1;
		Double Kc2;
		Double Kc3;
		Double Kc4;

		Media::IColorHandler::YUVType yuvType;
		if (this->yuvType == Media::IColorHandler::YUVT_UNKNOWN)
		{
			yuvType = this->yuvParam.Type;
		}
		else
		{
			yuvType = this->yuvType;
		}
		if (yuvType == YUVT_BT601)
		{
			Kr = 0.2985;
			Kb = 0.1135;
		}
		else if (yuvType == YUVT_BT709)
		{
			Kr = 0.2126;
			Kb = 0.0722;
		}
		else if (yuvType == YUVT_FCC)
		{
			Kr = 0.30;
			Kb = 0.11;
		}
		else if (yuvType == YUVT_BT470BG)
		{
			Kr = 0.299;
			Kb = 0.114;
		}
		else if (yuvType == YUVT_SMPTE170M)
		{
			Kr = 0.299;
			Kb = 0.114;
		}
		else if (yuvType == YUVT_SMPTE240M)
		{
			Kr = 0.212;
			Kb = 0.087;
		}

		Kg = 1 - Kr - Kb;
		Kc1 = (1 - Kr) / 0.5;
		Kc2 = -(2 * Kr - 2 * Kr * Kr) / Kg;
		Kc3 = -(2 * Kb - 2 * Kb * Kb) / Kg;
		Kc4 = (1 - Kb) / 0.5;

		i = 112;
		j = 0;
		k = 0;
		cnor = 16;
		currSum = 0;
		i = 1;
		while (i <= 112)
		{
			currSum += cnor;
			sumc[i++] = currSum;
		}
		c = currSum;
		while (i <= 127)
		{
			c += cnor;
			sumc[i++] = c;
		}
		sumc[128] = sumc[127];

		currSum = 1/currSum;
		i = 256;
		while (i--)
		{
			if (i == 0)
				i = 0;

			if (i > 128)
				c = Math_Pow(sumc[i - 128] * currSum, this->yuvParam.CGamma) * 16383;
			else if (i == 128)
				c = 0;
			else
				c = Math_Pow(sumc[-i + 128] * currSum, this->yuvParam.CGamma) * (-16383);

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
	}*/
}

Media::CS::CSYUV_RGB8HQ::CSYUV_RGB8HQ(Media::ColorProfile *srcColor, Media::ColorProfile *destColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess) : Media::CS::CSYUV_RGB8(srcColor, destColor, yuvType, colorSess)
{
	this->yuv2rgb14 = MemAlloc(Int64, 256 + 65536 * 2);
}

Media::CS::CSYUV_RGB8HQ::~CSYUV_RGB8HQ()
{
	MemFree(this->yuv2rgb14);
}

void Media::CS::CSYUV_RGB8HQ::UpdateTable()
{
	if (this->rgbUpdated)
	{
		this->rgbUpdated = false;
		this->SetupRGB13_8();
	}
	if (this->yuvUpdated)
	{
		this->yuvUpdated = false;
		this->SetupYUV_RGB13();
		this->SetupYUV14_RGB13();
	}
}
