#include "Stdafx.h"
#include <math.h>
#include <float.h>
#include <windows.h>
#include "MyMemory.h"
#include "Sync/Event.h"
#include "Media/IImgResizer.h"
#include "Media/Resizer/LanczosResizerH13_8.h"

#define PI 3.141592653589793


Double Media::Resizer::LanczosResizerH13_8::lanczos3_weight(Double phase)
{
	Double ret;
	
	if(fabs(phase) < DBL_EPSILON)
	{
		return 1.0;
	}

	if ((fabs(phase) * 2) >= nTap){
		return 0.0;
	}

	ret = sin(PI * phase) * sin(PI * phase / nTap * 2) / (PI * PI * phase * phase / nTap * 2);

	return ret;
}

void Media::Resizer::LanczosResizerH13_8::setup_interpolation_parameter(Int32 source_length, UOSInt result_length, LRH13PARAMETER *out, Int32 indexSep, Double offsetCorr)
{
	Int32 i,j,n;
	Double *work;
	Double  sum;
	Double  pos;

//	clear_parameter(out);
	
	out->length = result_length;
	out->tap = this->nTap;
	out->weight = MemAlloc(Int64, out->length * out->tap);
	out->index = MemAlloc(Int32, out->length * out->tap);

//	for(i=0;i<result_length;i++){
//		out->weight[i] = (int *)MemAlloc(sizeof(int)*out->tap);
//		out->index[i] = (int *)MemAlloc(sizeof(int)*out->tap);
//	}
	work = MemAlloc(Double, out->tap);

	for(i=0;i<result_length;i++){
		pos = (i+0.5)*source_length;
		pos /= result_length;
		n = (Int32)floor(pos - (this->nTap / 2 - 0.5) + offsetCorr);//2.5);
		pos = (n+0.5-pos);
		sum = 0;
		for(j=0;j<out->tap;j++){
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

		for(j=0;j<out->tap;j++){
			Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
			out->weight[i * out->tap+j] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
		}
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerH13_8::setup_decimation_parameter(Int32 source_length, UOSInt result_length, LRH13PARAMETER *out, Int32 indexSep, Int32 offsetCorr)
{
	Int32 i,j,n;
	Double *work;
	Double  sum;
	Double  pos, phase;

	out->length = result_length;
	out->tap = (this->nTap * (source_length) + (result_length - 1)) / result_length;

	out->weight = MemAlloc(Int64, out->length * out->tap);
	out->index = MemAlloc(Int32, out->length * out->tap);
	
//	for(i=0;i<result_length;i++){
//		out->weight[i] = (int *)MemAlloc(sizeof(int)*out->tap);
//		out->index[i] = (int *)MemAlloc(sizeof(int)*out->tap);
//	}
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
			}else if(n >= source_length){
				out->index[i * out->tap + j] = (source_length-1) * indexSep;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = lanczos3_weight(phase);
			sum += work[j];
			n += 1;
		}

		for(j=0;j<out->tap;j++){
			Int64 i64tmp = 0xffff & (Int64)((work[j] / sum) * 32767.0);
			out->weight[i * out->tap+j] = (i64tmp << 48) | (i64tmp << 32) | (i64tmp << 16) | i64tmp;
		}
	}

	MemFree(work);
}

/*-----------------------------------------------------------------*/
void Media::Resizer::LanczosResizerH13_8::horizontal_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep)
{
	Int64 toAdd = 0xff80ff80ff80ff80;
	Int64 toAdd2 = 0x8080808080808080;
	Int32 currWidth;
	Int32 currHeight;
	if (width & 1)
	{
		_asm
		{
		mov eax,height
		mov currHeight,eax
		pxor mm3,mm3
hflop:
		mov esi,outPt
		mov ecx,inPt

		mov eax,width
		mov currWidth,eax

		mov ebx,index
		mov edi,weight
hflop2:
		mov edx,tap
		pxor mm1,mm1
hflop3:
		mov eax,dword ptr [ebx]
		movq mm0,mmword ptr [ecx+eax]
		pmulhw mm0, mmword ptr [edi]
		paddsw mm1,mm0
		add edi,8
		add ebx,4
		dec edx
		jnz hflop3

		psraw mm1,4
		paddw mm1,toAdd
		packsswb mm1,mm3
		paddb mm1,toAdd2

		movd dword ptr [esi],mm1
		add esi,4
		dec currWidth
		jnz hflop2

		mov eax,sstep
		mov edx,dstep
		add inPt,eax
		add outPt,edx

		dec currHeight
		jnz hflop

		emms
		}
	}
	else
	{
		_asm
		{
		mov eax,height
		mov currHeight,eax
		pxor mm3,mm3
hflop4:
		mov esi,outPt
		mov ecx,inPt

		mov eax,width
		mov currWidth,eax

		shr currWidth,1

		mov ebx,index
		mov edi,weight
hflop5:
		mov edx,tap
		pxor mm1,mm1
		pxor mm2,mm2
hflop6:
		mov eax,dword ptr [ebx]
		movq mm0,mmword ptr [ecx+eax]
		add ebx,4
		pmulhw mm0, mmword ptr [edi]
		paddsw mm1,mm0
		add edi,8
		dec edx
		jnz hflop6

		mov edx,tap
hflop7:
		mov eax,dword ptr [ebx]
		movq mm0,mmword ptr [ecx+eax]
		add ebx,4
		pmulhw mm0, mmword ptr [edi]
		paddsw mm2,mm0
		add edi,8
		dec edx
		jnz hflop7

		psraw mm1,4
		psraw mm2,4
		paddw mm1,toAdd
		paddw mm2,toAdd
		packsswb mm1,mm2
		paddb mm1,toAdd2
		movq mmword ptr [esi],mm1
		add esi,8
		dec currWidth
		jnz hflop5

		mov eax,sstep
		mov edx,dstep
		add inPt,eax
		add outPt,edx

		dec currHeight
		jnz hflop4

		emms
		}
	}
}

