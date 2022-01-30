#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayList.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIRadioButton.h"

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
	UOSInt i = this->selectedChangeHdlrs->GetCount();
	while (i-- > 0)
	{
		this->selectedChangeHdlrs->GetItem(i)(this->selectedChangeObjs->GetItem(i), this->selected);
	}
}

UI::GUIRadioButton::GUIRadioButton(UI::GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *initText, Bool selected) : UI::GUIControl(ui, parent)
{
	this->selected = false;
	NEW_CLASS(this->selectedChangeHdlrs, Data::ArrayList<SelectedChangeHandler>());
	NEW_CLASS(this->selectedChangeObjs, Data::ArrayList<void *>());

	UInt32 style = WS_TABSTOP | WS_CHILD | BS_RADIOBUTTON;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)ui)->GetHInst(), parent, L"BUTTON", initText, style, 0, 0, 0, 200, 28);
	if (selected)
		Select();
}

UI::GUIRadioButton::~GUIRadioButton()
{
	DEL_CLASS(this->selectedChangeHdlrs);
	DEL_CLASS(this->selectedChangeObjs);
}

Text::CString UI::GUIRadioButton::GetObjectClass()
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

	UOSInt i = 0;
	UOSInt j = this->parent->GetChildCount();
	while (i < j)
	{
		UI::GUIControl *ctrl = this->parent->GetChild(i);
		if (ctrl != this)
		{
			if (ctrl->GetObjectClass().Equals(UTF8STRC("RadioButton")))
			{
				((UI::GUIRadioButton*)ctrl)->ChangeSelected(false);
			}
		}
		i++;
	}
	this->ChangeSelected(true);
}

void UI::GUIRadioButton::HandleSelectedChange(SelectedChangeHandler hdlr, void *userObj)
{
	this->selectedChangeHdlrs->Add(hdlr);
	this->selectedChangeObjs->Add(userObj);
}
