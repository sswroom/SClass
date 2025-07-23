#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/ColorProfile.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebSocketClient.h"
#include "SSWR/AVIRead/AVIRMQTTSubscribeTestForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeTestForm>();
	NN<Net::MQTTConn> client;
	if (me->client.NotNull())
	{
		me->ServerStop();
		me->txtHost->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		me->txtUsername->SetReadOnly(false);
		me->txtPassword->SetReadOnly(false);
		me->chkSSL->SetEnabled(true);
		me->chkWebSocket->SetEnabled(true);
		me->txtTopic->SetReadOnly(false);
		me->lblStatus->SetText(CSTR("Disconnected"));
		me->btnStart->SetText(CSTR("Start"));
	}
	else
	{
		Bool useSSL = me->chkSSL->IsChecked();
		Bool useWS = me->chkWebSocket->IsChecked();
		Text::StringBuilderUTF8 sb;
		Text::StringBuilderUTF8 sbTopic;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port;
		me->txtPort->GetText(sb);
		if (!sb.ToUInt16(port))
		{
			me->ui->ShowMsgOK(CSTR("Port is not valid"), CSTR("Error"), me);
			return;
		}
		else if (port <= 0)
		{
			me->ui->ShowMsgOK(CSTR("Port is out of range"), CSTR("Error"), me);
			return;
		}
		sb.ClearStr();
		me->txtHost->GetText(sb);
		if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToCString(), addr))
		{
			me->ui->ShowMsgOK(CSTR("Error in parsing host"), CSTR("Error"), me);
			return;
		}
		me->txtTopic->GetText(sbTopic);
		if (sbTopic.leng == 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter topic to subscribe"), CSTR("Error"), me);
			return;
		}
		Optional<Net::SSLEngine> ssl = me->ssl;
		NN<Net::SSLEngine> nnssl;
		if (useSSL && ssl.SetTo(nnssl))
		{
			NN<Crypto::Cert::X509Cert> cliCert;
			NN<Crypto::Cert::X509File> cliKey;
			if (me->cliCert.SetTo(cliCert) && me->cliKey.SetTo(cliKey))
			{
				nnssl->ClientSetCertASN1(cliCert, cliKey);
			}
		}
		if (useWS)
		{
			NN<Net::WebSocketClient> ws;
			NEW_CLASSNN(ws, Net::WebSocketClient(me->core->GetTCPClientFactory(), useSSL?ssl:0, sb.ToCString(), port, CSTR("/mqtt"), CSTR_NULL, Net::WebSocketClient::Protocol::MQTT, 10000));
			if (ws->IsDown())
			{
				ws.Delete();
				me->ui->ShowMsgOK(CSTR("Error in initializing websocket"), CSTR("Error"), me);
				return;
			}
			NEW_CLASSNN(client, Net::MQTTConn(ws, 0, 0));
		}
		else
		{
			NEW_CLASSNN(client, Net::MQTTConn(me->core->GetTCPClientFactory(), useSSL?ssl:0, sb.ToCString(), port, 0, 0, 30000));
		}
		if (client->IsError())
		{
			me->ui->ShowMsgOK(CSTR("Error in connecting to server"), CSTR("Error"), me);
			client.Delete();
			return;
		}
		me->client = client;
		client->HandlePublishMessage(OnPublishMessage, me);

		Text::CString username = CSTR_NULL;
		Text::CString password = CSTR_NULL;
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		sb.ClearStr();
		me->txtUsername->GetText(sb);
		if (sb.GetLength() > 0)
		{
			username.v = Text::StrCopyNewC(sb.ToString(), sb.GetLength()).Ptr();
			username.leng = sb.GetLength();
		}
		sb.ClearStr();
		me->txtPassword->GetText(sb);
		if (sb.GetLength() > 0)
		{
			password.v = Text::StrCopyNewC(sb.ToString(), sb.GetLength()).Ptr();
			password.leng = sb.GetLength();
		}
		sb.ClearStr();
		sb.AppendC(UTF8STRC("sswrMQTT/"));
		sb.AppendI64(dt.ToTicks());
		Bool succ = client->SendConnect(4, 30, sb.ToCString(), username, password);
		SDEL_TEXT(username.v);
		SDEL_TEXT(password.v);
		if (succ)
		{
			succ = (client->WaitConnAck(30000) == Net::MQTTConn::CS_ACCEPTED);
		}

		if (succ)
		{
			succ = false;
			me->totalCount = 0;
			if (client->SendSubscribe(1, sbTopic.ToCString()))
			{
				if (client->WaitSubAck(1, 30000) <= 2)
				{
					succ = true;
				}
			}
		}

		if (succ)
		{
			me->txtHost->SetReadOnly(true);
			me->txtPort->SetReadOnly(true);
			me->txtUsername->SetReadOnly(true);
			me->txtPassword->SetReadOnly(true);
			me->chkSSL->SetEnabled(false);
			me->chkWebSocket->SetEnabled(false);
			me->txtTopic->SetReadOnly(true);
			me->lblStatus->SetText(CSTR("Connected"));
			me->btnStart->SetText(CSTR("Stop"));
		}
		else
		{
			client.Delete();
			me->client = 0;
			me->ui->ShowMsgOK(CSTR("Error in communicating with server"), CSTR("Error"), me);
			return;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnCliCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeTestForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"AVIRMQTTExplorerCliCert", false);
	dlg->AddFilter(CSTR("*.crt"), CSTR("Cert file"));
	dlg->SetAllowMultiSel(false);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		NN<Net::ASN1Data> asn1;
		{
			IO::StmData::FileData fd(dlg->GetFileName(), false);
			if (!Optional<Net::ASN1Data>::ConvertFrom(me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ASN1Data)).SetTo(asn1))
			{
				me->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("MQTT Subscribe Test"), me);
				dlg.Delete();
				return;
			}
		}
		if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
		{
			asn1.Delete();
			me->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("MQTT Subscribe Test"), me);
			dlg.Delete();
			return;
		}
		NN<Crypto::Cert::X509File> x509 = NN<Crypto::Cert::X509File>::ConvertFrom(asn1);
		if (x509->GetFileType() != Crypto::Cert::X509File::FileType::Cert)
		{
			asn1.Delete();
			me->ui->ShowMsgOK(CSTR("It is not a cert file"), CSTR("MQTT Subscribe Test"), me);
			dlg.Delete();
			return;
		}
		me->cliCert.Delete();
		me->cliCert = NN<Crypto::Cert::X509Cert>::ConvertFrom(x509);
		NN<Text::String> s = dlg->GetFileName();
		UOSInt i = s->LastIndexOf(IO::Path::PATH_SEPERATOR);
		me->lblCliCert->SetText(s->ToCString().Substring(i + 1));
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnCliKeyClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeTestForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"AVIRMQTTExplorerCliKey", false);
	dlg->AddFilter(CSTR("*.key"), CSTR("Key file"));
	dlg->SetAllowMultiSel(false);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		NN<Net::ASN1Data> asn1;
		{
			IO::StmData::FileData fd(dlg->GetFileName(), false);
			if (!Optional<Net::ASN1Data>::ConvertFrom(me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ASN1Data)).SetTo(asn1))
			{
				me->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("MQTT Subscribe Test"), me);
				dlg.Delete();
				return;
			}
		}
		if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
		{
			asn1.Delete();
			me->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("MQTT Subscribe Test"), me);
			dlg.Delete();
			return;
		}
		me->cliKey.Delete();
		me->cliKey = NN<Crypto::Cert::X509File>::ConvertFrom(asn1);
		NN<Text::String> s = dlg->GetFileName();
		UOSInt i = s->LastIndexOf(IO::Path::PATH_SEPERATOR);
		me->lblCliKey->SetText(s->ToCString().Substring(i + 1));
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnPingTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeTestForm>();
	NN<Net::MQTTConn> client;
	if (me->client.SetTo(client))
	{
		if (client->IsError())
		{
			me->ServerStop();
			me->txtHost->SetReadOnly(false);
			me->txtPort->SetReadOnly(false);
			me->txtUsername->SetReadOnly(false);
			me->txtPassword->SetReadOnly(false);
			me->chkSSL->SetEnabled(true);
			me->chkWebSocket->SetEnabled(true);
			me->txtTopic->SetReadOnly(false);
			me->lblStatus->SetText(CSTR("Not Connected"));
			me->btnStart->SetText(CSTR("Start"));
		}
		else
		{
			client->ClearPackets();
			client->SendPing();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeTestForm>();
	Data::DateTime dt;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Net::MQTTConn> client;

	if (me->client.SetTo(client))
	{
		if (client->IsError())
		{
			me->ServerStop();
			me->txtHost->SetReadOnly(false);
			me->txtPort->SetReadOnly(false);
			me->txtUsername->SetReadOnly(false);
			me->txtPassword->SetReadOnly(false);
			me->chkSSL->SetEnabled(true);
			me->chkWebSocket->SetEnabled(true);
			me->txtTopic->SetReadOnly(false);
			me->lblStatus->SetText(CSTR("Not Connected"));
			me->btnStart->SetText(CSTR("Start"));
		}
		else
		{
			sptr = Text::StrUInt64(Text::StrConcatC(Text::StrUInt64(Text::StrConcatC(sbuff, UTF8STRC("Up: ")), client->GetTotalUpload()), UTF8STRC(", Dn: ")), client->GetTotalDownload());
			me->lblStatus->SetText(CSTRP(sbuff, sptr));
		}
	}

	UInt64 thisCount = me->totalCount;
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	if (thisCount < me->dispCount)
	{
		me->txtRecvRate->SetText(CSTR("0.0"));
	}
	else
	{
		Double rate = (Double)(thisCount - me->dispCount) / ts.DiffSecDbl(me->lastDispTime);
		sptr = Text::StrDouble(sbuff, rate);
		me->txtRecvRate->SetText(CSTRP(sbuff, sptr));
	}
	if (thisCount != me->dispCount)
	{
		sptr = Text::StrUInt64(sbuff, thisCount);
		me->txtTotalCount->SetText(CSTRP(sbuff, sptr));
	}
	me->dispCount = thisCount;
	me->lastDispTime = ts;
}


void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnPublishMessage(AnyType userObj, Text::CStringNN topic, const Data::ByteArrayR &message)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeTestForm>();
	Sync::Interlocked::IncrementU64(me->totalCount);
}

