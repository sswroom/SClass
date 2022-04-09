#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/ColorProfile.h"
#include "Media/ICCProfile.h"
#include "Media/CS/TransferFunc.h"
#include "Text/MyStringFloat.h"

//C:\Windows\System32\spool\drivers\color
///usr/share/color/icc

void Media::ColorProfile::ColorCoordinate::SetFromxyY(Double x, Double y, Double Y)
{
	this->Y = Y;
	this->X = Y * x / y;
	this->Z = Y * (1 - x - y) / y;
}

void Media::ColorProfile::ColorCoordinate::SetFromXYZ(Double X, Double Y, Double Z)
{
	this->X = X;
	this->Y = Y;
	this->Z = Z;
}

void Media::ColorProfile::ColorCoordinate::Getxy(Double *x, Double *y)
{
	Double avg = (this->X + this->Y + this->Z);
	*x = this->X / avg;
	*y = this->Y / avg;
}

void Media::ColorProfile::ColorPrimaries::SetWhiteType(WhitePointType wpType)
{
	GetWhitePointXY(wpType, &this->wx, &this->wy);
}

void Media::ColorProfile::ColorPrimaries::SetWhiteTemp(Double colorTemp)
{
	GetWhitePointXY(colorTemp, &this->wx, &this->wy);
}

Media::ColorProfile::ColorPrimaries::ColorPrimaries()
{
	this->SetColorType(Media::ColorProfile::CT_PUNKNOWN);
}

Media::ColorProfile::ColorPrimaries::~ColorPrimaries()
{
}

void Media::ColorProfile::ColorPrimaries::Set(const ColorPrimaries *primaries)
{
	this->colorType = primaries->colorType;
	this->rx = primaries->rx;
	this->ry = primaries->ry;
	this->gx = primaries->gx;
	this->gy = primaries->gy;
	this->bx = primaries->bx;
	this->by = primaries->by;
	this->wx = primaries->wx;
	this->wy = primaries->wy;
}

void Media::ColorProfile::ColorPrimaries::SetColorType(ColorType colorType)
{
	switch (colorType)
	{
	case CT_ADOBE:
		this->colorType = CT_ADOBE;
		rx = 0.6400;	ry = 0.3300;
		gx = 0.2100;	gy = 0.7100;
		bx = 0.1500;	by = 0.0600;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_APPLE:
		this->colorType = CT_APPLE;
		rx = 0.6250;	ry = 0.3400;
		gx = 0.2800;	gy = 0.5950;
		bx = 0.1550;	by = 0.0700;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_CIERGB:
		this->colorType = CT_CIERGB;
		rx = 0.7350;	ry = 0.2650;
		gx = 0.2740;	gy = 0.7170;
		bx = 0.1670;	by = 0.0090;
		this->SetWhiteType(WPT_E);
		break;

	case CT_COLORMATCH:
		this->colorType = CT_COLORMATCH;
		rx = 0.6300;	ry = 0.3400;
		gx = 0.2950;	gy = 0.6050;
		bx = 0.1500;	by = 0.0750;
		this->SetWhiteType(WPT_D65);
		break;
		
	case CT_SRGB:
	case CT_BT709:
		this->colorType = CT_BT709;
		rx = 0.6400;	ry = 0.3300;
		gx = 0.3000;	gy = 0.6000;
		bx = 0.1500;	by = 0.0600;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_BT470M: //NTSC
		this->colorType = CT_BT470M;
		rx = 0.6700;	ry = 0.3300;
		gx = 0.2100;	gy = 0.7100;
		bx = 0.1400;	by = 0.0800;
		this->SetWhiteType(WPT_C);
		break;

	case CT_BT470BG: //PAL
		this->colorType = CT_BT470BG;
		rx = 0.6400;	ry = 0.3300;
		gx = 0.2900;	gy = 0.6000;
		bx = 0.1500;	by = 0.0600;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_SMPTE170M:
		this->colorType = CT_SMPTE170M;
		rx = 0.640;	ry = 0.340;
		gx = 0.310;	gy = 0.595;
		bx = 0.155;	by = 0.070;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_SMPTE240M:
		this->colorType = CT_SMPTE240M;
		rx = 0.6300;	ry = 0.3400;
		gx = 0.3100;	gy = 0.5950;
		bx = 0.1550;	by = 0.0700;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_GENERIC_FILM:
		this->colorType = CT_GENERIC_FILM;
		rx = 0.681;	ry = 0.319;
		gx = 0.243;	gy = 0.692;
		bx = 0.145;	by = 0.049;
		this->SetWhiteType(WPT_C);
		break;

	case CT_BT2020:
		this->colorType = CT_BT2020;
		rx = 0.708;	ry = 0.292;
		gx = 0.170;	gy = 0.797;
		bx = 0.131;	by = 0.046;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_WIDE: //Wide Gamut
		this->colorType = CT_WIDE;
		rx = 0.7347;	ry = 0.2653;
		gx = 0.1152;	gy = 0.8264;
		bx = 0.1566;	by = 0.0177;
		this->SetWhiteType(WPT_D50);
		break;

	case CT_SGAMUT:
		this->colorType = CT_SGAMUT;
		rx = 0.73;	ry = 0.28;
		gx = 0.14;	gy = 0.855;
		bx = 0.10;	by = -0.05;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_SGAMUTCINE:
		this->colorType = CT_SGAMUTCINE;
		rx = 0.76600;	ry = 0.27500;
		gx = 0.22500;	gy = 0.80000;
		bx = 0.08900;	by = -0.08700;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_DCI_P3:
		this->colorType = CT_DCI_P3;
		rx = 0.68000;	ry = 0.32000;
		gx = 0.26500;	gy = 0.69000;
		bx = 0.15000;	by = 0.06000;
		this->SetWhiteType(WPT_DCI);
		break;

	case CT_ACESGAMUT:
		this->colorType = CT_ACESGAMUT;
		rx = 0.73470;	ry = 0.26530;
		gx = 0.00000;	gy = 1.00000;
		bx = 0.00010;	by = -0.07700;
		wx = 0.32168;	wy = 0.33767;
		break;

	case CT_ALEXAWIDE:
		this->colorType = CT_ALEXAWIDE;
		rx = 0.6840;	ry = 0.3130;
		gx = 0.2210;	gy = 0.8480;
		bx = 0.0861;	by = -0.1020;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_VGAMUT:
		this->colorType = CT_VGAMUT;
		rx = 0.730;	ry = 0.280;
		gx = 0.165;	gy = 0.840;
		bx = 0.100;	by = -0.030;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_CUSTOM:
		this->colorType = CT_CUSTOM;
		rx = 0.6400;	ry = 0.3300;
		gx = 0.3000;	gy = 0.6000;
		bx = 0.1500;	by = 0.0600;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_DISPLAY:
		this->colorType = CT_DISPLAY;
		rx = 0.6400;	ry = 0.3300;
		gx = 0.3000;	gy = 0.6000;
		bx = 0.1500;	by = 0.0600;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_VUNKNOWN:
		this->colorType = CT_VUNKNOWN;
		rx = 0.6400;	ry = 0.3300;
		gx = 0.3000;	gy = 0.6000;
		bx = 0.1500;	by = 0.0600;
		this->SetWhiteType(WPT_D65);
		break;

	case CT_PUNKNOWN:
	default:
		this->colorType = Media::ColorProfile::CT_PUNKNOWN;
		rx = 0.6400;	ry = 0.3300;
		gx = 0.3000;	gy = 0.6000;
		bx = 0.1500;	by = 0.0600;
		this->SetWhiteType(WPT_D65);
		break;
	}
}

