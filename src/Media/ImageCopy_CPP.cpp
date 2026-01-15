#include "Stdafx.h"
#include "MyMemory.h"

extern "C" void ImageCopy_ImgCopy(UInt8 *inPt, UInt8 *outPt, UIntOS copySize, UIntOS height, IntOS sstep, IntOS dstep)
{
	if ((IntOS)copySize == sstep && (IntOS)copySize == dstep)
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

extern "C" void ImageCopy_ImgCopyR(UInt8 *inPt, UInt8 *outPt, UIntOS copySize, UIntOS height, UIntOS sbpl, UIntOS dbpl, Bool upsideDown)
{
	IntOS dstep;
	if (upsideDown)
	{
		dstep = -(IntOS)dbpl;
		outPt += dbpl * (height - 1);
	}
	else
	{
		dstep = (IntOS)dbpl;
	}
	ImageCopy_ImgCopy(inPt, outPt, copySize, height, (IntOS)sbpl, dstep);
}
