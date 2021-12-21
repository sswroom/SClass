#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIListBox.h"
#include <windows.h>

#ifdef _WIN32_WCE
#define GWLP_WNDPROC GWL_WNDPROC
#endif

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::GUIListBox::LBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUIListBox *me = (UI::GUIListBox*)UI::GUICoreWin::MSGetWindowObj((ControlHandle*)hWnd, GWL_USERDATA);
	OSInt index;
	switch (msg)
	{
	case WM_RBUTTONDOWN:
//		x = LOWORD(lParam);
//		y = HIWORD(lParam);
		index = SendMessage((HWND)hWnd, LB_ITEMFROMPOINT, 0, lParam);
		me->SetSelectedIndex((UOSInt)index);
		me->EventRightClick((Int16)LOWORD(lParam), (Int16)HIWORD(lParam));
		break;
	default:
		return CallWindowProc((WNDPROC)me->clsData, (HWND)hWnd, msg, wParam, lParam);
	}
	return 0;
}

UI::GUIListBox::GUIListBox(UI::GUICore *ui, UI::GUIClientControl *parent, Bool multiSelect) : UI::GUIControl(ui, parent)
{
	Double w;
	Double h;
	NEW_CLASS(this->selChgHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->selChgObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->dblClickHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->dblClickObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->rightClickHdlrs, Data::ArrayList<UI::GUIControl::MouseEventHandler>());
	NEW_CLASS(this->rightClickObjs, Data::ArrayList<void*>());
	this->mulSel = multiSelect;

	parent->GetClientSize(&w, &h);
	UInt32 style = WS_TABSTOP | WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	if (this->mulSel)
	{
		style = style | LBS_EXTENDEDSEL;
	}
	this->InitControl(((UI::GUICoreWin*)ui)->GetHInst(), parent, L"LISTBOX", (const UTF8Char*)"ListBox", style, WS_EX_CLIENTEDGE, 0, 0, w, h);
	this->clsData = (ClassData*)UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)LBWndProc);
}

UI::GUIListBox::~GUIListBox()
{
	UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)this->clsData);
	DEL_CLASS(this->selChgHdlrs);
	DEL_CLASS(this->selChgObjs);
	DEL_CLASS(this->dblClickHdlrs);
	DEL_CLASS(this->dblClickObjs);
	DEL_CLASS(this->rightClickHdlrs);
	DEL_CLASS(this->rightClickObjs);
}

void UI::GUIListBox::EventSelectionChange()
{
	UOSInt i = this->selChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs->GetItem(i)(this->selChgObjs->GetItem(i));
	}
}

void UI::GUIListBox::EventDoubleClick()
{
	UOSInt i = this->dblClickHdlrs->GetCount();
	while (i-- > 0)
	{
		this->dblClickHdlrs->GetItem(i)(this->dblClickObjs->GetItem(i));
	}
}

void UI::GUIListBox::EventRightClick(OSInt x, OSInt y)
{
	UOSInt i = this->rightClickHdlrs->GetCount();
	if (i > 0)
	{
		OSInt scnX;
		OSInt scnY;
		this->GetScreenPosP(&scnX, &scnY);
		while (i-- > 0)
		{
			this->rightClickHdlrs->GetItem(i)(this->rightClickObjs->GetItem(i), x + scnX, y + scnY, UI::GUIControl::MBTN_RIGHT);
		}
	}
}

UOSInt UI::GUIListBox::AddItem(Text::String *itemText, void *itemObj)
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

