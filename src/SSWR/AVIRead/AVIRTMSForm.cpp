#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRTMSForm.h"

void __stdcall SSWR::AVIRead::AVIRTMSForm::OnLoadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTMSForm *me = (SSWR::AVIRead::AVIRTMSForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtTMSURL->GetText(&sb);
	SDEL_CLASS(me->tms);
	NEW_CLASS(me->tms, Map::TileMapServiceSource(me->core->GetSocketFactory(), me->core->GetEncFactory(), sb.ToCString()));
	if (me->tms->IsError())
	{
		me->txtStatus->SetText(CSTR("Error"));
	}
	else
	{
		me->txtStatus->SetText(CSTR("Success"));
	}
}

SSWR::AVIRead::AVIRTMSForm::AVIRTMSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 120, ui)
{
	this->core = core;
	this->tms = 0;
	this->SetText(CSTR("Tile Map Service"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblTMSURL, UI::GUILabel(ui, this, CSTR("TMS URL")));
	this->lblTMSURL->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtTMSURL, UI::GUITextBox(ui, this, CSTR("")));
	this->txtTMSURL->SetRect(104, 4, 500, 23, false);
	NEW_CLASS(this->btnLoad, UI::GUIButton(ui, this, CSTR("Load")));
	this->btnLoad->SetRect(104, 28, 75, 23, false);
	this->btnLoad->HandleButtonClick(OnLoadClicked, this);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, CSTR("Status")));
	this->lblStatus->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, CSTR("")));
	this->txtStatus->SetRect(104, 52, 100, 23, false);
	this->txtStatus->SetReadOnly(true);
}

SSWR::AVIRead::AVIRTMSForm::~AVIRTMSForm()
{
	SDEL_CLASS(this->tms);
}

void SSWR::AVIRead::AVIRTMSForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}