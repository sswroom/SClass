#include "Stdafx.h"
#include "Net/SSHConn.h"
#include "Net/SSLEngineFactory.h"
#include "Net/SSLUtil.h"
#include "SSWR/AVIRead/AVIRSSHInfoForm.h"
#include "Sync/ThreadUtil.h"

void __stdcall SSWR::AVIRead::AVIRSSHInfoForm::OnQueryClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRSSHInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSSHInfoForm>();
	Text::StringBuilderUTF8 sbHost;
	Text::StringBuilderUTF8 sbTmp;
	Text::PString hostName;
	UInt16 port;
	me->txtHost->GetText(sbHost);
	if (sbHost.GetLength() == 0)
	{
		me->txtStatus->SetText(CSTR("Please enter Host name"));
		return;
	}
	hostName = Net::SocketUtil::GetHostPort(sbHost, port);
	if (port == 0)
	{
		me->txtPort->GetText(sbTmp);
		if (!sbTmp.ToUInt16(port))
		{
			me->txtStatus->SetText(CSTR("Please enter valid port"));
			return;
		}
	}
	sbTmp.ClearStr();
	me->txtUserName->GetText(sbTmp);
	Data::ArrayListStringNN authMeths;
	Net::SSHConn ssh(me->sockf, hostName.ToCString(), port, 5000);
	UInt8 hostKey[20];
	me->txtHostKey->SetText(CSTR(""));
	me->txtBanner->SetText(CSTR(""));
	me->lbAuthMeth->ClearItems();
	if (ssh.IsError())
	{
		me->txtStatus->SetText(CSTR("Error in communicating with server"));
	}
	else if (!ssh.GetHostKeySHA1(hostKey))
	{
		me->txtStatus->SetText(CSTR("Error in getting host key"));
	}
	else if (sbTmp.leng > 0 && !ssh.GetAuthMethods(sbTmp.ToCString(), authMeths))
	{
		me->txtStatus->SetText(CSTR("Error in getting auth methods"));
	}
	else
	{
		me->txtStatus->SetText(CSTR("Success"));
		sbHost.ClearStr();
		sbHost.AppendHexBuff(BYTEARR(hostKey), ':', Text::LineBreakType::None);	
		me->txtHostKey->SetText(sbHost.ToCString());
		const UTF8Char *csptr = ssh.GetBanner();
		if (csptr)
		{
			me->txtBanner->SetText(Text::CStringNN::FromPtr(csptr));
		}
		Data::ArrayIterator<NotNullPtr<Text::String>> it = authMeths.Iterator();
		while (it.HasNext())
		{
			me->lbAuthMeth->AddItem(it.Next()->ToCString(), 0);
		}
		sbTmp.ClearStr();
		sbTmp.Append(CSTR("KeyExchange: "));
		if ((csptr = ssh.GetActiveAlgorithm(Net::SSHMethodType::KeyExchange)) != 0) sbTmp.AppendSlow(csptr);
		sbTmp.Append(CSTR("\r\nHostKey: "));
		if ((csptr = ssh.GetActiveAlgorithm(Net::SSHMethodType::HostKey)) != 0) sbTmp.AppendSlow(csptr);
		sbTmp.Append(CSTR("\r\nCryptoClientToServer: "));
		if ((csptr = ssh.GetActiveAlgorithm(Net::SSHMethodType::CryptoClientToServer)) != 0) sbTmp.AppendSlow(csptr);
		sbTmp.Append(CSTR("\r\nCryptoServerToClient: "));
		if ((csptr = ssh.GetActiveAlgorithm(Net::SSHMethodType::CryptoServerToClient)) != 0) sbTmp.AppendSlow(csptr);
		sbTmp.Append(CSTR("\r\nMACClientToServer: "));
		if ((csptr = ssh.GetActiveAlgorithm(Net::SSHMethodType::MACClientToServer)) != 0) sbTmp.AppendSlow(csptr);
		sbTmp.Append(CSTR("\r\nMACServerToClient: "));
		if ((csptr = ssh.GetActiveAlgorithm(Net::SSHMethodType::MACServerToClient)) != 0) sbTmp.AppendSlow(csptr);
		sbTmp.Append(CSTR("\r\nCompressClientToServer: "));
		if ((csptr = ssh.GetActiveAlgorithm(Net::SSHMethodType::CompressClientToServer)) != 0) sbTmp.AppendSlow(csptr);
		sbTmp.Append(CSTR("\r\nCompressServerToClient: "));
		if ((csptr = ssh.GetActiveAlgorithm(Net::SSHMethodType::CompressServerToClient)) != 0) sbTmp.AppendSlow(csptr);
		sbTmp.Append(CSTR("\r\nLangClientToServer: "));
		if ((csptr = ssh.GetActiveAlgorithm(Net::SSHMethodType::LangClientToServer)) != 0) sbTmp.AppendSlow(csptr);
		sbTmp.Append(CSTR("\r\nLangServerToClient: "));
		if ((csptr = ssh.GetActiveAlgorithm(Net::SSHMethodType::LangServerToClient)) != 0) sbTmp.AppendSlow(csptr);
		sbTmp.Append(CSTR("\r\nSignatureAlgorithm: "));
		if ((csptr = ssh.GetActiveAlgorithm(Net::SSHMethodType::SignatureAlgorithm)) != 0) sbTmp.AppendSlow(csptr);
		me->txtDesc->SetText(sbTmp.ToCString());
	}
	authMeths.FreeAll();
}

