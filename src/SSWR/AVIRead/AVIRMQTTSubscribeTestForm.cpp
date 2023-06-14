#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/LineChart.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/ColorProfile.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebSocketClient.h"
#include "SSWR/AVIRead/AVIRMQTTSubscribeTestForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTSubscribeTestForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeTestForm*)userObj;
	if (me->client)
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
		me->txtPort->GetText(&sb);
		if (!sb.ToUInt16(&port))
		{
			UI::MessageDialog::ShowDialog(CSTR("Port is not valid"), CSTR("Error"), me);
			return;
		}
		else if (port <= 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Port is out of range"), CSTR("Error"), me);
			return;
		}
		sb.ClearStr();
		me->txtHost->GetText(&sb);
		if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToCString(), &addr))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing host"), CSTR("Error"), me);
			return;
		}
		me->txtTopic->GetText(&sbTopic);
		if (sbTopic.leng == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter topic to subscribe"), CSTR("Error"), me);
			return;
		}
		Net::SSLEngine *ssl = me->ssl;
		if (useSSL)
		{
			if (me->cliCert && me->cliKey)
			{
				ssl->ClientSetCertASN1(me->cliCert, me->cliKey);
			}
		}
		if (useWS)
		{
			Net::WebSocketClient *ws;
			NEW_CLASS(ws, Net::WebSocketClient(me->core->GetSocketFactory(), useSSL?ssl:0, sb.ToCString(), port, CSTR("/mqtt"), CSTR_NULL, Net::WebSocketClient::Protocol::MQTT, 10000));
			if (ws->IsDown())
			{
				DEL_CLASS(ws);
				UI::MessageDialog::ShowDialog(CSTR("Error in initializing websocket"), CSTR("Error"), me);
				return;
			}
			NEW_CLASS(me->client, Net::MQTTConn(ws, 0, 0));
		}
		else
		{
			NEW_CLASS(me->client, Net::MQTTConn(me->core->GetSocketFactory(), useSSL?ssl:0, sb.ToCString(), port, 0, 0, 30000));
		}
		if (me->client->IsError())
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in connecting to server"), CSTR("Error"), me);
			DEL_CLASS(me->client);
			me->client = 0;
			return;
		}
		me->client->HandlePublishMessage(OnPublishMessage, me);

		Text::CString username = CSTR_NULL;
		Text::CString password = CSTR_NULL;
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		sb.ClearStr();
		me->txtUsername->GetText(&sb);
		if (sb.GetLength() > 0)
		{
			username.v = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
			username.leng = sb.GetLength();
		}
		sb.ClearStr();
		me->txtPassword->GetText(&sb);
		if (sb.GetLength() > 0)
		{
			password.v = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
			password.leng = sb.GetLength();
		}
		sb.ClearStr();
		sb.AppendC(UTF8STRC("sswrMQTT/"));
		sb.AppendI64(dt.ToTicks());
		Bool succ = me->client->SendConnect(4, 30, sb.ToCString(), username, password);
		SDEL_TEXT(username.v);
		SDEL_TEXT(password.v);
		if (succ)
		{
			succ = (me->client->WaitConnAck(30000) == Net::MQTTConn::CS_ACCEPTED);
		}

		if (succ)
		{
			succ = false;
			me->totalCount = 0;
			if (me->client->SendSubscribe(1, sbTopic.ToCString()))
			{
				if (me->client->WaitSubAck(1, 30000) <= 2)
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
			DEL_CLASS(me->client);
			me->client = 0;
			UI::MessageDialog::ShowDialog(CSTR("Error in communicating with server"), CSTR("Error"), me);
			return;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnCliCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTSubscribeTestForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeTestForm*)userObj;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"AVIRMQTTExplorerCliCert", false);
	dlg.AddFilter(CSTR("*.crt"), CSTR("Cert file"));
	dlg.SetAllowMultiSel(false);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		IO::StmData::FileData *fd;
		NEW_CLASS(fd, IO::StmData::FileData(dlg.GetFileName(), false));
		Net::ASN1Data *asn1 = (Net::ASN1Data*)me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ASN1Data);
		DEL_CLASS(fd);
		if (asn1 == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing file"), CSTR("MQTT Subscribe Test"), me);
			return;
		}
		if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
		{
			DEL_CLASS(asn1);
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing file"), CSTR("MQTT Subscribe Test"), me);
			return;
		}
		Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn1;
		if (x509->GetFileType() != Crypto::Cert::X509File::FileType::Cert)
		{
			DEL_CLASS(asn1);
			UI::MessageDialog::ShowDialog(CSTR("It is not a cert file"), CSTR("MQTT Subscribe Test"), me);
			return;
		}
		SDEL_CLASS(me->cliCert);
		me->cliCert = (Crypto::Cert::X509Cert*)x509;
		Text::String *s = dlg.GetFileName();
		UOSInt i = s->LastIndexOf(IO::Path::PATH_SEPERATOR);
		me->lblCliCert->SetText(s->ToCString().Substring(i + 1));
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnCliKeyClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTSubscribeTestForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeTestForm*)userObj;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"AVIRMQTTExplorerCliKey", false);
	dlg.AddFilter(CSTR("*.key"), CSTR("Key file"));
	dlg.SetAllowMultiSel(false);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		IO::StmData::FileData *fd;
		NEW_CLASS(fd, IO::StmData::FileData(dlg.GetFileName(), false));
		Net::ASN1Data *asn1 = (Net::ASN1Data*)me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ASN1Data);
		DEL_CLASS(fd);
		if (asn1 == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing file"), CSTR("MQTT Subscribe Test"), me);
			return;
		}
		if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
		{
			DEL_CLASS(asn1);
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing file"), CSTR("MQTT Subscribe Test"), me);
			return;
		}
		SDEL_CLASS(me->cliKey);
		me->cliKey = (Crypto::Cert::X509File*)asn1;
		Text::String *s = dlg.GetFileName();
		UOSInt i = s->LastIndexOf(IO::Path::PATH_SEPERATOR);
		me->lblCliKey->SetText(s->ToCString().Substring(i + 1));
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnPingTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTSubscribeTestForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeTestForm*)userObj;
	if (me->client)
	{
		if (me->client->IsError())
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
			me->client->ClearPackets();
			me->client->SendPing();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTSubscribeTestForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeTestForm*)userObj;
	Data::DateTime dt;
	UTF8Char sbuff[64];
	UTF8Char *sptr;

	if (me->client)
	{
		if (me->client->IsError())
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
			sptr = Text::StrUInt64(Text::StrConcatC(Text::StrUInt64(Text::StrConcatC(sbuff, UTF8STRC("Up: ")), me->client->GetTotalUpload()), UTF8STRC(", Dn: ")), me->client->GetTotalDownload());
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


void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnPublishMessage(void *userObj, Text::CString topic, const UInt8 *message, UOSInt msgSize)
{
	SSWR::AVIRead::AVIRMQTTSubscribeTestForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeTestForm*)userObj;
	Sync::Interlocked::Increment(&me->totalCount);
}

