#include "Stdafx.h"
#include "Map/MapDrawUtil.h"
#include "Math/Geometry/CompoundCurve.h"

Bool Map::MapDrawUtil::DrawPoint(NotNullPtr<Math::Geometry::Point> pt, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Media::DrawPen *p, Math::Coord2DDbl ofst)
{
	Math::Coord2DDbl coord = view->MapXYToScnXY(pt->GetCenter()) + ofst;
	NotNullPtr<Media::DrawBrush> b2 = img->NewBrushARGB(0xffff0000);
	img->DrawRect(coord - 8, Math::Size2DDbl(17, 17), 0, b2);
	img->DelBrush(b2);
	return true;
}

Bool Map::MapDrawUtil::DrawLineString(NotNullPtr<Math::Geometry::LineString> pl, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Media::DrawPen *p, Math::Coord2DDbl ofst)
{
	UOSInt nPoint;
	Math::Coord2DDbl *points = pl->GetPointList(nPoint);
	Math::Coord2DDbl *dpoints = MemAllocA(Math::Coord2DDbl, nPoint);
	view->MapXYToScnXY(points, dpoints, nPoint, ofst);
	img->DrawPolyline(dpoints, nPoint, p);
	MemFreeA(dpoints);
	return true;
}

Bool Map::MapDrawUtil::DrawPolyline(NotNullPtr<Math::Geometry::Polyline> pl, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Media::DrawPen *p, Math::Coord2DDbl ofst)
{
	UOSInt nPoint;
	UOSInt nPtOfst;
	Math::Coord2DDbl *points = pl->GetPointList(nPoint);
	UInt32 *ptOfsts = pl->GetPtOfstList(nPtOfst);
	UOSInt i;
	Math::Coord2DDbl *dpoints = MemAllocA(Math::Coord2DDbl, nPoint);
	UOSInt lastCnt;
	UOSInt thisCnt;

	view->MapXYToScnXY(points, dpoints, nPoint, ofst);
	lastCnt = nPoint;
	i = nPtOfst;
	while (i-- > 0)
	{
		thisCnt = ptOfsts[i];
		img->DrawPolyline(&dpoints[thisCnt * 2], lastCnt - thisCnt, p);
		lastCnt = thisCnt;
	}
	MemFreeA(dpoints);
	return true;
}

Bool Map::MapDrawUtil::DrawPolygon(NotNullPtr<Math::Geometry::Polygon> pg, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Media::DrawPen *p, Math::Coord2DDbl ofst)
{
	UOSInt nPoint;
	UOSInt nPtOfst;
	Math::Coord2DDbl *points = pg->GetPointList(nPoint);
	UInt32 *ptOfsts = pg->GetPtOfstList(nPtOfst);
	Math::Coord2DDbl *dpoints = MemAllocA(Math::Coord2DDbl, nPoint);
	UInt32 *myPtCnts = MemAlloc(UInt32, nPtOfst);
	view->MapXYToScnXY(points, dpoints, nPoint, ofst);

	UOSInt i = nPtOfst;
	while (i-- > 0)
	{
		myPtCnts[i] = (UInt32)nPoint - ptOfsts[i];
		nPoint = ptOfsts[i];
	}

	img->DrawPolyPolygon(dpoints, myPtCnts, nPtOfst, p, b);
	MemFreeA(dpoints);
	MemFree(myPtCnts);
	return true;
}

Bool Map::MapDrawUtil::DrawMultiPolygon(NotNullPtr<Math::Geometry::MultiPolygon> mpg, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Media::DrawPen *p, Math::Coord2DDbl ofst)
{
	Bool succ = false;
	NotNullPtr<Math::Geometry::Polygon> pg;
	UOSInt i = mpg->GetCount();
	while (i-- > 0)
	{
		if (pg.Set(mpg->GetItem(i)))
		{
			succ = DrawPolygon(pg, img, view, b, p, ofst) || succ;
		}
	}
	return succ;
}

