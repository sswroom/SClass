#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Math/Geometry/Ellipse.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/PieArea.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIMapControl.h"
#include "UI/MessageDialog.h"

#include <stdio.h>

void __stdcall UI::GUIMapControl::ImageUpdated(void *userObj)
{
	UI::GUIMapControl *me = (UI::GUIMapControl*)userObj;
	me->bgUpdated = true;
	if (!me->pauseUpdate)
	{
		me->Redraw();
	}
}

Bool UI::GUIMapControl::OnMouseDown(Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		this->SetCapture();
		if (this->mouseDownHdlr)
		{
			Bool done = this->mouseDownHdlr(this->mouseDownObj, scnPos, btn);
			if (done)
			{
				return true;
			}
		}
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		Int64 currTime = dt.ToTicks();
		if (currTime - this->mouseLDownTime < 250)
		{
			this->OnMouseWheel(scnPos, 1);
			this->mouseLDownTime = 0;
			return true;
		}
		else
		{
			this->mouseLDownTime = currTime;
		}	
		this->mouseCurrPos = this->mouseDownPos = scnPos;
		this->mouseDown = true;
		this->Focus();
	}
	else if (btn == UI::GUIControl::MBTN_RIGHT)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		Int64 currTime = dt.ToTicks();
		if (currTime - this->mouseRDownTime < 250)
		{
			this->OnMouseWheel(scnPos, -1);
			this->mouseRDownTime = 0;
			return true;
		}
		else
		{
			this->mouseRDownTime = currTime;
		}	
	}
	return false;
}

Bool UI::GUIMapControl::OnMouseUp(Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		this->ReleaseCapture();
		if (this->mouseUpHdlr)
		{
			Bool done = this->mouseUpHdlr(this->mouseUpObj, scnPos, btn);
			if (done)
			{
				return true;
			}
		}
		if (this->mouseDown)
		{
			Math::Coord2D<OSInt> diff = this->mouseCurrPos - this->mouseDownPos;
			Math::Coord2DDbl newCent = this->currSize.ToDouble() * 0.5 - diff.ToDouble();
			Math::Coord2DDbl mapPos = this->view->ScnXYToMapXY(newCent);
			this->view->SetCenterXY(mapPos);
			this->UpdateMap();
			this->mouseDown = false;
		}
		this->Redraw();
	}
	return false;
}

void UI::GUIMapControl::OnMouseMove(Math::Coord2D<OSInt> scnPos)
{
	if (this->mouseDown)
	{
		this->mouseCurrPos = scnPos;
		this->Redraw();
	}
	else
	{
		UOSInt i = this->mouseMoveHdlrs.GetCount();
		while (i-- > 0)
		{
			this->mouseMoveHdlrs.GetItem(i)(this->mouseMoveObjs.GetItem(i), scnPos);
		}
	}
/*	if (me->IsFormFocused())
	{
		SetFocus((HWND)hWnd);
	}*/
}

Bool UI::GUIMapControl::OnMouseWheel(Math::Coord2D<OSInt> scnPos, Int32 delta)
{
	Math::Coord2DDbl newPt;
	Math::Coord2DDbl pt = this->view->ScnXYToMapXY(scnPos.ToDouble());
	Double newScale;
	if (delta < 0)
	{
		newScale = this->view->GetMapScale() * 2.0;
	}
	else
	{
		newScale = this->view->GetMapScale() * 0.5;
	}
	NotNullPtr<Map::MapEnv> mapEnv;
	if (!mapEnv.Set(this->mapEnv))
		mapEnv = this->renderer->GetEnv();
	if (newScale < mapEnv->GetMinScale())
		newScale = mapEnv->GetMinScale();
	else if (newScale > mapEnv->GetMaxScale())
		newScale = mapEnv->GetMaxScale();
	this->view->SetMapScale(newScale);
	newPt = this->view->MapXYToScnXY(pt);
	pt = this->view->ScnXYToMapXY(newPt - scnPos.ToDouble() + (this->currSize.ToDouble() * 0.5));
	this->view->SetCenterXY(pt);
	this->EventScaleChanged(this->view->GetMapScale());
	this->UpdateMap();
	this->Redraw();
	return true;
}

void UI::GUIMapControl::OnGestureBegin(Math::Coord2D<OSInt> scnPos, UInt64 dist)
{
	this->gZoom = true;
	this->gZoomPos = scnPos;
	this->gZoomCurrPos = this->gZoomPos;
	this->gZoomDist = dist;
	this->gZoomCurrDist = dist;
}

