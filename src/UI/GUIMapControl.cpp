#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Math/Geometry/Ellipse.h"
#include "Math/Geometry/PieArea.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIMapControl.h"
#include "UI/MessageDialog.h"

void __stdcall UI::GUIMapControl::ImageUpdated(void *userObj)
{
	UI::GUIMapControl *me = (UI::GUIMapControl*)userObj;
	me->bgUpdated = true;
	if (!me->pauseUpdate)
	{
		me->Redraw();
	}
}

Bool UI::GUIMapControl::OnMouseDown(OSInt scnX, OSInt scnY, MouseButton btn)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		this->SetCapture();
		if (this->mouseDownHdlr)
		{
			Bool done = this->mouseDownHdlr(this->mouseDownObj, Math::Coord2D<OSInt>(scnX, scnY), btn);
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
			this->OnMouseWheel(scnX, scnY, 1);
			this->mouseLDownTime = 0;
			return true;
		}
		else
		{
			this->mouseLDownTime = currTime;
		}	
		this->mouseCurrX = this->mouseDownX = scnX;
		this->mouseCurrY = this->mouseDownY = scnY;
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
			this->OnMouseWheel(scnX, scnY, -1);
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

Bool UI::GUIMapControl::OnMouseUp(OSInt scnX, OSInt scnY, MouseButton btn)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		this->ReleaseCapture();
		if (this->mouseUpHdlr)
		{
			Bool done = this->mouseUpHdlr(this->mouseUpObj, Math::Coord2D<OSInt>(scnX, scnY), btn);
			if (done)
			{
				return true;
			}
		}
		if (this->mouseDown)
		{
			OSInt diffX = this->mouseCurrX - this->mouseDownX;
			OSInt diffY = this->mouseCurrY - this->mouseDownY;
			Double newCentX = UOSInt2Double(this->currSize.width) * 0.5 - OSInt2Double(diffX);
			Double newCentY = UOSInt2Double(this->currSize.height) * 0.5 - OSInt2Double(diffY);
			Math::Coord2DDbl mapPos = this->view->ScnXYToMapXY(Math::Coord2DDbl(newCentX, newCentY));
			this->view->SetCenterXY(mapPos);
			this->UpdateMap();
			this->mouseDown = false;
		}
		this->Redraw();
	}
	return false;
}

void UI::GUIMapControl::OnMouseMove(OSInt scnX, OSInt scnY)
{
	if (this->mouseDown)
	{
		this->mouseCurrX = scnX;
		this->mouseCurrY = scnY;
		this->Redraw();
	}
	else
	{
		OSInt x = scnX;
		OSInt y = scnY;
		UOSInt i = this->mouseMoveHdlrs.GetCount();
		while (i-- > 0)
		{
			this->mouseMoveHdlrs.GetItem(i)(this->mouseMoveObjs.GetItem(i), Math::Coord2D<OSInt>(x, y));
		}
	}
/*	if (me->IsFormFocused())
	{
		SetFocus((HWND)hWnd);
	}*/
}

Bool UI::GUIMapControl::OnMouseWheel(OSInt scnX, OSInt scnY, Int32 delta)
{
	Math::Coord2DDbl newPt;
	Math::Coord2DDbl pt = this->view->ScnXYToMapXY(Math::Coord2DDbl(OSInt2Double(scnX), OSInt2Double(scnY)));
	if (delta < 0)
	{
		this->view->SetMapScale(this->view->GetMapScale() * 2.0);
	}
	else
	{
		this->view->SetMapScale(this->view->GetMapScale() * 0.5);
	}
	newPt = this->view->MapXYToScnXY(pt);
	pt = this->view->ScnXYToMapXY(Math::Coord2DDbl(newPt.x - OSInt2Double(scnX) + (UOSInt2Double(this->currSize.width) * 0.5), newPt.y - OSInt2Double(scnY) + (UOSInt2Double(this->currSize.height) * 0.5)));
	this->view->SetCenterXY(pt);
	this->EventScaleChanged(this->view->GetMapScale());
	this->UpdateMap();
	this->Redraw();
	return true;
}

