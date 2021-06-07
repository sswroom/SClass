#include "Stdafx.h"
#include "Net/NTPClient.h"
#include "SSWR/AVIRead/AVIRNTPClientForm.h"

void __stdcall SSWR::AVIRead::AVIRNTPClientForm::OnGetTimeClicked(void *userObj)
{
	SSWR::AVIRead::AVIRNTPClientForm *me = (SSWR::AVIRead::AVIRNTPClientForm*)userObj;
	Net::NTPClient *cli;
	UTF8Char sbuff[64];
	Text::StringBuilderUTF8 sb;
	me->cboServer->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		Data::DateTime dt;
		UInt16 destPort = Net::NTPClient::GetDefaultPort();
		OSInt i;
		NEW_CLASS(cli, Net::NTPClient(me->core->GetSocketFactory(), 0));
		i = sb.IndexOf((const UTF8Char*)":");
		if (i >= 0)
		{
			destPort = (UInt16)(Text::StrToInt32(&sb.ToString()[i + 1]) & 0xffff);
			if (destPort == 0)
			{
			}
			sb.TrimToLength((UOSInt)i);
		}
		if (cli->GetServerTime(sb.ToString(), destPort, &dt))
		{
			dt.ToLocalTime();
			dt.ToString(sbuff);
			me->txtServerTime->SetText(sbuff);
			me->txtStatus->SetText((const UTF8Char*)"Time received from server");
		}
		else
		{
			me->txtStatus->SetText((const UTF8Char*)"Error in getting server time");
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
	me->cboServer->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		Data::DateTime dt;
		UInt16 destPort = Net::NTPClient::GetDefaultPort();
		OSInt i;
		NEW_CLASS(cli, Net::NTPClient(me->core->GetSocketFactory(), 0));
		i = sb.IndexOf((const UTF8Char*)":");
		if (i >= 0)
		{
			destPort = (UInt16)(Text::StrToInt32(&sb.ToString()[i + 1]) & 0xffff);
			if (destPort == 0)
			{
			}
			sb.TrimToLength((UOSInt)i);
		}
		if (cli->GetServerTime(sb.ToString(), destPort, &dt))
		{
			if (dt.SetAsComputerTime())
			{
				me->txtStatus->SetText((const UTF8Char*)"Time is sync");
			}
			else
			{
				me->txtStatus->SetText((const UTF8Char*)"Error in setting as computer time");
			}
			dt.ToLocalTime();
			dt.ToString(sbuff);
			me->txtServerTime->SetText(sbuff);
		}
		else
		{
			me->txtStatus->SetText((const UTF8Char*)"Error in getting server time");
		}
		DEL_CLASS(cli);
	}
}

SSWR::AVIRead::AVIRNTPClientForm::AVIRNTPClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 480, 160, ui)
{
	this->SetText((const UTF8Char*)"NTP Client");
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblServer, UI::GUILabel(ui, this, (const UTF8Char*)"Server"));
	this->lblServer->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboServer, UI::GUIComboBox(ui, this, true));
	this->cboServer->SetRect(104, 4, 300, 23, false);
	this->cboServer->AddItem((const UTF8Char*)"stdtime.gov.hk", 0);
	this->cboServer->AddItem((const UTF8Char*)"tock.stdtime.gov.tw", 0);
	this->cboServer->AddItem((const UTF8Char*)"watch.stdtime.gov.tw", 0);
	this->cboServer->AddItem((const UTF8Char*)"time.stdtime.gov.tw", 0);
	this->cboServer->AddItem((const UTF8Char*)"clock.stdtime.gov.tw", 0);
	this->cboServer->AddItem((const UTF8Char*)"tick.stdtime.gov.tw", 0);
	this->cboServer->AddItem((const UTF8Char*)"2.hk.pool.ntp.org", 0);
	this->cboServer->AddItem((const UTF8Char*)"1.asia.pool.ntp.org", 0);
	this->cboServer->AddItem((const UTF8Char*)"2.asia.pool.ntp.org", 0);
	this->cboServer->AddItem((const UTF8Char*)"0.pool.ntp.org", 0);
	this->cboServer->AddItem((const UTF8Char*)"1.pool.ntp.org", 0);
	this->cboServer->AddItem((const UTF8Char*)"2.pool.ntp.org", 0);
	this->cboServer->AddItem((const UTF8Char*)"3.pool.ntp.org", 0);
	NEW_CLASS(this->lblServerTime, UI::GUILabel(ui, this, (const UTF8Char*)"Server Time"));
	this->lblServerTime->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtServerTime, UI::GUITextBox(ui, this, (const UTF8Char*)"", false));
	this->txtServerTime->SetRect(104, 28, 200, 23, false);
	this->txtServerTime->SetReadOnly(true);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, (const UTF8Char*)"Status"));
	this->lblStatus->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, (const UTF8Char*)"", false));
	this->txtStatus->SetRect(104, 52, 200, 23, false);
	this->txtStatus->SetReadOnly(true);
	NEW_CLASS(this->btnGetTime, UI::GUIButton(ui, this, (const UTF8Char*)"&Get Time"));
	this->btnGetTime->SetRect(104, 76, 100, 23, false);
	this->btnGetTime->HandleButtonClick(OnGetTimeClicked, this);
	NEW_CLASS(this->btnSyncTime, UI::GUIButton(ui, this, (const UTF8Char*)"&Sync Time"));
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
