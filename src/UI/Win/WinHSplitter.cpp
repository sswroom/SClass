#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinHSplitter.h"

#define CLASSNAME L"HSplitter"
Int32 UI::Win::WinHSplitter::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

#if defined(_WIN32_WCE)
#define GetWindowLongPtr(a, b) GetWindowLongW(a, b)
#endif

OSInt __stdcall UI::Win::WinHSplitter::FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::Win::WinHSplitter *me = (UI::Win::WinHSplitter*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	POINT pt;
	HDC hdc;

	if (me == 0)
	{
		return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	}
	switch (msg)
	{
	case WM_PAINT:
		break;
	case WM_LBUTTONDOWN:
#ifdef _WIN32_WCE
		pt.x = (Int16)LOWORD(lParam);
		pt.y = (Int16)HIWORD(lParam);
#else
		GetCursorPos(&pt);
#endif
		me->EventMouseDown(GUIControl::MouseButton::MBTN_LEFT, Math::Coord2D<OSInt>(pt.x, pt.y));
		return 0;
	case WM_LBUTTONUP:
#ifdef _WIN32_WCE
		pt.x = (Int16)LOWORD(lParam);
		pt.y = (Int16)HIWORD(lParam);
#else
		GetCursorPos(&pt);
#endif
		me->EventMouseUp(GUIControl::MouseButton::MBTN_LEFT, Math::Coord2D<OSInt>(pt.x, pt.y));
		return 0;
	case WM_MOUSEMOVE:
		if (me->dragMode)
		{
#ifdef _WIN32_WCE
			pt.x = (Int16)LOWORD(lParam);
			pt.y = (Int16)HIWORD(lParam);
#else
			GetCursorPos(&pt);
#endif

			NN<GUIClientControl> nnparent;
			if (me->parent.SetTo(nnparent))
			{
				hdc = GetDC((HWND)nnparent->GetHandle());

				me->DrawXorBar(hdc, me->lastX, me->lastY);
				me->DrawXorBar(hdc, pt.x, pt.y);

				ReleaseDC((HWND)nnparent->GetHandle(), hdc);
			}
			me->lastX = pt.x;
			me->lastY = pt.y;
		}
		return 0;
	case WM_MOUSEWHEEL:
		return 0;
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::Win::WinHSplitter::Init(InstanceHandle *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::Win::WinHSplitter::FormWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst; 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE)0, IDC_SIZEWE); 
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1); 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::Win::WinHSplitter::Deinit(InstanceHandle *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

void UI::Win::WinHSplitter::DrawXorBar(HDC hdc, OSInt x, OSInt y)
{
	static UInt16 _dotPatternBmp[8] = {0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55};

	HBITMAP hbm;
	HBRUSH hbr;
	HBRUSH hbrushOld;
	OSInt drawX;

	Math::Size2D<UOSInt> sz;
	Math::Coord2D<OSInt> pos = this->GetPositionP();
	sz = this->GetSizeP();
	drawX = pos.x + x - this->dragX;
	if (drawX < dragMin)
	{
		drawX = dragMin;
	}
	else if (drawX > dragMax)
	{
		drawX = dragMax;
	}

	hbm = CreateBitmap(8, 8, 1, 1, _dotPatternBmp);
	hbr = CreatePatternBrush(hbm);

	SetBrushOrgEx(hdc, (int)drawX, (int)pos.y, 0);
	hbrushOld = (HBRUSH)SelectObject(hdc, hbr);

	PatBlt(hdc, (int)drawX, (int)pos.y, (int)sz.x, (int)sz.y, PATINVERT);
	
	SelectObject(hdc, hbrushOld);
	DeleteObject(hbr);
	DeleteObject(hbm);
}

void UI::Win::WinHSplitter::CalDragRange()
{
	UOSInt i;
	OSInt max;
	OSInt min;
	Bool foundLeft = false;
	Bool foundRight = false;
	Bool foundThis = false;
	NN<UI::GUIControl> ctrl;
	UI::GUIControl::DockType dockType;
	min = 0;
	NN<GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		Double maxD = nnparent->GetClientSize().x;
		max = Double2Int32(maxD * this->hdpi / 96.0);

		i = nnparent->GetChildCount();
		if (this->isRight)
		{
			while (i-- > 0)
			{
				if (nnparent->GetChild(i).SetTo(ctrl))
				{
					if (ctrl.Ptr() == this)
					{
						foundThis = true;
					}
					else
					{
						dockType = ctrl->GetDockType();
						if (dockType == UI::GUIControl::DOCK_RIGHT)
						{
							if (foundThis && !foundRight)
							{
								foundRight = true;
								max = ctrl->GetPositionP().x;
								max += (OSInt)ctrl->GetSizeP().x;
							}
						}
						else if (dockType == UI::GUIControl::DOCK_LEFT)
						{
							if (!foundLeft)
							{
								foundLeft = true;
								min = ctrl->GetPositionP().x;
								min += (OSInt)ctrl->GetSizeP().x;
							}
						}
					}
				}
			}
		}
		else
		{
			while (i-- > 0)
			{
				if (nnparent->GetChild(i).SetTo(ctrl))
				{
					if (ctrl.Ptr() == this)
					{
						foundThis = true;
					}
					else
					{
						dockType = ctrl->GetDockType();
						if (dockType == UI::GUIControl::DOCK_RIGHT)
						{
							if (!foundRight)
							{
								foundRight = true;
								max = ctrl->GetPositionP().x;
							}
						}
						else if (dockType == UI::GUIControl::DOCK_LEFT)
						{
							if (foundThis && !foundLeft)
							{
								foundLeft = true;
								min = ctrl->GetPositionP().x;
							}
						}
					}
				}
			}
		}
		this->dragMax = max;
		this->dragMin = min;
	}
}