void Media::ColorProfile::ColorPrimaries::GetWhitexy(Double *x, Double *y)
{
	*x = this->wx;
	*y = this->wy;
}

void Media::ColorProfile::ColorPrimaries::GetConvMatrix(Math::Matrix3 *matrix) const
{
	Double Sr;
	Double Sg;
	Double Sb;
	Double Xr;
	Double Yr;
	Double Zr;
	Double Xg;
	Double Yg;
	Double Zg;
	Double Xb;
	Double Yb;
	Double Zb;
	Double Xw;
	Double Yw;
	Double Zw;

	Xr = this->rx / this->ry;
	Yr = 1.0;
	Zr = (1 - this->rx - this->ry) / this->ry;
	Xg = this->gx / this->gy;
	Yg = 1.0;
	Zg = (1 - this->gx - this->gy) / this->gy;
	Xb = this->bx / this->by;
	Yb = 1.0;
	Zb = (1 - this->bx - this->by) / this->by;
	Xw = this->wx / this->wy;
	Yw = 1.0;
	Zw = (1 - this->wx - this->wy) / this->wy;
	Math::Matrix3 matrixTmp;
	matrixTmp.vec[0].val[0] = Xr;
	matrixTmp.vec[0].val[1] = Xg;
	matrixTmp.vec[0].val[2] = Xb;
	matrixTmp.vec[1].val[0] = Yr;
	matrixTmp.vec[1].val[1] = Yg;
	matrixTmp.vec[1].val[2] = Yb;
	matrixTmp.vec[2].val[0] = Zr;
	matrixTmp.vec[2].val[1] = Zg;
	matrixTmp.vec[2].val[2] = Zb;
	matrixTmp.Inverse();

	matrixTmp.Multiply(Xw, Yw, Zw, &Sr, &Sg, &Sb);
	matrix->vec[0].val[0] = Sr * Xr;
	matrix->vec[0].val[1] = Sg * Xg;
	matrix->vec[0].val[2] = Sb * Xb;
	matrix->vec[1].val[0] = Sr * Yr;
	matrix->vec[1].val[1] = Sg * Yg;
	matrix->vec[1].val[2] = Sb * Yb;
	matrix->vec[2].val[0] = Sr * Zr;
	matrix->vec[2].val[1] = Sg * Zg;
	matrix->vec[2].val[2] = Sb * Zb;
}

void Media::ColorProfile::ColorPrimaries::SetConvMatrix(Math::Matrix3 *matrix)
{
	Double Sr = matrix->vec[1].val[0];
	Double Sg = matrix->vec[1].val[1];
	Double Sb = matrix->vec[1].val[2];
	Double X;
//	Double Y;
	Double Z;
	X = matrix->vec[0].val[0] / Sr;
//	Y = matrix->vec[1].val[0] / Sr;
	Z = matrix->vec[2].val[0] / Sr;
	this->ry = 1 / (Z + X + 1);
	this->rx = X * this->ry;
	X = matrix->vec[0].val[1] / Sg;
//	Y = matrix->vec[1].val[1] / Sg;
	Z = matrix->vec[2].val[1] / Sg;
	this->gy = 1 / (Z + X + 1);
	this->gx = X * this->gy;
	X = matrix->vec[0].val[2] / Sb;
//	Y = matrix->vec[1].val[2] / Sb;
	Z = matrix->vec[2].val[2] / Sb;
	this->by = 1 / (Z + X + 1);
	this->bx = X * this->by;
	X = this->rx / this->ry * Sr + this->gx / this->gy * Sg + this->bx / this->by * Sb;
//	Y = 1.0;
	Z = (1 - this->rx - this->ry) / this->ry * Sr + (1 - this->gx - this->gy) / this->gy * Sg + (1 - this->bx - this->by) / this->by * Sb;
	this->wy = 1 / (Z + X + 1);
	this->wx = X * this->wy;
}

