#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinVSplitter.h"

#define CLASSNAME L"VSplitter"
Int32 UI::Win::WinVSplitter::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

#if defined(_WIN32_WCE)
#define GetWindowLongPtr(a, b) GetWindowLongW(a, b)
#endif

OSInt __stdcall UI::Win::WinVSplitter::FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::Win::WinVSplitter *me = (UI::Win::WinVSplitter*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	POINT pt;
	HDC hdc;
	UOSInt i;
	NotNullPtr<UI::GUIControl> ctrl;

	if (me == 0)
	{
		return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	}
	switch (msg)
	{
	case WM_PAINT:
		break;
	case WM_LBUTTONDOWN:
		GetCursorPos(&pt);
		me->EventMouseDown(GUIControl::MouseButton::MBTN_LEFT, Math::Coord2D<OSInt>(pt.x, pt.y));
		return 0;
	case WM_LBUTTONUP:
		if (me->dragMode)
		{
			OSInt drawY;
			Math::Size2D<UOSInt> sz;
			Bool foundThis;
			UI::GUIControl::DockType dockType;

			me->dragMode = false;
			me->ReleaseCapture();

			NotNullPtr<UI::GUIClientControl> nnparent;
			if (me->parent.SetTo(nnparent))
			{
				hdc = GetDC((HWND)nnparent->GetHandle());
				me->DrawXorBar(hdc, me->lastX, me->lastY);
				ReleaseDC((HWND)nnparent->GetHandle(), hdc);
			}

			Math::Coord2D<OSInt> pos = me->GetPositionP();
			drawY = pos.y + me->lastY - me->dragY;
			if (drawY < me->dragMin)
			{
				drawY = me->dragMin;
			}
			else if (drawY > me->dragMax)
			{
				drawY = me->dragMax;
			}
			foundThis = false;
			if (me->parent.SetTo(nnparent))
			{
				i = nnparent->GetChildCount();
				while (i-- > 0)
				{
					if (nnparent->GetChild(i).SetTo(ctrl))
					{
						if (ctrl.Ptr() == me)
						{
							foundThis = true;
						}
						else if (foundThis)
						{
							dockType = ctrl->GetDockType();
							if (dockType == UI::GUIControl::DOCK_BOTTOM && me->isBottom)
							{
								pos = ctrl->GetPositionP();
								sz = ctrl->GetSizeP();
								ctrl->SetAreaP(pos.x, drawY, pos.x + (OSInt)sz.x, pos.y + (OSInt)sz.y, false);
								nnparent->UpdateChildrenSize(true);
								break;
							}
							else if (dockType == UI::GUIControl::DOCK_TOP && !me->isBottom)
							{
								pos = ctrl->GetPositionP();
								sz = ctrl->GetSizeP();
								ctrl->SetAreaP(pos.x, pos.y, pos.x + (OSInt)sz.x, drawY, false);
								nnparent->UpdateChildrenSize(true);
								break;
							}
						}
					}
				}
			}
		}
		return 0;
	case WM_MOUSEMOVE:
		if (me->dragMode)
		{
			GetCursorPos(&pt);

			NotNullPtr<UI::GUIClientControl> nnparent;
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

void UI::Win::WinVSplitter::Init(InstanceHandle *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::Win::WinVSplitter::FormWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst; 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE)0, IDC_SIZENS); 
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1); 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::Win::WinVSplitter::Deinit(InstanceHandle *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

void UI::Win::WinVSplitter::DrawXorBar(HDC hdc, OSInt x, OSInt y)
{
	static UInt16 _dotPatternBmp[8] = {0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55};

	HBITMAP hbm;
	HBRUSH hbr;
	HBRUSH hbrushOld;
	OSInt drawY;

	OSInt cliOfstX = 0;
	OSInt cliOfstY = 0;
	Math::Size2D<UOSInt> sz;
	Math::Coord2DDbl lcliOfst;
	NotNullPtr<UI::GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		lcliOfst = nnparent->GetClientOfst();
	}
	else
	{
		lcliOfst = Math::Coord2DDbl(0, 0);
	}
	cliOfstX = Double2Int32(lcliOfst.x * this->hdpi / 96.0);
	cliOfstY = Double2Int32(lcliOfst.y * this->hdpi / 96.0);
	Math::Coord2D<OSInt> pos = this->GetPositionP();
	sz = this->GetSizeP();
	drawY = pos.y + y - this->dragY;
	if (drawY < dragMin)
	{
		drawY = dragMin;
	}
	else if (drawY > dragMax)
	{
		drawY = dragMax;
	}

	hbm = CreateBitmap(8, 8, 1, 1, _dotPatternBmp);
	hbr = CreatePatternBrush(hbm);

	SetBrushOrgEx(hdc, (int)(pos.x + cliOfstX), (int)(drawY + cliOfstY), 0);
	hbrushOld = (HBRUSH)SelectObject(hdc, hbr);

	PatBlt(hdc, (int)(pos.x + cliOfstX), (int)(drawY + cliOfstY), (int)sz.x, (int)sz.y, PATINVERT);
	
	SelectObject(hdc, hbrushOld);
	DeleteObject(hbr);
	DeleteObject(hbm);
}

void UI::Win::WinVSplitter::CalDragRange()
{
	UOSInt i;
	OSInt max;
	OSInt min;
	Bool foundTop = false;
	Bool foundBottom = false;
	Bool foundThis = false;
	NotNullPtr<UI::GUIControl> ctrl;
	UI::GUIControl::DockType dockType;
	min = 0;
	NotNullPtr<UI::GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		Double maxD = nnparent->GetClientSize().x;
		max = Double2Int32(maxD * this->hdpi / 96.0);

		i = nnparent->GetChildCount();
		if (this->isBottom)
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
						if (dockType == UI::GUIControl::DOCK_BOTTOM)
						{
							if (foundThis && !foundBottom)
							{
								foundBottom = true;
								max = ctrl->GetPositionP().y;
								max += (OSInt)ctrl->GetSizeP().y;
							}
						}
						else if (dockType == UI::GUIControl::DOCK_TOP)
						{
							if (!foundTop)
							{
								foundTop = true;
								min = ctrl->GetPositionP().y;
								min += (OSInt)ctrl->GetSizeP().y;
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
						if (dockType == UI::GUIControl::DOCK_BOTTOM)
						{
							if (!foundBottom)
							{
								foundBottom = true;
								max = ctrl->GetPositionP().y;
							}
						}
						else if (dockType == UI::GUIControl::DOCK_TOP)
						{
							if (foundThis && !foundTop)
							{
								foundTop = true;
								min = ctrl->GetPositionP().y;
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

UI::Win::WinVSplitter::WinVSplitter(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Int32 height, Bool isBottom) : UI::GUIVSplitter(ui, parent)
{
	this->dragMode = false;
	this->isBottom = isBottom;

	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		Init(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}

	UInt32 style = WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst(), parent, CLASSNAME, 0, style, 0, 0, 0, 10, height);
	this->SetDockType(isBottom?DOCK_BOTTOM:DOCK_TOP);
}

UI::Win::WinVSplitter::~WinVSplitter()
{
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
		Deinit(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}
}

OSInt UI::Win::WinVSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Win::WinVSplitter::EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
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
		
		{
			NotNullPtr<UI::GUIClientControl> nnparent;
			if (this->parent.SetTo(nnparent))
			{
				HDC hdc = GetDC((HWND)nnparent->GetHandle());
				this->DrawXorBar(hdc, pos.x, pos.y);
				ReleaseDC((HWND)nnparent->GetHandle(), hdc);
			}
		}
	}
}

void UI::Win::WinVSplitter::EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{

}
