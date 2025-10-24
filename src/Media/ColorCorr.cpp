#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Registry.h"
#include "Math/Math_C.h"
#include "Media/ColorCorr.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Media::ColorCorr::ColorCorr(UTF8Char *name)
{
	Int32 regVal;
	WChar buff[256];
	WChar *wptr;
	radd = 0;
	gadd = 0;
	badd = 0;
	rmul = 1;
	gmul = 1;
	bmul = 1;
	rpow = 1;
	gpow = 1;
	bpow = 1;
	rgamma = 0;
	ggamma = 0;
	bgamma = 0;

	corrTab32 = 0;
	corrTab32Valid = false;
	uncorrTab32 = 0;
	uncorrTab32Valid = false;

	wptr = Text::StrConcat(buff, L"Color\\");
	wptr = Text::StrUTF8_WChar(wptr, name, 0);
	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_ALL, L"SSWR", buff);
	radd = reg->GetValueI32(L"RAdd") * 0.0001;
	gadd = reg->GetValueI32(L"GAdd") * 0.0001;
	badd = reg->GetValueI32(L"BAdd") * 0.0001;
	regVal = reg->GetValueI32(L"RMul");
	if (regVal) rmul = regVal * 0.0001;
	regVal = reg->GetValueI32(L"GMul");
	if (regVal) gmul = regVal * 0.0001;
	regVal = reg->GetValueI32(L"BMul");
	if (regVal) bmul = regVal * 0.0001;
	regVal = reg->GetValueI32(L"RPow");
	if (regVal) rpow = regVal * 0.0001;
	regVal = reg->GetValueI32(L"GPow");
	if (regVal) gpow = regVal * 0.0001;
	regVal = reg->GetValueI32(L"BPow");
	if (regVal) bpow = regVal * 0.0001;
	rgamma = reg->GetValueI32(L"RGamma");
	ggamma = reg->GetValueI32(L"GGamma");
	bgamma = reg->GetValueI32(L"BGamma");
	IO::Registry::CloseRegistry(reg);
}

Media::ColorCorr::~ColorCorr()
{
	if (corrTab32)
	{
		MemFree(corrTab32);
		corrTab32 = 0;
		corrTab32Valid = false;
	}
	if (uncorrTab32)
	{
		MemFree(uncorrTab32);
		uncorrTab32 = 0;
		uncorrTab32Valid = false;
	}
}

Int32 Media::ColorCorr::Save(UTF8Char *name)
{
	WChar buff[256];
	WChar *wptr;
	wptr = Text::StrConcat(buff, L"Color\\");
	wptr = Text::StrUTF8_WChar(wptr, name, 0);
	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_ALL, L"SSWR", buff);
	reg->SetValue(L"RAdd", (Int32)(radd * 10000));
	reg->SetValue(L"GAdd", (Int32)(gadd * 10000));
	reg->SetValue(L"BAdd", (Int32)(badd * 10000));
	reg->SetValue(L"RMul", (Int32)(rmul * 10000));
	reg->SetValue(L"GMul", (Int32)(gmul * 10000));
	reg->SetValue(L"BMul", (Int32)(bmul * 10000));
	reg->SetValue(L"RPow", (Int32)(rpow * 10000));
	reg->SetValue(L"GPow", (Int32)(gpow * 10000));
	reg->SetValue(L"BPow", (Int32)(bpow * 10000));
	reg->SetValue(L"RGamma", rgamma);
	reg->SetValue(L"GGamma", ggamma);
	reg->SetValue(L"BGamma", bgamma);
	IO::Registry::CloseRegistry(reg);
	return 0;
}

Double Media::ColorCorr::GetRAdd()
{
	return radd;
}

Double Media::ColorCorr::GetRMul()
{
	return rmul;
}

Double Media::ColorCorr::GetRPow()
{
	return rpow;
}

Double Media::ColorCorr::GetGAdd()
{
	return gadd;
}

Double Media::ColorCorr::GetGMul()
{
	return gmul;
}