/*-----------------------------------------------------------------*/
void Media::Resizer::LanczosResizerH13_8::vertical_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep)
{
	Int64 toAdd = 0xff80ff80ff80ff80;
	Int64 toAdd2 = 0x80808080;
	Int64 zero = 0;
	Int32 currTap;
	Int32 currWidth;
	Int32 currHeight;
	_asm
	{
		mov eax,height
		mov currHeight,eax
vflop:
		mov esi,outPt
		mov ecx,inPt

		mov eax,width
		mov currWidth,eax

vflop2:
		mov ebx,index
		mov edi,weight

		mov eax,tap
		mov currTap,eax
		pxor mm1,mm1
vflop3:
		mov eax,dword ptr [ebx]
		movq mm0,qword ptr [ecx+eax]
		add ebx,4
		psllw mm0,1
		pmulhw mm0, mmword ptr [edi]
		paddsw mm1,mm0
		add edi,8
		dec currTap
		jnz vflop3

		movq mmword ptr [esi],mm1
		add esi,8
		add ecx,8
		dec currWidth
		jnz vflop2

		mov eax,tap
		mov edx,tap
		shl eax,2
		shl edx,3
		add index,eax
		add weight,edx

		mov edx,dstep
		add outPt,edx

		dec currHeight
		jnz vflop

		emms
	}
}

void Media::Resizer::LanczosResizerH13_8::expand(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep)
{
	Int32 currWidth;
	Int32 currHeight;
	_asm
	{
		mov eax,height
		mov currHeight,eax
		pxor mm1,mm1
vflop:
		mov esi,outPt
		mov ecx,inPt


		mov eax,width
		mov currWidth,eax

vflop2:
		movq mm0,dword ptr [ecx]
		//psrlw mm0,2
		movq qword ptr [esi],mm0
		add esi,8
		add ecx,8
		dec currWidth
		jnz vflop2

		mov eax,sstep
		mov edx,dstep
		add inPt,eax
		add outPt,edx

		dec currHeight
		jnz vflop

		emms
	}
}

void Media::Resizer::LanczosResizerH13_8::collapse(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep)
{
	Int64 toAdd = 0xff80ff80ff80ff80;
	Int64 toAdd2 = 0x80808080;
	Int32 currWidth;
	Int32 currHeight;
	_asm
	{
		mov eax,height
		mov currHeight,eax
		pxor mm1,mm1
vflop:
		mov esi,outPt
		mov ecx,inPt


		mov eax,width
		mov currWidth,eax

vflop2:
		movd mm0,dword ptr [ecx]
		psraw mm0,5
		paddw mm0,toAdd
		packsswb mm0,mm1
		paddb mm0,toAdd2
		movd dword ptr [esi],mm0
		add esi,4
		add ecx,8
		dec currWidth
		jnz vflop2

		mov eax,sstep
		mov edx,dstep
		add inPt,eax
		add outPt,edx

		dec currHeight
		jnz vflop

		emms
	}
}

