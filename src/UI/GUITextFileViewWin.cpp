#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/Clipboard.h"
#include "UI/GUICoreWin.h"
#include "UI/GUITextFileView.h"
#include "UI/MessageDialog.h"
#include <windows.h>

#define READBUFFSIZE 1048576
#define CLASSNAME L"TextFileView"
#define BGBRUSH ((HBRUSH)COLOR_WINDOWFRAME)
#define HK_PAGEUP 1
#define HK_PAGEDOWN 2
#define HK_LINEUP 3
#define HK_LINEDOWN 4
#define HK_HOME 5
#define HK_END 6
#define HK_COPY 7
#define HK_COPY_OLD 8
#define HK_LEFT 9
#define HK_RIGHT 10
#define HK_LINEHOME 11
#define HK_LINEEND 12

#define HK_SPAGEUP 13
#define HK_SPAGEDOWN 14
#define HK_SLINEUP 15
#define HK_SLINEDOWN 16
#define HK_SHOME 17
#define HK_SEND 18
#define HK_SLEFT 19
#define HK_SRIGHT 20
#define HK_SLINEHOME 21
#define HK_SLINEEND 22

OSInt UI::GUITextFileView::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::GUITextFileView::TFVWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam)
{
	UI::GUITextFileView *me = (UI::GUITextFileView*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	UI::GUIControl*ctrl;
	UOSInt currLineCnt;
	NMHDR *nmhdr;
	SCROLLINFO si;
	UInt32 textXPos;
	UOSInt textYPos;
	Int32 mouseXPos;
	Int32 mouseYPos;
	Int16 shiftLock;
	RECT rc;
	HDC hdc;
	Int16 scrollLock;
	switch (msg)
	{
	case WM_COMMAND:
		ctrl = (UI::GUIControl*)(OSInt)GetWindowLongPtr((HWND)lParam, GWL_USERDATA);
		if (ctrl)
		{
			return ctrl->OnNotify(HIWORD(wParam), 0);
		}
		break;
	case WM_KILLFOCUS:
		UnregisterHotKey((HWND)hWnd, HK_PAGEUP);
		UnregisterHotKey((HWND)hWnd, HK_PAGEDOWN);
		UnregisterHotKey((HWND)hWnd, HK_LINEUP);
		UnregisterHotKey((HWND)hWnd, HK_LINEDOWN);
		UnregisterHotKey((HWND)hWnd, HK_HOME);
		UnregisterHotKey((HWND)hWnd, HK_END);
		UnregisterHotKey((HWND)hWnd, HK_COPY);
		UnregisterHotKey((HWND)hWnd, HK_COPY_OLD);
		UnregisterHotKey((HWND)hWnd, HK_LEFT);
		UnregisterHotKey((HWND)hWnd, HK_RIGHT);
		UnregisterHotKey((HWND)hWnd, HK_LINEHOME);
		UnregisterHotKey((HWND)hWnd, HK_LINEEND);

		UnregisterHotKey((HWND)hWnd, HK_SPAGEUP);
		UnregisterHotKey((HWND)hWnd, HK_SPAGEDOWN);
		UnregisterHotKey((HWND)hWnd, HK_SLINEUP);
		UnregisterHotKey((HWND)hWnd, HK_SLINEDOWN);
		UnregisterHotKey((HWND)hWnd, HK_SHOME);
		UnregisterHotKey((HWND)hWnd, HK_SEND);
		UnregisterHotKey((HWND)hWnd, HK_SLEFT);
		UnregisterHotKey((HWND)hWnd, HK_SRIGHT);
		UnregisterHotKey((HWND)hWnd, HK_SLINEHOME);
		UnregisterHotKey((HWND)hWnd, HK_SLINEEND);
		DestroyCaret(); 
		break;
	case WM_SETFOCUS:
		RegisterHotKey((HWND)hWnd, HK_PAGEUP, 0, VK_PRIOR);
		RegisterHotKey((HWND)hWnd, HK_PAGEDOWN, 0, VK_NEXT);
		RegisterHotKey((HWND)hWnd, HK_LINEUP, 0, VK_UP);
		RegisterHotKey((HWND)hWnd, HK_LINEDOWN, 0, VK_DOWN);
		RegisterHotKey((HWND)hWnd, HK_HOME, MOD_CONTROL, VK_HOME);
		RegisterHotKey((HWND)hWnd, HK_END, MOD_CONTROL, VK_END);
		RegisterHotKey((HWND)hWnd, HK_COPY_OLD, MOD_CONTROL, VK_INSERT);
		RegisterHotKey((HWND)hWnd, HK_COPY, MOD_CONTROL, 0x43);
		RegisterHotKey((HWND)hWnd, HK_LEFT, 0, VK_LEFT);
		RegisterHotKey((HWND)hWnd, HK_RIGHT, 0, VK_RIGHT);
		RegisterHotKey((HWND)hWnd, HK_LINEHOME, 0, VK_HOME);
		RegisterHotKey((HWND)hWnd, HK_LINEEND, 0, VK_END);

		RegisterHotKey((HWND)hWnd, HK_SPAGEUP, MOD_SHIFT, VK_PRIOR);
		RegisterHotKey((HWND)hWnd, HK_SPAGEDOWN, MOD_SHIFT, VK_NEXT);
		RegisterHotKey((HWND)hWnd, HK_SLINEUP, MOD_SHIFT, VK_UP);
		RegisterHotKey((HWND)hWnd, HK_SLINEDOWN, MOD_SHIFT, VK_DOWN);
		RegisterHotKey((HWND)hWnd, HK_SHOME, MOD_CONTROL | MOD_SHIFT, VK_HOME);
		RegisterHotKey((HWND)hWnd, HK_SEND, MOD_CONTROL | MOD_SHIFT, VK_END);
		RegisterHotKey((HWND)hWnd, HK_SLEFT, MOD_SHIFT, VK_LEFT);
		RegisterHotKey((HWND)hWnd, HK_SRIGHT, MOD_SHIFT, VK_RIGHT);
		RegisterHotKey((HWND)hWnd, HK_SLINEHOME, MOD_SHIFT, VK_HOME);
		RegisterHotKey((HWND)hWnd, HK_SLINEEND, MOD_SHIFT, VK_END);

		CreateCaret((HWND)hWnd, 0, 2, me->pageLineHeight);
		me->UpdateCaretPos();
		ShowCaret((HWND)hWnd); 
		break;
	case WM_HOTKEY:
		switch (wParam)
		{
		case HK_PAGEUP:
		case HK_SPAGEUP:
			scrollLock = GetKeyState(VK_SCROLL);
			if (scrollLock & 1)
			{
				SetScrollPos((HWND)hWnd, SB_VERT, GetScrollPos((HWND)hWnd, SB_VERT) - me->pageLineCnt, TRUE);
				me->Redraw();
			}
			else
			{
				me->caretY -= me->pageLineCnt - 1;
				if (me->caretY < 0)
				{
					me->caretY = 0;
					me->caretX = 0;
				}
				me->UpdateCaretSel(false);
				me->EnsureCaretVisible();
				me->UpdateCaretPos();
				me->EventTextPosUpdated();
			}
			break;
		case HK_PAGEDOWN:
		case HK_SPAGEDOWN:
			scrollLock = GetKeyState(VK_SCROLL);
			if (scrollLock & 1)
			{
				SetScrollPos((HWND)hWnd, SB_VERT, GetScrollPos((HWND)hWnd, SB_VERT) + me->pageLineCnt, TRUE);
				me->Redraw();
			}
			else
			{
				me->caretY += me->pageLineCnt - 1;
				if (me->caretY >= me->lineOfsts->GetCount() - 1)
				{
					me->caretY = me->lineOfsts->GetCount() - 1;
					me->caretX = 0;
				}
				me->UpdateCaretSel(false);
				me->EnsureCaretVisible();
				me->UpdateCaretPos();
				me->EventTextPosUpdated();
			}
			break;
		case HK_LINEUP:
		case HK_SLINEUP:
			scrollLock = GetKeyState(VK_SCROLL);
			if (scrollLock & 1)
			{
				SetScrollPos((HWND)hWnd, SB_VERT, GetScrollPos((HWND)hWnd, SB_VERT) - 1, TRUE);
				me->Redraw();
			}
			else if (me->caretY > 0)
			{
				me->GetTextPos((Int32)me->caretDispX, (Int32)me->caretDispY - me->pageLineHeight, &textXPos, &textYPos);
				me->caretX = textXPos;
				me->caretY = textYPos;
				me->UpdateCaretSel(false);
				me->EnsureCaretVisible();
				me->UpdateCaretPos();
				me->EventTextPosUpdated();
			}
			break;
		case HK_LINEDOWN:
		case HK_SLINEDOWN:
			scrollLock = GetKeyState(VK_SCROLL);
			if (scrollLock & 1)
			{
				SetScrollPos((HWND)hWnd, SB_VERT, GetScrollPos((HWND)hWnd, SB_VERT) + 1, TRUE);
				me->Redraw();
			}
			else
			{
				me->GetTextPos(me->caretDispX, me->caretDispY + me->pageLineHeight, &textXPos, &textYPos);
				me->caretX = textXPos;
				me->caretY = textYPos;
				me->UpdateCaretSel(false);
				me->EnsureCaretVisible();
				me->UpdateCaretPos();
				me->EventTextPosUpdated();
			}
			break;
		case HK_LEFT:
		case HK_SLEFT:
			scrollLock = GetKeyState(VK_SCROLL);
			if (scrollLock & 1)
			{
				SetScrollPos((HWND)hWnd, SB_HORZ, GetScrollPos((HWND)hWnd, SB_HORZ) - 1, TRUE);
				me->Redraw();
			}
			else if (me->caretX > 0)
			{
				me->caretX -= 1;
				me->UpdateCaretSel(false);
				me->EnsureCaretVisible();
				me->UpdateCaretPos();
				me->EventTextPosUpdated();
			}
			else if (me->caretY > 0)
			{
				me->caretY -= 1;
				me->caretX = (UInt32)me->GetLineCharCnt(me->caretY);
				me->UpdateCaretSel(false);
				me->EnsureCaretVisible();
				me->UpdateCaretPos();
				me->EventTextPosUpdated();
			}
			break;
		case HK_RIGHT:
		case HK_SRIGHT:
			scrollLock = GetKeyState(VK_SCROLL);
			if (scrollLock & 1)
			{
				SetScrollPos((HWND)hWnd, SB_HORZ, GetScrollPos((HWND)hWnd, SB_HORZ) + 1, TRUE);
				me->Redraw();
			}
			else if (me->caretX >= me->GetLineCharCnt(me->caretY))
			{
				me->caretX = 0;
				me->caretY++;
				if (me->caretY >= me->lineOfsts->GetCount() - 1)
					me->caretY = me->lineOfsts->GetCount() - 1;
				me->UpdateCaretSel(false);
				me->EnsureCaretVisible();
				me->UpdateCaretPos();
				me->EventTextPosUpdated();
			}
			else
			{
				me->caretX += 1;
				me->UpdateCaretSel(false);
				me->EnsureCaretVisible();
				me->UpdateCaretPos();
				me->EventTextPosUpdated();
			}
			break;
		case HK_HOME:
		case HK_SHOME:
			me->caretX = 0;
			me->caretY = 0;
			me->UpdateCaretSel(true);
			SetScrollPos((HWND)hWnd, SB_VERT, 0, TRUE);
			me->Redraw();
			me->EventTextPosUpdated();
			break;
		case HK_END:
		case HK_SEND:
			me->caretX = 0;
			me->caretY = me->lineOfsts->GetCount() - 1;
			me->UpdateCaretSel(true);
			SetScrollPos((HWND)hWnd, SB_VERT, (Int32)me->lineOfsts->GetCount() - 1, TRUE);
			me->Redraw();
			me->EventTextPosUpdated();
			break;
		case HK_COPY:
		case HK_COPY_OLD:
			me->CopySelected();
			break;
		case HK_LINEHOME:
		case HK_SLINEHOME:
			me->caretX = 0;
			me->UpdateCaretSel(false);
			me->EnsureCaretVisible();
			me->UpdateCaretPos();
			me->EventTextPosUpdated();
			break;
		case HK_LINEEND:
		case HK_SLINEEND:
			me->caretX = (Int32)me->GetLineCharCnt(me->caretY);
			me->UpdateCaretSel(false);
			me->EnsureCaretVisible();
			me->UpdateCaretPos();
			me->EventTextPosUpdated();
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		mouseXPos = (Int16)LOWORD(lParam);
		mouseYPos = (Int16)HIWORD(lParam);
		me->GetTextPos(mouseXPos, mouseYPos, &textXPos, &textYPos);
		me->caretX = textXPos;
		me->caretY = textYPos;
		SetFocus((HWND)hWnd);
		shiftLock = GetKeyState(VK_LSHIFT) | GetKeyState(VK_RSHIFT);
		if ((shiftLock & 0x80) == 0)
		{
			me->selStartX = textXPos;
			me->selStartY = textYPos;
		}
		me->selEndX = textXPos;
		me->selEndY = textYPos;
		me->selLastX = textXPos;
		me->selLastY = textYPos;
		me->mouseDown = true;
		::SetCapture((HWND)hWnd);
		me->Redraw();
		me->EventTextPosUpdated();
		break;
	case WM_MOUSEMOVE:
		if (me->mouseDown)
		{
			OSInt lineOfst;
			Bool needRedraw = false;
			mouseXPos = (Int16)LOWORD(lParam);
			mouseYPos = (Int16)HIWORD(lParam);
			lineOfst = mouseXPos / me->pageLineHeight;
			if (lineOfst < 0)
			{
				SetScrollPos((HWND)hWnd, SB_VERT, (Int32)(GetScrollPos((HWND)hWnd, SB_VERT) + lineOfst), TRUE);
				needRedraw = true;
			}
			else if (lineOfst > me->pageLineCnt)
			{
				SetScrollPos((HWND)hWnd, SB_VERT, (Int32)(GetScrollPos((HWND)hWnd, SB_VERT) + lineOfst - me->pageLineCnt), TRUE);
				needRedraw = true;
			}
			me->GetTextPos(mouseXPos, mouseYPos, &textXPos, &textYPos);
			if (me->selLastX != textXPos || me->selLastY != textYPos)
			{
				me->selLastX = textXPos;
				me->selLastY = textYPos;
				me->selEndX = textXPos;
				me->selEndY = textYPos;
				me->caretX = textXPos;
				me->caretY = textYPos;
				needRedraw = true;
				me->EventTextPosUpdated();
			}
			if (needRedraw)
			{
				me->Redraw();
			}
		}
		break;
	case WM_LBUTTONUP:
		me->mouseDown = false;
		::ReleaseCapture();
		break;
	case WM_MOUSEWHEEL:
		{
			UInt32 ucNumLines = 0;
			SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ucNumLines, 0);
			if (ucNumLines == 0)
				ucNumLines = 3;
			SetScrollPos((HWND)hWnd, SB_VERT, GetScrollPos((HWND)hWnd, SB_VERT) - (Int16)(HIWORD(wParam)) / 120 * ucNumLines, TRUE);
			me->Redraw();
		}
		break;
	case WM_HSCROLL:
		si.cbSize = sizeof (si);
		si.fMask  = SIF_ALL;

		GetScrollInfo((HWND)hWnd, SB_HORZ, &si);
		mouseXPos = si.nPos;
		switch (LOWORD (wParam))
		{
		case SB_LINELEFT: 
			si.nPos -= 1;
			break;
		case SB_LINERIGHT: 
			si.nPos += 1;
			break;
		case SB_PAGELEFT:
			si.nPos -= si.nPage;
			break;
		case SB_PAGERIGHT:
			si.nPos += si.nPage;
			break;
		case SB_THUMBTRACK: 
			si.nPos = si.nTrackPos;
			break;
		default :
			break;
		}
		si.fMask = SIF_POS;
		SetScrollInfo ((HWND)hWnd, SB_HORZ, &si, TRUE);
		GetScrollInfo ((HWND)hWnd, SB_HORZ, &si);

		if (si.nPos != mouseXPos)
		{
			me->Redraw();
		}
		return 0;
	case WM_VSCROLL:
		si.cbSize = sizeof (si);
		si.fMask  = SIF_ALL;
		GetScrollInfo ((HWND)hWnd, SB_VERT, &si);

		mouseYPos = si.nPos;
		switch (LOWORD (wParam))
		{
		case SB_TOP:
			si.nPos = si.nMin;
			break;
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;
		case SB_LINEUP:
			si.nPos -= 1;
			break;
		case SB_LINEDOWN:
			si.nPos += 1;
			break;
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;
		default:
			break; 
		}
		si.fMask = SIF_POS;
		SetScrollInfo ((HWND)hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo ((HWND)hWnd, SB_VERT, &si);

		if (si.nPos != mouseYPos)
		{
			me->Redraw();
		}
		return 0;
	case WM_PAINT:
		me->OnPaint();
		me->UpdateCaretPos();
		return 0;
	case WM_NOTIFY:
		nmhdr = (NMHDR*)lParam;
		ctrl = (UI::GUIControl*)(OSInt)GetWindowLongPtr(nmhdr->hwndFrom, GWL_USERDATA);
		if (ctrl)
		{
			return ctrl->OnNotify(nmhdr->code, (void*)lParam);
		}
		break;
	case WM_TIMER:
		currLineCnt = me->lineOfsts->GetCount();
		if (me->lastLineCnt != currLineCnt)
		{
			Bool needRedraw = false;
			if (me->lastLineCnt < me->pageLineCnt && currLineCnt > me->pageLineCnt)
			{
				needRedraw = true;
			}
			me->lastLineCnt = currLineCnt;
			SetScrollRange((HWND)me->hwnd, SB_VERT, 0, (Int32)currLineCnt - 1, TRUE);
			if (needRedraw)
			{
				me->Redraw();
			}
		}
		break;
	case WM_SIZE:
		if (me)
		{
			if (me->bgBmp)
			{
				DeleteObject((HBITMAP)me->bgBmp);
			}
			{
				hdc = GetDC((HWND)hWnd);
				GetClientRect((HWND)hWnd, &rc);
				me->bgBmp = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
				ReleaseDC((HWND)hWnd, hdc);
			}
			me->UpdateScrollBar();
		}
		break;//DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

UInt32 __stdcall UI::GUITextFileView::ProcThread(void *userObj)
{
	UOSInt lineCurr;
	WChar lastC;
	WChar c;
	UI::GUITextFileView *me = (UI::GUITextFileView*)userObj;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		if (me->loadNewFile)
		{
			if (me->isSearching)
			{
				SDEL_TEXT(me->srchText);
				me->isSearching = false;
			}
			Sync::MutexUsage mutUsage(me->mut);
			me->loadNewFile = false;
			if (me->fs)
			{
				DEL_CLASS(me->fs);
			}
			NEW_CLASS(me->fs, IO::FileStream(me->fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
			me->lineOfsts->Clear();
			me->readingFile = true;
			me->readBuffOfst = 0;
			me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
			me->fileCodePage = me->codePage;
			mutUsage.EndUse();

			if (me->readBuffSize >= 3)
			{
				if (me->readBuff[0] == 0xff && me->readBuff[1] == 0xfe)
				{
					me->fileCodePage = 1200;
					me->readBuffSize -= 2;
					me->readBuffOfst = 2;
					MemCopyO(me->readBuff, &me->readBuff[2], me->readBuffSize);
				}
				else if (me->readBuff[0] == 0xfe && me->readBuff[1] == 0xff)
				{
					me->fileCodePage = 1201;
					me->readBuffSize -= 2;
					me->readBuffOfst = 2;
					MemCopyO(me->readBuff, &me->readBuff[2], me->readBuffSize);
				}
				else if (me->readBuff[0] == 0xef && me->readBuff[1] == 0xbb && me->readBuff[2] == 0xbf)
				{
					me->fileCodePage = 65001;
					me->readBuffSize -= 3;
					me->readBuffOfst = 3;
					MemCopyO(me->readBuff, &me->readBuff[3], me->readBuffSize);
				}
			}

			mutUsage.ReplaceMutex(me->mut);
			me->lineOfsts->Add(me->readBuffOfst);
			mutUsage.EndUse();
			lastC = 0;
			if (me->fileCodePage == 1200)
			{
				while (me->readBuffSize > 0)
				{
					if (me->loadNewFile || me->threadToStop)
						break;

					Sync::MutexUsage mutUsage(me->mut);
					lineCurr = 0;
					while (lineCurr < me->readBuffSize)
					{
						c = *(WChar*)&me->readBuff[lineCurr];
						if (lastC == 0xd)
						{
							if (c == 0xa)
							{
								c = 0;
								me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
							}
							else
							{
								me->lineOfsts->Add(me->readBuffOfst + lineCurr);
							}
						}
						else if (lastC == 0xa)
						{
							if (c == 0xd)
							{
								c = 0;
								me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
							}
							else
							{
								me->lineOfsts->Add(me->readBuffOfst + lineCurr);
							}
						}
						lastC = c;
						lineCurr += 2;
					}
					me->readBuffOfst += me->readBuffSize;

					me->fs->SeekFromBeginning(me->readBuffOfst);
					me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
					mutUsage.EndUse();
				}
			}
			else if (me->fileCodePage == 1201)
			{
				while (me->readBuffSize > 0)
				{
					if (me->loadNewFile || me->threadToStop)
						break;

					lineCurr = 0;
					Sync::MutexUsage mutUsage(me->mut);
					while (lineCurr < me->readBuffSize)
					{
						c = (me->readBuff[lineCurr] << 8) | me->readBuff[lineCurr + 1];
						if (lastC == 0xd)
						{
							if (c == 0xa)
							{
								c = 0;
								me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
							}
							else
							{
								me->lineOfsts->Add(me->readBuffOfst + lineCurr);
							}
						}
						else if (lastC == 0xa)
						{
							if (c == 0xd)
							{
								c = 0;
								me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
							}
							else
							{
								me->lineOfsts->Add(me->readBuffOfst + lineCurr);
							}
						}
						lastC = c;
						lineCurr += 2;
					}
					me->readBuffOfst += me->readBuffSize;

					me->fs->SeekFromBeginning(me->readBuffOfst);
					me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
					mutUsage.EndUse();
				}
			}
			else
			{
				while (me->readBuffSize > 0)
				{
					if (me->loadNewFile || me->threadToStop)
						break;

					lineCurr = 0;
					Sync::MutexUsage mutUsage(me->mut);
					while (lineCurr < me->readBuffSize)
					{
						c = me->readBuff[lineCurr];
						if (lastC == 0xd)
						{
							if (c == 0xa)
							{
								c = 0;
								me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
							}
							else
							{
								me->lineOfsts->Add(me->readBuffOfst + lineCurr);
							}
						}
						else if (lastC == 0xa)
						{
							if (c == 0xd)
							{
								c = 0;
								me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
							}
							else
							{
								me->lineOfsts->Add(me->readBuffOfst + lineCurr);
							}
						}
						lastC = c;
						lineCurr += 1;
					}
					me->readBuffOfst += me->readBuffSize;

					me->fs->SeekFromBeginning(me->readBuffOfst);
					me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
					mutUsage.EndUse();
				}
			}
			if (me->loadNewFile || me->threadToStop)
			{
			}
			else
			{
				me->lineOfsts->Add(me->readBuffOfst);
			}
			me->fileSize = me->readBuffOfst;
			me->readingFile = false;
		}
		else if (me->fs)
		{
			UInt64 size;
			Sync::MutexUsage mutUsage(me->mut);
			size = me->fs->GetLength();
			mutUsage.EndUse();
			if (size > me->fileSize)
			{
				OSInt i;
				Sync::MutexUsage mutUsage(me->mut);
				i = me->lineOfsts->GetCount();
				me->lineOfsts->RemoveAt(i - 1);
				me->readBuffOfst = me->lineOfsts->GetItem(i - 2);
				me->readingFile = true;
				me->fs->SeekFromBeginning(me->readBuffOfst);
				me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
				mutUsage.EndUse();
			
				lastC = 0;
				if (me->fileCodePage == 1200)
				{
					while (me->readBuffSize > 0)
					{
						if (me->loadNewFile || me->threadToStop)
							break;

						Sync::MutexUsage mutUsage(me->mut);
						lineCurr = 0;
						while (lineCurr < me->readBuffSize)
						{
							c = *(WChar*)&me->readBuff[lineCurr];
							if (lastC == 0xd)
							{
								if (c == 0xa)
								{
									c = 0;
									me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
								}
								else
								{
									me->lineOfsts->Add(me->readBuffOfst + lineCurr);
								}
							}
							else if (lastC == 0xa)
							{
								if (c == 0xd)
								{
									c = 0;
									me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
								}
								else
								{
									me->lineOfsts->Add(me->readBuffOfst + lineCurr);
								}
							}
							lastC = c;
							lineCurr += 2;
						}
						me->readBuffOfst += me->readBuffSize;

						me->fs->SeekFromBeginning(me->readBuffOfst);
						me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
						mutUsage.EndUse();
					}
				}
				else if (me->fileCodePage == 1201)
				{
					while (me->readBuffSize > 0)
					{
						if (me->loadNewFile || me->threadToStop)
							break;

						lineCurr = 0;
						Sync::MutexUsage mutUsage(me->mut);
						while (lineCurr < me->readBuffSize)
						{
							c = (me->readBuff[lineCurr] << 8) | me->readBuff[lineCurr + 1];
							if (lastC == 0xd)
							{
								if (c == 0xa)
								{
									c = 0;
									me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
								}
								else
								{
									me->lineOfsts->Add(me->readBuffOfst + lineCurr);
								}
							}
							else if (lastC == 0xa)
							{
								if (c == 0xd)
								{
									c = 0;
									me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
								}
								else
								{
									me->lineOfsts->Add(me->readBuffOfst + lineCurr);
								}
							}
							lastC = c;
							lineCurr += 2;
						}
						me->readBuffOfst += me->readBuffSize;

						me->fs->SeekFromBeginning(me->readBuffOfst);
						me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
						mutUsage.EndUse();
					}
				}
				else
				{
					while (me->readBuffSize > 0)
					{
						if (me->loadNewFile || me->threadToStop)
							break;

						lineCurr = 0;
						Sync::MutexUsage mutUsage(me->mut);
						while (lineCurr < me->readBuffSize)
						{
							c = me->readBuff[lineCurr];
							if (lastC == 0xd)
							{
								if (c == 0xa)
								{
									c = 0;
									me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
								}
								else
								{
									me->lineOfsts->Add(me->readBuffOfst + lineCurr);
								}
							}
							else if (lastC == 0xa)
							{
								if (c == 0xd)
								{
									c = 0;
									me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
								}
								else
								{
									me->lineOfsts->Add(me->readBuffOfst + lineCurr);
								}
							}
							lastC = c;
							lineCurr += 1;
						}
						me->readBuffOfst += me->readBuffSize;

						me->fs->SeekFromBeginning(me->readBuffOfst);
						me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
						mutUsage.EndUse();
					}
				}
				if (me->loadNewFile || me->threadToStop)
				{
				}
				else
				{
					me->lineOfsts->Add(me->readBuffOfst);
				}
				me->fileSize = me->readBuffOfst;
				me->readingFile = false;
			}
		}

		if (me->isSearching)
		{
			UInt8 *srchTxt;
			UOSInt srchTxtLen;
			UOSInt strLen;
			UInt32 startCaretX = me->caretX;
			UOSInt startCaretY = me->caretY;
			UInt32 srchCaretX;
			UOSInt srchCaretY;
			Int64 startLineOfst = me->lineOfsts->GetItem(startCaretY);
			Int64 nextLineOfst = me->lineOfsts->GetItem(startCaretY + 1);
			UInt8 *srchBuff;
			Int64 currOfst;
			UOSInt currSize;
			UOSInt srchIndex;
			Bool found = false;

			if (me->fs)
			{
				Text::Encoding enc(me->fileCodePage);
				strLen = Text::StrCharCnt(me->srchText);
				srchTxtLen = enc.UTF8CountBytesC(me->srchText, strLen);
				srchTxt = MemAlloc(UInt8, srchTxtLen + 1);
				enc.UTF8ToBytesC(srchTxt, me->srchText, strLen);
				srchTxt[srchTxtLen] = 0;

				srchBuff = MemAlloc(UInt8, READBUFFSIZE + 1);
				currOfst = startLineOfst;
				while (true)
				{
					Sync::MutexUsage mutUsage(me->mut);
					if (me->fs)
					{
						me->fs->SeekFromBeginning(currOfst);
						currSize = me->fs->Read(srchBuff, READBUFFSIZE);
						srchBuff[currSize] = 0;
					}
					else
					{
						currSize = 0;
					}
					mutUsage.EndUse();
					if (currSize <= srchTxtLen)
					{
						break;
					}

					srchIndex = Text::StrIndexOf((Char*)srchBuff, (Char*)srchTxt);
					if (srchIndex != INVALID_INDEX)
					{
						me->GetPosFromByteOfst(currOfst + srchIndex, &srchCaretX, &srchCaretY);
						if (srchCaretY == startCaretY && srchCaretX <= startCaretX)
						{
							OSInt tmpIndex;
							while (true)
							{
								tmpIndex = Text::StrIndexOf((Char*)srchBuff + srchIndex + 1, (Char*)srchTxt);
								if (tmpIndex == INVALID_INDEX)
									break;
								
								me->GetPosFromByteOfst(currOfst + srchIndex + 1 + tmpIndex, &srchCaretX, &srchCaretY);
								if (srchCaretY == startCaretY && srchCaretX <= startCaretX)
								{
									srchIndex += 1 + tmpIndex;
								}
								else
								{
									me->selStartX = srchCaretX;
									me->selStartY = srchCaretY;
									me->selEndX = srchCaretX + (Int32)strLen;
									me->selEndY = srchCaretY;
									me->GoToText(srchCaretY, srchCaretX);
									me->Redraw();
									found = true;
									break;
								}
							}
							if (found)
							{
								break;
							}
						}
						else
						{
							me->selStartX = srchCaretX;
							me->selStartY = srchCaretY;
							me->selEndX = srchCaretX + (Int32)strLen;
							me->selEndY = srchCaretY;
							me->GoToText(srchCaretY, srchCaretX);
							me->Redraw();
							found = true;
							break;
						}
					}
					currOfst += currSize - srchTxtLen + 1;
				}

				if (!found)
				{
					currOfst = 0;
					while (currOfst < nextLineOfst)
					{
						Sync::MutexUsage mutUsage(me->mut);
						if (me->fs)
						{
							currSize = (OSInt)(nextLineOfst - currOfst);
							if (currSize > READBUFFSIZE)
							{
								currSize = READBUFFSIZE;
							}
							me->fs->SeekFromBeginning(currOfst);
							currSize = me->fs->Read(srchBuff, currSize);
							srchBuff[currSize] = 0;
						}
						else
						{
							currSize = 0;
						}
						mutUsage.EndUse();
						if (currSize <= srchTxtLen)
						{
							break;
						}

						srchIndex = Text::StrIndexOf((Char*)srchBuff, (Char*)srchTxt);
						if (srchIndex != INVALID_INDEX)
						{
							me->GetPosFromByteOfst(currOfst + srchIndex, &srchCaretX, &srchCaretY);
							if (srchCaretY < startCaretY || srchCaretX < startCaretX)
							{
								me->selStartX = srchCaretX;
								me->selStartY = srchCaretY;
								me->selEndX = srchCaretX + (Int32)strLen;
								me->selEndY = srchCaretY;
								me->GoToText(srchCaretY, srchCaretX);
								me->Redraw();
								found = true;
								break;
							}
						}
						currOfst += currSize - srchTxtLen + 1;
					}
				}

				MemFree(srchBuff);
				MemFree(srchTxt);
			}
			SDEL_TEXT(me->srchText);
			me->isSearching = false;
		}
		me->evtThread->Wait(1000);
	}
	me->threadRunning = false;
	return 0;
}

void UI::GUITextFileView::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::GUITextFileView::TFVWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst; 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_IBEAM); 
    wc.hbrBackground = 0;//BGBRUSH; 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::GUITextFileView::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

void UI::GUITextFileView::OnPaint()
{
	WChar sbuff[21];
	Int32 xPos;
	Int32 yPos;
	PAINTSTRUCT ps;
	Int64 startOfst;
	Int64 endOfst;
	Int64 currOfst;
	Int64 nextOfst;
	UInt8 *rbuff;
	OSInt i;
	OSInt j;
	Text::Encoding *enc;
	WChar *line;
	WChar *sptr;
	WChar *sptr2;
	SIZE sz;
	RECT rc;
	RECT rcText;
	HDC hdcBmp;

	Int32 maxScnWidth;

	xPos = GetScrollPos((HWND)this->hwnd, SB_HORZ);
	yPos = GetScrollPos((HWND)this->hwnd, SB_VERT);

	Sync::MutexUsage mutUsage(this->mut);
	if (this->fs == 0 || this->bgBmp == 0)
	{
		mutUsage.EndUse();
		BeginPaint((HWND)this->hwnd, &ps);
		GetClientRect((HWND)this->hwnd, &rc);
		FillRect(ps.hdc, &rc, BGBRUSH);
		EndPaint((HWND)this->hwnd, &ps);
		return;
	}
	NEW_CLASS(enc, Text::Encoding(this->fileCodePage));
	startOfst = this->lineOfsts->GetItem(yPos);
	endOfst = this->lineOfsts->GetItem(yPos + this->pageLineCnt);
	if (endOfst == 0)
	{
		endOfst = this->lineOfsts->GetItem(this->lineOfsts->GetCount() - 1);
	}
	rbuff = MemAlloc(UInt8, (OSInt)(endOfst - startOfst));
	this->fs->SeekFromBeginning(startOfst);
	this->fs->Read(rbuff, (OSInt)(endOfst - startOfst));

	BeginPaint((HWND)this->hwnd, &ps);
	GetClientRect((HWND)this->hwnd, &rc);
	hdcBmp = CreateCompatibleDC(ps.hdc);
	SelectObject(hdcBmp, (HBITMAP)this->bgBmp);
	maxScnWidth = rc.right - rc.left + xPos;
	void *fnt = this->GetFont();
	if (fnt)
	{
		SelectObject(hdcBmp, (HFONT)fnt);
	}
	FillRect(hdcBmp, &rc, BGBRUSH);
	SelectObject(hdcBmp, (HFONT)this->drawFont);
	sptr = Text::StrInt64(sbuff, this->pageLineCnt + yPos);
	GetTextExtentPoint(hdcBmp, sbuff, (Int32)(sptr - sbuff), &sz);
	this->dispLineNumW = sz.cx + 8;

	Int32 selTopX;
	OSInt selTopY;
	Int32 selBottomX;
	OSInt selBottomY;
	selTopX = this->selStartX;
	selTopY = this->selStartY;
	selBottomX = this->selEndX;
	selBottomY = this->selEndY;
	if (selTopY > selBottomY)
	{
		selTopX = this->selEndX;
		selTopY = this->selEndY;
		selBottomX = this->selStartX;
		selBottomY = this->selStartY;
	}
	else if (selTopY < selBottomY)
	{
	}
	else if (selTopX > selBottomX)
	{
		selTopX = this->selEndX;
		selTopY = this->selEndY;
		selBottomX = this->selStartX;
		selBottomY = this->selStartY;
	}
	else if (selTopX < selBottomX)
	{
	}

	SetBkColor(hdcBmp, 0xffcccc);
	currOfst = startOfst;
	i = 0;
	while (i < this->pageLineCnt)
	{
		nextOfst = this->lineOfsts->GetItem(i + yPos + 1);
		if (nextOfst == 0 && i + yPos + 1 >= (OSInt)this->lineOfsts->GetCount() - 1)
		{
			nextOfst = endOfst;
		}
		if (nextOfst > currOfst)
		{
			if (nextOfst == 3319)
			{
				nextOfst = 3319;
			}
			j = enc->CountWChars(&rbuff[currOfst - startOfst], (OSInt)(nextOfst - currOfst));
			line = MemAlloc(WChar, j + 1);
			sptr = enc->WFromBytes(line, &rbuff[currOfst - startOfst], (OSInt)(nextOfst - currOfst), 0);
			Text::StrReplace(line, '\t', ' ');
			if (sptr)
			{
				if (sptr[-1] == 13)
				{
					if (sptr[-2] == 10)
					{
						sptr[-2] = 0;
						sptr -= 2;
					}
					else
					{
						sptr[-1] = 0;
						sptr -= 1;
					}
				}
				else if (sptr[-1] == 10)
				{
					if (sptr[-2] == 13)
					{
						sptr[-2] = 0;
						sptr -= 2;
					}
					else
					{
						sptr[-1] = 0;
						sptr -= 1;
					}
				}
				SIZE szWhole;
				GetTextExtentPoint(hdcBmp, line, (Int32)(sptr - line), &szWhole);
				if (maxScnWidth < szWhole.cx + sz.cx + 8)
				{
					maxScnWidth = szWhole.cx + sz.cx + 8;
				}

				rcText.top = (Int32)(i * sz.cy);
				rcText.bottom = rcText.top + sz.cy;
				rcText.left = sz.cx + 8 -xPos;
				rcText.right = rc.right - rc.left;
				sptr2 = Text::StrInt64(sbuff, i + yPos + 1);
				SetTextColor(hdcBmp, 0xFF0000);
				SetBkMode(hdcBmp, TRANSPARENT);
				TextOutW(hdcBmp, -xPos, rcText.top, sbuff, (Int32)(sptr2 - sbuff));
				//TextOutW(ps.hdc, -xPos, i * sz.cy, line, sptr - line);
				SetTextColor(hdcBmp, 0x000000);

				if (i + yPos > selTopY && i + yPos < selBottomY)
				{
					SetBkMode(hdcBmp, OPAQUE);
					DrawText(hdcBmp, line, (Int32)(sptr - line), &rcText, DT_EXPANDTABS);
				}
				else if (i + yPos == selTopY && selTopY == selBottomY)
				{
					if (sptr - line <= selTopX || selTopX == selBottomX)
					{
						SetBkMode(hdcBmp, TRANSPARENT);
						DrawText(hdcBmp, line, (Int32)(sptr - line), &rcText, DT_EXPANDTABS);
					}
					else if (sptr - line <= selBottomX)
					{
						if (selTopX > 0)
						{
							SetBkMode(hdcBmp, TRANSPARENT);
							DrawText(hdcBmp, line, selTopX, &rcText, DT_EXPANDTABS | DT_CALCRECT);
							DrawText(hdcBmp, line, selTopX, &rcText, DT_EXPANDTABS);
							rcText.left = rcText.right;
							rcText.right = rc.right - rc.left;
						}
						SetBkMode(hdcBmp, OPAQUE);
						DrawText(hdcBmp, &line[selTopX], (Int32)((sptr - line) - selTopX), &rcText, DT_EXPANDTABS);
					}
					else
					{
						if (selTopX > 0)
						{
							SetBkMode(hdcBmp, TRANSPARENT);
							DrawText(hdcBmp, line, selTopX, &rcText, DT_EXPANDTABS | DT_CALCRECT);
							DrawText(hdcBmp, line, selTopX, &rcText, DT_EXPANDTABS);
							rcText.left = rcText.right;
							rcText.right = rc.right - rc.left;
						}
						SetBkMode(hdcBmp, OPAQUE);
						DrawText(hdcBmp, &line[selTopX], selBottomX - selTopX, &rcText, DT_EXPANDTABS | DT_CALCRECT);
						DrawText(hdcBmp, &line[selTopX], selBottomX - selTopX, &rcText, DT_EXPANDTABS);
						rcText.left = rcText.right;
						rcText.right = rc.right - rc.left;
						SetBkMode(hdcBmp, TRANSPARENT);
						DrawText(hdcBmp, &line[selBottomX], (Int32)((sptr - line) - selBottomX), &rcText, DT_EXPANDTABS);
					}
				}
				else if (i + yPos == selTopY)
				{
					if (selTopX == 0)
					{
						SetBkMode(hdcBmp, OPAQUE);
						DrawText(hdcBmp, line, (Int32)(sptr - line), &rcText, DT_EXPANDTABS);
					}
					else if (sptr - line <= selTopX)
					{
						SetBkMode(hdcBmp, TRANSPARENT);
						DrawText(hdcBmp, line, (Int32)(sptr - line), &rcText, DT_EXPANDTABS);
					}
					else
					{
						SetBkMode(hdcBmp, TRANSPARENT);
						DrawText(hdcBmp, line, selTopX, &rcText, DT_EXPANDTABS | DT_CALCRECT);
						DrawText(hdcBmp, line, selTopX, &rcText, DT_EXPANDTABS);
						rcText.left = rcText.right;
						rcText.right = rc.right - rc.left;
						SetBkMode(hdcBmp, OPAQUE);
						DrawText(hdcBmp, &line[selTopX], (Int32)((sptr - line) - selTopX), &rcText, DT_EXPANDTABS);
					}
				}
				else if (i + yPos == selBottomY)
				{
					if (selBottomX == 0)
					{
						SetBkMode(hdcBmp, TRANSPARENT);
						DrawText(hdcBmp, line, (Int32)(sptr - line), &rcText, DT_EXPANDTABS);
					}
					else if (sptr - line <= selBottomX)
					{
						SetBkMode(hdcBmp, OPAQUE);
						DrawText(hdcBmp, line, (Int32)(sptr - line), &rcText, DT_EXPANDTABS);
					}
					else
					{
						SetBkMode(hdcBmp, OPAQUE);
						DrawText(hdcBmp, line, selBottomX, &rcText, DT_EXPANDTABS | DT_CALCRECT);
						DrawText(hdcBmp, line, selBottomX, &rcText, DT_EXPANDTABS);
						rcText.left = rcText.right;
						rcText.right = rc.right - rc.left;
						SetBkMode(hdcBmp, TRANSPARENT);
						DrawText(hdcBmp, &line[selBottomX], (Int32)(sptr - line - selBottomX), &rcText, DT_EXPANDTABS);
					}
				}
				else
				{
					SetBkMode(hdcBmp, TRANSPARENT);
					DrawText(hdcBmp, line, (Int32)(sptr - line), &rcText, DT_EXPANDTABS);
				}
			}
			MemFree(line);
		}

		currOfst = nextOfst;
		i++;
	}
	BitBlt(ps.hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdcBmp, 0, 0, SRCCOPY);
	DeleteDC(hdcBmp);

	DEL_CLASS(enc);
	EndPaint((HWND)this->hwnd, &ps);
	MemFree(rbuff);
	mutUsage.EndUse();
	SetScrollRange((HWND)this->hwnd, SB_HORZ, 0, maxScnWidth, TRUE);
}

void UI::GUITextFileView::UpdateScrollBar()
{
	if (this->drawFont == 0)
	{
		return;
	}

	SIZE sz;
	RECT rc;
	SCROLLINFO si;
	HDC hdc = GetDC((HWND)this->hwnd);
	HGDIOBJ lastFont = SelectObject(hdc, (HFONT)this->drawFont);
	GetTextExtentPointW(hdc, L"Test", 4, &sz);
	GetClientRect((HWND)this->hwnd, &rc);
	SelectObject(hdc, lastFont);
	ReleaseDC((HWND)this->hwnd, hdc);
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE;
	si.nPage = (rc.bottom - rc.top) / sz.cy;
	this->pageLineCnt = si.nPage;
	this->pageLineHeight = sz.cy;
	SetScrollInfo((HWND)this->hwnd, SB_VERT, &si, TRUE);

	si.nPage = rc.right - rc.left;
	SetScrollInfo((HWND)this->hwnd, SB_HORZ, &si, TRUE);
}

void UI::GUITextFileView::CopySelected()
{
	if (this->selStartX == this->selEndX && this->selStartY == this->selEndY)
	{
		return;
	}

	OSInt selTopX;
	OSInt selTopY;
	OSInt selBottomX;
	OSInt selBottomY;
	selTopX = this->selStartX;
	selTopY = this->selStartY;
	selBottomX = this->selEndX;
	selBottomY = this->selEndY;
	if (selTopY > selBottomY)
	{
		selTopX = this->selEndX;
		selTopY = this->selEndY;
		selBottomX = this->selStartX;
		selBottomY = this->selStartY;
	}
	else if (selTopY < selBottomY)
	{
	}
	else if (selTopX > selBottomX)
	{
		selTopX = this->selEndX;
		selTopY = this->selEndY;
		selBottomX = this->selStartX;
		selBottomY = this->selStartY;
	}
	else if (selTopX < selBottomX)
	{
	}
	Sync::MutexUsage mutUsage(this->mut);
	if (this->fs == 0)
	{
		mutUsage.EndUse();
		return;
	}

	Text::Encoding *enc;
	UInt8 *rbuff;
	UTF8Char *line;
	Int64 startOfst;
	Int64 endOfst;
	OSInt j;

	NEW_CLASS(enc, Text::Encoding(this->fileCodePage));
	if (selTopY == selBottomY)
	{
		startOfst = this->lineOfsts->GetItem(selTopY);
		endOfst = this->lineOfsts->GetItem(selTopY + 1);
		if (endOfst == 0)
		{
			mutUsage.EndUse();
			DEL_CLASS(enc);
			return;
//			endOfst = this->fs->GetLength();
		}
		rbuff = MemAlloc(UInt8, (OSInt)(endOfst - startOfst));
		this->fs->SeekFromBeginning(startOfst);
		this->fs->Read(rbuff, (OSInt)(endOfst - startOfst));
		mutUsage.EndUse();

		j = enc->CountUTF8Chars(rbuff, (OSInt)(endOfst - startOfst));
		line = MemAlloc(UTF8Char, j + 1);
		enc->UTF8FromBytes(line, rbuff, (OSInt)(endOfst - startOfst), 0);
		line[selBottomX] = 0;
		Win32::Clipboard::SetString(this->hwnd, &line[selTopX]);
		MemFree(line);
		MemFree(rbuff);
	}
	else
	{
		Text::StringBuilderUTF8 sb;

		startOfst = this->lineOfsts->GetItem(selTopY);
		endOfst = this->lineOfsts->GetItem(selBottomY);
		if (endOfst == 0)
		{
			mutUsage.EndUse();
			DEL_CLASS(enc);
			return;
//			endOfst = this->fs->GetLength();
		}
		else if (endOfst - startOfst > 1048576)
		{
			mutUsage.EndUse();
			DEL_CLASS(enc);
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Failed to copy because selected area is too long", (const UTF8Char*)"TextViewer", this);
			return;
		}
		rbuff = MemAlloc(UInt8, (UOSInt)(endOfst - startOfst));
		this->fs->SeekFromBeginning(startOfst);
		this->fs->Read(rbuff, (UOSInt)(endOfst - startOfst));

		j = enc->CountUTF8Chars(rbuff, (UOSInt)(endOfst - startOfst));
		line = MemAlloc(UTF8Char, j + 1);
		enc->UTF8FromBytes(line, rbuff, (UOSInt)(endOfst - startOfst), 0);
		sb.Append(&line[selTopX]);
		MemFree(line);
		MemFree(rbuff);

		startOfst = this->lineOfsts->GetItem(selBottomY);
		endOfst = this->lineOfsts->GetItem(selBottomY + 1);
		if (endOfst == 0)
		{
			mutUsage.EndUse();
			return;
//			endOfst = this->fs->GetLength();
		}
		rbuff = MemAlloc(UInt8, (UOSInt)(endOfst - startOfst));
		this->fs->SeekFromBeginning(startOfst);
		this->fs->Read(rbuff, (UOSInt)(endOfst - startOfst));
		mutUsage.EndUse();
		j = enc->CountUTF8Chars(rbuff, (UOSInt)(endOfst - startOfst));
		line = MemAlloc(UTF8Char, j + 1);
		enc->UTF8FromBytes(line, rbuff, (UOSInt)(endOfst - startOfst), 0);
		line[selBottomX] = 0;
		sb.Append(line);
		MemFree(line);
		MemFree(rbuff);

		Win32::Clipboard::SetString(this->hwnd, sb.ToString());
	}
	DEL_CLASS(enc);
}

void UI::GUITextFileView::UpdateCaretPos()
{
	Int32 yPos;
	Int32 xPos;
	Int32 xScr;
	yPos = GetScrollPos((HWND)this->hwnd, SB_VERT);
	xPos = this->dispLineNumW;
	xScr = GetScrollPos((HWND)this->hwnd, SB_HORZ);

	if (caretY < 0 || caretY >= this->lineOfsts->GetCount() - 1)
	{
	}
	else if (this->fs)
	{
		UInt8 *rbuff;
		UInt64 lineOfst;
		UInt64 nextOfst;
		Sync::MutexUsage mutUsage(this->mut);
		lineOfst = this->lineOfsts->GetItem(this->caretY);
		nextOfst = this->lineOfsts->GetItem(this->caretY + 1);
		if (nextOfst == 0 && this->lineOfsts->GetCount() - 1 <= this->caretY + 1)
		{
			nextOfst = this->lineOfsts->GetItem(this->lineOfsts->GetCount() - 1);
		}
		if (nextOfst > lineOfst)
		{
			WChar *line;
			SIZE sz;

			Text::Encoding enc(this->fileCodePage);
			rbuff = MemAlloc(UInt8, (UOSInt)(nextOfst - lineOfst));
			this->fs->SeekFromBeginning(lineOfst);
			this->fs->Read(rbuff, (UOSInt)(nextOfst - lineOfst));
			UOSInt charCnt = enc.CountWChars(rbuff, (UOSInt)(nextOfst - lineOfst));
			if (charCnt < this->caretX)
			{
				this->caretX = (UInt32)charCnt;
			}
			line = MemAlloc(WChar, charCnt + 1);
			enc.WFromBytes(line, rbuff, (OSInt)(nextOfst - lineOfst), 0);
			Text::StrReplace(line, '\t', ' ');
			HDC hdc = GetDC((HWND)this->hwnd);
			void *fnt = this->GetFont();
			if (fnt)
			{
				SelectObject(hdc, fnt);
			}
			GetTextExtentExPoint(hdc, line, this->caretX, 0, 0, 0, &sz);
			xPos = this->dispLineNumW + sz.cx;
			ReleaseDC((HWND)this->hwnd, hdc);
			MemFree(line);
			MemFree(rbuff);
		}
		mutUsage.EndUse();
	}

	this->caretDispX = xPos - xScr;
	this->caretDispY = (Int32)((this->caretY - yPos) * this->pageLineHeight);
	SetCaretPos(this->caretDispX, this->caretDispY);
}

void UI::GUITextFileView::EnsureCaretVisible()
{
	Bool needRedraw = false;
	OSInt yPos;
	Int32 xPos;
	Int32 xScr;
	yPos = GetScrollPos((HWND)this->hwnd, SB_VERT);
	xPos = this->dispLineNumW;
	xScr = GetScrollPos((HWND)this->hwnd, SB_HORZ);

	if ((OSInt)caretY >= yPos + this->pageLineCnt)
	{
		yPos = caretY - this->pageLineCnt + 1;
		SetScrollPos((HWND)this->hwnd, SB_VERT, (Int32)yPos, FALSE);
		needRedraw = true;
	}
	else if ((OSInt)caretY < yPos)
	{
		yPos = caretY;
		SetScrollPos((HWND)this->hwnd, SB_VERT, (Int32)yPos, FALSE);
		needRedraw = true;
	}

	if (caretY < 0 || caretY >= this->lineOfsts->GetCount() - 1)
	{
	}
	else if (this->fs)
	{
		UInt8 *rbuff;
		UInt64 lineOfst;
		UInt64 nextOfst;
		Sync::MutexUsage mutUsage(this->mut);
		lineOfst = this->lineOfsts->GetItem(this->caretY);
		nextOfst = this->lineOfsts->GetItem(this->caretY + 1);
		if (nextOfst == 0 && this->lineOfsts->GetCount() - 1 <= this->caretY + 1)
		{
			nextOfst = this->lineOfsts->GetItem(this->lineOfsts->GetCount() - 1);
		}
		if (nextOfst > lineOfst)
		{
			WChar *line;
			SIZE sz;

			Text::Encoding enc(this->fileCodePage);
			rbuff = MemAlloc(UInt8, (UOSInt)(nextOfst - lineOfst));
			this->fs->SeekFromBeginning(lineOfst);
			this->fs->Read(rbuff, (UOSInt)(nextOfst - lineOfst));
			UOSInt charCnt = enc.CountWChars(rbuff, (UOSInt)(nextOfst - lineOfst));
			if (charCnt < this->caretX)
			{
				this->caretX = (Int32)charCnt;
			}
			line = MemAlloc(WChar, charCnt + 1);
			enc.WFromBytes(line, rbuff, (OSInt)(nextOfst - lineOfst), 0);
			Text::StrReplace(line, '\t', ' ');
			HDC hdc = GetDC((HWND)this->hwnd);
			void *fnt = this->GetFont();
			if (fnt)
			{
				SelectObject(hdc, fnt);
			}
			GetTextExtentExPoint(hdc, line, this->caretX, 0, 0, 0, &sz);
			xPos = this->dispLineNumW + sz.cx;
			ReleaseDC((HWND)this->hwnd, hdc);
			MemFree(line);
			MemFree(rbuff);
		}
		mutUsage.EndUse();
	}
	if (xScr > xPos)
	{
		SetScrollPos((HWND)this->hwnd, SB_HORZ, xPos, FALSE);
		needRedraw = true;
	}
	else
	{
		RECT rc;
		GetClientRect((HWND)this->hwnd, &rc);
		if (xScr + (rc.right - rc.left) - 1 < xPos)
		{
			SetScrollPos((HWND)this->hwnd, SB_HORZ, xPos - (rc.right - rc.left - 1), FALSE);
			needRedraw = true;
		}
	}

	if (needRedraw)
	{
		this->Redraw();
	}
}

UOSInt UI::GUITextFileView::GetLineCharCnt(UOSInt lineNum)
{
	UInt64 lineOfst;
	UInt64 nextOfst;
	UInt8 *rbuff;

	lineOfst = this->lineOfsts->GetItem(lineNum);
	nextOfst = this->lineOfsts->GetItem(lineNum + 1);
	if (nextOfst == 0 && this->lineOfsts->GetCount() - 1 <= lineNum + 1)
	{
		nextOfst = this->lineOfsts->GetItem(this->lineOfsts->GetCount() - 1);
	}
	if (nextOfst > lineOfst)
	{
		Text::Encoding enc(this->fileCodePage);
		rbuff = MemAlloc(UInt8, (UOSInt)(nextOfst - lineOfst));
		this->fs->SeekFromBeginning(lineOfst);
		this->fs->Read(rbuff, (UOSInt)(nextOfst - lineOfst));
		UOSInt charCnt = enc.CountWChars(rbuff, (UOSInt)(nextOfst - lineOfst));
		WChar *line = MemAlloc(WChar, charCnt + 1);
		enc.WFromBytes(line, rbuff, (UOSInt)(nextOfst - lineOfst), 0);
		charCnt -= 1;
		if (charCnt > 0 && (line[charCnt - 1] == 0x0d || line[charCnt - 1] == 0x0a))
		{
			charCnt--;
		}
		MemFree(line);
		MemFree(rbuff);
		return charCnt;
	}
	else
	{
		return 0;
	}
}

void UI::GUITextFileView::GetPosFromByteOfst(UInt64 byteOfst, UInt32 *txtPosX, UOSInt *txtPosY)
{
	OSInt lineNum = this->lineOfsts->SortedIndexOf(byteOfst);
	if (lineNum >= 0)
	{
		*txtPosX = 0;
		*txtPosY = (UOSInt)lineNum;
		return;
	}
	lineNum = ~lineNum - 1;
	UInt64 thisOfst = this->lineOfsts->GetItem((UOSInt)lineNum);
	UOSInt buffSize = (UOSInt)(byteOfst - thisOfst);
	UInt8 *rbuff = MemAlloc(UInt8, buffSize);
	Text::Encoding enc(this->fileCodePage);
	Sync::MutexUsage mutUsage(this->mut);
	this->fs->SeekFromBeginning(thisOfst);
	this->fs->Read(rbuff, buffSize);
	mutUsage.EndUse();
	*txtPosX = (UInt32)enc.CountWChars(rbuff, buffSize);
	*txtPosY = lineNum;
	MemFree(rbuff);
}

void UI::GUITextFileView::UpdateCaretSel(Bool noRedraw)
{
	Int16 shiftLock = GetKeyState(VK_LSHIFT) | GetKeyState(VK_RSHIFT);
	if (shiftLock & 0x80)
	{
		this->selEndX = this->caretX;
		this->selEndY = this->caretY;
		if (!noRedraw)
			this->Redraw();
	}
	else if (this->selStartX == this->selEndX && this->selStartY == this->selEndY)
	{
		this->selStartX = this->caretX;
		this->selStartY = this->caretY;
		this->selEndX = this->caretX;
		this->selEndY = this->caretY;
	}
	else
	{
		this->selStartX = this->caretX;
		this->selStartY = this->caretY;
		this->selEndX = this->caretX;
		this->selEndY = this->caretY;
		if (!noRedraw)
			this->Redraw();
	}
}

void UI::GUITextFileView::EventTextPosUpdated()
{
	UOSInt i = this->textPosUpdHdlr->GetCount();
	while (i-- > 0)
	{
		this->textPosUpdHdlr->GetItem(i)(this->textPosUpdObj->GetItem(i), this->caretX, this->caretY);
	}
}

UI::GUITextFileView::GUITextFileView(UI::GUICore *ui, UI::GUIClientControl *parent) : UI::GUIControl(ui, parent)
{
	this->fileName = 0;
	this->threadToStop = false;
	this->threadRunning = false;
	this->loadNewFile = false;
	this->readingFile = false;
	this->readBuff = MemAlloc(UInt8, READBUFFSIZE);
	this->readBuffOfst = 0;
	this->readBuffSize = 0;
	this->fs = 0;
	this->codePage = 0;
	this->bgBmp = 0;
	this->lastLineCnt = -1;
	this->dispLineNumW = 0;
	this->pageLineCnt = 0;
	this->pageLineHeight = 12;
	this->mouseDown = false;
	this->selStartX = 0;
	this->selStartY = 0;
	this->selEndX = 0;
	this->selEndY = 0;
	this->caretX = 0;
	this->caretY = 0;
	this->isSearching = false;
	this->srchText = 0;
	this->fileSize = 0;
	NEW_CLASS(lineOfsts, Data::ArrayListUInt64());
	NEW_CLASS(this->textPosUpdHdlr, Data::ArrayList<TextPosEvent>());
	NEW_CLASS(this->textPosUpdObj, Data::ArrayList<void *>());

	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		Init(((UI::GUICoreWin*)this->ui)->GetHInst());
	}
	NEW_CLASS(this->evtThread, Sync::Event(true, (const UTF8Char*)"UI.MSWindowTextFileView.evtThread"));
	NEW_CLASS(this->mut, Sync::Mutex());
	Sync::Thread::Create(ProcThread, this);

	Int32 style = WS_CHILD | WS_HSCROLL | WS_VSCROLL | WS_TABSTOP;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)this->ui)->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"", style, WS_EX_CONTROLPARENT, 0, 0, 200, 200);
	SetTimer((HWND)this->hwnd, 1, 1000, 0);
}

