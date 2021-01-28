#include "Stdafx.h"	
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIComboBox.h"
#include "UI/GUICoreWin.h"

#include <windows.h>
#include <commctrl.h>

UI::GUIComboBox::GUIComboBox(GUICore *ui, UI::GUIClientControl *parent, Bool allowTyping) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->selChgHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->selChgObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->itemTexts, Data::ArrayList<const UTF8Char *>());
	this->autoComplete = false;
	this->minVisible = 5;
	this->allowEdit = allowTyping;
	Int32 style = WS_TABSTOP | WS_CHILD | WS_VSCROLL;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	if (allowTyping)
	{
		style = style | CBS_DROPDOWN;
	}
	else
	{
		style = style | CBS_DROPDOWNLIST;
	}
	this->InitControl(((GUICoreWin*)ui)->GetHInst(), parent, L"COMBOBOX", (const UTF8Char*)"", style, 0, 0, 0, 200, 28);
	if (allowTyping)
	{
		SendMessage((HWND)this->hwnd, CB_LIMITTEXT, 0, 0);
	}
}

UI::GUIComboBox::~GUIComboBox()
{
	this->ClearItems();
	DEL_CLASS(this->itemTexts);
	DEL_CLASS(this->selChgHdlrs);
	DEL_CLASS(this->selChgObjs);
}

void UI::GUIComboBox::EventSelectionChange()
{
	OSInt i = this->selChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs->GetItem(i)(this->selChgObjs->GetItem(i));
	}
}

void UI::GUIComboBox::EventTextChanged()
{
	if (this->autoComplete)
	{
		Text::StringBuilderUTF8 sb;
		this->GetText(&sb);
		if (sb.GetLength() > this->lastTextLeng)
		{
			OSInt i = 0;
			OSInt j = this->itemTexts->GetCount();
			while (i < j)
			{
				if (Text::StrStartsWith(this->itemTexts->GetItem(i), sb.ToString()))
				{
					this->SetSelectedIndex(i);
					this->SetTextSelection(sb.GetLength(), Text::StrCharCnt(this->itemTexts->GetItem(i)));
				}
				i++;
			}
		}
		this->lastTextLeng = sb.GetLength();
	}
}

void UI::GUIComboBox::SetText(const UTF8Char *text)
{
	if (this->allowEdit)
	{
		const WChar *wptr = Text::StrToWCharNew(text);
		SetWindowTextW((HWND)hwnd, wptr);
		Text::StrDelNew(wptr);
	}
	else
	{
		UTF8Char sbuff[256];
		OSInt i = 0;
		OSInt j = this->GetCount();
		while (i < j)
		{
			sbuff[0] = 0;
			this->GetItemText(sbuff, i);
			if (Text::StrEquals(text, sbuff))
			{
				this->SetSelectedIndex(i);
				break;
			}
			i++;
		}
	}
}

UTF8Char *UI::GUIComboBox::GetText(UTF8Char *buff)
{
	WChar sbuff[256];
	sbuff[0] = 0;
	GetWindowTextW((HWND)hwnd, sbuff, 256);
	return Text::StrWChar_UTF8(buff, sbuff, -1);
}

Bool UI::GUIComboBox::GetText(Text::StringBuilderUTF *sb)
{
	OSInt leng = GetWindowTextLengthW((HWND)hwnd);
	WChar *wptr = MemAlloc(WChar, leng + 1);
	GetWindowTextW((HWND)hwnd, wptr, (int)leng + 1);
	const UTF8Char *csptr = Text::StrToUTF8New(wptr);
	sb->Append(csptr);
	Text::StrDelNew(csptr);
	MemFree(wptr);
	return true;
}

OSInt UI::GUIComboBox::AddItem(const UTF8Char *itemText, void *itemObj)
{
	const WChar *wptr = Text::StrToWCharNew(itemText);
	OSInt i = SendMessage((HWND)hwnd, CB_ADDSTRING, 0, (LPARAM)wptr);
	Text::StrDelNew(wptr);
	if (i < 0)
		return i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, CB_SETITEMDATA, i, (LPARAM)itemObj);
	}
	this->itemTexts->Add(Text::StrCopyNew(itemText));
	return i;
}

OSInt UI::GUIComboBox::InsertItem(OSInt index, const UTF8Char *itemText, void *itemObj)
{
	const WChar *wptr = Text::StrToWCharNew(itemText);
	OSInt i = SendMessage((HWND)hwnd, CB_INSERTSTRING, index, (LPARAM)wptr);
	Text::StrDelNew(wptr);
	if (i < 0)
		return i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, CB_SETITEMDATA, i, (LPARAM)itemObj);
	}
	this->itemTexts->Insert(index, Text::StrCopyNew(itemText));
	return i;
}

void *UI::GUIComboBox::RemoveItem(OSInt index)
{
	void *obj = (void*)SendMessage((HWND)hwnd, CB_GETITEMDATA, index, 0);
	SendMessage((HWND)hwnd, CB_DELETESTRING, index, 0);
	const UTF8Char *s = this->itemTexts->RemoveAt(index);
	if (s)
		Text::StrDelNew(s);
	return obj;
}

void UI::GUIComboBox::ClearItems()
{
	SendMessage((HWND)hwnd, CB_RESETCONTENT, 0, 0);
	OSInt i = this->itemTexts->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->itemTexts->RemoveAt(i));
	}
}

OSInt UI::GUIComboBox::GetCount()
{
	return SendMessage((HWND)hwnd, CB_GETCOUNT, 0, 0);
}

void UI::GUIComboBox::SetSelectedIndex(OSInt index)
{
	if (index != SendMessage((HWND)hwnd, CB_GETCURSEL, 0, 0))
	{
		SendMessage((HWND)hwnd, CB_SETCURSEL, index, 0);
		EventSelectionChange();
	}
}

