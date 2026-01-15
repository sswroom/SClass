#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Manage/HiResClock.h"
#include "Map/MapDrawUtil.h"
#include "Math/Math_C.h"
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

#include <stdio.h>

void __stdcall UI::GUIMapControl::ImageUpdated(AnyType userObj)
{
	NN<UI::GUIMapControl> me = userObj.GetNN<UI::GUIMapControl>();
	me->bgUpdated = true;
	if (!me->pauseUpdate)
	{
		me->Redraw();
	}
}

UI::EventState UI::GUIMapControl::OnMouseDown(Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		this->SetCapture();
		if (this->mouseDownHdlr.func)
		{
			UI::EventState eventState = this->mouseDownHdlr.func(this->mouseDownHdlr.userObj, scnPos, btn);
			if (eventState == UI::EventState::StopEvent)
			{
				return UI::EventState::StopEvent;
			}
		}
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		Int64 currTime = dt.ToTicks();
		if (currTime - this->mouseLDownTime < 250)
		{
			this->OnMouseWheel(scnPos, 1);
			this->mouseLDownTime = 0;
			return UI::EventState::StopEvent;
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
		this->SetCapture();
		if (this->mouseDownHdlr.func)
		{
			UI::EventState eventState = this->mouseDownHdlr.func(this->mouseDownHdlr.userObj, scnPos, btn);
			if (eventState == UI::EventState::StopEvent)
			{
				return UI::EventState::StopEvent;
			}
		}
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		Int64 currTime = dt.ToTicks();
		if (currTime - this->mouseRDownTime < 250)
		{
			this->OnMouseWheel(scnPos, -1);
			this->mouseRDownTime = 0;
			return UI::EventState::StopEvent;
		}
		else
		{
			this->mouseRDownTime = currTime;
		}	
	}
	return UI::EventState::ContinueEvent;
}

UI::EventState UI::GUIMapControl::OnMouseUp(Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		this->ReleaseCapture();
		if (this->mouseUpHdlr.func)
		{
			UI::EventState eventState = this->mouseUpHdlr.func(this->mouseUpHdlr.userObj, scnPos, btn);
			if (eventState == UI::EventState::StopEvent)
			{
				return UI::EventState::StopEvent;
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
	else if (btn == UI::GUIControl::MBTN_RIGHT)
	{
		this->ReleaseCapture();
		if (this->mouseUpHdlr.func)
		{
			UI::EventState eventState = this->mouseUpHdlr.func(this->mouseUpHdlr.userObj, scnPos, btn);
			if (eventState == UI::EventState::StopEvent)
			{
				return UI::EventState::StopEvent;
			}
		}
	}
	return UI::EventState::ContinueEvent;
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
			Data::CallbackStorage<MouseMoveHandler> cb = this->mouseMoveHdlrs.GetItem(i);
			cb.func(cb.userObj, scnPos);
		}
	}
/*	if (me->IsFormFocused())
	{
		SetFocus((HWND)hWnd);
	}*/
}

UI::EventState UI::GUIMapControl::OnMouseWheel(Math::Coord2D<OSInt> scnPos, Int32 delta)
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
	NN<Map::MapEnv> mapEnv;
	if (!this->mapEnv.SetTo(mapEnv))
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
	return UI::EventState::StopEvent;
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
		NN<Map::MapEnv> mapEnv;
		if (!this->mapEnv.SetTo(mapEnv))
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