UI::GUITextFileView::~GUITextFileView()
{
	KillTimer((HWND)this->hwnd, 1);
	this->threadToStop = true;
	this->evtThread->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	MemFree(this->readBuff);
	if (this->fs)
	{
		DEL_CLASS(this->fs);
	}
	if (this->fileName)
	{
		Text::StrDelNew(this->fileName);
		this->fileName = 0;
	}
	if (this->bgBmp)
	{
		DeleteObject((HBITMAP)this->bgBmp);
		this->bgBmp = 0;
	}
	DEL_CLASS(this->evtThread);
	DEL_CLASS(this->mut);
	DEL_CLASS(this->lineOfsts);
	DEL_CLASS(this->textPosUpdHdlr);
	DEL_CLASS(this->textPosUpdObj);
	SDEL_TEXT(this->srchText);
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
		Deinit(((UI::GUICoreWin*)this->ui)->GetHInst());
	}
}

const UTF8Char *UI::GUITextFileView::GetObjectClass()
{
	return (const UTF8Char*)"TextFileView";
}

OSInt UI::GUITextFileView::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUITextFileView::UpdateFont()
{
	this->drawFont = this->GetFont();
	UpdateScrollBar();
}

Bool UI::GUITextFileView::IsLoading()
{
	return this->readingFile;
}

