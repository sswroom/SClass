#include "Stdafx.h"
#include "MyMemory.h"

extern "C" void ImageCopy_ImgCopy(UInt8 *inPt, UInt8 *outPt, OSInt copySize, OSInt height, OSInt sstep, OSInt dstep)
{
	if (copySize == sstep && copySize == dstep)
	{
		MemCopyNANC(outPt, inPt, copySize * height);
	}
	else
	{
		while (height-- > 0)
		{
			MemCopyNO(outPt, inPt, copySize);
			inPt += sstep;
			outPt += dstep;
		}
	}
}