SSWR::AVIRead::AVIRSSHInfoForm::AVIRSSHInfoForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("SSH Info"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->sockf = this->core->GetSocketFactory();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblHost = ui->NewLabel(*this, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(*this, CSTR("127.0.0.1"));
	this->txtHost->SetRect(104, 4, 200, 23, false);
	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("22"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	this->lblUserName = ui->NewLabel(*this, CSTR("User Name"));
	this->lblUserName->SetRect(4, 52, 100, 23, false);
	this->txtUserName = ui->NewTextBox(*this, CSTR(""));
	this->txtUserName->SetRect(104, 52, 200, 23, false);
	this->btnQuery = ui->NewButton(*this, CSTR("Query"));
	this->btnQuery->SetRect(104, 76, 75, 23, false);
	this->btnQuery->HandleButtonClick(OnQueryClicked, this);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(4, 100, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetRect(104, 100, 400, 23, false);
	this->txtStatus->SetReadOnly(true);
	this->lblHostKey = ui->NewLabel(*this, CSTR("Host Key"));
	this->lblHostKey->SetRect(4, 124, 100, 23, false);
	this->txtHostKey = ui->NewTextBox(*this, CSTR(""));
	this->txtHostKey->SetRect(104, 124, 400, 23, false);
	this->txtHostKey->SetReadOnly(true);
	this->lblBanner = ui->NewLabel(*this, CSTR("Banner"));
	this->lblBanner->SetRect(4, 148, 100, 23, false);
	this->txtBanner = ui->NewTextBox(*this, CSTR(""));
	this->txtBanner->SetRect(104, 148, 400, 23, false);
	this->txtBanner->SetReadOnly(true);
	this->lblAuthMeth = ui->NewLabel(*this, CSTR("Auth Methods"));
	this->lblAuthMeth->SetRect(4, 172, 100, 23, false);
	this->lbAuthMeth = ui->NewListBox(*this, false);
	this->lbAuthMeth->SetRect(104, 172, 300, 71, false);
	this->txtDesc = ui->NewTextBox(*this, CSTR(""), true);
	this->txtDesc->SetRect(4, 244, 500, 168, false);
	this->txtDesc->SetReadOnly(true);
}

SSWR::AVIRead::AVIRSSHInfoForm::~AVIRSSHInfoForm()
{
}

void SSWR::AVIRead::AVIRSSHInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
