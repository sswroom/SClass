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

Math::Double2D Media::ColorProfile::ColorCoordinate::Getxy()
{
	Double avg = (this->X + this->Y + this->Z);
	return Math::Double2D(this->X / avg, this->Y / avg);
}

void Media::ColorProfile::ColorPrimaries::SetWhiteType(WhitePointType wpType)
{
	this->w = GetWhitePointXY(wpType);
}

void Media::ColorProfile::ColorPrimaries::SetWhiteTemp(Double colorTemp)
{
	this->w = GetWhitePointXY(colorTemp);
}

Media::ColorProfile::ColorPrimaries::ColorPrimaries()
{
	this->SetColorType(Media::ColorProfile::CT_PUNKNOWN);
}

Media::ColorProfile::ColorPrimaries::~ColorPrimaries()
{
}

void Media::ColorProfile::ColorPrimaries::Set(NN<const ColorPrimaries> primaries)
{
	this->colorType = primaries->colorType;
	this->r = primaries->r;
	this->g = primaries->g;
	this->b = primaries->b;
	this->w = primaries->w;
}

void Media::ColorProfile::ColorPrimaries::SetColorType(ColorType colorType)
{
	switch (colorType)
	{
	case CT_ADOBE:
		this->colorType = CT_ADOBE;
		this->r = Math::Double2D(0.6400, 0.3300);
		this->g = Math::Double2D(0.2100, 0.7100);
		this->b = Math::Double2D(0.1500, 0.0600);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_APPLE:
		this->colorType = CT_APPLE;
		this->r = Math::Double2D(0.6250, 0.3400);
		this->g = Math::Double2D(0.2800, 0.5950);
		this->b = Math::Double2D(0.1550, 0.0700);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_CIERGB:
		this->colorType = CT_CIERGB;
		this->r = Math::Double2D(0.7350, 0.2650);
		this->g = Math::Double2D(0.2740, 0.7170);
		this->b = Math::Double2D(0.1670, 0.0090);
		this->SetWhiteType(WPT_E);
		break;

	case CT_COLORMATCH:
		this->colorType = CT_COLORMATCH;
		this->r = Math::Double2D(0.6300, 0.3400);
		this->g = Math::Double2D(0.2950, 0.6050);
		this->b = Math::Double2D(0.1500, 0.0750);
		this->SetWhiteType(WPT_D65);
		break;
		
	case CT_SRGB:
	case CT_BT709:
		this->colorType = CT_BT709;
		this->r = Math::Double2D(0.6400, 0.3300);
		this->g = Math::Double2D(0.3000, 0.6000);
		this->b = Math::Double2D(0.1500, 0.0600);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_BT470M: //NTSC
		this->colorType = CT_BT470M;
		this->r = Math::Double2D(0.6700, 0.3300);
		this->g = Math::Double2D(0.2100, 0.7100);
		this->b = Math::Double2D(0.1400, 0.0800);
		this->SetWhiteType(WPT_C);
		break;

	case CT_BT470BG: //PAL
		this->colorType = CT_BT470BG;
		this->r = Math::Double2D(0.6400, 0.3300);
		this->g = Math::Double2D(0.2900, 0.6000);
		this->b = Math::Double2D(0.1500, 0.0600);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_SMPTE170M:
		this->colorType = CT_SMPTE170M;
		this->r = Math::Double2D(0.640, 0.340);
		this->g = Math::Double2D(0.310, 0.595);
		this->b = Math::Double2D(0.155, 0.070);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_SMPTE240M:
		this->colorType = CT_SMPTE240M;
		this->r = Math::Double2D(0.6300, 0.3400);
		this->g = Math::Double2D(0.3100, 0.5950);
		this->b = Math::Double2D(0.1550, 0.0700);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_GENERIC_FILM:
		this->colorType = CT_GENERIC_FILM;
		this->r = Math::Double2D(0.681, 0.319);
		this->g = Math::Double2D(0.243, 0.692);
		this->b = Math::Double2D(0.145, 0.049);
		this->SetWhiteType(WPT_C);
		break;

	case CT_BT2020:
		this->colorType = CT_BT2020;
		this->r = Math::Double2D(0.708, 0.292);
		this->g = Math::Double2D(0.170, 0.797);
		this->b = Math::Double2D(0.131, 0.046);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_WIDE: //Wide Gamut
		this->colorType = CT_WIDE;
		this->r = Math::Double2D(0.7347, 0.2653);
		this->g = Math::Double2D(0.1152, 0.8264);
		this->b = Math::Double2D(0.1566, 0.0177);
		this->SetWhiteType(WPT_D50);
		break;

	case CT_SGAMUT:
		this->colorType = CT_SGAMUT;
		this->r = Math::Double2D(0.73, 0.28);
		this->g = Math::Double2D(0.14, 0.855);
		this->b = Math::Double2D(0.10, -0.05);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_SGAMUTCINE:
		this->colorType = CT_SGAMUTCINE;
		this->r = Math::Double2D(0.76600, 0.27500);
		this->g = Math::Double2D(0.22500, 0.80000);
		this->b = Math::Double2D(0.08900, -0.08700);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_SMPTE_RP431_2:
		this->colorType = CT_SMPTE_RP431_2;
		this->r = Math::Double2D(0.68000, 0.32000);
		this->g = Math::Double2D(0.26500, 0.69000);
		this->b = Math::Double2D(0.15000, 0.06000);
		this->SetWhiteType(WPT_SMPTE431);
		break;

	case CT_ACESGAMUT:
		this->colorType = CT_ACESGAMUT;
		this->r = Math::Double2D(0.73470, 0.26530);
		this->g = Math::Double2D(0.00000, 1.00000);
		this->b = Math::Double2D(0.00010, -0.07700);
		this->w = Math::Double2D(0.32168, 0.33767);
		break;

	case CT_ALEXAWIDE:
		this->colorType = CT_ALEXAWIDE;
		this->r = Math::Double2D(0.6840, 0.3130);
		this->g = Math::Double2D(0.2210, 0.8480);
		this->b = Math::Double2D(0.0861, -0.1020);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_VGAMUT:
		this->colorType = CT_VGAMUT;
		this->r = Math::Double2D(0.730, 0.280);
		this->g = Math::Double2D(0.165, 0.840);
		this->b = Math::Double2D(0.100, -0.030);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_GOPRO_PROTUNE:
		this->colorType = CT_GOPRO_PROTUNE;
		this->r = Math::Double2D(0.698448, 0.193026);
		this->g = Math::Double2D(0.329555, 1.024597);
		this->b = Math::Double2D(0.108443, -0.034679);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_SMPTE_EG432_1:
		this->colorType = CT_SMPTE_EG432_1;
		this->r = Math::Double2D(0.68000, 0.32000);
		this->g = Math::Double2D(0.26500, 0.69000);
		this->b = Math::Double2D(0.15000, 0.06000);
		this->SetWhiteType(WPT_SMPTE432);
		break;

	case CT_EBU3213:
		this->colorType = CT_EBU3213;
		this->r = Math::Double2D(0.630, 0.340);
		this->g = Math::Double2D(0.295, 0.605);
		this->b = Math::Double2D(0.155, 0.077);
		this->SetWhiteType(WPT_SMPTE432);
		break;

	case CT_SMPTE428:
		this->colorType = CT_SMPTE428;
		this->r = Math::Double2D(1.0, 0.0);
		this->g = Math::Double2D(0.0, 1.0);
		this->b = Math::Double2D(0.0, 0.0);
		this->SetWhiteType(WPT_SMPTE428);
		break;

	case CT_CUSTOM:
		this->colorType = CT_CUSTOM;
		this->r = Math::Double2D(0.6400, 0.3300);
		this->g = Math::Double2D(0.3000, 0.6000);
		this->b = Math::Double2D(0.1500, 0.0600);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_DISPLAY:
		this->colorType = CT_DISPLAY;
		this->r = Math::Double2D(0.6400, 0.3300);
		this->g = Math::Double2D(0.3000, 0.6000);
		this->b = Math::Double2D(0.1500, 0.0600);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_VUNKNOWN:
		this->colorType = CT_VUNKNOWN;
		this->r = Math::Double2D(0.6400, 0.3300);
		this->g = Math::Double2D(0.3000, 0.6000);
		this->b = Math::Double2D(0.1500, 0.0600);
		this->SetWhiteType(WPT_D65);
		break;

	case CT_PUNKNOWN:
	default:
		this->colorType = Media::ColorProfile::CT_PUNKNOWN;
		this->r = Math::Double2D(0.6400, 0.3300);
		this->g = Math::Double2D(0.3000, 0.6000);
		this->b = Math::Double2D(0.1500, 0.0600);
		this->SetWhiteType(WPT_D65);
		break;
	}
}

