#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/SSLUtil.h"
#include "SSWR/AVIRead/AVIRSSLInfoForm.h"
#include "Sync/ThreadUtil.h"

void __stdcall SSWR::AVIRead::AVIRSSLInfoForm::OnCheckClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSSLInfoForm *me = (SSWR::AVIRead::AVIRSSLInfoForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UInt16 port;
	me->txtPort->GetText(sb);
	if (!sb.ToUInt16(port))
	{
		me->txtStatus->SetText(CSTR("Please enter valid port"));
		return;
	}
	sb.ClearStr();
	me->txtHost->GetText(sb);
	Net::SocketUtil::AddressInfo addr;
	if (sb.GetLength() == 0)
	{
		me->txtStatus->SetText(CSTR("Please enter Host name"));
		return;
	}
	else if (!me->sockf->DNSResolveIP(sb.ToCString(), addr))
	{
		me->txtStatus->SetText(CSTR("Error in resolving host name"));
		return;
	}

	Socket *s;
	if (addr.addrType == Net::AddrType::IPv4)
	{
		s = me->sockf->CreateTCPSocketv4();
	}
	else if (addr.addrType == Net::AddrType::IPv6)
	{
		s = me->sockf->CreateTCPSocketv6();
	}
	else
	{
		me->txtStatus->SetText(CSTR("Error in address type"));
		return;
	}
	if (s == 0)
	{
		me->txtStatus->SetText(CSTR("Error in creating socket"));
		return;
	}
	if (!me->sockf->Connect(s, addr, port, 30000))
	{
		me->sockf->DestroySocket(s);
		me->txtStatus->SetText(CSTR("Error in connecting to remote host"));
		return;
	}
	UOSInt mode = me->cboMode->GetSelectedIndex();
	UInt8 packetBuff[16384];
	UOSInt retSize;
	if (mode == 1)
	{
		packetBuff[0] = 0x12; //Packet Header Type = PreLogin
		packetBuff[1] = 1; //Status = 1
		WriteMUInt16(&packetBuff[2], 58); //Packet Length
		WriteMUInt16(&packetBuff[4], 0); //SPID
		packetBuff[6] = 0; //Packet ID
		packetBuff[7] = 0; //Window
		packetBuff[8] = 0; //PL_OPTION_TOKEN = 0 (VERSION)
		WriteMUInt16(&packetBuff[9], 26); //PL_OFFSET
		WriteMUInt16(&packetBuff[11], 6); //PL_LENGTH
		packetBuff[13] = 1; //PL_OPTION_TOKEN = 1 (ENCRYPTION)
		WriteMUInt16(&packetBuff[14], 32); //PL_OFFSET
		WriteMUInt16(&packetBuff[16], 1); //PL_LENGTH
		packetBuff[18] = 2; //PL_OPTION_TOKEN = 2 (INSTOPT)
		WriteMUInt16(&packetBuff[19], 33); //PL_OFFSET
		WriteMUInt16(&packetBuff[21], 12); //PL_LENGTH
		packetBuff[23] = 3; //PL_OPTION_TOKEN = 3 (THREADID)
		WriteMUInt16(&packetBuff[24], 45); //PL_OFFSET
		WriteMUInt16(&packetBuff[26], 4); //PL_LENGTH
		packetBuff[28] = 4; //PL_OPTION_TOKEN = 4 (MARS)
		WriteMUInt16(&packetBuff[29], 49); //PL_OFFSET
		WriteMUInt16(&packetBuff[31], 1); //PL_LENGTH
		packetBuff[33] = 0xFF; //PL_OPTION_TOKEN = 0xff (TERMINATOR)
		packetBuff[34] = 9; //0 Data
		packetBuff[35] = 0; //0 Data
		packetBuff[36] = 0; //0 Data
		packetBuff[37] = 0; //0 Data
		packetBuff[38] = 0; //0 Data
		packetBuff[39] = 0; //0 Data
		packetBuff[40] = 0; //1 Data
		Text::StrConcatC(&packetBuff[41], UTF8STRC("MSSQLServer")); //2 Data 
		WriteUInt32(&packetBuff[53], Sync::ThreadUtil::GetThreadId());
		packetBuff[57] = 0;
		me->sockf->SetRecvTimeout(s, 5000);
		if ((retSize = me->sockf->SendData(s, packetBuff, 58, nullptr)) != 58)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Error in sending PreLogin packet: size = "));
			sb.AppendUOSInt(retSize);
			me->txtStatus->SetText(sb.ToCString());
			me->sockf->DestroySocket(s);
			return;
		}
		retSize = me->sockf->ReceiveData(s, packetBuff, sizeof(packetBuff), nullptr);
		if (retSize == 0)
		{
			me->sockf->DestroySocket(s);
			me->txtStatus->SetText(CSTR("Server does not have valid reply after PreLogin"));
			return;
		}
		packetBuff[0] = 0x12; //Packet Header Type = PreLogin
		packetBuff[1] = 1; //Status = 1
		WriteMUInt16(&packetBuff[4], 0); //SPID
		packetBuff[6] = 0; //Packet ID
		packetBuff[7] = 0; //Window
		retSize = Net::SSLUtil::GenSSLClientHello(&packetBuff[8], sb.ToCString());
		WriteMUInt16(&packetBuff[2], retSize + 8); //Packet Length
		if ((retSize = me->sockf->SendData(s, packetBuff, retSize + 8, nullptr)) == 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Error in sending SSL handshake packet: size = "));
			sb.AppendUOSInt(retSize);
			me->txtStatus->SetText(sb.ToCString());
			me->sockf->DestroySocket(s);
			return;
		}
		retSize = me->sockf->ReceiveData(s, packetBuff, sizeof(packetBuff), nullptr);
		if (retSize == 0)
		{
			me->sockf->DestroySocket(s);
			me->txtStatus->SetText(CSTR("Server does not have valid reply after SSL handshake"));
			return;
		}

		me->sockf->DestroySocket(s);
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Received after SSL handshake packet: size = "));
		sb.AppendUOSInt(retSize);
		me->txtStatus->SetText(sb.ToCString());
		return;
	}
	else if (mode == 0)
	{
		retSize = Net::SSLUtil::GenSSLClientHello(packetBuff, sb.ToCString());
		if ((retSize = me->sockf->SendData(s, packetBuff, retSize, nullptr)) == 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Error in sending SSL handshake packet: size = "));
			sb.AppendUOSInt(retSize);
			me->txtStatus->SetText(sb.ToCString());
			me->sockf->DestroySocket(s);
			return;
		}
		retSize = me->sockf->ReceiveData(s, packetBuff, sizeof(packetBuff), nullptr);
		if (retSize == 0)
		{
			me->sockf->DestroySocket(s);
			me->txtStatus->SetText(CSTR("Server does not have valid reply after SSL handshake"));
			return;
		}
		me->sockf->DestroySocket(s);
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Received after SSL handshake packet: size = "));
		sb.AppendUOSInt(retSize);
		me->txtStatus->SetText(sb.ToCString());
		return;
	}
	Net::SSLEngine::ErrorType err;
	Net::SSLClient *cli = me->ssl->ClientInit(s, sb.ToCString(), err);
	if (cli == 0)
	{
		me->sockf->DestroySocket(s);
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Error in initializing SSL: "));
		sb.Append(Net::SSLEngine::ErrorTypeGetName(err));
		me->txtStatus->SetText(sb.ToCString());
		return;
	}
	SDEL_CLASS(me->currCerts);
	NotNullPtr<const Data::ReadingList<Crypto::Cert::Certificate *>> certs;
	if (certs.Set(cli->GetRemoteCerts()))
	{
		me->currCerts = Crypto::Cert::X509File::CreateFromCerts(certs);
		if (me->currCerts)
		{
			Text::StringBuilderUTF8 sb;
			me->currCerts->ToString(sb);
			me->txtCert->SetText(sb.ToCString());
		}
	}
	else
	{
		me->txtCert->SetText(CSTR(""));
	}
	DEL_CLASS(cli);
	me->txtStatus->SetText(CSTR("Success"));
}

