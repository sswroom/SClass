#include "Stdafx.h"
#include "Math/Math.h"
#include "Math/Quadrilateral.h"
#include "Math/Triangle.h"

Math::Quadrilateral::Quadrilateral(Coord2DDbl tl, Coord2DDbl tr, Coord2DDbl br, Coord2DDbl bl)
{
	this->tl = tl;
	this->tr = tr;
	this->br = br;
	this->bl = bl;
}

Double Math::Quadrilateral::CalcMaxTiltAngle()
{
	Double deg360 = Math::PI * 2;
	Double deg90 = Math::PI * 0.5;
	Double deg270 = Math::PI + deg90;
	Double dir[4];
	dir[0] = Math_ArcTan2(tl.y - tr.y, tl.x - tr.x);
	dir[1] = Math_ArcTan2(tr.y - br.y, tr.x - br.x);
	dir[2] = Math_ArcTan2(br.y - bl.y, br.x - bl.x);
	dir[3] = Math_ArcTan2(bl.y - tl.y, bl.x - tl.x);
	Double ang[4];
	ang[0] = dir[0] - dir[1];
	ang[1] = dir[1] - dir[2];
	ang[2] = dir[2] - dir[3];
	ang[3] = dir[3] - dir[0];
	Double maxTiltAngle = 0;
	Double tiltAngle;
	UOSInt i = 4;
	while (i-- > 0)
	{
		if (ang[i] < 0) ang[i] += deg360;
		if (ang[i] < Math::PI)
		{
			tiltAngle = ang[i] - deg90;
		}
		else
		{
			tiltAngle = ang[i] - deg270;
		}
		if (tiltAngle < 0)
		{
			tiltAngle = -tiltAngle;
		}
		if (tiltAngle > maxTiltAngle) maxTiltAngle = tiltAngle;
	}
	return maxTiltAngle;
}

Double Math::Quadrilateral::CalcArea()
{
	return Math::Triangle(tl, tr, bl).CalcArea() + Math::Triangle(tr, bl, br).CalcArea();
}

Double Math::Quadrilateral::CalcLenLeft()
{
	return tl.CalcLengTo(bl);
}

Double Math::Quadrilateral::CalcLenTop()
{
	return tl.CalcLengTo(tr);
}

Double Math::Quadrilateral::CalcLenRight()
{
	return tr.CalcLengTo(br);
}

Double Math::Quadrilateral::CalcLenBottom()
{
	return br.CalcLengTo(bl);
}

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

Math::Quadrilateral Math::Quadrilateral::FromPolygon(Math::Coord2DDbl *pg)
{
	Double minX = pg[3].x;
	Double minY = pg[3].y;
	Double maxX = minX;
	Double maxY = minY;
	UOSInt i = 3;
	while (i-- > 0)
	{
		if (pg[i].x < minX) minX = pg[i].x;
		if (pg[i].x > maxX) maxX = pg[i].x;
		if (pg[i].y < minY) minY = pg[i].y;
		if (pg[i].y > maxY) maxY = pg[i].y;
	}

	Double diff;
	Double tldiff = pg[3].x - minX + pg[3].y - minY;
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
		quad.tl = pg[0];
		quad.br = pg[2];
		if (pg[1].x > pg[3].x)
		{
			quad.tr = pg[1];
			quad.bl = pg[3];
		}
		else
		{
			quad.tr = pg[3];
			quad.bl = pg[1];
		}
		break;
	case 1:
		quad.tl = pg[1];
		quad.br = pg[3];
		if (pg[0].x > pg[2].x)
		{
			quad.tr = pg[0];
			quad.bl = pg[2];
		}
		else
		{
			quad.tr = pg[2];
			quad.bl = pg[0];
		}
		break;
	case 2:
		quad.tl = pg[2];
		quad.br = pg[0];
		if (pg[1].x > pg[3].x)
		{
			quad.tr = pg[1];
			quad.bl = pg[3];
		}
		else
		{
			quad.tr = pg[3];
			quad.bl = pg[1];
		}
		break;
	default:
		quad.tl = pg[3];
		quad.br = pg[1];
		if (pg[0].x > pg[2].x)
		{
			quad.tr = pg[0];
			quad.bl = pg[2];
		}
		else
		{
			quad.tr = pg[2];
			quad.bl = pg[0];
		}
		break;
	}
	return quad;
}