Double Media::ColorCorr::GetGPow()
{
	return gpow;
}

Double Media::ColorCorr::GetBAdd()
{
	return badd;
}

Double Media::ColorCorr::GetBMul()
{
	return bmul;
}

Double Media::ColorCorr::GetBPow()
{
	return bpow;
}

Int32 Media::ColorCorr::GetRGamma()
{
	return rgamma;
}

Int32 Media::ColorCorr::GetGGamma()
{
	return ggamma;
}

Int32 Media::ColorCorr::GetBGamma()
{
	return bgamma;
}

void Media::ColorCorr::SetRVals(Double radd, Double rmul, Double rpow)
{
	this->radd = radd;
	this->rmul = rmul;
	this->rpow = rpow;
	this->corrTab32Valid = false;
	this->uncorrTab32Valid = false;
}

void Media::ColorCorr::SetGVals(Double gadd, Double gmul, Double gpow)
{
	this->gadd = gadd;
	this->gmul = gmul;
	this->gpow = gpow;
	this->corrTab32Valid = false;
	this->uncorrTab32Valid = false;
}

void Media::ColorCorr::SetBVals(Double badd, Double bmul, Double bpow)
{
	this->badd = badd;
	this->bmul = bmul;
	this->bpow = bpow;
	this->corrTab32Valid = false;
	this->uncorrTab32Valid = false;
}

void Media::ColorCorr::SetGammas(Int32 rgamma, Int32 ggamma, Int32 bgamma)
{
	this->rgamma = rgamma;
	this->ggamma = ggamma;
	this->bgamma = bgamma;
	this->corrTab32Valid = false;
	this->uncorrTab32Valid = false;
}

#define pow(a,b) Math_Pow(a, b)
#define iTransfer(val) ((val) <= -cssRGBC1)?(-pow((-(val) + cssRGBK3) / (1 + cssRGBK3), cssRGBC2)):(((val) < cssRGBC1)?((val) / cssRGBK2):(pow(((val) + cssRGBK3) / (1 + cssRGBK3), cssRGBC2)))
#define fTransfer(val) (((val) < -cssRGBK1)?((-1 - cssRGBK3) * pow(-(val), cssRGBK4) + cssRGBK3):(((val) <= cssRGBK1)?(cssRGBK2 * (val)):((1 + cssRGBK3) * pow((val), cssRGBK4) - cssRGBK3)))

