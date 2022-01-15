#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSYUV_RGB8.h"
#include "Math/Math.h"

void Media::CS::CSYUV_RGB8::SetupRGB13_8()
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

	if (this->destColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY)
	{
		rGammaVal = this->rgbParam->MonRGamma;
		gGammaVal = this->rgbParam->MonGGamma;
		bGammaVal = this->rgbParam->MonBGamma;
		rBright = this->rgbParam->MonRBright;
		gBright = this->rgbParam->MonGBright;
		bBright = this->rgbParam->MonBBright;
		rContr = this->rgbParam->MonRContr;
		gContr = this->rgbParam->MonGContr;
		bContr = this->rgbParam->MonBContr;
		tMul = this->rgbParam->MonVBrightness;
		rMul = this->rgbParam->MonRBrightness * tMul;
		gMul = this->rgbParam->MonGBrightness * tMul;
		bMul = this->rgbParam->MonBBrightness * tMul;
	}
	else if (this->destColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		rGammaVal = this->rgbParam->MonRGamma;
		gGammaVal = this->rgbParam->MonGGamma;
		bGammaVal = this->rgbParam->MonBGamma;
		rBright = this->rgbParam->MonRBright;
		gBright = this->rgbParam->MonGBright;
		bBright = this->rgbParam->MonBBright;
		rContr = this->rgbParam->MonRContr;
		gContr = this->rgbParam->MonGContr;
		bContr = this->rgbParam->MonBContr;
		tMul = this->rgbParam->MonPBrightness;
		rMul = this->rgbParam->MonRBrightness * tMul;
		gMul = this->rgbParam->MonGBrightness * tMul;
		bMul = this->rgbParam->MonBBrightness * tMul;
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
			rgbGammaCorr[i] = (UInt8)Math::Double2Int32(tmpV);

		thisV = this->fgFunc->ForwardTransfer(this->igFunc->InverseTransfer(i / 32767.0 * 4.0) * gMul);
		tmpV = (gBright - 1.0 + Math_Pow(thisV, gGammaVal) * gContr) * 255.0;
		if (tmpV < 0)
			rgbGammaCorr[i+65536] = 0;
		else if (tmpV > 255)
			rgbGammaCorr[i+65536] = 255;
		else
			rgbGammaCorr[i+65536] = (UInt8)Math::Double2Int32(tmpV);

		thisV = this->fbFunc->ForwardTransfer(this->ibFunc->InverseTransfer(i / 32767.0 * 4.0) * bMul);
		tmpV = (bBright - 1.0 + Math_Pow(thisV, bGammaVal) * bContr) * 255.0;
		if (tmpV < 0)
			rgbGammaCorr[i+131072] = 0;
		else if (tmpV > 255)
			rgbGammaCorr[i+131072] = 255;
		else
			rgbGammaCorr[i+131072] = (UInt8)Math::Double2Int32(tmpV);
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
			rgbGammaCorr[i] = (UInt8)Math::Double2Int32(tmpV);

		thisV = this->fgFunc->ForwardTransfer(this->igFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0) * gMul);
		tmpV = (gBright - 1.0 + Math_Pow(thisV, gGammaVal) * gContr) * 255.0;
		if (tmpV < 0)
			rgbGammaCorr[i+65536] = 0;
		else if (tmpV > 255)
			rgbGammaCorr[i+65536] = 255;
		else
			rgbGammaCorr[i+65536] = (UInt8)Math::Double2Int32(tmpV);

		thisV = this->fbFunc->ForwardTransfer(this->ibFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0) * bMul);
		tmpV = (bBright - 1.0 + Math_Pow(thisV, bGammaVal) * bContr) * 255.0;
		if (tmpV < 0)
			rgbGammaCorr[i+131072] = 0;
		else if (tmpV > 255)
			rgbGammaCorr[i+131072] = 255;
		else
			rgbGammaCorr[i+131072] = (UInt8)Math::Double2Int32(tmpV);
	}
}

