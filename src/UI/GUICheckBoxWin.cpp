#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "UI/GUICheckBox.h"
#include "UI/GUICoreWin.h"

#include <windows.h>

UI::GUICheckBox::GUICheckBox(GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *initText, Bool checked) : UI::GUIControl(ui, parent)
{
	this->checked = false;
	NEW_CLASS(this->checkedChangeHdlrs, Data::ArrayList<CheckedChangeHandler>());
	NEW_CLASS(this->checkedChangeObjs, Data::ArrayList<void *>());

	Int32 style = WS_TABSTOP | WS_CHILD | BS_CHECKBOX;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)ui)->GetHInst(), parent, L"BUTTON", initText, style, 0, 0, 0, 200, 28);
	SetChecked(checked);
}

UI::GUICheckBox::~GUICheckBox()
{
	DEL_CLASS(this->checkedChangeHdlrs);
	DEL_CLASS(this->checkedChangeObjs);
}

const UTF8Char *UI::GUICheckBox::GetObjectClass()
{
	return (const UTF8Char*)"CheckBox";
}

OSInt UI::GUICheckBox::OnNotify(UInt32 code, void *lParam)
{
	switch (code)
	{
	case BN_CLICKED:
		SetChecked(!IsChecked());
		break;
	}
	return 0;
}

void UI::GUICheckBox::EventCheckedChange(Bool newState)
{
	UOSInt i = this->checkedChangeHdlrs->GetCount();
	while (i-- > 0)
	{
		this->checkedChangeHdlrs->GetItem(i)(this->checkedChangeObjs->GetItem(i), newState);
	}
}

Bool UI::GUICheckBox::IsChecked()
{
	return this->checked;//SendMessage((HWND)this->hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void UI::GUICheckBox::SetChecked(Bool checked)
{
	if (this->checked != checked)
	{
		this->checked = checked;
		SendMessage((HWND)this->hwnd, BM_SETCHECK, checked?BST_CHECKED:BST_UNCHECKED, 0);
		this->EventCheckedChange(checked);
	}
}

void UI::GUICheckBox::HandleCheckedChange(UI::GUICheckBox::CheckedChangeHandler hdlr, void *obj)
{
	this->checkedChangeHdlrs->Add(hdlr);
	this->checkedChangeObjs->Add(obj);
}