void UI::GUIMapControl::OnDraw(NN<Media::DrawImage> img)
{
	Math::Coord2DDbl tl;
	NN<Media::DrawImage> bgImg;
	if (!this->bgImg.SetTo(bgImg))
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
		NN<Media::StaticImage> drawImg;
		Optional<Media::StaticImage> srcImg;
		NN<Media::StaticImage> nnsrcImg;
		Media::Resizer::LanczosResizerH8_8 resizer(4, 3, Media::AT_ALPHA_ALL_FF);
		Math::Size2D<UOSInt> sz;

		Sync::MutexUsage mutUsage(this->drawMut);
		if (this->drawHdlr.func)
		{
			NN<Media::DrawImage> tmpImg;
			if (this->eng->CloneImage(bgImg).SetTo(tmpImg))
			{
				this->drawHdlr.func(this->drawHdlr.userObj, tmpImg, 0, 0);
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
			Double rate = (Double)this->gZoomDist / (Double)this->gZoomCurrDist;
			Math::Size2DDbl srcSize = this->currSize.ToDouble() * rate;
			Math::Coord2DDbl srcPos = this->gZoomPos.ToDouble() - this->gZoomCurrPos.ToDouble() * rate;

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
			if (srcImg.SetTo(nnsrcImg))
			{
				NEW_CLASSNN(drawImg, Media::StaticImage(this->currSize, 0, 32, Media::PF_B8G8R8A8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_BT601, nnsrcImg->info.atype, Media::YCOFST_C_CENTER_LEFT));
				drawImg->info.hdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
				drawImg->info.vdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
				drawImg->info.color.Set(this->colorSess->GetRGBParam()->monProfile);
				resizer.Resize(nnsrcImg->data + (srcIX * 4) + (srcIY * (OSInt)(nnsrcImg->info.storeSize.x * 4)), (OSInt)nnsrcImg->info.storeSize.x * 4, srcSize.x, srcSize.y, srcPos.x - srcIX, srcPos.y - srcIY, drawImg->data, (OSInt)drawImg->info.storeSize.x * 4, sz.x, sz.y);
				mutUsage.EndUse();
				nnsrcImg.Delete();
				img->DrawImagePt2(drawImg, tl);
				drawImg.Delete();
			}
		}
		else
		{
			Double rate = (Double)this->gZoomCurrDist / (Double)this->gZoomDist;
			sz.x = (UOSInt)Double2OSInt(UOSInt2Double(this->currSize.x) * rate);
			sz.y = (UOSInt)Double2OSInt(UOSInt2Double(this->currSize.y) * rate);
			tl = this->gZoomCurrPos.ToDouble() - this->gZoomPos.ToDouble() * rate;
			if (srcImg.SetTo(nnsrcImg))
			{
				NEW_CLASSNN(drawImg, Media::StaticImage(this->currSize, 0, 32, Media::PF_B8G8R8A8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_BT601, nnsrcImg->info.atype, Media::YCOFST_C_CENTER_LEFT));
				drawImg->info.hdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
				drawImg->info.vdpi = this->view->GetHDPI() / this->view->GetDDPI() * 96.0;
				drawImg->info.color.Set(this->colorSess->GetRGBParam()->monProfile);
				resizer.Resize(nnsrcImg->data, (OSInt)nnsrcImg->info.storeSize.x * 4, UOSInt2Double(this->currSize.x), UOSInt2Double(this->currSize.y), 0, 0, drawImg->data, (OSInt)drawImg->info.storeSize.x * 4, sz.x, sz.y);
				mutUsage.EndUse();
				nnsrcImg.Delete();
				img->DrawImagePt2(drawImg, tl);
				drawImg.Delete();
			}
		}

		NN<Media::DrawBrush> bgBrush = img->NewBrushARGB(this->bgColor);
		if (tl.x > 0)
		{
			img->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(tl.x, UOSInt2Double(this->currSize.y)), nullptr, bgBrush);
		}
		if (tl.x + UOSInt2Double(sz.x) < UOSInt2Double(this->currSize.x))
		{
			img->DrawRect(Math::Coord2DDbl(UOSInt2Double(sz.x) + tl.x, 0), Math::Size2DDbl(UOSInt2Double(this->currSize.x - sz.x) - tl.x, UOSInt2Double(this->currSize.y)), nullptr, bgBrush);
		}
		if (tl.y > 0)
		{
			img->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(UOSInt2Double(this->currSize.x), tl.y), nullptr, bgBrush);
		}
		if (tl.y + UOSInt2Double(sz.y) < UOSInt2Double(this->currSize.y))
		{
			img->DrawRect(Math::Coord2DDbl(0, UOSInt2Double(sz.y) + tl.y), Math::Size2DDbl(UOSInt2Double(this->currSize.x), UOSInt2Double(this->currSize.y - sz.y) - tl.y), nullptr, bgBrush);
		}
		img->DelBrush(bgBrush);
	}
	else
	{
		NN<Media::DrawBrush> bgBrush = img->NewBrushARGB(this->bgColor);
		if (tl.x > 0)
		{
			img->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(tl.x, UOSInt2Double(this->currSize.y)), nullptr, bgBrush);
		}
		else if (tl.x < 0)
		{
			img->DrawRect(Math::Coord2DDbl(UOSInt2Double(this->currSize.x) + tl.x, 0), Math::Size2DDbl(-tl.x, UOSInt2Double(this->currSize.y)), nullptr, bgBrush);
		}
		if (tl.y > 0)
		{
			img->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(UOSInt2Double(this->currSize.x), tl.y), nullptr, bgBrush);
		}
		else if (tl.y < 0)
		{
			img->DrawRect(Math::Coord2DDbl(0, UOSInt2Double(this->currSize.y) + tl.y), Math::Size2DDbl(UOSInt2Double(this->currSize.x), -tl.y), nullptr, bgBrush);
		}
		img->DelBrush(bgBrush);

		Sync::MutexUsage mutUsage(this->drawMut);
		if (this->drawHdlr.func)
		{
			NN<Media::DrawImage> drawImg;
			if (this->eng->CloneImage(bgImg).SetTo(drawImg))
			{
				this->drawHdlr.func(this->drawHdlr.userObj, drawImg, 0, 0);
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

void UI::GUIMapControl::DrawScnObjects(NN<Media::DrawImage> img, Math::Coord2DDbl ofst)
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
		NN<Media::DrawPen> p = img->NewPenARGB(0xffff0000, 3 * hdpi / ddpi, nullptr, 0);
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
	if (j > 0)
	{
		NN<Media::DrawPen> p = img->NewPenARGB(0xffff0000, 3, nullptr, 0);
		NN<Media::DrawBrush> b = img->NewBrushARGB(0x403f0000);
		NN<Math::Geometry::Vector2D> vec;
		while (i < j)
		{
			if (this->selVecList.GetItem(i).SetTo(vec))
			{
				Map::MapDrawUtil::DrawVector(vec, img, view, b, p, ofst);
			}
			i++;
		}
		img->DelBrush(b);
		img->DelPen(p);
	}
}

void UI::GUIMapControl::ReleaseSelVecList()
{
	this->selVecList.DeleteAll();
}

UI::GUIMapControl::GUIMapControl(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UInt32 bgColor, NN<Map::DrawMapRenderer> renderer, NN<Map::MapView> view, NN<Media::ColorManagerSess> colorSess) : UI::GUICustomDraw(ui, parent, eng, colorSess)
{
	this->colorSess = colorSess;
	this->colorSess->AddHandler(*this);
	this->currSize = Math::Size2D<UOSInt>(640, 480);
	this->bgImg = nullptr;
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
	this->mapEnv = nullptr;
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
	this->renderer->SetUpdatedHandler(ImageUpdated, this);
}

UI::GUIMapControl::GUIMapControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, NN<Map::MapEnv> mapEnv, NN<Media::ColorManagerSess> colorSess) : UI::GUICustomDraw(ui, parent, eng, colorSess)
{
	this->colorSess = colorSess;
	this->colorSess->AddHandler(*this);
	this->currSize = Math::Size2D<UOSInt>(640, 480);
	this->bgImg = nullptr;
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
	this->mapEnv = mapEnv;
	this->imgTimeoutTick = 0;

	this->markerPos = Math::Coord2DDbl(0, 0);
	this->showMarker = false;
	this->pauseUpdate = false;
	Media::ColorProfile color(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASSNN(this->renderer, Map::DrawMapRenderer(this->eng, mapEnv, color, this->colorSess, Map::DrawMapRenderer::DT_PIXELDRAW));
	this->releaseRenderer = true;

	this->view = mapEnv->CreateMapView(Math::Size2DDbl(640, 480));
	view->SetDPI(this->hdpi, this->ddpi);
	view->ChangeViewXY(this->currSize.ToDouble(), Math::Coord2DDbl(114.2, 22.4), 10000);

	this->SetBGColor(bgColor);
	this->renderer->SetUpdatedHandler(ImageUpdated, this);
	mapEnv->AddUpdatedHandler(ImageUpdated, this);
}

UI::GUIMapControl::~GUIMapControl()
{
	NN<Map::MapEnv> mapEnv;
	if (this->mapEnv.SetTo(mapEnv))
	{
		mapEnv->RemoveUpdatedHandler(ImageUpdated, this);
	}
	NN<Media::DrawImage> img;
	if (this->bgImg.SetTo(img))
	{
		this->eng->DeleteImage(img);
	}
	this->ReleaseSelVecList();
	this->renderer->SetUpdatedHandler(0, 0);

	if (this->releaseRenderer)
	{
		this->renderer.Delete();
	}
	this->view.Delete();
	this->colorSess->RemoveHandler(*this);
}

void UI::GUIMapControl::OnSizeChanged(Bool updateScn)
{
	Sync::MutexUsage mutUsage(this->drawMut);
	this->currSize = this->GetSizeP();
	this->view->UpdateSize(this->currSize.ToDouble());
	NN<Media::DrawImage> img;
	if (this->bgImg.SetTo(img))
	{
		this->eng->DeleteImage(img);
		this->bgImg = nullptr;
	}
	if (this->currSize.x > 0 && this->currSize.y > 0)
	{
		this->bgImg = this->eng->CreateImage32(this->currSize, Media::AT_ALPHA_ALL_FF);
		if (this->bgImg.SetTo(img))
		{
			img->SetHDPI(this->view->GetHDPI() / this->view->GetDDPI() * 96.0);
			img->SetVDPI(this->view->GetHDPI() / this->view->GetDDPI() * 96.0);
			img->SetColorProfile(this->colorSess->GetRGBParam()->monProfile);
			img->SetColorSess(this->colorSess);
		}
	}
	mutUsage.EndUse();
	this->UpdateMap();

	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->resizeHandlers.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUIMapControl::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void UI::GUIMapControl::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	this->renderer->ColorUpdated();
	this->SetBGColor(this->bgColor);
	NN<Media::DrawImage> img;
	if (this->bgImg.SetTo(img))
	{
		img->SetColorProfile(rgbParam->monProfile);
	}
}

void UI::GUIMapControl::SetDPI(Double hdpi, Double ddpi)
{
	this->hdpi = hdpi;
	this->ddpi = ddpi;
	this->view->SetDPI(hdpi, ddpi);
	NN<Media::DrawImage> img;
	if (this->bgImg.SetTo(img))
	{
		img->SetHDPI(hdpi / ddpi * 96.0);
		img->SetVDPI(hdpi / ddpi * 96.0);
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
		Data::CallbackStorage<ScaleChangedHandler> cb = this->scaleChgHdlrs.GetItem(i);
		cb.func(cb.userObj, newScale);
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

void UI::GUIMapControl::SetRenderer(NN<Map::DrawMapRenderer> renderer)
{
	Sync::MutexUsage mutUsage(this->drawMut);
	this->renderer->SetUpdatedHandler(0, 0);
	this->renderer = renderer;
	mutUsage.EndUse();
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
	NN<Media::DrawImage> bgImg;
	if (this->bgImg.SetTo(bgImg))
	{
		Double t;
		UOSInt i;
		UInt32 imgDurMS;
		Math::Coord2DDbl center = this->view->GetCenter();
		Manage::HiResClock clk;
		NN<Media::DrawBrush> b = bgImg->NewBrushARGB(this->bgDispColor);
		bgImg->DrawRect(Math::Coord2DDbl(0, 0), bgImg->GetSize().ToDouble(), nullptr, b);
		bgImg->DelBrush(b);
		this->renderer->DrawMap(bgImg, this->view, imgDurMS);
		t = clk.GetTimeDiff();
		if (imgDurMS != 0)
		{
			this->imgTimeoutTick = Data::DateTimeUtil::GetCurrTimeMillis() + imgDurMS - Double2Int32(t * 1000);
		}
		else
		{
			this->imgTimeoutTick = 0;
		}
		i = this->mapUpdHdlrs.GetCount();
		while (i-- > 0)
		{
			Data::CallbackStorage<MapUpdatedHandler> cb = this->mapUpdHdlrs.GetItem(i);
			cb.func(cb.userObj, center, t);
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
	NN<Map::MapEnv> mapEnv;
	if (!this->mapEnv.SetTo(mapEnv))
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

void UI::GUIMapControl::SetSelectedVector(Optional<Math::Geometry::Vector2D> vec)
{
	this->ReleaseSelVecList();
	NN<Math::Geometry::Vector2D> nnvec;
	if (vec.SetTo(nnvec))
	{
		this->selVecList.Add(nnvec);
	}
	this->Redraw();
}

void UI::GUIMapControl::SetSelectedVectors(NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList)
{
	this->ReleaseSelVecList();
	this->selVecList.AddAll(vecList);
	this->Redraw();
}

void UI::GUIMapControl::SetVAngle(Double angleRad)
{
	this->view->SetVAngle(angleRad);
}

void UI::GUIMapControl::SetHAngle(Double angleRad)
{
	this->view->SetHAngle(angleRad);
}

void UI::GUIMapControl::HandleScaleChanged(ScaleChangedHandler hdlr, AnyType userObj)
{
	this->scaleChgHdlrs.Add({hdlr, userObj});
	hdlr(userObj, this->view->GetMapScale());
}

void UI::GUIMapControl::HandleMapUpdated(MapUpdatedHandler hdlr, AnyType userObj)
{
	this->mapUpdHdlrs.Add({hdlr, userObj});
}

void UI::GUIMapControl::HandleMouseMove(MouseMoveHandler hdlr, AnyType userObj)
{
	this->mouseMoveHdlrs.Add({hdlr, userObj});
}

void UI::GUIMapControl::HandleMouseUp(MouseEventHandler hdlr, AnyType userObj)
{
	this->mouseUpHdlr = {hdlr, userObj};
}

void UI::GUIMapControl::HandleMouseDown(MouseEventHandler hdlr, AnyType userObj)
{
	this->mouseDownHdlr = {hdlr, userObj};
}

void UI::GUIMapControl::HandleCustomDraw(DrawHandler hdlr, AnyType userObj)
{
	this->drawHdlr = {hdlr, userObj};
}

void UI::GUIMapControl::SetMapUpdated()
{
	this->bgUpdated = true;
}

void UI::GUIMapControl::UpdateMapView(NN<Map::MapView> view)
{
	this->view.Delete();
	this->view = view;
	this->view->UpdateSize(this->currSize.ToDouble());
	this->EventScaleChanged(this->view->GetMapScale());
}

NN<Map::MapView> UI::GUIMapControl::CloneMapView()
{
	return this->view->Clone();
}

void UI::GUIMapControl::PauseUpdate(Bool pauseUpdate)
{
	this->pauseUpdate = pauseUpdate;
}