void UI::GUIMapControl::OnGestureStep(Math::Coord2D<OSInt> scnPos, UInt64 dist)
{
	if (this->gZoom)
	{
		this->gZoomCurrDist = dist;
		this->gZoomCurrPos = scnPos;
		this->Redraw();
	}
}

void UI::GUIMapControl::OnGestureEnd(Math::Coord2D<OSInt> scnPos, UInt64 dist)
{
	if (this->gZoom)
	{
		Math::Coord2DDbl pt;
		Math::Coord2DDbl newPt;

		this->gZoomCurrPos = scnPos;
		this->gZoom = false;
		pt = this->view->ScnXYToMapXY(this->gZoomPos.ToDouble());
		Double newScale = this->view->GetMapScale() * (Double)this->gZoomDist /(Double)dist;
		NotNullPtr<Map::MapEnv> mapEnv;
		if (!mapEnv.Set(this->mapEnv))
			mapEnv = this->renderer->GetEnv();
		if (newScale < mapEnv->GetMinScale())
			newScale = mapEnv->GetMinScale();
		else if (newScale > mapEnv->GetMaxScale())
			newScale = mapEnv->GetMaxScale();
		this->view->SetMapScale(newScale);
		newPt = this->view->MapXYToScnXY(pt);
		pt = this->view->ScnXYToMapXY(newPt - this->gZoomCurrPos.ToDouble() + (this->currSize.ToDouble() * 0.5));
		this->view->SetCenterXY(pt);
		this->EventScaleChanged(this->view->GetMapScale());
		this->UpdateMap();
		this->Redraw();
	}
}

void UI::GUIMapControl::OnJSButtonDown(OSInt buttonId)
{
	if (buttonId == 3)
	{
		this->view->SetMapScale(this->view->GetMapScale() * 2.0);
		this->EventScaleChanged(this->view->GetMapScale());
		this->UpdateMap();
		this->Redraw();
	}
	else if (buttonId == 4)
	{
		this->view->SetMapScale(this->view->GetMapScale() * 0.5);
		this->EventScaleChanged(this->view->GetMapScale());
		this->UpdateMap();
		this->Redraw();
	}
}

void UI::GUIMapControl::OnJSButtonUp(OSInt buttonId)
{
}

void UI::GUIMapControl::OnJSAxis(OSInt axis1, OSInt axis2, OSInt axis3, OSInt axis4)
{
	axis1 >>= 10;
	axis2 >>= 10;
	if (axis1 < 0)
		axis1++;
	if (axis2 < 0)
		axis2++;
	if (axis1 != 0 || axis2 != 0)
	{
		Math::Coord2D<OSInt> diff = Math::Coord2D<OSInt>(axis1, axis2);
		Math::Coord2DDbl newCent = this->currSize.ToDouble() * 0.5 - diff.ToDouble();
		Math::Coord2DDbl mapPos = this->view->ScnXYToMapXY(newCent);
		this->view->SetCenterXY(mapPos);
		this->UpdateMap();
		this->Redraw();
	}
}

void UI::GUIMapControl::OnTimerTick()
{
	if (this->imgTimeoutTick != 0)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.ToTicks() >= this->imgTimeoutTick)
		{
			this->imgTimeoutTick = 0;
			this->bgUpdated = true;
			if (!this->pauseUpdate)
			{
				this->Redraw();
			}
		}
	}
}

