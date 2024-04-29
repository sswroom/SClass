#include "Stdafx.h"
#include "Map/RegionalMapSource.h"
#include "SSWR/AVIRead/AVIRRegionalMapForm.h"

void __stdcall SSWR::AVIRead::AVIRRegionalMapForm::OnMapsSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRegionalMapForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRegionalMapForm>();
	NN<const Map::RegionalMapSource::MapInfo> map;
	if (me->lvMaps->GetSelectedItem().GetOpt<const Map::RegionalMapSource::MapInfo>().SetTo(map))
	{
		me->txtDesc->SetText(Text::CStringNN(map->desc, map->descLen));
	}
	else
	{
		me->txtDesc->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRRegionalMapForm::OnMapsDblClk(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRRegionalMapForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRegionalMapForm>();
	NN<const Map::RegionalMapSource::MapInfo> map;
	if (me->lvMaps->GetItem(index).GetOpt<const Map::RegionalMapSource::MapInfo>().SetTo(map))
	{
		me->layer = Map::RegionalMapSource::OpenMap(map, me->core->GetSocketFactory(), me->ssl, me->core->GetEncFactory(), me->core->GetParserList(), me->core->GetWebBrowser(), me->envCSys);
		if (me->layer.NotNull())
		{
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
	}
}

SSWR::AVIRead::AVIRRegionalMapForm::AVIRRegionalMapForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl, NN<Math::CoordinateSystem> envCSys) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Regional Map Source"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->ssl = ssl;
	this->envCSys = envCSys;
	this->layer = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->txtDesc = ui->NewTextBox(*this, CSTR(""), true);
	this->txtDesc->SetRect(0, 0, 100, 71, false);
	this->txtDesc->SetReadOnly(true);
	this->txtDesc->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvMaps = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
	this->lvMaps->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMaps->HandleSelChg(OnMapsSelChg, this);
	this->lvMaps->HandleDblClk(OnMapsDblClk, this);
	this->lvMaps->AddColumn(CSTR("Name"), 150);
	this->lvMaps->AddColumn(CSTR("Region"), 100);
	this->lvMaps->AddColumn(CSTR("Type"), 100);
	this->lvMaps->AddColumn(CSTR("Organization"), 200);

	UOSInt i = 0;
	UOSInt j;
	const Map::RegionalMapSource::MapInfo *maps = Map::RegionalMapSource::GetMapInfos(&j);
	while (i < j)
	{
		this->lvMaps->AddItem(Text::CStringNN(maps[i].name, maps[i].nameLen), (void*)&maps[i]);
		this->lvMaps->SetSubItem(i, 1, Text::CStringNN(maps[i].region, maps[i].regionLen));
		this->lvMaps->SetSubItem(i, 2, Map::RegionalMapSource::MapTypeGetName(maps[i].mapType));
		this->lvMaps->SetSubItem(i, 3, Text::CStringNN(maps[i].org, maps[i].orgLen));
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

Optional<Map::MapDrawLayer> SSWR::AVIRead::AVIRRegionalMapForm::GetMapLayer()
{
	return this->layer;
}
