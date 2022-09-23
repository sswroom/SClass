#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRWMTSForm.h"

void __stdcall SSWR::AVIRead::AVIRWMTSForm::OnLoadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWMTSForm *me = (SSWR::AVIRead::AVIRWMTSForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtWMTSURL->GetText(&sb);
	SDEL_CLASS(me->wmts);
	NEW_CLASS(me->wmts, Map::WebMapTileServiceSource(me->core->GetSocketFactory(), me->core->GetEncFactory(), sb.ToCString()));
	if (me->wmts->IsError())
	{
		me->txtStatus->SetText(CSTR("Error"));
	}
	else
	{
		me->txtStatus->SetText(CSTR("Success"));
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

SSWR::AVIRead::AVIRWMTSForm::AVIRWMTSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 120, ui)
{
	this->core = core;
	this->wmts = 0;
	this->SetText(CSTR("Web Map Tile Service"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblWMTSURL, UI::GUILabel(ui, this, CSTR("WMTS URL")));
	this->lblWMTSURL->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtWMTSURL, UI::GUITextBox(ui, this, CSTR("")));
	this->txtWMTSURL->SetRect(104, 4, 500, 23, false);
	NEW_CLASS(this->btnLoad, UI::GUIButton(ui, this, CSTR("Load")));
	this->btnLoad->SetRect(104, 28, 75, 23, false);
	this->btnLoad->HandleButtonClick(OnLoadClicked, this);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, CSTR("Status")));
	this->lblStatus->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, CSTR("")));
	this->txtStatus->SetRect(104, 52, 100, 23, false);
	this->txtStatus->SetReadOnly(true);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(104, 76, 75, 23, false);
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