Bool Media::ColorProfile::ColorPrimaries::Equals(const ColorPrimaries *primaries)
{
	return this->rx == primaries->rx && this->ry == primaries->ry && this->gx == primaries->gx && this->gy == primaries->gy && this->bx == primaries->bx && this->by == primaries->by && this->wx == primaries->wx && this->wy == primaries->wy;
}

void Media::ColorProfile::ColorPrimaries::GetWhitePointXY(WhitePointType wpType, Double *x, Double *y)
{
	switch (wpType)
	{
	case WPT_A:
		*x = 0.44757;
		*y = 0.40745;
		break;
	case WPT_B:
		*x = 0.34842;
		*y = 0.35161;
		break;
	case WPT_C:
		*x = 0.310063;
		*y = 0.316158;
		break;
	case WPT_D50:
//		*x = 0.34567;
//		*y = 0.35850;
		*x = 0.34566918689481363576071096962462;
		*y = 0.35849618022319972180696414679702;
// XYZ: 0.96422 	1.00000 	0.82521
		break;
	case WPT_D55:
		*x = 0.33242;
		*y = 0.34743;
		break;
	default:
	case WPT_D65:
//		*x = 0.31271;
//		*y = 0.32902;
		*x = 0.31272661468101207514888296647254;
		*y = 0.32902313032606192215312736485375;
// XYZ: 0.95047 	1.00000 	1.08883
		break;
	case WPT_D75:
		*x = 0.29902;
		*y = 0.31485;
		break;
	case WPT_E:
		*x = 1 / 3.0;
		*y = 1 / 3.0;
		break;
	case WPT_F1:
		*x = 0.31310;
		*y = 0.33727;
		break;
	case WPT_F2:
		*x = 0.37208;
		*y = 0.37529;
		break;
	case WPT_F3:
		*x = 0.40910;
		*y = 0.39430;
		break;
	case WPT_F4:
		*x = 0.44018;
		*y = 0.40329;
		break;
	case WPT_F5:
		*x = 0.31379;
		*y = 0.34531;
		break;
	case WPT_F6:
		*x = 0.37790;
		*y = 0.38835;
		break;
	case WPT_F7:
		*x = 0.31292;
		*y = 0.32933;
		break;
	case WPT_F8:
		*x = 0.34588;
		*y = 0.35875;
		break;
	case WPT_F9:
		*x = 0.37417;
		*y = 0.37281;
		break;
	case WPT_F10:
		*x = 0.34609;
		*y = 0.35986;
		break;
	case WPT_F11:
		*x = 0.38052;
		*y = 0.37713;
		break;
	case WPT_F12:
		*x = 0.43695;
		*y = 0.40441;
		break;
	case WPT_DCI:
		*x = 0.31400;
		*y = 0.35100;
		break;
	}

}

void Media::ColorProfile::ColorPrimaries::GetWhitePointXY(Double colorTemp, Double *x, Double *y)
{
	Double currX;
	Double currY;
	Double t = 1000.0 / colorTemp;
	if (colorTemp <= 7000)
	{
		currX = 0.244063 + 0.09911 * t + 2.9678 * t * t - 4.6070 * t * t * t;
	}
	else
	{
		currX = 0.237040 + 0.24748 * t + 1.9018 * t * t - 2.0064 * t * t * t;
	}
	currY = 2.870 * currX - 3.000 * currX * currX - 0.275;
	*x = currX;
	*y = currY;
}

void Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(WhitePointType wpType, Math::Vector3 *vec)
{
	Math::Vector3 xyY;
	GetWhitePointXY(wpType, &xyY.val[0], &xyY.val[1]);
	xyY.val[2] = 1.0;
	xyYToXYZ(&xyY, vec);
}

void Media::ColorProfile::ColorPrimaries::GetMatrixBradford(Math::Matrix3 *mat)
{
	mat->vec[0].Set( 0.8951,  0.2664, -0.1614);
	mat->vec[1].Set(-0.7502,  1.7135,  0.0367);
	mat->vec[2].Set( 0.0389, -0.0685,  1.0296);
}

void Media::ColorProfile::ColorPrimaries::GetMatrixVonKries(Math::Matrix3 *mat)
{
	mat->vec[0].Set( 0.40024,  0.70760, -0.08081);
	mat->vec[1].Set(-0.22630,  1.16532,  0.04570);
	mat->vec[2].Set( 0.00000,  0.00000,  0.91822);
}

void Media::ColorProfile::ColorPrimaries::xyYToXYZ(Math::Vector3 *xyY, Math::Vector3 *XYZ)
{
	Double v = xyY->val[2] / xyY->val[1];
	XYZ->val[1] = xyY->val[2];
	XYZ->val[0] = xyY->val[0] * v;
	XYZ->val[2] = (1 - xyY->val[0] - xyY->val[1]) * v;
}

