#include "Stdafx.h"
#include "Net/ConnectionInfo.h"
#include "Net/WOLClient.h"
#include "SSWR/AVIRead/AVIRWOLForm.h"

void __stdcall SSWR::AVIRead::AVIRWOLForm::OnSendClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWOLForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWOLForm>();
	UInt8 macBuff[16];
	Net::WOLClient *cli;
	Text::StringBuilderUTF8 sb;
	UInt32 ip = (UInt32)me->cboAdapter->GetSelectedItem().GetUOSInt();
	if (ip == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please select an adapter"), CSTR("WOL"), me);
		return;
	}
	me->txtDeviceMac->GetText(sb);
	if ((sb.GetLength() == 12 && Text::StrHex2BytesS(sb.ToString(), macBuff, 0) == 6) || (sb.GetLength() == 17 && Text::StrHex2BytesS(sb.ToString(), macBuff, sb.v[2]) == 6))
	{
		NEW_CLASS(cli, Net::WOLClient(me->core->GetSocketFactory(), ip, me->core->GetLog()));
		if (cli->IsError())
		{
			me->ui->ShowMsgOK(CSTR("Error in listening to the port"), CSTR("WOL"), me);
		}
		else if (!cli->WakeDevice(macBuff))
		{
			me->ui->ShowMsgOK(CSTR("Error sending the packet"), CSTR("WOL"), me);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Packet sent"), CSTR("WOL"), me);
		}
		DEL_CLASS(cli);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Please select valid device MAC"), CSTR("WOL"), me);
		return;
	}
}

SSWR::AVIRead::AVIRWOLForm::AVIRWOLForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 400, 120, ui)
{
	this->SetText(CSTR("Wake-On-LAN"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblAdapter = ui->NewLabel(*this, CSTR("Adapter"));
	this->lblAdapter->SetRect(4, 4, 100, 23, false);
	this->cboAdapter = ui->NewComboBox(*this, false);
	this->cboAdapter->SetRect(104, 4, 300, 23, false);
	this->lblDeviceMac = ui->NewLabel(*this, CSTR("MAC Address"));
	this->lblDeviceMac->SetRect(4, 28, 100, 23, false);
	this->txtDeviceMac = ui->NewTextBox(*this, CSTR(""), false);
	this->txtDeviceMac->SetRect(104, 28, 200, 23, false);
	this->btnSend = ui->NewButton(*this, CSTR("&Send"));
	this->btnSend->SetRect(104, 52, 100, 23, false);
	this->btnSend->HandleButtonClick(OnSendClicked, this);

	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NN<Net::ConnectionInfo> connInfo;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 ip;
	this->core->GetSocketFactory()->GetConnInfoList(connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItemNoCheck(i);
		if (connInfo->GetConnectionStatus() == Net::ConnectionInfo::CS_UP && connInfo->GetConnectionType() != Net::ConnectionInfo::ConnectionType::Loopback)
		{
			k = 0;
			while (true)
			{
				ip = connInfo->GetIPAddress(k);
				if (ip == 0)
					break;
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
				this->cboAdapter->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)ip);
				k++;
			}
		}
		connInfo.Delete();
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