void UI::GUIMapControl::OnDraw(NotNullPtr<Media::DrawImage> img)
{
	Math::Coord2DDbl tl;
	NotNullPtr<Media::DrawImage> bgImg;
	if (!bgImg.Set(this->bgImg))
	{
		return;
	}
	if (this->mouseDown)
	{
		tl = (this->mouseCurrPos - this->mouseDownPos).ToDouble();
	}
	else
	{
		tl = Math::Coord2DDbl(0, 0);
	}
	if (this->bgUpdated)
	{
		this->bgUpdated = false;
		this->UpdateMap();
	}

	if (this->gZoom)
	{
		NotNullPtr<Media::StaticImage> drawImg;
		Media::StaticImage *srcImg;
		Media::Resizer::LanczosResizerH8_8 resizer(4, 3, Media::AT_NO_ALPHA);
		Math::Size2D<UOSInt> sz;

		Sync::MutexUsage mutUsage(this->drawMut);
		if (this->drawHdlr)
		{
			NotNullPtr<Media::DrawImage> tmpImg;
			if (tmpImg.Set(this->eng->CloneImage(bgImg)))
			{
				this->drawHdlr(this->drawHdlrObj, tmpImg, 0, 0);
				srcImg = tmpImg->ToStaticImage();
				this->eng->DeleteImage(tmpImg);
			}
			else
			{
				srcImg = bgImg->ToStaticImage();
			}
		}
		else
		{
			srcImg = bgImg->ToStaticImage();
		}
		
		if (this->gZoomCurrDist > this->gZoomDist)
		{
			tl = Math::Coord2DDbl(0, 0);
			sz = this->currSize;
			NEW_CLASSNN(drawImg, Media::StaticImage(this->currSize, 0, 32, Media::PF_B8G8R8A8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			Double rate = (Double)this->gZoomDist / (Double)this->gZoomCurrDist;
			Math::Size2DDbl srcSize = this->currSize.ToDouble() * rate;
			Math::Coord2DDbl srcPos = this->gZoomPos.ToDouble() - this->gZoomCurrPos.ToDouble() * rate;
			drawImg->info.hdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
			drawImg->info.vdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
			drawImg->info.color.Set(this->colorSess->GetRGBParam()->monProfile);

			if (srcPos.x < 0)
			{
				tl.x = -srcPos.x / rate;
				srcPos.x = 0;
			}
			else if (srcPos.x + srcSize.x > UOSInt2Double(this->currSize.x))
			{
				tl.x = (UOSInt2Double(this->currSize.x) - srcSize.x - srcPos.x) / rate;
				srcPos.x = UOSInt2Double(this->currSize.x) - srcSize.x;
			}
			if (srcPos.y < 0)
			{
				tl.y = -srcPos.y / rate;
				srcPos.y = 0;
			}
			else if (srcPos.y + srcSize.y > UOSInt2Double(this->currSize.y))
			{
				tl.y = (UOSInt2Double(this->currSize.y) - srcSize.y - srcPos.y) / rate;
				srcPos.y = UOSInt2Double(this->currSize.y) - srcSize.y;
			}
			Int32 srcIX = (Int32)srcPos.x;
			Int32 srcIY = (Int32)srcPos.y;
			resizer.Resize((srcIX * 4) + (srcIY * (OSInt)(srcImg->info.storeSize.x * 4)) + srcImg->data, (OSInt)srcImg->info.storeSize.x * 4, srcSize.x, srcSize.y, srcPos.x - srcIX, srcPos.y - srcIY, drawImg->data, (OSInt)drawImg->info.storeSize.x * 4, sz.x, sz.y);
		}
		else
		{
			Double rate = (Double)this->gZoomCurrDist / (Double)this->gZoomDist;
			sz.x = (UOSInt)Double2OSInt(UOSInt2Double(this->currSize.x) * rate);
			sz.y = (UOSInt)Double2OSInt(UOSInt2Double(this->currSize.y) * rate);
			tl = this->gZoomCurrPos.ToDouble() - this->gZoomPos.ToDouble() * rate;
			NEW_CLASSNN(drawImg, Media::StaticImage(this->currSize, 0, 32, Media::PF_B8G8R8A8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			drawImg->info.hdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
			drawImg->info.vdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
			drawImg->info.color.Set(this->colorSess->GetRGBParam()->monProfile);
			resizer.Resize(srcImg->data, (OSInt)srcImg->info.storeSize.x * 4, UOSInt2Double(this->currSize.x), UOSInt2Double(this->currSize.y), 0, 0, drawImg->data, (OSInt)drawImg->info.storeSize.x * 4, sz.x, sz.y);
		}
		mutUsage.EndUse();
		DEL_CLASS(srcImg);

		img->DrawImagePt2(drawImg, tl);
		drawImg.Delete();


		NotNullPtr<Media::DrawBrush> bgBrush = img->NewBrushARGB(this->bgColor);
		if (tl.x > 0)
		{
			img->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(tl.x, UOSInt2Double(this->currSize.y)), 0, bgBrush);
		}
		if (tl.x + UOSInt2Double(sz.x) < UOSInt2Double(this->currSize.x))
		{
			img->DrawRect(Math::Coord2DDbl(UOSInt2Double(sz.x) + tl.x, 0), Math::Size2DDbl(UOSInt2Double(this->currSize.x - sz.x) - tl.x, UOSInt2Double(this->currSize.y)), 0, bgBrush);
		}
		if (tl.y > 0)
		{
			img->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(UOSInt2Double(this->currSize.x), tl.y), 0, bgBrush);
		}
		if (tl.y + UOSInt2Double(sz.y) < UOSInt2Double(this->currSize.y))
		{
			img->DrawRect(Math::Coord2DDbl(0, UOSInt2Double(sz.y) + tl.y), Math::Size2DDbl(UOSInt2Double(this->currSize.x), UOSInt2Double(this->currSize.y - sz.y) - tl.y), 0, bgBrush);
		}
		img->DelBrush(bgBrush);
	}
	else
	{
		NotNullPtr<Media::DrawBrush> bgBrush = img->NewBrushARGB(this->bgColor);
		if (tl.x > 0)
		{
			img->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(tl.x, UOSInt2Double(this->currSize.y)), 0, bgBrush);
		}
		else if (tl.x < 0)
		{
			img->DrawRect(Math::Coord2DDbl(UOSInt2Double(this->currSize.x) + tl.x, 0), Math::Size2DDbl(-tl.x, UOSInt2Double(this->currSize.y)), 0, bgBrush);
		}
		if (tl.y > 0)
		{
			img->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(UOSInt2Double(this->currSize.x), tl.y), 0, bgBrush);
		}
		else if (tl.y < 0)
		{
			img->DrawRect(Math::Coord2DDbl(0, UOSInt2Double(this->currSize.y) + tl.y), Math::Size2DDbl(UOSInt2Double(this->currSize.x), -tl.y), 0, bgBrush);
		}
		img->DelBrush(bgBrush);

		Sync::MutexUsage mutUsage(this->drawMut);
		if (this->drawHdlr)
		{
			NotNullPtr<Media::DrawImage> drawImg;
			if (drawImg.Set(this->eng->CloneImage(bgImg)))
			{
				this->drawHdlr(this->drawHdlrObj, drawImg, 0, 0);
				this->DrawScnObjects(drawImg, Math::Coord2DDbl(0, 0));
				img->DrawImagePt(drawImg, tl);
				this->eng->DeleteImage(drawImg);
			}
		}
		else
		{
			img->DrawImagePt(bgImg, tl);
			this->DrawScnObjects(img, tl);
		}
		mutUsage.EndUse();
	}
}