UOSInt UI::GUIListBox::AddItem(const UTF8Char *itemText, void *itemObj)
{
	UOSInt i = Text::StrUTF8_WCharCnt(itemText);
	WChar *s = MemAlloc(WChar, i + 1);
	Text::StrUTF8_WChar(s, itemText, 0);
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

UOSInt UI::GUIListBox::AddItem(const WChar *itemText, void *itemObj)
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

UOSInt UI::GUIListBox::InsertItem(UOSInt index, Text::String *itemText, void *itemObj)
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

UOSInt UI::GUIListBox::InsertItem(UOSInt index, const UTF8Char *itemText, void *itemObj)
{
	const WChar *wptr = Text::StrToWCharNew(itemText);
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

UOSInt UI::GUIListBox::InsertItem(UOSInt index, const WChar *itemText, void *itemObj)
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

void *UI::GUIListBox::RemoveItem(UOSInt index)
{
	void *obj = (void*)SendMessage((HWND)hwnd, LB_GETITEMDATA, index, 0);
	SendMessage((HWND)hwnd, LB_DELETESTRING, index, 0);
	return obj;
}

void *UI::GUIListBox::GetItem(UOSInt index)
{
	return (void*)SendMessage((HWND)hwnd, LB_GETITEMDATA, index, 0);
}

void UI::GUIListBox::ClearItems()
{
	SendMessage((HWND)hwnd, LB_RESETCONTENT, 0, 0);
}

UOSInt UI::GUIListBox::GetCount()
{
	return (UOSInt)SendMessage((HWND)hwnd, LB_GETCOUNT, 0, 0);
}

void UI::GUIListBox::SetSelectedIndex(UOSInt index)
{
	SendMessage((HWND)hwnd, LB_SETCURSEL, index, 0);
	this->EventSelectionChange();
}

UOSInt UI::GUIListBox::GetSelectedIndex()
{
	return (UOSInt)(OSInt)SendMessage((HWND)hwnd, LB_GETCURSEL, 0, 0);
}

Bool UI::GUIListBox::GetSelectedIndices(Data::ArrayList<UInt32> *indices)
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

void *UI::GUIListBox::GetSelectedItem()
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItem(currSel);
}

UTF8Char *UI::GUIListBox::GetSelectedItemText(UTF8Char *buff)
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItemText(buff, currSel);
}

WChar *UI::GUIListBox::GetSelectedItemText(WChar *buff)
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItemText(buff, currSel);
}

Text::String *UI::GUIListBox::GetSelectedItemTextNew()
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItemTextNew(currSel);
}

UTF8Char *UI::GUIListBox::GetItemText(UTF8Char *buff, UOSInt index)
{
	Text::String *s = this->GetItemTextNew(index);
	if (s == 0)
	{
		return 0;
	}
	buff = s->ConcatTo(buff);
	s->Release();
	return buff;
}

WChar *UI::GUIListBox::GetItemText(WChar *buff, UOSInt index)
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

void UI::GUIListBox::SetItemText(UOSInt index, const UTF8Char *text)
{
	void *item = GetItem(index);
	this->RemoveItem(index);
	this->InsertItem(index, text, item);
}

Text::String *UI::GUIListBox::GetItemTextNew(UOSInt index)
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
		Text::String *ret = Text::String::NewNotNull(sbuff);
		MemFree(sbuff);
		return ret;
	}
}

OSInt UI::GUIListBox::GetItemHeight()
{
	TEXTMETRIC tm;
	HDC hdc = GetDC((HWND)this->hwnd);
	GetTextMetrics(hdc, &tm);
	return tm.tmHeight;
}

const UTF8Char *UI::GUIListBox::GetObjectClass()
{
	return (const UTF8Char*)"ListBox";
}

OSInt UI::GUIListBox::OnNotify(UInt32 code, void *lParam)
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

void UI::GUIListBox::HandleSelectionChange(UI::UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs->Add(hdlr);
	this->selChgObjs->Add(userObj);
}

void UI::GUIListBox::HandleDoubleClicked(UI::UIEvent hdlr, void *userObj)
{
	this->dblClickHdlrs->Add(hdlr);
	this->dblClickObjs->Add(userObj);
}

void UI::GUIListBox::HandleRightClicked(UI::GUIControl::MouseEventHandler hdlr, void *userObj)
{
	this->rightClickHdlrs->Add(hdlr);
	this->rightClickObjs->Add(userObj);
}