void Media::ColorCorr::CorrImage32(const UInt8 *src, OSInt sbpl, UInt32 sgamma, UInt8 *dest, OSInt dbpl, UInt32 width, UInt32 height)
{
	Int32 i;
	if ((Int32)sgamma != this->corrTab32Gamma || !this->corrTab32Valid)
	{
		Double cssRGBK1;
		Double cssRGBK2;
		Double cssRGBK3;
		Double cssRGBK4;
		Double cssRGBC1;
		Double cssRGBC2;
		Double gammaV = (sgamma * 0.0001);
		Double rgammaV = 1 / (rgamma * 0.0001);
		Double ggammaV = 1 / (ggamma * 0.0001);
		Double bgammaV = 1 / (bgamma * 0.0001);

		cssRGBK1 = 0.0031308;
		cssRGBK2 = 12.92;
		cssRGBK3 = 0.055;
		cssRGBK4 = 1.0 / 2.4;
		cssRGBC1 = cssRGBK1 * cssRGBK2;
		cssRGBC2 = 1.0 / cssRGBK4;

		if (this->corrTab32 == 0)
		{
			this->corrTab32 = MemAlloc(UInt8, 3 * 10024);
		}
		UInt8 *ptr = this->corrTab32;
		i = 0;
		while (i < 256)
		{
			Double v1 = i / 255.0;
			Double v2 = (pow((sgamma?pow(v1, gammaV):iTransfer(v1)) + this->radd, this->rpow) * this->rmul);
			Double v3 = (rgamma?pow(v2, rgammaV):fTransfer(v2)) * 255;
			if (v3 < 0) v3 = 0; else if (v3 > 255) v3 = 255;
			ptr[0] = 0;
			ptr[1] = 0;
			ptr[2] = (UInt8)v3;
			ptr[3] = 0xff;
			ptr += 4;
/*			*(Int16*)&ptr[0] = 0;
			*(Int16*)&ptr[2] = 0;
			*(Int16*)&ptr[4] = (Int32)v3;
			*(Int16*)&ptr[6] = 16383;
			ptr += 8;*/
			i++;
		}
		i = 0;
		while (i < 256)
		{
			Double v1 = i / 255.0;
			Double v2 = (pow((sgamma?pow(v1, gammaV):iTransfer(v1)) + this->gadd, this->gpow) * this->gmul);
			Double v3 = (ggamma?pow(v2, ggammaV):fTransfer(v2)) * 255;
			if (v3 < 0) v3 = 0; else if (v3 > 255) v3 = 255;
			ptr[0] = 0;
			ptr[1] = (UInt8)v3;
			ptr[2] = 0;
			ptr[3] = 0;
			ptr += 4;
/*			*(Int16*)&ptr[0] = 0;
			*(Int16*)&ptr[2] = (Int32)v2;
			*(Int16*)&ptr[4] = 0;
			*(Int16*)&ptr[6] = 0;
			ptr += 8;*/
			i++;
		}
		i = 0;
		while (i < 256)
		{
			Double v1 = i / 255.0;
			Double v2 = (pow((sgamma?pow(v1, gammaV):iTransfer(v1)) + this->badd, this->bpow) * this->bmul);
			Double v3 = (bgamma?pow(v2, bgammaV):fTransfer(v2)) * 255;
			if (v3 < 0) v3 = 0; else if (v3 > 255) v3 = 255;
			ptr[0] = (UInt8)v3;
			ptr[1] = 0;
			ptr[2] = 0;
			ptr[3] = 0;
			ptr += 4;
/*			*(Int16*)&ptr[0] = (Int32)v2;
			*(Int16*)&ptr[2] = 0;
			*(Int16*)&ptr[4] = 0;
			*(Int16*)&ptr[6] = 0;
			ptr += 8;*/
			i++;
		}
	}

	UInt8 *tab = corrTab32;
#ifdef HAS_ASM32
	_asm
	{
		mov ebx,tab
		mov esi,src
		mov edi,dest
		mov ecx,height
ci32lop:
		push ecx
		push esi
		push edi

		mov ecx,width
ci32lop2:
		push edi
		movzx eax,byte ptr [esi+2]
		mov edx,dword ptr [ebx+eax*4]
		mov edi,edx

		movzx eax,byte ptr [esi+1]
		mov edx,dword ptr [ebx+eax*4+1024]
		add edi,edx

		movzx eax,byte ptr [esi]
		mov edx,dword ptr [ebx+eax*4+2048]
		add edx,edi
		pop edi
		mov dword ptr [edi],edx

		add edi,4
		add esi,4
		dec ecx
		jnz ci32lop2

		pop edi
		pop esi
		pop ecx
		add esi,sbpl
		add edi,dbpl
		dec ecx
		jnz ci32lop
	}
#else
	Int32 *tmpTab = (Int32*)tab;
	const UInt8 *tmpSrc;
	UInt8 *tmpDest;
	Int32 j;
	i = height;
	while (i-- > 0)
	{
		tmpSrc = src;
		tmpDest = dest;
		j = width;
		while (j-- > 0)
		{
			*(Int32*)tmpDest =  tmpTab[tmpSrc[0] + 512] + tmpTab[tmpSrc[1] + 256] + tmpTab[tmpSrc[2]];
			tmpSrc += 4;
			tmpDest += 4;
		}
		src += sbpl;
		dest += dbpl;
	}
#endif
}

void Media::ColorCorr::UncorrImage32(const UInt8 *src, OSInt sbpl, UInt32 sgamma, UInt8 *dest, OSInt dbpl, UInt32 width, UInt32 height)
{
}