void Media::ColorProfile::ColorPrimaries::XYZToxyY(Math::Vector3 *XYZ, Math::Vector3 *xyY)
{
	Double sum = XYZ->val[0] + XYZ->val[1] + XYZ->val[2];
	xyY->val[2] = XYZ->val[1];
	xyY->val[0] = XYZ->val[0] / sum;
	xyY->val[1] = XYZ->val[1] / sum;
}

void Media::ColorProfile::ColorPrimaries::GetAdaptationMatrix(Math::Matrix3 *mat, WhitePointType srcWP, WhitePointType destWP)
{
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Vector3 vec1;
	Math::Vector3 vec2;
	Math::Vector3 vec3;

	//color.primaries.GetConvMatrix(&mat);
	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(mat);
	mat2.Set(mat);
	mat3.SetIdentity();

	Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(srcWP, &vec3);
	mat->Multiply(&vec3, &vec1);
	Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(destWP, &vec3);
	mat->Multiply(&vec3, &vec2);
	mat->Inverse();
	mat3.vec[0].val[0] = vec2.val[0] / vec1.val[0];
	mat3.vec[1].val[1] = vec2.val[1] / vec1.val[1];
	mat3.vec[2].val[2] = vec2.val[2] / vec1.val[2];
	mat->Multiply(&mat3);
	mat->Multiply(&mat2);
}

Media::ColorProfile::ColorProfile()
{
	this->rawICC = 0;
}

Media::ColorProfile::ColorProfile(const ColorProfile *profile)
{
	this->rawICC = 0;
	this->Set(profile);
}

Media::ColorProfile::ColorProfile(CommonProfileType cpt)
{
	this->rawICC = 0;
	this->SetCommonProfile(cpt);
}

Media::ColorProfile::~ColorProfile()
{
	if (this->rawICC)
	{
		MemFree((UInt8*)this->rawICC);
		this->rawICC = 0;
	}
}

void Media::ColorProfile::Set(const ColorProfile *profile)
{
	this->rtransfer.Set(&profile->rtransfer);
	this->gtransfer.Set(&profile->gtransfer);
	this->btransfer.Set(&profile->btransfer);
	this->primaries.Set(&profile->primaries);
	this->SetRAWICC(profile->rawICC);
}

