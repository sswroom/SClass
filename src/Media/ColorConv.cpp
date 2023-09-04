#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Registry.h"
#include "Math/Math.h"
#include "Media/ColorConv.h"
#include "Media/RGBLUTGen.h"
#include "Media/CS/TransferFunc.h"

Media::ColorConv::ColorConv(NotNullPtr<const Media::ColorProfile> srcColor, NotNullPtr<const Media::ColorProfile> destColor, Media::ColorManagerSess *colorSess) : srcColor(srcColor), destColor(destColor)
{
	this->colorSess = colorSess;
	this->rgbTable = MemAlloc(UInt8, 256 * 4 * 8 + 262144);
	Media::RGBLUTGen rgbGen(this->colorSess);
	rgbGen.GenRGBA8_LRGBC((Int64*)this->rgbTable, this->srcColor, this->destColor.GetPrimaries(), 14);
	rgbGen.GenLRGB_BGRA8(this->rgbTable + 8192, this->destColor, 14, Media::CS::TransferFunc::GetRefLuminance(this->srcColor.rtransfer));
}

Media::ColorConv::~ColorConv()
{
	MemFree(this->rgbTable);
}

void Media::ColorConv::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	Media::RGBLUTGen rgbGen(this->colorSess);
	rgbGen.GenRGBA8_LRGBC((Int64*)this->rgbTable, this->srcColor, this->destColor.GetPrimaries(), 14);
	rgbGen.GenLRGB_BGRA8(this->rgbTable + 8192, this->destColor, 14, Media::CS::TransferFunc::GetRefLuminance(this->srcColor.rtransfer));
}

UInt32 Media::ColorConv::ConvRGB8(UInt32 c)
{
	UInt8 *rgbTable = this->rgbTable;
#if defined(HAS_ASM32)
	_asm
	{
		mov edx,rgbTable;
		mov ecx,c
		movzx eax,cl
		shr ecx,8
		movq xmm1,mmword ptr [edx+eax*8+4096]
		movzx eax,cl
		shr ecx,8
		movq xmm0,mmword ptr [edx+eax*8+2048]
		movzx eax,cl
		paddsw xmm1,xmm0
		shr ecx,8
		movq xmm0,mmword ptr [edx+eax*8]
		paddsw xmm1,xmm0
		movq xmm0,mmword ptr [edx+ecx*8+6144]
		paddsw xmm1,xmm0
		pextrw ecx,xmm1,3
		mov ah,byte ptr [edx+ecx+204800]
		pextrw ecx,xmm1,2
		mov al,byte ptr [edx+ecx+139264]
		shl eax,16
		pextrw ecx,xmm1,1
		mov ah,byte ptr [edx+ecx+73728]
		pextrw ecx,xmm1,0
		mov al,byte ptr [edx+ecx+8192]
	}
#else
	UInt8 *srcC = (UInt8*)&c;
	UInt8 outp[4];
	Int32 a = ReadInt16(&rgbTable[srcC[0] * 8 + 4096 + 6]);
	Int32 r = ReadInt16(&rgbTable[srcC[0] * 8 + 4096 + 4]);
	Int32 g = ReadInt16(&rgbTable[srcC[0] * 8 + 4096 + 2]);
	Int32 b = ReadInt16(&rgbTable[srcC[0] * 8 + 4096 + 0]);
	a += ReadInt16(&rgbTable[srcC[1] * 8 + 2048 + 6]);
	r += ReadInt16(&rgbTable[srcC[1] * 8 + 2048 + 4]);
	g += ReadInt16(&rgbTable[srcC[1] * 8 + 2048 + 2]);
	b += ReadInt16(&rgbTable[srcC[1] * 8 + 2048 + 0]);
	a += ReadInt16(&rgbTable[srcC[2] * 8 + 6]);
	r += ReadInt16(&rgbTable[srcC[2] * 8 + 4]);
	g += ReadInt16(&rgbTable[srcC[2] * 8 + 2]);
	b += ReadInt16(&rgbTable[srcC[2] * 8 + 0]);
	a += ReadInt16(&rgbTable[srcC[3] * 8 + 6144 + 6]);
	r += ReadInt16(&rgbTable[srcC[3] * 8 + 6144 + 4]);
	g += ReadInt16(&rgbTable[srcC[3] * 8 + 6144 + 2]);
	b += ReadInt16(&rgbTable[srcC[3] * 8 + 6144 + 0]);
	if (a > 65535)
		a = 65535;
	else if (a < 0)
		a = 0;
	if (r > 65535)
		r = 65535;
	else if (r < 0)
		r = 0;
	if (g > 65535)
		g = 65535;
	else if (g < 0)
		g = 0;
	if (b > 65535)
		b = 65535;
	else if (b < 0)
		b = 0;
	outp[0] = rgbTable[b + 8192];
	outp[1] = rgbTable[g + 73728];
	outp[2] = rgbTable[r + 139264];
	outp[3] = rgbTable[a + 204800];
	return ReadUInt32(outp);
#endif
}

