#include "Stdafx.h"
#include "Net/NTPClient.h"
#include "SSWR/AVIRead/AVIRNTPClientForm.h"

void __stdcall SSWR::AVIRead::AVIRNTPClientForm::OnGetTimeClicked(void *userObj)
{
	SSWR::AVIRead::AVIRNTPClientForm *me = (SSWR::AVIRead::AVIRNTPClientForm*)userObj;
	Net::NTPClient *cli;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	me->cboServer->GetText(sb);
	if (sb.GetLength() > 0)
	{
		Data::Timestamp ts;
		UInt16 destPort = Net::NTPClient::GetDefaultPort();
		UOSInt i;
		NEW_CLASS(cli, Net::NTPClient(me->core->GetSocketFactory(), 0, me->core->GetLog()));
		i = sb.IndexOf(':');
		if (i != INVALID_INDEX)
		{
			destPort = (UInt16)(Text::StrToInt32(&sb.ToString()[i + 1]) & 0xffff);
			if (destPort == 0)
			{
			}
			sb.TrimToLength((UOSInt)i);
		}
		if (cli->GetServerTime(sb.ToCString(), destPort, ts))
		{
			sptr = ts.ToLocalTime().ToString(sbuff);
			me->txtServerTime->SetText(CSTRP(sbuff, sptr));
			me->txtStatus->SetText(CSTR("Time received from server"));
		}
		else
		{
			me->txtStatus->SetText(CSTR("Error in getting server time"));
		}
		DEL_CLASS(cli);
	}
}

void __stdcall SSWR::AVIRead::AVIRNTPClientForm::OnSyncTimeClicked(void *userObj)
{
	SSWR::AVIRead::AVIRNTPClientForm *me = (SSWR::AVIRead::AVIRNTPClientForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Net::NTPClient *cli;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	me->cboServer->GetText(sb);
	if (sb.GetLength() > 0)
	{
		Data::Timestamp ts;
		UInt16 destPort = Net::NTPClient::GetDefaultPort();
		UOSInt i;
		NEW_CLASS(cli, Net::NTPClient(me->core->GetSocketFactory(), 0, me->core->GetLog()));
		i = sb.IndexOf(':');
		if (i != INVALID_INDEX)
		{
			destPort = (UInt16)(Text::StrToInt32(&sb.ToString()[i + 1]) & 0xffff);
			if (destPort == 0)
			{
			}
			sb.TrimToLength((UOSInt)i);
		}
		if (cli->GetServerTime(sb.ToCString(), destPort, ts))
		{
			if (ts.SetAsComputerTime())
			{
				me->txtStatus->SetText(CSTR("Time is sync"));
			}
			else
			{
				me->txtStatus->SetText(CSTR("Error in setting as computer time"));
			}
			sptr = ts.ToLocalTime().ToString(sbuff);
			me->txtServerTime->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			me->txtStatus->SetText(CSTR("Error in getting server time"));
		}
		DEL_CLASS(cli);
	}
}

SSWR::AVIRead::AVIRNTPClientForm::AVIRNTPClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 480, 160, ui)
{
	this->SetText(CSTR("NTP Client"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblServer, UI::GUILabel(ui, this, CSTR("Server")));
	this->lblServer->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboServer, UI::GUIComboBox(ui, this, true));
	this->cboServer->SetRect(104, 4, 300, 23, false);
	this->cboServer->AddItem(CSTR("stdtime.gov.hk"), 0);
	this->cboServer->AddItem(CSTR("tock.stdtime.gov.tw"), 0);
	this->cboServer->AddItem(CSTR("watch.stdtime.gov.tw"), 0);
	this->cboServer->AddItem(CSTR("time.stdtime.gov.tw"), 0);
	this->cboServer->AddItem(CSTR("clock.stdtime.gov.tw"), 0);
	this->cboServer->AddItem(CSTR("tick.stdtime.gov.tw"), 0);
	this->cboServer->AddItem(CSTR("2.hk.pool.ntp.org"), 0);
	this->cboServer->AddItem(CSTR("1.asia.pool.ntp.org"), 0);
	this->cboServer->AddItem(CSTR("2.asia.pool.ntp.org"), 0);
	this->cboServer->AddItem(CSTR("0.pool.ntp.org"), 0);
	this->cboServer->AddItem(CSTR("1.pool.ntp.org"), 0);
	this->cboServer->AddItem(CSTR("2.pool.ntp.org"), 0);
	this->cboServer->AddItem(CSTR("3.pool.ntp.org"), 0);
	NEW_CLASS(this->lblServerTime, UI::GUILabel(ui, this, CSTR("Server Time")));
	this->lblServerTime->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtServerTime, UI::GUITextBox(ui, this, CSTR(""), false));
	this->txtServerTime->SetRect(104, 28, 200, 23, false);
	this->txtServerTime->SetReadOnly(true);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, CSTR("Status")));
	this->lblStatus->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, CSTR(""), false));
	this->txtStatus->SetRect(104, 52, 200, 23, false);
	this->txtStatus->SetReadOnly(true);
	NEW_CLASS(this->btnGetTime, UI::GUIButton(ui, this, CSTR("&Get Time")));
	this->btnGetTime->SetRect(104, 76, 100, 23, false);
	this->btnGetTime->HandleButtonClick(OnGetTimeClicked, this);
	NEW_CLASS(this->btnSyncTime, UI::GUIButton(ui, this, CSTR("&Sync Time")));
	this->btnSyncTime->SetRect(214, 76, 100, 23, false);
	this->btnSyncTime->HandleButtonClick(OnSyncTimeClicked, this);
	this->cboServer->SetSelectedIndex(0);
}

SSWR::AVIRead::AVIRNTPClientForm::~AVIRNTPClientForm()
{
}

void SSWR::AVIRead::AVIRNTPClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
