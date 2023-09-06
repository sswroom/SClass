#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSYUV10_RGB8.h"
#include "Math/Math.h"

void Media::CS::CSYUV10_RGB8::SetupRGB13_8()
{
	Int32 i;
	Double tmpV;
	Double thisV;
	Double rGammaVal;
	Double gGammaVal;
	Double bGammaVal;
	Double rBright;
	Double gBright;
	Double bBright;
	Double rContr;
	Double gContr;
	Double bContr;
	Double tMul;
	Double rMul;
	Double gMul;
	Double bMul;

	if (this->destColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY)
	{
		rGammaVal = this->rgbParam.MonRGamma;
		gGammaVal = this->rgbParam.MonGGamma;
		bGammaVal = this->rgbParam.MonBGamma;
		rBright = this->rgbParam.MonRBright;
		gBright = this->rgbParam.MonGBright;
		bBright = this->rgbParam.MonBBright;
		rContr = this->rgbParam.MonRContr;
		gContr = this->rgbParam.MonGContr;
		bContr = this->rgbParam.MonBContr;
		tMul = this->rgbParam.MonVBrightness;
		rMul = this->rgbParam.MonRBrightness * tMul;
		gMul = this->rgbParam.MonGBrightness * tMul;
		bMul = this->rgbParam.MonBBrightness * tMul;
	}
	else if (this->destColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		rGammaVal = this->rgbParam.MonRGamma;
		gGammaVal = this->rgbParam.MonGGamma;
		bGammaVal = this->rgbParam.MonBGamma;
		rBright = this->rgbParam.MonRBright;
		gBright = this->rgbParam.MonGBright;
		bBright = this->rgbParam.MonBBright;
		rContr = this->rgbParam.MonRContr;
		gContr = this->rgbParam.MonGContr;
		bContr = this->rgbParam.MonBContr;
		tMul = this->rgbParam.MonPBrightness;
		rMul = this->rgbParam.MonRBrightness * tMul;
		gMul = this->rgbParam.MonGBrightness * tMul;
		bMul = this->rgbParam.MonBBrightness * tMul;
	}
	else
	{
		rGammaVal = 1.0;
		gGammaVal = 1.0;
		bGammaVal = 1.0;
		rBright = 1.0;
		gBright = 1.0;
		bBright = 1.0;
		rContr = 1.0;
		gContr = 1.0;
		bContr = 1.0;
		tMul = 1.0;
		rMul = 1.0;
		gMul = 1.0;
		bMul = 1.0;
	}

	i = 32768;
	while (i--)
	{
		thisV = this->frFunc->ForwardTransfer(this->irFunc->InverseTransfer(i / 32767.0 * 4.0) * rMul);
		
		tmpV = (rBright - 1.0 + Math_Pow(thisV, rGammaVal) * rContr) * 255.0;
		if (tmpV < 0)
			rgbGammaCorr[i] = 0;
		else if (tmpV > 255)
			rgbGammaCorr[i] = 255;
		else
			rgbGammaCorr[i] = (UInt8)Double2Int32(tmpV);

		thisV = this->fgFunc->ForwardTransfer(this->igFunc->InverseTransfer(i / 32767.0 * 4.0) * gMul);
		tmpV = (gBright - 1.0 + Math_Pow(thisV, gGammaVal) * gContr) * 255.0;
		if (tmpV < 0)
			rgbGammaCorr[i+65536] = 0;
		else if (tmpV > 255)
			rgbGammaCorr[i+65536] = 255;
		else
			rgbGammaCorr[i+65536] = (UInt8)Double2Int32(tmpV);

		thisV = this->fbFunc->ForwardTransfer(this->ibFunc->InverseTransfer(i / 32767.0 * 4.0) * bMul);
		tmpV = (bBright - 1.0 + Math_Pow(thisV, bGammaVal) * bContr) * 255.0;
		if (tmpV < 0)
			rgbGammaCorr[i+131072] = 0;
		else if (tmpV > 255)
			rgbGammaCorr[i+131072] = 255;
		else
			rgbGammaCorr[i+131072] = (UInt8)Double2Int32(tmpV);
	}
	i = 65536;
	while (i-- > 32768)
	{
		thisV = this->frFunc->ForwardTransfer(this->irFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0) * rMul);
		tmpV = (rBright - 1.0 + Math_Pow(thisV, rGammaVal) * rContr) * 255.0;
		if (tmpV < 0)
			rgbGammaCorr[i] = 0;
		else if (tmpV > 255)
			rgbGammaCorr[i] = 255;
		else
			rgbGammaCorr[i] = (UInt8)Double2Int32(tmpV);

		thisV = this->fgFunc->ForwardTransfer(this->igFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0) * gMul);
		tmpV = (gBright - 1.0 + Math_Pow(thisV, gGammaVal) * gContr) * 255.0;
		if (tmpV < 0)
			rgbGammaCorr[i+65536] = 0;
		else if (tmpV > 255)
			rgbGammaCorr[i+65536] = 255;
		else
			rgbGammaCorr[i+65536] = (UInt8)Double2Int32(tmpV);

		thisV = this->fbFunc->ForwardTransfer(this->ibFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0) * bMul);
		tmpV = (bBright - 1.0 + Math_Pow(thisV, bGammaVal) * bContr) * 255.0;
		if (tmpV < 0)
			rgbGammaCorr[i+131072] = 0;
		else if (tmpV > 255)
			rgbGammaCorr[i+131072] = 255;
		else
			rgbGammaCorr[i+131072] = (UInt8)Double2Int32(tmpV);
	}
}