Bool Map::MapDrawUtil::DrawMultiSurface(NotNullPtr<Math::Geometry::MultiSurface> ms, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Media::DrawPen *p, Math::Coord2DDbl ofst)
{
	Bool succ = false;
	NotNullPtr<Math::Geometry::Vector2D> vec;
	UOSInt pgInd = ms->GetCount();
	while (pgInd-- > 0)
	{
		if (vec.Set(ms->GetItem(pgInd)))
		{
			succ = DrawVector(vec, img, view, b, p, ofst) || succ;
		}
	}
	return succ;
}

Bool Map::MapDrawUtil::DrawCurvePolygon(NotNullPtr<Math::Geometry::CurvePolygon> cp, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Media::DrawPen *p, Math::Coord2DDbl ofst)
{
	Data::ArrayList<UInt32> ptOfst;
	Data::ArrayListA<Math::Coord2DDbl> ptList;
	UOSInt nPoint;
	Math::Geometry::Vector2D *vec;
	UOSInt i = 0;
	UOSInt j = cp->GetCount();
	while (i < j)
	{
		vec = cp->GetItem(i);
		if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::CompoundCurve)
		{
			ptOfst.Add((UInt32)ptList.GetCount());
			((Math::Geometry::CompoundCurve*)vec)->GetDrawPoints(ptList);
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::LineString)
		{
			ptOfst.Add((UInt32)ptList.GetCount());
			const Math::Coord2DDbl *ptArr = ((Math::Geometry::LineString*)vec)->GetPointListRead(nPoint);
			ptList.AddRange(ptArr, nPoint);
		}
		else
		{
			printf("MapDrawUtil: DrawCurvePolygon unexpected type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()).v);
		}
		i++;
	}
	if (ptList.GetCount() > 0)
	{
		Math::Coord2DDbl *pointArr = ptList.GetPtr(nPoint);
		UOSInt nPtOfst;
		UInt32 *ptOfstArr = ptOfst.GetPtr(nPtOfst);
		Math::Coord2DDbl *dpoints = MemAllocA(Math::Coord2DDbl, nPoint);
		UOSInt k;
		UOSInt l;

		view->MapXYToScnXY(pointArr, dpoints, nPoint, ofst);
		k = nPtOfst;
		l = 1;
		while (l < k)
		{
			ptOfstArr[l - 1] = ptOfstArr[l] - ptOfstArr[l - 1];
			l++;
		}
		ptOfstArr[k - 1] = (UInt32)(nPoint - ptOfstArr[k - 1]);
		img->DrawPolyPolygon(dpoints, ptOfstArr, nPtOfst, p, b);
		MemFreeA(dpoints);
		return true;
	}
	return false;
}

Bool Map::MapDrawUtil::DrawGeometryCollection(NotNullPtr<Math::Geometry::GeometryCollection> geomColl, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Media::DrawPen *p, Math::Coord2DDbl ofst)
{
	Bool succ = false;
	NotNullPtr<Math::Geometry::Vector2D> vec;
	UOSInt pgInd = geomColl->GetCount();
	while (pgInd-- > 0)
	{
		if (vec.Set(geomColl->GetItem(pgInd)))
		{
			succ = DrawVector(vec, img, view, b, p, ofst) || succ;
		}
	}
	return succ;
}

Bool Map::MapDrawUtil::DrawEllipse(NotNullPtr<Math::Geometry::Ellipse> circle, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Media::DrawPen *p, Math::Coord2DDbl ofst)
{
	Math::Coord2DDbl bl = view->MapXYToScnXY(circle->GetTL());
	Math::Coord2DDbl tr = view->MapXYToScnXY(circle->GetBR());
	img->DrawEllipse(Math::Coord2DDbl(bl.x + ofst.x, tr.y + ofst.y), Math::Size2DDbl(tr.x - bl.x, bl.y - tr.y), p, b);
	return true;
}

