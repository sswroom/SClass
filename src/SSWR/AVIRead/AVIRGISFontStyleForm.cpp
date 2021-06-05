#include "Stdafx.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRGISFontEditForm.h"
#include "SSWR/AVIRead/AVIRGISFontStyleForm.h"
#include "UI/FontDialog.h"
#include "UtilUI/ColorDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::AddStyleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontStyleForm *me = (SSWR::AVIRead::AVIRGISFontStyleForm*)userObj;
	UOSInt newStyle = me->env->AddFontStyle(0, (const UTF8Char*)"Arial", 12, false, 0xff000000, 0, 0xff000000);

	SSWR::AVIRead::AVIRGISFontEditForm *frm;
	NEW_CLASS(frm, SSWR::AVIRead::AVIRGISFontEditForm(0, me->ui, me->core, me->env, me->eng, newStyle));
	if (frm->ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		me->fontStyle = newStyle;
		me->fsFonts->Redraw();
		me->changed = true;
	}
	else
	{
		me->env->RemoveFontStyle(newStyle);
	}
	DEL_CLASS(frm);
}

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::RemoveStyleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontStyleForm *me = (SSWR::AVIRead::AVIRGISFontStyleForm*)userObj;
	me->env->RemoveFontStyle(me->fontStyle);
	me->fsFonts->UpdateFontStyles();
	me->fsFonts->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::EditStyleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontStyleForm *me = (SSWR::AVIRead::AVIRGISFontStyleForm*)userObj;
	SSWR::AVIRead::AVIRGISFontEditForm *frm;
	if (me->fontStyle < me->env->GetFontStyleCount())
	{
		NEW_CLASS(frm, SSWR::AVIRead::AVIRGISFontEditForm(0, me->ui, me->core, me->env, me->eng, me->fontStyle));
		if (frm->ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->fsFonts->Redraw();
			me->changed = true;
		}
		DEL_CLASS(frm);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::FontsSelectedChg(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontStyleForm *me = (SSWR::AVIRead::AVIRGISFontStyleForm *)userObj;
	UOSInt i = me->fsFonts->GetSelectedFontStyle();
	me->fontStyle = i;
}

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::FontsDoubleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontStyleForm *me = (SSWR::AVIRead::AVIRGISFontStyleForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::OKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontStyleForm *me = (SSWR::AVIRead::AVIRGISFontStyleForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISFontStyleForm::CancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISFontStyleForm *me = (SSWR::AVIRead::AVIRGISFontStyleForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void SSWR::AVIRead::AVIRGISFontStyleForm::UpdatePreview()
{
	this->fsFonts->UpdateFontStyles();
	this->fsFonts->Redraw();
}

SSWR::AVIRead::AVIRGISFontStyleForm::AVIRGISFontStyleForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::MapEnv *env, Media::DrawEngine *eng, UOSInt fontStyle) : UI::GUIForm(parent, 480, 306, ui)
{
	this->core = core;
	this->env = env;
	this->eng = eng;
	this->fontStyle = fontStyle;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->colorSess->AddHandler(this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->SetText((const UTF8Char*)"Font Style");
	this->SetFont(0, 8.25, false);

	NEW_CLASS(this->pnlBtn, UI::GUIPanel(ui, this));
	this->pnlBtn->SetRect(0, 0, 100, 48, false);
	this->pnlBtn->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this->pnlBtn, (const UTF8Char*)"OK"));
	this->btnOK->SetRect(130, 12, 100, 23, false);
	this->btnOK->HandleButtonClick(OKClicked, this);
	this->SetDefaultButton(this->btnOK);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlBtn, (const UTF8Char*)"Cancel"));
	this->btnCancel->SetRect(250, 12, 100, 23, false);
	this->btnCancel->HandleButtonClick(CancelClicked, this);
	this->SetCancelButton(this->btnCancel);

	NEW_CLASS(this->grpStyle, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Style"));
	this->grpStyle->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlStyle, UI::GUIPanel(ui, this->grpStyle));
	this->pnlStyle->SetRect(0, 0, 100, 32, false);
	this->pnlStyle->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnAddStyle, UI::GUIButton(ui, this->pnlStyle, (const UTF8Char*)"New Style"));
	this->btnAddStyle->SetRect(8, 8, 75, 23, false);
	this->btnAddStyle->HandleButtonClick(AddStyleClicked, this);
	NEW_CLASS(this->btnRemoveStyle, UI::GUIButton(ui, this->pnlStyle, (const UTF8Char*)"Remove Style"));
	this->btnRemoveStyle->SetRect(88, 8, 88, 23, false);
	this->btnRemoveStyle->HandleButtonClick(RemoveStyleClicked, this);
	NEW_CLASS(this->btnEditStyle, UI::GUIButton(ui, this->pnlStyle, (const UTF8Char*)"Edit Style"));
	this->btnEditStyle->SetRect(181, 8, 88, 23, false);
	this->btnEditStyle->HandleButtonClick(EditStyleClicked, this);

	NEW_CLASS(this->fsFonts, SSWR::AVIRead::AVIRFontSelector(ui, this->grpStyle, this->core, this->env, fontStyle, this->colorSess));
	this->fsFonts->SetDockType(UI::GUIControl::DOCK_FILL);
	this->fsFonts->HandleSelChg(FontsSelectedChg, this);

	this->changed = false;
	this->UpdatePreview();
}

SSWR::AVIRead::AVIRGISFontStyleForm::~AVIRGISFontStyleForm()
{
	this->colorSess->RemoveHandler(this);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISFontStyleForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGISFontStyleForm::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
}

void SSWR::AVIRead::AVIRGISFontStyleForm::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
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
