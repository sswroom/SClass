#include "Stdafx.h"

extern "C" void NearestNeighbourResizer32_32_Resize(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt dheight, OSInt dbpl, OSInt *xindex, OSInt *yindex)
{
	OSInt i;
	UInt8 *linePt;
	OSInt *xindexTmp;
	dbpl -= dwidth << 2;
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
