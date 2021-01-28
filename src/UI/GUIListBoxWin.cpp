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
	UI::GUIListBox *me = (UI::GUIListBox*)UI::GUICoreWin::MSGetWindowObj(hWnd, GWL_USERDATA);
	OSInt index;
	switch (msg)
	{
	case WM_RBUTTONDOWN:
//		x = LOWORD(lParam);
//		y = HIWORD(lParam);
		index = SendMessage((HWND)hWnd, LB_ITEMFROMPOINT, 0, lParam);
		me->SetSelectedIndex(index);
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
	Int32 style = WS_TABSTOP | WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	if (this->mulSel)
	{
		style = style | LBS_EXTENDEDSEL;
	}
	this->InitControl(((UI::GUICoreWin*)ui)->GetHInst(), parent, L"LISTBOX", (const UTF8Char*)"ListBox", style, WS_EX_CLIENTEDGE, 0, 0, w, h);
	this->clsData = (void*)UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)LBWndProc);
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
	OSInt i = this->selChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs->GetItem(i)(this->selChgObjs->GetItem(i));
	}
}

void UI::GUIListBox::EventDoubleClick()
{
	OSInt i = this->dblClickHdlrs->GetCount();
	while (i-- > 0)
	{
		this->dblClickHdlrs->GetItem(i)(this->dblClickObjs->GetItem(i));
	}
}

void UI::GUIListBox::EventRightClick(OSInt x, OSInt y)
{
	OSInt i = this->rightClickHdlrs->GetCount();
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

OSInt UI::GUIListBox::AddItem(const UTF8Char *itemText, void *itemObj)
{
	OSInt i = Text::StrUTF8_WCharCnt(itemText, -1);
	WChar *s = MemAlloc(WChar, i + 1);
	Text::StrUTF8_WChar(s, itemText, -1, 0);
	i = SendMessage((HWND)hwnd, LB_ADDSTRING, 0, (LPARAM)s);
	MemFree(s);
	if (i < 0)
		return i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, LB_SETITEMDATA, i, (LPARAM)itemObj);
	}
	return i;
}

OSInt UI::GUIListBox::AddItem(const WChar *itemText, void *itemObj)
{
	OSInt i = SendMessage((HWND)hwnd, LB_ADDSTRING, 0, (LPARAM)itemText);
	if (i < 0)
		return i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, LB_SETITEMDATA, i, (LPARAM)itemObj);
	}
	return i;
}

OSInt UI::GUIListBox::InsertItem(OSInt index, const UTF8Char *itemText, void *itemObj)
{
	const WChar *wptr = Text::StrToWCharNew(itemText);
	OSInt i = SendMessage((HWND)hwnd, LB_INSERTSTRING, index, (LPARAM)wptr);
	Text::StrDelNew(wptr);
	if (i < 0)
		return i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, LB_SETITEMDATA, i, (LPARAM)itemObj);
	}
	return i;
}

OSInt UI::GUIListBox::InsertItem(OSInt index, const WChar *itemText, void *itemObj)
{
	OSInt i = SendMessage((HWND)hwnd, LB_INSERTSTRING, index, (LPARAM)itemText);
	if (i < 0)
		return i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, LB_SETITEMDATA, i, (LPARAM)itemObj);
	}
	return i;
}

void *UI::GUIListBox::RemoveItem(OSInt index)
{
	void *obj = (void*)SendMessage((HWND)hwnd, LB_GETITEMDATA, index, 0);
	SendMessage((HWND)hwnd, LB_DELETESTRING, index, 0);
	return obj;
}

void *UI::GUIListBox::GetItem(OSInt index)
{
	return (void*)SendMessage((HWND)hwnd, LB_GETITEMDATA, index, 0);
}

void UI::GUIListBox::ClearItems()
{
	SendMessage((HWND)hwnd, LB_RESETCONTENT, 0, 0);
}

OSInt UI::GUIListBox::GetCount()
{
	return SendMessage((HWND)hwnd, LB_GETCOUNT, 0, 0);
}

void UI::GUIListBox::SetSelectedIndex(OSInt index)
{
	SendMessage((HWND)hwnd, LB_SETCURSEL, index, 0);
	this->EventSelectionChange();
}

OSInt UI::GUIListBox::GetSelectedIndex()
{
	return SendMessage((HWND)hwnd, LB_GETCURSEL, 0, 0);
}

Bool UI::GUIListBox::GetSelectedIndices(Data::ArrayList<Int32> *indices)
{
	if (this->mulSel)
	{
		OSInt ret;
		OSInt i = 0;
		OSInt j = this->GetCount();
		while (i < j)
		{
			ret = SendMessage((HWND)hwnd, LB_GETSEL, i, 0);
			if (ret < 0)
				return false;
			if (ret)
				indices->Add((Int32)i);
			i++;
		}
		return true;
	}
	else
	{
		OSInt i = SendMessage((HWND)hwnd, LB_GETCURSEL, 0, 0);
		if (i >= 0)
			indices->Add((Int32)i);
		return true;
	}
}

void *UI::GUIListBox::GetSelectedItem()
{
	OSInt currSel = GetSelectedIndex();
	if (currSel <= -1)
		return 0;
	return GetItem(currSel);
}

UTF8Char *UI::GUIListBox::GetSelectedItemText(UTF8Char *buff)
{
	OSInt currSel = GetSelectedIndex();
	if (currSel <= -1)
		return 0;
	return GetItemText(buff, currSel);
}

WChar *UI::GUIListBox::GetSelectedItemText(WChar *buff)
{
	OSInt currSel = GetSelectedIndex();
	if (currSel <= -1)
		return 0;
	return GetItemText(buff, currSel);
}

const UTF8Char *UI::GUIListBox::GetSelectedItemTextNew()
{
	OSInt currSel = GetSelectedIndex();
	if (currSel <= -1)
		return 0;
	return GetItemTextNew(currSel);
}

UTF8Char *UI::GUIListBox::GetItemText(UTF8Char *buff, OSInt index)
{
	const UTF8Char *u8ptr = this->GetItemTextNew(index);
	if (u8ptr == 0)
	{
		return 0;
	}
	buff = Text::StrConcat(buff, u8ptr);
	this->DelTextNew(u8ptr);
	return buff;
}

WChar *UI::GUIListBox::GetItemText(WChar *buff, OSInt index)
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

void UI::GUIListBox::SetItemText(OSInt index, const UTF8Char *text)
{
	void *item = GetItem(index);
	this->RemoveItem(index);
	this->InsertItem(index, text, item);
}

const UTF8Char *UI::GUIListBox::GetItemTextNew(OSInt index)
{
	OSInt strLen = SendMessageW((HWND)hwnd, LB_GETTEXTLEN, index, 0);
	if (strLen == LB_ERR)
		return 0;
	WChar *sbuff = MemAlloc(WChar, strLen + 1);
	strLen = SendMessageW((HWND)hwnd, LB_GETTEXT, index, (LPARAM)sbuff);
	if (strLen == LB_ERR)
	{
		MemFree(sbuff);
		return 0;
	}
	else
	{
		const UTF8Char *ret = Text::StrToUTF8New(sbuff);
		MemFree(sbuff);
		return ret;
	}
}

void UI::GUIListBox::DelTextNew(const UTF8Char *text)
{
	Text::StrDelNew(text);
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

OSInt UI::GUIListBox::OnNotify(Int32 code, void *lParam)
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
