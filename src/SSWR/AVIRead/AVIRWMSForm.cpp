#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRWMSForm.h"

void __stdcall SSWR::AVIRead::AVIRWMSForm::OnLoadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWMSForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWMSForm>();
	Text::StringBuilderUTF8 sb;
	me->txtWMSURL->GetText(sb);
	me->wms.Delete();
	NN<Map::WebMapService> wms;
	NEW_CLASSNN(wms, Map::WebMapService(me->core->GetTCPClientFactory(), me->ssl, me->core->GetEncFactory(), sb.ToCString(), (Map::WebMapService::Version)me->cboWMSVersion->GetSelectedItem().GetOSInt(), me->envCsys));
	me->wms = wms;
	if (wms->IsError())
	{
		me->txtStatus->SetText(CSTR("Error"));
	}
	else
	{
		me->txtStatus->SetText(CSTR("Success"));
		Data::ArrayListStringNN nameList;
		UOSInt selIndex;
		UOSInt i = 0;
		UOSInt j = wms->GetLayerNames(nameList);
		me->cboLayer->ClearItems();
		while (i < j)
		{
			me->cboLayer->AddItem(nameList.GetItemNoCheck(i), 0);
			i++;
		}
		if (j > 0)
		{
			me->cboLayer->SetSelectedIndex(0);
		}

		nameList.Clear();
		i = 0;
		j = wms->GetMapImageTypeNames(nameList);
		selIndex = 0;
		me->cboMapImageType->ClearItems();
		while (i < j)
		{
			me->cboMapImageType->AddItem(nameList.GetItemNoCheck(i), 0);
			if (nameList.GetItemNoCheck(i)->Equals(UTF8STRC("image/png")))
				selIndex = i;
			i++;
		}
		if (j > 0)
		{
			me->cboMapImageType->SetSelectedIndex(selIndex);
		}

		nameList.Clear();
		i = 0;
		j = wms->GetInfoTypeNames(nameList);
		me->cboInfoType->ClearItems();
		while (i < j)
		{
			me->cboInfoType->AddItem(nameList.GetItemNoCheck(i), 0);
			i++;
		}
		if (j > 0)
		{
			me->cboInfoType->SetSelectedIndex(wms->GetInfoType());
		}

	}
}

void __stdcall SSWR::AVIRead::AVIRWMSForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWMSForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWMSForm>();
	NN<Map::WebMapService> wms;
	if (me->wms.SetTo(wms) && !wms->IsError())
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRWMSForm::OnLayerSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWMSForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWMSForm>();
	NN<Map::WebMapService> wms;
	if (me->wms.SetTo(wms) && !wms->IsError())
	{
		wms->SetLayer(me->cboLayer->GetSelectedIndex());
		me->cboLayerCRS->ClearItems();
		Data::ArrayListNN<Text::String> nameList;
		UOSInt i = 0;
		UOSInt j = wms->GetLayerCRSNames(nameList);
		while (i < j)
		{
			me->cboLayerCRS->AddItem(nameList.GetItemNoCheck(i), 0);
			i++;
		}
		if (j > 0)
		{
			me->cboLayerCRS->SetSelectedIndex(wms->GetLayerCRS());
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWMSForm::OnLayerCRSSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWMSForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWMSForm>();
	NN<Map::WebMapService> wms;
	if (me->wms.SetTo(wms) && !wms->IsError())
	{
		wms->SetLayerCRS(me->cboLayerCRS->GetSelectedIndex());
	}
}

void __stdcall SSWR::AVIRead::AVIRWMSForm::OnMapImageTypeSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWMSForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWMSForm>();
	NN<Map::WebMapService> wms;
	if (me->wms.SetTo(wms) && !wms->IsError())
	{
		wms->SetMapImageType(me->cboMapImageType->GetSelectedIndex());
	}
}

void __stdcall SSWR::AVIRead::AVIRWMSForm::OnInfoTypeSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWMSForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWMSForm>();
	NN<Map::WebMapService> wms;
	if (me->wms.SetTo(wms) && !wms->IsError())
	{
		wms->SetInfoType(me->cboInfoType->GetSelectedIndex());
	}
}