Bool Map::MapDrawUtil::DrawVectorImage(NotNullPtr<Math::Geometry::VectorImage> vimg, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Media::DrawPen *p, Math::Coord2DDbl ofst)
{
	UInt32 nPoints;
	Math::Coord2DDbl pts[5];
	Math::RectAreaDbl bounds;
	
	if (vimg->IsScnCoord())
	{
		vimg->GetScreenBounds(img->GetWidth(), img->GetHeight(), img->GetHDPI(), img->GetVDPI(), &bounds.tl.x, &bounds.tl.y, &bounds.br.x, &bounds.br.y);
		pts[0] = bounds.tl + ofst;
		pts[1].x = bounds.tl.x + ofst.x;
		pts[1].y = bounds.br.y + ofst.y;
		pts[2] = bounds.br + ofst;
		pts[3].x = bounds.br.x + ofst.x;
		pts[3].y = bounds.tl.y + ofst.y;
		pts[4] = bounds.tl + ofst;
	}
	else
	{
		bounds = vimg->GetBounds();
		Math::Coord2DDbl pt1 = view->MapXYToScnXY(bounds.tl);
		Math::Coord2DDbl pt2 = view->MapXYToScnXY(bounds.br);
		pts[0] = pt1 + ofst;
		pts[1].x = pt1.x + ofst.x;
		pts[1].y = pt2.y + ofst.y;
		pts[2] = pt2 + ofst;
		pts[3].x = pt2.x + ofst.x;
		pts[3].y = pt1.y + ofst.y;
		pts[4] = pt1 + ofst;
	}
	nPoints = 5;
	img->DrawPolyPolygon(pts, &nPoints, 1, p, b);
	return true;
}

/*
Bool Map::MapDrawUtil::DrawPieArea(NotNullPtr<Math::Geometry::PieArea> pieArea, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Media::DrawPen *p, Math::Coord2DDbl ofst)
{
	BITMAPINFOHEADER bmih;
	bmih.biSize = sizeof(bmih);
	bmih.biWidth = (LONG)this->currWidth;
	bmih.biHeight = (LONG)this->currHeight;
	bmih.biPlanes = 1;
	bmih.biBitCount = 32;
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = 0;
	bmih.biXPelsPerMeter = 300;
	bmih.biYPelsPerMeter = 300;
	bmih.biClrUsed = 0;
	bmih.biClrImportant = 0;
	void *pbits;
	HDC hdcBmp = CreateCompatibleDC(hdc);
	HBITMAP hBmp = CreateDIBSection(hdcBmp, (BITMAPINFO*)&bmih, 0, &pbits, 0, 0);
	if (hBmp)
	{
		MemClear(pbits, this->currWidth * this->currHeight * 4);
		SelectObject(hdcBmp, hBmp);

		Double x1;
		Double y1;
		Double x2;
		Double y2;
		Double r;
		Double a1;
		Double a2;
		x1 = pie->GetCX();
		y1 = pie->GetCY();
		r = pie->GetR();
		x2 = x1 + r;
		y2 = y1 + r;
		x1 = x1 - r;
		y1 = y1 - r;
		view->MapXYToScnXY(x1, y1, &x1, &y1);
		view->MapXYToScnXY(x2, y2, &x2, &y2);
		r = (x1 - x2) / 2;
		if (r < 0)
		{
			r = -r;
		}
		a1 = pie->GetArcAngle1();
		a2 = pie->GetArcAngle2();
		Double cx = (x1 + x2) * 0.5 + xOfst;
		Double cy = (y1 + y2) * 0.5 + yOfst;

		HBRUSH hbr = CreateSolidBrush(0xffffff);
		SelectObject(hdcBmp, hbr);
		Pie(hdcBmp, Double2Int32(x1 + xOfst), Double2Int32(y1 + yOfst), Double2Int32(x2 + xOfst), Double2Int32(y2 + yOfst), Double2Int32(cx + r * Math_Sin(a2)), Double2Int32(cy - r * Math_Cos(a2)), Double2Int32(cx + r * Math_Sin(a1)), Double2Int32(cy - r * Math_Cos(a1)));
		DeleteObject(hbr);


		UInt32 *ptr = (UInt32*)pbits;
		OSInt cnt = this->currWidth * this->currHeight;
		while (cnt-- > 0)
		{
			if (*ptr)
			{
				*ptr = 0x403f0000;
			}
			ptr++;
		}
		BLENDFUNCTION bf;
		bf.BlendOp = 0;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 255;
		bf.AlphaFormat = AC_SRC_ALPHA;
		AlphaBlend(hdc, 0, 0, (int)this->currWidth, (int)this->currHeight, hdcBmp, 0, 0, (int)this->currWidth, (int)this->currHeight, bf);

		HPEN p = CreatePen(PS_SOLID, 3, 0x0000ff);
		HGDIOBJ lastPen = SelectObject(hdc, p);
		SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
		Pie(hdc, Double2Int32(x1 + xOfst), Double2Int32(y1 + yOfst), Double2Int32(x2 + xOfst), Double2Int32(y2 + yOfst), Double2Int32(cx + r * Math_Sin(a2)), Double2Int32(cy - r * Math_Cos(a2)), Double2Int32(cx + r * Math_Sin(a1)), Double2Int32(cy - r * Math_Cos(a1)));
		SelectObject(hdc, lastPen);
		DeleteObject(p);

		DeleteObject(hBmp);
	}
	DeleteDC(hdcBmp);
}*/

