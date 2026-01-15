#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGISLineStyleForm.h"
#include "SSWR/AVIRead/AVIRGISLineEditForm.h"
#include "Text/MyString.h"
#include "UtilUI/ColorDialog.h"

#define MNU_SET_DEFAULT 101

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::AddStyleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineStyleForm>();
	UOSInt newStyle = me->env->AddLineStyle();

	SSWR::AVIRead::AVIRGISLineEditForm frm(nullptr, me->ui, me->core, me->env, me->eng, newStyle);
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

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::RemoveStyleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineStyleForm>();
	me->env->RemoveLineStyle(me->lineStyle);
	me->lineSelector->UpdateLineStyles();
	me->lineSelector->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::EditStyleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineStyleForm>();
	SSWR::AVIRead::AVIRGISLineEditForm frm(nullptr, me->ui, me->core, me->env, me->eng, me->lineStyle);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		me->lineSelector->Redraw();
		me->changed = true;
	}
}

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::LineSelChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineStyleForm>();
	UTF8Char sbuff[256];
	me->lineStyle = me->lineSelector->GetSelectedLineStyle();
	sbuff[0] = 0;
	me->env->GetLineStyleName(me->lineStyle, sbuff);
	me->changed = true;
}

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::LineDblClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineStyleForm>();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::OKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineStyleForm>();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISLineStyleForm::CancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineStyleForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRGISLineStyleForm::AVIRGISLineStyleForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, NN<Media::DrawEngine> eng, UOSInt lineStyle) : UI::GUIForm(parent, 462, 334, ui)
{
	this->core = core;
	this->env = env;
	this->eng = eng;
	this->lineStyle = lineStyle;
	this->changed = false;
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->SetText(CSTR("Line Style"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->pnlButtons = ui->NewPanel(*this);
	this->pnlButtons->SetRect(0, 0, 456, 32, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->grpStyle = ui->NewGroupBox(*this, CSTR("Style"));
	this->grpStyle->SetRect(0, 0, 456, 160, false);
	this->grpStyle->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlStyle = ui->NewPanel(this->grpStyle);
	this->pnlStyle->SetRect(0, 0, 456, 32, false);
	this->pnlStyle->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASSNN(this->lineSelector, SSWR::AVIRead::AVIRLineSelector(ui, this->grpStyle, this->core, this->env, this->lineStyle, this->colorSess));
	this->lineSelector->SetRect(0, 0, 456, 120, false);
	this->lineSelector->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lineSelector->HandleSelChg(LineSelChanged, this);
	this->lineSelector->HandleDblClk(LineDblClicked, this);
	this->btnAddStyle = ui->NewButton(this->pnlStyle, CSTR("New Style"));
	this->btnAddStyle->SetRect(8, 8, 75, 23, false);
	this->btnAddStyle->HandleButtonClick(AddStyleClicked, this);
	this->btnRemoveStyle = ui->NewButton(this->pnlStyle, CSTR("Remove Style"));
	this->btnRemoveStyle->SetRect(88, 8, 88, 23, false);
	this->btnRemoveStyle->HandleButtonClick(RemoveStyleClicked, this);
	this->btnEditStyle = ui->NewButton(this->pnlStyle, CSTR("Edit Style"));
	this->btnEditStyle->SetRect(181, 8, 88, 23, false);
	this->btnEditStyle->HandleButtonClick(EditStyleClicked, this);

	this->btnOK = ui->NewButton(this->pnlButtons, CSTR("OK"));
	this->btnOK->SetRect(150, 8, 75, 23, false);
	this->btnOK->HandleButtonClick(OKClicked, this);
	this->btnCancel = ui->NewButton(this->pnlButtons, CSTR("Cancel"));
	this->btnCancel->SetRect(231, 8, 75, 23, false);
	this->btnCancel->HandleButtonClick(CancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	NEW_CLASSNN(this->mnuLayer, UI::GUIPopupMenu());
	this->mnuLayer->AddItem(CSTR("&Set as default"), MNU_SET_DEFAULT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->lineSelector->SetPopupMenu(this->mnuLayer);
}

SSWR::AVIRead::AVIRGISLineStyleForm::~AVIRGISLineStyleForm()
{
	this->mnuLayer.Delete();
	this->ClearChildren();
	this->eng->EndColorSess(this->colorSess);
	this->core->GetColorManager()->DeleteSess(this->colorSess);
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
