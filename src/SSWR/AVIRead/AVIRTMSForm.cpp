#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRTMSForm.h"

void __stdcall SSWR::AVIRead::AVIRTMSForm::OnLoadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTMSForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTMSForm>();
	NN<Map::TileMapServiceSource> tms;
	Text::StringBuilderUTF8 sb;
	me->txtTMSURL->GetText(sb);
	me->tms.Delete();
	NEW_CLASSNN(tms, Map::TileMapServiceSource(me->core->GetTCPClientFactory(), me->ssl, me->core->GetEncFactory(), sb.ToCString()));
	me->tms = tms;
	if (tms->IsError())
	{
		me->txtStatus->SetText(CSTR("Error"));
	}
	else
	{
		me->txtStatus->SetText(CSTR("Success"));
	}
}

void __stdcall SSWR::AVIRead::AVIRTMSForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTMSForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTMSForm>();
	NN<Map::TileMapServiceSource> tms;
	if (me->tms.SetTo(tms) && !tms->IsError())
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

SSWR::AVIRead::AVIRTMSForm::AVIRTMSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl) : UI::GUIForm(parent, 640, 120, ui)
{
	this->core = core;
	this->ssl = ssl;
	this->tms = 0;
	this->SetText(CSTR("Tile Map Service"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblTMSURL = ui->NewLabel(*this, CSTR("TMS URL"));
	this->lblTMSURL->SetRect(4, 4, 100, 23, false);
	this->txtTMSURL = ui->NewTextBox(*this, CSTR(""));
	this->txtTMSURL->SetRect(104, 4, 500, 23, false);
	this->btnLoad = ui->NewButton(*this, CSTR("Load"));
	this->btnLoad->SetRect(104, 28, 75, 23, false);
	this->btnLoad->HandleButtonClick(OnLoadClicked, this);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(4, 52, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetRect(104, 52, 100, 23, false);
	this->txtStatus->SetReadOnly(true);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(104, 76, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
}

SSWR::AVIRead::AVIRTMSForm::~AVIRTMSForm()
{
	this->tms.Delete();
}

void SSWR::AVIRead::AVIRTMSForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<Map::TileMap> SSWR::AVIRead::AVIRTMSForm::GetTileMap()
{
	Optional<Map::TileMap> tile = this->tms;
	this->tms = 0;
	return tile;
}