void UI::GUIMapControl::DrawScnObjects(NotNullPtr<Media::DrawImage> img, Math::Coord2DDbl ofst)
{
	Double hdpi = this->view->GetHDPI();
	Double ddpi = this->view->GetDDPI();
	if (this->showMarker)
	{
		Math::Coord2DDbl scnPos;
		Int32 x;
		Int32 y;
		scnPos = this->view->MapXYToScnXY(this->markerPos);
		x = Double2Int32(scnPos.x + ofst.x);
		y = Double2Int32(scnPos.y + ofst.y);
		Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3 * hdpi / ddpi, 0, 0);
		if (this->markerHasDir)
		{
			Double ptAdd = 8 * hdpi / ddpi;
			Double dirAdj = Math::PI * -0.5;
			Double dir2 = Math::PI * 0.75;
			Double dir1 = this->markerDir + dir2 + dirAdj;
			dir2 = this->markerDir - dir2 + dirAdj;
			img->DrawLine(x + ptAdd * Math_Cos(dir1), y + ptAdd * Math_Sin(dir1), x, y, p);
			img->DrawLine(x + ptAdd * Math_Cos(dir2), y + ptAdd * Math_Sin(dir2), x, y, p);
		}
		else
		{
			Double ptAdd = 5 * hdpi / ddpi;
			img->DrawLine(x - ptAdd, y - ptAdd, x + ptAdd, y + ptAdd, p);
			img->DrawLine(x - ptAdd, y + ptAdd, x + ptAdd, y - ptAdd, p);
		}
		img->DelPen(p);
	}
	UOSInt i = 0;
	UOSInt j = this->selVecList.GetCount();
	while (i < j)
	{
		Math::Geometry::Vector2D *vec = this->selVecList.GetItem(i);
		Math::Geometry::Vector2D::VectorType vecType = vec->GetVectorType();
		if (vecType == Math::Geometry::Vector2D::VectorType::LineString)
		{
			Math::Geometry::LineString *pl = (Math::Geometry::LineString*)vec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			UOSInt nPoint;
			Math::Coord2DDbl *points = pl->GetPointList(nPoint);
			Math::Coord2DDbl *dpoints = MemAllocA(Math::Coord2DDbl, nPoint);
			view->MapXYToScnXY(points, dpoints, nPoint, ofst);
			img->DrawPolyline(dpoints, nPoint, p);
			MemFreeA(dpoints);
			img->DelPen(p);
		}
		else if (vecType == Math::Geometry::Vector2D::VectorType::Polyline)
		{
			Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
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

			img->DelPen(p);
		}
		else if (vecType == Math::Geometry::Vector2D::VectorType::Polygon)
		{
			Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			NotNullPtr<Media::DrawBrush> b = img->NewBrushARGB(0x403f0000);
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
			img->DelPen(p);
			img->DelBrush(b);
		}
		else if (vecType == Math::Geometry::Vector2D::VectorType::MultiPolygon)
		{
			Math::Geometry::MultiPolygon *mpg = (Math::Geometry::MultiPolygon*)vec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			NotNullPtr<Media::DrawBrush> b = img->NewBrushARGB(0x403f0000);
			UOSInt npg = mpg->GetCount();
			UOSInt nPoint;
			UOSInt nPtOfst;
			while (npg-- > 0)
			{
				Math::Geometry::Polygon *pg = mpg->GetItem(npg);
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
			}
			img->DelPen(p);
			img->DelBrush(b);
		}
		else if (vecType == Math::Geometry::Vector2D::VectorType::Ellipse)
		{
			Math::Geometry::Ellipse *circle = (Math::Geometry::Ellipse*)vec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			NotNullPtr<Media::DrawBrush> b = img->NewBrushARGB(0x403f0000);
			Math::Coord2DDbl bl = view->MapXYToScnXY(circle->GetTL());
			Math::Coord2DDbl tr = view->MapXYToScnXY(circle->GetBR());
			img->DrawEllipse(Math::Coord2DDbl(bl.x + ofst.x, tr.y + ofst.y), Math::Size2DDbl(tr.x - bl.x, bl.y - tr.y), p, b);
			img->DelPen(p);
			img->DelBrush(b);
		}
		else if (vecType == Math::Geometry::Vector2D::VectorType::PieArea)
		{
//			Math::PieArea *pie = (Math::PieArea*)this->selVec;
/*			BITMAPINFOHEADER bmih;
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
			DeleteDC(hdcBmp);*/
		}
		else if (vecType == Math::Geometry::Vector2D::VectorType::Image)
		{
			Math::Geometry::VectorImage *vimg = (Math::Geometry::VectorImage*)vec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			NotNullPtr<Media::DrawBrush> b = img->NewBrushARGB(0x403f0000);
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
			img->DelPen(p);
			img->DelBrush(b);
		}
		else if (vecType == Math::Geometry::Vector2D::VectorType::Point)
		{
			Math::Geometry::Point *pt = (Math::Geometry::Point*)vec;
			Math::Coord2DDbl coord = view->MapXYToScnXY(pt->GetCenter());
			NotNullPtr<Media::DrawBrush> b = img->NewBrushARGB(0xffff0000);
			img->DrawRect(coord - 8, Math::Size2DDbl(17, 17), 0, b);
			img->DelBrush(b);
		}
		i++;
	}
}

