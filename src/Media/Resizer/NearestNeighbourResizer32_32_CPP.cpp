#include "Stdafx.h"

extern "C" void NearestNeighbourResizer32_32_Resize(const UInt8 *inPt, UInt8 *outPt, UIntOS dwidth, UIntOS dheight, IntOS dbpl, IntOS *xindex, IntOS *yindex)
{
	UIntOS i;
	const UInt8 *linePt;
	IntOS *xindexTmp;
	dbpl -= (IntOS)dwidth << 2;
	while (dheight-- > 0)
	{
		linePt = inPt + *yindex;
		yindex++;
		xindexTmp = xindex;
		i = dwidth;
		while (i-- > 0)
		{
			*(Int32*)outPt = *(Int32*)&linePt[*xindexTmp++];
			outPt += 4;
		}
		outPt += dbpl;
	}
}
