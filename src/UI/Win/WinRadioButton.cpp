#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayList.hpp"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinRadioButton.h"

#include <windows.h>

void UI::Win::WinRadioButton::ChangeSelected(Bool selVal)
{
	if (selVal)
	{
		SendMessage((HWND)this->hwnd.OrNull(), BM_SETCHECK, BST_CHECKED, 0);
		this->selected = true;
	}
	else
	{
		SendMessage((HWND)this->hwnd.OrNull(), BM_SETCHECK, BST_UNCHECKED, 0);
		this->selected = false;
	}
	this->EventSelectedChange(selVal);
}

UI::Win::WinRadioButton::WinRadioButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool selected) : UI::GUIRadioButton(ui, parent)
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

UI::Win::WinRadioButton::~WinRadioButton()
{
}

IntOS UI::Win::WinRadioButton::OnNotify(UInt32 code, void *lParam)
{
	switch (code)
	{
	case BN_CLICKED:
		Select();
		break;
	}
	return 0;
}

Bool UI::Win::WinRadioButton::IsSelected()
{
	return SendMessage((HWND)this->hwnd.OrNull(), BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void UI::Win::WinRadioButton::Select()
{
	if (this->selected)
		return;

	NN<UI::GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		Data::ArrayIterator<NN<GUIControl>> it = nnparent->ChildIterator();
		while (it.HasNext())
		{
			NN<UI::GUIControl> ctrl = it.Next();
			if (ctrl.Ptr() != this)
			{
				if (ctrl->GetObjectClass().Equals(UTF8STRC("RadioButton")))
				{
					NN<UI::Win::WinRadioButton>::ConvertFrom(ctrl)->ChangeSelected(false);
				}
			}
		}
	}
	this->ChangeSelected(true);
}
