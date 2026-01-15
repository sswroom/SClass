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

IntOS __stdcall UI::Win::WinListBox::LBWndProc(void *hWnd, UInt32 msg, UIntOS wParam, IntOS lParam)
{
	UI::Win::WinListBox *me = (UI::Win::WinListBox*)UI::Win::WinCore::MSGetWindowObj((ControlHandle*)hWnd, GWL_USERDATA);
	IntOS index;
	switch (msg)
	{
	case WM_RBUTTONDOWN:
//		x = LOWORD(lParam);
//		y = HIWORD(lParam);
		index = SendMessage((HWND)hWnd, LB_ITEMFROMPOINT, 0, lParam);
		me->SetSelectedIndex((UIntOS)index);
		me->EventRightClick(Math::Coord2D<IntOS>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)) + me->GetScreenPosP());
		break;
	default:
		return CallWindowProc(me->wndproc, (HWND)hWnd, msg, wParam, lParam);
	}
	return 0;
}

UI::Win::WinListBox::WinListBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Bool multiSelect) : UI::GUIListBox(ui, parent)
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
	this->wndproc = (WNDPROC)UI::Win::WinCore::MSSetWindowObj(this->hwnd.OrNull(), GWLP_WNDPROC, (IntOS)LBWndProc);
}

UI::Win::WinListBox::~WinListBox()
{
	UI::Win::WinCore::MSSetWindowObj(this->hwnd.OrNull(), GWLP_WNDPROC, (IntOS)this->wndproc);
}


UIntOS UI::Win::WinListBox::AddItem(NN<Text::String> itemText, AnyType itemObj)
{
	UIntOS i = Text::StrUTF8_WCharCntC(itemText->v, itemText->leng);
	WChar *s = MemAlloc(WChar, i + 1);
	Text::StrUTF8_WCharC(s, itemText->v, itemText->leng, 0);
	i = (UIntOS)SendMessage((HWND)hwnd.OrNull(), LB_ADDSTRING, 0, (LPARAM)s);
	MemFree(s);
	if (i == INVALID_INDEX)
		return i;
	if (itemObj.NotNull())
	{
		SendMessage((HWND)hwnd.OrNull(), LB_SETITEMDATA, i, (LPARAM)itemObj.p);
	}
	return i;
}

UIntOS UI::Win::WinListBox::AddItem(Text::CStringNN itemText, AnyType itemObj)
{
	UIntOS i = Text::StrUTF8_WCharCnt(itemText.v);
	WChar *s = MemAlloc(WChar, i + 1);
	Text::StrUTF8_WChar(s, itemText.v, 0);
	i = (UIntOS)SendMessage((HWND)hwnd.OrNull(), LB_ADDSTRING, 0, (LPARAM)s);
	MemFree(s);
	if (i == INVALID_INDEX)
		return i;
	if (itemObj.NotNull())
	{
		SendMessage((HWND)hwnd.OrNull(), LB_SETITEMDATA, i, (LPARAM)itemObj.p);
	}
	return i;
}

UIntOS UI::Win::WinListBox::AddItem(const WChar *itemText, AnyType itemObj)
{
	IntOS i = SendMessage((HWND)hwnd.OrNull(), LB_ADDSTRING, 0, (LPARAM)itemText);
	if (i < 0)
		return (UIntOS)i;
	if (itemObj.NotNull())
	{
		SendMessage((HWND)hwnd.OrNull(), LB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj.p);
	}
	return (UIntOS)i;
}

UIntOS UI::Win::WinListBox::InsertItem(UIntOS index, NN<Text::String> itemText, AnyType itemObj)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(itemText->v);
	IntOS i = SendMessage((HWND)hwnd.OrNull(), LB_INSERTSTRING, index, (LPARAM)wptr.Ptr());
	Text::StrDelNew(wptr);
	if (i < 0)
		return INVALID_INDEX;
	if (itemObj.NotNull())
	{
		SendMessage((HWND)hwnd.OrNull(), LB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj.p);
	}
	return (UIntOS)i;
}

UIntOS UI::Win::WinListBox::InsertItem(UIntOS index, Text::CStringNN itemText, AnyType itemObj)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(itemText.v);
	IntOS i = SendMessage((HWND)hwnd.OrNull(), LB_INSERTSTRING, index, (LPARAM)wptr.Ptr());
	Text::StrDelNew(wptr);
	if (i < 0)
		return INVALID_INDEX;
	if (itemObj.NotNull())
	{
		SendMessage((HWND)hwnd.OrNull(), LB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj.p);
	}
	return (UIntOS)i;
}

UIntOS UI::Win::WinListBox::InsertItem(UIntOS index, const WChar *itemText, AnyType itemObj)
{
	IntOS i = SendMessage((HWND)hwnd.OrNull(), LB_INSERTSTRING, index, (LPARAM)itemText);
	if (i < 0)
		return INVALID_INDEX;
	if (itemObj.NotNull())
	{
		SendMessage((HWND)hwnd.OrNull(), LB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj.p);
	}
	return (UIntOS)i;
}

