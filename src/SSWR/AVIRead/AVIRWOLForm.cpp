#include "Stdafx.h"
#include "Net/ConnectionInfo.h"
#include "Net/WOLClient.h"
#include "SSWR/AVIRead/AVIRWOLForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRWOLForm::OnSendClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWOLForm *me = (SSWR::AVIRead::AVIRWOLForm*)userObj;
	UInt8 macBuff[16];
	Net::WOLClient *cli;
	Text::StringBuilderUTF8 sb;
	UInt32 ip = (UInt32)(OSInt)me->cboAdapter->GetSelectedItem();
	if (ip == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please select an adapter", (const UTF8Char*)"WOL", me);
		return;
	}
	me->txtDeviceMac->GetText(&sb);
	if ((sb.GetLength() == 12 || sb.GetLength() == 17) && Text::StrHex2Bytes(sb.ToString(), macBuff) == 6)
	{
		NEW_CLASS(cli, Net::WOLClient(me->core->GetSocketFactory(), ip));
		if (cli->IsError())
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in listening to the port", (const UTF8Char*)"WOL", me);
		}
		else if (!cli->WakeDevice(macBuff))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error sending the packet", (const UTF8Char*)"WOL", me);
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Packet sent", (const UTF8Char*)"WOL", me);
		}
		DEL_CLASS(cli);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please select valid device MAC", (const UTF8Char*)"WOL", me);
		return;
	}
}

SSWR::AVIRead::AVIRWOLForm::AVIRWOLForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 400, 120, ui)
{
	this->SetText((const UTF8Char*)"Wake-On-LAN");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblAdapter, UI::GUILabel(ui, this, (const UTF8Char*)"Adapter"));
	this->lblAdapter->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboAdapter, UI::GUIComboBox(ui, this, false));
	this->cboAdapter->SetRect(104, 4, 300, 23, false);
	NEW_CLASS(this->lblDeviceMac, UI::GUILabel(ui, this, (const UTF8Char*)"Server Time"));
	this->lblDeviceMac->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDeviceMac, UI::GUITextBox(ui, this, (const UTF8Char*)"", false));
	this->txtDeviceMac->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->btnSend, UI::GUIButton(ui, this, (const UTF8Char*)"&Send"));
	this->btnSend->SetRect(104, 52, 100, 23, false);
	this->btnSend->HandleButtonClick(OnSendClicked, this);

	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[32];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 ip;
	this->core->GetSocketFactory()->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		if (connInfo->GetConnectionStatus() == Net::ConnectionInfo::CS_UP && connInfo->GetConnectionType() != Net::ConnectionInfo::CT_LOOPBACK)
		{
			k = 0;
			while (true)
			{
				ip = connInfo->GetIPAddress(k);
				if (ip == 0)
					break;
				Net::SocketUtil::GetIPv4Name(sbuff, ip);
				this->cboAdapter->AddItem(sbuff, (void*)(OSInt)ip);
				k++;
			}
		}
		DEL_CLASS(connInfo);
		i++;
	}
	if (this->cboAdapter->GetCount() > 0)
	{
		this->cboAdapter->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRWOLForm::~AVIRWOLForm()
{
}

void SSWR::AVIRead::AVIRWOLForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