void Media::Resizer::LanczosResizerH13_8::mt_horizontal_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep)
{
	Int32 currHeight;
	Int32 lastHeight = height;
	Int32 i = this->nThread;
	Bool fin;
	while (i-- > 0)
	{
		currHeight = MulDiv(i, height, this->nThread);
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
		evtMain->Wait();
	}
}

void Media::Resizer::LanczosResizerH13_8::mt_vertical_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	Bool fin;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
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
		evtMain->Wait();
	}
}

void Media::Resizer::LanczosResizerH13_8::mt_expand(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	Bool fin;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->stats[i].inPt = inPt + currHeight * sstep;
		this->stats[i].outPt = outPt + currHeight * dstep;
		this->stats[i].width = width;
		this->stats[i].height = lastHeight - currHeight;
		this->stats[i].sstep = sstep;
		this->stats[i].dstep = dstep;

		this->stats[i].status = 7;
		this->stats[i].evt->Set();
		lastHeight = currHeight;
	}
	while (true)
	{
		fin = true;
		i = this->nThread;
		while (i-- > 0)
		{
			if (this->stats[i].status == 7)
			{
				fin = false;
				break;
			}
		}
		if (fin)
			break;
		evtMain->Wait();
	}
}

void Media::Resizer::LanczosResizerH13_8::mt_collapse(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	Bool fin;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->stats[i].inPt = inPt + currHeight * sstep;
		this->stats[i].outPt = outPt + currHeight * dstep;
		this->stats[i].width = width;
		this->stats[i].height = lastHeight - currHeight;
		this->stats[i].sstep = sstep;
		this->stats[i].dstep = dstep;

		this->stats[i].status = 9;
		this->stats[i].evt->Set();
		lastHeight = currHeight;
	}
	while (true)
	{
		fin = true;
		i = this->nThread;
		while (i-- > 0)
		{
			if (this->stats[i].status == 9)
			{
				fin = false;
				break;
			}
		}
		if (fin)
			break;
		evtMain->Wait();
	}}

UInt32 Media::Resizer::LanczosResizerH13_8::WorkerThread(AnyType obj)
{
	NN<LanczosResizerH13_8> lr = obj.GetNN<LanczosResizerH13_8>();
	Int32 threadId = lr->currId;
	LRH13THREADSTAT *ts = &lr->stats[threadId];

	ts->status = 1;
	lr->evtMain->Set();
	while (true)
	{
		ts->evt->Wait();
		if (ts->status == 2)
		{
			break;
		}
		else if (ts->status == 3)
		{
			LanczosResizerH13_8::horizontal_filter(ts->inPt, ts->outPt, ts->width, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep);
			ts->status = 4;
			lr->evtMain->Set();
		}
		else if (ts->status == 5)
		{
			LanczosResizerH13_8::vertical_filter(ts->inPt, ts->outPt, ts->width, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep);
			ts->status = 6;
			lr->evtMain->Set();
		}
		else if (ts->status == 7)
		{
			LanczosResizerH13_8::expand(ts->inPt, ts->outPt, ts->width, ts->height, ts->sstep, ts->dstep);
			ts->status = 8;
			lr->evtMain->Set();
		}
		else if (ts->status == 9)
		{
			LanczosResizerH13_8::collapse(ts->inPt, ts->outPt, ts->width, ts->height, ts->sstep, ts->dstep);
			ts->status = 10;
			lr->evtMain->Set();
		}
	}
	lr->stats[threadId].status = 0;
	lr->evtMain->Set();
	return 0;
}

void Media::Resizer::LanczosResizerH13_8::DestoryHori()
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

void Media::Resizer::LanczosResizerH13_8::DestoryVert()
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

Media::Resizer::LanczosResizerH13_8::LanczosResizerH13_8(Int32 nTap)
{
//	SYSTEM_INFO sysInfo;
	Int32 i;
	SYSTEM_INFO sysInfo;

	GetSystemInfo(&sysInfo);
	nThread = sysInfo.dwNumberOfProcessors;
	if (nThread <= 0)
		nThread = 1;

	this->nTap = nTap << 1;
	NEW_CLASS(evtMain, Sync::Event(L"Media.LanczosResizerH13_8.evtMain"));
	stats = MemAlloc(LRH13THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event(L"Media.LanczosResizerH13_8.stats.evt"));
		stats[i].status = 0;
		currId = i;
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)WorkerThread, this, 0, 0);
		while (stats[i].status == 0)
		{
			evtMain->Wait();
		}
	}

	hsSize = 0;
	hdSize = 0;
	hIndex = 0;
	hWeight = 0;
	hTap = 0;

	vsSize = 0;
	vdSize = 0;
	vIndex = 0;
	vWeight = 0;
	vTap = 0;

	buffW = 0;
	buffH = 0;
	buffPtr = 0;
}