void Media::ColorProfile::SetCommonProfile(Media::ColorProfile::CommonProfileType cpt)
{
	switch (cpt)
	{
	case CPT_ADOBE:
		this->primaries.SetColorType(Media::ColorProfile::CT_ADOBE);
		this->rtransfer.Set(Media::CS::TRANT_GAMMA, 563.0 / 256.0);
		this->gtransfer.Set(Media::CS::TRANT_GAMMA, 563.0 / 256.0);
		this->btransfer.Set(Media::CS::TRANT_GAMMA, 563.0 / 256.0);
		break;
	case CPT_APPLE:
		this->primaries.SetColorType(Media::ColorProfile::CT_APPLE);
		this->rtransfer.Set(Media::CS::TRANT_GAMMA, 1.80078125);
		this->gtransfer.Set(Media::CS::TRANT_GAMMA, 1.80078125);
		this->btransfer.Set(Media::CS::TRANT_GAMMA, 1.80078125);
		break;
	case CPT_CIERGB:
		this->primaries.SetColorType(Media::ColorProfile::CT_CIERGB);
		this->rtransfer.Set(Media::CS::TRANT_GAMMA, 2.19921875);
		this->gtransfer.Set(Media::CS::TRANT_GAMMA, 2.19921875);
		this->btransfer.Set(Media::CS::TRANT_GAMMA, 2.19921875);
		break;
	case CPT_COLORMATCH:
		this->primaries.SetColorType(Media::ColorProfile::CT_COLORMATCH);
		this->rtransfer.Set(Media::CS::TRANT_GAMMA, 1.80078125);
		this->gtransfer.Set(Media::CS::TRANT_GAMMA, 1.80078125);
		this->btransfer.Set(Media::CS::TRANT_GAMMA, 1.80078125);
		break;
	case CPT_BT709:
		this->primaries.SetColorType(Media::ColorProfile::CT_BT709);
		this->rtransfer.Set(Media::CS::TRANT_BT709, 2.2);
		this->gtransfer.Set(Media::CS::TRANT_BT709, 2.2);
		this->btransfer.Set(Media::CS::TRANT_BT709, 2.2);
		break;
	case CPT_NTSC:
		this->primaries.SetColorType(Media::ColorProfile::CT_BT470M);
		this->rtransfer.Set(Media::CS::TRANT_NTSC, 2.2);
		this->gtransfer.Set(Media::CS::TRANT_NTSC, 2.2);
		this->btransfer.Set(Media::CS::TRANT_NTSC, 2.2);
		break;
	case CPT_PAL:
		this->primaries.SetColorType(Media::ColorProfile::CT_BT470BG);
		this->rtransfer.Set(Media::CS::TRANT_GAMMA, 2.8);////////////////
		this->gtransfer.Set(Media::CS::TRANT_GAMMA, 2.8);////////////////
		this->btransfer.Set(Media::CS::TRANT_GAMMA, 2.8);////////////////
		break;
	case CPT_SGI: //////////////////
		this->primaries.SetColorType(Media::ColorProfile::CT_APPLE);
		this->rtransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
		this->gtransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
		this->btransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
		break;
	case CPT_SMPTE240M:
		this->primaries.SetColorType(Media::ColorProfile::CT_SMPTE240M);
		this->rtransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
		this->gtransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
		this->btransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
		break;
	case CPT_SMPTEC:
		this->primaries.SetColorType(Media::ColorProfile::CT_SMPTE240M);
		this->rtransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
		this->gtransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
		this->btransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
		break;
	case CPT_SRGB:
		this->primaries.SetColorType(Media::ColorProfile::CT_SRGB);
		this->rtransfer.Set(Media::CS::TRANT_sRGB, 2.2);
		this->gtransfer.Set(Media::CS::TRANT_sRGB, 2.2);
		this->btransfer.Set(Media::CS::TRANT_sRGB, 2.2);
		break;
	case CPT_WIDE:
		this->primaries.SetColorType(Media::ColorProfile::CT_WIDE);
		this->rtransfer.Set(Media::CS::TRANT_GAMMA, 563.0 / 256.0);
		this->gtransfer.Set(Media::CS::TRANT_GAMMA, 563.0 / 256.0);
		this->btransfer.Set(Media::CS::TRANT_GAMMA, 563.0 / 256.0);
		break;
	case CPT_BT2020:
		this->primaries.SetColorType(Media::ColorProfile::CT_BT2020);
		this->rtransfer.Set(Media::CS::TRANT_BT709, 2.2);
		this->gtransfer.Set(Media::CS::TRANT_BT709, 2.2);
		this->btransfer.Set(Media::CS::TRANT_BT709, 2.2);
		break;
	case CPT_BT2100:
		this->primaries.SetColorType(Media::ColorProfile::CT_BT2020);
		this->rtransfer.Set(Media::CS::TRANT_BT2100, 2.2);
		this->gtransfer.Set(Media::CS::TRANT_BT2100, 2.2);
		this->btransfer.Set(Media::CS::TRANT_BT2100, 2.2);
		break;
	case CPT_CUSTOM:
		this->primaries.colorType = Media::ColorProfile::CT_CUSTOM;
		break;
	case CPT_PDISPLAY:
		this->primaries.SetColorType(Media::ColorProfile::CT_DISPLAY);
		this->rtransfer.Set(Media::CS::TRANT_PDISPLAY, 2.2);
		this->gtransfer.Set(Media::CS::TRANT_PDISPLAY, 2.2);
		this->btransfer.Set(Media::CS::TRANT_PDISPLAY, 2.2);
		break;
	case CPT_VDISPLAY:
		this->primaries.SetColorType(Media::ColorProfile::CT_DISPLAY);
		this->rtransfer.Set(Media::CS::TRANT_VDISPLAY, 2.2);
		this->gtransfer.Set(Media::CS::TRANT_VDISPLAY, 2.2);
		this->btransfer.Set(Media::CS::TRANT_VDISPLAY, 2.2);
		break;
	case CPT_PUNKNOWN:
		this->primaries.SetColorType(Media::ColorProfile::CT_PUNKNOWN);
		this->rtransfer.Set(Media::CS::TRANT_PUNKNOWN, 2.2);
		this->gtransfer.Set(Media::CS::TRANT_PUNKNOWN, 2.2);
		this->btransfer.Set(Media::CS::TRANT_PUNKNOWN, 2.2);
		break;
	case CPT_VUNKNOWN:
		this->primaries.SetColorType(Media::ColorProfile::CT_VUNKNOWN);
		this->rtransfer.Set(Media::CS::TRANT_VUNKNOWN, 2.2);
		this->gtransfer.Set(Media::CS::TRANT_VUNKNOWN, 2.2);
		this->btransfer.Set(Media::CS::TRANT_VUNKNOWN, 2.2);
		break;
	case CPT_FILE:
	case CPT_OS:
	case CPT_EDID:
		break;
	}
}

void Media::ColorProfile::RGB32ToXYZ(Int32 rgb, Double *X, Double *Y, Double *Z)
{
	Math::Matrix3 matrix;
	this->primaries.GetConvMatrix(&matrix);
	Media::CS::TransferFunc *rtrant = Media::CS::TransferFunc::CreateFunc(&this->rtransfer);
	Media::CS::TransferFunc *gtrant = Media::CS::TransferFunc::CreateFunc(&this->gtransfer);
	Media::CS::TransferFunc *btrant = Media::CS::TransferFunc::CreateFunc(&this->btransfer);
	Double mul = 1 / 255.0;
	Double r = rtrant->InverseTransfer(((rgb >> 16) & 255) * mul);
	Double g = gtrant->InverseTransfer(((rgb >> 8) & 255) * mul);
	Double b = btrant->InverseTransfer((rgb & 255) * mul);
	matrix.Multiply(r, g, b, X, Y, Z);
	DEL_CLASS(rtrant);
	DEL_CLASS(gtrant);
	DEL_CLASS(btrant);
}