void Media::CS::CSYUV10_RGB8::SetupYUV_RGB13()
{
	Double sumy[1024];
	Double sumc[1024];
	Double currSum;
	Double ynor;
	Double cnor;
	Double c;

	OSInt i;

	int updateY = 1;
	int updateC = 1;
	if (updateY)
	{
		i = 64;
		ynor = 64;
		currSum = 0;
		i = 65;
		while (i <= 940)
		{
			currSum += ynor;
			sumy[i++] = currSum;
		}
		while (i < 1024)
			sumy[i++] = currSum;

		i = 65;
		while (i--)
			sumy[i] = 0;

		currSum = 1 / sumy[1023];
		long y;
		i = 1024;
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

		Media::ColorProfile::YUVType yuvType;
//		Bool fullRange = (this->yuvType & Media::ColorProfile::YUVT_FLAG_YUV_0_255) != 0;
		if ((this->yuvType & Media::ColorProfile::YUVT_MASK) == Media::ColorProfile::YUVT_UNKNOWN)
		{
			yuvType = this->colorSess->GetDefYUVType();
		}
		else
		{
			yuvType = (Media::ColorProfile::YUVType)(this->yuvType & Media::ColorProfile::YUVT_MASK);
		}
		if (yuvType == Media::ColorProfile::YUVT_BT601)
		{
			Kr = 0.2985;
			Kb = 0.1135;
		}
		else if (yuvType == Media::ColorProfile::YUVT_BT709)
		{
			Kr = 0.2126;
			Kb = 0.0722;
		}
		else if (yuvType == Media::ColorProfile::YUVT_FCC)
		{
			Kr = 0.30;
			Kb = 0.11;
		}
		else if (yuvType == Media::ColorProfile::YUVT_BT470BG)
		{
			Kr = 0.299;
			Kb = 0.114;
		}
		else if (yuvType == Media::ColorProfile::YUVT_SMPTE170M)
		{
			Kr = 0.299;
			Kb = 0.114;
		}
		else if (yuvType == Media::ColorProfile::YUVT_SMPTE240M)
		{
			Kr = 0.212;
			Kb = 0.087;
		}
		else
		{
			Kr = 0.2985;
			Kb = 0.1135;
		}

		Kg = 1 - Kr - Kb;
		Kc1 = (1 - Kr) / 0.5;
		Kc2 = -(2 * Kr - 2 * Kr * Kr) / Kg;
		Kc3 = -(2 * Kb - 2 * Kb * Kb) / Kg;
		Kc4 = (1 - Kb) / 0.5;

		i = 448;
		cnor = 64;
		currSum = 0;
		i = 1;
		while (i <= 448)
		{
			currSum += cnor;
			sumc[i++] = currSum;
		}
		c = currSum;
		while (i <= 511)
		{
			c += cnor;
			sumc[i++] = c;
		}
		sumc[512] = sumc[511];

		currSum = 1/currSum;
		i = 1024;
		while (i--)
		{
			if (i == 0)
				i = 0;

			if (i > 512)
				c = Math_Pow(sumc[i - 512] * currSum, this->yuvParam.CGamma) * 16383;
			else if (i == 512)
				c = 0;
			else
				c = Math_Pow(sumc[-i + 512] * currSum, this->yuvParam.CGamma) * (-16383);

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
			yuv2rgb[i + 1024] = (((Int64)u2g8) << 16) | u2b8;
			yuv2rgb[i + 2048] = (((Int64)v2g8) << 16) | (((Int64)v2r8) << 32);
		}
	}
}

