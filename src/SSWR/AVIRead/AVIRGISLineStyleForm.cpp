#include "Stdafx.h"
#include "Text/MyString.h"
#include "UtilUI/ColorDialog.h"
#include "UI/FontDialog.h"
#include "SSWR/AVIRead/AVIRGISLineStyleForm.h"
#include "SSWR/AVIRead/AVIRGISLineEditForm.h"

#define MNU_SET_DEFAULT 101

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::AddStyleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineStyleForm *me = (SSWR::AVIRead::AVIRGISLineStyleForm*)userObj;
	UOSInt newStyle = me->env->AddLineStyle();

	SSWR::AVIRead::AVIRGISLineEditForm frm(0, me->ui, me->core, me->env, me->eng, newStyle);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		me->lineStyle = newStyle;
		me->lineSelector->Redraw();
		me->changed = true;
	}
	else
	{
		me->env->RemoveLineStyle(newStyle);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::RemoveStyleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineStyleForm *me = (SSWR::AVIRead::AVIRGISLineStyleForm*)userObj;
	me->env->RemoveLineStyle(me->lineStyle);
	me->lineSelector->UpdateLineStyles();
	me->lineSelector->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::EditStyleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineStyleForm *me = (SSWR::AVIRead::AVIRGISLineStyleForm*)userObj;
	SSWR::AVIRead::AVIRGISLineEditForm frm(0, me->ui, me->core, me->env, me->eng, me->lineStyle);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		me->lineSelector->Redraw();
		me->changed = true;
	}
}

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::LineSelChanged(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineStyleForm *me = (SSWR::AVIRead::AVIRGISLineStyleForm*)userObj;
	UTF8Char sbuff[256];
	me->lineStyle = me->lineSelector->GetSelectedLineStyle();
	sbuff[0] = 0;
	me->env->GetLineStyleName(me->lineStyle, sbuff);
	me->changed = true;
}

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::LineDblClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineStyleForm *me = (SSWR::AVIRead::AVIRGISLineStyleForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::OKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineStyleForm *me = (SSWR::AVIRead::AVIRGISLineStyleForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::CancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineStyleForm *me = (SSWR::AVIRead::AVIRGISLineStyleForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRGISLineStyleForm::AVIRGISLineStyleForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Map::MapEnv *env, NotNullPtr<Media::DrawEngine> eng, UOSInt lineStyle) : UI::GUIForm(parent, 462, 334, ui)
{
	this->core = core;
	this->env = env;
	this->eng = eng;
	this->lineStyle = lineStyle;
	this->changed = false;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->SetText(CSTR("Line Style"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	NEW_CLASS(this->pnlButtons, UI::GUIPanel(ui, this));
	this->pnlButtons->SetRect(0, 0, 456, 32, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->grpStyle, UI::GUIGroupBox(ui, this, CSTR("Style")));
	this->grpStyle->SetRect(0, 0, 456, 160, false);
	this->grpStyle->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlStyle, UI::GUIPanel(ui, this->grpStyle));
	this->pnlStyle->SetRect(0, 0, 456, 32, false);
	this->pnlStyle->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lineSelector, SSWR::AVIRead::AVIRLineSelector(ui, this->grpStyle, this->core, this->env, this->lineStyle, this->colorSess));
	this->lineSelector->SetRect(0, 0, 456, 120, false);
	this->lineSelector->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lineSelector->HandleSelChg(LineSelChanged, this);
	this->lineSelector->HandleDblClk(LineDblClicked, this);
	NEW_CLASS(this->btnAddStyle, UI::GUIButton(ui, this->pnlStyle, CSTR("New Style")));
	this->btnAddStyle->SetRect(8, 8, 75, 23, false);
	this->btnAddStyle->HandleButtonClick(AddStyleClicked, this);
	NEW_CLASS(this->btnRemoveStyle, UI::GUIButton(ui, this->pnlStyle, CSTR("Remove Style")));
	this->btnRemoveStyle->SetRect(88, 8, 88, 23, false);
	this->btnRemoveStyle->HandleButtonClick(RemoveStyleClicked, this);
	NEW_CLASS(this->btnEditStyle, UI::GUIButton(ui, this->pnlStyle, CSTR("Edit Style")));
	this->btnEditStyle->SetRect(181, 8, 88, 23, false);
	this->btnEditStyle->HandleButtonClick(EditStyleClicked, this);

	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this->pnlButtons, CSTR("OK")));
	this->btnOK->SetRect(150, 8, 75, 23, false);
	this->btnOK->HandleButtonClick(OKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlButtons, CSTR("Cancel")));
	this->btnCancel->SetRect(231, 8, 75, 23, false);
	this->btnCancel->HandleButtonClick(CancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	NEW_CLASS(this->mnuLayer, UI::GUIPopupMenu());
	this->mnuLayer->AddItem(CSTR("&Set as default"), MNU_SET_DEFAULT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->lineSelector->SetPopupMenu(this->mnuLayer);
}

SSWR::AVIRead::AVIRGISLineStyleForm::~AVIRGISLineStyleForm()
{
	DEL_CLASS(this->mnuLayer);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISLineStyleForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_SET_DEFAULT:
		this->env->SetDefLineStyle((UOSInt)this->lineSelector->GetSelectedLineStyle());
		this->lineSelector->Redraw();
		break;
	}
}

void SSWR::AVIRead::AVIRGISLineStyleForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

UOSInt SSWR::AVIRead::AVIRGISLineStyleForm::GetLineStyle()
{
	return this->lineStyle;
}

Bool SSWR::AVIRead::AVIRGISLineStyleForm::IsChanged()
{
	return this->changed;
}
