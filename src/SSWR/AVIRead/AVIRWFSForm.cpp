#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRWFSForm.h"

void __stdcall SSWR::AVIRead::AVIRWFSForm::OnLoadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWFSForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWFSForm>();
	NN<Map::WebFeatureService> wfs;
	Text::StringBuilderUTF8 sb;
	me->txtWFSURL->GetText(sb);
	me->wfs.Delete();
	NEW_CLASSNN(wfs, Map::WebFeatureService(me->core->GetTCPClientFactory(), nullptr, me->core->GetEncFactory(), sb.ToCString(), (Map::WebFeatureService::Version)me->cboWFSVersion->GetSelectedItem().GetIntOS()));
	me->wfs = wfs;
	if (wfs->IsError())
	{
		me->txtStatus->SetText(CSTR("Error"));
	}
	else
	{
		me->txtStatus->SetText(CSTR("Success"));
		Data::ArrayListStringNN nameList;
		UIntOS i = 0;
		UIntOS j = wfs->GetFeatureNames(&nameList);
		me->cboFeature->ClearItems();
		while (i < j)
		{
			me->cboFeature->AddItem(Text::String::OrEmpty(nameList.GetItem(i)), 0);
			i++;
		}
		if (j > 0)
		{
			me->cboFeature->SetSelectedIndex(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWFSForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWFSForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWFSForm>();
	NN<Map::WebFeatureService> wfs;
	if (me->wfs.SetTo(wfs) && !wfs->IsError())
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRWFSForm::OnFeatureSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWFSForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWFSForm>();
	NN<Map::WebFeatureService> wfs;
	if (me->wfs.SetTo(wfs) && !wfs->IsError())
	{
		wfs->SetFeature(me->cboFeature->GetSelectedIndex());
	}
}

SSWR::AVIRead::AVIRWFSForm::AVIRWFSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 240, ui)
{
	this->core = core;
	this->wfs = nullptr;
	this->SetText(CSTR("Web Feature Service"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblWFSURL = ui->NewLabel(*this, CSTR("WFS URL"));
	this->lblWFSURL->SetRect(4, 4, 100, 23, false);
	this->txtWFSURL = ui->NewTextBox(*this, CSTR(""));
	this->txtWFSURL->SetRect(104, 4, 500, 23, false);
	this->lblWFSVersion = ui->NewLabel(*this, CSTR("Version"));
	this->lblWFSVersion->SetRect(4, 28, 100, 23, false);
	this->cboWFSVersion = ui->NewComboBox(*this, false);
	this->cboWFSVersion->SetRect(104, 28, 100, 23, false);
	this->cboWFSVersion->AddItem(CSTR("Any"), (void*)Map::WebFeatureService::Version::ANY);
	this->cboWFSVersion->AddItem(CSTR("2.0.0"), (void*)Map::WebFeatureService::Version::V2_0_0);
	this->cboWFSVersion->AddItem(CSTR("1.1.0"), (void*)Map::WebFeatureService::Version::V1_1_0);
	this->cboWFSVersion->AddItem(CSTR("1.0.0"), (void*)Map::WebFeatureService::Version::V1_0_0);
	this->cboWFSVersion->SetSelectedIndex(0);
	this->btnLoad = ui->NewButton(*this, CSTR("Load"));
	this->btnLoad->SetRect(104, 52, 75, 23, false);
	this->btnLoad->HandleButtonClick(OnLoadClicked, this);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(4, 76, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetRect(104, 76, 100, 23, false);
	this->txtStatus->SetReadOnly(true);
	this->lblFeature = ui->NewLabel(*this, CSTR("Feature"));
	this->lblFeature->SetRect(4, 100, 100, 23, false);
	this->cboFeature = ui->NewComboBox(*this, false);
	this->cboFeature->SetRect(104, 100, 200, 23, false);
	this->cboFeature->HandleSelectionChange(OnFeatureSelChg, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(104, 124, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
}

SSWR::AVIRead::AVIRWFSForm::~AVIRWFSForm()
{
	this->wfs.Delete();
}

void SSWR::AVIRead::AVIRWFSForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<Map::MapDrawLayer> SSWR::AVIRead::AVIRWFSForm::LoadLayer()
{
	NN<Map::WebFeatureService> wfs;
	if (this->wfs.SetTo(wfs))
		return wfs->LoadAsLayer();
	return nullptr;
}