void SSWR::AVIRead::AVIRMQTTSubscribeTestForm::ServerStop()
{
	if (this->client)
	{
		DEL_CLASS(this->client);
		this->client = 0;
	}
}

SSWR::AVIRead::AVIRMQTTSubscribeTestForm::AVIRMQTTSubscribeTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 200, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MQTT Subscribe Test"));
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->cliCert = 0;
	this->cliKey = 0;
	this->totalCount = 0;
	this->dispCount = 0;
	this->lastDispTime = Data::Timestamp::UtcNow();

	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this, CSTR("Host")));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this, CSTR("127.0.0.1")));
	this->txtHost->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, CSTR("Port")));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, CSTR("1883")));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblUsername, UI::GUILabel(ui, this, CSTR("User Name")));
	this->lblUsername->SetRect(254, 4, 100, 23, false);
	NEW_CLASS(this->txtUsername, UI::GUITextBox(ui, this, CSTR("")));
	this->txtUsername->SetRect(354, 4, 100, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this, CSTR("Password")));
	this->lblPassword->SetRect(254, 28, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this, CSTR("")));
	this->txtPassword->SetRect(354, 28, 100, 23, false);
	NEW_CLASS(this->chkSSL, UI::GUICheckBox(ui, this, CSTR("Use SSL"), false));
	this->chkSSL->SetRect(504, 4, 100, 23, false);
	NEW_CLASS(this->chkWebSocket, UI::GUICheckBox(ui, this, CSTR("WebSocket"), false));
	this->chkWebSocket->SetRect(504, 28, 100, 23, false);
	NEW_CLASS(this->btnCliCert, UI::GUIButton(ui, this, CSTR("Client Cert")));
	this->btnCliCert->SetRect(604, 4, 75, 23, false);
	this->btnCliCert->HandleButtonClick(OnCliCertClicked, this);
	NEW_CLASS(this->lblCliCert, UI::GUILabel(ui, this, CSTR("")));
	this->lblCliCert->SetRect(684, 4, 100, 23, false);
	NEW_CLASS(this->btnCliKey, UI::GUIButton(ui, this, CSTR("Client Key")));
	this->btnCliKey->SetRect(604, 28, 75, 23, false);
	this->btnCliKey->HandleButtonClick(OnCliKeyClicked, this);
	NEW_CLASS(this->lblCliKey, UI::GUILabel(ui, this, CSTR("")));
	this->lblCliKey->SetRect(684, 28, 100, 23, false);
	NEW_CLASS(this->lblTopic, UI::GUILabel(ui, this, CSTR("Topic")));
	this->lblTopic->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtTopic, UI::GUITextBox(ui, this, CSTR("#")));
	this->txtTopic->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, CSTR("Start")));
	this->btnStart->SetRect(104, 76, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->lblTotalCount, UI::GUILabel(ui, this, CSTR("Total Count")));
	this->lblTotalCount->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtTotalCount, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtTotalCount->SetRect(104, 100, 100, 23, false);
	this->txtTotalCount->SetReadOnly(true);
	NEW_CLASS(this->lblRecvRate, UI::GUILabel(ui, this, CSTR("Recv Rate")));
	this->lblRecvRate->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtRecvRate, UI::GUITextBox(ui, this, CSTR("0")));
	this->txtRecvRate->SetRect(104, 124, 100, 23, false);
	this->txtRecvRate->SetReadOnly(true);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, CSTR("Not Connected")));
	this->lblStatus->SetRect(4, 148, 150, 23, false);

	this->client = 0;

	this->AddTimer(30000, OnPingTimerTick, this);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRMQTTSubscribeTestForm::~AVIRMQTTSubscribeTestForm()
{
	this->ServerStop();
	SDEL_CLASS(this->cliCert);
	SDEL_CLASS(this->cliKey);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRMQTTSubscribeTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
