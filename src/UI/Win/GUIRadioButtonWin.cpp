#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayList.h"
#include "UI/GUIRadioButton.h"
#include "UI/Win/WinCore.h"

#include <windows.h>

void UI::GUIRadioButton::ChangeSelected(Bool selVal)
{
	if (selVal)
	{
		SendMessage((HWND)this->hwnd, BM_SETCHECK, BST_CHECKED, 0);
		this->selected = true;
	}
	else
	{
		SendMessage((HWND)this->hwnd, BM_SETCHECK, BST_UNCHECKED, 0);
		this->selected = false;
	}
	UOSInt i = this->selectedChangeHdlrs.GetCount();
	while (i-- > 0)
	{
		this->selectedChangeHdlrs.GetItem(i)(this->selectedChangeObjs.GetItem(i), this->selected);
	}
}

UI::GUIRadioButton::GUIRadioButton(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText, Bool selected) : UI::GUIControl(ui, parent)
{
	this->selected = false;

	UInt32 style = WS_TABSTOP | WS_CHILD | BS_RADIOBUTTON;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, L"BUTTON", initText.v, style, 0, 0, 0, 200, 28);
	if (selected)
		Select();
}

UI::GUIRadioButton::~GUIRadioButton()
{
}

Text::CStringNN UI::GUIRadioButton::GetObjectClass() const
{
	return CSTR("RadioButton");
}

OSInt UI::GUIRadioButton::OnNotify(UInt32 code, void *lParam)
{
	switch (code)
	{
	case BN_CLICKED:
		Select();
		break;
	}
	return 0;
}

Bool UI::GUIRadioButton::IsSelected()
{
	return SendMessage((HWND)this->hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void UI::GUIRadioButton::Select()
{
	if (this->selected)
		return;

	NotNullPtr<UI::GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		Data::ArrayIterator<NotNullPtr<GUIControl>> it = nnparent->ChildIterator();
		while (it.HasNext())
		{
			NotNullPtr<UI::GUIControl> ctrl = it.Next();
			if (ctrl.Ptr() != this)
			{
				if (ctrl->GetObjectClass().Equals(UTF8STRC("RadioButton")))
				{
					NotNullPtr<UI::GUIRadioButton>::ConvertFrom(ctrl)->ChangeSelected(false);
				}
			}
		}
	}
	this->ChangeSelected(true);
}

void UI::GUIRadioButton::HandleSelectedChange(SelectedChangeHandler hdlr, void *userObj)
{
	this->selectedChangeHdlrs.Add(hdlr);
	this->selectedChangeObjs.Add(userObj);
}