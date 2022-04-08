#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImageAlphaBlend.h"

Media::ImageAlphaBlend::ImageAlphaBlend() : sProfile(Media::ColorProfile::CPT_SRGB), dProfile(Media::ColorProfile::CPT_SRGB), oProfile(Media::ColorProfile::CPT_SRGB)
{
	this->changed = true;
}

Media::ImageAlphaBlend::~ImageAlphaBlend()
{
}

void Media::ImageAlphaBlend::SetSourceProfile(const Media::ColorProfile *sProfile)
{
	if (!this->sProfile.Equals(sProfile))
	{
		this->sProfile.Set(sProfile);
		this->changed = true;
	}
}

void Media::ImageAlphaBlend::SetDestProfile(const Media::ColorProfile *dProfile)
{
	if (!this->dProfile.Equals(dProfile))
	{
		this->dProfile.Set(dProfile);
		this->changed = true;
	}
}

void Media::ImageAlphaBlend::SetOutputProfile(const Media::ColorProfile *oProfile)
{
	if (!this->oProfile.Equals(oProfile))
	{
		this->oProfile.Set(oProfile);
		this->changed = true;
	}
}

/*void Media::ImageAlphaBlend::Add(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height)
{
#ifdef HAS_ASM32
	_asm
	{
		mov eax,0
		
		movd mm2,eax

		mov edi,dest
		mov esi,src
		mov edx,height
iabaddlop:
		push esi
		push edi

		mov ecx,width
iadaddlop2:
		mov al,byte ptr [esi+3]
		mov ah,255
		shl eax,16
		mov al,byte ptr [esi+3]
		mov ah,al

		movd mm0,eax
		movd mm1,dword ptr [esi]
		punpcklbw mm0, mm2
		punpcklbw mm1, mm2
		pmullw mm0,mm1
		movq mm1,mm0
		psrlw mm1,8
		paddusw mm0,mm1
		movq mm1,mm2
		punpcklbw mm1, [edi]
		paddusw mm0,mm1
		psrlw mm0,8
		packuswb mm0,mm2
		movd dword ptr [edi],mm0

		add esi,4
		add edi,4
		dec ecx
		jnz iadaddlop2

		pop edi
		pop esi
		add edi,dbpl
		add esi,sbpl
		dec edx
		jnz iabaddlop
	
		emms
	}
#else
	__m64 valMM2 = _mm_cvtsi32_si64(0);
	__m64 valMM0;
	__m64 valMM1;
	Int32 i;
	Int32 tmpVA;
	while (height-- > 0)
	{
		i = 0;
		while (i < width)
		{
			tmpVA = src[(i << 2) + 3];
			tmpVA = (tmpVA << 8) | tmpVA;
			tmpVA = (tmpVA << 16) | tmpVA | 0xff000000;

			valMM0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(tmpVA), valMM2);
			valMM1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(((Int32*)src)[i]), valMM2);
			valMM1 = _mm_mullo_pi16(valMM0, valMM1);
			valMM0 = valMM1;
			valMM1 = _mm_srli_pi16(valMM1, 8);
			valMM0 = _mm_adds_pu16(valMM0, valMM1);
			valMM0 = _mm_adds_pu16(valMM0, _mm_unpacklo_pi8(valMM2, _mm_cvtsi32_si64(((Int32*)dest)[i])));
			valMM0 = _mm_srli_pi16(valMM0, 8);
			((Int32*)dest)[i] = _mm_cvtsi64_si32(_mm_packs_pu16(valMM0, valMM2));

			i++;
		}
		src += sbpl;
		dest += dbpl;
	}

	_mm_empty();
#endif
}

void Media::ImageAlphaBlend::Subtract(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height)
{
#ifdef HAS_ASM32
	_asm
	{
		mov eax,0
		
		movd mm2,eax

		mov edi,dest
		mov esi,src
		mov edx,height
iabsublop:
		push esi
		push edi

		mov ecx,width
iadsublop2:
		mov al,byte ptr [esi+3]
		mov ah,255
		shl eax,16
		mov al,byte ptr [esi+3]
		mov ah,al

		movd mm0,eax
		movd mm1,dword ptr [esi]
		punpcklbw mm0, mm2
		punpcklbw mm1, mm2
		pmullw mm0,mm1
		movq mm1,mm0
		psrlw mm1,8
		paddusw mm0,mm1
		movq mm1,mm2
		punpcklbw mm1, [edi]
		psubusw mm0,mm1
		psrlw mm0,8
		packuswb mm0,mm2
		movd dword ptr [edi],mm0

		add esi,4
		add edi,4
		dec ecx
		jnz iadsublop2

		pop edi
		pop esi
		add edi,dbpl
		add esi,sbpl
		dec edx
		jnz iabsublop
	
		emms
	}
#else
	__m64 valMM2 = _mm_cvtsi32_si64(0);
	__m64 valMM0;
	__m64 valMM1;
	Int32 i;
	Int32 tmpVA;
	while (height-- > 0)
	{
		i = 0;
		while (i < width)
		{
			tmpVA = src[(i << 2) + 3];
			tmpVA = (tmpVA << 8) | tmpVA;
			tmpVA = (tmpVA << 16) | tmpVA | 0xff000000;

			valMM0 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(tmpVA), valMM2);
			valMM1 = _mm_unpacklo_pi8(_mm_cvtsi32_si64(((Int32*)src)[i]), valMM2);
			valMM1 = _mm_mullo_pi16(valMM0, valMM1);
			valMM0 = valMM1;
			valMM1 = _mm_srli_pi16(valMM1, 8);
			valMM0 = _mm_adds_pu16(valMM0, valMM1);
			valMM0 = _mm_subs_pu16(valMM0, _mm_unpacklo_pi8(valMM2, _mm_cvtsi32_si64(((Int32*)dest)[i])));
			valMM0 = _mm_srli_pi16(valMM0, 8);
			((Int32*)dest)[i] = _mm_cvtsi64_si32(_mm_packs_pu16(valMM0, valMM2));

			i++;
		}
		src += sbpl;
		dest += dbpl;
	}

	_mm_empty();
#endif
}*/
