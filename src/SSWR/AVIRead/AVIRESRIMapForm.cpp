#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/SerialPort.h"
#include "IO/Device/MTKGPSNMEA.h"
#include "IO/StmData/FileData.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMTileMap.h"
#include "SSWR/AVIRead/AVIRESRIMapForm.h"
#include "Text/StringBuilder.hpp"
#include "UI/GUITextBox.h"

SSWR::AVIRead::AVIRESRIMapForm::MapServer SSWR::AVIRead::AVIRESRIMapForm::mapSvrs[] = {
//	http://server.arcgisonline.com/ArcGIS/rest/services/ESRI_Imagery_World_2D/MapServer
	{UTF8STRC("World Imagery"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer")},
	{UTF8STRC("World Street Map"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/World_Street_Map/MapServer")},
	{UTF8STRC("World Topographic Map"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/World_Topo_Map/MapServer")},
	{UTF8STRC("World Shaded Relief"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/World_Shaded_Relief/MapServer")},
	{UTF8STRC("World Physical Map"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/World_Physical_Map/MapServer")},
	{UTF8STRC("World Terrain Map"), UTF8STRC("https://server.arcgisonline.com/ArcGIS/rest/services/World_Terrain_Base/MapServer")},
	{UTF8STRC("USA Topographic Maps"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/USA_Topo_Maps/MapServer")},
	{UTF8STRC("Ocean Basemap"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/Ocean_Basemap/MapServer")},
	{UTF8STRC("World Light Gray Base"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/Canvas/World_Light_Gray_Base/MapServer")},
	{UTF8STRC("World Light Gray Reference"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/Canvas/World_Light_Gray_Reference/MapServer")},
	{UTF8STRC("National Geographic World Map"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/NatGeo_World_Map/MapServer")},
	{UTF8STRC("World Boundaries and Places"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/Reference/World_Boundaries_and_Places/MapServer")},
	{UTF8STRC("World Boundaries and Places Alternate"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/Reference/World_Boundaries_and_Places_Alternate/MapServer")},
	{UTF8STRC("World Reference Overlay"), UTF8STRC("https://server.arcgisonline.com/ArcGIS/rest/services/Reference/World_Reference_Overlay/MapServer")},
	{UTF8STRC("World Transportation"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/Reference/World_Transportation/MapServer")},
	{UTF8STRC("DeLorme World Basemap"), UTF8STRC("https://server.arcgisonline.com/ArcGIS/rest/services/Specialty/DeLorme_World_Base_Map/MapServer")},
	{UTF8STRC("World Navigation Charts"), UTF8STRC("https://services.arcgisonline.com/ArcGIS/rest/services/Specialty/World_Navigation_Charts/MapServer")},
	{UTF8STRC("Soil Survey Map"), UTF8STRC("https://server.arcgisonline.com/ArcGIS/rest/services/Specialty/Soil_Survey_Map/MapServer")}
};

void __stdcall SSWR::AVIRead::AVIRESRIMapForm::OKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRESRIMapForm> me = userObj.GetNN<SSWR::AVIRead::AVIRESRIMapForm>();
	Map::ESRI::ESRIMapServer *esriMap = 0;
	if (me->radPredefine->IsSelected())
	{
		UOSInt i = me->cboPredefine->GetSelectedIndex();
		NN<MapServer> v;
		if (me->cboPredefine->GetItem(i).GetOpt<MapServer>().SetTo(v))
		{
			NEW_CLASS(esriMap, Map::ESRI::ESRIMapServer(Text::CStringNN(v->url, v->urlLen), me->core->GetTCPClientFactory(), me->ssl, false));
		}
		else
		{
			return;
		}
	}
	else
	{
		UInt32 srid;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		Bool noResource = me->chkNoResource->IsChecked();
		if (noResource)
		{
			if (!me->txtSRID->GetText(sbuff).SetTo(sptr) || !Text::StrToUInt32(sbuff, srid))
			{
				me->ui->ShowMsgOK(CSTR("Please enter valid SRID"), CSTR("ESRI Map"), me);
				return;
			}
		}
		else
		{
			srid = 4326;
		}
		sbuff[0] = 0;
		sptr = me->txtOther->GetText(sbuff).Or(sbuff);
		if (sbuff[0] == 0)
		{
			return;
		}
		else
		{
			NEW_CLASS(esriMap, Map::ESRI::ESRIMapServer(CSTRP(sbuff, sptr), me->core->GetTCPClientFactory(), me->ssl, noResource));
			if (noResource)
			{
				esriMap->SetSRID(srid);
			}
		}
	}
	if (esriMap->IsError())
	{
		me->ui->ShowMsgOK(CSTR("Error in loading server info"), CSTR("ESRI Map"), me);
		return;
	}
	me->esriMap = esriMap;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRESRIMapForm::CancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRESRIMapForm> me = userObj.GetNN<SSWR::AVIRead::AVIRESRIMapForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRESRIMapForm::OnOtherChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRESRIMapForm> me = userObj.GetNN<SSWR::AVIRead::AVIRESRIMapForm>();
	me->radOther->Select();
}

void __stdcall SSWR::AVIRead::AVIRESRIMapForm::OnNoResourceChg(AnyType userObj, Bool newValue)
{
	NN<SSWR::AVIRead::AVIRESRIMapForm> me = userObj.GetNN<SSWR::AVIRead::AVIRESRIMapForm>();
	me->radOther->Select();
	me->txtSRID->SetReadOnly(!newValue);
}

SSWR::AVIRead::AVIRESRIMapForm::AVIRESRIMapForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl) : UI::GUIForm(parent, 640, 144, ui)
{
	this->SetText(CSTR("Add ESRI Map"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->ssl = ssl;
	this->esriMap = nullptr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	this->radPredefine = ui->NewRadioButton(*this, CSTR("Predefine"), true);
	this->radPredefine->SetRect(8, 8, 100, 23, false);
	this->cboPredefine = ui->NewComboBox(*this, false);
	this->cboPredefine->SetRect(108, 8, 150, 23, false);
	this->radOther = ui->NewRadioButton(*this, CSTR("Other"), false);
	this->radOther->SetRect(8, 32, 100, 23, false);
	this->txtOther = ui->NewTextBox(*this, CSTR(""));
	this->txtOther->SetRect(108, 32, 500, 23, false);
	this->txtOther->HandleTextChanged(OnOtherChanged, this);
	this->chkNoResource = ui->NewCheckBox(*this, CSTR("No Resource"), false);
	this->chkNoResource->SetRect(108, 56, 120, 23, false);
	this->chkNoResource->HandleCheckedChange(OnNoResourceChg, this);
	this->lblSRID = ui->NewLabel(*this, CSTR("SRID"));
	this->lblSRID->SetRect(228, 56, 100, 23, false);
	this->txtSRID = ui->NewTextBox(*this, CSTR("4326"));
	this->txtSRID->SetRect(328, 56, 100, 23, false);
	this->txtSRID->SetReadOnly(true);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(250, 88, 75, 23, false);
	this->btnOK->HandleButtonClick(OKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(330, 88, 75, 23, false);
	this->btnCancel->HandleButtonClick(CancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	OSInt i = 0;
	OSInt j = sizeof(mapSvrs) / sizeof(mapSvrs[0]);
	while (i < j)
	{
		this->cboPredefine->AddItem({mapSvrs[i].name, mapSvrs[i].nameLen}, (void*)&mapSvrs[i]);
		i++;
	}
	if (j > 0)
	{
		this->cboPredefine->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRESRIMapForm::~AVIRESRIMapForm()
{
}

void SSWR::AVIRead::AVIRESRIMapForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<Map::ESRI::ESRIMapServer> SSWR::AVIRead::AVIRESRIMapForm::GetSelectedMap()
{
	return this->esriMap;
}