void UI::GUIMapControl::OnGestureBegin(OSInt scnX, OSInt scnY, UInt64 dist)
{
	this->gZoom = true;
	this->gZoomX = scnX;
	this->gZoomY = scnY;
	this->gZoomCurrX = this->gZoomX;
	this->gZoomCurrY = this->gZoomY;
	this->gZoomDist = dist;
	this->gZoomCurrDist = dist;
}

void UI::GUIMapControl::OnGestureStep(OSInt scnX, OSInt scnY, UInt64 dist)
{
	if (this->gZoom)
	{
		this->gZoomCurrDist = dist;
		this->gZoomCurrX = scnX;
		this->gZoomCurrY = scnY;
		this->Redraw();
	}
}

void UI::GUIMapControl::OnGestureEnd(OSInt scnX, OSInt scnY, UInt64 dist)
{
	if (this->gZoom)
	{
		Math::Coord2DDbl pt;
		Math::Coord2DDbl newPt;

		this->gZoomCurrX = scnX;
		this->gZoomCurrY = scnY;

		this->gZoom = false;
		pt = this->view->ScnXYToMapXY(Math::Coord2DDbl(OSInt2Double(this->gZoomX), OSInt2Double(this->gZoomY)));
		this->view->SetMapScale(this->view->GetMapScale() * (Double)this->gZoomDist /(Double)dist);
		newPt = this->view->MapXYToScnXY(pt);
		pt = this->view->ScnXYToMapXY(Math::Coord2DDbl(newPt.x - OSInt2Double(this->gZoomCurrX) + (UOSInt2Double(this->currSize.width) * 0.5), newPt.y - OSInt2Double(this->gZoomCurrY) + (UOSInt2Double(this->currSize.height) * 0.5)));
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
		OSInt diffX = axis1;
		OSInt diffY = axis2;
		Double newCentX = UOSInt2Double(this->currSize.width) * 0.5 - OSInt2Double(diffX);
		Double newCentY = UOSInt2Double(this->currSize.height) * 0.5 - OSInt2Double(diffY);
		Math::Coord2DDbl mapPos = this->view->ScnXYToMapXY(Math::Coord2DDbl(newCentX, newCentY));
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

void UI::GUIMapControl::OnDraw(Media::DrawImage *img)
{
	Double tlx;
	Double tly;
	if (this->bgImg == 0)
	{
		return;
	}
	if (this->mouseDown)
	{
		tlx = OSInt2Double(this->mouseCurrX - this->mouseDownX);
		tly = OSInt2Double(this->mouseCurrY - this->mouseDownY);
	}
	else
	{
		tlx = 0;
		tly = 0;
	}
	if (this->bgUpdated)
	{
		this->bgUpdated = false;
		this->UpdateMap();
	}

	if (this->gZoom)
	{
		Media::StaticImage *drawImg = 0;
		Media::StaticImage *srcImg;
		Media::Resizer::LanczosResizerH8_8 resizer(4, 3, Media::AT_NO_ALPHA);
		UOSInt w;
		UOSInt h;

		Sync::MutexUsage mutUsage(&this->drawMut);
		if (this->drawHdlr)
		{
			Media::DrawImage *tmpImg = this->eng->CloneImage(this->bgImg);
			this->drawHdlr(this->drawHdlrObj, tmpImg, 0, 0);
			srcImg = tmpImg->ToStaticImage();
			this->eng->DeleteImage(tmpImg);
		}
		else
		{
			srcImg = this->bgImg->ToStaticImage();
		}
		
		if (this->gZoomCurrDist > this->gZoomDist)
		{
			tlx = 0;
			tly = 0;
			w = this->currSize.width;
			h = this->currSize.height;
			NEW_CLASS(drawImg, Media::StaticImage(this->currSize.width, this->currSize.height, 0, 32, Media::PF_B8G8R8A8, 0, 0, Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			Double rate = (Double)this->gZoomDist / (Double)this->gZoomCurrDist;
			Double srcW = UOSInt2Double(this->currSize.width) * rate;
			Double srcH = UOSInt2Double(this->currSize.height) * rate;
			Double srcX = OSInt2Double(this->gZoomX) - OSInt2Double(this->gZoomCurrX) * rate;
			Double srcY = OSInt2Double(this->gZoomY) - OSInt2Double(this->gZoomCurrY) * rate;
			drawImg->info.hdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
			drawImg->info.vdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
			drawImg->info.color->Set(&this->colorSess->GetRGBParam()->monProfile);

			if (srcX < 0)
			{
				tlx = -srcX / rate;
				srcX = 0;
			}
			else if (srcX + srcW > UOSInt2Double(this->currSize.width))
			{
				tlx = (UOSInt2Double(this->currSize.width) - srcW - srcX) / rate;
				srcX = UOSInt2Double(this->currSize.width) - srcW;
			}
			if (srcY < 0)
			{
				tly = -srcY / rate;
				srcY = 0;
			}
			else if (srcY + srcH > UOSInt2Double(this->currSize.height))
			{
				tly = (UOSInt2Double(this->currSize.height) - srcH - srcY) / rate;
				srcY = UOSInt2Double(this->currSize.height) - srcH;
			}
			Int32 srcIX = (Int32)srcX;
			Int32 srcIY = (Int32)srcY;
			resizer.Resize((srcIX * 4) + (srcIY * (OSInt)(srcImg->info.storeWidth * 4)) + srcImg->data, (OSInt)srcImg->info.storeWidth * 4, srcW, srcH, srcX - srcIX, srcY - srcIY, drawImg->data, (OSInt)drawImg->info.storeWidth * 4, w, h);
		}
		else
		{
			Double rate = (Double)this->gZoomCurrDist / (Double)this->gZoomDist;
			w = (UOSInt)Double2OSInt(UOSInt2Double(this->currSize.width) * rate);
			h = (UOSInt)Double2OSInt(UOSInt2Double(this->currSize.height) * rate);
			tlx = OSInt2Double(this->gZoomCurrX) - OSInt2Double(this->gZoomX) * rate;
			tly = OSInt2Double(this->gZoomCurrY) - OSInt2Double(this->gZoomY) * rate;
			NEW_CLASS(drawImg, Media::StaticImage(this->currSize.width, this->currSize.height, 0, 32, Media::PF_B8G8R8A8, 0, 0, Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			drawImg->info.hdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
			drawImg->info.vdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
			drawImg->info.color->Set(&this->colorSess->GetRGBParam()->monProfile);
			resizer.Resize(srcImg->data, (OSInt)srcImg->info.storeWidth * 4, UOSInt2Double(this->currSize.width), UOSInt2Double(this->currSize.height), 0, 0, drawImg->data, (OSInt)drawImg->info.storeWidth * 4, w, h);
		}
		mutUsage.EndUse();
		DEL_CLASS(srcImg);

		img->DrawImagePt2(drawImg, tlx, tly);
		DEL_CLASS(drawImg);


		Media::DrawBrush *bgBrush = img->NewBrushARGB(this->bgColor);
		if (tlx > 0)
		{
			img->DrawRect(0, 0, tlx, UOSInt2Double(this->currSize.height), 0, bgBrush);
		}
		if (tlx + UOSInt2Double(w) < UOSInt2Double(this->currSize.width))
		{
			img->DrawRect(UOSInt2Double(w) + tlx, 0, UOSInt2Double(this->currSize.width - w) - tlx, UOSInt2Double(this->currSize.height), 0, bgBrush);
		}
		if (tly > 0)
		{
			img->DrawRect(0, 0, UOSInt2Double(this->currSize.width), tly, 0, bgBrush);
		}
		if (tly + UOSInt2Double(h) < UOSInt2Double(this->currSize.height))
		{
			img->DrawRect(0, UOSInt2Double(h) + tly, UOSInt2Double(this->currSize.width), UOSInt2Double(this->currSize.height - h) - tly, 0, bgBrush);
		}
		img->DelBrush(bgBrush);
	}
	else
	{
		Media::DrawBrush *bgBrush = img->NewBrushARGB(this->bgColor);
		if (tlx > 0)
		{
			img->DrawRect(0, 0, tlx, UOSInt2Double(this->currSize.height), 0, bgBrush);
		}
		else if (tlx < 0)
		{
			img->DrawRect(UOSInt2Double(this->currSize.width) + tlx, 0, -tlx, UOSInt2Double(this->currSize.height), 0, bgBrush);
		}
		if (tly > 0)
		{
			img->DrawRect(0, 0, UOSInt2Double(this->currSize.width), tly, 0, bgBrush);
		}
		else if (tly < 0)
		{
			img->DrawRect(0, UOSInt2Double(this->currSize.height) + tly, UOSInt2Double(this->currSize.width), -tly, 0, bgBrush);
		}
		img->DelBrush(bgBrush);

		Sync::MutexUsage mutUsage(&this->drawMut);
		if (this->drawHdlr)
		{
			Media::DrawImage *drawImg = this->eng->CloneImage(this->bgImg);
			this->drawHdlr(this->drawHdlrObj, drawImg, 0, 0);
			this->DrawScnObjects(drawImg, Math::Coord2DDbl(0, 0));
			img->DrawImagePt(drawImg, tlx, tly);
			this->eng->DeleteImage(drawImg);
		}
		else
		{
			img->DrawImagePt(this->bgImg, tlx, tly);
			this->DrawScnObjects(img, Math::Coord2DDbl(tlx, tly));
		}
		mutUsage.EndUse();
	}
}

void UI::GUIMapControl::DrawScnObjects(Media::DrawImage *img, Math::Coord2DDbl ofst)
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
	if (this->selVec)
	{
		Math::Geometry::Vector2D::VectorType vecType = this->selVec->GetVectorType();
		if (vecType == Math::Geometry::Vector2D::VectorType::LineString)
		{
			Math::Geometry::LineString *pl = (Math::Geometry::LineString*)this->selVec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			UOSInt nPoint;
			Math::Coord2DDbl *points = pl->GetPointList(&nPoint);
			Math::Coord2DDbl *dpoints = MemAllocA(Math::Coord2DDbl, nPoint);
			view->MapXYToScnXY(points, dpoints, nPoint, ofst);
			img->DrawPolyline(dpoints, nPoint, p);
			MemFreeA(dpoints);
			img->DelPen(p);
		}
		else if (vecType == Math::Geometry::Vector2D::VectorType::Polyline)
		{
			Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)this->selVec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			UOSInt nPoint;
			UOSInt nPtOfst;
			Math::Coord2DDbl *points = pl->GetPointList(&nPoint);
			UInt32 *ptOfsts = pl->GetPtOfstList(&nPtOfst);
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
		else if (this->selVec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polygon)
		{
			Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)this->selVec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			Media::DrawBrush *b = img->NewBrushARGB(0x403f0000);
			UOSInt nPoint;
			UOSInt nPtOfst;
			Math::Coord2DDbl *points = pg->GetPointList(&nPoint);
			UInt32 *ptOfsts = pg->GetPtOfstList(&nPtOfst);
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
		else if (this->selVec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Ellipse)
		{
			Math::Geometry::Ellipse *circle = (Math::Geometry::Ellipse*)this->selVec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			Media::DrawBrush *b = img->NewBrushARGB(0x403f0000);
			img->DrawEllipse(circle->GetLeft(), circle->GetTop(), circle->GetWidth(), circle->GetHeight(), p, b);
			img->DelPen(p);
			img->DelBrush(b);
		}
		else if (this->selVec->GetVectorType() == Math::Geometry::Vector2D::VectorType::PieArea)
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
		else if (this->selVec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Image)
		{
			Math::Geometry::VectorImage *vimg = (Math::Geometry::VectorImage*)this->selVec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			Media::DrawBrush *b = img->NewBrushARGB(0x403f0000);
			UInt32 nPoints;
			Math::Coord2DDbl pts[5];
			Math::RectAreaDbl bounds;
			
			if (vimg->IsScnCoord())
			{
				vimg->GetScreenBounds(img->GetWidth(), img->GetHeight(), img->GetHDPI(), img->GetVDPI(), &bounds.tl.x, &bounds.tl.y, &bounds.br.x, &bounds.br.y);
				pts[0] = bounds.tl;
				pts[1].x = bounds.tl.x;
				pts[1].y = bounds.br.y;
				pts[2] = bounds.br;
				pts[3].x = bounds.br.x;
				pts[3].y = bounds.tl.y;
				pts[4] = bounds.tl;
			}
			else
			{
				vimg->GetBounds(&bounds);
				Math::Coord2DDbl pt1 = view->MapXYToScnXY(bounds.tl);
				Math::Coord2DDbl pt2 = view->MapXYToScnXY(bounds.br);
				pts[0] = pt1;
				pts[1].x = pt1.x;
				pts[1].y = pt2.y;
				pts[2] = pt2;
				pts[3].x = pt2.x;
				pts[3].y = pt1.y;
				pts[4] = pt1;
			}
			nPoints = 5;
			img->DrawPolyPolygon(pts, &nPoints, 1, p, b);
			img->DelPen(p);
			img->DelBrush(b);
		}
	}
}

UI::GUIMapControl::GUIMapControl(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, UInt32 bgColor, Map::DrawMapRenderer *renderer, Map::MapView *view, Media::ColorManagerSess *colorSess) : UI::GUICustomDraw(ui, parent, eng)
{
	this->colorSess = colorSess;
	if (this->colorSess)
	{
		this->colorSess->AddHandler(this);
	}
	this->currSize = Math::Size2D<UOSInt>(640, 480);
	this->bgImg = 0;
	this->mouseDown = false;
	this->mouseCurrX = 0;
	this->mouseCurrY = 0;
	this->mouseDownX = 0;
	this->mouseDownY = 0;
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
	this->selVec = 0;
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

UI::GUIMapControl::GUIMapControl(GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, Map::MapEnv *mapEnv, Media::ColorManagerSess *colorSess) : UI::GUICustomDraw(ui, parent, eng)
{
	this->colorSess = colorSess;
	if (this->colorSess)
	{
		this->colorSess->AddHandler(this);
	}
	this->currSize = Math::Size2D<UOSInt>(640, 480);
	this->bgImg = 0;
	this->mouseDown = false;
	this->mouseCurrX = 0;
	this->mouseCurrY = 0;
	this->mouseDownX = 0;
	this->mouseDownY = 0;
	this->gZoom = false;
	this->bgColor = mapEnv->GetBGColor();
	this->bgUpdated = false;
	this->mouseDownHdlr = 0;
	this->mouseUpHdlr = 0;
	this->drawHdlr = 0;
	this->drawHdlrObj = 0;
	this->mapEnv = mapEnv;
	this->imgTimeoutTick = 0;

	this->markerPos = Math::Coord2DDbl(0, 0);
	this->showMarker = false;
	this->selVec = 0;
	Media::ColorProfile color(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->renderer, Map::DrawMapRenderer(this->eng, mapEnv, &color, this->colorSess, Map::DrawMapRenderer::DT_PIXELDRAW));
	this->releaseRenderer = true;

	this->view = mapEnv->CreateMapView(Math::Size2D<Double>(640, 480));
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
	if (this->bgImg)
	{
		this->eng->DeleteImage(this->bgImg);
	}
	if (this->selVec)
	{
		DEL_CLASS(this->selVec);
		this->selVec = 0;
	}
	if (this->renderer)
	{
		this->renderer->SetUpdatedHandler(0, 0);
	}

	if (this->releaseRenderer)
	{
		DEL_CLASS(this->renderer);
	}
	DEL_CLASS(this->view);
	if (this->colorSess)
	{
		this->colorSess->RemoveHandler(this);
	}
}

void UI::GUIMapControl::OnSizeChanged(Bool updateScn)
{
	Sync::MutexUsage mutUsage(&this->drawMut);
	this->currSize = this->GetSizeP();
	this->view->UpdateSize(this->currSize.ToDouble());
	if (this->bgImg)
	{
		this->eng->DeleteImage(this->bgImg);
		this->bgImg = 0;
	}
	if (this->currSize.width > 0 && this->currSize.height > 0)
	{
		this->bgImg = this->eng->CreateImage32(this->currSize.width, this->currSize.height, Media::AT_NO_ALPHA);
		this->bgImg->SetHDPI(this->view->GetHDPI() / this->view->GetDDPI() * 96.0);
		this->bgImg->SetVDPI(this->view->GetHDPI() / this->view->GetDDPI() * 96.0);
		this->bgImg->SetColorProfile(&this->colorSess->GetRGBParam()->monProfile);
	}
	mutUsage.EndUse();
	this->UpdateMap();

	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers.GetItem(i)(this->resizeHandlersObjs.GetItem(i));
	}
}

void UI::GUIMapControl::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
}

void UI::GUIMapControl::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	this->renderer->ColorUpdated();
	this->SetBGColor(this->bgColor);
	if (this->bgImg)
	{
		this->bgImg->SetColorProfile(&rgbParam->monProfile);
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
	this->bgDispColor = Media::ColorConv::ConvARGB(&srcProfile, &destProfile, this->colorSess, this->bgColor);
	if (!this->pauseUpdate)
	{
		this->UpdateMap();
		this->Redraw();
	}
}

void UI::GUIMapControl::SetRenderer(Map::DrawMapRenderer *renderer)
{
	Sync::MutexUsage mutUsage(&this->drawMut);
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
	Sync::MutexUsage mutUsage(&this->drawMut);
	if (this->bgImg && this->renderer)
	{
		Double t;
		UOSInt i;
		UInt32 imgDurMS;
		Math::Coord2DDbl center = this->view->GetCenter();
		Manage::HiResClock clk;
		Media::DrawBrush *b = this->bgImg->NewBrushARGB(this->bgDispColor);
		this->bgImg->DrawRect(0, 0, UOSInt2Double(this->bgImg->GetWidth()), UOSInt2Double(this->bgImg->GetHeight()), 0, b);
		this->bgImg->DelBrush(b);
		this->renderer->DrawMap(this->bgImg, this->view, &imgDurMS);
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
	mutUsage.EndUse();
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
		return Math::Coord2D<OSInt>(Double2OSInt(scnPos.x) - this->mouseDownX + this->mouseCurrX,
			Double2OSInt(scnPos.y) - this->mouseDownY + this->mouseCurrY);
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
	if (sz.width <= 0 || sz.height <= 0)
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
	if (this->selVec)
	{
		DEL_CLASS(this->selVec);
		this->selVec = 0;
	}
	this->selVec = vec;
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

void UI::GUIMapControl::UpdateMapView(Map::MapView *view)
{
	if (view)
	{
		DEL_CLASS(this->view);
		this->view = view;
		this->view->UpdateSize(this->currSize.ToDouble());
		this->EventScaleChanged(this->view->GetMapScale());
	}
}

Map::MapView *UI::GUIMapControl::CloneMapView()
{
	Map::MapView *view = this->view->Clone();
	return view;
}

void UI::GUIMapControl::PauseUpdate(Bool pauseUpdate)
{
	this->pauseUpdate = pauseUpdate;
}
