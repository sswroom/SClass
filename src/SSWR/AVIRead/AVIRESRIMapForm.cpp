#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/SerialPort.h"
#include "IO/Device/MTKGPSNMEA.h"
#include "IO/StmData/FileData.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMTileMap.h"
#include "SSWR/AVIRead/AVIRESRIMapForm.h"
#include "Text/StringBuilder.h"
#include "UI/MessageDialog.h"
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

void __stdcall SSWR::AVIRead::AVIRESRIMapForm::OKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRESRIMapForm *me = (SSWR::AVIRead::AVIRESRIMapForm*)userObj;
	if (me->radPredefine->IsSelected())
	{
		UOSInt i = me->cboPredefine->GetSelectedIndex();
		MapServer *v = (MapServer*)me->cboPredefine->GetItem(i);
		if (v)
		{
			me->url = Text::String::New(v->url, v->urlLen);
		}
		else
		{
			return;
		}
	}
	else
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		sbuff[0] = 0;
		sptr = me->txtOther->GetText(sbuff);
		if (sbuff[0] == 0)
		{
			return;
		}
		else
		{
			me->url = Text::String::NewP(sbuff, sptr);
		}
	}
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRESRIMapForm::CancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRESRIMapForm *me = (SSWR::AVIRead::AVIRESRIMapForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRESRIMapForm::OnOtherChanged(void *userObj)
{
	SSWR::AVIRead::AVIRESRIMapForm *me = (SSWR::AVIRead::AVIRESRIMapForm*)userObj;
	me->radOther->Select();
}

SSWR::AVIRead::AVIRESRIMapForm::AVIRESRIMapForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 120, ui)
{
	this->SetText(CSTR("Add ESRI Map"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->url = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->radPredefine, UI::GUIRadioButton(ui, this, CSTR("Predefine"), true));
	this->radPredefine->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->cboPredefine, UI::GUIComboBox(ui, this, false));
	this->cboPredefine->SetRect(108, 8, 150, 23, false);
	NEW_CLASS(this->radOther, UI::GUIRadioButton(ui, this, CSTR("Other"), false));
	this->radOther->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->txtOther, UI::GUITextBox(ui, this, CSTR("")));
	this->txtOther->SetRect(108, 32, 500, 23, false);
	this->txtOther->HandleTextChanged(OnOtherChanged, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(250, 64, 75, 23, false);
	this->btnOK->HandleButtonClick(OKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("Cancel")));
	this->btnCancel->SetRect(330, 64, 75, 23, false);
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
	SDEL_STRING(this->url);
}

void SSWR::AVIRead::AVIRESRIMapForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Text::String *SSWR::AVIRead::AVIRESRIMapForm::GetSelectedURL()
{
	return this->url;
}
