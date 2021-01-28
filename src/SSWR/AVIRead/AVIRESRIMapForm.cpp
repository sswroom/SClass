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
	{"World Imagery", "http://services.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer"},
	{"World Street Map", "http://services.arcgisonline.com/ArcGIS/rest/services/World_Street_Map/MapServer"},
	{"World Topographic Map", "http://services.arcgisonline.com/ArcGIS/rest/services/World_Topo_Map/MapServer"},
	{"World Shaded Relief", "http://services.arcgisonline.com/ArcGIS/rest/services/World_Shaded_Relief/MapServer"},
	{"World Physical Map", "http://services.arcgisonline.com/ArcGIS/rest/services/World_Physical_Map/MapServer"},
	{"World Terrain Map", "http://server.arcgisonline.com/ArcGIS/rest/services/World_Terrain_Base/MapServer"},
	{"USA Topographic Maps", "http://services.arcgisonline.com/ArcGIS/rest/services/USA_Topo_Maps/MapServer"},
	{"Ocean Basemap", "http://services.arcgisonline.com/ArcGIS/rest/services/Ocean_Basemap/MapServer"},
	{"World Light Gray Base", "http://services.arcgisonline.com/ArcGIS/rest/services/Canvas/World_Light_Gray_Base/MapServer"},
	{"World Light Gray Reference", "http://services.arcgisonline.com/ArcGIS/rest/services/Canvas/World_Light_Gray_Reference/MapServer"},
	{"National Geographic World Map", "http://services.arcgisonline.com/ArcGIS/rest/services/NatGeo_World_Map/MapServer"},
	{"World Boundaries and Places", "http://services.arcgisonline.com/ArcGIS/rest/services/Reference/World_Boundaries_and_Places/MapServer"},
	{"World Boundaries and Places Alternate", "http://services.arcgisonline.com/ArcGIS/rest/services/Reference/World_Boundaries_and_Places_Alternate/MapServer"},
	{"World Reference Overlay", "http://server.arcgisonline.com/ArcGIS/rest/services/Reference/World_Reference_Overlay/MapServer"},
	{"World Transportation", "http://services.arcgisonline.com/ArcGIS/rest/services/Reference/World_Transportation/MapServer"},
	{"DeLorme World Basemap", "http://server.arcgisonline.com/ArcGIS/rest/services/Specialty/DeLorme_World_Base_Map/MapServer"},
	{"World Navigation Charts", "http://services.arcgisonline.com/ArcGIS/rest/services/Specialty/World_Navigation_Charts/MapServer"},
	{"Soil Survey Map", "http://server.arcgisonline.com/ArcGIS/rest/services/Specialty/Soil_Survey_Map/MapServer"}
};

void __stdcall SSWR::AVIRead::AVIRESRIMapForm::OKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRESRIMapForm *me = (SSWR::AVIRead::AVIRESRIMapForm*)userObj;
	if (me->radPredefine->IsSelected())
	{
		OSInt i = me->cboPredefine->GetSelectedIndex();
		const UTF8Char *v = (const UTF8Char*)me->cboPredefine->GetItem(i);
		if (v)
		{
			me->url = Text::StrCopyNew(v);
		}
		else
		{
			return;
		}
	}
	else
	{
		UTF8Char sbuff[512];
		sbuff[0] = 0;
		me->txtOther->GetText(sbuff);
		if (sbuff[0] == 0)
		{
			return;
		}
		else
		{
			me->url = Text::StrCopyNew(sbuff);
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
	this->SetText((const UTF8Char*)"Add ESRI Map");
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->url = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->radPredefine, UI::GUIRadioButton(ui, this, (const UTF8Char*)"Predefine", true));
	this->radPredefine->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->cboPredefine, UI::GUIComboBox(ui, this, false));
	this->cboPredefine->SetRect(108, 8, 150, 23, false);
	NEW_CLASS(this->radOther, UI::GUIRadioButton(ui, this, (const UTF8Char*)"Other", false));
	this->radOther->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->txtOther, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtOther->SetRect(108, 32, 500, 23, false);
	this->txtOther->HandleTextChanged(OnOtherChanged, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, (const UTF8Char*)"OK"));
	this->btnOK->SetRect(250, 64, 75, 23, false);
	this->btnOK->HandleButtonClick(OKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"Cancel"));
	this->btnCancel->SetRect(330, 64, 75, 23, false);
	this->btnCancel->HandleButtonClick(CancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	OSInt i = 0;
	OSInt j = sizeof(mapSvrs) / sizeof(mapSvrs[0]);
	while (i < j)
	{
		this->cboPredefine->AddItem((const UTF8Char*)mapSvrs[i].name, (void*)mapSvrs[i].url);
		i++;
	}
	if (j > 0)
	{
		this->cboPredefine->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRESRIMapForm::~AVIRESRIMapForm()
{
	if (this->url)
	{
		Text::StrDelNew(this->url);
		this->url = 0;
	}
}

void SSWR::AVIRead::AVIRESRIMapForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

const UTF8Char *SSWR::AVIRead::AVIRESRIMapForm::GetSelectedURL()
{
	return this->url;
}
