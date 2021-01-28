#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSRGB8_RGB8.h"
#include "Media/CS/TransferFunc.h"
#include "Math/Math.h"

extern "C"
{
	void CSRGB8_RGB8_UpdateRGBTablePal(UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable, UOSInt nColor);
	void CSRGB8_RGB8_Convert(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UOSInt srcNBits, UOSInt destNbits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable);
}

void Media::CS::CSRGB8_RGB8::UpdateRGBTable()
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
		this->rgbTable = MemAlloc(UInt8, 768);
	}
	Media::ColorProfile *srcProfile;
	Media::ColorProfile *destProfile;
	if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		srcProfile = this->colorSess->GetDefVProfile();
	}
	else if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		srcProfile = this->colorSess->GetDefPProfile();
	}
	else if (this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || this->srcColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		srcProfile = this->rgbParam->monProfile;
	}
	else
	{
		srcProfile = this->srcColor;
	}
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
		destProfile = this->rgbParam->monProfile;
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
		if (this->destColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN || this->destColor->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
		{
			destProfile = srcProfile;
		}
		else
		{
			destProfile = this->destColor;
		}
	}
	Media::CS::TransferFunc *irFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetRTranParam());
	Media::CS::TransferFunc *igFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetGTranParam());
	Media::CS::TransferFunc *ibFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetBTranParam());
	Media::CS::TransferFunc *frFunc = Media::CS::TransferFunc::CreateFunc(destProfile->GetRTranParam());
	Media::CS::TransferFunc *fgFunc = Media::CS::TransferFunc::CreateFunc(destProfile->GetGTranParam());
	Media::CS::TransferFunc *fbFunc = Media::CS::TransferFunc::CreateFunc(destProfile->GetBTranParam());
	OSInt i;


	i = 256;
	while (i-- > 0)
	{
		Double rv = frFunc->ForwardTransfer(irFunc->InverseTransfer(i / 255.0) * rMul);
		Double gv = fgFunc->ForwardTransfer(igFunc->InverseTransfer(i / 255.0) * gMul);
		Double bv = fbFunc->ForwardTransfer(ibFunc->InverseTransfer(i / 255.0) * bMul);
		Double rV = (rBright - 1.0 + Math::Pow(rv, rGammaVal) * rContr) * 255.0;
		Double gV = (gBright - 1.0 + Math::Pow(gv, gGammaVal) * gContr) * 255.0;
		Double bV = (bBright - 1.0 + Math::Pow(bv, bGammaVal) * bContr) * 255.0;
		if (rV > 255.0)
			this->rgbTable[i + 512] = 255;
		else if (rV < 0)
			this->rgbTable[i + 512] = 0;
		else
			this->rgbTable[i + 512] = (UInt8)Math::Double2Int32(rV);
		if (gV > 255.0)
			this->rgbTable[i + 256] = 255;
		else if (gV < 0)
			this->rgbTable[i + 256] = 0;
		else
			this->rgbTable[i + 256] = (UInt8)Math::Double2Int32(gV);
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

	if (this->srcPal)
	{
		CSRGB8_RGB8_UpdateRGBTablePal(this->srcPal, this->destPal, this->rgbTable, 1 << this->srcNBits);
	}
}

Media::CS::CSRGB8_RGB8::CSRGB8_RGB8(UOSInt srcNBits, Media::PixelFormat srcPF, UOSInt destNBits, Media::PixelFormat destPF, Bool invert, const Media::ColorProfile *srcColor, const Media::ColorProfile *destColor, Media::ColorManagerSess *colorSess) : Media::CS::CSConverter(colorSess)
{
	this->srcNBits = srcNBits;
	this->destNBits = destNBits;
	NEW_CLASS(this->rgbParam, Media::IColorHandler::RGBPARAM2());
	NEW_CLASS(this->srcColor, Media::ColorProfile(srcColor));
	NEW_CLASS(this->destColor, Media::ColorProfile(destColor));
	this->invert = invert;
	this->srcPal = 0;
	this->destPal = 0;

	this->rgbParam->Set(colorSess->GetRGBParam());
	if (this->srcNBits <= 8)
	{
		this->srcPal = MemAlloc(UInt8, 4 << this->srcNBits);
		this->destPal = MemAlloc(UInt8, 4 << this->srcNBits);
	}
	this->rgbTable = 0;
	this->rgbUpdated = true;
}

Media::CS::CSRGB8_RGB8::~CSRGB8_RGB8()
{
	if (this->rgbTable)
	{
		MemFree(this->rgbTable);
		this->rgbTable = 0;
	}
	if (this->srcPal)
	{
		MemFree(this->srcPal);
		this->srcPal = 0;
	}
	if (this->destPal)
	{
		MemFree(this->destPal);
		this->destPal = 0;
	}
	DEL_CLASS(this->srcColor);
	DEL_CLASS(this->destColor);
	DEL_CLASS(this->rgbParam);
}

void Media::CS::CSRGB8_RGB8::ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
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
	CSRGB8_RGB8_Convert(srcPtr[0], destPtr, dispWidth, dispHeight, srcStoreWidth * srcNBits >> 3, destRGBBpl, srcNBits, destNBits, this->srcPal, this->destPal, this->rgbTable);
}

UOSInt Media::CS::CSRGB8_RGB8::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return width * height * (this->srcNBits >> 3);
}

UOSInt Media::CS::CSRGB8_RGB8::GetDestFrameSize(UOSInt width, UOSInt height)
{
	return width * height * (this->destNBits >> 3);
}

void Media::CS::CSRGB8_RGB8::SetPalette(UInt8 *pal)
{
	if (this->srcPal)
	{
		MemCopyNO(this->srcPal, pal, 4 << this->srcNBits);
		this->rgbUpdated = true;
	}
}

void Media::CS::CSRGB8_RGB8::YUVParamChanged(const YUVPARAM *yuv)
{
}

void Media::CS::CSRGB8_RGB8::RGBParamChanged(const RGBPARAM2 *rgb)
{
	this->rgbParam->Set(rgb);
	this->rgbUpdated = true;
}