Int32 Media::ColorProfile::XYZToRGB32(Double a, Double X, Double Y, Double Z)
{
	Math::Matrix3 matrix;
	Double R;
	Double G;
	Double B;
	Int32 outVal;
	Int32 iVal;
	this->primaries.GetConvMatrix(&matrix);
	Media::CS::TransferFunc *rtrant = Media::CS::TransferFunc::CreateFunc(&this->rtransfer);
	Media::CS::TransferFunc *gtrant = Media::CS::TransferFunc::CreateFunc(&this->gtransfer);
	Media::CS::TransferFunc *btrant = Media::CS::TransferFunc::CreateFunc(&this->btransfer);
	matrix.Inverse();
	matrix.Multiply(X, Y, Z, &R, &G, &B);
	R = rtrant->ForwardTransfer(R);
	G = gtrant->ForwardTransfer(G);
	B = btrant->ForwardTransfer(B);

	outVal = 0;
	iVal = Double2Int32(B * 255.0);
	if (iVal < 0)
		iVal = 0;
	else if (iVal > 255)
		iVal = 255;
	outVal |= iVal;
	iVal = Double2Int32(G * 255.0);
	if (iVal < 0)
		iVal = 0;
	else if (iVal > 255)
		iVal = 255;
	outVal |= iVal << 8;
	iVal = Double2Int32(R * 255.0);
	if (iVal < 0)
		iVal = 0;
	else if (iVal > 255)
		iVal = 255;
	outVal |= iVal << 16;
	iVal = Double2Int32(a * 255.0);
	if (iVal < 0)
		iVal = 0;
	else if (iVal > 255)
		iVal = 255;
	outVal |= iVal << 24;
	DEL_CLASS(rtrant);
	DEL_CLASS(gtrant);
	DEL_CLASS(btrant);
	return outVal;
}

void Media::ColorProfile::XYZWPTransform(WhitePointType srcWP, Double srcX, Double srcY, Double srcZ, WhitePointType destWP, Double *outX, Double *outY, Double *outZ)
{
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Matrix3 mat4;
	Math::Vector3 vec1;
	Math::Vector3 vec2;
	Math::Vector3 vec3;
	Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(srcWP, &vec2);
	Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(destWP, &vec3);
	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(&mat2);
	mat3.Set(&mat2);
	mat4.SetIdentity();
	mat2.Multiply(&vec2, &vec1);
	mat2.Multiply(&vec3, &vec2);
	mat2.Inverse();
	mat4.vec[0].val[0] = vec2.val[0] / vec1.val[0];
	mat4.vec[1].val[1] = vec2.val[1] / vec1.val[1];
	mat4.vec[2].val[2] = vec2.val[2] / vec1.val[2];
	mat2.Multiply(&mat4);
	mat2.Multiply(&mat3);
	mat2.Multiply(srcX, srcY, srcZ, outX, outY, outZ);
}

Bool Media::ColorProfile::Equals(const ColorProfile *profile)
{
	return this->primaries.Equals(&profile->primaries) && this->rtransfer.Equals(&profile->rtransfer) && this->gtransfer.Equals(&profile->gtransfer) && this->btransfer.Equals(&profile->btransfer);
}

Media::CS::TransferParam *Media::ColorProfile::GetRTranParam()
{
	return &this->rtransfer;
}

Media::CS::TransferParam *Media::ColorProfile::GetGTranParam()
{
	return &this->gtransfer;
}

Media::CS::TransferParam *Media::ColorProfile::GetBTranParam()
{
	return &this->btransfer;
}

const Media::CS::TransferParam *Media::ColorProfile::GetRTranParamRead() const
{
	return &this->rtransfer;
}

const Media::CS::TransferParam *Media::ColorProfile::GetGTranParamRead() const
{
	return &this->gtransfer;
}

const Media::CS::TransferParam *Media::ColorProfile::GetBTranParamRead() const
{
	return &this->btransfer;
}

Media::ColorProfile::ColorPrimaries *Media::ColorProfile::GetPrimaries()
{
	return &this->primaries;
}

const Media::ColorProfile::ColorPrimaries *Media::ColorProfile::GetPrimariesRead() const
{
	return &this->primaries;
}

void Media::ColorProfile::ToString(Text::StringBuilderUTF8 *sb)
{
	sb->AppendC(UTF8STRC("-R Transfer: "));
	sb->Append(Media::CS::TransferFunc::GetTransferFuncName(this->GetRTranParam()->GetTranType()));
	sb->AppendC(UTF8STRC("\r\n-G Transfer: "));
	sb->Append(Media::CS::TransferFunc::GetTransferFuncName(this->GetGTranParam()->GetTranType()));
	sb->AppendC(UTF8STRC("\r\n-B Transfer: "));
	sb->Append(Media::CS::TransferFunc::GetTransferFuncName(this->GetBTranParam()->GetTranType()));
	sb->AppendC(UTF8STRC("\r\n-Gamma: "));
	Text::SBAppendF64(sb, this->GetRTranParam()->GetGamma());
	Media::ColorProfile::ColorPrimaries *primaries = this->GetPrimaries(); 
	sb->AppendC(UTF8STRC("\r\n-RGB Primary: "));
	sb->Append(Media::ColorProfile::ColorTypeGetName(primaries->colorType));
	sb->AppendC(UTF8STRC("\r\n-Red:   "));
	Text::SBAppendF64(sb, primaries->rx);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, primaries->ry);
	sb->AppendC(UTF8STRC("\r\n-Green: "));
	Text::SBAppendF64(sb, primaries->gx);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, primaries->gy);
	sb->AppendC(UTF8STRC("\r\n-Blue:  "));
	Text::SBAppendF64(sb, primaries->bx);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, primaries->by);
	sb->AppendC(UTF8STRC("\r\n-White: "));
	Text::SBAppendF64(sb, primaries->wx);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, primaries->wy);
	sb->AppendC(UTF8STRC("\r\n"));

	if (this->rawICC)
	{
		Media::ICCProfile *icc = Media::ICCProfile::Parse(rawICC, ReadMUInt32(this->rawICC));
		if (icc)
		{
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("ICC Profile:\r\n"));
			icc->ToString(sb);
			DEL_CLASS(icc);
		}
	}
}