AnyType UI::Win::WinListBox::RemoveItem(UIntOS index)
{
	AnyType obj = (void*)SendMessage((HWND)hwnd.OrNull(), LB_GETITEMDATA, index, 0);
	SendMessage((HWND)hwnd.OrNull(), LB_DELETESTRING, index, 0);
	return obj;
}

AnyType UI::Win::WinListBox::GetItem(UIntOS index)
{
	return (void*)SendMessage((HWND)hwnd.OrNull(), LB_GETITEMDATA, index, 0);
}

void UI::Win::WinListBox::ClearItems()
{
	SendMessage((HWND)hwnd.OrNull(), LB_RESETCONTENT, 0, 0);
}

UIntOS UI::Win::WinListBox::GetCount()
{
	return (UIntOS)SendMessage((HWND)hwnd.OrNull(), LB_GETCOUNT, 0, 0);
}

void UI::Win::WinListBox::SetSelectedIndex(UIntOS index)
{
	SendMessage((HWND)hwnd.OrNull(), LB_SETCURSEL, index, 0);
	this->EventSelectionChange();
}

UIntOS UI::Win::WinListBox::GetSelectedIndex()
{
	return (UIntOS)(IntOS)SendMessage((HWND)hwnd.OrNull(), LB_GETCURSEL, 0, 0);
}

Bool UI::Win::WinListBox::GetSelectedIndices(NN<Data::ArrayListNative<UInt32>> indices)
{
	if (this->mulSel)
	{
		IntOS ret;
		UIntOS i = 0;
		UIntOS j = this->GetCount();
		while (i < j)
		{
			ret = SendMessage((HWND)hwnd.OrNull(), LB_GETSEL, i, 0);
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
		IntOS i = SendMessage((HWND)hwnd.OrNull(), LB_GETCURSEL, 0, 0);
		if (i >= 0)
			indices->Add((UInt32)i);
		return true;
	}
}

AnyType UI::Win::WinListBox::GetSelectedItem()
{
	UIntOS currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItem(currSel);
}

UnsafeArrayOpt<UTF8Char> UI::Win::WinListBox::GetSelectedItemText(UnsafeArray<UTF8Char> buff)
{
	UIntOS currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return nullptr;
	return GetItemText(buff, currSel);
}

WChar *UI::Win::WinListBox::GetSelectedItemText(WChar *buff)
{
	UIntOS currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItemText(buff, currSel);
}

Optional<Text::String> UI::Win::WinListBox::GetSelectedItemTextNew()
{
	UIntOS currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return nullptr;
	return GetItemTextNew(currSel);
}

UnsafeArrayOpt<UTF8Char> UI::Win::WinListBox::GetItemText(UnsafeArray<UTF8Char> buff, UIntOS index)
{
	NN<Text::String> s;
	if (!this->GetItemTextNew(index).SetTo(s))
	{
		return nullptr;
	}
	buff = s->ConcatTo(buff);
	s->Release();
	return buff;
}

WChar *UI::Win::WinListBox::GetItemText(WChar *buff, UIntOS index)
{
	IntOS strLen = SendMessageW((HWND)hwnd.OrNull(), LB_GETTEXT, index, (LPARAM)buff);
	if (strLen == LB_ERR)
	{
		return 0;
	}
	else
	{
		return &buff[strLen];
	}
}

void UI::Win::WinListBox::SetItemText(UIntOS index, Text::CStringNN text)
{
	AnyType item = GetItem(index);
	this->RemoveItem(index);
	this->InsertItem(index, text, item);
}

Optional<Text::String> UI::Win::WinListBox::GetItemTextNew(UIntOS index)
{
	IntOS strLen = SendMessageW((HWND)hwnd.OrNull(), LB_GETTEXTLEN, index, 0);
	if (strLen == LB_ERR)
		return nullptr;
	WChar *sbuff = MemAlloc(WChar, (UIntOS)strLen + 1);
	strLen = SendMessageW((HWND)hwnd.OrNull(), LB_GETTEXT, index, (LPARAM)sbuff);
	if (strLen == LB_ERR)
	{
		MemFree(sbuff);
		return nullptr;
	}
	else
	{
		NN<Text::String> ret = Text::String::NewNotNull(sbuff);
		MemFree(sbuff);
		return ret.Ptr();
	}
}

IntOS UI::Win::WinListBox::GetItemHeight()
{
	TEXTMETRIC tm;
	HDC hdc = GetDC((HWND)this->hwnd.OrNull());
	GetTextMetrics(hdc, &tm);
	return tm.tmHeight;
}

IntOS UI::Win::WinListBox::OnNotify(UInt32 code, void *lParam)
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