Bool Map::MapDrawUtil::DrawVector(NotNullPtr<Math::Geometry::Vector2D> vec, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Media::DrawPen *p, Math::Coord2DDbl ofst)
{
	switch (vec->GetVectorType())
	{
	case Math::Geometry::Vector2D::VectorType::Point:
		return DrawPoint(NotNullPtr<Math::Geometry::Point>::ConvertFrom(vec), img, view, b, p, ofst);
	case Math::Geometry::Vector2D::VectorType::Polyline:
		return DrawPolyline(NotNullPtr<Math::Geometry::Polyline>::ConvertFrom(vec), img, view, b, p, ofst);
	case Math::Geometry::Vector2D::VectorType::LineString:
		return DrawLineString(NotNullPtr<Math::Geometry::LineString>::ConvertFrom(vec), img, view, b, p, ofst);
	case Math::Geometry::Vector2D::VectorType::Polygon:
		return DrawPolygon(NotNullPtr<Math::Geometry::Polygon>::ConvertFrom(vec), img, view, b, p, ofst);
	case Math::Geometry::Vector2D::VectorType::MultiPolygon:
		return DrawMultiPolygon(NotNullPtr<Math::Geometry::MultiPolygon>::ConvertFrom(vec), img, view, b, p, ofst);
	case Math::Geometry::Vector2D::VectorType::GeometryCollection:
		return DrawGeometryCollection(NotNullPtr<Math::Geometry::GeometryCollection>::ConvertFrom(vec), img, view, b, p, ofst);
	case Math::Geometry::Vector2D::VectorType::MultiSurface:
		return DrawMultiSurface(NotNullPtr<Math::Geometry::MultiSurface>::ConvertFrom(vec), img, view, b, p, ofst);
	case Math::Geometry::Vector2D::VectorType::CurvePolygon:
		return DrawCurvePolygon(NotNullPtr<Math::Geometry::CurvePolygon>::ConvertFrom(vec), img, view, b, p, ofst);
	case Math::Geometry::Vector2D::VectorType::Ellipse:
		return DrawEllipse(NotNullPtr<Math::Geometry::Ellipse>::ConvertFrom(vec), img, view, b, p, ofst);
	case Math::Geometry::Vector2D::VectorType::Image:
		return DrawVectorImage(NotNullPtr<Math::Geometry::VectorImage>::ConvertFrom(vec), img, view, b, p, ofst);
	case Math::Geometry::Vector2D::VectorType::MultiPoint:
	case Math::Geometry::Vector2D::VectorType::CircularString:
	case Math::Geometry::Vector2D::VectorType::CompoundCurve:
	case Math::Geometry::Vector2D::VectorType::MultiCurve:
	case Math::Geometry::Vector2D::VectorType::Curve:
	case Math::Geometry::Vector2D::VectorType::Surface:
	case Math::Geometry::Vector2D::VectorType::PolyhedralSurface:
	case Math::Geometry::Vector2D::VectorType::Tin:
	case Math::Geometry::Vector2D::VectorType::Triangle:
	case Math::Geometry::Vector2D::VectorType::String:
	case Math::Geometry::Vector2D::VectorType::PieArea:
	case Math::Geometry::Vector2D::VectorType::Unknown:
	default:
		printf("MapDrawUtil: unsupported type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()).v);
		return false;
	}
}