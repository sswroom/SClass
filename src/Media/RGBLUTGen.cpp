#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/RGBLUTGen.h"
#include "Media/CS/TransferFunc.h"

Media::RGBLUTGen::RGBLUTGen(Media::ColorSess *colorSess)
{
	this->colorSess = colorSess;
}

Media::RGBLUTGen::~RGBLUTGen()
{
}

void Media::RGBLUTGen::GenRGB8_LRGB(UInt16 *rgbTable, Media::ColorProfile *srcProfile, Int32 nBitLRGB)
{
	const Media::IColorHandler::RGBPARAM2 *rgbParam;
	Media::CS::TransferParam *rTran;
	Media::CS::TransferParam *gTran;
	Media::CS::TransferParam *bTran;
	if (srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			rgbParam = this->colorSess->GetRGBParam();
			NEW_CLASS(rTran, Media::CS::TransferParam(rgbParam->monProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(rgbParam->monProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(rgbParam->monProfile->GetBTranParam()));
		}
	}
	else if (srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefVProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefPProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else
	{
		NEW_CLASS(rTran, Media::CS::TransferParam(srcProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(srcProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(srcProfile->GetBTranParam()));
	}
	Double maxRGBVal = (1 << nBitLRGB) - 1;
	Media::CS::TransferFunc *irFunc = Media::CS::TransferFunc::CreateFunc(rTran);
	Media::CS::TransferFunc *igFunc = Media::CS::TransferFunc::CreateFunc(gTran);
	Media::CS::TransferFunc *ibFunc = Media::CS::TransferFunc::CreateFunc(bTran);
	UOSInt i;
	i = 256;
	while (i-- > 0)
	{
		rgbTable[i] = (UInt16)Math::Double2Int32(irFunc->InverseTransfer(Math::UOSInt2Double(i) / 255.0) * maxRGBVal);
		rgbTable[i + 256] = (UInt16)Math::Double2Int32(igFunc->InverseTransfer(Math::UOSInt2Double(i) / 255.0) * maxRGBVal);
		rgbTable[i + 512] = (UInt16)Math::Double2Int32(ibFunc->InverseTransfer(Math::UOSInt2Double(i) / 255.0) * maxRGBVal);
	}
	DEL_CLASS(irFunc);
	DEL_CLASS(igFunc);
	DEL_CLASS(ibFunc);
	DEL_CLASS(rTran);
	DEL_CLASS(gTran);
	DEL_CLASS(bTran);
}

void Media::RGBLUTGen::GenRGBA8_LRGBC(Int64 *rgbTable, Media::ColorProfile *srcProfile, Media::ColorProfile::ColorPrimaries *destPrimaries, Int32 nBitLRGB)
{
	const Media::IColorHandler::RGBPARAM2 *rgbParam;
	Media::CS::TransferParam *rTran;
	Media::CS::TransferParam *gTran;
	Media::CS::TransferParam *bTran;
	if (srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			rgbParam = this->colorSess->GetRGBParam();
			NEW_CLASS(rTran, Media::CS::TransferParam(rgbParam->monProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(rgbParam->monProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(rgbParam->monProfile->GetBTranParam()));
		}
	}
	else if (srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefVProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefPProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else
	{
		NEW_CLASS(rTran, Media::CS::TransferParam(srcProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(srcProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(srcProfile->GetBTranParam()));
	}
	Double maxRGBVal = (1 << nBitLRGB) - 1;
	Media::CS::TransferFunc *irFunc = Media::CS::TransferFunc::CreateFunc(rTran);
	Media::CS::TransferFunc *igFunc = Media::CS::TransferFunc::CreateFunc(gTran);
	Media::CS::TransferFunc *ibFunc = Media::CS::TransferFunc::CreateFunc(bTran);

	Math::Matrix3 mat1;
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Matrix3 mat4;
	Math::Matrix3 mat5;
	Math::Vector3 vec1;
	Math::Vector3 vec2;
	Math::Vector3 vec3;
	srcProfile->primaries.GetConvMatrix(&mat1);
	if (destPrimaries->colorType == Media::ColorProfile::CT_DISPLAY)
	{
		if (this->colorSess)
		{
			rgbParam = this->colorSess->GetRGBParam();
			Media::ColorProfile::ColorPrimaries primaries;
			primaries.Set(rgbParam->monProfile->GetPrimaries());
			primaries.GetConvMatrix(&mat5);
			vec2.Set(rgbParam->monProfile->GetPrimaries()->wx, rgbParam->monProfile->GetPrimaries()->wy, 1.0);
		}
		else
		{
			Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(Media::ColorProfile::WPT_D65, &vec2);
		}
	}
	else
	{
		destPrimaries->GetConvMatrix(&mat5);
		vec2.Set(destPrimaries->wx, destPrimaries->wy, 1.0);
	}
	mat5.Inverse();

	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(&mat2);
	mat3.Set(&mat2);
	mat4.SetIdentity();
	vec1.Set(srcProfile->primaries.wx, srcProfile->primaries.wy, 1.0);
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

	Double thisV;

	Int16 v[4];
	Double cV;

	UOSInt i;
	i = 256;
	while (i-- > 0)
	{
		thisV = irFunc->InverseTransfer(Math::UOSInt2Double(i) / 255.0) * maxRGBVal;
		v[2] = Math::SDouble2Int16(thisV * mat1.vec[0].val[0]);
		v[1] = Math::SDouble2Int16(thisV * mat1.vec[1].val[0]);
		v[0] = Math::SDouble2Int16(thisV * mat1.vec[2].val[0]);
		v[3] = 0;
		rgbTable[i] = *(Int64*)&v[0];

		thisV = igFunc->InverseTransfer(Math::UOSInt2Double(i) / 255.0) * maxRGBVal;
		v[2] = Math::SDouble2Int16(thisV * mat1.vec[0].val[1]);
		v[1] = Math::SDouble2Int16(thisV * mat1.vec[1].val[1]);
		v[0] = Math::SDouble2Int16(thisV * mat1.vec[2].val[1]);
		v[3] = 0;
		rgbTable[i + 256] = *(Int64*)&v[0];

		thisV = ibFunc->InverseTransfer(Math::UOSInt2Double(i) / 255.0) * maxRGBVal;
		v[2] = Math::SDouble2Int16(thisV * mat1.vec[0].val[2]);
		v[1] = Math::SDouble2Int16(thisV * mat1.vec[1].val[2]);
		v[0] = Math::SDouble2Int16(thisV * mat1.vec[2].val[2]);
		v[3] = 0;
		rgbTable[i + 512] = *(Int64*)&v[0];

		cV = Math::UOSInt2Double(i) / 255.0 * maxRGBVal;
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		v[3] = (Int16)Math::Double2Int32(cV);
		rgbTable[i + 768] = *(Int64*)&v[0];
	}
	DEL_CLASS(irFunc);
	DEL_CLASS(igFunc);
	DEL_CLASS(ibFunc);
	DEL_CLASS(rTran);
	DEL_CLASS(gTran);
	DEL_CLASS(bTran);
}

void Media::RGBLUTGen::GenRGB16_LRGBC(Int64 *rgbTable, Media::ColorProfile *srcProfile, Media::ColorProfile::ColorPrimaries *destPrimaries, Int32 nBitLRGB)
{
	const Media::IColorHandler::RGBPARAM2 *rgbParam;
	Media::CS::TransferParam *rTran;
	Media::CS::TransferParam *gTran;
	Media::CS::TransferParam *bTran;
	if (srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY || srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			rgbParam = this->colorSess->GetRGBParam();
			NEW_CLASS(rTran, Media::CS::TransferParam(rgbParam->monProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(rgbParam->monProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(rgbParam->monProfile->GetBTranParam()));
		}
	}
	else if (srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefVProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefPProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else
	{
		NEW_CLASS(rTran, Media::CS::TransferParam(srcProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(srcProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(srcProfile->GetBTranParam()));
	}
	Double maxRGBVal = (1 << nBitLRGB) - 1;
	Media::CS::TransferFunc *irFunc = Media::CS::TransferFunc::CreateFunc(rTran);
	Media::CS::TransferFunc *igFunc = Media::CS::TransferFunc::CreateFunc(gTran);
	Media::CS::TransferFunc *ibFunc = Media::CS::TransferFunc::CreateFunc(bTran);

	Math::Matrix3 mat1;
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Matrix3 mat4;
	Math::Matrix3 mat5;
	Math::Vector3 vec1;
	Math::Vector3 vec2;
	Math::Vector3 vec3;
	srcProfile->GetPrimaries()->GetConvMatrix(&mat1);
	if (destPrimaries->colorType == Media::ColorProfile::CT_DISPLAY)
	{
		if (this->colorSess)
		{
			rgbParam = this->colorSess->GetRGBParam();
			Media::ColorProfile::ColorPrimaries primaries;
			primaries.Set(rgbParam->monProfile->GetPrimaries());
			primaries.GetConvMatrix(&mat5);
			vec2.Set(rgbParam->monProfile->GetPrimaries()->wx, rgbParam->monProfile->GetPrimaries()->wy, 1.0);
		}
		else
		{
			Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(Media::ColorProfile::WPT_D65, &vec2);
		}
	}
	else
	{
		destPrimaries->GetConvMatrix(&mat5);
		vec2.Set(destPrimaries->wx, destPrimaries->wy, 1.0);
	}
	mat5.Inverse();

	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(&mat2);
	mat3.Set(&mat2);
	mat4.SetIdentity();
	vec1.Set(srcProfile->GetPrimaries()->wx, srcProfile->GetPrimaries()->wy, 1.0);
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

	Double thisV;

	Int16 v[4];
	Double cV;

#if _OSINT_SIZE == 16
	UInt32 i;
#else
	UOSInt i;
#endif
	i = 65536;
	while (i-- > 0)
	{
		thisV = irFunc->InverseTransfer(Math::UOSInt2Double(i) / 65535.0) * maxRGBVal;
		v[2] = Math::SDouble2Int16(thisV * mat1.vec[0].val[0]);
		v[1] = Math::SDouble2Int16(thisV * mat1.vec[1].val[0]);
		v[0] = Math::SDouble2Int16(thisV * mat1.vec[2].val[0]);
		v[3] = 0;
		rgbTable[i] = *(Int64*)&v[0];

		thisV = igFunc->InverseTransfer(Math::UOSInt2Double(i) / 65535.0) * maxRGBVal;
		v[2] = Math::SDouble2Int16(thisV * mat1.vec[0].val[1]);
		v[1] = Math::SDouble2Int16(thisV * mat1.vec[1].val[1]);
		v[0] = Math::SDouble2Int16(thisV * mat1.vec[2].val[1]);
		v[3] = 0;
		rgbTable[i + 65536] = *(Int64*)&v[0];

		thisV = ibFunc->InverseTransfer(Math::UOSInt2Double(i) / 65535.0) * maxRGBVal;
		v[2] = Math::SDouble2Int16(thisV * mat1.vec[0].val[2]);
		v[1] = Math::SDouble2Int16(thisV * mat1.vec[1].val[2]);
		v[0] = Math::SDouble2Int16(thisV * mat1.vec[2].val[2]);
		v[3] = 0;
		rgbTable[i + 131072] = *(Int64*)&v[0];

		cV = Math::UOSInt2Double(i) / 255.0 * maxRGBVal;
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		v[3] = (Int16)Math::Double2Int32(cV);
		rgbTable[i + 196608] = *(Int64*)&v[0];
	}
	DEL_CLASS(ibFunc);
	DEL_CLASS(igFunc);
	DEL_CLASS(irFunc);
	DEL_CLASS(rTran);
	DEL_CLASS(gTran);
	DEL_CLASS(bTran);
}

void Media::RGBLUTGen::GenLRGB_BGRA8(UInt8 *rgbTable, Media::ColorProfile *destProfile, Int32 nBitLRGB, Double srcRefLuminance)
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

	Media::CS::TransferParam *rTran;
	Media::CS::TransferParam *gTran;
	Media::CS::TransferParam *bTran;
	Media::CS::TransferType tranType = destProfile->GetRTranParam()->GetTranType();

	if (tranType == Media::CS::TRANT_VDISPLAY && this->colorSess == 0)
	{
		tranType = Media::CS::TRANT_sRGB;
		NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
	}
	else if (tranType == Media::CS::TRANT_PDISPLAY && this->colorSess == 0)
	{
		tranType = Media::CS::TRANT_sRGB;
		NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
	}
	else if (tranType == Media::CS::TRANT_VUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefVProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (tranType == Media::CS::TRANT_PUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefPProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (tranType == Media::CS::TRANT_VDISPLAY || tranType == Media::CS::TRANT_PDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();
		NEW_CLASS(rTran, Media::CS::TransferParam(rgbParam->monProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(rgbParam->monProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(rgbParam->monProfile->GetBTranParam()));
	}
	else
	{
		NEW_CLASS(rTran, Media::CS::TransferParam(destProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(destProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(destProfile->GetBTranParam()));
	}

	if (tranType == Media::CS::TRANT_VDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();

		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonVBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;

		if (srcRefLuminance != 0)
		{
			rMul = rMul * srcRefLuminance / rgbParam->monLuminance;
			gMul = gMul * srcRefLuminance / rgbParam->monLuminance;
			bMul = bMul * srcRefLuminance / rgbParam->monLuminance;
		}
	}
	else if (tranType == Media::CS::TRANT_PDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();

		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonPBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;

		if (srcRefLuminance != 0)
		{
			rMul = rMul * srcRefLuminance / rgbParam->monLuminance;
			gMul = gMul * srcRefLuminance / rgbParam->monLuminance;
			bMul = bMul * srcRefLuminance / rgbParam->monLuminance;
		}
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

	Media::CS::TransferFunc *frFunc = Media::CS::TransferFunc::CreateFunc(rTran);
	Media::CS::TransferFunc *fgFunc = Media::CS::TransferFunc::CreateFunc(gTran);
	Media::CS::TransferFunc *fbFunc = Media::CS::TransferFunc::CreateFunc(bTran);
	Int32 ibitVal = (1 << nBitLRGB) - 1;
	Double bitVal = ibitVal;
#if _OSINT_SIZE == 16
	Int32 i;
#else
	OSInt i;
#endif
	i = 65536;
	while (i-- > 0)
	{
		Double rv = frFunc->ForwardTransfer(((Int16)i) / bitVal * rMul);
		Double gv = fgFunc->ForwardTransfer(((Int16)i) / bitVal * gMul);
		Double bv = fbFunc->ForwardTransfer(((Int16)i) / bitVal * bMul);
		if (rv < 0)
			rv = 0;
		if (gv < 0)
			gv = 0;
		if (bv < 0)
			bv = 0;
		Double rV = (rBright - 1.0 + Math_Pow(rv, rGammaVal) * rContr) * 255.0;
		Double gV = (gBright - 1.0 + Math_Pow(gv, gGammaVal) * gContr) * 255.0;
		Double bV = (bBright - 1.0 + Math_Pow(bv, bGammaVal) * bContr) * 255.0;
		if (i > 32767)
			rgbTable[i + 196608] = 0;
		else if (i > ibitVal)
			rgbTable[i + 196608] = 255;
		else
			rgbTable[i + 196608] = (UInt8)((i >> (nBitLRGB - 8)) & 0xff);
		if (rV > 255.0)
			rgbTable[i + 131072] = 255;
		else if (rV < 0)
			rgbTable[i + 131072] = 0;
		else
			rgbTable[i + 131072] = (UInt8)Math::Double2Int32(rV);
		if (gV > 255.0)
			rgbTable[i + 65536] = 255;
		else if (gV < 0)
			rgbTable[i + 65536] = 0;
		else
			rgbTable[i + 65536] = (UInt8)Math::Double2Int32(gV);
		if (bV > 255.0)
			rgbTable[i + 0] = 255;
		else if (bV < 0)
			rgbTable[i + 0] = 0;
		else
			rgbTable[i + 0] = (UInt8)Math::Double2Int32(bV);
	}
	DEL_CLASS(frFunc);
	DEL_CLASS(fgFunc);
	DEL_CLASS(fbFunc);
	DEL_CLASS(rTran);
	DEL_CLASS(gTran);
	DEL_CLASS(bTran);
}

void Media::RGBLUTGen::GenLRGB_RGB16(UInt8 *rgbTable, Media::ColorProfile *destProfile, Int32 nBitLRGB, Double srcRefLuminance)
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

	Media::CS::TransferParam *rTran;
	Media::CS::TransferParam *gTran;
	Media::CS::TransferParam *bTran;
	Media::CS::TransferType tranType = destProfile->GetRTranParam()->GetTranType();

	if (tranType == Media::CS::TRANT_VDISPLAY && this->colorSess == 0)
	{
		tranType = Media::CS::TRANT_sRGB;
		NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
	}
	else if (tranType == Media::CS::TRANT_PDISPLAY && this->colorSess == 0)
	{
		tranType = Media::CS::TRANT_sRGB;
		NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
	}
	else if (tranType == Media::CS::TRANT_VUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefVProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (tranType == Media::CS::TRANT_PUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefPProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (tranType == Media::CS::TRANT_VDISPLAY || tranType == Media::CS::TRANT_PDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();
		NEW_CLASS(rTran, Media::CS::TransferParam(rgbParam->monProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(rgbParam->monProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(rgbParam->monProfile->GetBTranParam()));
	}
	else
	{
		NEW_CLASS(rTran, Media::CS::TransferParam(destProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(destProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(destProfile->GetBTranParam()));
	}

	if (tranType == Media::CS::TRANT_VDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();

		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonVBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;

		if (srcRefLuminance != 0)
		{
			rMul = rMul * srcRefLuminance / rgbParam->monLuminance;
			gMul = gMul * srcRefLuminance / rgbParam->monLuminance;
			bMul = bMul * srcRefLuminance / rgbParam->monLuminance;
		}
	}
	else if (tranType == Media::CS::TRANT_PDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();

		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonPBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;

		if (srcRefLuminance != 0)
		{
			rMul = rMul * srcRefLuminance / rgbParam->monLuminance;
			gMul = gMul * srcRefLuminance / rgbParam->monLuminance;
			bMul = bMul * srcRefLuminance / rgbParam->monLuminance;
		}
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

	Media::CS::TransferFunc *frFunc = Media::CS::TransferFunc::CreateFunc(rTran);
	Media::CS::TransferFunc *fgFunc = Media::CS::TransferFunc::CreateFunc(gTran);
	Media::CS::TransferFunc *fbFunc = Media::CS::TransferFunc::CreateFunc(bTran);
	Int32 ibitVal = (1 << nBitLRGB) - 1;
	Double bitVal = ibitVal;
#if _OSINT_SIZE == 16
	Int32 i;
#else
	OSInt i;
#endif
	i = 65536;
	while (i-- > 0)
	{
		Double rv = frFunc->ForwardTransfer(((Int16)i) / bitVal * rMul);
		Double gv = fgFunc->ForwardTransfer(((Int16)i) / bitVal * gMul);
		Double bv = fbFunc->ForwardTransfer(((Int16)i) / bitVal * bMul);
		if (rv < 0)
			rv = 0;
		if (gv < 0)
			gv = 0;
		if (bv < 0)
			bv = 0;
		Double rV = (rBright - 1.0 + Math_Pow(rv, rGammaVal) * rContr) * 31.0;
		Double gV = (gBright - 1.0 + Math_Pow(gv, gGammaVal) * gContr) * 63.0;
		Double bV = (bBright - 1.0 + Math_Pow(bv, bGammaVal) * bContr) * 31.0;
		if (rV > 31.0)
			*(UInt16*)&rgbTable[i * 2 + 262144] = 31 << 11;
		else if (rV < 0)
			*(UInt16*)&rgbTable[i * 2 + 262144] = 0;
		else
			*(UInt16*)&rgbTable[i * 2 + 262144] = (UInt16)(((UInt16)Math::Double2Int32(rV)) << 11);
		if (gV > 63.0)
			*(UInt16*)&rgbTable[i * 2 + 131072] = 63 << 5;
		else if (gV < 0)
			*(UInt16*)&rgbTable[i * 2 + 131072] = 0;
		else
			*(UInt16*)&rgbTable[i * 2 + 131072] = (UInt16)(((UInt16)Math::Double2Int32(gV)) << 5);
		if (bV > 31.0)
			*(UInt16*)&rgbTable[i * 2 + 0] = 31;
		else if (bV < 0)
			*(UInt16*)&rgbTable[i * 2 + 0] = 0;
		else
			*(UInt16*)&rgbTable[i * 2 + 0] = (UInt16)Math::Double2Int32(bV);
	}
	DEL_CLASS(frFunc);
	DEL_CLASS(fgFunc);
	DEL_CLASS(fbFunc);
	DEL_CLASS(rTran);
	DEL_CLASS(gTran);
	DEL_CLASS(bTran);
}

void Media::RGBLUTGen::GenLRGB_A2B10G10R10(UInt8 *rgbTable, Media::ColorProfile *destProfile, Int32 nBitLRGB, Double srcRefLuminance)
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

	Media::CS::TransferParam *rTran;
	Media::CS::TransferParam *gTran;
	Media::CS::TransferParam *bTran;
	Media::CS::TransferType tranType = destProfile->GetRTranParam()->GetTranType();

	if (tranType == Media::CS::TRANT_VDISPLAY && this->colorSess == 0)
	{
		tranType = Media::CS::TRANT_sRGB;
		NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
	}
	else if (tranType == Media::CS::TRANT_PDISPLAY && this->colorSess == 0)
	{
		tranType = Media::CS::TRANT_sRGB;
		NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
	}
	else if (tranType == Media::CS::TRANT_VUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefVProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (tranType == Media::CS::TRANT_PUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefPProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (tranType == Media::CS::TRANT_VDISPLAY || tranType == Media::CS::TRANT_PDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();
		NEW_CLASS(rTran, Media::CS::TransferParam(rgbParam->monProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(rgbParam->monProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(rgbParam->monProfile->GetBTranParam()));
	}
	else
	{
		NEW_CLASS(rTran, Media::CS::TransferParam(destProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(destProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(destProfile->GetBTranParam()));
	}

	if (tranType == Media::CS::TRANT_VDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();

		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonVBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;

		if (srcRefLuminance != 0)
		{
			rMul = rMul * srcRefLuminance / rgbParam->monLuminance;
			gMul = gMul * srcRefLuminance / rgbParam->monLuminance;
			bMul = bMul * srcRefLuminance / rgbParam->monLuminance;
		}
	}
	else if (tranType == Media::CS::TRANT_PDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();

		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonPBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;

		if (srcRefLuminance != 0)
		{
			rMul = rMul * srcRefLuminance / rgbParam->monLuminance;
			gMul = gMul * srcRefLuminance / rgbParam->monLuminance;
			bMul = bMul * srcRefLuminance / rgbParam->monLuminance;
		}
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

	Media::CS::TransferFunc *frFunc = Media::CS::TransferFunc::CreateFunc(rTran);
	Media::CS::TransferFunc *fgFunc = Media::CS::TransferFunc::CreateFunc(gTran);
	Media::CS::TransferFunc *fbFunc = Media::CS::TransferFunc::CreateFunc(bTran);
	Int32 ibitVal = (1 << nBitLRGB) - 1;
	Double bitVal = ibitVal;
#if _OSINT_SIZE == 16
	Int32 i;
#else
	OSInt i;
#endif
	i = 65536;
	while (i-- > 0)
	{
		Double rv = frFunc->ForwardTransfer(((Int16)i) / bitVal * rMul);
		Double gv = fgFunc->ForwardTransfer(((Int16)i) / bitVal * gMul);
		Double bv = fbFunc->ForwardTransfer(((Int16)i) / bitVal * bMul);
		if (rv < 0)
			rv = 0;
		if (gv < 0)
			gv = 0;
		if (bv < 0)
			bv = 0;
		Double rV = (rBright - 1.0 + Math_Pow(rv, rGammaVal) * rContr) * 1023.0;
		Double gV = (gBright - 1.0 + Math_Pow(gv, gGammaVal) * gContr) * 1023.0;
		Double bV = (bBright - 1.0 + Math_Pow(bv, bGammaVal) * bContr) * 1023.0;
		if (rV > 1023.0)
			*(Int32*)&rgbTable[i * 4 + 524288] = 1023;
		else if (rV < 0)
			*(Int32*)&rgbTable[i * 4 + 524288] = 0;
		else
			*(Int32*)&rgbTable[i * 4 + 524288] = Math::Double2Int32(rV);
		if (gV > 1023.0)
			*(Int32*)&rgbTable[i * 4 + 262144] = ((Int32)1023) << 10;
		else if (gV < 0)
			*(Int32*)&rgbTable[i * 4 + 262144] = 0;
		else
			*(Int32*)&rgbTable[i * 4 + 262144] = Math::Double2Int32(gV) << 10;
		if (bV > 1023.0)
			*(UInt32*)&rgbTable[i * 4 + 0] = 0xc0000000 | (((Int32)1023) << 20);
		else if (bV < 0)
			*(UInt32*)&rgbTable[i * 4 + 0] = 0xc0000000;
		else
			*(UInt32*)&rgbTable[i * 4 + 0] = 0xc0000000 | (UInt32)(Math::Double2Int32(bV) << 20);
	}
	DEL_CLASS(frFunc);
	DEL_CLASS(fgFunc);
	DEL_CLASS(fbFunc);
	DEL_CLASS(rTran);
	DEL_CLASS(gTran);
	DEL_CLASS(bTran);
}

void Media::RGBLUTGen::GenLARGB_A2B10G10R10(UInt8 *rgbTable, Media::ColorProfile *destProfile, Int32 nBitLRGB, Double srcRefLuminance)
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

	Media::CS::TransferParam *rTran;
	Media::CS::TransferParam *gTran;
	Media::CS::TransferParam *bTran;
	Media::CS::TransferType tranType = destProfile->GetRTranParam()->GetTranType();

	if (tranType == Media::CS::TRANT_VDISPLAY && this->colorSess == 0)
	{
		tranType = Media::CS::TRANT_sRGB;
		NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
	}
	else if (tranType == Media::CS::TRANT_PDISPLAY && this->colorSess == 0)
	{
		tranType = Media::CS::TRANT_sRGB;
		NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
	}
	else if (tranType == Media::CS::TRANT_VUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefVProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (tranType == Media::CS::TRANT_PUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefPProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (tranType == Media::CS::TRANT_VDISPLAY || tranType == Media::CS::TRANT_PDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();
		NEW_CLASS(rTran, Media::CS::TransferParam(rgbParam->monProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(rgbParam->monProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(rgbParam->monProfile->GetBTranParam()));
	}
	else
	{
		NEW_CLASS(rTran, Media::CS::TransferParam(destProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(destProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(destProfile->GetBTranParam()));
	}

	if (tranType == Media::CS::TRANT_VDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();

		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonVBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;

		if (srcRefLuminance != 0)
		{
			rMul = rMul * srcRefLuminance / rgbParam->monLuminance;
			gMul = gMul * srcRefLuminance / rgbParam->monLuminance;
			bMul = bMul * srcRefLuminance / rgbParam->monLuminance;
		}
	}
	else if (tranType == Media::CS::TRANT_PDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();

		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonPBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;

		if (srcRefLuminance != 0)
		{
			rMul = rMul * srcRefLuminance / rgbParam->monLuminance;
			gMul = gMul * srcRefLuminance / rgbParam->monLuminance;
			bMul = bMul * srcRefLuminance / rgbParam->monLuminance;
		}
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

	Media::CS::TransferFunc *frFunc = Media::CS::TransferFunc::CreateFunc(rTran);
	Media::CS::TransferFunc *fgFunc = Media::CS::TransferFunc::CreateFunc(gTran);
	Media::CS::TransferFunc *fbFunc = Media::CS::TransferFunc::CreateFunc(bTran);
	Int32 ibitVal = (1 << nBitLRGB) - 1;
	Double bitVal = ibitVal;
#if _OSINT_SIZE == 16
	Int32 i;
#else
	OSInt i;
#endif
	i = 65536;
	while (i-- > 0)
	{
		Double rv = frFunc->ForwardTransfer(((Int16)i) / bitVal * rMul);
		Double gv = fgFunc->ForwardTransfer(((Int16)i) / bitVal * gMul);
		Double bv = fbFunc->ForwardTransfer(((Int16)i) / bitVal * bMul);
		if (rv < 0)
			rv = 0;
		if (gv < 0)
			gv = 0;
		if (bv < 0)
			bv = 0;
		Double rV = (rBright - 1.0 + Math_Pow(rv, rGammaVal) * rContr) * 1023.0;
		Double gV = (gBright - 1.0 + Math_Pow(gv, gGammaVal) * gContr) * 1023.0;
		Double bV = (bBright - 1.0 + Math_Pow(bv, bGammaVal) * bContr) * 1023.0;
		if (i > 32767)
			WriteUInt32(&rgbTable[i * 4 + 786432], 0);
		else if (i > ibitVal)
			WriteUInt32(&rgbTable[i * 4 + 786432], 0xc0000000);
		else
			WriteInt32(&rgbTable[i * 4 + 786432], (Int32)(((i >> (nBitLRGB - 2)) & 0xff) << 30));
		if (rV > 1023.0)
			WriteUInt32(&rgbTable[i * 4 + 524288], 1023);
		else if (rV < 0)
			WriteUInt32(&rgbTable[i * 4 + 524288], 0);
		else
			WriteInt32(&rgbTable[i * 4 + 524288], Math::Double2Int32(rV));
		if (gV > 1023.0)
			WriteInt32(&rgbTable[i * 4 + 262144], ((Int32)1023) << 10);
		else if (gV < 0)
			WriteInt32(&rgbTable[i * 4 + 262144], 0);
		else
			WriteInt32(&rgbTable[i * 4 + 262144], Math::Double2Int32(gV) << 10);
		if (bV > 1023.0)
			WriteInt32(&rgbTable[i * 4 + 0], ((Int32)1023) << 20);
		else if (bV < 0)
			WriteInt32(&rgbTable[i * 4 + 0], 0);
		else
			WriteInt32(&rgbTable[i * 4 + 0], Math::Double2Int32(bV) << 20);
	}
	DEL_CLASS(frFunc);
	DEL_CLASS(fgFunc);
	DEL_CLASS(fbFunc);
	DEL_CLASS(rTran);
	DEL_CLASS(gTran);
	DEL_CLASS(bTran);
}

void Media::RGBLUTGen::GenLARGB_B8G8R8A8(UInt8 *rgbTable, Media::ColorProfile *destProfile, Int32 nBitLRGB, Double srcRefLuminance)
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

	Media::CS::TransferParam *rTran;
	Media::CS::TransferParam *gTran;
	Media::CS::TransferParam *bTran;
	Media::CS::TransferType tranType = destProfile->GetRTranParam()->GetTranType();

	if (tranType == Media::CS::TRANT_VDISPLAY && this->colorSess == 0)
	{
		tranType = Media::CS::TRANT_sRGB;
		NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
	}
	else if (tranType == Media::CS::TRANT_PDISPLAY && this->colorSess == 0)
	{
		tranType = Media::CS::TRANT_sRGB;
		NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
	}
	else if (tranType == Media::CS::TRANT_VUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefVProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (tranType == Media::CS::TRANT_PUNKNOWN)
	{
		if (this->colorSess == 0)
		{
			NEW_CLASS(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASS(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			Media::ColorProfile *defProfile = this->colorSess->GetDefPProfile();
			NEW_CLASS(rTran, Media::CS::TransferParam(defProfile->GetRTranParam()));
			NEW_CLASS(gTran, Media::CS::TransferParam(defProfile->GetGTranParam()));
			NEW_CLASS(bTran, Media::CS::TransferParam(defProfile->GetBTranParam()));
		}
	}
	else if (tranType == Media::CS::TRANT_VDISPLAY || tranType == Media::CS::TRANT_PDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();
		NEW_CLASS(rTran, Media::CS::TransferParam(rgbParam->monProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(rgbParam->monProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(rgbParam->monProfile->GetBTranParam()));
	}
	else
	{
		NEW_CLASS(rTran, Media::CS::TransferParam(destProfile->GetRTranParam()));
		NEW_CLASS(gTran, Media::CS::TransferParam(destProfile->GetGTranParam()));
		NEW_CLASS(bTran, Media::CS::TransferParam(destProfile->GetBTranParam()));
	}

	if (tranType == Media::CS::TRANT_VDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();

		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonVBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;

		if (srcRefLuminance != 0)
		{
			rMul = rMul * srcRefLuminance / rgbParam->monLuminance;
			gMul = gMul * srcRefLuminance / rgbParam->monLuminance;
			bMul = bMul * srcRefLuminance / rgbParam->monLuminance;
		}
	}
	else if (tranType == Media::CS::TRANT_PDISPLAY)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = this->colorSess->GetRGBParam();

		rGammaVal = rgbParam->MonRGamma;
		gGammaVal = rgbParam->MonGGamma;
		bGammaVal = rgbParam->MonBGamma;
		rBright = rgbParam->MonRBright;
		gBright = rgbParam->MonGBright;
		bBright = rgbParam->MonBBright;
		rContr = rgbParam->MonRContr;
		gContr = rgbParam->MonGContr;
		bContr = rgbParam->MonBContr;
		tMul = rgbParam->MonPBrightness;
		rMul = rgbParam->MonRBrightness * tMul;
		gMul = rgbParam->MonGBrightness * tMul;
		bMul = rgbParam->MonBBrightness * tMul;

		if (srcRefLuminance != 0)
		{
			rMul = rMul * srcRefLuminance / rgbParam->monLuminance;
			gMul = gMul * srcRefLuminance / rgbParam->monLuminance;
			bMul = bMul * srcRefLuminance / rgbParam->monLuminance;
		}
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

	Media::CS::TransferFunc *frFunc = Media::CS::TransferFunc::CreateFunc(rTran);
	Media::CS::TransferFunc *fgFunc = Media::CS::TransferFunc::CreateFunc(gTran);
	Media::CS::TransferFunc *fbFunc = Media::CS::TransferFunc::CreateFunc(bTran);
	Int32 ibitVal = (1 << nBitLRGB) - 1;
	Double bitVal = ibitVal;
#if _OSINT_SIZE == 16
	Int32 i;
#else
	OSInt i;
#endif
	i = 65536;
	while (i-- > 0)
	{
		Double rv = frFunc->ForwardTransfer(((Int16)i) / bitVal * rMul);
		Double gv = fgFunc->ForwardTransfer(((Int16)i) / bitVal * gMul);
		Double bv = fbFunc->ForwardTransfer(((Int16)i) / bitVal * bMul);
		if (rv < 0)
			rv = 0;
		if (gv < 0)
			gv = 0;
		if (bv < 0)
			bv = 0;
		Double rV = (rBright - 1.0 + Math_Pow(rv, rGammaVal) * rContr) * 255.0;
		Double gV = (gBright - 1.0 + Math_Pow(gv, gGammaVal) * gContr) * 255.0;
		Double bV = (bBright - 1.0 + Math_Pow(bv, bGammaVal) * bContr) * 255.0;
		if (i > 32767)
			WriteUInt32(&rgbTable[i * 4 + 786432], 0);
		else if (i > ibitVal)
			WriteUInt32(&rgbTable[i * 4 + 786432], 0xff000000);
		else
			WriteInt32(&rgbTable[i * 4 + 786432], (Int32)(((i >> (nBitLRGB - 8)) & 0xff) << 24));
		if (rV > 255.0)
			WriteUInt32(&rgbTable[i * 4 + 524288], 0xff0000);
		else if (rV < 0)
			WriteInt32(&rgbTable[i * 4 + 524288], 0);
		else
			WriteInt32(&rgbTable[i * 4 + 524288], Math::Double2Int32(rV) << 16);
		if (gV > 255.0)
			WriteInt32(&rgbTable[i * 4 + 262144], 0xff00);
		else if (gV < 0)
			WriteInt32(&rgbTable[i * 4 + 262144], 0);
		else
			WriteInt32(&rgbTable[i * 4 + 262144], Math::Double2Int32(gV) << 8);
		if (bV > 255.0)
			WriteInt32(&rgbTable[i * 4 + 0], 0xff);
		else if (bV < 0)
			WriteInt32(&rgbTable[i * 4 + 0], 0);
		else
			WriteInt32(&rgbTable[i * 4 + 0], Math::Double2Int32(bV));
	}
	DEL_CLASS(frFunc);
	DEL_CLASS(fgFunc);
	DEL_CLASS(fbFunc);
	DEL_CLASS(rTran);
	DEL_CLASS(gTran);
	DEL_CLASS(bTran);
}