Math::Double2D Media::ColorProfile::ColorPrimaries::GetWhitexy() const
{
	return this->w;
}

void Media::ColorProfile::ColorPrimaries::GetConvMatrix(NN<Math::Matrix3> matrix) const
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

	Xr = this->r.x / this->r.y;
	Yr = 1.0;
	Zr = (1 - this->r.x - this->r.y) / this->r.y;
	Xg = this->g.x / this->g.y;
	Yg = 1.0;
	Zg = (1 - this->g.x - this->g.y) / this->g.y;
	Xb = this->b.x / this->b.y;
	Yb = 1.0;
	Zb = (1 - this->b.x - this->b.y) / this->b.y;
	Xw = this->w.x / this->w.y;
	Yw = 1.0;
	Zw = (1 - this->w.x - this->w.y) / this->w.y;
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

	matrixTmp.Multiply(Xw, Yw, Zw, Sr, Sg, Sb);
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

void Media::ColorProfile::ColorPrimaries::SetConvMatrix(NN<const Math::Matrix3> matrix)
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
	this->r.y = 1 / (Z + X + 1);
	this->r.x = X * this->r.y;
	X = matrix->vec[0].val[1] / Sg;
//	Y = matrix->vec[1].val[1] / Sg;
	Z = matrix->vec[2].val[1] / Sg;
	this->g.y = 1 / (Z + X + 1);
	this->g.x = X * this->g.y;
	X = matrix->vec[0].val[2] / Sb;
