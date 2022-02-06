#include "Stdafx.h"	
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Math/Math.h"
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
	NEW_CLASS(this->itemTexts, Data::ArrayList<Text::String *>());
	this->autoComplete = false;
	this->minVisible = 5;
	this->allowEdit = allowTyping;
	UInt32 style = WS_TABSTOP | WS_CHILD | WS_VSCROLL;
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
	UOSInt i = this->selChgHdlrs->GetCount();
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
			UOSInt i = 0;
			UOSInt j = this->itemTexts->GetCount();
			while (i < j)
			{
				if (this->itemTexts->GetItem(i)->StartsWith(sb.ToString(), sb.GetLength()))
				{
					this->SetSelectedIndex(i);
					this->SetTextSelection(sb.GetLength(), this->itemTexts->GetItem(i)->leng);
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
		UOSInt i = 0;
		UOSInt j = this->GetCount();
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
	return Text::StrWChar_UTF8(buff, sbuff);
}

Bool UI::GUIComboBox::GetText(Text::StringBuilderUTF8 *sb)
{
	UOSInt leng = (UOSInt)GetWindowTextLengthW((HWND)hwnd);
	WChar *wptr = MemAlloc(WChar, leng + 1);
	GetWindowTextW((HWND)hwnd, wptr, (int)leng + 1);
	sb->AppendW(wptr);
	MemFree(wptr);
	return true;
}

UOSInt UI::GUIComboBox::AddItem(Text::String *itemText, void *itemObj)
{
	const WChar *wptr = Text::StrToWCharNew(itemText->v);
	OSInt i = SendMessage((HWND)hwnd, CB_ADDSTRING, 0, (LPARAM)wptr);
	Text::StrDelNew(wptr);
	if (i < 0)
		return (UOSInt)i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, CB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj);
	}
	this->itemTexts->Add(itemText->Clone());
	return (UOSInt)i;
}

UOSInt UI::GUIComboBox::AddItem(Text::CString itemText, void *itemObj)
{
	const WChar *wptr = Text::StrToWCharNew(itemText.v);
	OSInt i = SendMessage((HWND)hwnd, CB_ADDSTRING, 0, (LPARAM)wptr);
	Text::StrDelNew(wptr);
	if (i < 0)
		return (UOSInt)i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, CB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj);
	}
	this->itemTexts->Add(Text::String::New(itemText));
	return (UOSInt)i;
}

UOSInt UI::GUIComboBox::InsertItem(UOSInt index, Text::String *itemText, void *itemObj)
{
	const WChar *wptr = Text::StrToWCharNew(itemText->v);
	OSInt i = SendMessage((HWND)hwnd, CB_INSERTSTRING, index, (LPARAM)wptr);
	Text::StrDelNew(wptr);
	if (i < 0)
		return (UOSInt)i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, CB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj);
	}
	this->itemTexts->Insert(index, itemText->Clone());
	return (UOSInt)i;
}

UOSInt UI::GUIComboBox::InsertItem(UOSInt index, Text::CString itemText, void *itemObj)
{
	const WChar *wptr = Text::StrToWCharNew(itemText.v);
	OSInt i = SendMessage((HWND)hwnd, CB_INSERTSTRING, index, (LPARAM)wptr);
	Text::StrDelNew(wptr);
	if (i < 0)
		return (UOSInt)i;
	if (itemObj)
	{
		SendMessage((HWND)hwnd, CB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj);
	}
	this->itemTexts->Insert(index, Text::String::New(itemText));
	return (UOSInt)i;
}

void *UI::GUIComboBox::RemoveItem(UOSInt index)
{
	void *obj = (void*)SendMessage((HWND)hwnd, CB_GETITEMDATA, index, 0);
	SendMessage((HWND)hwnd, CB_DELETESTRING, index, 0);
	Text::String *s = this->itemTexts->RemoveAt(index);
	if (s)
		s->Release();
	return obj;
}

void UI::GUIComboBox::ClearItems()
{
	SendMessage((HWND)hwnd, CB_RESETCONTENT, 0, 0);
	UOSInt i = this->itemTexts->GetCount();
	while (i-- > 0)
	{
		this->itemTexts->RemoveAt(i)->Release();
	}
}

UOSInt UI::GUIComboBox::GetCount()
{
	return (UOSInt)SendMessage((HWND)hwnd, CB_GETCOUNT, 0, 0);
}