Media::Resizer::LanczosResizerH13_8::~LanczosResizerH13_8()
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

		evtMain->Wait();
	}
	i = nThread;
	while (i-- > 0)
	{
		DEL_CLASS(stats[i].evt);
	}
	DEL_CLASS(evtMain);
	MemFree(stats);

	DestoryHori();
	DestoryVert();
	if (buffPtr)
	{
		MemFree(buffPtr);
		buffPtr = 0;
	}
}

void Media::Resizer::LanczosResizerH13_8::Resize(UInt8 *src, Int32 sbpl, Int32 swidth, Int32 sheight, UInt8 *dest, Int32 dbpl, UOSInt dwidth, UOSInt dheight)
{
	LRH13PARAMETER prm;
	if (dwidth < 16 || dheight < 16)
		return;

	if (swidth != dwidth && sheight != dheight)
	{
		if (this->hsSize != swidth || this->hdSize != dwidth)
		{
			DestoryHori();

			if (swidth > dwidth)
			{
				setup_decimation_parameter(swidth, dwidth, &prm, 8, 0);
			}
			else
			{
				setup_interpolation_parameter(swidth, dwidth,&prm, 8, 0);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hIndex = prm.index;
			hWeight = prm.weight;
			hTap = prm.tap;
		}

		if (this->vsSize != sheight || this->vdSize != dheight)
		{
			DestoryVert();

			if (sheight > dheight)
			{
				setup_decimation_parameter(sheight, dheight, &prm, sbpl, 0);
			}
			else
			{
				setup_interpolation_parameter(sheight, dheight, &prm, sbpl, 0);
			}
			vsSize = sheight;
			vdSize = dheight;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
		}
		
		if (dheight != buffH || (swidth != buffW))
		{
			if (buffPtr)
			{
				MemFree(buffPtr);
				buffPtr = 0;
			}
			buffW = swidth;
			buffH = dheight;
			buffPtr = MemAlloc(UInt8, buffW * buffH << 3);
		}
		mt_vertical_filter(src, buffPtr, swidth, dheight, vTap, vIndex, vWeight, sbpl, swidth << 3);
		mt_horizontal_filter(buffPtr, dest, dwidth, dheight, hTap,hIndex, hWeight, swidth << 3, dbpl);
	}
	else if (swidth != dwidth)
	{
		if (hsSize != swidth || hdSize != dwidth)
		{
			DestoryHori();

			if (swidth > dwidth)
			{
				setup_decimation_parameter(swidth, dwidth, &prm, 8, 0);
			}
			else
			{
				setup_interpolation_parameter(swidth, dwidth, &prm, 8, 0);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hIndex = prm.index;
			hWeight = prm.weight;
			hTap = prm.tap;
		}
		if (dheight != buffH || (swidth != buffW))
		{
			if (buffPtr)
			{
				MemFree(buffPtr);
				buffPtr = 0;
			}
			buffW = swidth;
			buffH = dheight;
			buffPtr = MemAlloc(UInt8, buffW * buffH << 3);
		}
		mt_expand(src, buffPtr, swidth, sheight, sbpl, swidth << 3);
		mt_horizontal_filter(buffPtr, dest, dwidth, sheight, hTap, hIndex, hWeight, swidth << 3, dbpl);
	}
	else if (sheight != dheight)
	{
		if (vsSize != sheight || vdSize != dheight)
		{
			DestoryVert();

			if (sheight > dheight)
			{
				setup_decimation_parameter(sheight, dheight, &prm, sbpl, 0);
			}
			else
			{
				setup_interpolation_parameter(sheight, dheight, &prm, sbpl, 0);
			}
			vsSize = sheight;
			vdSize = dheight;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
		}
		if (dheight != buffH || (swidth != buffW))
		{
			if (buffPtr)
			{
				MemFree(buffPtr);
				buffPtr = 0;
			}
			buffW = swidth;
			buffH = dheight;
			buffPtr = MemAlloc(UInt8, buffW * buffH << 3);
		}
		mt_vertical_filter(src, buffPtr, swidth, dheight, vTap, vIndex, vWeight, sbpl, swidth << 3);
		mt_collapse(buffPtr, dest, swidth, dheight, swidth << 3, dbpl);
	}
	else
	{
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
			mov ecx,swidth
			rep movsd
			pop edi
			pop esi
			add edi,ebx
			add esi,edx
			dec sheight
			jnz dstslop
		}
	}
}
