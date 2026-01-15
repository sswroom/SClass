#include "Stdafx.h"
#include "IO/FileAnalyse/SSLFileAnalyse.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "Net/SSLEngineFactory.h"
#include "Net/SSLUtil.h"
#include "SSWR/AVIRead/AVIRSSLInfoForm.h"
#include "Sync/ThreadUtil.h"

void __stdcall SSWR::AVIRead::AVIRSSLInfoForm::OnCheckClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSSLInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSSLInfoForm>();
	Text::StringBuilderUTF8 sb;
	Text::PString hostName;
	NN<Text::String> sslHost;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	me->txtHost->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->txtStatus->SetText(CSTR("Please enter Host name"));
		return;
	}
	hostName = Net::SocketUtil::GetHostPort(sb, port);
	if (!me->sockf->DNSResolveIP(hostName.ToCString(), addr))
	{
		me->txtStatus->SetText(CSTR("Error in resolving host name"));
		return;
	}
	sslHost = Text::String::New(hostName.ToCString());
	if (port == 0)
	{
		sb.ClearStr();
		me->txtPort->GetText(sb);
		if (!sb.ToUInt16(port))
		{
			me->txtStatus->SetText(CSTR("Please enter valid port"));
			sslHost->Release();
			return;
		}
	}
	Net::SSLVer ver = (Net::SSLVer)me->cboVersion->GetSelectedItem().GetIntOS();
	Optional<Socket> s;
	NN<Socket> nns;
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
		sslHost->Release();
		return;
	}
	if (!s.SetTo(nns))
	{
		me->txtStatus->SetText(CSTR("Error in creating socket"));
		sslHost->Release();
		return;
	}
	if (!me->sockf->Connect(nns, addr, port, 30000))
	{
		me->sockf->DestroySocket(nns);
		sslHost->Release();
		me->txtStatus->SetText(CSTR("Error in connecting to remote host"));
		return;
	}
	UIntOS mode = me->cboMode->GetSelectedIndex();
	UnsafeArray<UInt8> nnpacketBuff;
	UInt8 packetBuff[16384];
	UIntOS retSize;
	UIntOS readSize;
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
		me->sockf->SetRecvTimeout(nns, 5000);
		if ((retSize = me->sockf->SendData(nns, packetBuff, 58, nullptr)) != 58)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Error in sending PreLogin packet: size = "));
			sb.AppendUIntOS(retSize);
			me->txtStatus->SetText(sb.ToCString());
			me->sockf->DestroySocket(nns);
			sslHost->Release();
			return;
		}
		retSize = me->sockf->ReceiveData(nns, packetBuff, sizeof(packetBuff), nullptr);
		if (retSize == 0)
		{
			me->sockf->DestroySocket(nns);
			me->txtStatus->SetText(CSTR("Server does not have valid reply after PreLogin"));
			sslHost->Release();
			return;
		}
		packetBuff[0] = 0x12; //Packet Header Type = PreLogin
		packetBuff[1] = 1; //Status = 1
		WriteMUInt16(&packetBuff[4], 0); //SPID
		packetBuff[6] = 0; //Packet ID
		packetBuff[7] = 0; //Window
		retSize = Net::SSLUtil::GenSSLClientHello(&packetBuff[8], sslHost->ToCString(), ver);
		sslHost->Release();
		WriteMUInt16(&packetBuff[2], retSize + 8); //Packet Length
		if ((retSize = me->sockf->SendData(nns, packetBuff, retSize + 8, nullptr)) == 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Error in sending SSL handshake packet: size = "));
			sb.AppendUIntOS(retSize);
			me->txtStatus->SetText(sb.ToCString());
			me->sockf->DestroySocket(nns);
			return;
		}
		retSize = 0;
		while (true)
		{
			readSize = me->sockf->ReceiveData(nns, &packetBuff[retSize], sizeof(packetBuff) - retSize, nullptr);
			if (readSize == 0)
			{
				me->sockf->DestroySocket(nns);
				me->txtStatus->SetText(CSTR("Server does not have valid reply after SSL handshake"));
				return;
			}
			retSize += readSize;
			if (packetBuff[0] != 0x12 || packetBuff[1] != 1)
				break;
			if (ReadMUInt16(&packetBuff[2]) <= retSize)
			{
				break;
			}
		}

		me->sockf->DestroySocket(nns);
		if (packetBuff[0] != 0x12 || packetBuff[1] != 1)
		{
			me->txtStatus->SetText(CSTR("Server does not response valid SSL handshake packet"));
		}
		else
		{
			me->currCerts.Delete();
			Text::StringBuilderUTF8 sb;
			Net::SSLUtil::ParseResponse(&packetBuff[8], retSize - 8, sb, me->currCerts);
			if (me->packetBuff.SetTo(nnpacketBuff))
			{
				MemFreeArr(nnpacketBuff);
			}
			me->packetBuff = nnpacketBuff = MemAllocArr(UInt8, retSize - 8);
			MemCopyNO(nnpacketBuff.Ptr(), &packetBuff[8], retSize - 8);
			me->packetSize = retSize - 8;
			me->txtStatus->SetText(sb.ToCString());
			NN<Crypto::Cert::X509File> cert;
			if (me->currCerts.SetTo(cert))
			{
				Text::StringBuilderUTF8 sb;
				cert->ToString(sb);
				me->txtCert->SetText(sb.ToCString());
			}
			return;
		}
	}
	else
	{
		retSize = Net::SSLUtil::GenSSLClientHello(packetBuff, sslHost->ToCString(), ver);
		sslHost->Release();
		if ((retSize = me->sockf->SendData(nns, packetBuff, retSize, nullptr)) == 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Error in sending SSL handshake packet: size = "));
			sb.AppendUIntOS(retSize);
			me->txtStatus->SetText(sb.ToCString());
			me->sockf->DestroySocket(nns);
			return;
		}
		retSize = 0;
		while (true)
		{
			readSize = me->sockf->ReceiveData(nns, &packetBuff[retSize], sizeof(packetBuff) - retSize, nullptr);
			if (readSize == 0)
			{
				me->sockf->DestroySocket(nns);
				me->txtStatus->SetText(CSTR("Server does not have valid reply after SSL handshake"));
				return;
			}
			retSize += readSize;
			if (!Net::SSLUtil::IncompleteHandshake(packetBuff, retSize))
				break;
		}
		
		me->sockf->DestroySocket(nns);
		me->currCerts.Delete();
		Text::StringBuilderUTF8 sb;
		Net::SSLUtil::ParseResponse(packetBuff, retSize, sb, me->currCerts);
		if (me->packetBuff.SetTo(nnpacketBuff))
		{
			MemFreeArr(nnpacketBuff);
		}
		me->packetBuff = nnpacketBuff = MemAllocArr(UInt8, retSize);
		MemCopyNO(nnpacketBuff.Ptr(), packetBuff, retSize);
		me->packetSize = retSize;
		me->txtStatus->SetText(sb.ToCString());
		NN<Crypto::Cert::X509File> cert;
		if (me->currCerts.SetTo(cert))
		{
			Text::StringBuilderUTF8 sb;
			cert->ToString(sb);
			me->txtCert->SetText(sb.ToCString());
		}
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRSSLInfoForm::OnCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSSLInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSSLInfoForm>();
	NN<Crypto::Cert::X509File> cert;
	if (me->currCerts.SetTo(cert))
	{
		me->core->OpenObject(cert->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRSSLInfoForm::OnRAWClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSSLInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSSLInfoForm>();
	UnsafeArray<UInt8> packetBuff;
	if (me->packetBuff.SetTo(packetBuff) && me->packetSize > 0)
	{
		IO::StmData::MemoryDataCopy md(Data::ByteArrayR(packetBuff, me->packetSize));
		IO::FileAnalyse::SSLFileAnalyse *fileAnalyse;
		NEW_CLASS(fileAnalyse, IO::FileAnalyse::SSLFileAnalyse(md));
		me->core->OpenHex(md, fileAnalyse);
	}
}

SSWR::AVIRead::AVIRSSLInfoForm::AVIRSSLInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("SSL Info"));
	this->SetFont(nullptr, 8.25, false);
	
	this->core = core;
	this->sockf = this->core->GetSocketFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->currCerts = nullptr;
	this->packetBuff = nullptr;
	this->packetSize = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblHost = ui->NewLabel(*this, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(*this, CSTR("127.0.0.1"));
	this->txtHost->SetRect(104, 4, 200, 23, false);
	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("443"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	this->lblMode = ui->NewLabel(*this, CSTR("Mode"));
	this->lblMode->SetRect(4, 52, 100, 23, false);
	this->cboMode = ui->NewComboBox(*this, false);
	this->cboMode->SetRect(104, 52, 100, 23, false);
	this->cboMode->AddItem(CSTR("Standard"), 0);
	this->cboMode->AddItem(CSTR("MSSQL Server"), 0);
	this->cboMode->SetSelectedIndex(0);
	this->lblVersion = ui->NewLabel(*this, CSTR("Version"));
	this->lblVersion->SetRect(4, 76, 100, 23, false);
	this->cboVersion = ui->NewComboBox(*this, false);
	this->cboVersion->SetRect(104, 76, 100, 23, false);
	this->cboVersion->AddItem(CSTR("SSL 3.0"), (void*)Net::SSLVer::SSL3_0);
	this->cboVersion->AddItem(CSTR("TLS 1.0"), (void*)Net::SSLVer::TLS1_0);
	this->cboVersion->AddItem(CSTR("TLS 1.1"), (void*)Net::SSLVer::TLS1_1);
	this->cboVersion->AddItem(CSTR("TLS 1.2"), (void*)Net::SSLVer::TLS1_2);
	this->cboVersion->AddItem(CSTR("TLS 1.3"), (void*)Net::SSLVer::TLS1_3);
	this->cboVersion->SetSelectedIndex(3);
	this->btnCheck = ui->NewButton(*this, CSTR("Check"));
	this->btnCheck->SetRect(204, 76, 75, 23, false);
	this->btnCheck->HandleButtonClick(OnCheckClicked, this);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(4, 124, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetRect(104, 124, 400, 23, false);
	this->txtStatus->SetReadOnly(true);
	this->lblCert = ui->NewLabel(*this, CSTR("Cert"));
	this->lblCert->SetRect(4, 148, 100, 23, false);
	this->txtCert = ui->NewTextBox(*this, CSTR(""), true);
	this->txtCert->SetRect(104, 148, 150, 144, false);
	this->txtCert->SetReadOnly(true);
	this->btnCert = ui->NewButton(*this, CSTR("View Cert"));
	this->btnCert->SetRect(254, 148, 75, 23, false);
	this->btnCert->HandleButtonClick(OnCertClicked, this);
	this->btnRAW = ui->NewButton(*this, CSTR("View RAW"));
	this->btnRAW->SetRect(334, 148, 75, 23, false);
	this->btnRAW->HandleButtonClick(OnRAWClicked, this);
}

SSWR::AVIRead::AVIRSSLInfoForm::~AVIRSSLInfoForm()
{
	UnsafeArray<UInt8> packetBuff;
	this->ssl.Delete();
	this->currCerts.Delete();
	if (this->packetBuff.SetTo(packetBuff))
	{
		MemFreeArr(packetBuff);
	}
}

void SSWR::AVIRead::AVIRSSLInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