void UI::GUIComboBox::SetSelectedIndex(UOSInt index)
{
	if (index != (UOSInt)SendMessage((HWND)hwnd, CB_GETCURSEL, 0, 0))
	{
		SendMessage((HWND)hwnd, CB_SETCURSEL, index, 0);
		EventSelectionChange();
	}
}

UOSInt UI::GUIComboBox::GetSelectedIndex()
{
	OSInt si = SendMessage((HWND)hwnd, CB_GETCURSEL, 0, 0);
	if (si >= 0)
		return (UOSInt)si;
	if (this->autoComplete)
	{
		Text::StringBuilderUTF8 sb;
		this->GetText(&sb);
		UOSInt i = this->itemTexts->GetCount();
		while (i-- > 0)
		{
			if (this->itemTexts->GetItem(i)->Equals(sb.ToString(), sb.GetLength()))
				return i;
		}
	}
	return INVALID_INDEX;
}

UTF8Char *UI::GUIComboBox::GetSelectedItemText(UTF8Char *buff)
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItemText(buff, currSel);
}

void *UI::GUIComboBox::GetSelectedItem()
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItem(currSel);
}

UTF8Char *UI::GUIComboBox::GetItemText(UTF8Char *buff, UOSInt index)
{
	WChar sbuff[512];
	OSInt strLen = SendMessageW((HWND)hwnd, CB_GETLBTEXT, index, (LPARAM)sbuff);
	if (strLen == CB_ERR)
	{
		return 0;
	}
	else
	{
		return Text::StrWChar_UTF8(buff, sbuff);
	}
}

void *UI::GUIComboBox::GetItem(UOSInt index)
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
	UOSInt itemHeight = GetSelectionHeight();
	Double xOfst = 0;
	Double yOfst = 0;
	if (this->parent)
	{
		this->parent->GetClientOfst(&xOfst, &yOfst);
	}
	this->lxPos = left;
	this->lyPos = top;
	MoveWindow((HWND)hwnd, (int)((left + xOfst) * this->hdpi / this->ddpi), (int)((top + yOfst) * this->hdpi / this->ddpi), (int)((right - left) * this->hdpi / this->ddpi), (int)((bottom - top + UOSInt2Double(itemHeight * this->minVisible)) * this->hdpi / this->ddpi), updateScn?TRUE:FALSE);
	this->lxPos2 = left + right - left;
	this->lyPos2 = top + bottom - top;
	this->OnSizeChanged(updateScn);
}

Text::CString UI::GUIComboBox::GetObjectClass()
{
	return CSTR("ComboBox");
}

OSInt UI::GUIComboBox::OnNotify(UInt32 code, void *lParam)
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
	UOSInt itemHeight = GetSelectionHeight();
	Double xOfst = 0;
	Double yOfst = 0;
	if (this->parent)
	{
		this->parent->GetClientOfst(&xOfst, &yOfst);
	}
	MoveWindow((HWND)hwnd, (int)((this->lxPos + xOfst) * this->hdpi / this->ddpi), (int)((this->lyPos + yOfst) * this->hdpi / this->ddpi), (int)((this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi), (int)((this->lyPos2 - this->lyPos + UOSInt2Double(itemHeight * this->minVisible)) * this->hdpi / this->ddpi), redraw?TRUE:FALSE);
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

void UI::GUIComboBox::SetTextSelection(UOSInt startPos, UOSInt endPos)
{
	SendMessage((HWND)this->hwnd, CB_SETEDITSEL, 0, (LPARAM)((startPos & 0xffff) | (endPos << 16)));
}

UOSInt UI::GUIComboBox::GetListMinVisible()
{
	//return SendMessage((HWND)this->hwnd, CB_GETMINVISIBLE, 0, 0);
	return this->minVisible;
}

Bool UI::GUIComboBox::SetListMinVisible(UOSInt itemCount)
{
	this->minVisible = itemCount;
	this->UpdatePos(false);
	return true;
	//return SendMessage((HWND)this->hwnd, CB_SETMINVISIBLE, itemCount, 0) == TRUE;
}

UOSInt UI::GUIComboBox::GetSelectionHeight()
{
	return (UOSInt)SendMessage((HWND)this->hwnd, CB_GETITEMHEIGHT, (WPARAM)-1, 0);
}

Bool UI::GUIComboBox::SetListItemHeight(UOSInt itemHeight)
{
	return SendMessage((HWND)this->hwnd, CB_SETITEMHEIGHT, 0, (LPARAM)itemHeight) == TRUE;
}
