#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Math/Quadrilateral.h"
#include "Math/RectAreaDbl.h"
#include "Math/Triangle.h"

Math::Quadrilateral::Quadrilateral(Coord2DDbl tl, Coord2DDbl tr, Coord2DDbl br, Coord2DDbl bl)
{
	this->tl = tl;
	this->tr = tr;
	this->br = br;
	this->bl = bl;
}

Double Math::Quadrilateral::CalcMaxTiltAngle() const
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

Double Math::Quadrilateral::CalcArea() const
{
	return Math::Triangle(tl, tr, bl).CalcArea() + Math::Triangle(tr, bl, br).CalcArea();
}

Double Math::Quadrilateral::CalcLenLeft() const
{
	return tl.CalcLengTo(bl);
}

Double Math::Quadrilateral::CalcLenTop() const
{
	return tl.CalcLengTo(tr);
}

Double Math::Quadrilateral::CalcLenRight() const
{
	return tr.CalcLengTo(br);
}

Double Math::Quadrilateral::CalcLenBottom() const
{
	return br.CalcLengTo(bl);
}

Bool Math::Quadrilateral::InsideOrTouch(Math::Coord2DDbl pt) const
{
	Double d1 = Sign(pt, this->tl, this->tr);
	Double d2 = Sign(pt, this->tr, this->br);
	Double d3 = Sign(pt, this->br, this->bl);
	Double d4 = Sign(pt, this->bl, this->tl);
	if (d1 <= 0 && d2 <= 0 && d3 <= 0 && d4 <= 0)
	{
		return true;
	}
	if (d1 >= 0 && d2 >= 0 && d3 >= 0 && d4 >= 0)
	{
		return true;
	}
	return false;

/*	if (pt == tl || pt == tr || pt == bl || pt == br)
		return true;
	Double a1 = Math_ArcTan2(pt.y - tl.y, pt.x - tl.x);
	Double a2 = Math_ArcTan2(pt.y - tr.y, pt.x - tr.x);
	Double a3 = Math_ArcTan2(pt.y - br.y, pt.x - br.x);
	Double a4 = Math_ArcTan2(pt.y - bl.y, pt.x - bl.x);
	Double ta1 = a2 - a1;
	Double ta2 = a3 - a2;
	Double ta3 = a4 - a3;
	Double ta4 = a1 - a4;
	if (ta1 < 0) ta1 = -ta1;
	if (ta2 < 0) ta2 = -ta2;
	if (ta3 < 0) ta3 = -ta3;
	if (ta4 < 0) ta4 = -ta4;
	if (ta1 > Math::PI) ta1 = 2 * Math::PI - ta1;
	if (ta2 > Math::PI) ta2 = 2 * Math::PI - ta2;
	if (ta3 > Math::PI) ta3 = 2 * Math::PI - ta3;
	if (ta4 > Math::PI) ta4 = 2 * Math::PI - ta4;
	return Math::PI * 2 - 0.0000000001 <= ta1 + ta2 + ta3 + ta4;*/
}

Math::RectAreaDbl Math::Quadrilateral::GetExterior() const
{
	Math::Coord2DDbl min = this->tl.Min(this->tr).Min(this->br).Min(this->bl);
	Math::Coord2DDbl max = this->tl.Max(this->tr).Max(this->br).Max(this->bl);
	return Math::RectAreaDbl(min, max);
}

Bool Math::Quadrilateral::IsRectangle() const
{
	return this->tl.y == this->tr.y && this->tl.x == this->bl.x && this->bl.y == this->br.y && this->tr.x == this->br.x;
}

Bool Math::Quadrilateral::IsNonRotateRectangle() const
{
	return this->tl.y == this->tr.y && this->tl.x == this->bl.x && this->bl.y == this->br.y && this->tr.x == this->br.x && this->tl.x <= this->br.x && this->tl.y <= this->br.y;
}

UOSInt Math::Quadrilateral::CalcIntersactsAtY(UnsafeArray<Double> xArr, Double y) const
{
	UOSInt ret = 0;
	if (this->tr.y == y)
	{
		if (this->tl.y == y || this->br.y == y)
		{
			UOSInt eqCnt = 1;
			Double min = tr.x;
			Double max = tr.x;
			if (this->tl.y == y) { eqCnt++; if (min > tl.x) min = tl.x; if (max < tl.x) max = tl.x; }
			if (this->br.y == y) { eqCnt++; if (min > br.x) min = br.x; if (max < br.x) max = br.x; }
			if (this->bl.y == y) { eqCnt++; if (min > bl.x) min = bl.x; if (max < bl.x) max = bl.x; }
			xArr[ret] = min;
			xArr[ret + 1] = max;
			ret += 2;
			if (eqCnt == 2)
			{
				if (this->tl.y == y)
				{
					ret += CalcIntersactAtY(this->br, this->bl, y, &xArr[ret]);
				}
				else
				{
					ret += CalcIntersactAtY(this->bl, this->tl, y, &xArr[ret]);
				}
			}
			return ret;
		}
		else
		{
			xArr[ret] = tr.x;
			xArr[ret + 1] = tr.x;
		}
	}
	else if (this->bl.y == y)
	{
		if (this->tl.y == y || this->br.y == y)
		{
			UOSInt eqCnt = 1;
			Double min = bl.x;
			Double max = bl.x;
			if (this->tl.y == y) { eqCnt++; if (min > tl.x) min = tl.x; if (max < tl.x) max = tl.x; }
			if (this->br.y == y) { eqCnt++; if (min > br.x) min = br.x; if (max < br.x) max = br.x; }
			xArr[ret] = min;
			xArr[ret + 1] = max;
			ret += 2;
			if (eqCnt == 2)
			{
				if (this->tl.y == y)
				{
					ret += CalcIntersactAtY(this->tr, this->br, y, &xArr[ret]);
				}
				else
				{
					ret += CalcIntersactAtY(this->tl, this->tr, y, &xArr[ret]);
				}
			}
			return ret;
		}
		else
		{
			xArr[ret] = bl.x;
			xArr[ret + 1] = bl.x;
		}
	}
	ret += CalcIntersactAtY(this->tl, this->tr, y, &xArr[ret]);
	ret += CalcIntersactAtY(this->tr, this->br, y, &xArr[ret]);
	ret += CalcIntersactAtY(this->br, this->bl, y, &xArr[ret]);
	ret += CalcIntersactAtY(this->bl, this->tl, y, &xArr[ret]);
	return ret;
}

Double Math::Quadrilateral::Sign(Coord2DDbl p1, Coord2DDbl p2, Coord2DDbl p3)
{
	Coord2DDbl diff13 = p1 - p3;
	Coord2DDbl diff23 = p2 - p3;
	return diff13.x * diff23.y - diff23.x * diff13.y;
}

Math::Quadrilateral Math::Quadrilateral::FromPolygon(UnsafeArray<Math::Coord2D<UOSInt>> pg)
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

Math::Quadrilateral Math::Quadrilateral::FromPolygon(UnsafeArray<Math::Coord2DDbl> pg)
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

UOSInt Math::Quadrilateral::CalcIntersactAtY(Math::Coord2DDbl p1, Coord2DDbl p2, Double y, UnsafeArray<Double> xArr)
{
	if ((p1.y > y) ^ (p2.y > y))
	{
		Coord2DDbl diff = p1 - p2;
		xArr[0] = p1.x - diff.x * (p1.y - y) / diff.y;
		return 1;
	}
	return 0;
}
