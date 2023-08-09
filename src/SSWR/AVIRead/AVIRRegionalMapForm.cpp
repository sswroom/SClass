#include "Stdafx.h"
#include "Map/RegionalMapSource.h"
#include "SSWR/AVIRead/AVIRRegionalMapForm.h"

void __stdcall SSWR::AVIRead::AVIRRegionalMapForm::OnMapsSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRegionalMapForm *me = (SSWR::AVIRead::AVIRRegionalMapForm*)userObj;
	const Map::RegionalMapSource::MapInfo *map = (const Map::RegionalMapSource::MapInfo*)me->lvMaps->GetSelectedItem();
	if (map)
	{
		me->txtDesc->SetText(Text::CString(map->desc, map->descLen));
	}
	else
	{
		me->txtDesc->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRRegionalMapForm::OnMapsDblClk(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRRegionalMapForm *me = (SSWR::AVIRead::AVIRRegionalMapForm*)userObj;
	const Map::RegionalMapSource::MapInfo *map = (const Map::RegionalMapSource::MapInfo*)me->lvMaps->GetItem(index);
	me->layer = Map::RegionalMapSource::OpenMap(map, me->core->GetSocketFactory(), me->ssl, me->core->GetEncFactory(), me->core->GetParserList(), me->core->GetWebBrowser());
	if (me->layer)
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

SSWR::AVIRead::AVIRRegionalMapForm::AVIRRegionalMapForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Net::SSLEngine *ssl) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Regional Map Source"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->ssl = ssl;
	this->layer = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->txtDesc, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtDesc->SetRect(0, 0, 100, 71, false);
	this->txtDesc->SetReadOnly(true);
	this->txtDesc->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lvMaps, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvMaps->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMaps->HandleSelChg(OnMapsSelChg, this);
	this->lvMaps->HandleDblClk(OnMapsDblClk, this);
	this->lvMaps->AddColumn(CSTR("Name"), 150);
	this->lvMaps->AddColumn(CSTR("Region"), 100);
	this->lvMaps->AddColumn(CSTR("Organization"), 200);

	UOSInt i = 0;
	UOSInt j;
	const Map::RegionalMapSource::MapInfo *maps = Map::RegionalMapSource::GetMapInfos(&j);
	while (i < j)
	{
		this->lvMaps->AddItem(Text::CString(maps[i].name, maps[i].nameLen), (void*)&maps[i]);
		this->lvMaps->SetSubItem(i, 1, Text::CString(maps[i].region, maps[i].regionLen));
		this->lvMaps->SetSubItem(i, 2, Text::CString(maps[i].org, maps[i].orgLen));
		i++;
	}
}

SSWR::AVIRead::AVIRRegionalMapForm::~AVIRRegionalMapForm()
{
}

void SSWR::AVIRead::AVIRRegionalMapForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Map::MapDrawLayer *SSWR::AVIRead::AVIRRegionalMapForm::GetMapLayer()
{
	return this->layer;
}
