#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/CSI420_RGB32C.h"
#include "Sync/ThreadUtil.h"

#define YVADJ 0.25

Media::CS::CSI420_RGB32C::CSI420_RGB32C(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess, Media::PixelFormat destPF) : Media::CS::CSYUV420_RGB32C(srcProfile, destProfile, yuvType, colorSess, destPF)
{
}

Media::CS::CSI420_RGB32C::~CSI420_RGB32C()
{
}

void Media::CS::CSI420_RGB32C::ConvertV2(UnsafeArray<UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UInt32 isLast = 1;
	UInt32 isFirst = 0;
	UOSInt i = this->nThread;
	UOSInt lastHeight = dispHeight;
	UOSInt currHeight;
	UOSInt cSize = dispWidth << 4;
	if (srcStoreWidth & 1)
	{
		srcStoreWidth = srcStoreWidth + 1;//+ 4 - (width & 3);
	}
	if (srcStoreHeight & 1)
	{
		srcStoreHeight = srcStoreHeight + 1;
	}
	
	if (ftype == Media::FT_MERGED_TF || ftype == Media::FT_MERGED_BF)
	{
		UnsafeArray<UInt8> yStart = srcPtr[0];
		UnsafeArray<UInt8> uStart = yStart + srcStoreWidth * srcStoreHeight;
		UnsafeArray<UInt8> vStart = uStart + ((srcStoreWidth * srcStoreHeight) >> 2);

		if (ftype == Media::FT_MERGED_TF)
		{
		}
		else
		{
			vStart += srcStoreWidth >> 1;
			uStart += srcStoreWidth >> 1;
			yStart += srcStoreWidth;
		}

		if ((ycOfst == Media::YCOFST_C_CENTER_LEFT || ycOfst == Media::YCOFST_C_CENTER_CENTER) && (srcStoreWidth & 7) == 0)
		{
			if (ftype == Media::FT_MERGED_TF)
			{
				if (this->yvParamO.index == 0 || (this->yvParamO.length != dispHeight >> 1) || this->yvStepO != srcStoreWidth)
				{
					if (this->yvParamO.index)
					{
						MemFreeA(this->yvParamO.weight);
						MemFreeA(this->yvParamO.index);
						this->yvParamO.weight = 0;
						this->yvParamO.index = 0;
					}
					this->yvStepO = srcStoreWidth;
					SetupInterpolationParameter(dispHeight >> 2, dispHeight >> 1, this->yvParamO, this->yvStepO, 0);
				}
			}
			else
			{
				if (this->yvParamE.index == 0 || (this->yvParamE.length != dispHeight >> 1) || this->yvStepE != srcStoreWidth)
				{
					if (this->yvParamE.index)
					{
						MemFreeA(this->yvParamE.weight);
						MemFreeA(this->yvParamE.index);
						this->yvParamE.weight = 0;
						this->yvParamE.index = 0;
					}
					this->yvStepE = srcStoreWidth;
					SetupInterpolationParameter(dispHeight >> 2, dispHeight >> 1, this->yvParamE, this->yvStepE, YVADJ);
				}
			}

			i = this->nThread;
			isLast = 1;
			lastHeight = dispHeight >> 1;
			while (i-- > 0)
			{
				if (i == 0)
					isFirst = 1;
				currHeight = MulDivUOS(i, dispHeight >> 1, this->nThread);

				if (ftype == Media::FT_MERGED_TF)
				{
					stats[i].yPtr = yStart + (srcStoreWidth * currHeight << 1);
					stats[i].uPtr = uStart;
					stats[i].vPtr = vStart;
					stats[i].yvParam = this->yvParamO;
				}
				else
				{
					stats[i].yPtr = yStart + (srcStoreWidth * (currHeight << 1));
					stats[i].uPtr = uStart;// + (srcStoreWidth >> 1);
					stats[i].vPtr = vStart;// + (srcStoreWidth >> 1);
					stats[i].yvParam = this->yvParamE;
				}
				stats[i].yBpl = srcStoreWidth << 1;
				stats[i].uvBpl = currHeight;
				stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
				stats[i].isFirst = isFirst;
				stats[i].isLast = isLast;
				stats[i].ycOfst = ycOfst;
				isLast = 0;
				stats[i].width = dispWidth;
				stats[i].height = lastHeight - currHeight;
				stats[i].dbpl = destRGBBpl;

				if (stats[i].csLineSize < dispWidth)
				{
					if (stats[i].csLineBuff)
						MemFreeA64(stats[i].csLineBuff);
					if (stats[i].csLineBuff2)
						MemFreeA64(stats[i].csLineBuff2);
					stats[i].csLineSize = dispWidth;
					stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
					stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
				}
				stats[i].status = 11;
				stats[i].evt->Set();
				lastHeight = currHeight;
			}
			WaitForWorker(11);

		}
		else
		{
			lastHeight = dispHeight >> 1;
			while (i-- > 0)
			{
				if (i == 0)
					isFirst = 1;
				currHeight = MulDivUOS(i, dispHeight >> 1, nThread) & (UOSInt)~1;

				stats[i].yPtr = yStart + srcStoreWidth * (currHeight << 1);
				stats[i].yBpl = srcStoreWidth << 1;
				stats[i].vPtr = vStart + ((srcStoreWidth * currHeight) >> 1);
				stats[i].uPtr = uStart + ((srcStoreWidth * currHeight) >> 1);
				stats[i].uvBpl = srcStoreWidth;
				stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
				stats[i].isFirst = isFirst;
				stats[i].isLast = isLast;
				stats[i].ycOfst = ycOfst;
				isLast = 0;
				stats[i].width = dispWidth;
				stats[i].height = lastHeight - currHeight;
				stats[i].dbpl = destRGBBpl;

				if (stats[i].csLineSize < dispWidth)
				{
					if (stats[i].csLineBuff)
						MemFreeA64(stats[i].csLineBuff);
					if (stats[i].csLineBuff2)
						MemFreeA64(stats[i].csLineBuff2);
					stats[i].csLineSize = dispWidth;
					stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1) + 15);
					stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1) + 15);
				}
				stats[i].status = 3;
				stats[i].evt->Set();
				lastHeight = currHeight;
			}
			WaitForWorker(3);
		}
	}
	else if (ftype == Media::FT_INTERLACED_BFF || ftype == Media::FT_INTERLACED_TFF || ftype == Media::FT_INTERLACED_NODEINT)
	{
		UOSInt j;
		if (i & 1)
		{
			
		}
		else
		{
			if ((ycOfst == Media::YCOFST_C_CENTER_LEFT || ycOfst == Media::YCOFST_C_CENTER_CENTER) && (srcStoreWidth & 3) == 0)
			{
				UnsafeArray<UInt8> uStart = srcPtr[0] + srcStoreWidth * srcStoreHeight;
				UnsafeArray<UInt8> vStart = uStart + ((srcStoreWidth * srcStoreHeight) >> 2);

				if (this->yvParamO.index == 0 || this->yvParamO.length != (dispHeight >> 1) || this->yvStepO != srcStoreWidth)
				{
					if (this->yvParamO.index)
					{
						MemFreeA(this->yvParamO.weight);
						MemFreeA(this->yvParamO.index);
						this->yvParamO.weight = 0;
						this->yvParamO.index = 0;
					}
					this->yvStepO = srcStoreWidth;
					SetupInterpolationParameter(dispHeight >> 2, dispHeight >> 1, this->yvParamO, this->yvStepO, 0);
				}
				if (this->yvParamE.index == 0 || this->yvParamE.length != (dispHeight >> 1) || this->yvStepE != srcStoreWidth)
				{
					if (this->yvParamE.index)
					{
						MemFreeA(this->yvParamE.weight);
						MemFreeA(this->yvParamE.index);
						this->yvParamE.weight = 0;
						this->yvParamE.index = 0;
					}
					this->yvStepE = srcStoreWidth;
					SetupInterpolationParameter(dispHeight >> 2, dispHeight >> 1, this->yvParamE, this->yvStepE, YVADJ);
				}

				j = this->nThread >> 1;
				isLast = 1;
				isFirst = 0;
				lastHeight = dispHeight;
				while (i-- > j)
				{
					if (i == j)
						isFirst = 1;
					currHeight = MulDivUOS(i - j, dispHeight, j) & (UOSInt)~3;

					stats[i].yPtr = srcPtr[0] + srcStoreWidth * currHeight;
					stats[i].uPtr = uStart;
					stats[i].vPtr = vStart;
					stats[i].yBpl = srcStoreWidth << 1;
					stats[i].uvBpl = currHeight >> 1;
					stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
					stats[i].isFirst = isFirst;
					stats[i].isLast = isLast;
					stats[i].ycOfst = ycOfst;
					isLast = 0;
					stats[i].width = dispWidth;
					stats[i].height = (lastHeight - currHeight) >> 1;
					stats[i].dbpl = destRGBBpl << 1;
					stats[i].yvParam = this->yvParamO;

					if (stats[i].csLineSize < dispWidth)
					{
						if (stats[i].csLineBuff)
							MemFreeA64(stats[i].csLineBuff);
						if (stats[i].csLineBuff2)
							MemFreeA64(stats[i].csLineBuff2);
						stats[i].csLineSize = dispWidth;
						stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
						stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
					}
					stats[i].status = 11;
					stats[i].evt->Set();
					lastHeight = currHeight;
				}
				i = j;
				isLast = 1;
				isFirst = 0;
				lastHeight = dispHeight + 1;
				while (i-- > 0)
				{
					if (i == 0)
						isFirst = 1;
					currHeight = (MulDivUOS(i, dispHeight, j) & (UOSInt)~3) + 1;

					stats[i].yPtr = srcPtr[0] + srcStoreWidth * currHeight;
					stats[i].uPtr = uStart + (srcStoreWidth >> 1);
					stats[i].vPtr = vStart + (srcStoreWidth >> 1);
					stats[i].yBpl = srcStoreWidth << 1;
					stats[i].uvBpl = currHeight >> 1;
					stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
					stats[i].isFirst = isFirst;
					stats[i].isLast = isLast;
					stats[i].ycOfst = ycOfst;
					isLast = 0;
					stats[i].width = dispWidth;
					stats[i].height = (lastHeight - currHeight) >> 1;
					stats[i].dbpl = destRGBBpl << 1;
					stats[i].yvParam = this->yvParamE;

					if (stats[i].csLineSize < dispWidth)
					{
						if (stats[i].csLineBuff)
							MemFreeA64(stats[i].csLineBuff);
						if (stats[i].csLineBuff2)
							MemFreeA64(stats[i].csLineBuff2);
						stats[i].csLineSize = dispWidth;
						stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
						stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
					}
					stats[i].status = 11;
					stats[i].evt->Set();
					lastHeight = currHeight;
				}
				WaitForWorker(11);
			}
			else
			{
				j = this->nThread >> 1;

				isLast = 1;
				while (i-- > j)
				{
					if (i == j)
						isFirst = 1;
					currHeight = MulDivUOS(i - j, dispHeight, j) & (UOSInt)~3;

					stats[i].yPtr = srcPtr[0] + srcStoreWidth * currHeight;
					stats[i].yBpl = srcStoreWidth << 1;
					stats[i].uPtr = srcPtr[0] + srcStoreWidth * srcStoreHeight + ((srcStoreWidth >> 1) * (currHeight >> 1));
					stats[i].vPtr = stats[i].uPtr + ((srcStoreWidth * srcStoreHeight) >> 2);
					stats[i].uvBpl = srcStoreWidth;
					stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
					stats[i].isFirst = isFirst;
					stats[i].isLast = isLast;
					stats[i].ycOfst = ycOfst;
					isLast = 0;
					stats[i].width = dispWidth;
					stats[i].height = (lastHeight - currHeight) >> 1;
					stats[i].dbpl = destRGBBpl << 1;

					if (stats[i].csLineSize < dispWidth)
					{
						if (stats[i].csLineBuff)
							MemFreeA64(stats[i].csLineBuff);
						if (stats[i].csLineBuff2)
							MemFreeA64(stats[i].csLineBuff2);
						stats[i].csLineSize = dispWidth;
						stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
						stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
					}
					stats[i].status = 3;
					stats[i].evt->Set();
					lastHeight = currHeight;
				}

				i = j;
				isLast = 1;
				isFirst = 0;
				lastHeight = dispHeight + 1;
				while (i-- > 0)
				{
					if (i == 0)
						isFirst = 1;
					currHeight = (MulDivUOS(i, dispHeight, j) & (UOSInt)~3) + 1;

					stats[i].yPtr = srcPtr[0] + srcStoreWidth * currHeight;
					stats[i].yBpl = srcStoreWidth << 1;
					stats[i].vPtr = srcPtr[0] + srcStoreWidth * srcStoreHeight + ((srcStoreWidth >> 1) * ((currHeight >> 1) + 1));
					stats[i].uPtr = stats[i].vPtr + ((srcStoreWidth * srcStoreHeight) >> 2);
					stats[i].uvBpl = srcStoreWidth;
					stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
					stats[i].isFirst = isFirst;
					stats[i].isLast = isLast;
					stats[i].ycOfst = ycOfst;
					isLast = 0;
					stats[i].width = dispWidth;
					stats[i].height = (lastHeight - currHeight) >> 1;
					stats[i].dbpl = destRGBBpl << 1;

					if (stats[i].csLineSize < dispWidth)
					{
						if (stats[i].csLineBuff)
							MemFreeA64(stats[i].csLineBuff);
						if (stats[i].csLineBuff2)
							MemFreeA64(stats[i].csLineBuff2);
						stats[i].csLineSize = dispWidth;
						stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
						stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
					}
					stats[i].status = 3;
					stats[i].evt->Set();
					lastHeight = currHeight;
				}
				WaitForWorker(3);
			}
		}
	}
	else
	{
		if ((ycOfst == Media::YCOFST_C_CENTER_LEFT || ycOfst == Media::YCOFST_C_CENTER_CENTER))
		{
			UnsafeArray<UInt8> uStart = srcPtr[0] + srcStoreWidth * srcStoreHeight;
			UnsafeArray<UInt8> vStart = uStart + ((srcStoreWidth * srcStoreHeight) >> 2);

			if (this->yvParamO.index == 0 || this->yvParamO.length != dispHeight || this->yvStepO != (srcStoreWidth >> 1))
			{
				if (this->yvParamO.index)
				{
					MemFreeA(this->yvParamO.weight);
					MemFreeA(this->yvParamO.index);
					this->yvParamO.weight = 0;
					this->yvParamO.index = 0;
				}
				this->yvStepO = srcStoreWidth >> 1;
				SetupInterpolationParameter(dispHeight >> 1, dispHeight, this->yvParamO, this->yvStepO, 0);
			}

			i = this->nThread;
			isLast = 1;
			lastHeight = dispHeight;
			while (i-- > 0)
			{
				if (i == 0)
					isFirst = 1;
				currHeight = MulDivUOS(i, dispHeight, this->nThread);

				stats[i].yPtr = srcPtr[0] + srcStoreWidth * currHeight;
				stats[i].uPtr = uStart;
				stats[i].vPtr = vStart;
				stats[i].yBpl = srcStoreWidth;
				stats[i].uvBpl = currHeight;
				stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
				stats[i].isFirst = isFirst;
				stats[i].isLast = isLast;
				stats[i].ycOfst = ycOfst;
				isLast = 0;
				stats[i].width = dispWidth;
				stats[i].height = lastHeight - currHeight;
				stats[i].dbpl = destRGBBpl;
				stats[i].yvParam = this->yvParamO;

				if (stats[i].csLineSize < dispWidth)
				{
					if (stats[i].csLineBuff)
						MemFreeA64(stats[i].csLineBuff);
					if (stats[i].csLineBuff2)
						MemFreeA64(stats[i].csLineBuff2);
					stats[i].csLineSize = dispWidth;
					stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
					stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
				}
				stats[i].status = 11;
				stats[i].evt->Set();
				lastHeight = currHeight;
			}
			WaitForWorker(11);
		}
		else
		{
			while (i-- > 0)
			{
				if (i == 0)
					isFirst = 1;
				currHeight = MulDivUOS(i, dispHeight, nThread) & (UOSInt)~1;

				stats[i].yPtr = srcPtr[0] + srcStoreWidth * currHeight;
				stats[i].yBpl = srcStoreWidth;
				stats[i].uPtr = srcPtr[0] + srcStoreWidth * srcStoreHeight + ((srcStoreWidth * currHeight) >> 2);
				stats[i].vPtr = stats[i].uPtr + ((srcStoreWidth * srcStoreHeight) >> 2);
				stats[i].uvBpl = srcStoreWidth >> 1;
				stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
				stats[i].isFirst = isFirst;
				stats[i].isLast = isLast;
				stats[i].ycOfst = ycOfst;
				isLast = 0;
				stats[i].width = dispWidth;
				stats[i].height = lastHeight - currHeight;
				stats[i].dbpl = destRGBBpl;

				if (stats[i].csLineSize < dispWidth)
				{
					if (stats[i].csLineBuff)
						MemFreeA64(stats[i].csLineBuff);
					if (stats[i].csLineBuff2)
						MemFreeA64(stats[i].csLineBuff2);
					stats[i].csLineSize = dispWidth;
					stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
					stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
				}
				stats[i].status = 3;
				stats[i].evt->Set();
				lastHeight = currHeight;
			}
			WaitForWorker(3);
		}
	}
}
