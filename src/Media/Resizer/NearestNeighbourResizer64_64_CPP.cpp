#include "Stdafx.h"

extern "C" void NearestNeighbourResizer64_64_Resize(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt dheight, OSInt dbpl, OSInt *xindex, OSInt *yindex)
{
	UOSInt i;
	const UInt8 *linePt;
	OSInt *xindexTmp;
	dbpl -= (OSInt)dwidth << 3;
	while (dheight-- > 0)
	{
		linePt = inPt + *yindex;
		yindex++;
		xindexTmp = xindex;
		i = dwidth;
		while (i-- > 0)
		{
			*(Int64*)outPt = *(Int64*)&linePt[*xindexTmp++];
			outPt += 8;
		}
		outPt += dbpl;
	}
}