SSWR::AVIRead::AVIRWMSForm::AVIRWMSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl, NN<Math::CoordinateSystem> envCsys) : UI::GUIForm(parent, 640, 240, ui)
{
	this->core = core;
	this->ssl = ssl;
	this->envCsys = envCsys;
	this->wms = 0;
	this->SetText(CSTR("Web Map Service"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblWMSURL = ui->NewLabel(*this, CSTR("WMS URL"));
	this->lblWMSURL->SetRect(4, 4, 100, 23, false);
	this->txtWMSURL = ui->NewTextBox(*this, CSTR(""));
	this->txtWMSURL->SetRect(104, 4, 500, 23, false);
	this->lblWMSVersion = ui->NewLabel(*this, CSTR("Version"));
	this->lblWMSVersion->SetRect(4, 28, 100, 23, false);
	this->cboWMSVersion = ui->NewComboBox(*this, false);
	this->cboWMSVersion->SetRect(104, 28, 100, 23, false);
	this->cboWMSVersion->AddItem(CSTR("Any"), (void*)Map::WebMapService::Version::ANY);
	this->cboWMSVersion->AddItem(CSTR("1.3.0"), (void*)Map::WebMapService::Version::V1_3_0);
	this->cboWMSVersion->AddItem(CSTR("1.1.1"), (void*)Map::WebMapService::Version::V1_1_1);
	this->cboWMSVersion->AddItem(CSTR("1.1.1 Tiled"), (void*)Map::WebMapService::Version::V1_1_1_TILED);
	this->cboWMSVersion->SetSelectedIndex(0);
	this->btnLoad = ui->NewButton(*this, CSTR("Load"));
	this->btnLoad->SetRect(104, 52, 75, 23, false);
	this->btnLoad->HandleButtonClick(OnLoadClicked, this);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(4, 76, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetRect(104, 76, 100, 23, false);
	this->txtStatus->SetReadOnly(true);
	this->lblLayer = ui->NewLabel(*this, CSTR("Layer"));
	this->lblLayer->SetRect(4, 100, 100, 23, false);
	this->cboLayer = ui->NewComboBox(*this, false);
	this->cboLayer->SetRect(104, 100, 200, 23, false);
	this->cboLayer->HandleSelectionChange(OnLayerSelChg, this);
	this->lblLayerCRS = ui->NewLabel(*this, CSTR("CRS"));
	this->lblLayerCRS->SetRect(4, 124, 100, 23, false);
	this->cboLayerCRS = ui->NewComboBox(*this, false);
	this->cboLayerCRS->SetRect(104, 124, 200, 23, false);
	this->cboLayerCRS->HandleSelectionChange(OnLayerCRSSelChg, this);
	this->lblMapImageType = ui->NewLabel(*this, CSTR("Map Image Type"));
	this->lblMapImageType->SetRect(4, 148, 100, 23, false);
	this->cboMapImageType = ui->NewComboBox(*this, false);
	this->cboMapImageType->SetRect(104, 148, 200, 23, false);
	this->cboMapImageType->HandleSelectionChange(OnMapImageTypeSelChg, this);
	this->lblInfoType = ui->NewLabel(*this, CSTR("Info Type"));
	this->lblInfoType->SetRect(4, 172, 100, 23, false);
	this->cboInfoType = ui->NewComboBox(*this, false);
	this->cboInfoType->SetRect(104, 172, 200, 23, false);
	this->cboInfoType->HandleSelectionChange(OnInfoTypeSelChg, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(104, 196, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
}

SSWR::AVIRead::AVIRWMSForm::~AVIRWMSForm()
{
	this->wms.Delete();
}

void SSWR::AVIRead::AVIRWMSForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRWMSForm::SetURL(Text::CStringNN url)
{
	this->txtWMSURL->SetText(url);
	OnLoadClicked(this);
}

Optional<Map::DrawMapService> SSWR::AVIRead::AVIRWMSForm::GetDrawMapService()
{
	Optional<Map::DrawMapService> mapService = this->wms;
	this->wms = 0;
	return mapService;
}