void UI::GUIMapControl::ReleaseSelVecList()
{
	UOSInt i = this->selVecList.GetCount();
	Math::Geometry::Vector2D *vec;
	while (i-- > 0)
	{
		vec = this->selVecList.GetItem(i);
		DEL_CLASS(vec);
	}
	this->selVecList.Clear();
}

UI::GUIMapControl::GUIMapControl(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, UInt32 bgColor, Map::DrawMapRenderer *renderer, NotNullPtr<Map::MapView> view, NotNullPtr<Media::ColorManagerSess> colorSess) : UI::GUICustomDraw(ui, parent, eng)
{
	this->colorSess = colorSess;
	this->colorSess->AddHandler(*this);
	this->currSize = Math::Size2D<UOSInt>(640, 480);
	this->bgImg = 0;
	this->mouseDown = false;
	this->mouseCurrPos = Math::Coord2D<OSInt>(0, 0);
	this->mouseDownPos = Math::Coord2D<OSInt>(0, 0);
	this->mouseLDownTime = 0;
	this->mouseRDownTime = 0;
	this->gZoom = false;
	this->bgColor = bgColor;
	this->bgUpdated = false;
	this->mouseDownHdlr = 0;
	this->mouseUpHdlr = 0;
	this->drawHdlr = 0;
	this->drawHdlrObj = 0;
	this->mapEnv = 0;
	this->imgTimeoutTick = 0;

	this->markerPos = Math::Coord2DDbl(0, 0);
	this->showMarker = false;
	this->renderer = renderer;
	this->pauseUpdate = false;
	this->releaseRenderer = false;

	this->view = view;
	view->SetDPI(this->hdpi, this->ddpi);
	view->ChangeViewXY(this->currSize.ToDouble(), Math::Coord2DDbl(114.2, 22.4), 10000);
	this->SetBGColor(bgColor);
	if (this->renderer)
	{
		this->renderer->SetUpdatedHandler(ImageUpdated, this);
	}
}

