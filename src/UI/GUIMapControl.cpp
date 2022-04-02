#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Manage/HiResClock.h"
#include "Math/Ellipse.h"
#include "Math/Math.h"
#include "Math/PieArea.h"
#include "Math/Polyline.h"
#include "Math/VectorImage.h"
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
			Bool done = this->mouseDownHdlr(this->mouseDownObj, scnX, scnY, btn);
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
			Bool done = this->mouseUpHdlr(this->mouseUpObj, scnX, scnY, btn);
			if (done)
			{
				return true;
			}
		}
		if (this->mouseDown)
		{
			OSInt diffX = this->mouseCurrX - this->mouseDownX;
			OSInt diffY = this->mouseCurrY - this->mouseDownY;
			Double newCentX = UOSInt2Double(this->currWidth) * 0.5 - OSInt2Double(diffX);
			Double newCentY = UOSInt2Double(this->currHeight) * 0.5 - OSInt2Double(diffY);
			Double mapX;
			Double mapY;
			this->view->ScnXYToMapXY(newCentX, newCentY, &mapX, &mapY);
			this->view->SetCenterXY(mapX, mapY);
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
		UOSInt i = this->mouseMoveHdlrs->GetCount();
		while (i-- > 0)
		{
			this->mouseMoveHdlrs->GetItem(i)(this->mouseMoveObjs->GetItem(i), x, y);
		}
	}
/*	if (me->IsFormFocused())
	{
		SetFocus((HWND)hWnd);
	}*/
}

