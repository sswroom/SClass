#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/Win/WinCheckBox.h"
#include "UI/Win/WinCore.h"

#include <windows.h>

UI::Win::WinCheckBox::WinCheckBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool checked) : UI::GUICheckBox(ui, parent)
{
	this->checked = false;
	UInt32 style = WS_TABSTOP | WS_CHILD | BS_CHECKBOX;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, L"BUTTON", initText.v, style, 0, 0, 0, 200, 28);
	this->SetChecked(checked);
}

UI::Win::WinCheckBox::~WinCheckBox()
{
}

OSInt UI::Win::WinCheckBox::OnNotify(UInt32 code, void *lParam)
{
	switch (code)
	{
	case BN_CLICKED:
		SetChecked(!IsChecked());
		break;
	}
	return 0;
}

Bool UI::Win::WinCheckBox::IsChecked()
{
	return this->checked;
}

void UI::Win::WinCheckBox::SetChecked(Bool checked)
{
	if (this->checked != checked)
	{
		this->checked = checked;
		SendMessage((HWND)this->hwnd.OrNull(), BM_SETCHECK, checked?BST_CHECKED:BST_UNCHECKED, 0);
		this->EventCheckedChange(checked);
	}
}
