#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSUYVY_RGB8.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void CSUYVY_RGB8_do_uyvy2rgb(UInt8 *src, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
}

/*void CSUYVY_RGB8_do_uyvy2rgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgb, UInt8 *rgbGammaCorr)
{
	Int32 hleft;
	OSInt wsize = (width >> 1) - 2;
	Int32 cnt;
	Int64 tmpVal;

	_asm
	{
		mov esi,src
		mov edi,dest
		mov ebx,yuv2rgb
		mov edx,rgbGammaCorr

		mov ecx,height
		mov hleft,ecx
u2rlop:
		push edi
		mov ecx,wsize
		mov cnt,ecx

		movzx eax,byte ptr [esi+1]
		movq xmm0,mmword ptr [ebx+eax*8]
		movzx eax,byte ptr [esi+3]
		movq xmm1,mmword ptr [ebx+eax*8]
		movzx eax,byte ptr [esi]
		movq xmm2,mmword ptr [ebx+eax*8+2048]
		movzx eax,byte ptr [esi+2]
		movq xmm3,mmword ptr [ebx+eax*8+4096]
		paddsw xmm0,xmm2
		paddsw xmm0,xmm3
		movq tmpVal,xmm0
		psraw xmm2,1
		psraw xmm3,1
		paddsw xmm1,xmm2
		paddsw xmm1,xmm3
		
		movzx eax,word ptr tmpVal[4]
		movzx ecx,byte ptr [edx+eax]
		or ecx,0xff00
		shl ecx,16
		movzx eax,word ptr tmpVal[2]
		mov ch,byte ptr [edx+eax+65536]
		movzx eax,word ptr tmpVal
		mov cl,byte ptr [edx+eax+131072]
		mov dword ptr [edi],ecx
		add edi,8
		add esi,4
u2rlop2:
		movzx eax,byte ptr [esi+1]
		movq xmm0,mmword ptr [ebx+eax*8]
		movzx eax,byte ptr [esi]
		movq xmm2,mmword ptr [ebx+eax*8+2048]
		movzx eax,byte ptr [esi+2]
		movq xmm3,mmword ptr [ebx+eax*8+4096]
		paddsw xmm0,xmm2
		paddsw xmm0,xmm3
		movq tmpVal,xmm0

		movzx eax,word ptr tmpVal[4]
		movzx ecx,byte ptr [edx+eax]
		or ecx,0xff00
		shl ecx,16
		movzx eax,word ptr tmpVal[2]
		mov ch,byte ptr [edx+eax+65536]
		movzx eax,word ptr tmpVal
		mov cl,byte ptr [edx+eax+131072]
		mov dword ptr [edi],ecx

		psraw xmm2,1
		psraw xmm3,1
		paddsw xmm1,xmm2
		paddsw xmm1,xmm3
		movq tmpVal,xmm1

		movzx eax,word ptr tmpVal[4]
		movzx ecx,byte ptr [edx+eax]
		or ecx,0xff00
		shl ecx,16
		movzx eax,word ptr tmpVal[2]
		mov ch,byte ptr [edx+eax+65536]
		movzx eax,word ptr tmpVal
		mov cl,byte ptr [edx+eax+131072]
		mov dword ptr [edi-4],ecx

		movzx eax,byte ptr [esi+3]
		movq xmm1,mmword ptr [ebx+eax*8]
		paddsw xmm1,xmm2
		paddsw xmm1,xmm3

		add esi,4
		add edi,8
		dec cnt
		jnz u2rlop2

		movzx eax,byte ptr [esi+1]
		movq xmm0,mmword ptr [ebx+eax*8]
		movzx eax,byte ptr [esi]
		movq xmm2,mmword ptr [ebx+eax*8+2048]
		movzx eax,byte ptr [esi+2]
		movq xmm3,mmword ptr [ebx+eax*8+4096]
		paddsw xmm0,xmm2
		paddsw xmm0,xmm3
		movq tmpVal,xmm0

		movzx eax,word ptr tmpVal[4]
		movzx ecx,byte ptr [edx+eax]
		or ecx,0xff00
		shl ecx,16
		movzx eax,word ptr tmpVal[2]
		mov ch,byte ptr [edx+eax+65536]
		movzx eax,word ptr tmpVal
		mov cl,byte ptr [edx+eax+131072]
		mov dword ptr [edi],ecx

		movzx eax,byte ptr [esi+3]
		movq xmm0,mmword ptr [ebx+eax*8]
		paddsw xmm0,xmm2
		paddsw xmm0,xmm3
		movq tmpVal,xmm0

		movzx eax,word ptr tmpVal[4]
		movzx ecx,byte ptr [edx+eax]
		or ecx,0xff00
		shl ecx,16
		movzx eax,word ptr tmpVal[2]
		mov ch,byte ptr [edx+eax+65536]
		movzx eax,word ptr tmpVal
		mov cl,byte ptr [edx+eax+131072]
		mov dword ptr [edi+4],ecx

		psraw xmm2,1
		psraw xmm3,1
		paddsw xmm1,xmm2
		paddsw xmm1,xmm3
		movq tmpVal,xmm1

		movzx eax,word ptr tmpVal[4]
		movzx ecx,byte ptr [edx+eax]
		or ecx,0xff00
		shl ecx,16
		movzx eax,word ptr tmpVal[2]
		mov ch,byte ptr [edx+eax+65536]
		movzx eax,word ptr tmpVal
		mov cl,byte ptr [edx+eax+131072]
		mov dword ptr [edi-4],ecx

		add esi,4

		pop edi
		add edi,dbpl
		dec hleft
		jnz u2rlop
	}
}*/

