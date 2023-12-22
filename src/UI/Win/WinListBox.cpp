#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinListBox.h"

#ifdef _WIN32_WCE
#define GWLP_WNDPROC GWL_WNDPROC
#endif

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::Win::WinListBox::LBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::Win::WinListBox *me = (UI::Win::WinListBox*)UI::Win::WinCore::MSGetWindowObj((ControlHandle*)hWnd, GWL_USERDATA);
	OSInt index;
	switch (msg)
	{
	case WM_RBUTTONDOWN:
//		x = LOWORD(lParam);
//		y = HIWORD(lParam);
		index = SendMessage((HWND)hWnd, LB_ITEMFROMPOINT, 0, lParam);
		me->SetSelectedIndex((UOSInt)index);
		me->EventRightClick(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)) + me->GetScreenPosP());
		break;
	default:
		return CallWindowProc(me->wndproc, (HWND)hWnd, msg, wParam, lParam);
	}
	return 0;
}

UI::Win::WinListBox::WinListBox(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Bool multiSelect) : UI::GUIListBox(ui, parent)
{
	this->mulSel = multiSelect;

	Math::Size2DDbl sz = parent->GetClientSize();
	UInt32 style = WS_TABSTOP | WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	if (this->mulSel)
	{
		style = style | LBS_EXTENDEDSEL;
	}
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, L"LISTBOX", (const UTF8Char*)"ListBox", style, WS_EX_CLIENTEDGE, 0, 0, sz.x, sz.y);
	this->wndproc = (WNDPROC)UI::Win::WinCore::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)LBWndProc);
}

UI::Win::WinListBox::~WinListBox()
{
	UI::Win::WinCore::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)this->wndproc);
}


UOSInt UI::Win::WinListBox::AddItem(NotNullPtr<Text::String> itemText, void *itemObj)
{
	UOSInt i = Text::StrUTF8_WCharCntC(itemText->v, itemText->leng);
	WChar *s = MemAlloc(WChar, i + 1);
	Text::StrUTF8_WCharC(s, itemText->v, itemText->leng, 0);
	i = (UOSInt)SendMessage((HWND)hwnd, LB_ADDSTRING, 0, (LPARAM)s);
	MemFree(s);
	if (i == INVALID_INDEX)
		return i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, LB_SETITEMDATA, i, (LPARAM)itemObj);
	}
	return i;
}

UOSInt UI::Win::WinListBox::AddItem(Text::CStringNN itemText, void *itemObj)
{
	UOSInt i = Text::StrUTF8_WCharCnt(itemText.v);
	WChar *s = MemAlloc(WChar, i + 1);
	Text::StrUTF8_WChar(s, itemText.v, 0);
	i = (UOSInt)SendMessage((HWND)hwnd, LB_ADDSTRING, 0, (LPARAM)s);
	MemFree(s);
	if (i == INVALID_INDEX)
		return i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, LB_SETITEMDATA, i, (LPARAM)itemObj);
	}
	return i;
}

UOSInt UI::Win::WinListBox::AddItem(const WChar *itemText, void *itemObj)
{
	OSInt i = SendMessage((HWND)hwnd, LB_ADDSTRING, 0, (LPARAM)itemText);
	if (i < 0)
		return (UOSInt)i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, LB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj);
	}
	return (UOSInt)i;
}

UOSInt UI::Win::WinListBox::InsertItem(UOSInt index, Text::String *itemText, void *itemObj)
{
	const WChar *wptr = Text::StrToWCharNew(itemText->v);
	OSInt i = SendMessage((HWND)hwnd, LB_INSERTSTRING, index, (LPARAM)wptr);
	Text::StrDelNew(wptr);
	if (i < 0)
		return INVALID_INDEX;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, LB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj);
	}
	return (UOSInt)i;
}

UOSInt UI::Win::WinListBox::InsertItem(UOSInt index, Text::CStringNN itemText, void *itemObj)
{
	const WChar *wptr = Text::StrToWCharNew(itemText.v);
	OSInt i = SendMessage((HWND)hwnd, LB_INSERTSTRING, index, (LPARAM)wptr);
	Text::StrDelNew(wptr);
	if (i < 0)
		return INVALID_INDEX;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, LB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj);
	}
	return (UOSInt)i;
}

UOSInt UI::Win::WinListBox::InsertItem(UOSInt index, const WChar *itemText, void *itemObj)
{
	OSInt i = SendMessage((HWND)hwnd, LB_INSERTSTRING, index, (LPARAM)itemText);
	if (i < 0)
		return INVALID_INDEX;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, LB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj);
	}
	return (UOSInt)i;
}

