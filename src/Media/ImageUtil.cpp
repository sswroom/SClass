#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Media/ImageUtil.h"

void Media::ImageUtil::DrawHLineNA32(UInt8 *pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, OSInt y, OSInt x1, OSInt x2, UInt32 col)
{
	if (y < 0 || y >= h || x1 >= (OSInt)w || x2 < 0)
	{
		return;
	}
	if (x1 < 0)
	{
		x1 = 0;
	}
	if (x2 >= (OSInt)w)
	{
		x2 = (OSInt)w;
	}
	if (x2 <= x1)
	{
		return;
	}
	UOSInt cnt = (UOSInt)(x2 - x1);
	pixelPtr = pixelPtr + (UOSInt)y * bpl + (UOSInt)x1 * 4;
	while (cnt-- > 0)
	{
		WriteInt32(pixelPtr, col);
		pixelPtr += 4;
	}
}

void Media::ImageUtil::DrawVLineNA32(UInt8 *pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, OSInt x, OSInt y1, OSInt y2, UInt32 col)
{
	if (x < 0 || x >= w || y1 >= (OSInt)h || y2 < 0)
	{
		return;
	}
	if (y1 < 0)
	{
		y1 = 0;
	}
	if (y2 >= (OSInt)h)
	{
		y2 = (OSInt)h;
	}
	if (y2 <= y1)
	{
		return;
	}
	UOSInt cnt = (UOSInt)(y2 - y1);
	pixelPtr = pixelPtr + (UOSInt)y1 * bpl + (UOSInt)x * 4;
	while (cnt-- > 0)
	{
		WriteInt32(pixelPtr, col);
		pixelPtr += bpl;
	}
}