UI::GUIMapControl::GUIMapControl(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, NotNullPtr<Map::MapEnv> mapEnv, NotNullPtr<Media::ColorManagerSess> colorSess) : UI::GUICustomDraw(ui, parent, eng)
{
	this->colorSess = colorSess;
	this->colorSess->AddHandler(*this);
	this->currSize = Math::Size2D<UOSInt>(640, 480);
	this->bgImg = 0;
	this->mouseDown = false;
	this->mouseCurrPos = Math::Coord2D<OSInt>(0, 0);
	this->mouseDownPos = Math::Coord2D<OSInt>(0, 0);
	this->mouseLDownTime = 0;
	this->mouseRDownTime = 0;
	this->gZoom = false;
	this->bgColor = mapEnv->GetBGColor();
	this->bgUpdated = false;
	this->mouseDownHdlr = 0;
	this->mouseUpHdlr = 0;
	this->drawHdlr = 0;
	this->drawHdlrObj = 0;
	this->mapEnv = mapEnv.Ptr();
	this->imgTimeoutTick = 0;

	this->markerPos = Math::Coord2DDbl(0, 0);
	this->showMarker = false;
	this->pauseUpdate = false;
	Media::ColorProfile color(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->renderer, Map::DrawMapRenderer(this->eng, mapEnv, color, this->colorSess.Ptr(), Map::DrawMapRenderer::DT_PIXELDRAW));
	this->releaseRenderer = true;

	this->view = mapEnv->CreateMapView(Math::Size2DDbl(640, 480));
	view->SetDPI(this->hdpi, this->ddpi);
	view->ChangeViewXY(this->currSize.ToDouble(), Math::Coord2DDbl(114.2, 22.4), 10000);

	this->SetBGColor(bgColor);
	if (this->renderer)
	{
		this->renderer->SetUpdatedHandler(ImageUpdated, this);
	}
	this->mapEnv->AddUpdatedHandler(ImageUpdated, this);
}

UI::GUIMapControl::~GUIMapControl()
{
	if (this->mapEnv)
	{
		this->mapEnv->RemoveUpdatedHandler(ImageUpdated, this);
	}
	NotNullPtr<Media::DrawImage> img;
	if (img.Set(this->bgImg))
	{
		this->eng->DeleteImage(img);
	}
	this->ReleaseSelVecList();
	if (this->renderer)
	{
		this->renderer->SetUpdatedHandler(0, 0);
	}

	if (this->releaseRenderer)
	{
		DEL_CLASS(this->renderer);
	}
	this->view.Delete();
	this->colorSess->RemoveHandler(*this);
}

void UI::GUIMapControl::OnSizeChanged(Bool updateScn)
{
	Sync::MutexUsage mutUsage(this->drawMut);
	this->currSize = this->GetSizeP();
	this->view->UpdateSize(this->currSize.ToDouble());
	NotNullPtr<Media::DrawImage> img;
	if (img.Set(this->bgImg))
	{
		this->eng->DeleteImage(img);
		this->bgImg = 0;
	}
	if (this->currSize.x > 0 && this->currSize.y > 0)
	{
		this->bgImg = this->eng->CreateImage32(this->currSize, Media::AT_NO_ALPHA);
		this->bgImg->SetHDPI(this->view->GetHDPI() / this->view->GetDDPI() * 96.0);
		this->bgImg->SetVDPI(this->view->GetHDPI() / this->view->GetDDPI() * 96.0);
		this->bgImg->SetColorProfile(this->colorSess->GetRGBParam()->monProfile);
	}
	mutUsage.EndUse();
	this->UpdateMap();

	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers.GetItem(i)(this->resizeHandlersObjs.GetItem(i));
	}
}

void UI::GUIMapControl::YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam)
{
}

void UI::GUIMapControl::RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam)
{
	this->renderer->ColorUpdated();
	this->SetBGColor(this->bgColor);
	if (this->bgImg)
	{
		this->bgImg->SetColorProfile(rgbParam->monProfile);
	}
}

void UI::GUIMapControl::SetDPI(Double hdpi, Double ddpi)
{
	this->hdpi = hdpi;
	this->ddpi = ddpi;
	this->view->SetDPI(hdpi, ddpi);
	if (this->bgImg)
	{
		this->bgImg->SetHDPI(hdpi / ddpi * 96.0);
		this->bgImg->SetVDPI(hdpi / ddpi * 96.0);
	}
	if (!this->pauseUpdate)
	{
		this->UpdateMap();
		this->Redraw();
	}
}