void *UI::Win::WinListBox::RemoveItem(UOSInt index)
{
	void *obj = (void*)SendMessage((HWND)hwnd, LB_GETITEMDATA, index, 0);
	SendMessage((HWND)hwnd, LB_DELETESTRING, index, 0);
	return obj;
}

void *UI::Win::WinListBox::GetItem(UOSInt index)
{
	return (void*)SendMessage((HWND)hwnd, LB_GETITEMDATA, index, 0);
}

void UI::Win::WinListBox::ClearItems()
{
	SendMessage((HWND)hwnd, LB_RESETCONTENT, 0, 0);
}

UOSInt UI::Win::WinListBox::GetCount()
{
	return (UOSInt)SendMessage((HWND)hwnd, LB_GETCOUNT, 0, 0);
}

void UI::Win::WinListBox::SetSelectedIndex(UOSInt index)
{
	SendMessage((HWND)hwnd, LB_SETCURSEL, index, 0);
	this->EventSelectionChange();
}

UOSInt UI::Win::WinListBox::GetSelectedIndex()
{
	return (UOSInt)(OSInt)SendMessage((HWND)hwnd, LB_GETCURSEL, 0, 0);
}

Bool UI::Win::WinListBox::GetSelectedIndices(Data::ArrayList<UInt32> *indices)
{
	if (this->mulSel)
	{
		OSInt ret;
		UOSInt i = 0;
		UOSInt j = this->GetCount();
		while (i < j)
		{
			ret = SendMessage((HWND)hwnd, LB_GETSEL, i, 0);
			if (ret < 0)
				return false;
			if (ret)
				indices->Add((UInt32)i);
			i++;
		}
		return true;
	}
	else
	{
		OSInt i = SendMessage((HWND)hwnd, LB_GETCURSEL, 0, 0);
		if (i >= 0)
			indices->Add((UInt32)i);
		return true;
	}
}

void *UI::Win::WinListBox::GetSelectedItem()
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItem(currSel);
}

UTF8Char *UI::Win::WinListBox::GetSelectedItemText(UTF8Char *buff)
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItemText(buff, currSel);
}

WChar *UI::Win::WinListBox::GetSelectedItemText(WChar *buff)
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItemText(buff, currSel);
}

Optional<Text::String> UI::Win::WinListBox::GetSelectedItemTextNew()
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItemTextNew(currSel);
}

UTF8Char *UI::Win::WinListBox::GetItemText(UTF8Char *buff, UOSInt index)
{
	NotNullPtr<Text::String> s;
	if (!this->GetItemTextNew(index).SetTo(s))
	{
		return 0;
	}
	buff = s->ConcatTo(buff);
	s->Release();
	return buff;
}

WChar *UI::Win::WinListBox::GetItemText(WChar *buff, UOSInt index)
{
	OSInt strLen = SendMessageW((HWND)hwnd, LB_GETTEXT, index, (LPARAM)buff);
	if (strLen == LB_ERR)
	{
		return 0;
	}
	else
	{
		return &buff[strLen];
	}
}

void UI::Win::WinListBox::SetItemText(UOSInt index, Text::CStringNN text)
{
	void *item = GetItem(index);
	this->RemoveItem(index);
	this->InsertItem(index, text, item);
}

Optional<Text::String> UI::Win::WinListBox::GetItemTextNew(UOSInt index)
{
	OSInt strLen = SendMessageW((HWND)hwnd, LB_GETTEXTLEN, index, 0);
	if (strLen == LB_ERR)
		return 0;
	WChar *sbuff = MemAlloc(WChar, (UOSInt)strLen + 1);
	strLen = SendMessageW((HWND)hwnd, LB_GETTEXT, index, (LPARAM)sbuff);
	if (strLen == LB_ERR)
	{
		MemFree(sbuff);
		return 0;
	}
	else
	{
		NotNullPtr<Text::String> ret = Text::String::NewNotNull(sbuff);
		MemFree(sbuff);
		return ret.Ptr();
	}
}

OSInt UI::Win::WinListBox::GetItemHeight()
{
	TEXTMETRIC tm;
	HDC hdc = GetDC((HWND)this->hwnd);
	GetTextMetrics(hdc, &tm);
	return tm.tmHeight;
}

OSInt UI::Win::WinListBox::OnNotify(UInt32 code, void *lParam)
{
	switch (code)
	{
	case LBN_SELCHANGE:
		this->EventSelectionChange();
		return 0;
	case LBN_DBLCLK:
		this->EventDoubleClick();
		return 0;
	}
	return 0;
}