void __stdcall SSWR::AVIRead::AVIRSSLInfoForm::OnCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSSLInfoForm *me = (SSWR::AVIRead::AVIRSSLInfoForm *)userObj;
	if (me->currCerts)
	{
		me->core->OpenObject(me->currCerts->Clone().Ptr());
	}
}

SSWR::AVIRead::AVIRSSLInfoForm::AVIRSSLInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("SSL Info"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->sockf = this->core->GetSocketFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, true);
	this->currCerts = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this, CSTR("Host")));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this, CSTR("127.0.0.1")));
	this->txtHost->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, CSTR("Port")));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, CSTR("443")));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblMode, UI::GUILabel(ui, this, CSTR("Port")));
	this->lblMode->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->cboMode, UI::GUIComboBox(ui, this, false));
	this->cboMode->SetRect(104, 52, 100, 23, false);
	this->cboMode->AddItem(CSTR("Standard"), 0);
	this->cboMode->AddItem(CSTR("MSSQL Server"), 0);
	this->cboMode->SetSelectedIndex(0);
	NEW_CLASS(this->btnCheck, UI::GUIButton(ui, this, CSTR("Check")));
	this->btnCheck->SetRect(204, 52, 75, 23, false);
	this->btnCheck->HandleButtonClick(OnCheckClicked, this);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, CSTR("Status")));
	this->lblStatus->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, CSTR("")));
	this->txtStatus->SetRect(104, 100, 400, 23, false);
	this->txtStatus->SetReadOnly(true);
	NEW_CLASS(this->lblCert, UI::GUILabel(ui, this, CSTR("Cert")));
	this->lblCert->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtCert, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtCert->SetRect(104, 124, 150, 144, false);
	this->txtCert->SetReadOnly(true);
	NEW_CLASS(this->btnCert, UI::GUIButton(ui, this, CSTR("View")));
	this->btnCert->SetRect(254, 124, 75, 23, false);
	this->btnCert->HandleButtonClick(OnCertClicked, this);
}

SSWR::AVIRead::AVIRSSLInfoForm::~AVIRSSLInfoForm()
{
	SDEL_CLASS(this->ssl);
	SDEL_CLASS(this->currCerts);
}

void SSWR::AVIRead::AVIRSSLInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