void UI::GUIMapControl::EventScaleChanged(Double newScale)
{
	UOSInt i = this->scaleChgHdlrs.GetCount();
	while (i-- > 0)
	{
		this->scaleChgHdlrs.GetItem(i)(this->scaleChgObjs.GetItem(i), newScale);
	}
}

void UI::GUIMapControl::SetBGColor(UInt32 bgColor)
{
	this->bgColor = bgColor;
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
	this->bgDispColor = Media::ColorConv::ConvARGB(srcProfile, destProfile, this->colorSess.Ptr(), this->bgColor);
	if (!this->pauseUpdate)
	{
		this->UpdateMap();
		this->Redraw();
	}
}

void UI::GUIMapControl::SetRenderer(Map::DrawMapRenderer *renderer)
{
	Sync::MutexUsage mutUsage(this->drawMut);
	if (this->renderer)
	{
		this->renderer->SetUpdatedHandler(0, 0);
	}
	this->renderer = renderer;
	mutUsage.EndUse();
	if (renderer == 0)
		return;
	renderer->SetUpdatedHandler(ImageUpdated, this);
	if (!this->pauseUpdate)
	{
		this->UpdateMap();
		this->Redraw();
	}
}

void UI::GUIMapControl::UpdateMap()
{
	Sync::MutexUsage mutUsage(this->drawMut);
	NotNullPtr<Media::DrawImage> bgImg;
	if (bgImg.Set(this->bgImg) && this->renderer)
	{
		Double t;
		UOSInt i;
		UInt32 imgDurMS;
		Math::Coord2DDbl center = this->view->GetCenter();
		Manage::HiResClock clk;
		NotNullPtr<Media::DrawBrush> b = bgImg->NewBrushARGB(this->bgDispColor);
		bgImg->DrawRect(Math::Coord2DDbl(0, 0), bgImg->GetSize().ToDouble(), 0, b);
		bgImg->DelBrush(b);
		this->renderer->DrawMap(bgImg, this->view, imgDurMS);
		t = clk.GetTimeDiff();
		if (imgDurMS != 0)
		{
			Data::DateTime dt;
			dt.SetCurrTimeUTC();
			this->imgTimeoutTick = dt.ToTicks() + imgDurMS - Double2Int32(t * 1000);
		}
		else
		{
			this->imgTimeoutTick = 0;
		}
		i = this->mapUpdHdlrs.GetCount();
		while (i-- > 0)
		{
			this->mapUpdHdlrs.GetItem(i)(this->mapUpdObjs.GetItem(i), center, t);
		}
	}
}

Math::Coord2DDbl UI::GUIMapControl::ScnXY2MapXY(Math::Coord2D<OSInt> scnPos)
{
	return this->view->ScnXYToMapXY(scnPos.ToDouble());
}

Math::Coord2DDbl UI::GUIMapControl::ScnXYD2MapXY(Math::Coord2DDbl scnPos)
{
	return this->view->ScnXYToMapXY(scnPos);
}

Math::Coord2D<OSInt> UI::GUIMapControl::MapXY2ScnXY(Math::Coord2DDbl mapPos)
{
	Math::Coord2DDbl scnPos = this->view->MapXYToScnXY(mapPos);
	if (this->mouseDown)
	{
		return Math::Coord2D<OSInt>(Double2OSInt(scnPos.x) - this->mouseDownPos.x + this->mouseCurrPos.x,
			Double2OSInt(scnPos.y) - this->mouseDownPos.y + this->mouseCurrPos.y);
	}
	else
	{
		return Math::Coord2D<OSInt>(Double2OSInt(scnPos.x), Double2OSInt(scnPos.y));
	}
}

void UI::GUIMapControl::SetMapScale(Double newScale)
{
	if (this->view->GetMapScale() == newScale)
		return;
	NotNullPtr<Map::MapEnv> mapEnv;
	if (!mapEnv.Set(this->mapEnv))
		mapEnv = this->renderer->GetEnv();
	if (newScale < mapEnv->GetMinScale())
		newScale = mapEnv->GetMinScale();
	else if (newScale > mapEnv->GetMaxScale())
		newScale = mapEnv->GetMaxScale();
	this->view->SetMapScale(newScale);
	this->EventScaleChanged(newScale);
	if (!this->pauseUpdate)
	{
		this->UpdateMap();
		this->Redraw();
	}
}