void SSWR::AVIRead::AVIRMQTTSubscribeTestForm::ServerStop()
{
	this->client.Delete();
}

SSWR::AVIRead::AVIRMQTTSubscribeTestForm::AVIRMQTTSubscribeTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 200, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MQTT Subscribe Test"));
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->cliCert = 0;
	this->cliKey = 0;
	this->totalCount = 0;
	this->dispCount = 0;
	this->lastDispTime = Data::Timestamp::UtcNow();

	this->lblHost = ui->NewLabel(*this, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(*this, CSTR("127.0.0.1"));
	this->txtHost->SetRect(104, 4, 100, 23, false);
	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("1883"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	this->lblUsername = ui->NewLabel(*this, CSTR("User Name"));
	this->lblUsername->SetRect(254, 4, 100, 23, false);
	this->txtUsername = ui->NewTextBox(*this, CSTR(""));
	this->txtUsername->SetRect(354, 4, 100, 23, false);
	this->lblPassword = ui->NewLabel(*this, CSTR("Password"));
	this->lblPassword->SetRect(254, 28, 100, 23, false);
	this->txtPassword = ui->NewTextBox(*this, CSTR(""));
	this->txtPassword->SetRect(354, 28, 100, 23, false);
	this->chkSSL = ui->NewCheckBox(*this, CSTR("Use SSL"), false);
	this->chkSSL->SetRect(504, 4, 100, 23, false);
	this->chkWebSocket = ui->NewCheckBox(*this, CSTR("WebSocket"), false);
	this->chkWebSocket->SetRect(504, 28, 100, 23, false);
	this->btnCliCert = ui->NewButton(*this, CSTR("Client Cert"));
	this->btnCliCert->SetRect(604, 4, 75, 23, false);
	this->btnCliCert->HandleButtonClick(OnCliCertClicked, this);
	this->lblCliCert = ui->NewLabel(*this, CSTR(""));
	this->lblCliCert->SetRect(684, 4, 100, 23, false);
	this->btnCliKey = ui->NewButton(*this, CSTR("Client Key"));
	this->btnCliKey->SetRect(604, 28, 75, 23, false);
	this->btnCliKey->HandleButtonClick(OnCliKeyClicked, this);
	this->lblCliKey = ui->NewLabel(*this, CSTR(""));
	this->lblCliKey->SetRect(684, 28, 100, 23, false);
	this->lblTopic = ui->NewLabel(*this, CSTR("Topic"));
	this->lblTopic->SetRect(4, 52, 100, 23, false);
	this->txtTopic = ui->NewTextBox(*this, CSTR("#"));
	this->txtTopic->SetRect(104, 52, 200, 23, false);
	this->btnStart = ui->NewButton(*this, CSTR("Start"));
	this->btnStart->SetRect(104, 76, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lblTotalCount = ui->NewLabel(*this, CSTR("Total Count"));
	this->lblTotalCount->SetRect(4, 100, 100, 23, false);
	this->txtTotalCount = ui->NewTextBox(*this, CSTR("0"));
	this->txtTotalCount->SetRect(104, 100, 100, 23, false);
	this->txtTotalCount->SetReadOnly(true);
	this->lblRecvRate = ui->NewLabel(*this, CSTR("Recv Rate"));
	this->lblRecvRate->SetRect(4, 124, 100, 23, false);
	this->txtRecvRate = ui->NewTextBox(*this, CSTR("0"));
	this->txtRecvRate->SetRect(104, 124, 100, 23, false);
	this->txtRecvRate->SetReadOnly(true);
	this->lblStatus = ui->NewLabel(*this, CSTR("Not Connected"));
	this->lblStatus->SetRect(4, 148, 150, 23, false);

	this->client = 0;

	this->AddTimer(30000, OnPingTimerTick, this);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRMQTTSubscribeTestForm::~AVIRMQTTSubscribeTestForm()
{
	this->ServerStop();
	this->cliCert.Delete();
	this->cliKey.Delete();
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
