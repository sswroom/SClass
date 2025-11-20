#include "Stdafx.h"	
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "Math/Math_C.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/Win/WinComboBox.h"
#include "UI/Win/WinCore.h"

#include <windows.h>
#include <commctrl.h>

UI::Win::WinComboBox::WinComboBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool allowTyping) : UI::GUIComboBox(ui, parent)
{
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
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, L"COMBOBOX", (const UTF8Char*)"", style, 0, 0, 0, 200, 28);
	if (allowTyping)
	{
		SendMessage((HWND)this->hwnd.OrNull(), CB_LIMITTEXT, 0, 0);
	}
}

UI::Win::WinComboBox::~WinComboBox()
{
}

void UI::Win::WinComboBox::SetText(Text::CStringNN text)
{
	if (this->allowEdit)
	{
		UnsafeArray<const WChar> wptr = Text::StrToWCharNew(text.v);
		SetWindowTextW((HWND)hwnd.OrNull(), wptr.Ptr());
		Text::StrDelNew(wptr);
	}
	else
	{
		UTF8Char sbuff[256];
		UnsafeArray<UTF8Char> sptr;
		UOSInt i = 0;
		UOSInt j = this->GetCount();
		while (i < j)
		{
			sbuff[0] = 0;
			if (this->GetItemText(sbuff, i).SetTo(sptr) && text.Equals(sbuff, (UOSInt)(sptr - sbuff)))
			{
				this->SetSelectedIndex(i);
				break;
			}
			i++;
		}
	}
}

UnsafeArrayOpt<UTF8Char> UI::Win::WinComboBox::GetText(UnsafeArray<UTF8Char> buff)
{
	WChar wbuff[256];
	wbuff[0] = 0;
	GetWindowTextW((HWND)hwnd.OrNull(), wbuff, 256);
	return Text::StrWChar_UTF8(buff, wbuff);
}

Bool UI::Win::WinComboBox::GetText(NN<Text::StringBuilderUTF8> sb)
{
	UOSInt leng = (UOSInt)GetWindowTextLengthW((HWND)hwnd.OrNull());
	WChar *wptr = MemAlloc(WChar, leng + 1);
	GetWindowTextW((HWND)hwnd.OrNull(), wptr, (int)leng + 1);
	sb->AppendW(wptr);
	MemFree(wptr);
	return true;
}

void UI::Win::WinComboBox::BeginUpdate()
{

}

void UI::Win::WinComboBox::EndUpdate()
{
	
}

UOSInt UI::Win::WinComboBox::AddItem(NN<Text::String> itemText, AnyType itemObj)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(itemText->v);
	OSInt i = SendMessage((HWND)hwnd.OrNull(), CB_ADDSTRING, 0, (LPARAM)wptr.Ptr());
	Text::StrDelNew(wptr);
	if (i < 0)
		return (UOSInt)i;
	if (itemObj.NotNull())
	{
		SendMessage((HWND)hwnd.OrNull(), CB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj.p);
	}
	this->itemTexts.Add(itemText->Clone());
	return (UOSInt)i;
}

UOSInt UI::Win::WinComboBox::AddItem(Text::CStringNN itemText, AnyType itemObj)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(itemText.v);
	OSInt i = SendMessage((HWND)hwnd.OrNull(), CB_ADDSTRING, 0, (LPARAM)wptr.Ptr());
	Text::StrDelNew(wptr);
	if (i < 0)
		return (UOSInt)i;
	if (itemObj.NotNull())
	{
		SendMessage((HWND)hwnd.OrNull(), CB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj.p);
	}
	this->itemTexts.Add(Text::String::New(itemText));
	return (UOSInt)i;
}

UOSInt UI::Win::WinComboBox::InsertItem(UOSInt index, NN<Text::String> itemText, AnyType itemObj)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(itemText->v);
	OSInt i = SendMessage((HWND)hwnd.OrNull(), CB_INSERTSTRING, index, (LPARAM)wptr.Ptr());
	Text::StrDelNew(wptr);
	if (i < 0)
		return (UOSInt)i;
	if (itemObj.NotNull())
	{
		SendMessage((HWND)hwnd.OrNull(), CB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj.p);
	}
	this->itemTexts.Insert(index, itemText->Clone());
	return (UOSInt)i;
}

UOSInt UI::Win::WinComboBox::InsertItem(UOSInt index, Text::CStringNN itemText, AnyType itemObj)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(itemText.v);
	OSInt i = SendMessage((HWND)hwnd.OrNull(), CB_INSERTSTRING, index, (LPARAM)wptr.Ptr());
	Text::StrDelNew(wptr);
	if (i < 0)
		return (UOSInt)i;
	if (itemObj.NotNull())
	{
		SendMessage((HWND)hwnd.OrNull(), CB_SETITEMDATA, (WPARAM)i, (LPARAM)itemObj.p);
	}
	this->itemTexts.Insert(index, Text::String::New(itemText));
	return (UOSInt)i;
}

AnyType UI::Win::WinComboBox::RemoveItem(UOSInt index)
{
	AnyType obj = (void*)SendMessage((HWND)hwnd.OrNull(), CB_GETITEMDATA, index, 0);
	SendMessage((HWND)hwnd.OrNull(), CB_DELETESTRING, index, 0);
	OPTSTR_DEL(this->itemTexts.RemoveAt(index));
	return obj;
}