//	Y = matrix->vec[1].val[2] / Sb;
	Z = matrix->vec[2].val[2] / Sb;
	this->b.y = 1 / (Z + X + 1);
	this->b.x = X * this->b.y;
	X = this->r.x / this->r.y * Sr + this->g.x / this->g.y * Sg + this->b.x / this->b.y * Sb;
//	Y = 1.0;
	Z = (1 - this->r.x - this->r.y) / this->r.y * Sr + (1 - this->g.x - this->g.y) / this->g.y * Sg + (1 - this->b.x - this->b.y) / this->b.y * Sb;
	this->w.y = 1 / (Z + X + 1);
	this->w.x = X * this->w.y;
}

Bool Media::ColorProfile::ColorPrimaries::Equals(NN<const ColorPrimaries> primaries) const
{
	return this->r == primaries->r && this->g == primaries->g && this->b == primaries->b && this->w == primaries->w;
}

Math::Double2D Media::ColorProfile::ColorPrimaries::GetWhitePointXY(WhitePointType wpType)
{
	switch (wpType)
	{
	case WPT_A:
		return Math::Double2D(0.44757, 0.40745);
	case WPT_B:
		return Math::Double2D(0.34842, 0.35161);
	case WPT_C:
		return Math::Double2D(0.310063, 0.316158);
	case WPT_D50:
//		return Math::Double2D(0.34567, 0.35850);
		return Math::Double2D(0.34566918689481363576071096962462, 0.35849618022319972180696414679702);
// XYZ: 0.96422 	1.00000 	0.82521
	case WPT_D55:
		return Math::Double2D(0.33242, 0.34743);
	default:
	case WPT_D65:
//		return Math::Double2D(0.31271, 0.32902);
		return Math::Double2D(0.31272661468101207514888296647254, 0.32902313032606192215312736485375);
// XYZ: 0.95047 	1.00000 	1.08883
	case WPT_D75:
		return Math::Double2D(0.29902, 0.31485);
	case WPT_E:
		return Math::Double2D(1 / 3.0, 1 / 3.0);
	case WPT_F1:
		return Math::Double2D(0.31310, 0.33727);
	case WPT_F2:
		return Math::Double2D(0.37208, 0.37529);
	case WPT_F3:
		return Math::Double2D(0.40910, 0.39430);
	case WPT_F4:
		return Math::Double2D(0.44018, 0.40329);
	case WPT_F5:
		return Math::Double2D(0.31379, 0.34531);
	case WPT_F6:
		return Math::Double2D(0.37790, 0.38835);
	case WPT_F7:
		return Math::Double2D(0.31292, 0.32933);
	case WPT_F8:
		return Math::Double2D(0.34588, 0.35875);
	case WPT_F9:
		return Math::Double2D(0.37417, 0.37281);
	case WPT_F10:
		return Math::Double2D(0.34609, 0.35986);
	case WPT_F11:
		return Math::Double2D(0.38052, 0.37713);
	case WPT_F12:
		return Math::Double2D(0.43695, 0.40441);
	case WPT_SMPTE431:
		return Math::Double2D(0.31400, 0.35100);
	case WPT_SMPTE432:
		return Math::Double2D(0.3127, 0.3290);
	case WPT_SMPTE428:
		return Math::Double2D(1.0/3, 1.0/3);
	}

}