OSInt UI::GUIComboBox::GetSelectedIndex()
{
	OSInt i = SendMessage((HWND)hwnd, CB_GETCURSEL, 0, 0);
	if (i >= 0)
		return i;
	if (this->autoComplete)
	{
		Text::StringBuilderUTF8 sb;
		this->GetText(&sb);
		i = this->itemTexts->GetCount();
		while (i-- > 0)
		{
			if (Text::StrEquals(this->itemTexts->GetItem(i), sb.ToString()))
				return i;
		}
	}
	return -1;
}

UTF8Char *UI::GUIComboBox::GetSelectedItemText(UTF8Char *buff)
{
	OSInt currSel = GetSelectedIndex();
	if (currSel <= -1)
		return 0;
	return GetItemText(buff, currSel);
}

void *UI::GUIComboBox::GetSelectedItem()
{
	OSInt currSel = GetSelectedIndex();
	if (currSel <= -1)
		return 0;
	return GetItem(currSel);
}

UTF8Char *UI::GUIComboBox::GetItemText(UTF8Char *buff, OSInt index)
{
	WChar sbuff[512];
	OSInt strLen = SendMessageW((HWND)hwnd, CB_GETLBTEXT, index, (LPARAM)sbuff);
	if (strLen == CB_ERR)
	{
		return 0;
	}
	else
	{
		return Text::StrWChar_UTF8(buff, sbuff, -1);
	}
}

void *UI::GUIComboBox::GetItem(OSInt index)
{
	return (void*)SendMessage((HWND)hwnd, CB_GETITEMDATA, index, 0);
}

void UI::GUIComboBox::GetSize(Double *width, Double *height)
{
//	OSInt itemHeight = GetSelectionHeight();
	RECT rect;
	GetWindowRect((HWND)hwnd, &rect);
	if (width)
		*width = (rect.right - rect.left) * this->ddpi / this->hdpi;
	if (height)
		*height = (rect.bottom - rect.top) * this->ddpi / this->hdpi;// - itemHeight * this->minVisible;
}

void UI::GUIComboBox::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	OSInt itemHeight = GetSelectionHeight();
	Double xOfst = 0;
	Double yOfst = 0;
	if (this->parent)
	{
		this->parent->GetClientOfst(&xOfst, &yOfst);
	}
	this->lxPos = left;
	this->lyPos = top;
	MoveWindow((HWND)hwnd, (int)((left + xOfst) * this->hdpi / this->ddpi), (int)((top + yOfst) * this->hdpi / this->ddpi), (int)((right - left) * this->hdpi / this->ddpi), (int)((bottom - top + (itemHeight * this->minVisible)) * this->hdpi / this->ddpi), updateScn?TRUE:FALSE);
	this->lxPos2 = left + right - left;
	this->lyPos2 = top + bottom - top;
	this->OnSizeChanged(updateScn);
}

const UTF8Char *UI::GUIComboBox::GetObjectClass()
{
	return (const UTF8Char*)"ComboBox";
}

OSInt UI::GUIComboBox::OnNotify(Int32 code, void *lParam)
{
	switch (code)
	{
	case CBN_SELCHANGE:
		this->EventSelectionChange();
		break;
	case CBN_EDITCHANGE:
		this->EventTextChanged();
		break;
	case CBN_SETFOCUS:
	case CBN_KILLFOCUS:
		return 0;
	default:
		break;
	}
	return 0;
}

void UI::GUIComboBox::HandleSelectionChange(UI::UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs->Add(hdlr);
	this->selChgObjs->Add(userObj);
}

void UI::GUIComboBox::UpdatePos(Bool redraw)
{
	OSInt itemHeight = GetSelectionHeight();
	Double xOfst = 0;
	Double yOfst = 0;
	if (this->parent)
	{
		this->parent->GetClientOfst(&xOfst, &yOfst);
	}
	MoveWindow((HWND)hwnd, (int)((this->lxPos + xOfst) * this->hdpi / this->ddpi), (int)((this->lyPos + yOfst) * this->hdpi / this->ddpi), (int)((this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi), (int)((this->lyPos2 - this->lyPos + (itemHeight * this->minVisible)) * this->hdpi / this->ddpi), redraw?TRUE:FALSE);
}

void UI::GUIComboBox::SetAutoComplete(Bool autoComplete)
{
	if (!this->autoComplete && autoComplete)
	{
		Text::StringBuilderUTF8 sb;
		this->GetText(&sb);
		this->lastTextLeng = sb.GetLength();
	}
	this->autoComplete = autoComplete;
}

void UI::GUIComboBox::SetTextSelection(OSInt startPos, OSInt endPos)
{
	SendMessage((HWND)this->hwnd, CB_SETEDITSEL, 0, (startPos & 0xffff) | (endPos << 16));
}

OSInt UI::GUIComboBox::GetListMinVisible()
{
	//return SendMessage((HWND)this->hwnd, CB_GETMINVISIBLE, 0, 0);
	return this->minVisible;
}

Bool UI::GUIComboBox::SetListMinVisible(OSInt itemCount)
{
	this->minVisible = itemCount;
	this->UpdatePos(false);
	return true;
	//return SendMessage((HWND)this->hwnd, CB_SETMINVISIBLE, itemCount, 0) == TRUE;
}

OSInt UI::GUIComboBox::GetSelectionHeight()
{
	return SendMessage((HWND)this->hwnd, CB_GETITEMHEIGHT, -1, 0);
}

Bool UI::GUIComboBox::SetListItemHeight(OSInt itemHeight)
{
	return SendMessage((HWND)this->hwnd, CB_SETITEMHEIGHT, 0, itemHeight) == TRUE;
}
