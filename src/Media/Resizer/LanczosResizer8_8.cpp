#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/LanczosFilter.h"
#include "Math/Math_C.h"
#include "Media/ImageResizer.h"
#include "Media/Resizer/LanczosResizer8_8.h"
#include "Sync/Event.h"
#include "Sync/ThreadUtil.h"
#ifndef HAS_ASM32
#include <mmintrin.h>
#endif

void Media::Resizer::LanczosResizer8_8::setup_interpolation_parameter(Double source_length, Int32 source_max_pos, Int32 result_length, PARAMETER *out, Int32 indexSep, Double offsetCorr)
{
	Int32 i,j,n;
	Double *work;
	Double  sum;
	Double  pos;
	
	out->length = result_length;
	out->tap = this->nTap;
	out->weight = MemAlloc(Int32, out->length * out->tap);
	out->index = MemAlloc(Int32, out->length * out->tap);

	work = MemAlloc(Double, out->tap);

	for(i=0;i<result_length;i++){
		pos = (i+0.5)*source_length;
		pos = pos / result_length + offsetCorr;
		n = (Int32)floor(pos - (this->nTap / 2 - 0.5));//2.5);
		pos = (n+0.5-pos);
		sum = 0;
		for(j=0;j<out->tap;j++){
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if(n >= source_max_pos){
				out->index[i * out->tap + j] = (source_max_pos - 1) * indexSep;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = lanczos3_weight(pos);
			sum += work[j];
			pos += 1;
			n += 1;
		}

		for(j=0;j<out->tap;j++){
			out->weight[i * out->tap+j] = (Int32)((work[j] / sum) * (1<<16));
		}
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizer8_8::setup_decimation_parameter(Double source_length, Int32 source_max_pos, Int32 result_length, PARAMETER *out, Int32 indexSep, Double offsetCorr)
{
	Int32 i,j,n;
	Double *work;
	Double  sum;
	Double  pos, phase;

	out->length = result_length;
	out->tap = Double2Int32((this->nTap * (source_length) + (result_length - 1)) / result_length);

	out->weight = MemAlloc(Int32, out->length * out->tap);
	out->index = MemAlloc(Int32, out->length * out->tap);
	
	work = MemAlloc(Double, out->tap);

	for(i=0;i<result_length;i++){
		pos = (i - (this->nTap / 2) + 0.5) * source_length / result_length + 0.5;
		n = (Int32)floor(pos + offsetCorr);
		sum = 0;
		for(j=0;j<out->tap;j++){
			phase = (n+0.5)*result_length;
			phase /= source_length;
			phase -= (i+0.5);
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if(n >= source_max_pos){
				out->index[i * out->tap + j] = (source_max_pos-1) * indexSep;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = lanczos3_weight(phase);
			sum += work[j];
			n += 1;
		}

		for(j=0;j<out->tap;j++){
			out->weight[i * out->tap+j] = (int)((work[j] / sum) * (1<<16));
		}
	}

	MemFree(work);
}

/*-----------------------------------------------------------------*/
void Media::Resizer::LanczosResizer8_8::horizontal_filter(UInt8 *inPt, UInt8 *outPt,Int32 width, Int32 height, Int32 tap, Int32 *index, Int32 *weight, UInt32 sstep, UInt32 dstep)
{
	if (width & 3)
	{
#ifdef HAS_ASM32
		Int64 bg;
		Int64 ra;
		Int32 rTmp;
		Int32 gTmp;
		Int32 bTmp;
		_asm
		{
			mov esi,outPt
			mov ecx,height
	hflop:
			push esi
			push ecx
			mov ecx,width

			mov ebx,index
			mov edi,weight
	hflop2:
			push ecx
			push esi
			mov dword ptr bg[0],0
			mov dword ptr bg[4],0
			mov dword ptr ra[0],0

			mov ecx,tap
	hflop3:
			push ecx
			mov esi,inPt
			mov eax,dword ptr [ebx]
			movzx edx,byte ptr [esi+eax+2]
			mov rTmp,edx
			movzx edx,byte ptr [esi+eax+1]
			mov gTmp,edx
			movzx edx,byte ptr [esi+eax+0]
			mov bTmp,edx
			add ebx,4
			mov ecx,dword ptr [edi]
			mov eax,rTmp
			imul ecx
			add dword ptr ra[0],eax
			mov eax,gTmp
			imul ecx
			add dword ptr bg[4],eax
			mov eax,bTmp
			imul ecx
			add dword ptr bg[0],eax
			add edi,4
			pop ecx
			dec ecx
			jnz hflop3

			pop esi
			sar dword ptr bg[0],16
			sar dword ptr bg[4],16
			sar dword ptr ra[0],16
			movq mm0,bg
			packssdw mm0,ra
			packuswb mm0,ra
			movq bg,mm0
			mov eax,dword ptr bg[0]
			mov dword ptr [esi],eax
			add esi,4
			
			pop ecx
			dec ecx
			jnz hflop2
			pop ecx

			pop esi
			add esi,dstep

			mov edx,inPt
			add edx,sstep
			mov inPt,edx

			dec ecx
			jnz hflop

			emms
		}
#else
		Int64 bg;
		Int64 ra;
		Int32 aTmp;
		Int32 rTmp;
		Int32 gTmp;
		Int32 bTmp;
		UInt8 *tmpPt;
		Int32 *tmpWeight;
		Int32 *tmpIndex;
		OSInt i;
		Int32 j;
		while (height-- > 0)
		{
			tmpPt = outPt;

			tmpWeight = weight;
			tmpIndex = index;
			i = width;
			while (i-- > 0)
			{
				bg = 0;
				ra = 0;

				j = tap;
				while (j-- > 0)
				{
					aTmp = inPt[3 + *tmpIndex];
					rTmp = inPt[2 + *tmpIndex];
					gTmp = inPt[1 + *tmpIndex];
					bTmp = inPt[0 + *tmpIndex];
					tmpIndex++;
					((Int32*)&ra)[1] += aTmp * tmpWeight[0];
					((Int32*)&ra)[0] += rTmp * tmpWeight[0];
					((Int32*)&bg)[1] += gTmp * tmpWeight[0];
					((Int32*)&bg)[0] += bTmp * tmpWeight[0];
					tmpWeight++;
				}

				*(Int32*)tmpPt = _mm_cvtsi64_si32(_mm_packs_pu16(_mm_packs_pi32(_mm_srai_pi32(*(__m64*)&bg, 16), _mm_srai_pi32(*(__m64*)&ra, 16)), _mm_cvtsi32_si64(0)));
				tmpPt += 4;
			}

			outPt += dstep;
			inPt += sstep;
		}
		_mm_empty();
#endif
	}
	else
	{
#ifdef HAS_ASM32
		Int64 bg;
		Int64 ra;
		Int64 rgbv1;
		Int64 rgbv2;
		Int64 rgbv3;
		Int64 rgbv4;
		Int32 rTmp;
		Int32 gTmp;
		Int32 bTmp;
		_asm
		{
			mov esi,outPt
			mov ecx,height
hflop4:
			push esi
			push ecx
			mov ecx,width
			shr ecx,2

			mov ebx,index
			mov edi,weight
hflop5:
			push ecx
			push esi
			mov dword ptr bg[0],0
			mov dword ptr bg[4],0
			mov dword ptr ra[0],0

			mov ecx,tap
hflop6:
			push ecx
			mov esi,inPt
			mov eax,dword ptr [ebx]
			movzx edx,byte ptr [esi+eax+2]
			mov rTmp,edx
			movzx edx,byte ptr [esi+eax+1]
			mov gTmp,edx
			movzx edx,byte ptr [esi+eax+0]
			mov bTmp,edx
			add ebx,4
			mov ecx,dword ptr [edi]
			mov eax,rTmp
			imul ecx
			add dword ptr ra[0],eax
			//mov ecx,dword ptr [edi]
			mov eax,gTmp
			imul ecx
			add dword ptr bg[4],eax
			//mov ecx,dword ptr [edi]
			mov eax,bTmp
			imul ecx
			add dword ptr bg[0],eax
			add edi,4
			pop ecx
			dec ecx
			jnz hflop6

			sar dword ptr bg[0],16
			sar dword ptr bg[4],16
			sar dword ptr ra[0],16
			movq mm0,bg
			packssdw mm0,ra
			movq rgbv1,mm0

			mov dword ptr bg[0],0
			mov dword ptr bg[4],0
			mov dword ptr ra[0],0
			mov ecx,tap
hflop7:
			push ecx
			mov esi,inPt
			mov eax,dword ptr [ebx]
			movzx edx,byte ptr [esi+eax+2]
			mov rTmp,edx
			movzx edx,byte ptr [esi+eax+1]
			mov gTmp,edx
			movzx edx,byte ptr [esi+eax+0]
			mov bTmp,edx
			add ebx,4
			mov ecx,dword ptr [edi]
			mov eax,rTmp
			imul ecx
			add dword ptr ra[0],eax
			//mov ecx,dword ptr [edi]
			mov eax,gTmp
			imul ecx
			add dword ptr bg[4],eax
			//mov ecx,dword ptr [edi]
			mov eax,bTmp
			imul ecx
			add dword ptr bg[0],eax
			add edi,4
			pop ecx
			dec ecx
			jnz hflop7

			sar dword ptr bg[0],16
			sar dword ptr bg[4],16
			sar dword ptr ra[0],16
			movq mm0,bg
			packssdw mm0,ra
			movq rgbv2,mm0

			mov dword ptr bg[0],0
			mov dword ptr bg[4],0
			mov dword ptr ra[0],0
			mov ecx,tap
hflop8:
			push ecx
			mov esi,inPt
			mov eax,dword ptr [ebx]
			movzx edx,byte ptr [esi+eax+2]
			mov rTmp,edx
			movzx edx,byte ptr [esi+eax+1]
			mov gTmp,edx
			movzx edx,byte ptr [esi+eax+0]
			mov bTmp,edx
			add ebx,4
			mov ecx,dword ptr [edi]
			mov eax,rTmp
			imul ecx
			add dword ptr ra[0],eax
			//mov ecx,dword ptr [edi]
			mov eax,gTmp
			imul ecx
			add dword ptr bg[4],eax
			//mov ecx,dword ptr [edi]
			mov eax,bTmp
			imul ecx
			add dword ptr bg[0],eax
			add edi,4
			pop ecx
			dec ecx
			jnz hflop8

			sar dword ptr bg[0],16
			sar dword ptr bg[4],16
			sar dword ptr ra[0],16
			movq mm0,bg
			packssdw mm0,ra
			movq rgbv3,mm0

			mov dword ptr bg[0],0
			mov dword ptr bg[4],0
			mov dword ptr ra[0],0
			mov ecx,tap
hflop9:
			push ecx
			mov esi,inPt
			mov eax,dword ptr [ebx]
			movzx edx,byte ptr [esi+eax+2]
			mov rTmp,edx
			movzx edx,byte ptr [esi+eax+1]
			mov gTmp,edx
			movzx edx,byte ptr [esi+eax+0]
			mov bTmp,edx
			add ebx,4
			mov ecx,dword ptr [edi]
			mov eax,rTmp
			imul ecx
			add dword ptr ra[0],eax
			//mov ecx,dword ptr [edi]
			mov eax,gTmp
			imul ecx
			add dword ptr bg[4],eax
			//mov ecx,dword ptr [edi]
			mov eax,bTmp
			imul ecx
			add dword ptr bg[0],eax
			add edi,4
			pop ecx
			dec ecx
			jnz hflop9

			sar dword ptr bg[0],16
			sar dword ptr bg[4],16
			sar dword ptr ra[0],16
			movq mm0,bg
			packssdw mm0,ra
			movq rgbv4,mm0

			movq mm0,rgbv1
			movq mm1,rgbv3
			packuswb mm0,rgbv2
			packuswb mm1,rgbv4
		
			pop esi
			movq mmword ptr [esi],mm0
			movq mmword ptr [esi+8],mm1
			add esi,16
			
			pop ecx
			dec ecx
			jnz hflop5
			pop ecx

			pop esi
			add esi,dstep

			mov edx,inPt
			add edx,sstep
			mov inPt,edx

			dec ecx
			jnz hflop4

			emms
		}
#else
		Int64 bg;
		Int64 ra;
		__m64 rgbv1;
		__m64 rgbv2;
		__m64 rgbv3;
		__m64 rgbv4;
		Int32 aTmp;
		Int32 rTmp;
		Int32 gTmp;
		Int32 bTmp;
		UInt8 *tmpPt;
		Int32 *tmpWeight;
		Int32 *tmpIndex;
		OSInt i;
		Int32 j;

		while (height-- > 0)
		{
			tmpPt = outPt;

			tmpWeight = weight;
			tmpIndex = index;
			i = width >> 2;
			while (i-- > 0)
			{
				bg = 0;
				ra = 0;

				j = tap;
				while (j-- > 0)
				{
					aTmp = inPt[3 + *tmpIndex];
					rTmp = inPt[2 + *tmpIndex];
					gTmp = inPt[1 + *tmpIndex];
					bTmp = inPt[0 + *tmpIndex];
					tmpIndex++;
					((Int32*)&ra)[1] += aTmp * tmpWeight[0];
					((Int32*)&ra)[0] += rTmp * tmpWeight[0];
					((Int32*)&bg)[1] += gTmp * tmpWeight[0];
					((Int32*)&bg)[0] += bTmp * tmpWeight[0];
					tmpWeight++;
				}
				rgbv1 = _mm_packs_pi32(_mm_srai_pi32(*(__m64*)&bg, 16), _mm_srai_pi32(*(__m64*)&ra, 16));

				bg = 0;
				ra = 0;

				j = tap;
				while (j-- > 0)
				{
					aTmp = inPt[3 + *tmpIndex];
					rTmp = inPt[2 + *tmpIndex];
					gTmp = inPt[1 + *tmpIndex];
					bTmp = inPt[0 + *tmpIndex];
					tmpIndex++;
					((Int32*)&ra)[1] += aTmp * tmpWeight[0];
					((Int32*)&ra)[0] += rTmp * tmpWeight[0];
					((Int32*)&bg)[1] += gTmp * tmpWeight[0];
					((Int32*)&bg)[0] += bTmp * tmpWeight[0];
					tmpWeight++;
				}
				rgbv2 = _mm_packs_pi32(_mm_srai_pi32(*(__m64*)&bg, 16), _mm_srai_pi32(*(__m64*)&ra, 16));

				bg = 0;
				ra = 0;

				j = tap;
				while (j-- > 0)
				{
					aTmp = inPt[3 + *tmpIndex];
					rTmp = inPt[2 + *tmpIndex];
					gTmp = inPt[1 + *tmpIndex];
					bTmp = inPt[0 + *tmpIndex];
					tmpIndex++;
					((Int32*)&ra)[1] += aTmp * tmpWeight[0];
					((Int32*)&ra)[0] += rTmp * tmpWeight[0];
					((Int32*)&bg)[1] += gTmp * tmpWeight[0];
					((Int32*)&bg)[0] += bTmp * tmpWeight[0];
					tmpWeight++;
				}
				rgbv3 = _mm_packs_pi32(_mm_srai_pi32(*(__m64*)&bg, 16), _mm_srai_pi32(*(__m64*)&ra, 16));

				bg = 0;
				ra = 0;

				j = tap;
				while (j-- > 0)
				{
					aTmp = inPt[3 + *tmpIndex];
					rTmp = inPt[2 + *tmpIndex];
					gTmp = inPt[1 + *tmpIndex];
					bTmp = inPt[0 + *tmpIndex];
					tmpIndex++;
					((Int32*)&ra)[1] += aTmp * tmpWeight[0];
					((Int32*)&ra)[0] += rTmp * tmpWeight[0];
					((Int32*)&bg)[1] += gTmp * tmpWeight[0];
					((Int32*)&bg)[0] += bTmp * tmpWeight[0];
					tmpWeight++;
				}
				rgbv4 = _mm_packs_pi32(_mm_srai_pi32(*(__m64*)&bg, 16), _mm_srai_pi32(*(__m64*)&ra, 16));

				*(__m64*)&tmpPt[0] = _mm_packs_pu16(rgbv1, rgbv2);
				*(__m64*)&tmpPt[8] = _mm_packs_pu16(rgbv3, rgbv4);
				tmpPt += 16;
			}

			outPt += dstep;
			inPt += sstep;
		}

		_mm_empty();
#endif
	}
}

/*-----------------------------------------------------------------*/
void Media::Resizer::LanczosResizer8_8::vertical_filter(UInt8 *inPt, UInt8 *outPt, Int32 width, Int32 height, Int32 tap, Int32 *index, Int32 *weight, UInt32 sstep, UInt32 dstep)
{
/*	int i,j,k;
	unsigned long l;

	int r, g, b;

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			r = 0;
			g = 0;
			b = 0;
			for(k = 0; k < tap; k++)
			{
				r += ((long) inPt[(l = (index[i*tap+k] * sstep + (j << 2))) + 2]) * weight[i*tap+k];
				g += ((long) inPt[l + 1]) * weight[i*tap+k];
				b += ((long) inPt[l]) * weight[i*tap+k];
			}
			outPt[(l = ((i * dstep) + (j << 2))) + 2] = (unsigned char)(r >> 16);
			outPt[l + 1] = (unsigned char)(g >> 16);
			outPt[l] = (unsigned char)(b >> 16);
		}
	}*/
	if (width & 3)
	{
#ifdef HAS_ASM32
		Int64 bg;
		Int64 ra;
		Int32 rTmp;
		Int32 gTmp;
		Int32 bTmp;
		UInt8* tmpPt = inPt;
		_asm
		{
			mov esi,outPt
			mov ecx,height
	vflop:
			push esi
			push ecx
			mov ecx,width

	vflop2:
			push ecx
			push esi
			mov dword ptr bg[0],0
			mov dword ptr bg[4],0
			mov dword ptr ra[0],0

			mov ebx,index
			mov edi,weight
			mov ecx,tap
	vflop3:
			push ecx

			mov esi,tmpPt
			mov eax,dword ptr [ebx]
			//mul dword ptr sstep
			movzx edx,byte ptr [esi+eax+2]
			mov rTmp,edx
			movzx edx,byte ptr [esi+eax+1]
			mov gTmp,edx
			movzx edx,byte ptr [esi+eax+0]
			mov bTmp,edx

			add ebx,4

			mov ecx,dword ptr [edi]
			mov eax,rTmp
			imul ecx
			add dword ptr ra[0],eax
			mov ecx,dword ptr [edi]
			mov eax,gTmp
			imul ecx
			add dword ptr bg[4],eax
			mov ecx,dword ptr [edi]
			mov eax,bTmp
			imul ecx
			add dword ptr bg[0],eax

			add edi,4
			pop ecx
			dec ecx
			jnz vflop3


			pop esi
			sar dword ptr bg[0],16
			sar dword ptr bg[4],16
			sar dword ptr ra[0],16
			movq mm0,bg
			packssdw mm0,ra
			packuswb mm0,ra
			movq bg,mm0
			mov eax,dword ptr bg[0]
			mov dword ptr [esi],eax
			add esi,4
			add tmpPt,4
			
			pop ecx
			dec ecx
			jnz vflop2
			pop ecx

			mov esi,inPt
			mov tmpPt,esi

			mov esi,tap
			shl esi,2
			add index,esi
			add weight,esi

			pop esi
			add esi,dstep

			dec ecx
			jnz vflop

			emms
		}
#else
		Int64 bg;
		Int64 ra;
		Int32 aTmp;
		Int32 rTmp;
		Int32 gTmp;
		Int32 bTmp;
		UInt8 *tmpIn = inPt;
		UInt8 *tmpOut;
		Int32 *tmpWeight;
		Int32 *tmpIndex;
		OSInt i;
		Int32 j;
		while (height-- > 0)
		{
			tmpOut = outPt;

			i = width;
			while (i-- > 0)
			{
				bg = 0;
				ra = 0;

				tmpWeight = weight;
				tmpIndex = index;
				j = tap;
				while (j-- > 0)
				{
					aTmp = tmpIn[3 + *tmpIndex];
					rTmp = tmpIn[2 + *tmpIndex];
					gTmp = tmpIn[1 + *tmpIndex];
					bTmp = tmpIn[0 + *tmpIndex];
					tmpIndex++;
					((Int32*)&ra)[1] += aTmp * tmpWeight[0];
					((Int32*)&ra)[0] += rTmp * tmpWeight[0];
					((Int32*)&bg)[1] += gTmp * tmpWeight[0];
					((Int32*)&bg)[0] += bTmp * tmpWeight[0];
					tmpWeight++;
				}

				*(Int32*)tmpOut = _mm_cvtsi64_si32(_mm_packs_pu16(_mm_packs_pi32(_mm_srai_pi32(*(__m64*)&bg, 16), _mm_srai_pi32(*(__m64*)&ra, 16)), _mm_cvtsi32_si64(0)));
				tmpOut += 4;
				tmpIn += 4;
			}
			tmpIn = inPt;
			index += tap;
			weight += tap;
			outPt += dstep;
		}
		_mm_empty();
#endif
	}
	else
	{
#ifdef HAS_ASM32
		Int64 bg;
		Int64 ra;
		Int64 rgbv1;
		Int64 rgbv2;
		Int64 rgbv3;
		Int64 rgbv4;
		Int32 rTmp;
		Int32 gTmp;
		Int32 bTmp;
		UInt8* tmpPt = inPt;
		_asm
		{
			mov esi,outPt
			mov ecx,height
	vflop4:
			push esi
			push ecx
			mov ecx,width
			shr ecx,2

	vflop5:
			push ecx
			push esi
			mov dword ptr bg[0],0
			mov dword ptr bg[4],0
			mov dword ptr ra[0],0

			mov ebx,index
			mov edi,weight
			mov ecx,tap
	vflop6:
			push ecx

			mov esi,tmpPt
			mov eax,dword ptr [ebx]
			//mul dword ptr sstep
			movzx edx,byte ptr [esi+eax+2]
			mov rTmp,edx
			movzx edx,byte ptr [esi+eax+1]
			mov gTmp,edx
			movzx edx,byte ptr [esi+eax+0]
			mov bTmp,edx
//pmullw?
			add ebx,4

			mov ecx,dword ptr [edi]
			mov eax,rTmp
			imul ecx
			add dword ptr ra[0],eax
		//	mov ecx,dword ptr [edi]
			mov eax,gTmp
			imul ecx
			add dword ptr bg[4],eax
		//	mov ecx,dword ptr [edi]
			mov eax,bTmp
			imul ecx
			add dword ptr bg[0],eax

			add edi,4
			pop ecx
			dec ecx
			jnz vflop6


			sar dword ptr bg[0],16
			sar dword ptr bg[4],16
			sar dword ptr ra[0],16
			movq mm0,bg
			packssdw mm0,ra
			movq rgbv1,mm0
			add tmpPt,4

			mov dword ptr bg[0],0
			mov dword ptr bg[4],0
			mov dword ptr ra[0],0

			mov ebx,index
			mov edi,weight
			mov ecx,tap
	vflop7:
			push ecx

			mov esi,tmpPt
			mov eax,dword ptr [ebx]
			//mul dword ptr sstep
			movzx edx,byte ptr [esi+eax+2]
			mov rTmp,edx
			movzx edx,byte ptr [esi+eax+1]
			mov gTmp,edx
			movzx edx,byte ptr [esi+eax+0]
			mov bTmp,edx

			add ebx,4

			mov ecx,dword ptr [edi]
			mov eax,rTmp
			imul ecx
			add dword ptr ra[0],eax
		//	mov ecx,dword ptr [edi]
			mov eax,gTmp
			imul ecx
			add dword ptr bg[4],eax
		//	mov ecx,dword ptr [edi]
			mov eax,bTmp
			imul ecx
			add dword ptr bg[0],eax

			add edi,4
			pop ecx
			dec ecx
			jnz vflop7


			sar dword ptr bg[0],16
			sar dword ptr bg[4],16
			sar dword ptr ra[0],16
			movq mm0,bg
			packssdw mm0,ra
			movq rgbv2,mm0
			add tmpPt,4

			mov dword ptr bg[0],0
			mov dword ptr bg[4],0
			mov dword ptr ra[0],0

			mov ebx,index
			mov edi,weight
			mov ecx,tap
	vflop8:
			push ecx

			mov esi,tmpPt
			mov eax,dword ptr [ebx]
			//mul dword ptr sstep
			movzx edx,byte ptr [esi+eax+2]
			mov rTmp,edx
			movzx edx,byte ptr [esi+eax+1]
			mov gTmp,edx
			movzx edx,byte ptr [esi+eax+0]
			mov bTmp,edx

			add ebx,4

			mov ecx,dword ptr [edi]
			mov eax,rTmp
			imul ecx
			add dword ptr ra[0],eax
		//	mov ecx,dword ptr [edi]
			mov eax,gTmp
			imul ecx
			add dword ptr bg[4],eax
		//	mov ecx,dword ptr [edi]
			mov eax,bTmp
			imul ecx
			add dword ptr bg[0],eax

			add edi,4
			pop ecx
			dec ecx
			jnz vflop8


			sar dword ptr bg[0],16
			sar dword ptr bg[4],16
			sar dword ptr ra[0],16
			movq mm0,bg
			packssdw mm0,ra
			movq rgbv3,mm0
			add tmpPt,4

			mov dword ptr bg[0],0
			mov dword ptr bg[4],0
			mov dword ptr ra[0],0

			mov ebx,index
			mov edi,weight
			mov ecx,tap
	vflop9:
			push ecx

			mov esi,tmpPt
			mov eax,dword ptr [ebx]
			//mul dword ptr sstep
			movzx edx,byte ptr [esi+eax+2]
			mov rTmp,edx
			movzx edx,byte ptr [esi+eax+1]
			mov gTmp,edx
			movzx edx,byte ptr [esi+eax+0]
			mov bTmp,edx

			add ebx,4

			mov ecx,dword ptr [edi]
			mov eax,rTmp
			imul ecx
			add dword ptr ra[0],eax
		//	mov ecx,dword ptr [edi]
			mov eax,gTmp
			imul ecx
			add dword ptr bg[4],eax
		//	mov ecx,dword ptr [edi]
			mov eax,bTmp
			imul ecx
			add dword ptr bg[0],eax

			add edi,4
			pop ecx
			dec ecx
			jnz vflop9


			sar dword ptr bg[0],16
			sar dword ptr bg[4],16
			sar dword ptr ra[0],16
			movq mm0,bg
			packssdw mm0,ra
			movq rgbv4,mm0
			add tmpPt,4

			pop esi
			movq mm0,rgbv1
			movq mm1,rgbv3
			packuswb mm0,rgbv2
			packuswb mm1,rgbv4
			movq mmword ptr [esi],mm0
			movq mmword ptr [esi+8],mm1
			add esi,16
			
			pop ecx
			dec ecx
			jnz vflop5
			pop ecx

			mov esi,inPt
			mov tmpPt,esi

			mov esi,tap
			shl esi,2
			add index,esi
			add weight,esi

			pop esi
			add esi,dstep

			dec ecx
			jnz vflop4

			emms
		}
#else
		Int64 bg;
		Int64 ra;
		__m64 rgbv1;
		__m64 rgbv2;
		__m64 rgbv3;
		__m64 rgbv4;
		Int32 aTmp;
		Int32 rTmp;
		Int32 gTmp;
		Int32 bTmp;
		UInt8 *tmpIn = inPt;
		UInt8 *tmpOut;
		Int32 *tmpWeight;
		Int32 *tmpIndex;
		OSInt i;
		Int32 j;
		while (height-- > 0)
		{
			tmpOut = outPt;

			i = width;
			while (i-- > 0)
			{
				bg = 0;
				ra = 0;

				tmpWeight = weight;
				tmpIndex = index;
				j = tap;
				while (j-- > 0)
				{
					aTmp = tmpIn[3 + *tmpIndex];
					rTmp = tmpIn[2 + *tmpIndex];
					gTmp = tmpIn[1 + *tmpIndex];
					bTmp = tmpIn[0 + *tmpIndex];
					tmpIndex++;
					((Int32*)&ra)[1] += aTmp * tmpWeight[0];
					((Int32*)&ra)[0] += rTmp * tmpWeight[0];
					((Int32*)&bg)[1] += gTmp * tmpWeight[0];
					((Int32*)&bg)[0] += bTmp * tmpWeight[0];
					tmpWeight++;
				}
				tmpIn += 4;
				rgbv1 = _mm_packs_pi32(_mm_srai_pi32(*(__m64*)&bg, 16), _mm_srai_pi32(*(__m64*)&ra, 16));

				bg = 0;
				ra = 0;

				tmpWeight = weight;
				tmpIndex = index;
				j = tap;
				while (j-- > 0)
				{
					aTmp = tmpIn[3 + *tmpIndex];
					rTmp = tmpIn[2 + *tmpIndex];
					gTmp = tmpIn[1 + *tmpIndex];
					bTmp = tmpIn[0 + *tmpIndex];
					tmpIndex++;
					((Int32*)&ra)[1] += aTmp * tmpWeight[0];
					((Int32*)&ra)[0] += rTmp * tmpWeight[0];
					((Int32*)&bg)[1] += gTmp * tmpWeight[0];
					((Int32*)&bg)[0] += bTmp * tmpWeight[0];
					tmpWeight++;
				}
				tmpIn += 4;
				rgbv2 = _mm_packs_pi32(_mm_srai_pi32(*(__m64*)&bg, 16), _mm_srai_pi32(*(__m64*)&ra, 16));

				bg = 0;
				ra = 0;

				tmpWeight = weight;
				tmpIndex = index;
				j = tap;
				while (j-- > 0)
				{
					aTmp = tmpIn[3 + *tmpIndex];
					rTmp = tmpIn[2 + *tmpIndex];
					gTmp = tmpIn[1 + *tmpIndex];
					bTmp = tmpIn[0 + *tmpIndex];
					tmpIndex++;
					((Int32*)&ra)[1] += aTmp * tmpWeight[0];
					((Int32*)&ra)[0] += rTmp * tmpWeight[0];
					((Int32*)&bg)[1] += gTmp * tmpWeight[0];
					((Int32*)&bg)[0] += bTmp * tmpWeight[0];
					tmpWeight++;
				}
				tmpIn += 4;
				rgbv3 = _mm_packs_pi32(_mm_srai_pi32(*(__m64*)&bg, 16), _mm_srai_pi32(*(__m64*)&ra, 16));

				bg = 0;
				ra = 0;

				tmpWeight = weight;
				tmpIndex = index;
				j = tap;
				while (j-- > 0)
				{
					aTmp = tmpIn[3 + *tmpIndex];
					rTmp = tmpIn[2 + *tmpIndex];
					gTmp = tmpIn[1 + *tmpIndex];
					bTmp = tmpIn[0 + *tmpIndex];
					tmpIndex++;
					((Int32*)&ra)[1] += aTmp * tmpWeight[0];
					((Int32*)&ra)[0] += rTmp * tmpWeight[0];
					((Int32*)&bg)[1] += gTmp * tmpWeight[0];
					((Int32*)&bg)[0] += bTmp * tmpWeight[0];
					tmpWeight++;
				}
				tmpIn += 4;
				rgbv4 = _mm_packs_pi32(_mm_srai_pi32(*(__m64*)&bg, 16), _mm_srai_pi32(*(__m64*)&ra, 16));

				*(__m64*)&tmpOut[0] = _mm_packs_pu16(rgbv1, rgbv2);
				*(__m64*)&tmpOut[8] = _mm_packs_pu16(rgbv3, rgbv4);
				tmpOut += 16;
			}
			tmpIn = inPt;
			index += tap;
			weight += tap;
			outPt += dstep;
		}
		_mm_empty();
#endif
	}
}

void Media::Resizer::LanczosResizer8_8::mt_horizontal_filter(UInt8 *inPt, UInt8 *outPt,Int32 width, Int32 height, Int32 tap, Int32 *index, Int32 *weight, UInt32 sstep, UInt32 dstep)
{
	Int32 currHeight;
	Int32 lastHeight = height;
	Int32 i = this->nThread;
	Bool fin;
	while (i-- > 0)
	{
		currHeight = MulDiv32(i, height, this->nThread);
		this->stats[i].inPt = inPt + currHeight * sstep;
		this->stats[i].outPt = outPt + currHeight * dstep;
		this->stats[i].width = width;
		this->stats[i].height = lastHeight - currHeight;
		this->stats[i].tap = tap;
		this->stats[i].index = index;
		this->stats[i].weight = weight;
		this->stats[i].sstep = sstep;
		this->stats[i].dstep = dstep;

		this->stats[i].status = 3;
		this->stats[i].evt->Set();
		lastHeight = currHeight;
	}
	while (true)
	{
		fin = true;
		i = this->nThread;
		while (i-- > 0)
		{
			if (this->stats[i].status == 3)
			{
				fin = false;
				break;
			}
		}
		if (fin)
			break;
		this->evtMain.Wait();
	}
}

void Media::Resizer::LanczosResizer8_8::mt_vertical_filter(UInt8 *inPt, UInt8 *outPt, Int32 width, Int32 height, Int32 tap, Int32 *index, Int32 *weight, UInt32 sstep, UInt32 dstep)
{
	Int32 currHeight;
	Int32 lastHeight = height;
	Int32 i = this->nThread;
	Bool fin;
	while (i-- > 0)
	{
		currHeight = MulDiv32(i, height, this->nThread);
		this->stats[i].inPt = inPt;
		this->stats[i].outPt = outPt + currHeight * dstep;
		this->stats[i].width = width;
		this->stats[i].height = lastHeight - currHeight;
		this->stats[i].tap = tap;
		this->stats[i].index = index + currHeight * tap;
		this->stats[i].weight = weight + currHeight * tap;
		this->stats[i].sstep = sstep;
		this->stats[i].dstep = dstep;

		this->stats[i].status = 5;
		this->stats[i].evt->Set();
		lastHeight = currHeight;
	}
	while (true)
	{
		fin = true;
		i = this->nThread;
		while (i-- > 0)
		{
			if (this->stats[i].status == 5)
			{
				fin = false;
				break;
			}
		}
		if (fin)
			break;
		this->evtMain.Wait();
	}
}

UInt32 Media::Resizer::LanczosResizer8_8::WorkerThread(AnyType obj)
{
	NN<LanczosResizer8_8> lr = obj.GetNN<LanczosResizer8_8>();
	Int32 threadId = lr->currId;
	LRTHREADSTAT *ts = &lr->stats[threadId];

	ts->status = 1;
	lr->evtMain.Set();
	while (true)
	{
		ts->evt->Wait();
		if (ts->status == 2)
		{
			break;
		}
		else if (ts->status == 3)
		{
			LanczosResizer8_8::horizontal_filter(ts->inPt, ts->outPt, ts->width, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep);
			ts->status = 4;
			lr->evtMain.Set();
		}
		else if (ts->status == 5)
		{
			LanczosResizer8_8::vertical_filter(ts->inPt, ts->outPt, ts->width, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep);
			ts->status = 6;
			lr->evtMain.Set();
		}
	}
	lr->stats[threadId].status = 0;
	lr->evtMain.Set();
	return 0;
}

void Media::Resizer::LanczosResizer8_8::DestoryHori()
{
	if (hIndex)
	{
		MemFree(hIndex);
		hIndex = 0;
	}
	if (hWeight)
	{
		MemFree(hWeight);
		hWeight = 0;
	}
	hsSize = 0;
}

void Media::Resizer::LanczosResizer8_8::DestoryVert()
{
	if (vIndex)
	{
		MemFree(vIndex);
		vIndex = 0;
	}
	if (vWeight)
	{
		MemFree(vWeight);
		vWeight = 0;
	}
	vsSize = 0;
}

Media::Resizer::LanczosResizer8_8::LanczosResizer8_8(Int32 nTap) : Media::ImageResizer(Media::AT_NO_ALPHA)
{
//	SYSTEM_INFO sysInfo;
	Int32 i;

	nThread = Sync::ThreadUtil::GetThreadCnt();
	if (nThread <= 0)
		nThread = 1;

	this->nTap = nTap << 1;
	stats = MemAlloc(LRTHREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event());
		stats[i].status = 0;
		currId = i;
		Sync::ThreadUtil::Create(WorkerThread, this);
		while (stats[i].status == 0)
		{
			this->evtMain.Wait();
		}
	}

	hsSize = 0;
	hdSize = 0;
	hsOfst = 0;
	hIndex = 0;
	hWeight = 0;
	hTap = 0;

	vsSize = 0;
	vdSize = 0;
	vsOfst = 0;
	vIndex = 0;
	vWeight = 0;
	vTap = 0;

	buffW = 0;
	buffH = 0;
	buffPtr = 0;
}

Media::Resizer::LanczosResizer8_8::~LanczosResizer8_8()
{
	Int32 i = nThread;
	Bool exited;
	while (i-- > 0)
	{
		if (stats[i].status != 0)
		{
			stats[i].status = 2;
			stats[i].evt->Set();
		}
	}
	while (true)
	{
		exited = true;
		i = nThread;
		while (i-- > 0)
		{
			if (stats[i].status != 0)
			{
				exited = false;
				break;
			}
		}
		if (exited)
			break;

		this->evtMain.Wait();
	}
	i = nThread;
	while (i-- > 0)
	{
		DEL_CLASS(stats[i].evt);
	}
	MemFree(stats);

	DestoryHori();
	DestoryVert();
	if (buffPtr)
	{
		MemFree(buffPtr);
		buffPtr = 0;
	}
}

void Media::Resizer::LanczosResizer8_8::Resize(UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight)
{
	PARAMETER prm;
	Double w = xOfst + swidth;
	Double h = yOfst + sheight;
	Int32 siWidth = (Int32)w;
	Int32 siHeight = (Int32)h;
	w -= siWidth;
	h -= siHeight;
	if (w > 0)
		siWidth++;
	if (h > 0)
		siHeight++;

	if (siWidth != dwidth && siHeight != dheight)
	{
		if (this->hsSize != swidth || this->hdSize != dwidth || this->hsOfst != xOfst)
		{
			DestoryHori();

			if (swidth > dwidth)
			{
				setup_decimation_parameter(swidth, siWidth, dwidth, &prm, 4, xOfst);
			}
			else
			{
				setup_interpolation_parameter(swidth, siWidth, dwidth,&prm, 4, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			hIndex = prm.index;
			hWeight = prm.weight;
			hTap = prm.tap;
		}

		if (this->vsSize != sheight || this->vdSize != dheight || this->vsOfst != yOfst)
		{
			DestoryVert();

			if (sheight > dheight)
			{
				setup_decimation_parameter(sheight, siHeight, dheight, &prm, sbpl, yOfst);
			}
			else
			{
				setup_interpolation_parameter(sheight, siHeight, dheight, &prm, sbpl, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
		}
		
		if (dheight != buffH || (siWidth != buffW))
		{
			if (buffPtr)
			{
				MemFree(buffPtr);
				buffPtr = 0;
			}
			buffW = siWidth;
			buffH = dheight;
			buffPtr = MemAlloc(UInt8, buffW * buffH << 2);
		}
		mt_vertical_filter(src, buffPtr, siWidth, dheight, vTap, vIndex, vWeight, sbpl, siWidth << 2);
		mt_horizontal_filter(buffPtr, dest, dwidth, dheight, hTap,hIndex, hWeight, siWidth << 2, dbpl);
	}
	else if (siWidth != dwidth)
	{
		if (hsSize != swidth || hdSize != dwidth || this->hsOfst != xOfst)
		{
			DestoryHori();

			if (swidth > dwidth)
			{
				setup_decimation_parameter(swidth, siWidth, dwidth, &prm, 4, xOfst);
			}
			else
			{
				setup_interpolation_parameter(swidth, siWidth, dwidth, &prm, 4, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			hIndex = prm.index;
			hWeight = prm.weight;
			hTap = prm.tap;
		}
		mt_horizontal_filter(src, dest, dwidth, siHeight, hTap, hIndex, hWeight, sbpl, dbpl);
	}
	else if (siHeight != dheight)
	{
		if (vsSize != sheight || vdSize != dheight || vsOfst != yOfst)
		{
			DestoryVert();

			if (sheight > dheight)
			{
				setup_decimation_parameter(sheight, siHeight, dheight, &prm, sbpl, yOfst);
			}
			else
			{
				setup_interpolation_parameter(sheight, siHeight, dheight, &prm, sbpl, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
		}
		mt_vertical_filter(src, dest, siWidth, dheight, vTap, vIndex, vWeight, sbpl,dbpl);
	}
	else
	{
#ifdef HAS_ASM32
		_asm
		{
			mov esi,src
			mov edi,dest
			mov ebx,dbpl
			mov edx,sbpl
			cld
dstslop:
			push esi
			push edi
			mov ecx,siWidth
			rep movsd
			pop edi
			pop esi
			add edi,ebx
			add esi,edx
			dec siHeight
			jnz dstslop
		}
#else
		while (sheight-- > 0)
		{
			MemCopyNO(dest, src, siWidth << 2);
			dest += dbpl;
			src += sbpl;
		}
#endif
	}
}

Bool Media::Resizer::LanczosResizer8_8::IsSupported(Media::FrameInfo *srcInfo)
{
	if (srcInfo->fourcc != 0)
		return false;
	if (srcInfo->storeBPP != 32)
		return false;
	return true;
}

Media::StaticImage *Media::Resizer::LanczosResizer8_8::ProcessToNewPartial(Media::StaticImage *srcImage, Double srcX1, Double srcY1, Double srcX2, Double srcY2)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *img;
	if (!IsSupported(&srcImage->info))
		return 0;
	Math::Size2D<UOSInt> targetSize = this->targetSize;
	if (targetSize.x == 0)
	{
		targetSize.x = srcImage->info.dispSize.x;
	}
	if (targetSize.y == 0)
	{
		targetSize.y = srcImage->info.dispSize.y;
	}
	CalOutputSize(srcImage->info, targetSize, destInfo, rar);
	NEW_CLASS(img, Media::StaticImage(destInfo));
	Int32 tlx = (Int32)srcX1;
	Int32 tly = (Int32)srcY1;
	Resize(srcImage->data + tlx * 4 + tly * srcImage->GetDataBpl(), srcImage->GetDataBpl(), srcX2 - srcX1, srcY2 - srcY1, srcX1 - tlx, srcY1 - tly, img->data, img->GetDataBpl(), destInfo.dispSize.x, destInfo.dispSize.y);
	return img;
}