Math::Double2D Media::ColorProfile::ColorPrimaries::GetWhitePointXY(Double colorTemp)
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
	return Math::Double2D(currX, currY);
}

Math::Vector3 Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(WhitePointType wpType)
{
	return xyYToXYZ(Math::Vector3(GetWhitePointXY(wpType), 1.0));
}

void Media::ColorProfile::ColorPrimaries::GetMatrixBradford(NN<Math::Matrix3> mat)
{
	mat->vec[0].Set( 0.8951,  0.2664, -0.1614);
	mat->vec[1].Set(-0.7502,  1.7135,  0.0367);
	mat->vec[2].Set( 0.0389, -0.0685,  1.0296);
}

void Media::ColorProfile::ColorPrimaries::GetMatrixVonKries(NN<Math::Matrix3> mat)
{
	mat->vec[0].Set( 0.40024,  0.70760, -0.08081);
	mat->vec[1].Set(-0.22630,  1.16532,  0.04570);
	mat->vec[2].Set( 0.00000,  0.00000,  0.91822);
}

Math::Vector3 Media::ColorProfile::ColorPrimaries::xyYToXYZ(const Math::Vector3 &xyY)
{
	Double v = xyY.val[2] / xyY.val[1];
	return Math::Vector3(xyY.val[0] * v, xyY.val[2], (1 - xyY.val[0] - xyY.val[1]) * v);
}

Math::Vector3 Media::ColorProfile::ColorPrimaries::XYZToxyY(const Math::Vector3 &XYZ)
{
	Double sum = XYZ.val[0] + XYZ.val[1] + XYZ.val[2];
	return Math::Vector3(XYZ.val[0] / sum, XYZ.val[1] / sum, XYZ.val[1]);
}