Bool UI::GUIMapControl::OnMouseWheel(OSInt scnX, OSInt scnY, Int32 delta)
{
	Double ptX;
	Double ptY;
	Double newPtX;
	Double newPtY;
	this->view->ScnXYToMapXY(OSInt2Double(scnX), OSInt2Double(scnY), &ptX, &ptY);
	if (delta < 0)
	{
		this->view->SetMapScale(this->view->GetMapScale() * 2.0);
	}
	else
	{
		this->view->SetMapScale(this->view->GetMapScale() * 0.5);
	}
	this->view->MapXYToScnXY(ptX, ptY, &newPtX, &newPtY);
	this->view->ScnXYToMapXY(newPtX - OSInt2Double(scnX) + (UOSInt2Double(this->currWidth) * 0.5), newPtY - OSInt2Double(scnY) + (UOSInt2Double(this->currHeight) * 0.5), &ptX, &ptY);
	this->view->SetCenterXY(ptX, ptY);
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
		Double ptX;
		Double ptY;
		Double newPtX;
		Double newPtY;

		this->gZoomCurrX = scnX;
		this->gZoomCurrY = scnY;

		this->gZoom = false;
		this->view->ScnXYToMapXY(OSInt2Double(this->gZoomX), OSInt2Double(this->gZoomY), &ptX, &ptY);
		this->view->SetMapScale(this->view->GetMapScale() * (Double)this->gZoomDist /(Double)dist);
		this->view->MapXYToScnXY(ptX, ptY, &newPtX, &newPtY);
		this->view->ScnXYToMapXY(newPtX - OSInt2Double(this->gZoomCurrX) + (UOSInt2Double(this->currWidth) * 0.5), newPtY - OSInt2Double(this->gZoomCurrY) + (UOSInt2Double(this->currHeight) * 0.5), &ptX, &ptY);
		this->view->SetCenterXY(ptX, ptY);
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
		Double newCentX = UOSInt2Double(this->currWidth) * 0.5 - OSInt2Double(diffX);
		Double newCentY = UOSInt2Double(this->currHeight) * 0.5 - OSInt2Double(diffY);
		Double mapX;
		Double mapY;
		this->view->ScnXYToMapXY(newCentX, newCentY, &mapX, &mapY);
		this->view->SetCenterXY(mapX, mapY);
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

		Sync::MutexUsage mutUsage(this->drawMut);
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
			w = this->currWidth;
			h = this->currHeight;
			NEW_CLASS(drawImg, Media::StaticImage(this->currWidth, this->currHeight, 0, 32, Media::PF_B8G8R8A8, 0, 0, Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			Double rate = (Double)this->gZoomDist / (Double)this->gZoomCurrDist;
			Double srcW = UOSInt2Double(this->currWidth) * rate;
			Double srcH = UOSInt2Double(this->currHeight) * rate;
			Double srcX = OSInt2Double(this->gZoomX) - OSInt2Double(this->gZoomCurrX) * rate;
			Double srcY = OSInt2Double(this->gZoomY) - OSInt2Double(this->gZoomCurrY) * rate;
			drawImg->info->hdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
			drawImg->info->vdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
			drawImg->info->color->Set(this->colorSess->GetRGBParam()->monProfile);

			if (srcX < 0)
			{
				tlx = -srcX / rate;
				srcX = 0;
			}
			else if (srcX + srcW > UOSInt2Double(this->currWidth))
			{
				tlx = (UOSInt2Double(this->currWidth) - srcW - srcX) / rate;
				srcX = UOSInt2Double(this->currWidth) - srcW;
			}
			if (srcY < 0)
			{
				tly = -srcY / rate;
				srcY = 0;
			}
			else if (srcY + srcH > UOSInt2Double(this->currHeight))
			{
				tly = (UOSInt2Double(this->currHeight) - srcH - srcY) / rate;
				srcY = UOSInt2Double(this->currHeight) - srcH;
			}
			Int32 srcIX = (Int32)srcX;
			Int32 srcIY = (Int32)srcY;
			resizer.Resize((srcIX * 4) + (srcIY * (OSInt)(srcImg->info->storeWidth * 4)) + srcImg->data, (OSInt)srcImg->info->storeWidth * 4, srcW, srcH, srcX - srcIX, srcY - srcIY, drawImg->data, (OSInt)drawImg->info->storeWidth * 4, w, h);
		}
		else
		{
			Double rate = (Double)this->gZoomCurrDist / (Double)this->gZoomDist;
			w = (UOSInt)Double2OSInt(UOSInt2Double(this->currWidth) * rate);
			h = (UOSInt)Double2OSInt(UOSInt2Double(this->currHeight) * rate);
			tlx = OSInt2Double(this->gZoomCurrX) - OSInt2Double(this->gZoomX) * rate;
			tly = OSInt2Double(this->gZoomCurrY) - OSInt2Double(this->gZoomY) * rate;
			NEW_CLASS(drawImg, Media::StaticImage(this->currWidth, this->currHeight, 0, 32, Media::PF_B8G8R8A8, 0, 0, Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			drawImg->info->hdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
			drawImg->info->vdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
			drawImg->info->color->Set(this->colorSess->GetRGBParam()->monProfile);
			resizer.Resize(srcImg->data, (OSInt)srcImg->info->storeWidth * 4, UOSInt2Double(this->currWidth), UOSInt2Double(this->currHeight), 0, 0, drawImg->data, (OSInt)drawImg->info->storeWidth * 4, w, h);
		}
		mutUsage.EndUse();
		DEL_CLASS(srcImg);

		img->DrawImagePt2(drawImg, tlx, tly);
		DEL_CLASS(drawImg);


		Media::DrawBrush *bgBrush = img->NewBrushARGB(this->bgColor);
		if (tlx > 0)
		{
			img->DrawRect(0, 0, tlx, UOSInt2Double(this->currHeight), 0, bgBrush);
		}
		if (tlx + UOSInt2Double(w) < UOSInt2Double(this->currWidth))
		{
			img->DrawRect(UOSInt2Double(w) + tlx, 0, UOSInt2Double(this->currWidth - w) - tlx, UOSInt2Double(this->currHeight), 0, bgBrush);
		}
		if (tly > 0)
		{
			img->DrawRect(0, 0, UOSInt2Double(this->currWidth), tly, 0, bgBrush);
		}
		if (tly + UOSInt2Double(h) < UOSInt2Double(this->currHeight))
		{
			img->DrawRect(0, UOSInt2Double(h) + tly, UOSInt2Double(this->currWidth), UOSInt2Double(this->currHeight - h) - tly, 0, bgBrush);
		}
		img->DelBrush(bgBrush);
	}
	else
	{
		Media::DrawBrush *bgBrush = img->NewBrushARGB(this->bgColor);
		if (tlx > 0)
		{
			img->DrawRect(0, 0, tlx, UOSInt2Double(this->currHeight), 0, bgBrush);
		}
		else if (tlx < 0)
		{
			img->DrawRect(UOSInt2Double(this->currWidth) + tlx, 0, -tlx, UOSInt2Double(this->currHeight), 0, bgBrush);
		}
		if (tly > 0)
		{
			img->DrawRect(0, 0, UOSInt2Double(this->currWidth), tly, 0, bgBrush);
		}
		else if (tly < 0)
		{
			img->DrawRect(0, UOSInt2Double(this->currHeight) + tly, UOSInt2Double(this->currWidth), -tly, 0, bgBrush);
		}
		img->DelBrush(bgBrush);

		Sync::MutexUsage mutUsage(this->drawMut);
		if (this->drawHdlr)
		{
			Media::DrawImage *drawImg = this->eng->CloneImage(this->bgImg);
			this->drawHdlr(this->drawHdlrObj, drawImg, 0, 0);
			this->DrawScnObjects(drawImg, 0, 0);
			img->DrawImagePt(drawImg, tlx, tly);
			this->eng->DeleteImage(drawImg);
		}
		else
		{
			img->DrawImagePt(this->bgImg, tlx, tly);
			this->DrawScnObjects(img, tlx, tly);
		}
		mutUsage.EndUse();
	}
}

void UI::GUIMapControl::DrawScnObjects(Media::DrawImage *img, Double xOfst, Double yOfst)
{
	Double hdpi = this->view->GetHDPI();
	Double ddpi = this->view->GetDDPI();
	if (this->showMarker)
	{
		Double scnX;
		Double scnY;
		Int32 x;
		Int32 y;
		this->view->MapXYToScnXY(this->markerX, this->markerY, &scnX, &scnY);
		x = Double2Int32(scnX + xOfst);
		y = Double2Int32(scnY + yOfst);
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
		if (this->selVec->GetVectorType() == Math::Vector2D::VectorType::Polyline)
		{
			Math::Polyline *pl = (Math::Polyline*)this->selVec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			UOSInt nPoint;
			UOSInt nPtOfst;
			Double *points = pl->GetPointList(&nPoint);
			UInt32 *ptOfsts = pl->GetPtOfstList(&nPtOfst);
			UOSInt i;
			Double *dpoints = MemAlloc(Double, nPoint * 2);
			UOSInt lastCnt;
			UOSInt thisCnt;

			view->MapXYToScnXY(points, dpoints, nPoint, xOfst, yOfst);
			lastCnt = nPoint;
			i = nPtOfst;
			while (i-- > 0)
			{
				thisCnt = ptOfsts[i];
				img->DrawPolyline(&dpoints[thisCnt * 2], lastCnt - thisCnt, p);
				lastCnt = thisCnt;
			}
			MemFree(dpoints);

			img->DelPen(p);
		}
		else if (this->selVec->GetVectorType() == Math::Vector2D::VectorType::Polygon)
		{
			Math::Polygon *pg = (Math::Polygon*)this->selVec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			Media::DrawBrush *b = img->NewBrushARGB(0x403f0000);
			UOSInt nPoint;
			UOSInt nPtOfst;
			Double *points = pg->GetPointList(&nPoint);
			UInt32 *ptOfsts = pg->GetPtOfstList(&nPtOfst);
			Double *dpoints = MemAlloc(Double, nPoint * 2);
			UInt32 *myPtCnts = MemAlloc(UInt32, nPtOfst);
			view->MapXYToScnXY(points, dpoints, nPoint, xOfst, yOfst);

			UOSInt i = nPtOfst;
			while (i-- > 0)
			{
				myPtCnts[i] = (UInt32)nPoint - ptOfsts[i];
				nPoint = ptOfsts[i];
			}

			img->DrawPolyPolygon(dpoints, myPtCnts, nPtOfst, p, b);
			MemFree(dpoints);
			MemFree(myPtCnts);
			img->DelPen(p);
			img->DelBrush(b);
		}
		else if (this->selVec->GetVectorType() == Math::Vector2D::VectorType::Ellipse)
		{
			Math::Ellipse *circle = (Math::Ellipse*)this->selVec;
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
				circle->GetBounds(&x1, &y1, &x2, &y2);
				view->MapXYToScnXY(x1, y1, &x1, &y1);
				view->MapXYToScnXY(x2, y2, &x2, &y2);

				HBRUSH hbr = CreateSolidBrush(0xffffff);
				SelectObject(hdcBmp, hbr);
				Ellipse(hdcBmp, Double2Int32(x1 + xOfst), Double2Int32(y1 + yOfst), Double2Int32(x2 + xOfst), Double2Int32(y2 + yOfst));
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
				Ellipse(hdc, Double2Int32(x1 + xOfst), Double2Int32(y1 + yOfst), Double2Int32(x2 + xOfst), Double2Int32(y2 + yOfst));
				SelectObject(hdc, lastPen);
				DeleteObject(p);

				DeleteObject(hBmp);
			}
			DeleteDC(hdcBmp);*/
		}
		else if (this->selVec->GetVectorType() == Math::Vector2D::VectorType::PieArea)
		{
			Math::PieArea *pie = (Math::PieArea*)this->selVec;
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
		else if (this->selVec->GetVectorType() == Math::Vector2D::VectorType::Image)
		{
			Math::VectorImage *vimg = (Math::VectorImage*)this->selVec;
			Media::DrawPen *p = img->NewPenARGB(0xffff0000, 3, 0, 0);
			Media::DrawBrush *b = img->NewBrushARGB(0x403f0000);
			UInt32 nPoints;
			Double pts[10];
			Double x1;
			Double y1;
			Double x2;
			Double y2;
			
			if (vimg->IsScnCoord())
			{
				vimg->GetScreenBounds(img->GetWidth(), img->GetHeight(), img->GetHDPI(), img->GetVDPI(), &x1, &y1, &x2, &y2);
				pts[0] = x1;
				pts[1] = y1;
				pts[2] = x1;
				pts[3] = y2;
				pts[4] = x2;
				pts[5] = y2;
				pts[6] = x2;
				pts[7] = y1;
				pts[8] = x1;
				pts[9] = y1;
			}
			else
			{
				vimg->GetBounds(&x1, &y1, &x2, &y2);
				view->MapXYToScnXY(x1, y1, &x1, &y1);
				view->MapXYToScnXY(x2, y2, &x2, &y2);
				pts[0] = x1;
				pts[1] = y1;
				pts[2] = x1;
				pts[3] = y2;
				pts[4] = x2;
				pts[5] = y2;
				pts[6] = x2;
				pts[7] = y1;
				pts[8] = x1;
				pts[9] = y1;
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
	this->currWidth = 640;
	this->currHeight = 480;
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

	this->markerX = 0;
	this->markerY = 0;
	this->showMarker = false;
	this->selVec = 0;
	this->renderer = renderer;
	this->pauseUpdate = false;
	this->releaseRenderer = false;
	NEW_CLASS(this->drawMut, Sync::Mutex());

	this->view = view;
	view->SetDPI(this->hdpi, this->ddpi);
	view->ChangeViewXY(this->currWidth, this->currHeight, 114.2, 22.4, 10000);
	NEW_CLASS(this->scaleChgHdlrs, Data::ArrayList<ScaleChangedHandler>());
	NEW_CLASS(this->scaleChgObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->mapUpdHdlrs, Data::ArrayList<MapUpdatedHandler>());
	NEW_CLASS(this->mapUpdObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->mouseMoveHdlrs, Data::ArrayList<MouseMoveHandler>());
	NEW_CLASS(this->mouseMoveObjs, Data::ArrayList<void *>());
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
	this->currWidth = 640;
	this->currHeight = 480;
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

	this->markerX = 0;
	this->markerY = 0;
	this->showMarker = false;
	this->selVec = 0;
	Media::ColorProfile color(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->renderer, Map::DrawMapRenderer(this->eng, mapEnv, &color, this->colorSess, Map::DrawMapRenderer::DT_PIXELDRAW));
	this->releaseRenderer = true;
	NEW_CLASS(this->drawMut, Sync::Mutex());

	this->view = mapEnv->CreateMapView(640, 480);
	view->SetDPI(this->hdpi, this->ddpi);
	view->ChangeViewXY(this->currWidth, this->currHeight, 114.2, 22.4, 10000);
	NEW_CLASS(this->scaleChgHdlrs, Data::ArrayList<ScaleChangedHandler>());
	NEW_CLASS(this->scaleChgObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->mapUpdHdlrs, Data::ArrayList<MapUpdatedHandler>());
	NEW_CLASS(this->mapUpdObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->mouseMoveHdlrs, Data::ArrayList<MouseMoveHandler>());
	NEW_CLASS(this->mouseMoveObjs, Data::ArrayList<void *>());

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
	DEL_CLASS(this->scaleChgHdlrs);
	DEL_CLASS(this->scaleChgObjs);
	DEL_CLASS(this->mouseMoveHdlrs);
	DEL_CLASS(this->mouseMoveObjs);
	DEL_CLASS(this->mapUpdHdlrs);
	DEL_CLASS(this->mapUpdObjs);
	DEL_CLASS(this->drawMut);
	if (this->colorSess)
	{
		this->colorSess->RemoveHandler(this);
	}
}

void UI::GUIMapControl::OnSizeChanged(Bool updateScn)
{
	Sync::MutexUsage mutUsage(this->drawMut);
	this->GetSizeP(&this->currWidth, &this->currHeight);
	this->view->UpdateSize(this->currWidth, this->currHeight);
	if (this->bgImg)
	{
		this->eng->DeleteImage(this->bgImg);
		this->bgImg = 0;
	}
	if (this->currWidth > 0 && this->currHeight > 0)
	{
		this->bgImg = this->eng->CreateImage32(this->currWidth, this->currHeight, Media::AT_NO_ALPHA);
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

void UI::GUIMapControl::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
}

void UI::GUIMapControl::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
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
	UOSInt i = this->scaleChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->scaleChgHdlrs->GetItem(i)(this->scaleChgObjs->GetItem(i), newScale);
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
	if (this->bgImg && this->renderer)
	{
		Double centerX;
		Double centerY;
		Double t;
		UOSInt i;
		UInt32 imgDurMS;
		centerX = this->view->GetCenterX();
		centerY = this->view->GetCenterY();
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
		i = this->mapUpdHdlrs->GetCount();
		while (i-- > 0)
		{
			this->mapUpdHdlrs->GetItem(i)(this->mapUpdObjs->GetItem(i), centerX, centerY, t);
		}
	}
	mutUsage.EndUse();
}

void UI::GUIMapControl::ScnXY2MapXY(OSInt scnX, OSInt scnY, Double *mapX, Double *mapY)
{
	this->view->ScnXYToMapXY(OSInt2Double(scnX), OSInt2Double(scnY), mapX, mapY);
}

void UI::GUIMapControl::ScnXYD2MapXY(Double scnX, Double scnY, Double *mapX, Double *mapY)
{
	this->view->ScnXYToMapXY(scnX, scnY, mapX, mapY);
}

void UI::GUIMapControl::MapXY2ScnXY(Double mapX, Double mapY, OSInt *scnX, OSInt *scnY)
{
	Double x;
	Double y;
	this->view->MapXYToScnXY(mapX, mapY, &x, &y);
	if (this->mouseDown)
	{
		*scnX = Double2Int32(x) - this->mouseDownX + this->mouseCurrX;
		*scnY = Double2Int32(y) - this->mouseDownY + this->mouseCurrY;
	}
	else
	{
		*scnX = Double2Int32(x);
		*scnY = Double2Int32(y);
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

void UI::GUIMapControl::PanToMapXY(Double mapX, Double mapY)
{
	if (this->view->GetCenterX() == mapX && this->view->GetCenterY() == mapY)
		return;
	this->view->SetCenterXY(mapX, mapY);
	if (!this->pauseUpdate)
	{
		this->UpdateMap();
		this->Redraw();
	}
}

void UI::GUIMapControl::ZoomToRect(Double mapX1, Double mapY1, Double mapX2, Double mapY2)
{
	this->view->SetViewBounds(mapX1, mapY1, mapX2, mapY2);
	if (!this->pauseUpdate)
	{
		this->UpdateMap();
		this->Redraw();
	}
}

Bool UI::GUIMapControl::InMapMapXY(Double mapX, Double mapY)
{
	UOSInt w;
	UOSInt h;
	this->GetSizeP(&w, &h);
	if (w <= 0 || h <= 0)
		return true;
	return this->view->InViewXY(mapX, mapY);
}

void UI::GUIMapControl::ShowMarkerMapXY(Double mapX, Double mapY)
{
	this->markerX = mapX;
	this->markerY = mapY;
	this->markerHasDir = false;
	this->showMarker = true;
	if (!this->pauseUpdate)
	{
		this->Redraw();
	}
}

void UI::GUIMapControl::ShowMarkerMapXYDir(Double mapX, Double mapY, Double dir, Math::Unit::Angle::AngleUnit unit)
{
	this->markerX = mapX;
	this->markerY = mapY;
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

void UI::GUIMapControl::SetSelectedVector(Math::Vector2D *vec)
{
	if (this->selVec)
	{
		DEL_CLASS(this->selVec);
		this->selVec = 0;
	}
	this->selVec = vec;
	this->Redraw();
}

void UI::GUIMapControl::HandleScaleChanged(ScaleChangedHandler hdlr, void *userObj)
{
	this->scaleChgHdlrs->Add(hdlr);
	this->scaleChgObjs->Add(userObj);
	hdlr(userObj, this->view->GetMapScale());
}

void UI::GUIMapControl::HandleMapUpdated(MapUpdatedHandler hdlr, void *userObj)
{
	this->mapUpdHdlrs->Add(hdlr);
	this->mapUpdObjs->Add(userObj);
}

void UI::GUIMapControl::HandleMouseMove(MouseMoveHandler hdlr, void *userObj)
{
	this->mouseMoveHdlrs->Add(hdlr);
	this->mouseMoveObjs->Add(userObj);
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
		this->view->UpdateSize(this->currWidth, this->currHeight);
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