void UI::Win::WinComboBox::ClearItems()
{
	SendMessage((HWND)hwnd.OrNull(), CB_RESETCONTENT, 0, 0);
	UOSInt i = this->itemTexts.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->itemTexts.RemoveAt(i));
	}
}

UOSInt UI::Win::WinComboBox::GetCount()
{
	return (UOSInt)SendMessage((HWND)hwnd.OrNull(), CB_GETCOUNT, 0, 0);
}

void UI::Win::WinComboBox::SetSelectedIndex(UOSInt index)
{
	if (index != (UOSInt)SendMessage((HWND)hwnd.OrNull(), CB_GETCURSEL, 0, 0))
	{
		SendMessage((HWND)hwnd.OrNull(), CB_SETCURSEL, index, 0);
		EventSelectionChange();
	}
}

UOSInt UI::Win::WinComboBox::GetSelectedIndex()
{
	OSInt si = SendMessage((HWND)hwnd.OrNull(), CB_GETCURSEL, 0, 0);
	if (si >= 0)
		return (UOSInt)si;
	if (this->autoComplete)
	{
		Text::StringBuilderUTF8 sb;
		this->GetText(sb);
		UOSInt i = this->itemTexts.GetCount();
		while (i-- > 0)
		{
			if (Text::String::OrEmpty(this->itemTexts.GetItem(i))->Equals(sb.ToString(), sb.GetLength()))
				return i;
		}
	}
	return INVALID_INDEX;
}

AnyType UI::Win::WinComboBox::GetSelectedItem()
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItem(currSel);
}

AnyType UI::Win::WinComboBox::GetItem(UOSInt index)
{
	return (void*)SendMessage((HWND)hwnd.OrNull(), CB_GETITEMDATA, index, 0);
}

Math::Size2DDbl UI::Win::WinComboBox::GetSize()
{
//	OSInt itemHeight = GetSelectionHeight();
	RECT rect;
	GetWindowRect((HWND)hwnd.OrNull(), &rect);
	return Math::Size2DDbl(rect.right - rect.left, rect.bottom - rect.top) * this->ddpi / this->hdpi;
}

void UI::Win::WinComboBox::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	UOSInt itemHeight = GetSelectionHeight();
	Math::Coord2DDbl ofst = Math::Coord2DDbl(0, 0);
	NN<GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		ofst = nnparent->GetClientOfst();
	}
	this->lxPos = left;
	this->lyPos = top;
	MoveWindow((HWND)hwnd.OrNull(), (int)((left + ofst.x) * this->hdpi / this->ddpi), (int)((top + ofst.y) * this->hdpi / this->ddpi), (int)((right - left) * this->hdpi / this->ddpi), (int)((bottom - top + UOSInt2Double(itemHeight * this->minVisible)) * this->hdpi / this->ddpi), updateScn?TRUE:FALSE);
	this->lxPos2 = left + right - left;
	this->lyPos2 = top + bottom - top;
	this->OnSizeChanged(updateScn);
}

OSInt UI::Win::WinComboBox::OnNotify(UInt32 code, void *lParam)
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

void UI::Win::WinComboBox::UpdatePos(Bool redraw)
{
	UOSInt itemHeight = GetSelectionHeight();
	Math::Coord2DDbl ofst = Math::Coord2DDbl(0, 0);
	NN<GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		ofst = nnparent->GetClientOfst();
	}
	MoveWindow((HWND)hwnd.OrNull(), (int)((this->lxPos + ofst.x) * this->hdpi / this->ddpi), (int)((this->lyPos + ofst.y) * this->hdpi / this->ddpi), (int)((this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi), (int)((this->lyPos2 - this->lyPos + UOSInt2Double(itemHeight * this->minVisible)) * this->hdpi / this->ddpi), redraw?TRUE:FALSE);
}

void UI::Win::WinComboBox::SetTextSelection(UOSInt startPos, UOSInt endPos)
{
	SendMessage((HWND)this->hwnd.OrNull(), CB_SETEDITSEL, 0, (LPARAM)((startPos & 0xffff) | (endPos << 16)));
}

UOSInt UI::Win::WinComboBox::GetListMinVisible()
{
	//return SendMessage((HWND)this->hwnd, CB_GETMINVISIBLE, 0, 0);
	return this->minVisible;
}

Bool UI::Win::WinComboBox::SetListMinVisible(UOSInt itemCount)
{
	this->minVisible = itemCount;
	this->UpdatePos(false);
	return true;
	//return SendMessage((HWND)this->hwnd, CB_SETMINVISIBLE, itemCount, 0) == TRUE;
}

UOSInt UI::Win::WinComboBox::GetSelectionHeight()
{
	return (UOSInt)SendMessage((HWND)this->hwnd.OrNull(), CB_GETITEMHEIGHT, (WPARAM)-1, 0);
}

Bool UI::Win::WinComboBox::SetListItemHeight(UOSInt itemHeight)
{
	return SendMessage((HWND)this->hwnd.OrNull(), CB_SETITEMHEIGHT, 0, (LPARAM)itemHeight) == TRUE;
}