void Media::CS::CSYUV_RGB8::SetupYUV_RGB13()
{
	Double sumy[256];
	Double sumc[256];
	Double currSum;
	Double ynor;
	Double cnor;
	Double c;

	OSInt i;

	OSInt updateY = 1;
	OSInt updateC = 1;
	if (updateY)
	{
		i = 16;
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
		Int32 y;
		i = 256;
		while (i--)
		{
			y = (Int32)(0x7fff * (Math_Pow(sumy[i] * currSum, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
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
			if (this->colorSess)
			{
				yuvType = this->colorSess->GetDefYUVType();
			}
			else
			{
				yuvType = Media::ColorProfile::YUVT_BT601;
			}
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

		i = 112;
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
}

Media::CS::CSYUV_RGB8::CSYUV_RGB8(const Media::ColorProfile *srcColor, const  Media::ColorProfile *destColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess) : Media::CS::CSConverter(colorSess)
{
	NEW_CLASS(this->srcColor, Media::ColorProfile(srcColor));
	NEW_CLASS(this->destColor, Media::ColorProfile(destColor));
	NEW_CLASS(this->rgbParam, Media::IColorHandler::RGBPARAM2());
	this->yuvType = yuvType;
	this->rgbGammaCorr = MemAlloc(UInt8, 65536 * 3);
	this->yuv2rgb = MemAlloc(Int64, 768);

	this->rgbUpdated = true;
	this->yuvUpdated = true;

	if (colorSess)
	{
		MemCopyNO(&this->yuvParam, colorSess->GetYUVParam(), sizeof(YUVPARAM));
		this->rgbParam->Set(colorSess->GetRGBParam());
	}
	else
	{
		Media::MonitorColorManager::SetDefaultYUV(&this->yuvParam);
		Media::MonitorColorManager::SetDefaultRGB(this->rgbParam);
	}

	Media::CS::TransferParam *srcRTran;
	Media::CS::TransferParam *srcGTran;
	Media::CS::TransferParam *srcBTran;
	Media::CS::TransferParam *destRTran;
	Media::CS::TransferParam *destGTran;
	Media::CS::TransferParam *destBTran;
	if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		if (this->colorSess)
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefVProfile();
			NEW_CLASS(srcRTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(srcGTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(srcBTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
		else
		{
			NEW_CLASS(srcRTran, Media::CS::TransferParam(Media::CS::TRANT_BT709, 2.2));
			NEW_CLASS(srcGTran, Media::CS::TransferParam(Media::CS::TRANT_BT709, 2.2));
			NEW_CLASS(srcBTran, Media::CS::TransferParam(Media::CS::TRANT_BT709, 2.2));
		}
	}
	else if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		if (this->colorSess)
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefPProfile();
			NEW_CLASS(srcRTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(srcGTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(srcBTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
		else
		{
			NEW_CLASS(srcRTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(srcGTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(srcBTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
	}
	else if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		NEW_CLASS(srcRTran, Media::CS::TransferParam(this->rgbParam->monProfile->GetRTranParam()));
		NEW_CLASS(srcGTran, Media::CS::TransferParam(this->rgbParam->monProfile->GetGTranParam()));
		NEW_CLASS(srcBTran, Media::CS::TransferParam(this->rgbParam->monProfile->GetBTranParam()));
	}
	else
	{
		NEW_CLASS(srcRTran, Media::CS::TransferParam(this->srcColor->GetRTranParam()));
		NEW_CLASS(srcGTran, Media::CS::TransferParam(this->srcColor->GetGTranParam()));
		NEW_CLASS(srcBTran, Media::CS::TransferParam(this->srcColor->GetBTranParam()));
	}
	if (this->destColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || this->destColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		NEW_CLASS(destRTran, Media::CS::TransferParam(this->rgbParam->monProfile->GetRTranParam()));
		NEW_CLASS(destGTran, Media::CS::TransferParam(this->rgbParam->monProfile->GetGTranParam()));
		NEW_CLASS(destBTran, Media::CS::TransferParam(this->rgbParam->monProfile->GetBTranParam()));
	}
	else if (this->destColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN || this->destColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		NEW_CLASS(destRTran, Media::CS::TransferParam(srcRTran));
		NEW_CLASS(destGTran, Media::CS::TransferParam(srcGTran));
		NEW_CLASS(destBTran, Media::CS::TransferParam(srcBTran));
	}
	else
	{
		NEW_CLASS(destRTran, Media::CS::TransferParam(this->destColor->GetRTranParam()));
		NEW_CLASS(destGTran, Media::CS::TransferParam(this->destColor->GetGTranParam()));
		NEW_CLASS(destBTran, Media::CS::TransferParam(this->destColor->GetBTranParam()));
	}
	this->irFunc = Media::CS::TransferFunc::CreateFunc(srcRTran);
	this->igFunc = Media::CS::TransferFunc::CreateFunc(srcGTran);
	this->ibFunc = Media::CS::TransferFunc::CreateFunc(srcBTran);
	this->frFunc = Media::CS::TransferFunc::CreateFunc(destRTran);
	this->fgFunc = Media::CS::TransferFunc::CreateFunc(destGTran);
	this->fbFunc = Media::CS::TransferFunc::CreateFunc(destBTran);
	DEL_CLASS(srcRTran);
	DEL_CLASS(srcGTran);
	DEL_CLASS(srcBTran);
	DEL_CLASS(destRTran);
	DEL_CLASS(destGTran);
	DEL_CLASS(destBTran);
}

Media::CS::CSYUV_RGB8::~CSYUV_RGB8()
{
	MemFree(this->rgbGammaCorr);
	MemFree(this->yuv2rgb);
	DEL_CLASS(this->irFunc);
	DEL_CLASS(this->igFunc);
	DEL_CLASS(this->ibFunc);
	DEL_CLASS(this->frFunc);
	DEL_CLASS(this->fgFunc);
	DEL_CLASS(this->fbFunc);
	DEL_CLASS(this->srcColor);
	DEL_CLASS(this->destColor);
	DEL_CLASS(this->rgbParam);
}

void Media::CS::CSYUV_RGB8::UpdateTable()
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

void Media::CS::CSYUV_RGB8::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuv)
{
	MemCopyNO(&this->yuvParam, yuv, sizeof(YUVPARAM));
	this->yuvUpdated = true;
}

void Media::CS::CSYUV_RGB8::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgb)
{
	Media::ColorProfile *srcColor;
	Media::ColorProfile *destColor;
	if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		srcColor = this->colorSess->GetDefVProfile();
	}
	else if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		srcColor = this->colorSess->GetDefPProfile();
	}
	else if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		srcColor = rgb->monProfile;
	}
	else
	{
		srcColor = this->srcColor;
	}
	if (this->destColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || this->destColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		destColor = rgb->monProfile;
	}
	else if (this->destColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN || this->destColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
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
	this->irFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetRTranParam());
	this->igFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetGTranParam());
	this->ibFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetBTranParam());
	DEL_CLASS(this->frFunc);
	DEL_CLASS(this->fgFunc);
	DEL_CLASS(this->fbFunc);
	this->frFunc = Media::CS::TransferFunc::CreateFunc(destColor->GetRTranParam());
	this->fgFunc = Media::CS::TransferFunc::CreateFunc(destColor->GetGTranParam());
	this->fbFunc = Media::CS::TransferFunc::CreateFunc(destColor->GetBTranParam());
	this->rgbParam->Set(rgb);
	this->rgbUpdated = true;
}

UOSInt Media::CS::CSYUV_RGB8::GetDestFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 2;
}