void Media::ColorProfile::ColorPrimaries::GetAdaptationMatrix(NN<Math::Matrix3> mat, WhitePointType srcWP, WhitePointType destWP)
{
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Vector3 vec1;
	Math::Vector3 vec2;

	//color.primaries.GetConvMatrix(&mat);
	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(mat);
	mat2.Set(mat);
	mat3.SetIdentity();

	vec1 = mat->Multiply(Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(srcWP));
	vec2 = mat->Multiply(Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(destWP));
	mat->Inverse();
	mat3.vec[0].val[0] = vec2.val[0] / vec1.val[0];
	mat3.vec[1].val[1] = vec2.val[1] / vec1.val[1];
	mat3.vec[2].val[2] = vec2.val[2] / vec1.val[2];
	mat->Multiply(mat3);
	mat->Multiply(mat2);
}

Media::ColorProfile::ColorProfile()
{
	this->rawICC = 0;
}

Media::ColorProfile::ColorProfile(NN<const ColorProfile> profile)
{
	this->rawICC = 0;
	this->Set(profile);
}

Media::ColorProfile::ColorProfile(CommonProfileType cpt)
{
	this->rawICC = 0;
	this->SetCommonProfile(cpt);
}

Media::ColorProfile::ColorProfile(const ColorProfile &profile)
{
	this->rawICC = 0;
	this->Set(profile);
}

Media::ColorProfile::~ColorProfile()
{
	UnsafeArray<const UInt8> rawICC;
	if (this->rawICC.SetTo(rawICC))
	{
		MemFreeArr(rawICC);
		this->rawICC = 0;
	}
}