UOSInt UI::GUITextFileView::GetLineCount()
{
	return this->lineOfsts->GetCount() - 1;
}

void UI::GUITextFileView::SetCodePage(UInt32 codePage)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->codePage = codePage;
}

Bool UI::GUITextFileView::LoadFile(const UTF8Char *fileName)
{
	while (this->isSearching)
	{
		Sync::Thread::Sleep(10);
	}

	Sync::MutexUsage mutUsage(this->mut);
	if (this->fileName)
	{
		Text::StrDelNew(this->fileName);
	}
	this->fileName = Text::StrCopyNew(fileName);
	this->loadNewFile = true;
	this->fileSize = 0;

	this->lastLineCnt = 0;
	SetScrollRange((HWND)this->hwnd, SB_VERT, 0, 0, TRUE);

	this->caretX = 0;
	this->caretY = 0;
	this->selStartX = 0;
	this->selStartY = 0;
	this->selEndX = 0;
	this->selEndY = 0;
	this->selLastX = 0;
	this->selLastY = 0;
	this->evtThread->Set();
	mutUsage.EndUse();
	this->EventTextPosUpdated();
	return true;
}

const UTF8Char *UI::GUITextFileView::GetFileName()
{
	return this->fileName;
}

void UI::GUITextFileView::GetTextPos(Int32 scnPosX, Int32 scnPosY, UInt32 *textPosX, UOSInt *textPosY)
{
	OSInt textY = GetScrollPos((HWND)this->hwnd, SB_VERT) + (scnPosY / this->pageLineHeight);
	Int32 drawX;
	Int32 textX = 0;
	if (textY >= (OSInt)this->lineOfsts->GetCount() - 1)
	{
		textY = this->lineOfsts->GetCount() - 1;
		textX = 0;
		*textPosX = textX;
		*textPosY = textY;
		return;
	}
	drawX = scnPosX + GetScrollPos((HWND)this->hwnd, SB_HORZ) - this->dispLineNumW;
	if (drawX < 0)
	{
		textX = 0;
		*textPosX = textX;
		*textPosY = textY;
		return;
	}
	if (this->fs)
	{
		UInt8 *rbuff;
		UInt64 lineOfst;
		UInt64 nextOfst;
		Sync::MutexUsage mutUsage(this->mut);
		lineOfst = this->lineOfsts->GetItem(textY);
		nextOfst = this->lineOfsts->GetItem(textY + 1);
		if (nextOfst == 0 && (OSInt)this->lineOfsts->GetCount() - 1 <= textY + 1)
		{
			nextOfst = this->lineOfsts->GetItem(this->lineOfsts->GetCount() - 1);
		}
		if (nextOfst > lineOfst)
		{
			WChar *line;
			SIZE sz;

			Text::Encoding enc(this->fileCodePage);
			rbuff = MemAlloc(UInt8, (UOSInt)(nextOfst - lineOfst));
			this->fs->SeekFromBeginning(lineOfst);
			this->fs->Read(rbuff, (UOSInt)(nextOfst - lineOfst));
			UOSInt charCnt = enc.CountWChars(rbuff, (UOSInt)(nextOfst - lineOfst));
			line = MemAlloc(WChar, charCnt + 1);
			enc.WFromBytes(line, rbuff, (UOSInt)(nextOfst - lineOfst), 0);
			Text::StrReplace(line, '\t', ' ');
			charCnt -= 1;
			if (charCnt > 0 && (line[charCnt] == 0xd || line[charCnt] == 0xa))
			{
				charCnt--;
			}
			HDC hdc = GetDC((HWND)this->hwnd);
			void *fnt = this->GetFont();
			if (fnt)
			{
				SelectObject(hdc, fnt);
			}
			GetTextExtentExPoint(hdc, line, (Int32)charCnt, drawX, &textX, 0, &sz);
			ReleaseDC((HWND)this->hwnd, hdc);
			MemFree(line);
			MemFree(rbuff);
		}
		mutUsage.EndUse();
	}
	*textPosX = textX;
	*textPosY = textY;
}

