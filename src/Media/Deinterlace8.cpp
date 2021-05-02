#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/Deinterlace8.h"
#include "Sync/Event.h"
#include "Sync/Thread.h"
#include <float.h>
#include <math.h>

#define LANCZOS_NTAP 6
#define PI 3.141592653589793

extern "C"
{
	void Deinterlace8_VerticalFilter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
	void Deinterlace8_VerticalFilterOdd(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
	void Deinterlace8_VerticalFilterEven(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
}

/*void Deinterlace8_VerticalFilter(UInt8 *inPt, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, Int32 *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	Int32 currWidth;
	Int32 currHeight;
	_asm
	{
		shr width,1
		mov eax,height
		mov currHeight,eax
		pxor xmm6,xmm6

		mov ebx,index
		mov edi,weight
		mov esi,outPt
vflop:
		push esi
		mov ecx,inPt

		mov eax,width
		mov currWidth,eax

vflop2:
		push ebx
		push edi

		mov edx,tap
		pxor xmm1,xmm1
		pxor xmm3,xmm3
		ALIGN 16
vflop3:
		mov eax,dword ptr [ebx]
		movq xmm0,mmword ptr [ecx+eax]
		mov eax,dword ptr [ebx+4]
		movq xmm2,mmword ptr [ecx+eax]
		movdqa xmm5,xmmword ptr [edi]
		punpcklbw xmm0,xmm0
		punpcklbw xmm2,xmm2
		movdqu xmm4,xmm0
		punpcklwd xmm0,xmm2
		punpckhwd xmm4,xmm2
		psrlw xmm0,1
		psrlw xmm4,1
		pmaddwd xmm0, xmm5
		pmaddwd xmm4, xmm5
		paddd xmm1,xmm0
		paddd xmm3,xmm4
		add ebx,8
		add edi,16
		sub edx,2
		jnz vflop3

		pop edi
		pop ebx

		psrad xmm1,22
		psrad xmm3,22
		packssdw xmm1,xmm3
		packuswb xmm1,xmm6
		movq mmword ptr [esi],xmm1
		add esi,8
		add ecx,8
		dec currWidth
		jnz vflop2

		mov eax,tap
		pop esi
		lea ebx,[ebx+eax*4]
		lea edi,[edi+eax*8]

		add esi,dstep

		dec currHeight
		jnz vflop
	}
}

void Deinterlace8_VerticalFilterOdd(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, OSInt width, OSInt height, OSInt tap, Int32 *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	Int32 currWidth;
	Int32 currHeight;
	if ((width & 3) != 0 || (((OSInt)outPt) & 15) != 0 || (dstep & 15) != 0)
	{
		_asm
		{
			shr width,1
			mov eax,height
			shr eax,1
			mov currHeight,eax
			pxor xmm6,xmm6

			mov ebx,index
			mov edi,weight
			mov esi,outPt
vfoslop:
			push esi
			mov ecx,inPtCurr

			mov eax,width
			shr eax,1
			ALIGN 16
vfoslop4:
			movdqu xmm1,mmword ptr [ecx]
			movdqu mmword ptr [esi],xmm1
			add ecx,16
			add esi,16
			dec eax
			jnz vfoslop4
			mov eax,sstep
			pop esi
			add esi,dstep
			add inPtCurr,eax

			mov eax,tap
			lea ebx,[ebx+eax*4]
			lea edi,[edi+eax*8]

			push esi
			mov ecx,inPt

			mov eax,width
			mov currWidth,eax

vfoslop2:
			push ebx
			push edi

			mov edx,tap
			pxor xmm1,xmm1
			pxor xmm3,xmm3
			ALIGN 16
vfoslop3:
			mov eax,dword ptr [ebx]
			movq xmm0,mmword ptr [ecx+eax]
			mov eax,dword ptr [ebx+4]
			movq xmm2,mmword ptr [ecx+eax]
			movdqa xmm5,xmmword ptr [edi]
			punpcklbw xmm0,xmm0
			punpcklbw xmm2,xmm2
			movdqu xmm4,xmm0
			punpcklwd xmm0,xmm2
			punpckhwd xmm4,xmm2
			psrlw xmm0,1
			psrlw xmm4,1
			pmaddwd xmm0, xmm5
			pmaddwd xmm4, xmm5
			paddd xmm1,xmm0
			paddd xmm3,xmm4
			add ebx,8
			add edi,16
			sub edx,2
			jnz vfoslop3

			pop edi
			pop ebx

			psrad xmm1,22
			psrad xmm3,22
			packssdw xmm1,xmm3
			packuswb xmm1,xmm6
			movq mmword ptr [esi],xmm1
			add esi,8
			add ecx,8
			dec currWidth
			jnz vfoslop2

			mov eax,tap
			pop esi
			lea ebx,[ebx+eax*4]
			lea edi,[edi+eax*8]

			add esi,dstep

			dec currHeight
			jnz vfoslop
		}
	}
	else
	{
		_asm
		{
			shr width,2
			mov eax,height
			shr eax,1
			mov currHeight,eax
			pxor xmm6,xmm6

			mov ebx,index
			mov edi,weight
			mov esi,outPt
vfolop:
			push esi
			mov ecx,inPtCurr

			mov eax,width
			ALIGN 16
vfolop4:
			movdqu xmm1,mmword ptr [ecx]
			movntdq mmword ptr [esi],xmm1
			add ecx,16
			add esi,16
			dec eax
			jnz vfolop4
			mov eax,sstep
			pop esi
			add esi,dstep
			add inPtCurr,eax

			mov eax,tap
			lea ebx,[ebx+eax*4]
			lea edi,[edi+eax*8]

			push esi
			mov ecx,inPt

			mov eax,width
			mov currWidth,eax

vfolop2:
			push ebx
			push edi

			mov edx,tap
			pxor xmm4,xmm4
			pxor xmm5,xmm5
			pxor xmm6,xmm6
			pxor xmm7,xmm7
			ALIGN 16
vfolop3:
			mov eax,dword ptr [ebx]
			movq xmm2,mmword ptr [ecx+eax]
			mov eax,dword ptr [ebx+4]
			movq xmm3,mmword ptr [ecx+eax]
			movdqa xmm0,xmmword ptr [edi]
			punpcklbw xmm2,xmm2
			punpcklbw xmm3,xmm3
			movdqu xmm1,xmm2
			punpcklwd xmm1,xmm3
			punpckhwd xmm2,xmm3
			psrlw xmm1,1
			psrlw xmm2,1
			pmaddwd xmm1,xmm0
			pmaddwd xmm2,xmm0
			paddd xmm4,xmm1
			paddd xmm5,xmm2

			mov eax,dword ptr [ebx]
			movq xmm2,mmword ptr [ecx+eax+8]
			mov eax,dword ptr [ebx+4]
			movq xmm3,mmword ptr [ecx+eax+8]
			punpcklbw xmm2,xmm2
			punpcklbw xmm3,xmm3
			movdqu xmm1,xmm2
			punpcklwd xmm1,xmm3
			punpckhwd xmm2,xmm3
			psrlw xmm1,1
			psrlw xmm2,1
			pmaddwd xmm1,xmm0
			pmaddwd xmm2,xmm0
			paddd xmm6,xmm1
			paddd xmm7,xmm2

			add ebx,8
			add edi,16
			sub edx,2
			jnz vfolop3

			pop edi
			pop ebx

			psrad xmm4,22
			psrad xmm5,22
			psrad xmm6,22
			psrad xmm7,22
			packssdw xmm4,xmm5
			packssdw xmm6,xmm7
			packuswb xmm4,xmm6
			movntdq mmword ptr [esi],xmm4
			add esi,16
			add ecx,16
			dec currWidth
			jnz vfolop2

			mov eax,tap
			pop esi
			lea ebx,[ebx+eax*4]
			lea edi,[edi+eax*8]

			add esi,dstep

			dec currHeight
			jnz vfolop
			sfence
		}
	}
}

void Deinterlace8_VerticalFilterEven(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, OSInt width, OSInt height, Int32 tap, Int32 *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	Int32 currWidth;
	Int32 currHeight;
	if ((width & 3) != 0 || (((OSInt)outPt) & 15) != 0 || (dstep & 15) != 0)
	{
		_asm
		{
			shr width,1
			mov eax,height
			shr eax,1
			mov currHeight,eax
			pxor xmm6,xmm6

			mov ebx,index
			mov edi,weight
			mov esi,outPt
vfeslop:
			push esi
			mov ecx,inPt

			mov eax,width
			mov currWidth,eax

vfeslop2:
			push ebx
			push edi

			mov edx,tap
			pxor xmm1,xmm1
			pxor xmm3,xmm3
			ALIGN 16
vfeslop3:
			mov eax,dword ptr [ebx]
			movq xmm0,mmword ptr [ecx+eax]
			mov eax,dword ptr [ebx+4]
			movq xmm2,mmword ptr [ecx+eax]
			movdqa xmm5,xmmword ptr [edi]
			punpcklbw xmm0,xmm0
			punpcklbw xmm2,xmm2
			movdqu xmm4,xmm0
			punpcklwd xmm0,xmm2
			punpckhwd xmm4,xmm2
			psrlw xmm0,1
			psrlw xmm4,1
			pmaddwd xmm0, xmm5
			pmaddwd xmm4, xmm5
			paddd xmm1,xmm0
			paddd xmm3,xmm4
			add ebx,8
			add edi,16
			sub edx,2
			jnz vfeslop3

			pop edi
			pop ebx

			psrad xmm1,22
			psrad xmm3,22
			packssdw xmm1,xmm3
			packuswb xmm1,xmm6
			movq mmword ptr [esi],xmm1
			add esi,8
			add ecx,8
			dec currWidth
			jnz vfeslop2

			mov eax,tap
			pop esi
			lea ebx,[ebx+eax*4]
			lea edi,[edi+eax*8]
			add esi,dstep

			push esi
			mov ecx,inPtCurr


			mov eax,width
			shr eax,1
			ALIGN 16
vfeslop4:
			movdqu xmm1,mmword ptr [ecx]
			movdqu mmword ptr [esi],xmm1
			add ecx,16
			add esi,16
			dec eax
			jnz vfeslop4
			mov eax,sstep
			pop esi
			add esi,dstep
			add inPtCurr,eax

			mov eax,tap
			lea ebx,[ebx+eax*4]
			lea edi,[edi+eax*8]


			dec currHeight
			jnz vfeslop
		}
	}
	else
	{
		_asm
		{
			shr width,2
			mov eax,height
			shr eax,1
			mov currHeight,eax
			pxor xmm6,xmm6

			mov ebx,index
			mov edi,weight
			mov esi,outPt
vfelop:
			push esi
			mov ecx,inPt


			mov eax,width
			mov currWidth,eax

vfelop2:
			push ebx
			push edi

			mov edx,tap
			pxor xmm4,xmm4
			pxor xmm5,xmm5
			pxor xmm6,xmm6
			pxor xmm7,xmm7
			ALIGN 16
vfelop3:
			mov eax,dword ptr [ebx]
			movq xmm2,mmword ptr [ecx+eax]
			mov eax,dword ptr [ebx+4]
			movq xmm3,mmword ptr [ecx+eax]
			movdqa xmm0,xmmword ptr [edi]
			punpcklbw xmm2,xmm2
			punpcklbw xmm3,xmm3
			movdqu xmm1,xmm2
			punpcklwd xmm1,xmm3
			punpckhwd xmm2,xmm3
			psrlw xmm1,1
			psrlw xmm2,1
			pmaddwd xmm1, xmm0
			pmaddwd xmm2, xmm0
			paddd xmm4,xmm1
			paddd xmm5,xmm2

			mov eax,dword ptr [ebx]
			movq xmm2,mmword ptr [ecx+eax+8]
			mov eax,dword ptr [ebx+4]
			movq xmm3,mmword ptr [ecx+eax+8]
			punpcklbw xmm2,xmm2
			punpcklbw xmm3,xmm3
			movdqu xmm1,xmm2
			punpcklwd xmm1,xmm3
			punpckhwd xmm2,xmm3
			psrlw xmm1,1
			psrlw xmm2,1
			pmaddwd xmm1, xmm0
			pmaddwd xmm2, xmm0
			paddd xmm6,xmm1
			paddd xmm7,xmm2

			add ebx,8
			add edi,16
			sub edx,2
			jnz vfelop3

			pop edi
			pop ebx

			psrad xmm4,22
			psrad xmm5,22
			psrad xmm6,22
			psrad xmm7,22
			packssdw xmm4,xmm5
			packssdw xmm6,xmm7
			packuswb xmm4,xmm6
			movntdq mmword ptr [esi],xmm4
			add esi,16
			add ecx,16
			dec currWidth
			jnz vfelop2

			mov eax,tap
			pop esi
			lea ebx,[ebx+eax*4]
			lea edi,[edi+eax*8]
			add esi,dstep

			push esi
			mov ecx,inPtCurr

			mov eax,width
			ALIGN 16
vfelop4:
			movdqu xmm1,mmword ptr [ecx]
			movntdq mmword ptr [esi],xmm1
			add ecx,16
			add esi,16
			dec eax
			jnz vfelop4
			mov eax,sstep
			pop esi
			add esi,dstep
			add inPtCurr,eax

			mov eax,tap
			lea ebx,[ebx+eax*4]
			lea edi,[edi+eax*8]

			dec currHeight
			jnz vfelop
			sfence
		}
	}
}
*/

Double Media::Deinterlace8::lanczos3_weight(Double phase)
{
	Double ret;
	
	if(fabs(phase) < DBL_EPSILON)
	{
		return 1.0;
	}

	if ((fabs(phase) * 2) >= LANCZOS_NTAP){
		return 0.0;
	}

	ret = sin(PI * phase) * sin(PI * phase / LANCZOS_NTAP * 2) / (PI * PI * phase * phase / LANCZOS_NTAP * 2);

	return ret;
}

void Media::Deinterlace8::SetupInterpolationParameter(UOSInt source_length, UOSInt result_length, DI8PARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UInt32 i,j;
	Int32 n;
	Double *work;
	Double  sum;
	Double  pos;

	out->length = result_length;
	out->tap = LANCZOS_NTAP;
	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(OSInt, out->length * out->tap);

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (i+0.5)*source_length;
		pos = pos / result_length + offsetCorr;
		n = (Int32)floor(pos - (LANCZOS_NTAP / 2 - 0.5));//2.5);
		pos = (n+0.5-pos);
		sum = 0;
		for(j = 0; j < out->tap; j++)
		{
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if(n >= source_length){
				out->index[i * out->tap + j] = (source_length - 1) * indexSep;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = lanczos3_weight(pos);
			sum += work[j];
			pos += 1;
			n += 1;
		}

		j = 0;
		while (j < out->tap)
		{
			UInt16 v1 = (UInt16)(0xffff & Math::Double2Int32((work[j] / sum) * 32767.0));
			UInt16 v2 = (UInt16)(0xffff & Math::Double2Int32((work[j + 1] / sum) * 32767.0));
			UInt16 *tmpPtr = (UInt16*)&out->weight[i * out->tap + j];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;
			j += 2;
		}
		i++;
	}

	MemFree(work);
}

UInt32 __stdcall Media::Deinterlace8::ProcThread(void *obj)
{
	DI8THREADSTAT *stat = (DI8THREADSTAT*) obj;
	stat->status = 1;
	while (true)
	{
		stat->evt->Wait();
		if (stat->status == 4)
			break;

		if (stat->status == 2)
		{
			Deinterlace8_VerticalFilter(stat->inPt, stat->outPt, stat->width, stat->height, stat->tap, stat->index, stat->weight, stat->sstep, stat->dstep);
			stat->status = 3;
			stat->evtMain->Set();
		}
		else if (stat->status == 5)
		{
//			Deinterlace8_VerticalFilter(stat->inPt, stat->outPt, stat->width, stat->height, stat->tap, stat->index, stat->weight, stat->sstep, stat->dstep);
			Deinterlace8_VerticalFilterOdd(stat->inPt, stat->inPtCurr, stat->outPt, stat->width, stat->height, stat->tap, stat->index, stat->weight, stat->sstep, stat->dstep);
			stat->status = 3;
			stat->evtMain->Set();
		}
		else if (stat->status == 6)
		{
//			Deinterlace8_VerticalFilter(stat->inPt, stat->outPt, stat->width, stat->height, stat->tap, stat->index, stat->weight, stat->sstep, stat->dstep);
			Deinterlace8_VerticalFilterEven(stat->inPt, stat->inPtCurr, stat->outPt, stat->width, stat->height, stat->tap, stat->index, stat->weight, stat->sstep, stat->dstep);
			stat->status = 3;
			stat->evtMain->Set();
		}
	}
	stat->status = 0;
	stat->evtMain->Set();
	return 0;
}

Media::Deinterlace8::Deinterlace8(UOSInt fieldCnt, OSInt fieldSep)
{
	this->oddParam.index = 0;
	this->oddParam.weight = 0;
	this->evenParam.index = 0;
	this->evenParam.weight = 0;
	this->fieldCnt = 0;
	this->fieldSep = 0;
	Reinit(fieldCnt, fieldSep);

	nCore = Sync::Thread::GetThreadCnt();
	if (nCore > 4)
	{
		nCore = 4;
	}

	this->stats = MemAlloc(DI8THREADSTAT, nCore);
	UOSInt i = nCore;

	NEW_CLASS(evtMain, Sync::Event((const UTF8Char*)"Media.Deinterlace8.evtMain"));
	while (i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event((const UTF8Char*)"Media.Deinterlace8.stats.evt"));
		stats[i].status = 0;
		stats[i].evtMain = this->evtMain;
	}

	i = nCore;
	while (i-- > 0)
	{
		Sync::Thread::Create(ProcThread, &stats[i]);
	}

}

Media::Deinterlace8::~Deinterlace8()
{
	UOSInt i = nCore;
	while (i-- > 0)
	{
		stats[i].status = 4;
		stats[i].evt->Set();
	}
	Bool allExit;
	while (true)
	{
		evtMain->Wait(100);
		allExit = true;
		i = nCore;
		while (i-- > 0)
		{
			if (stats[i].status != 0)
			{
				allExit = false;
				break;
			}
		}
		if (allExit)
			break;
	}

	i = nCore;
	while (i-- > 0)
	{
		DEL_CLASS(stats[i].evt);
	}
	DEL_CLASS(evtMain);
	MemFree(this->stats);
	if (this->oddParam.index)
	{
		MemFreeA(this->oddParam.weight);
		MemFreeA(this->oddParam.index);
		this->oddParam.weight = 0;
		this->oddParam.index = 0;
	}
	if (this->evenParam.index)
	{
		MemFreeA(this->evenParam.weight);
		MemFreeA(this->evenParam.index);
		this->evenParam.weight = 0;
		this->evenParam.index = 0;
	}
}

void Media::Deinterlace8::Reinit(UOSInt fieldCnt, OSInt fieldSep)
{
	if (fieldCnt == this->fieldCnt && fieldSep == this->fieldSep)
		return;

	if (this->oddParam.index)
	{
		MemFreeA(this->oddParam.weight);
		MemFreeA(this->oddParam.index);
		this->oddParam.weight = 0;
		this->oddParam.index = 0;
	}
	if (this->evenParam.index)
	{
		MemFreeA(this->evenParam.weight);
		MemFreeA(this->evenParam.index);
		this->evenParam.weight = 0;
		this->evenParam.index = 0;
	}
	Media::Deinterlace8::SetupInterpolationParameter(fieldCnt, fieldCnt << 1, &oddParam, fieldSep, 0.25);
	Media::Deinterlace8::SetupInterpolationParameter(fieldCnt, fieldCnt << 1, &evenParam, fieldSep, -0.25);
	this->fieldCnt = fieldCnt;
	this->fieldSep = fieldSep;
}

void Media::Deinterlace8::Deinterlace(UInt8 *src, UInt8 *dest, Bool bottomField, UOSInt width, OSInt dstep)
{
	if (!bottomField)
	{
		UOSInt imgHeight = oddParam.length >> 1;

		UOSInt thisLine;
		UOSInt lastLine = imgHeight << 1;
		UOSInt i = nCore;
		while (i-- > 0)
		{
			thisLine = MulDivUOS(imgHeight, i, nCore) * 2;
			stats[i].inPt = src;
			stats[i].inPtCurr = src + (this->fieldSep * thisLine >> 1);
			stats[i].outPt = dest + dstep * thisLine;
			stats[i].width = width;
			stats[i].height = lastLine - thisLine;
			stats[i].tap = oddParam.tap;
			stats[i].index = oddParam.index + thisLine * oddParam.tap;
			stats[i].weight = oddParam.weight + thisLine * oddParam.tap;
			stats[i].sstep = this->fieldSep;
			stats[i].dstep = dstep;

			stats[i].status = 5;
			stats[i].evt->Set();
			lastLine = thisLine;
		}

		while (true)
		{
			evtMain->Wait(100);
			Bool allFin;
			allFin = true;
			i = nCore;
			while (i-- > 0)
			{
				if (stats[i].status == 5)
				{
					allFin = false;
					break;
				}
			}
			if (allFin)
				break;
		}
	}
	else
	{
		UOSInt imgHeight = evenParam.length >> 1;

		UOSInt thisLine;
		UOSInt lastLine = imgHeight << 1;
		UOSInt i = nCore;
		while (i-- > 0)
		{
			thisLine = MulDivUOS(imgHeight, i, nCore) * 2;
			stats[i].inPt = src;
			stats[i].inPtCurr = src + (this->fieldSep * thisLine >> 1);
			stats[i].outPt = dest + dstep * thisLine;
			stats[i].width = width;
			stats[i].height = lastLine - thisLine;
			stats[i].tap = evenParam.tap;
			stats[i].index = evenParam.index + thisLine * evenParam.tap;
			stats[i].weight = evenParam.weight + thisLine * evenParam.tap;
			stats[i].sstep = this->fieldSep;
			stats[i].dstep = dstep;

			stats[i].status = 6;
			stats[i].evt->Set();
			lastLine = thisLine;
		}

		while (true)
		{
			evtMain->Wait(100);
			Bool allFin;
			allFin = true;
			i = nCore;
			while (i-- > 0)
			{
				if (stats[i].status == 6)
				{
					allFin = false;
					break;
				}
			}
			if (allFin)
				break;
		}
	}
}