void Media::ColorProfile::Set(NN<const ColorProfile> profile)
{
	this->rtransfer.Set(profile->rtransfer);
	this->gtransfer.Set(profile->gtransfer);
	this->btransfer.Set(profile->btransfer);
	this->primaries.Set(profile->primaries);
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

void Media::ColorProfile::RGB32ToXYZ(Int32 rgb, OutParam<Double> X, OutParam<Double> Y, OutParam<Double> Z) const
{
	Math::Matrix3 matrix;
	this->primaries.GetConvMatrix(matrix);
	NN<Media::CS::TransferFunc> rtrant = Media::CS::TransferFunc::CreateFunc(this->rtransfer);
	NN<Media::CS::TransferFunc> gtrant = Media::CS::TransferFunc::CreateFunc(this->gtransfer);
	NN<Media::CS::TransferFunc> btrant = Media::CS::TransferFunc::CreateFunc(this->btransfer);
	Double mul = 1 / 255.0;
	Double r = rtrant->InverseTransfer(((rgb >> 16) & 255) * mul);
	Double g = gtrant->InverseTransfer(((rgb >> 8) & 255) * mul);
	Double b = btrant->InverseTransfer((rgb & 255) * mul);
	matrix.Multiply(r, g, b, X, Y, Z);
	rtrant.Delete();
	gtrant.Delete();
	btrant.Delete();
}

Int32 Media::ColorProfile::XYZToRGB32(Double a, Double X, Double Y, Double Z) const
{
	Math::Matrix3 matrix;
	Double R;
	Double G;
	Double B;
	Int32 outVal;
	Int32 iVal;
	this->primaries.GetConvMatrix(matrix);
	NN<Media::CS::TransferFunc> rtrant = Media::CS::TransferFunc::CreateFunc(this->rtransfer);
	NN<Media::CS::TransferFunc> gtrant = Media::CS::TransferFunc::CreateFunc(this->gtransfer);
	NN<Media::CS::TransferFunc> btrant = Media::CS::TransferFunc::CreateFunc(this->btransfer);
	matrix.Inverse();
	matrix.Multiply(X, Y, Z, R, G, B);
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
	rtrant.Delete();
	gtrant.Delete();
	btrant.Delete();
	return outVal;
}

void Media::ColorProfile::XYZWPTransform(WhitePointType srcWP, Double srcX, Double srcY, Double srcZ, WhitePointType destWP, OutParam<Double> outX, OutParam<Double> outY, OutParam<Double> outZ)
{
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Matrix3 mat4;
	Math::Vector3 vec1;
	Math::Vector3 vec2;

	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(mat2);
	mat3.Set(mat2);
	mat4.SetIdentity();
	vec1 = mat2.Multiply(Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(srcWP));
	vec2 = mat2.Multiply(Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(destWP));
	mat2.Inverse();
	mat4.vec[0].val[0] = vec2.val[0] / vec1.val[0];
	mat4.vec[1].val[1] = vec2.val[1] / vec1.val[1];
	mat4.vec[2].val[2] = vec2.val[2] / vec1.val[2];
	mat2.Multiply(mat4);
	mat2.Multiply(mat3);
	mat2.Multiply(srcX, srcY, srcZ, outX, outY, outZ);
}

Bool Media::ColorProfile::Equals(NN<const ColorProfile> profile) const
{
	return this->primaries.Equals(profile->primaries) && this->rtransfer.Equals(profile->rtransfer) && this->gtransfer.Equals(profile->gtransfer) && this->btransfer.Equals(profile->btransfer);
}

NN<Media::CS::TransferParam> Media::ColorProfile::GetRTranParam()
{
	return this->rtransfer;
}

NN<Media::CS::TransferParam> Media::ColorProfile::GetGTranParam()
{
	return this->gtransfer;
}

NN<Media::CS::TransferParam> Media::ColorProfile::GetBTranParam()
{
	return this->btransfer;
}

NN<const Media::CS::TransferParam> Media::ColorProfile::GetRTranParamRead() const
{
	return this->rtransfer;
}

NN<const Media::CS::TransferParam> Media::ColorProfile::GetGTranParamRead() const
{
	return this->gtransfer;
}

NN<const Media::CS::TransferParam> Media::ColorProfile::GetBTranParamRead() const
{
	return this->btransfer;
}

NN<Media::ColorProfile::ColorPrimaries> Media::ColorProfile::GetPrimaries()
{
	return this->primaries;
}

NN<const Media::ColorProfile::ColorPrimaries> Media::ColorProfile::GetPrimariesRead() const
{
	return this->primaries;
}

void Media::ColorProfile::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("-R Transfer: "));
	sb->Append(Media::CS::TransferTypeGetName(this->GetRTranParamRead()->GetTranType()));
	sb->AppendC(UTF8STRC("\r\n-G Transfer: "));
	sb->Append(Media::CS::TransferTypeGetName(this->GetGTranParamRead()->GetTranType()));
	sb->AppendC(UTF8STRC("\r\n-B Transfer: "));
	sb->Append(Media::CS::TransferTypeGetName(this->GetBTranParamRead()->GetTranType()));
	sb->AppendC(UTF8STRC("\r\n-Gamma: "));
	Text::SBAppendF64(sb, this->GetRTranParamRead()->GetGamma());
	NN<const Media::ColorProfile::ColorPrimaries> primaries = this->GetPrimariesRead(); 
	sb->AppendC(UTF8STRC("\r\n-RGB Primary: "));
	sb->Append(Media::ColorProfile::ColorTypeGetName(primaries->colorType));
	sb->AppendC(UTF8STRC("\r\n-Red:   "));
	Text::SBAppendF64(sb, primaries->r.x);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, primaries->r.y);
	sb->AppendC(UTF8STRC("\r\n-Green: "));
	Text::SBAppendF64(sb, primaries->g.x);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, primaries->g.y);
	sb->AppendC(UTF8STRC("\r\n-Blue:  "));
	Text::SBAppendF64(sb, primaries->b.x);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, primaries->b.y);
	sb->AppendC(UTF8STRC("\r\n-White: "));
	Text::SBAppendF64(sb, primaries->w.x);
	sb->AppendC(UTF8STRC(", "));
	Text::SBAppendF64(sb, primaries->w.y);
	sb->AppendC(UTF8STRC("\r\n"));

	UnsafeArray<const UInt8> rawICC;
	if (this->rawICC.SetTo(rawICC))
	{
		NN<Media::ICCProfile> icc;
		if (Media::ICCProfile::Parse(Data::ByteArrayR(rawICC, ReadMUInt32(&rawICC[0]))).SetTo(icc))
		{
			sb->AppendC(UTF8STRC("\r\n"));
			sb->AppendC(UTF8STRC("ICC Profile:\r\n"));
			icc->ToString(sb);
			icc.Delete();
		}
	}
}

