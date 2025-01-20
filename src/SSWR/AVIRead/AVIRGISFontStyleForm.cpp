#include "Stdafx.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRGISFontEditForm.h"
#include "SSWR/AVIRead/AVIRGISFontStyleForm.h"
#include "UtilUI/ColorDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::AddStyleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISFontStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontStyleForm>();
	UOSInt newStyle = me->env->AddFontStyle(CSTR("Unnamed"), CSTR("Arial"), 12, false, 0xff000000, 0, 0xff000000);

	SSWR::AVIRead::AVIRGISFontEditForm frm(0, me->ui, me->core, me->env, me->eng, newStyle);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		me->fontStyle = newStyle;
		me->fsFonts->Redraw();
		me->changed = true;
	}
	else
	{
		me->env->RemoveFontStyle(newStyle);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::RemoveStyleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISFontStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontStyleForm>();
	me->env->RemoveFontStyle(me->fontStyle);
	me->fsFonts->UpdateFontStyles();
	me->fsFonts->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::EditStyleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISFontStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontStyleForm>();
	if (me->fontStyle < me->env->GetFontStyleCount())
	{
		SSWR::AVIRead::AVIRGISFontEditForm frm(0, me->ui, me->core, me->env, me->eng, me->fontStyle);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->fsFonts->Redraw();
			me->changed = true;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::FontsSelectedChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISFontStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontStyleForm>();
	UOSInt i = me->fsFonts->GetSelectedFontStyle();
	me->fontStyle = i;
}

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::FontsDoubleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISFontStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontStyleForm>();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::OKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISFontStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontStyleForm>();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::CancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISFontStyleForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISFontStyleForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void SSWR::AVIRead::AVIRGISFontStyleForm::UpdatePreview()
{
	this->fsFonts->UpdateFontStyles();
	this->fsFonts->Redraw();
}

SSWR::AVIRead::AVIRGISFontStyleForm::AVIRGISFontStyleForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, NN<Media::DrawEngine> eng, UOSInt fontStyle) : UI::GUIForm(parent, 480, 306, ui)
{
	this->core = core;
	this->env = env;
	this->eng = eng;
	this->fontStyle = fontStyle;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->colorSess->AddHandler(*this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->SetText(CSTR("Font Style"));
	this->SetFont(0, 0, 8.25, false);

	this->pnlBtn = ui->NewPanel(*this);
	this->pnlBtn->SetRect(0, 0, 100, 48, false);
	this->pnlBtn->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnOK = ui->NewButton(this->pnlBtn, CSTR("OK"));
	this->btnOK->SetRect(130, 12, 100, 23, false);
	this->btnOK->HandleButtonClick(OKClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->btnCancel = ui->NewButton(this->pnlBtn, CSTR("Cancel"));
	this->btnCancel->SetRect(250, 12, 100, 23, false);
	this->btnCancel->HandleButtonClick(CancelClicked, this);
	this->SetCancelButton(this->btnCancel);

	this->grpStyle = ui->NewGroupBox(*this, CSTR("Style"));
	this->grpStyle->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlStyle = ui->NewPanel(this->grpStyle);
	this->pnlStyle->SetRect(0, 0, 100, 32, false);
	this->pnlStyle->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnAddStyle = ui->NewButton(this->pnlStyle, CSTR("New Style"));
	this->btnAddStyle->SetRect(8, 8, 75, 23, false);
	this->btnAddStyle->HandleButtonClick(AddStyleClicked, this);
	this->btnRemoveStyle = ui->NewButton(this->pnlStyle, CSTR("Remove Style"));
	this->btnRemoveStyle->SetRect(88, 8, 88, 23, false);
	this->btnRemoveStyle->HandleButtonClick(RemoveStyleClicked, this);
	this->btnEditStyle = ui->NewButton(this->pnlStyle, CSTR("Edit Style"));
	this->btnEditStyle->SetRect(181, 8, 88, 23, false);
	this->btnEditStyle->HandleButtonClick(EditStyleClicked, this);

	NEW_CLASSNN(this->fsFonts, SSWR::AVIRead::AVIRFontSelector(ui, this->grpStyle, this->core, this->env, fontStyle, this->colorSess));
	this->fsFonts->SetDockType(UI::GUIControl::DOCK_FILL);
	this->fsFonts->HandleSelChg(FontsSelectedChg, this);

	this->changed = false;
	this->UpdatePreview();
}

SSWR::AVIRead::AVIRGISFontStyleForm::~AVIRGISFontStyleForm()
{
	this->colorSess->RemoveHandler(*this);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISFontStyleForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGISFontStyleForm::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void SSWR::AVIRead::AVIRGISFontStyleForm::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
}

UOSInt SSWR::AVIRead::AVIRGISFontStyleForm::GetFontStyle()
{
	return this->fontStyle;
}

Bool SSWR::AVIRead::AVIRGISFontStyleForm::IsChanged()
{
	return this->changed;
}
