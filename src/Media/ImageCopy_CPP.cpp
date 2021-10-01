#include "Stdafx.h"
#include "MyMemory.h"

extern "C" void ImageCopy_ImgCopy(UInt8 *inPt, UInt8 *outPt, UOSInt copySize, UOSInt height, OSInt sstep, OSInt dstep)
{
	if ((OSInt)copySize == sstep && (OSInt)copySize == dstep)
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

extern "C" void ImageCopy_ImgCopyR(UInt8 *inPt, UInt8 *outPt, UOSInt copySize, UOSInt height, UOSInt sbpl, UOSInt dbpl, Bool upsideDown)
{
	OSInt dstep;
	if (upsideDown)
	{
		dstep = -(OSInt)dbpl;
		outPt += dbpl * (height - 1);
	}
	else
	{
		dstep = (OSInt)dbpl;
	}
	ImageCopy_ImgCopy(inPt, outPt, copySize, height, (OSInt)sbpl, dstep);
}