void Media::ColorProfile::SetRAWICC(UnsafeArrayOpt<const UInt8> iccData)
{
	UnsafeArray<const UInt8> icc;
	if (this->rawICC.SetTo(icc))
	{
		MemFreeArr(icc);
		this->rawICC = 0;
	}
	if (iccData.SetTo(icc))
	{
		UOSInt leng = ReadMUInt32(&icc[0]);
		UnsafeArray<UInt8> newICC = MemAllocArr(UInt8, leng);
		MemCopyNO(newICC.Ptr(), icc.Ptr(), leng);
		this->rawICC = UnsafeArray<const UInt8>(newICC);
	}
}

UnsafeArrayOpt<const UInt8> Media::ColorProfile::GetRAWICC() const
{
	return this->rawICC;
}

Text::CStringNN Media::ColorProfile::YUVTypeGetName(Media::ColorProfile::YUVType yuvType)
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

Text::CStringNN Media::ColorProfile::ColorTypeGetName(ColorType colorType)
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
	case CT_SMPTE_EG432_1:
		return CSTR("SMPTE EG432-1 (DCI-P3)");
	case CT_ACESGAMUT:
		return CSTR("ACES-Gamut");
	case CT_ALEXAWIDE:
		return CSTR("ALEXA Wide Gamut");
	case CT_VGAMUT:
		return CSTR("V-Gamut");
	case CT_GOPRO_PROTUNE:
		return CSTR("GoPro Protune");
	case CT_SMPTE_RP431_2:
		return CSTR("SMPTE RP431-2");
	case CT_EBU3213:
		return CSTR("EBU 3213");
	case CT_SMPTE428:
		return CSTR("SMPTE ST 428-1");
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

Text::CStringNN Media::ColorProfile::CommonProfileTypeGetName(CommonProfileType cpt)
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

void Media::ColorProfile::GetYUVConstants(YUVType yuvType, OutParam<Double> Kr, OutParam<Double> Kb)
{
	switch (yuvType & Media::ColorProfile::YUVT_MASK)
	{
	case Media::ColorProfile::YUVT_UNKNOWN:
	case Media::ColorProfile::YUVT_BT601:
	default:
		Kr.Set(0.2985);
		Kb.Set(0.1135);
		break;
	case Media::ColorProfile::YUVT_BT709:
		Kr.Set(0.2126);
		Kb.Set(0.0722);
		break;
	case Media::ColorProfile::YUVT_FCC:
		Kr.Set(0.30);
		Kb.Set(0.11);
		break;
	case Media::ColorProfile::YUVT_BT470BG:
		Kr.Set(0.299);
		Kb.Set(0.114);
		break;
	case Media::ColorProfile::YUVT_SMPTE170M:
		Kr.Set(0.299);
		Kb.Set(0.114);
		break;
	case Media::ColorProfile::YUVT_SMPTE240M:
		Kr.Set(0.212);
		Kb.Set(0.087);
		break;
	case Media::ColorProfile::YUVT_BT2020:
		Kr.Set(0.2627);
		Kb.Set(0.0593);
		break;
	}
}


void Media::ColorProfile::YUV2RGB(YUVType yuvType, Double y, Double u, Double v, OutParam<Double> r, OutParam<Double> g, OutParam<Double> b)
{
	Double Kr;
	Double Kb;
	Double Kg;
	Double Kc1;
	Double Kc2;
	Double Kc3;
	Double Kc4;
	GetYUVConstants(yuvType, Kr, Kb);
	Kg = 1 - Kr - Kb;
	Kc1 = (1 - Kr) / 0.5;
	Kc2 = -(2 * Kr - 2 * Kr * Kr) / Kg;
	Kc3 = -(2 * Kb - 2 * Kb * Kb) / Kg;
	Kc4 = (1 - Kb) / 0.5;
	r.Set(y + Kc1 * v);
	g.Set(y + Kc2 * u + Kc3 * v);
	b.Set(y + Kc4 * u);
}