Double UI::GUIMapControl::GetMapScale()
{
	return this->view->GetMapScale();
}

Double UI::GUIMapControl::GetViewScale()
{
	return this->view->GetViewScale();
}

void UI::GUIMapControl::PanToMapXY(Math::Coord2DDbl mapPos)
{
	if (this->view->GetCenter() == mapPos)
		return;
	this->view->SetCenterXY(mapPos);
	if (!this->pauseUpdate)
	{
		this->UpdateMap();
		this->Redraw();
	}
}

void UI::GUIMapControl::ZoomToRect(Math::RectAreaDbl mapRect)
{
	this->view->SetViewBounds(mapRect);
	if (!this->pauseUpdate)
	{
		this->UpdateMap();
		this->Redraw();
	}
}

Bool UI::GUIMapControl::InMapMapXY(Math::Coord2DDbl mapPos)
{
	Math::Size2D<UOSInt> sz = this->GetSizeP();
	if (sz.x <= 0 || sz.y <= 0)
		return true;
	return this->view->InViewXY(mapPos);
}

void UI::GUIMapControl::ShowMarkerMapXY(Math::Coord2DDbl mapPos)
{
	this->markerPos = mapPos;
	this->markerHasDir = false;
	this->showMarker = true;
	if (!this->pauseUpdate)
	{
		this->Redraw();
	}
}

void UI::GUIMapControl::ShowMarkerMapXYDir(Math::Coord2DDbl mapPos, Double dir, Math::Unit::Angle::AngleUnit unit)
{
	this->markerPos = mapPos;
	this->markerDir = Math::Unit::Angle::Convert(unit, Math::Unit::Angle::AU_RADIAN, dir);
	this->markerHasDir = true;
	this->showMarker = true;
	if (!this->pauseUpdate)
	{
		this->Redraw();
	}
}

void UI::GUIMapControl::HideMarker()
{
	this->showMarker = false;
	if (!this->pauseUpdate)
	{
		this->Redraw();
	}
}

void UI::GUIMapControl::SetSelectedVector(Math::Geometry::Vector2D *vec)
{
	this->ReleaseSelVecList();
	if (vec)
	{
		this->selVecList.Add(vec);
	}
	this->Redraw();
}

void UI::GUIMapControl::SetSelectedVectors(NotNullPtr<Data::ArrayList<Math::Geometry::Vector2D*>> vecList)
{
	this->ReleaseSelVecList();
	this->selVecList.AddAll(vecList);
	this->Redraw();
}

void UI::GUIMapControl::SetVAngle(Double angleRad)
{
	this->view->SetVAngle(angleRad);
}

void UI::GUIMapControl::HandleScaleChanged(ScaleChangedHandler hdlr, void *userObj)
{
	this->scaleChgHdlrs.Add(hdlr);
	this->scaleChgObjs.Add(userObj);
	hdlr(userObj, this->view->GetMapScale());
}

void UI::GUIMapControl::HandleMapUpdated(MapUpdatedHandler hdlr, void *userObj)
{
	this->mapUpdHdlrs.Add(hdlr);
	this->mapUpdObjs.Add(userObj);
}

void UI::GUIMapControl::HandleMouseMove(MouseMoveHandler hdlr, void *userObj)
{
	this->mouseMoveHdlrs.Add(hdlr);
	this->mouseMoveObjs.Add(userObj);
}

void UI::GUIMapControl::HandleMouseUp(MouseEventHandler hdlr, void *userObj)
{
	this->mouseUpHdlr = hdlr;
	this->mouseUpObj = userObj;
}

void UI::GUIMapControl::HandleMouseDown(MouseEventHandler hdlr, void *userObj)
{
	this->mouseDownHdlr = hdlr;
	this->mouseDownObj = userObj;
}

void UI::GUIMapControl::HandleCustomDraw(DrawHandler hdlr, void *userObj)
{
	this->drawHdlr = hdlr;
	this->drawHdlrObj = userObj;
}

void UI::GUIMapControl::SetMapUpdated()
{
	this->bgUpdated = true;
}

void UI::GUIMapControl::UpdateMapView(NotNullPtr<Map::MapView> view)
{
	this->view.Delete();
	this->view = view;
	this->view->UpdateSize(this->currSize.ToDouble());
	this->EventScaleChanged(this->view->GetMapScale());
}

NotNullPtr<Map::MapView> UI::GUIMapControl::CloneMapView()
{
	return this->view->Clone();
}

void UI::GUIMapControl::PauseUpdate(Bool pauseUpdate)
{
	this->pauseUpdate = pauseUpdate;
}