UInt32 Media::CS::CSUYVY_RGB8::WorkerThread(void *obj)
{
	CSUYVY_RGB8 *converter = (CSUYVY_RGB8*)obj;
	UOSInt threadId = converter->currId;
	THREADSTAT *ts = &converter->stats[threadId];

	ts->status = 1;
	converter->evtMain->Set();
	while (true)
	{
		ts->evt->Wait();
		if (ts->status == 2)
		{
			break;
		}
		else if (ts->status == 3)
		{
			CSUYVY_RGB8_do_uyvy2rgb(ts->yPtr, ts->dest, ts->width, ts->height, ts->dbpl, converter->yuv2rgb, converter->rgbGammaCorr);
			ts->status = 4;
			converter->evtMain->Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain->Set();
	return 0;
}

Media::CS::CSUYVY_RGB8::CSUYVY_RGB8(const Media::ColorProfile *srcColor, const Media::ColorProfile *destColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess) : Media::CS::CSYUV_RGB8(srcColor, destColor, yuvType, colorSess)
{
	UOSInt i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();

	NEW_CLASS(evtMain, Sync::Event());
	stats = MemAlloc(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event());
		stats[i].status = 0;

		currId = i;
		Sync::ThreadUtil::Create(WorkerThread, this);
		while (stats[i].status == 0)
		{
			evtMain->Wait();
		}
	}
}

Media::CS::CSUYVY_RGB8::~CSUYVY_RGB8()
{
	UOSInt i = nThread;
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
				if (stats[i].status == 2)
				{
					stats[i].evt->Set();
					exited = false;
					break;
				}
				else if (stats[i].status > 0)
				{
					stats[i].status = 2;
					stats[i].evt->Set();
					exited = false;
					break;
				}
				else
				{
					stats[i].evt->Set();
					exited = false;
					break;
				}
			}
		}
		if (exited)
			break;

		evtMain->Wait(100);
	}
	i = nThread;
	while (i-- > 0)
	{
		DEL_CLASS(stats[i].evt);
	}
	DEL_CLASS(evtMain);
	MemFree(stats);
}

void Media::CS::CSUYVY_RGB8::ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UOSInt i = this->nThread;
	UOSInt lastHeight = dispHeight;
	UOSInt currHeight;
//	Int32 cSize = width << 4;

	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, dispHeight, nThread) & (UOSInt)~1;

		stats[i].yPtr = srcPtr[0] + currHeight * (srcStoreWidth << 1);
		stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
		stats[i].width = dispWidth;
		stats[i].height = lastHeight - currHeight;
		stats[i].dbpl = destRGBBpl;

		stats[i].status = 3;
		stats[i].evt->Set();
		lastHeight = currHeight;
	}
	Bool exited;
	while (true)
	{
		exited = true;
		i = nThread;
		while (i-- > 0)
		{
			if (stats[i].status == 3)
			{
				stats[i].evt->Set();
				exited = false;
				break;
			}
		}
		if (exited)
			break;
		evtMain->Wait();
	}
}

UOSInt Media::CS::CSUYVY_RGB8::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 1;
}