UI::Win::WinHSplitter::WinHSplitter(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Int32 width, Bool isRight) : UI::GUIHSplitter(ui, parent)
{
	this->dragMode = false;
	this->isRight = isRight;

	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		Init(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}

	UInt32 style = WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst(), parent, CLASSNAME, 0, style, 0, 0, 0, width, 10);
	this->SetDockType(isRight?DOCK_RIGHT:DOCK_LEFT);
}

UI::Win::WinHSplitter::~WinHSplitter()
{
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
		Deinit(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}
}

OSInt UI::Win::WinHSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Win::WinHSplitter::EventMouseDown(GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
	if (btn == GUIControl::MouseButton::MBTN_LEFT)
	{
		this->dragMode = true;
		this->dragX = pos.x;
		this->dragY = pos.y;
		this->lastX = pos.x;
		this->lastY = pos.y;
		this->CalDragRange();
		this->SetCapture();
		
		NN<GUIClientControl> nnparent;
		if (this->parent.SetTo(nnparent))
		{
			HDC hdc = GetDC((HWND)nnparent->GetHandle());
			this->DrawXorBar(hdc, pos.x, pos.y);
			ReleaseDC((HWND)nnparent->GetHandle(), hdc);
		}
	}
}

void UI::Win::WinHSplitter::EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
	if (btn == GUIControl::MouseButton::MBTN_LEFT && this->dragMode)
	{
		OSInt drawX;
		Math::Coord2D<OSInt> pos;
		Math::Size2D<UOSInt> sz;
		Bool foundThis;
		UI::GUIControl::DockType dockType;

		this->dragMode = false;
		this->ReleaseCapture();

		NN<GUIClientControl> nnparent;
		if (this->parent.SetTo(nnparent))
		{
			HDC hdc = GetDC((HWND)nnparent->GetHandle());
			this->DrawXorBar(hdc, this->lastX, this->lastY);
			ReleaseDC((HWND)nnparent->GetHandle(), hdc);
		}

		pos = this->GetPositionP();
		drawX = pos.x + this->lastX - this->dragX;
		if (drawX < this->dragMin)
		{
			drawX = this->dragMin;
		}
		else if (drawX > this->dragMax)
		{
			drawX = this->dragMax;
		}
		foundThis = false;
		if (this->parent.SetTo(nnparent))
		{
			UOSInt i = nnparent->GetChildCount();
			while (i-- > 0)
			{
				NN<UI::GUIControl> ctrl;
				if (nnparent->GetChild(i).SetTo(ctrl))
				{
					if (ctrl.Ptr() == this)
					{
						foundThis = true;
					}
					else if (foundThis)
					{
						dockType = ctrl->GetDockType();
						if (dockType == UI::GUIControl::DOCK_RIGHT && this->isRight)
						{
							pos = ctrl->GetPositionP();
							sz = ctrl->GetSizeP();
							ctrl->SetAreaP(drawX, pos.y, pos.x + (OSInt)sz.x, pos.y + (OSInt)sz.y, false);
							nnparent->UpdateChildrenSize(true);
							break;
						}
						else if (dockType == UI::GUIControl::DOCK_LEFT && !this->isRight)
						{
							pos = ctrl->GetPositionP();
							sz = ctrl->GetSizeP();
							ctrl->SetAreaP(pos.x, pos.y, drawX, pos.y + (OSInt)sz.y, false);
							nnparent->UpdateChildrenSize(true);
							break;
						}
					}
				}
			}
		}
	}
}
