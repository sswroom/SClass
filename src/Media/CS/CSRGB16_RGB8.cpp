#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSRGB16_RGB8.h"
#include "Media/CS/TransferFunc.h"
#include "Math/Math.h"

extern "C"
{
	void CSRGB16_RGB8_Convert(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UOSInt srcNBits, UOSInt destNbits, UInt8 *rgbTable);
}

void Media::CS::CSRGB16_RGB8::UpdateRGBTable()
{
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

	if (this->rgbTable == 0)
	{
		this->rgbTable = MemAlloc(UInt8, 65536 * 3);
	}
	Media::ColorProfile *srcProfile;
	Media::ColorProfile *destProfile;
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
	if (this->destProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY)
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
		destProfile = this->rgbParam->monProfile;
	}
	else if (this->destProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
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
		destProfile = this->rgbParam->monProfile;
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

		if (this->destProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN || this->destProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
		{
			destProfile = srcProfile;
		}
		else
		{
			destProfile = this->destProfile;
		}
	}
	Media::CS::TransferFunc *irFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetRTranParam());
	Media::CS::TransferFunc *igFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetGTranParam());
	Media::CS::TransferFunc *ibFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetBTranParam());
	Media::CS::TransferFunc *frFunc = Media::CS::TransferFunc::CreateFunc(destProfile->GetRTranParam());
	Media::CS::TransferFunc *fgFunc = Media::CS::TransferFunc::CreateFunc(destProfile->GetGTranParam());
	Media::CS::TransferFunc *fbFunc = Media::CS::TransferFunc::CreateFunc(destProfile->GetBTranParam());
	OSInt i;

	i = 65536;
	while (i-- > 0)
	{
		Double rv = frFunc->ForwardTransfer(irFunc->InverseTransfer(i / 65535.0) * rMul);
		Double gv = fgFunc->ForwardTransfer(igFunc->InverseTransfer(i / 65535.0) * gMul);
		Double bv = fbFunc->ForwardTransfer(ibFunc->InverseTransfer(i / 65535.0) * bMul);
		Double rV = (rBright - 1.0 + Math::Pow(rv, rGammaVal) * rContr) * 255.0;
		Double gV = (gBright - 1.0 + Math::Pow(gv, gGammaVal) * gContr) * 255.0;
		Double bV = (bBright - 1.0 + Math::Pow(bv, bGammaVal) * bContr) * 255.0;
		if (rV > 255.0)
			this->rgbTable[i + 131072] = 255;
		else if (rV < 0)
			this->rgbTable[i + 131072] = 0;
		else
			this->rgbTable[i + 131072] = (UInt8)Math::Double2Int32(rV);
		if (gV > 255.0)
			this->rgbTable[i + 65536] = 255;
		else if (gV < 0)
			this->rgbTable[i + 65536] = 0;
		else
			this->rgbTable[i + 65536] = (UInt8)Math::Double2Int32(gV);
		if (bV > 255.0)
			this->rgbTable[i + 0] = 255;
		else if (bV < 0)
			this->rgbTable[i + 0] = 0;
		else
			this->rgbTable[i + 0] = (UInt8)Math::Double2Int32(bV);
	}
	DEL_CLASS(irFunc);
	DEL_CLASS(igFunc);
	DEL_CLASS(ibFunc);
	DEL_CLASS(frFunc);
	DEL_CLASS(fgFunc);
	DEL_CLASS(fbFunc);
}

Media::CS::CSRGB16_RGB8::CSRGB16_RGB8(UOSInt srcNBits, Media::PixelFormat srcPF, UOSInt destNBits, Media::PixelFormat destPF, Bool invert, const Media::ColorProfile *srcProfile, const Media::ColorProfile *destProfile, Media::ColorManagerSess *colorSess) : Media::CS::CSConverter(colorSess)
{
	this->srcNBits = srcNBits;
	this->srcPF = srcPF;
	this->destNBits = destNBits;
	this->destPF = destPF;
	NEW_CLASS(this->rgbParam, Media::IColorHandler::RGBPARAM2());
	NEW_CLASS(this->srcProfile, Media::ColorProfile(srcProfile));
	NEW_CLASS(this->destProfile, Media::ColorProfile(destProfile));
	this->invert = invert;

	this->rgbParam->Set(colorSess->GetRGBParam());
	this->rgbTable = 0;
	this->rgbUpdated = true;
}

Media::CS::CSRGB16_RGB8::~CSRGB16_RGB8()
{
	if (this->rgbTable)
	{
		MemFree(this->rgbTable);
		this->rgbTable = 0;
	}
	DEL_CLASS(this->rgbParam);
	DEL_CLASS(this->srcProfile);
	DEL_CLASS(this->destProfile);
}

void Media::CS::CSRGB16_RGB8::ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	if (this->rgbUpdated)
	{
		UpdateRGBTable();
		this->rgbUpdated = false;
	}
	if (invert)
	{
		destPtr = ((UInt8*)destPtr) + (OSInt)(srcStoreHeight - 1) * destRGBBpl;
		destRGBBpl = -destRGBBpl;
	}
	CSRGB16_RGB8_Convert(srcPtr[0], destPtr, dispWidth, dispHeight, srcStoreWidth * srcNBits >> 3, destRGBBpl, srcNBits, destNBits, this->rgbTable);
}

UOSInt Media::CS::CSRGB16_RGB8::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return width * height * (this->srcNBits >> 3);
}

UOSInt Media::CS::CSRGB16_RGB8::GetDestFrameSize(UOSInt width, UOSInt height)
{
	return width * height * (this->destNBits >> 3);
}

void Media::CS::CSRGB16_RGB8::YUVParamChanged(const YUVPARAM *yuv)
{
}

void Media::CS::CSRGB16_RGB8::RGBParamChanged(const RGBPARAM2 *rgb)
{
	this->rgbParam->Set(rgb);
	this->rgbUpdated = true;
}