void Media::ColorProfile::SetRAWICC(const UInt8 *iccData)
{
	if (this->rawICC)
	{
		MemFree((UInt8*)this->rawICC);
		this->rawICC = 0;
	}
	if (iccData)
	{
		UOSInt leng = ReadMUInt32(iccData);
		UInt8 *newICC = MemAlloc(UInt8, leng);
		MemCopyNO(newICC, iccData, leng);
		this->rawICC = newICC;
	}
}

const UInt8 *Media::ColorProfile::GetRAWICC()
{
	return this->rawICC;
}

Text::CString Media::ColorProfile::YUVTypeGetName(Media::ColorProfile::YUVType yuvType)
{
	if (yuvType & Media::ColorProfile::YUVT_FLAG_YUV_0_255)
	{
		switch (yuvType & Media::ColorProfile::YUVT_MASK)
		{
		case Media::ColorProfile::YUVT_BT601:
			return CSTR("ITU-T BT. 601 (Full Range)");
		case Media::ColorProfile::YUVT_BT709:
			return CSTR("ITU-T BT. 709 (Full Range)");
		case Media::ColorProfile::YUVT_FCC:
			return CSTR("FCC (Full Range)");
		case Media::ColorProfile::YUVT_BT470BG:
			return CSTR("ITU-T BT. 470BG (Full Range)");
		case Media::ColorProfile::YUVT_SMPTE170M:
			return CSTR("SMPTE 170M (Full Range)");
		case Media::ColorProfile::YUVT_SMPTE240M:
			return CSTR("SMPTE 240M (Full Range)");
		case Media::ColorProfile::YUVT_BT2020:
			return CSTR("ITU-T BT. 2020 (Full Range)");
		case Media::ColorProfile::YUVT_UNKNOWN:
		default:
			return CSTR("Unknown (Full Range)");
		}
	}
	else
	{
		switch (yuvType & Media::ColorProfile::YUVT_MASK)
		{
		case Media::ColorProfile::YUVT_BT601:
			return CSTR("ITU-T BT. 601");
		case Media::ColorProfile::YUVT_BT709:
			return CSTR("ITU-T BT. 709");
		case Media::ColorProfile::YUVT_FCC:
			return CSTR("FCC");
		case Media::ColorProfile::YUVT_BT470BG:
			return CSTR("ITU-T BT. 470BG");
		case Media::ColorProfile::YUVT_SMPTE170M:
			return CSTR("SMPTE 170M");
		case Media::ColorProfile::YUVT_SMPTE240M:
			return CSTR("SMPTE 240M");
		case Media::ColorProfile::YUVT_BT2020:
			return CSTR("ITU-T BT. 2020");
		case Media::ColorProfile::YUVT_UNKNOWN:
		default:
			return CSTR("Unknown");
		}
	}
}

Text::CString Media::ColorProfile::ColorTypeGetName(ColorType colorType)
{
	switch (colorType)
	{
	case CT_SRGB:
	case CT_BT709:
		return CSTR("BT.709/sRGB");
	case CT_BT470M:
		return CSTR("BT.470M");
	case CT_BT470BG:
		return CSTR("BT.470BG");
	case CT_SMPTE170M:
		return CSTR("SMPTE 170M");
	case CT_SMPTE240M:
		return CSTR("SMPTE 240M");
	case CT_GENERIC_FILM:
		return CSTR("Generic Film");
	case CT_BT2020:
		return CSTR("BT.2020");
	case CT_ADOBE:
		return CSTR("Adobe RGB");
	case CT_APPLE:
		return CSTR("Apple RGB");
	case CT_CIERGB:
		return CSTR("CIE RGB");
	case CT_COLORMATCH:
		return CSTR("Colormatch RGB");
	case CT_WIDE:
		return CSTR("Wide");
	case CT_SGAMUT:
		return CSTR("Sony S-Gamut");
	case CT_SGAMUTCINE:
		return CSTR("Sony S-Gamut3.Cine");
	case CT_DCI_P3:
		return CSTR("DCI-P3");
	case CT_ACESGAMUT:
		return CSTR("ACES-Gamut");
	case CT_ALEXAWIDE:
		return CSTR("ALEXA Wide Gamut");
	case CT_VGAMUT:
		return CSTR("V-Gamut");
	case CT_CUSTOM:
		return CSTR("Custom");
	case CT_DISPLAY:
		return CSTR("As Display Color");
	case CT_PUNKNOWN:
	case CT_VUNKNOWN:
	default:
		return CSTR("Unknown");
	}
}

