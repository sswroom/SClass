#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRWMTSForm.h"

void __stdcall SSWR::AVIRead::AVIRWMTSForm::OnLoadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWMTSForm *me = (SSWR::AVIRead::AVIRWMTSForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtWMTSURL->GetText(sb);
	SDEL_CLASS(me->wmts);
	NEW_CLASS(me->wmts, Map::WebMapTileServiceSource(me->core->GetSocketFactory(), me->ssl, me->core->GetEncFactory(), sb.ToCString()));
	if (me->wmts->IsError())
	{
		me->txtStatus->SetText(CSTR("Error"));
	}
	else
	{
		me->txtStatus->SetText(CSTR("Success"));
		Data::ArrayList<Text::String*> nameList;
		UOSInt i = 0;
		UOSInt j = me->wmts->GetLayerNames(&nameList);
		me->cboLayer->ClearItems();
		while (i < j)
		{
			me->cboLayer->AddItem(Text::String::OrEmpty(nameList.GetItem(i)), 0);
			i++;
		}
		if (j > 0)
		{
			me->cboLayer->SetSelectedIndex(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWMTSForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWMTSForm *me = (SSWR::AVIRead::AVIRWMTSForm *)userObj;
	if (me->wmts && !me->wmts->IsError())
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRWMTSForm::OnLayerSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRWMTSForm *me = (SSWR::AVIRead::AVIRWMTSForm *)userObj;
	if (me->wmts && !me->wmts->IsError())
	{
		me->wmts->SetLayer(me->cboLayer->GetSelectedIndex());
		Data::ArrayListStringNN nameList;
		UOSInt i = 0;
		UOSInt j = me->wmts->GetMatrixSetNames(&nameList);
		me->cboMatrixSet->ClearItems();
		while (i < j)
		{
			me->cboMatrixSet->AddItem(Text::String::OrEmpty(nameList.GetItem(i)), 0);
			i++;
		}
		if (j > 0)
		{
			me->cboMatrixSet->SetSelectedIndex(0);
		}

		nameList.Clear();
		i = 0;
		j = me->wmts->GetResourceTileTypeNames(&nameList);
		me->cboResourceTileType->ClearItems();
		while (i < j)
		{
			me->cboResourceTileType->AddItem(Text::String::OrEmpty(nameList.GetItem(i)), 0);
			i++;
		}
		if (j > 0)
		{
			me->cboResourceTileType->SetSelectedIndex(0);
		}

		nameList.Clear();
		i = 0;
		j = me->wmts->GetResourceInfoTypeNames(&nameList);
		me->cboResourceInfoType->ClearItems();
		while (i < j)
		{
			me->cboResourceInfoType->AddItem(Text::String::OrEmpty(nameList.GetItem(i)), 0);
			i++;
		}
		if (j > 0)
		{
			me->cboResourceInfoType->SetSelectedIndex(me->wmts->GetResourceInfoType());
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWMTSForm::OnMatrixSetSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRWMTSForm *me = (SSWR::AVIRead::AVIRWMTSForm *)userObj;
	if (me->wmts && !me->wmts->IsError())
	{
		me->wmts->SetMatrixSet(me->cboMatrixSet->GetSelectedIndex());
	}
}

void __stdcall SSWR::AVIRead::AVIRWMTSForm::OnResourceTileTypeSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRWMTSForm *me = (SSWR::AVIRead::AVIRWMTSForm *)userObj;
	if (me->wmts && !me->wmts->IsError())
	{
		me->wmts->SetResourceTileType(me->cboResourceTileType->GetSelectedIndex());
	}
}

void __stdcall SSWR::AVIRead::AVIRWMTSForm::OnResourceInfoTypeSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRWMTSForm *me = (SSWR::AVIRead::AVIRWMTSForm *)userObj;
	if (me->wmts && !me->wmts->IsError())
	{
		me->wmts->SetResourceInfoType(me->cboResourceInfoType->GetSelectedIndex());
	}
}

SSWR::AVIRead::AVIRWMTSForm::AVIRWMTSForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl) : UI::GUIForm(parent, 640, 240, ui)
{
	this->core = core;
	this->ssl = ssl;
	this->wmts = 0;
	this->SetText(CSTR("Web Map Tile Service"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblWMTSURL = ui->NewLabel(*this, CSTR("WMTS URL"));
	this->lblWMTSURL->SetRect(4, 4, 100, 23, false);
	this->txtWMTSURL = ui->NewTextBox(*this, CSTR(""));
	this->txtWMTSURL->SetRect(104, 4, 500, 23, false);
	this->btnLoad = ui->NewButton(*this, CSTR("Load"));
	this->btnLoad->SetRect(104, 28, 75, 23, false);
	this->btnLoad->HandleButtonClick(OnLoadClicked, this);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(4, 52, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetRect(104, 52, 100, 23, false);
	this->txtStatus->SetReadOnly(true);
	this->lblLayer = ui->NewLabel(*this, CSTR("Layer"));
	this->lblLayer->SetRect(4, 76, 100, 23, false);
	this->cboLayer = ui->NewComboBox(*this, false);
	this->cboLayer->SetRect(104, 76, 200, 23, false);
	this->cboLayer->HandleSelectionChange(OnLayerSelChg, this);
	this->lblMatrixSet = ui->NewLabel(*this, CSTR("Matrix Set"));
	this->lblMatrixSet->SetRect(4, 100, 100, 23, false);
	this->cboMatrixSet = ui->NewComboBox(*this, false);
	this->cboMatrixSet->SetRect(104, 100, 200, 23, false);
	this->cboMatrixSet->HandleSelectionChange(OnMatrixSetSelChg, this);
	this->lblResourceTileType = ui->NewLabel(*this, CSTR("Tile Type"));
	this->lblResourceTileType->SetRect(4, 124, 100, 23, false);
	this->cboResourceTileType = ui->NewComboBox(*this, false);
	this->cboResourceTileType->SetRect(104, 124, 200, 23, false);
	this->cboResourceTileType->HandleSelectionChange(OnResourceTileTypeSelChg, this);
	this->lblResourceInfoType = ui->NewLabel(*this, CSTR("Info Type"));
	this->lblResourceInfoType->SetRect(4, 148, 100, 23, false);
	this->cboResourceInfoType = ui->NewComboBox(*this, false);
	this->cboResourceInfoType->SetRect(104, 148, 200, 23, false);
	this->cboResourceInfoType->HandleSelectionChange(OnResourceInfoTypeSelChg, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(104, 172, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
}

SSWR::AVIRead::AVIRWMTSForm::~AVIRWMTSForm()
{
	SDEL_CLASS(this->wmts);
}

void SSWR::AVIRead::AVIRWMTSForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Map::TileMap *SSWR::AVIRead::AVIRWMTSForm::GetTileMap()
{
	Map::TileMap *tile = this->wmts;
	this->wmts = 0;
	return tile;
}