void Media::ColorProfile::RGB2RGB(NN<Media::ColorProfile> srcColor, NN<Media::ColorProfile> destColor, Double srcR, Double srcG, Double srcB, OutParam<Double> destR, OutParam<Double> destG, OutParam<Double> destB)
{
	NN<Media::CS::TransferFunc> func1r;
	NN<Media::CS::TransferFunc> func1g;
	NN<Media::CS::TransferFunc> func1b;
	NN<Media::CS::TransferFunc> func2r;
	NN<Media::CS::TransferFunc> func2g;
	NN<Media::CS::TransferFunc> func2b;
	Math::Matrix3 mat;
	Math::Matrix3 mat2;
	Double v1;
	Double v2;
	Double v3;
	srcColor->primaries.GetConvMatrix(mat);
	destColor->primaries.GetConvMatrix(mat2);
	mat2.Inverse();
	mat.MultiplyBA(mat2);
	func1r = Media::CS::TransferFunc::CreateFunc(srcColor->rtransfer);
	func1g = Media::CS::TransferFunc::CreateFunc(srcColor->gtransfer);
	func1b = Media::CS::TransferFunc::CreateFunc(srcColor->btransfer);
	func2r = Media::CS::TransferFunc::CreateFunc(destColor->rtransfer);
	func2g = Media::CS::TransferFunc::CreateFunc(destColor->gtransfer);
	func2b = Media::CS::TransferFunc::CreateFunc(destColor->btransfer);
	v1 = func1r->InverseTransfer(srcR);
	v2 = func1g->InverseTransfer(srcG);
	v3 = func1b->InverseTransfer(srcB);
	destR.Set(func2r->ForwardTransfer(mat.vec[0].val[0] * v1 + mat.vec[0].val[1] * v2 + mat.vec[0].val[2] * v3));
	destG.Set(func2g->ForwardTransfer(mat.vec[1].val[0] * v1 + mat.vec[1].val[1] * v2 + mat.vec[1].val[2] * v3));
	destB.Set(func2b->ForwardTransfer(mat.vec[2].val[0] * v1 + mat.vec[2].val[1] * v2 + mat.vec[2].val[2] * v3));
	func1r.Delete();
	func1g.Delete();
	func1b.Delete();
	func2r.Delete();
	func2g.Delete();
	func2b.Delete();
}

void Media::ColorProfile::GetConvMatrix(NN<Math::Matrix3> mat, NN<const ColorProfile::ColorPrimaries> srcColor, NN<const ColorProfile::ColorPrimaries> destColor)
{
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Matrix3 mat4;
	Math::Matrix3 mat5;
	Math::Vector3 vec1;
	Math::Vector3 vec2;
	srcColor->GetConvMatrix(mat);
	destColor->GetConvMatrix(mat5);
	mat5.Inverse();

	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(mat2);
	mat3.Set(mat2);
	mat4.SetIdentity();
	vec2 = Media::ColorProfile::ColorPrimaries::xyYToXYZ(Math::Vector3(destColor->w, 1.0));
	vec1 = Media::ColorProfile::ColorPrimaries::xyYToXYZ(Math::Vector3(srcColor->w, 1.0));
	vec1 = mat2.Multiply(vec1);
	vec2 = mat2.Multiply(vec2);
	mat2.Inverse();
	mat4.vec[0].val[0] = vec2.val[0] / vec1.val[0];
	mat4.vec[1].val[1] = vec2.val[1] / vec1.val[1];
	mat4.vec[2].val[2] = vec2.val[2] / vec1.val[2];
	mat2.Multiply(mat4);
	mat2.Multiply(mat3);
	mat->MultiplyBA(mat2);
	mat->MultiplyBA(mat5);
}