UOSInt UI::GUITextFileView::GetTextPosY()
{
	return this->caretY;
}

UInt32 UI::GUITextFileView::GetTextPosX()
{
	return this->caretX;
}

void UI::GUITextFileView::GoToText(UOSInt newPosY, UInt32 newPosX)
{
	UOSInt lineCharCnt;
	if (newPosY >= this->lineOfsts->GetCount())
	{
		newPosY = this->lineOfsts->GetCount() - 1;
	}
	if (newPosY < 0)
	{
		newPosY = 0;
	}
	if (newPosX < 0)
	{
		newPosX = 0;
	}
	else if (newPosX > (lineCharCnt = this->GetLineCharCnt(newPosY)))
	{
		newPosX = (Int32)lineCharCnt;
	}
	this->caretX = newPosX;
	this->caretY = newPosY;
	this->EnsureCaretVisible();
	this->EventTextPosUpdated();
}

void UI::GUITextFileView::SearchText(const UTF8Char *txt)
{
	if (this->fs && !this->isSearching)
	{
		SDEL_TEXT(this->srchText);
		this->srchText = Text::StrCopyNew(txt);
		this->isSearching = true;
		this->evtThread->Set();
	}
}

void UI::GUITextFileView::HandleTextPosUpdate(TextPosEvent hdlr, void *obj)
{
	this->textPosUpdHdlr->Add(hdlr);
	this->textPosUpdObj->Add(obj);
}