Media::CS::CSYUV10_RGB8::CSYUV10_RGB8(NotNullPtr<const Media::ColorProfile> srcColor, NotNullPtr<const Media::ColorProfile> destColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess) : Media::CS::CSConverter(colorSess), srcColor(srcColor), destColor(destColor)
{
	this->yuvType = yuvType;
	this->rgbGammaCorr = MemAlloc(UInt8, 65536 * 3);
	this->yuv2rgb = MemAlloc(Int64, 3072);

	this->rgbUpdated = true;
	this->yuvUpdated = true;

	MemCopyNO(&this->yuvParam, colorSess->GetYUVParam().Ptr(), sizeof(YUVPARAM));
	this->rgbParam.Set(colorSess->GetRGBParam());

	NotNullPtr<Media::ColorProfile> srcProfile;
	NotNullPtr<Media::ColorProfile> destProfile;

	if (this->srcColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		srcProfile = this->colorSess->GetDefVProfile();
	}
	else if (this->srcColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		srcProfile = this->colorSess->GetDefPProfile();
	}
	else if (this->srcColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || this->srcColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		srcProfile = this->rgbParam.monProfile;
	}
	else
	{
		srcProfile = this->srcColor;
	}
	if (this->destColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || this->destColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		destProfile = this->rgbParam.monProfile;
	}
	else if (this->destColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN || this->destColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		destProfile = srcProfile;
	}
	else
	{
		destProfile = this->destColor;
	}
	this->irFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetRTranParam());
	this->igFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetGTranParam());
	this->ibFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetBTranParam());
	this->frFunc = Media::CS::TransferFunc::CreateFunc(destProfile->GetRTranParam());
	this->fgFunc = Media::CS::TransferFunc::CreateFunc(destProfile->GetGTranParam());
	this->fbFunc = Media::CS::TransferFunc::CreateFunc(destProfile->GetBTranParam());
}

void Media::CS::CSYUV10_RGB8::UpdateTable()
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
	}
}

void Media::CS::CSYUV10_RGB8::Release()
{
	MemFree(this->rgbGammaCorr);
	MemFree(this->yuv2rgb);
	DEL_CLASS(this->irFunc);
	DEL_CLASS(this->igFunc);
	DEL_CLASS(this->ibFunc);
	DEL_CLASS(this->frFunc);
	DEL_CLASS(this->fgFunc);
	DEL_CLASS(this->fbFunc);
}

void Media::CS::CSYUV10_RGB8::YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuv)
{
	MemCopyNO(&this->yuvParam, yuv.Ptr(), sizeof(YUVPARAM));
	this->yuvUpdated = true;
}

void Media::CS::CSYUV10_RGB8::RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgb)
{
	NotNullPtr<const Media::ColorProfile> srcColor;
	NotNullPtr<const Media::ColorProfile> destColor;
	if (this->srcColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		srcColor = this->colorSess->GetDefVProfile();
	}
	else if (this->srcColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		srcColor = this->colorSess->GetDefPProfile();
	}
	else if (this->srcColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || this->srcColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		srcColor = rgb->monProfile;
	}
	else
	{
		srcColor = this->srcColor;
	}
	if (this->destColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || this->destColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		destColor = rgb->monProfile;
	}
	else if (this->destColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN || this->destColor.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		destColor = srcColor;
	}
	else
	{
		destColor = this->destColor;
	}

	DEL_CLASS(this->irFunc);
	DEL_CLASS(this->igFunc);
	DEL_CLASS(this->ibFunc);
	this->irFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetRTranParamRead());
	this->igFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetGTranParamRead());
	this->ibFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetBTranParamRead());
	DEL_CLASS(this->frFunc);
	DEL_CLASS(this->fgFunc);
	DEL_CLASS(this->fbFunc);
	this->frFunc = Media::CS::TransferFunc::CreateFunc(destColor->GetRTranParamRead());
	this->fgFunc = Media::CS::TransferFunc::CreateFunc(destColor->GetGTranParamRead());
	this->fbFunc = Media::CS::TransferFunc::CreateFunc(destColor->GetBTranParamRead());
	this->rgbParam.Set(rgb);
	this->rgbUpdated = true;
}

UOSInt Media::CS::CSYUV10_RGB8::GetDestFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 2;
}