UInt32 Media::ColorConv::ConvARGB(NotNullPtr<const Media::ColorProfile> srcColor, NotNullPtr<const Media::ColorProfile> destColor, Media::ColorManagerSess *colorSess, UInt32 c)
{
	UInt8 buff[4];
	const Media::IColorHandler::RGBPARAM2 *rgbParam;
	NotNullPtr<Media::CS::TransferParam> rTran;
	NotNullPtr<Media::CS::TransferParam> gTran;
	NotNullPtr<Media::CS::TransferParam> bTran;
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

	if (srcColor->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_VDISPLAY || srcColor->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		if (colorSess == 0)
		{
			NEW_CLASSNN(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASSNN(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASSNN(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			rgbParam = colorSess->GetRGBParam();
			NEW_CLASSNN(rTran, Media::CS::TransferParam(rgbParam->monProfile.GetRTranParamRead()));
			NEW_CLASSNN(gTran, Media::CS::TransferParam(rgbParam->monProfile.GetGTranParamRead()));
			NEW_CLASSNN(bTran, Media::CS::TransferParam(rgbParam->monProfile.GetBTranParamRead()));
		}
	}
	else if (srcColor->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		if (colorSess == 0)
		{
			NEW_CLASSNN(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASSNN(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASSNN(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			NotNullPtr<Media::ColorProfile> defProfile = colorSess->GetDefVProfile();
			NEW_CLASSNN(rTran, Media::CS::TransferParam(defProfile->GetRTranParamRead()));
			NEW_CLASSNN(gTran, Media::CS::TransferParam(defProfile->GetGTranParamRead()));
			NEW_CLASSNN(bTran, Media::CS::TransferParam(defProfile->GetBTranParamRead()));
		}
	}
	else if (srcColor->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		if (colorSess == 0)
		{
			NEW_CLASSNN(rTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASSNN(gTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
			NEW_CLASSNN(bTran, Media::CS::TransferParam(Media::CS::TRANT_sRGB, 2.2));
		}
		else
		{
			NotNullPtr<Media::ColorProfile> defProfile = colorSess->GetDefPProfile();
			NEW_CLASSNN(rTran, Media::CS::TransferParam(defProfile->GetRTranParamRead()));
			NEW_CLASSNN(gTran, Media::CS::TransferParam(defProfile->GetGTranParamRead()));
			NEW_CLASSNN(bTran, Media::CS::TransferParam(defProfile->GetBTranParamRead()));
		}
	}
	else
	{
		NEW_CLASSNN(rTran, Media::CS::TransferParam(srcColor->GetRTranParamRead()));
		NEW_CLASSNN(gTran, Media::CS::TransferParam(srcColor->GetGTranParamRead()));
		NEW_CLASSNN(bTran, Media::CS::TransferParam(srcColor->GetBTranParamRead()));
	}
	Media::CS::TransferFunc *srFunc = Media::CS::TransferFunc::CreateFunc(rTran);
	Media::CS::TransferFunc *sgFunc = Media::CS::TransferFunc::CreateFunc(gTran);
	Media::CS::TransferFunc *sbFunc = Media::CS::TransferFunc::CreateFunc(bTran);

	Math::Matrix3 mat1;
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Matrix3 mat4;
	Math::Matrix3 mat5;
	Math::Vector3 vec1;
	Math::Vector3 vec2;
	srcColor->GetPrimariesRead()->GetConvMatrix(mat1);
	if (destColor->GetPrimariesRead()->colorType == Media::ColorProfile::CT_DISPLAY)
	{
		if (colorSess)
		{
			rgbParam = colorSess->GetRGBParam();
			rgbParam->monProfile.GetPrimariesRead()->GetConvMatrix(mat5);
			vec2.Set(rgbParam->monProfile.GetPrimariesRead()->w, 1.0);
		}
		else
		{
			vec2 = Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(Media::ColorProfile::WPT_D65);
		}
	}
	else
	{
		destColor->GetPrimariesRead()->GetConvMatrix(mat5);
		vec2.Set(destColor->GetPrimariesRead()->w, 1.0);
	}
	mat5.Inverse();

	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(mat2);
	mat3.Set(mat2);
	mat4.SetIdentity();
	vec2 = Media::ColorProfile::ColorPrimaries::xyYToXYZ(vec2);
	vec1 = Media::ColorProfile::ColorPrimaries::xyYToXYZ(Math::Vector3(srcColor->GetPrimariesRead()->w, 1.0));
	vec1 = mat2.Multiply(vec1);
	vec2 = mat2.Multiply(vec2);
	mat2.Inverse();
	mat4.vec[0].val[0] = vec2.val[0] / vec1.val[0];
	mat4.vec[1].val[1] = vec2.val[1] / vec1.val[1];
	mat4.vec[2].val[2] = vec2.val[2] / vec1.val[2];
	mat2.Multiply(mat4);
	mat2.Multiply(mat3);
	mat1.MyMultiply(mat2);

	mat1.MyMultiply(mat5);

	if (destColor->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_VDISPLAY && colorSess != 0)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = colorSess->GetRGBParam();

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
		rTran->Set(rgbParam->monProfile.GetRTranParamRead());
		gTran->Set(rgbParam->monProfile.GetGTranParamRead());
		bTran->Set(rgbParam->monProfile.GetBTranParamRead());
	}
	else if (destColor->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_PDISPLAY && colorSess != 0)
	{
		const Media::IColorHandler::RGBPARAM2 *rgbParam = colorSess->GetRGBParam();

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
		rTran->Set(rgbParam->monProfile.GetRTranParamRead());
		gTran->Set(rgbParam->monProfile.GetGTranParamRead());
		bTran->Set(rgbParam->monProfile.GetBTranParamRead());
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
		if (destColor->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_VDISPLAY)
		{
			rTran->Set(Media::CS::TRANT_sRGB, 2.2);
			gTran->Set(Media::CS::TRANT_sRGB, 2.2);
			bTran->Set(Media::CS::TRANT_sRGB, 2.2);
		}
		else if (destColor->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_PDISPLAY)
		{
			rTran->Set(Media::CS::TRANT_sRGB, 2.2);
			gTran->Set(Media::CS::TRANT_sRGB, 2.2);
			bTran->Set(Media::CS::TRANT_sRGB, 2.2);
		}
		else if (destColor->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
		{
			if (colorSess == 0)
			{
				rTran->Set(Media::CS::TRANT_sRGB, 2.2);
				gTran->Set(Media::CS::TRANT_sRGB, 2.2);
				bTran->Set(Media::CS::TRANT_sRGB, 2.2);
			}
			else
			{
				NotNullPtr<Media::ColorProfile> defProfile = colorSess->GetDefVProfile();
				rTran->Set(defProfile->GetRTranParamRead());
				gTran->Set(defProfile->GetGTranParamRead());
				bTran->Set(defProfile->GetBTranParamRead());
			}
		}
		else if (destColor->GetRTranParamRead()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
		{
			if (colorSess == 0)
			{
				rTran->Set(Media::CS::TRANT_sRGB, 2.2);
				gTran->Set(Media::CS::TRANT_sRGB, 2.2);
				bTran->Set(Media::CS::TRANT_sRGB, 2.2);
			}
			else
			{
				NotNullPtr<Media::ColorProfile> defProfile = colorSess->GetDefPProfile();
				rTran->Set(defProfile->GetRTranParam());
				gTran->Set(defProfile->GetGTranParam());
				bTran->Set(defProfile->GetBTranParam());
			}
		}
		else
		{
			rTran->Set(destColor->GetRTranParamRead());
			gTran->Set(destColor->GetGTranParamRead());
			bTran->Set(destColor->GetBTranParamRead());
		}
	}

	Media::CS::TransferFunc *drFunc = Media::CS::TransferFunc::CreateFunc(rTran);
	Media::CS::TransferFunc *dgFunc = Media::CS::TransferFunc::CreateFunc(gTran);
	Media::CS::TransferFunc *dbFunc = Media::CS::TransferFunc::CreateFunc(bTran);
	*(UInt32*)buff = c;
	vec1.Set(srFunc->InverseTransfer(buff[2] / 255.0), sgFunc->InverseTransfer(buff[1] / 255.0), sbFunc->InverseTransfer(buff[0] / 255.0));
	vec2 = mat1.Multiply(vec1);

	Double rv = (rBright - 1.0 + Math_Pow(drFunc->ForwardTransfer(vec2.val[0] * rMul), rGammaVal) * rContr) * 255.0;
	Double gv = (gBright - 1.0 + Math_Pow(dgFunc->ForwardTransfer(vec2.val[1] * gMul), gGammaVal) * gContr) * 255.0;
	Double bv = (bBright - 1.0 + Math_Pow(dbFunc->ForwardTransfer(vec2.val[2] * bMul), bGammaVal) * bContr) * 255.0;
	if (rv > 255.0)
		buff[2] = 255;
	else if (rv < 0)
		buff[2] = 0;
	else
		buff[2] = (UInt8)Double2Int32(rv);
	if (gv > 255.0)
		buff[1] = 255;
	else if (gv < 0)
		buff[1] = 0;
	else
		buff[1] = (UInt8)Double2Int32(gv);
	if (bv > 255.0)
		buff[0] = 255;
	else if (bv < 0)
		buff[0] = 0;
	else
		buff[0] = (UInt8)Double2Int32(bv);

	DEL_CLASS(srFunc);
	DEL_CLASS(sgFunc);
	DEL_CLASS(sbFunc);
	DEL_CLASS(drFunc);
	DEL_CLASS(dgFunc);
	DEL_CLASS(dbFunc);
	rTran.Delete();
	gTran.Delete();
	bTran.Delete();
	return *(UInt32*)buff;
}
