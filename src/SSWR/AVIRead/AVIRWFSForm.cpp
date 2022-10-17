#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRWFSForm.h"

void __stdcall SSWR::AVIRead::AVIRWFSForm::OnLoadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWFSForm *me = (SSWR::AVIRead::AVIRWFSForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtWFSURL->GetText(&sb);
	SDEL_CLASS(me->wfs);
	NEW_CLASS(me->wfs, Map::WebFeatureService(me->core->GetSocketFactory(), 0, me->core->GetEncFactory(), sb.ToCString(), (Map::WebFeatureService::Version)(OSInt)me->cboWFSVersion->GetSelectedItem()));
	if (me->wfs->IsError())
	{
		me->txtStatus->SetText(CSTR("Error"));
	}
	else
	{
		me->txtStatus->SetText(CSTR("Success"));
		Data::ArrayList<Text::String*> nameList;
		UOSInt i = 0;
		UOSInt j = me->wfs->GetFeatureNames(&nameList);
		me->cboFeature->ClearItems();
		while (i < j)
		{
			me->cboFeature->AddItem(nameList.GetItem(i), 0);
			i++;
		}
		if (j > 0)
		{
			me->cboFeature->SetSelectedIndex(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWFSForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWFSForm *me = (SSWR::AVIRead::AVIRWFSForm *)userObj;
	if (me->wfs && !me->wfs->IsError())
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRWFSForm::OnFeatureSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRWFSForm *me = (SSWR::AVIRead::AVIRWFSForm *)userObj;
	if (me->wfs && !me->wfs->IsError())
	{
		me->wfs->SetFeature(me->cboFeature->GetSelectedIndex());
	}
}

SSWR::AVIRead::AVIRWFSForm::AVIRWFSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 240, ui)
{
	this->core = core;
	this->wfs = 0;
	this->SetText(CSTR("Web Feature Service"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblWFSURL, UI::GUILabel(ui, this, CSTR("WFS URL")));
	this->lblWFSURL->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtWFSURL, UI::GUITextBox(ui, this, CSTR("")));
	this->txtWFSURL->SetRect(104, 4, 500, 23, false);
	NEW_CLASS(this->lblWFSVersion, UI::GUILabel(ui, this, CSTR("Version")));
	this->lblWFSVersion->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboWFSVersion, UI::GUIComboBox(ui, this, false));
	this->cboWFSVersion->SetRect(104, 28, 100, 23, false);
	this->cboWFSVersion->AddItem(CSTR("Any"), (void*)Map::WebFeatureService::Version::ANY);
	this->cboWFSVersion->AddItem(CSTR("2.0.0"), (void*)Map::WebFeatureService::Version::V2_0_0);
	this->cboWFSVersion->AddItem(CSTR("1.1.0"), (void*)Map::WebFeatureService::Version::V1_1_0);
	this->cboWFSVersion->AddItem(CSTR("1.0.0"), (void*)Map::WebFeatureService::Version::V1_0_0);
	this->cboWFSVersion->SetSelectedIndex(0);
	NEW_CLASS(this->btnLoad, UI::GUIButton(ui, this, CSTR("Load")));
	this->btnLoad->SetRect(104, 52, 75, 23, false);
	this->btnLoad->HandleButtonClick(OnLoadClicked, this);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, CSTR("Status")));
	this->lblStatus->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, CSTR("")));
	this->txtStatus->SetRect(104, 76, 100, 23, false);
	this->txtStatus->SetReadOnly(true);
	NEW_CLASS(this->lblFeature, UI::GUILabel(ui, this, CSTR("Feature")));
	this->lblFeature->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->cboFeature, UI::GUIComboBox(ui, this, false));
	this->cboFeature->SetRect(104, 100, 200, 23, false);
	this->cboFeature->HandleSelectionChange(OnFeatureSelChg, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(104, 124, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
}

SSWR::AVIRead::AVIRWFSForm::~AVIRWFSForm()
{
	SDEL_CLASS(this->wfs);
}

void SSWR::AVIRead::AVIRWFSForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Map::IMapDrawLayer *SSWR::AVIRead::AVIRWFSForm::LoadLayer()
{
	return this->wfs->LoadAsLayer();
}
