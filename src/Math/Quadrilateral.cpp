#include "Stdafx.h"
#include "Math/Quadrilateral.h"

Math::Quadrilateral Math::Quadrilateral::FromPolygon(Math::Coord2D<UOSInt> *pg)
{
	UOSInt minX = pg[3].x;
	UOSInt minY = pg[3].y;
	UOSInt maxX = minX;
	UOSInt maxY = minY;
	UOSInt i = 3;
	while (i-- > 0)
	{
		if (pg[i].x < minX) minX = pg[i].x;
		if (pg[i].x > maxX) maxX = pg[i].x;
		if (pg[i].y < minY) minY = pg[i].y;
		if (pg[i].y > maxY) maxY = pg[i].y;
	}

	UOSInt diff;
	UOSInt tldiff = pg[3].x - minX + pg[3].y - minY;
	UOSInt tlIndex = 3;
	i = 3;
	while (i-- > 0)
	{
		diff = pg[i].x - minX + pg[i].y - minY;
		if (diff < tldiff)
		{
			tldiff = diff;
			tlIndex = i;
		}
	}
	Math::Quadrilateral quad;
	switch (tlIndex)
	{
	case 0:
		quad.tl.x = UOSInt2Double(pg[0].x);
		quad.tl.y = UOSInt2Double(pg[0].y);
		quad.br.x = UOSInt2Double(pg[2].x);
		quad.br.y = UOSInt2Double(pg[2].y);
		if (pg[1].x > pg[3].x)
		{
			quad.tr.x = UOSInt2Double(pg[1].x);
			quad.tr.y = UOSInt2Double(pg[1].y);
			quad.bl.x = UOSInt2Double(pg[3].x);
			quad.bl.y = UOSInt2Double(pg[3].y);
		}
		else
		{
			quad.tr.x = UOSInt2Double(pg[3].x);
			quad.tr.y = UOSInt2Double(pg[3].y);
			quad.bl.x = UOSInt2Double(pg[1].x);
			quad.bl.y = UOSInt2Double(pg[1].y);
		}
		break;
	case 1:
		quad.tl.x = UOSInt2Double(pg[1].x);
		quad.tl.y = UOSInt2Double(pg[1].y);
		quad.br.x = UOSInt2Double(pg[3].x);
		quad.br.y = UOSInt2Double(pg[3].y);
		if (pg[0].x > pg[2].x)
		{
			quad.tr.x = UOSInt2Double(pg[0].x);
			quad.tr.y = UOSInt2Double(pg[0].y);
			quad.bl.x = UOSInt2Double(pg[2].x);
			quad.bl.y = UOSInt2Double(pg[2].y);
		}
		else
		{
			quad.tr.x = UOSInt2Double(pg[2].x);
			quad.tr.y = UOSInt2Double(pg[2].y);
			quad.bl.x = UOSInt2Double(pg[0].x);
			quad.bl.y = UOSInt2Double(pg[0].y);
		}
		break;
	case 2:
		quad.tl.x = UOSInt2Double(pg[2].x);
		quad.tl.y = UOSInt2Double(pg[2].y);
		quad.br.x = UOSInt2Double(pg[0].x);
		quad.br.y = UOSInt2Double(pg[0].y);
		if (pg[1].x > pg[3].x)
		{
			quad.tr.x = UOSInt2Double(pg[1].x);
			quad.tr.y = UOSInt2Double(pg[1].y);
			quad.bl.x = UOSInt2Double(pg[3].x);
			quad.bl.y = UOSInt2Double(pg[3].y);
		}
		else
		{
			quad.tr.x = UOSInt2Double(pg[3].x);
			quad.tr.y = UOSInt2Double(pg[3].y);
			quad.bl.x = UOSInt2Double(pg[1].x);
			quad.bl.y = UOSInt2Double(pg[1].y);
		}
		break;
	default:
		quad.tl.x = UOSInt2Double(pg[3].x);
		quad.tl.y = UOSInt2Double(pg[3].y);
		quad.br.x = UOSInt2Double(pg[1].x);
		quad.br.y = UOSInt2Double(pg[1].y);
		if (pg[0].x > pg[2].x)
		{
			quad.tr.x = UOSInt2Double(pg[0].x);
			quad.tr.y = UOSInt2Double(pg[0].y);
			quad.bl.x = UOSInt2Double(pg[2].x);
			quad.bl.y = UOSInt2Double(pg[2].y);
		}
		else
		{
			quad.tr.x = UOSInt2Double(pg[2].x);
			quad.tr.y = UOSInt2Double(pg[2].y);
			quad.bl.x = UOSInt2Double(pg[0].x);
			quad.bl.y = UOSInt2Double(pg[0].y);
		}
		break;
	}
	return quad;
}