Text::CString Media::ColorProfile::CommonProfileTypeGetName(CommonProfileType cpt)
{
	switch (cpt)
	{
	case CPT_ADOBE:
		return CSTR("Adobe RGB");
	case CPT_APPLE:
		return CSTR("Apple RGB");
	case CPT_CIERGB:
		return CSTR("CIE RGB");
	case CPT_COLORMATCH:
		return CSTR("ColorMatch RGB");
	case CPT_BT709:
		return CSTR("ITU-R Bt.709");
	case CPT_NTSC:
		return CSTR("NTSC");
	case CPT_PAL:
		return CSTR("PAL");
	case CPT_SGI:
		return CSTR("SGI");
	case CPT_SMPTE240M:
		return CSTR("SMPTE 240M");
	case CPT_SMPTEC:
		return CSTR("SMPTE C");
	case CPT_SRGB:
		return CSTR("sRGB");
	case CPT_WIDE:
		return CSTR("Wide");
	case CPT_BT2020:
		return CSTR("BT.2020");
	case CPT_BT2100:
		return CSTR("BT.2100");
	case CPT_FILE:
		return CSTR("File");
	case CPT_CUSTOM:
		return CSTR("Custom");
	case CPT_PDISPLAY:
		return CSTR("From Display Unit (For Picture)");
	case CPT_VDISPLAY:
		return CSTR("From Display Unit (For Video)");
	case CPT_OS:
		return CSTR("OS");
	case CPT_EDID:
		return CSTR("From EDID");
	case CPT_VUNKNOWN:
		return CSTR("Unknown (For Video)");
	case CPT_PUNKNOWN:
		return CSTR("Unknwon (For Picture)");
	default:
		return CSTR("Unknown");
	}
}

void Media::ColorProfile::GetYUVConstants(YUVType yuvType, Double *Kr, Double *Kb)
{
	switch (yuvType & Media::ColorProfile::YUVT_MASK)
	{
	case Media::ColorProfile::YUVT_UNKNOWN:
	case Media::ColorProfile::YUVT_BT601:
	default:
		*Kr = 0.2985;
		*Kb = 0.1135;
		break;
	case Media::ColorProfile::YUVT_BT709:
		*Kr = 0.2126;
		*Kb = 0.0722;
		break;
	case Media::ColorProfile::YUVT_FCC:
		*Kr = 0.30;
		*Kb = 0.11;
		break;
	case Media::ColorProfile::YUVT_BT470BG:
		*Kr = 0.299;
		*Kb = 0.114;
		break;
	case Media::ColorProfile::YUVT_SMPTE170M:
		*Kr = 0.299;
		*Kb = 0.114;
		break;
	case Media::ColorProfile::YUVT_SMPTE240M:
		*Kr = 0.212;
		*Kb = 0.087;
		break;
	case Media::ColorProfile::YUVT_BT2020:
		*Kr = 0.2627;
		*Kb = 0.0593;
		break;
	}
}


void Media::ColorProfile::YUV2RGB(YUVType yuvType, Double y, Double u, Double v, Double *r, Double *g, Double *b)
{
	Double Kr;
	Double Kb;
	Double Kg;
	Double Kc1;
	Double Kc2;
	Double Kc3;
	Double Kc4;
	GetYUVConstants(yuvType, &Kr, &Kb);
	Kg = 1 - Kr - Kb;
	Kc1 = (1 - Kr) / 0.5;
	Kc2 = -(2 * Kr - 2 * Kr * Kr) / Kg;
	Kc3 = -(2 * Kb - 2 * Kb * Kb) / Kg;
	Kc4 = (1 - Kb) / 0.5;
	*r = y + Kc1 * v;
	*g = y + Kc2 * u + Kc3 * v;
	*b = y + Kc4 * u;
}

void Media::ColorProfile::RGB2RGB(Media::ColorProfile *srcColor, Media::ColorProfile *destColor, Double srcR, Double srcG, Double srcB, Double *destR, Double *destG, Double *destB)
{
	Media::CS::TransferFunc *func1r;
	Media::CS::TransferFunc *func1g;
	Media::CS::TransferFunc *func1b;
	Media::CS::TransferFunc *func2r;
	Media::CS::TransferFunc *func2g;
	Media::CS::TransferFunc *func2b;
	Math::Matrix3 mat;
	Math::Matrix3 mat2;
	Double v1;
	Double v2;
	Double v3;
	srcColor->primaries.GetConvMatrix(&mat);
	destColor->primaries.GetConvMatrix(&mat2);
	mat2.Inverse();
	mat.MyMultiply(&mat2);
	func1r = Media::CS::TransferFunc::CreateFunc(&srcColor->rtransfer);
	func1g = Media::CS::TransferFunc::CreateFunc(&srcColor->gtransfer);
	func1b = Media::CS::TransferFunc::CreateFunc(&srcColor->btransfer);
	func2r = Media::CS::TransferFunc::CreateFunc(&destColor->rtransfer);
	func2g = Media::CS::TransferFunc::CreateFunc(&destColor->gtransfer);
	func2b = Media::CS::TransferFunc::CreateFunc(&destColor->btransfer);
	v1 = func1r->InverseTransfer(srcR);
	v2 = func1g->InverseTransfer(srcG);
	v3 = func1b->InverseTransfer(srcB);
	*destR = func2r->ForwardTransfer(mat.vec[0].val[0] * v1 + mat.vec[0].val[1] * v2 + mat.vec[0].val[2] * v3);
	*destG = func2g->ForwardTransfer(mat.vec[1].val[0] * v1 + mat.vec[1].val[1] * v2 + mat.vec[1].val[2] * v3);
	*destB = func2b->ForwardTransfer(mat.vec[2].val[0] * v1 + mat.vec[2].val[1] * v2 + mat.vec[2].val[2] * v3);
	DEL_CLASS(func1r);
	DEL_CLASS(func1g);
	DEL_CLASS(func1b);
	DEL_CLASS(func2r);
	DEL_CLASS(func2g);
	DEL_CLASS(func2b);
}
