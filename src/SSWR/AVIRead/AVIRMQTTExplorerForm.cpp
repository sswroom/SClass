#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/ChartPlotter.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/ColorProfile.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebSocketClient.h"
#include "SSWR/AVIRead/AVIRMQTTExplorerForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTExplorerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTExplorerForm>();
	if (me->client.NotNull())
	{
		me->ServerStop();
		me->txtHost->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		me->txtUsername->SetReadOnly(false);
		me->txtPassword->SetReadOnly(false);
		me->chkSSL->SetEnabled(true);
		me->chkWebSocket->SetEnabled(true);
		me->lvRecvTopic->ClearItems();
		me->ClearTopics();
		me->lblStatus->SetText(CSTR("Disconnected"));
		me->btnStart->SetText(CSTR("Start"));
	}
	else
	{
		me->lvRecvTopic->ClearItems();
		me->ClearTopics();

		Bool useSSL = me->chkSSL->IsChecked();
		Bool useWS = me->chkWebSocket->IsChecked();
		Text::StringBuilderUTF8 sb;
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
		Optional<Net::SSLEngine> ssl = me->ssl;
		NN<Net::SSLEngine> nnssl;
		NN<Net::MQTTConn> client;
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
			NEW_CLASSNN(ws, Net::WebSocketClient(me->core->GetTCPClientFactory(), useSSL?ssl:0, sb.ToCString(), port, CSTR("/mqtt"), CSTR_NULL, Net::WebSocketClient::Protocol::MQTT, 30000));
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
			if (client->SendSubscribe(1, CSTR("#")))
			{
				if (client->WaitSubAck(1, 30000) <= 2)
				{
					succ = true;
				}
			}
		}
		if (succ)
		{
			succ = false;
			if (client->SendSubscribe(2, CSTR("$SYS/#")))
			{
				if (client->WaitSubAck(2, 30000) <= 2)
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

void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnCliCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTExplorerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTExplorerForm>();
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
				me->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("MQTT Explorer"), me);
				return;
			}
		}
		if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
		{
			asn1.Delete();
			me->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("MQTT Explorer"), me);
			return;
		}
		NN<Crypto::Cert::X509File> x509 = NN<Crypto::Cert::X509File>::ConvertFrom(asn1);
		if (x509->GetFileType() != Crypto::Cert::X509File::FileType::Cert)
		{
			asn1.Delete();
			me->ui->ShowMsgOK(CSTR("It is not a cert file"), CSTR("MQTT Explorer"), me);
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

void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnCliKeyClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTExplorerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTExplorerForm>();
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
				me->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("MQTT Explorer"), me);
				return;
			}
		}
		if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
		{
			asn1.Delete();
			me->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("MQTT Explorer"), me);
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

void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnPublishClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTExplorerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTExplorerForm>();
	NN<Net::MQTTConn> client;
	if (!me->client.SetTo(client))
	{
		return;
	}

	Text::StringBuilderUTF8 sbTopic;
	Text::StringBuilderUTF8 sbContent;
	me->txtPubTopic->GetText(sbTopic);
	me->txtPubContent->GetText(sbContent);
	if (sbTopic.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter topic"), CSTR("MQTT Explorer"), me);
		return;
	}
	if (sbContent.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter content"), CSTR("MQTT Explorer"), me);
		return;
	}
	if (!client->SendPublish(sbTopic.ToCString(), sbContent.ToCString(), me->chkPubDUP->IsChecked(), (UInt8)me->cboPubQoS->GetSelectedIndex(), me->chkPubRetain->IsChecked()))
	{
		me->ui->ShowMsgOK(CSTR("Error in publishing topic"), CSTR("MQTT Explorer"), me);
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnTopicSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTExplorerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTExplorerForm>();
	me->currTopic = me->lvRecvTopic->GetSelectedItem().GetOpt<SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus>();
	me->UpdateTopicChart();
}

void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnPingTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTExplorerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTExplorerForm>();
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

void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTExplorerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTExplorerForm>();
	NN<const Data::ArrayListNN<SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus>> topicList;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus> topicSt;
	UOSInt i;
	UOSInt j;
	Sync::MutexUsage mutUsage(me->topicMut);
	topicList = me->topicMap.GetValues();
	i = 0;
	j = topicList->GetCount();
	if (me->topicListChanged)
	{
		me->topicListChanged = false;
		me->lvRecvTopic->ClearItems();
		while (i < j)
		{
			topicSt = topicList->GetItemNoCheck(i);
			if (topicSt->updated)
			{
				topicSt->updated = false;
				if (topicSt.Ptr() == me->currTopic.OrNull())
				{
					me->UpdateTopicChart();
				}
			}
			me->lvRecvTopic->AddItem(topicSt->topic, topicSt);
			me->lvRecvTopic->SetSubItem(i, 1, {topicSt->currValue, topicSt->currValueLen});
			sptr = Text::StrUOSInt(sbuff, topicSt->recvCnt);
			me->lvRecvTopic->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			sptr = Data::Timestamp(topicSt->lastRecvTime, Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
			me->lvRecvTopic->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			i++;
		}
	}
	else
	{
		while (i < j)
		{
			topicSt = topicList->GetItemNoCheck(i);
			if (topicSt->updated)
			{
				topicSt->updated = false;
				me->lvRecvTopic->SetSubItem(i, 1, {topicSt->currValue, topicSt->currValueLen});
				sptr = Text::StrUOSInt(sbuff, topicSt->recvCnt);
				me->lvRecvTopic->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Data::Timestamp(topicSt->lastRecvTime, Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
				me->lvRecvTopic->SetSubItem(i, 3, CSTRP(sbuff, sptr));

				if (topicSt.Ptr() == me->currTopic.OrNull())
				{
					me->UpdateTopicChart();
				}
			}
			i++;
		}
	}
	mutUsage.EndUse();

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
			me->lblStatus->SetText(CSTR("Not Connected"));
			me->btnStart->SetText(CSTR("Start"));
		}
		else
		{
			sptr = Text::StrUInt64(Text::StrConcatC(Text::StrUInt64(Text::StrConcatC(sbuff, UTF8STRC("Up: ")), client->GetTotalUpload()), UTF8STRC(", Dn: ")), client->GetTotalDownload());
			me->lblStatus->SetText(CSTRP(sbuff, sptr));
		}
	}
}


void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnPublishMessage(AnyType userObj, Text::CStringNN topic, const Data::ByteArrayR &message)
{
	NN<SSWR::AVIRead::AVIRMQTTExplorerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTExplorerForm>();
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Received message, topic = "));
	sb.Append(topic);
	sb.AppendC(UTF8STRC(", message = "));
	sb.AppendC(message.Arr(), message.GetSize());
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);

	Data::Timestamp ts = Data::Timestamp::UtcNow();
	NN<SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus> topicSt;
	Sync::MutexUsage mutUsage(me->topicMut);
	if (!me->topicMap.GetC(topic).SetTo(topicSt))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("New Topic: "));
		sb.Append(topic);
		me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);

		topicSt = MemAllocNN(SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus);
		topicSt->topic = Text::String::New(topic);
		topicSt->currValue = MemAllocArr(UTF8Char, message.GetSize() + 1);
		Text::StrConcatC(topicSt->currValue, message.Arr(), message.GetSize());
		topicSt->currValueLen = message.GetSize();
		topicSt->updated = true;
		topicSt->recvCnt = 1;
		topicSt->lastRecvTime = ts.inst;
		me->topicMap.PutNN(topicSt->topic, topicSt);
		me->topicListChanged = true;
	}
	else
	{
		MemFreeArr(topicSt->currValue);
		topicSt->currValue = MemAllocArr(UTF8Char, message.GetSize() + 1);
		Text::StrConcatC(topicSt->currValue, message.Arr(), message.GetSize());	
		topicSt->currValueLen = message.GetSize();
		topicSt->updated = true;
		topicSt->recvCnt++;
		topicSt->lastRecvTime = ts.inst;
	}
	Double dVal;
	UOSInt i;
	topicSt->dateList[(topicSt->recvCnt - 1) & 255] = Data::Timestamp(topicSt->lastRecvTime, Data::DateTimeUtil::GetLocalTzQhr());
	if (Text::StrToDouble(topicSt->currValue, dVal))
	{
		topicSt->valueList[(topicSt->recvCnt - 1) & 255] = dVal;
	}
	else
	{
		dVal = 0;
		i = Text::StrIndexOfC(topicSt->currValue, topicSt->currValueLen, UTF8STRC(" seconds"));
		if (i != INVALID_INDEX && i > 0)
		{
			sb.ClearStr();
			sb.AppendC(topicSt->currValue, (UOSInt)i);
			Text::StrToDouble(sb.ToString(), dVal);
		}
		topicSt->valueList[(topicSt->recvCnt - 1) & 255] = dVal;
	}
}

void SSWR::AVIRead::AVIRMQTTExplorerForm::UpdateTopicChart()
{
	Optional<Media::DrawImage> dimg;
	NN<Media::DrawEngine> deng = this->core->GetDrawEngine();
	Math::Size2D<UOSInt> sz = this->pbRecvTopic->GetSizeP();
	NN<Media::DrawImage> gimg;
	if (sz.x > 0 && sz.y > 0)
	{
		if (!this->dispImg.SetTo(gimg) || gimg->GetWidth() != sz.x || gimg->GetHeight() != sz.y)
		{
			if (this->dispImg.SetTo(gimg))
			{
				deng->DeleteImage(gimg);
			}
			dimg = deng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF);
			this->dispImg = dimg;
		}
		if (this->dispImg.SetTo(gimg))
		{
			NN<TopicStatus> currTopic;
			if (!this->currTopic.SetTo(currTopic) || currTopic->recvCnt <= 1)
			{
				NN<Media::DrawBrush> b;
				b = gimg->NewBrushARGB(0xffffffff);
				gimg->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), 0, b);
				gimg->DelBrush(b);
			}
			else
			{
				if (currTopic->recvCnt < 256)
				{
					UOSInt recvCnt = currTopic->recvCnt;
					Data::ChartPlotter *chart;
					NEW_CLASS(chart, Data::ChartPlotter(CSTR_NULL));
					;
					chart->AddLineChart(currTopic->topic, Data::ChartPlotter::NewData(currTopic->valueList, recvCnt), Data::ChartPlotter::NewData(currTopic->dateList, recvCnt), 0xFFFF0000);
					chart->Plot(gimg, 0, 0, UOSInt2Double(sz.x), UOSInt2Double(sz.y));
					DEL_CLASS(chart);
				}
				else
				{
					UOSInt recvCnt = currTopic->recvCnt;
					Int64 *dateList = MemAlloc(Int64, 256);
					Double *valueList = MemAlloc(Double, 256);
					if (recvCnt & 255)
					{
						MemCopyNO(dateList, &currTopic->dateList[recvCnt & 255], sizeof(Int64) * (256 - (recvCnt & 255)));
						MemCopyNO(valueList, &currTopic->valueList[recvCnt & 255], sizeof(Double) * (256 - (recvCnt & 255)));
						MemCopyNO(&dateList[256 - (recvCnt & 255)], currTopic->dateList, sizeof(Int64) * (recvCnt & 255));
						MemCopyNO(&valueList[256 - (recvCnt & 255)], currTopic->valueList, sizeof(Double) * (recvCnt & 255));
					}
					else
					{
						MemCopyNO(dateList, currTopic->dateList, sizeof(Int64) * 256);
						MemCopyNO(valueList, currTopic->valueList, sizeof(Double) * 256);
					}
					
					Data::ChartPlotter *chart;
					NEW_CLASS(chart, Data::ChartPlotter(CSTR_NULL));
					chart->AddLineChart(currTopic->topic, Data::ChartPlotter::NewData(currTopic->valueList, 256), Data::ChartPlotter::NewData(currTopic->dateList, 256), 0xFFFF0000);
					chart->Plot(gimg, 0, 0, UOSInt2Double(sz.x), UOSInt2Double(sz.y));
					DEL_CLASS(chart);
					MemFree(dateList);
					MemFree(valueList);
				}
				
			}
			
			this->pbRecvTopic->SetImageDImg(this->dispImg);
		}
	}
}

void SSWR::AVIRead::AVIRMQTTExplorerForm::ServerStop()
{
	this->client.Delete();
}

void SSWR::AVIRead::AVIRMQTTExplorerForm::ClearTopics()
{
	UOSInt i;
	NN<SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus> topicSt;
	NN<const Data::ArrayListNN<SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus>> topicList;
	topicList = this->topicMap.GetValues();
	i = topicList->GetCount();
	while (i-- > 0)
	{
		topicSt = topicList->GetItemNoCheck(i);
		topicSt->topic->Release();
		MemFreeArr(topicSt->currValue);
		MemFreeNN(topicSt);
	}
	this->topicMap.Clear();
}

SSWR::AVIRead::AVIRMQTTExplorerForm::AVIRMQTTExplorerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MQTT Explorer"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->currTopic = 0;
	this->dispImg = 0;
	this->cliCert = 0;
	this->cliKey = 0;

	this->pnlConnect = ui->NewPanel(*this);
	this->pnlConnect->SetRect(0, 0, 100, 103, false);
	this->pnlConnect->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblHost = ui->NewLabel(this->pnlConnect, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(this->pnlConnect, CSTR("127.0.0.1"));
	this->txtHost->SetRect(104, 4, 100, 23, false);
	this->lblPort = ui->NewLabel(this->pnlConnect, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlConnect, CSTR("1883"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	this->lblUsername = ui->NewLabel(this->pnlConnect, CSTR("User Name"));
	this->lblUsername->SetRect(254, 4, 100, 23, false);
	this->txtUsername = ui->NewTextBox(this->pnlConnect, CSTR(""));
	this->txtUsername->SetRect(354, 4, 100, 23, false);
	this->lblPassword = ui->NewLabel(this->pnlConnect, CSTR("Password"));
	this->lblPassword->SetRect(254, 28, 100, 23, false);
	this->txtPassword = ui->NewTextBox(this->pnlConnect, CSTR(""));
	this->txtPassword->SetRect(354, 28, 100, 23, false);
	this->chkSSL = ui->NewCheckBox(this->pnlConnect, CSTR("Use SSL"), false);
	this->chkSSL->SetRect(504, 4, 100, 23, false);
	this->chkWebSocket = ui->NewCheckBox(this->pnlConnect, CSTR("WebSocket"), false);
	this->chkWebSocket->SetRect(504, 28, 100, 23, false);
	this->btnCliCert = ui->NewButton(this->pnlConnect, CSTR("Client Cert"));
	this->btnCliCert->SetRect(604, 4, 75, 23, false);
	this->btnCliCert->HandleButtonClick(OnCliCertClicked, this);
	this->lblCliCert = ui->NewLabel(this->pnlConnect, CSTR(""));
	this->lblCliCert->SetRect(684, 4, 100, 23, false);
	this->btnCliKey = ui->NewButton(this->pnlConnect, CSTR("Client Key"));
	this->btnCliKey->SetRect(604, 28, 75, 23, false);
	this->btnCliKey->HandleButtonClick(OnCliKeyClicked, this);
	this->lblCliKey = ui->NewLabel(this->pnlConnect, CSTR(""));
	this->lblCliKey->SetRect(684, 28, 100, 23, false);
	this->btnStart = ui->NewButton(this->pnlConnect, CSTR("Start"));
	this->btnStart->SetRect(104, 52, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lblStatus = ui->NewLabel(this->pnlConnect, CSTR("Not Connected"));
	this->lblStatus->SetRect(4, 80, 150, 23, false);
	this->tcDetail = ui->NewTabControl(*this);
	this->tcDetail->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpRecv = this->tcDetail->AddTabPage(CSTR("Recv"));
	this->pbRecvTopic = ui->NewPictureBoxSimple(this->tpRecv, this->core->GetDrawEngine(), false);
	this->pbRecvTopic->SetRect(0, 0, 100, 300, false);
	this->pbRecvTopic->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->vspRecvTopic = ui->NewVSplitter(this->tpRecv, 3, true);
	this->lvRecvTopic = ui->NewListView(this->tpRecv, UI::ListViewStyle::Table, 4);
	this->lvRecvTopic->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvRecvTopic->SetShowGrid(true);
	this->lvRecvTopic->SetFullRowSelect(true);
	this->lvRecvTopic->AddColumn(CSTR("Topic"), 200);
	this->lvRecvTopic->AddColumn(CSTR("Message"), 200);
	this->lvRecvTopic->AddColumn(CSTR("Count"), 60);
	this->lvRecvTopic->AddColumn(CSTR("Update Time"), 150);
	this->lvRecvTopic->HandleSelChg(OnTopicSelChg, this);

	this->tpPublish = this->tcDetail->AddTabPage(CSTR("Publish"));
	this->pnlPubTopic = ui->NewPanel(this->tpPublish);
	this->pnlPubTopic->SetRect(0, 0, 100, 103, false);
	this->pnlPubTopic->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPubTopic = ui->NewLabel(this->pnlPubTopic, CSTR("Topic"));
	this->lblPubTopic->SetRect(4, 4, 100, 23, false);
	this->txtPubTopic = ui->NewTextBox(this->pnlPubTopic, CSTR(""));
	this->txtPubTopic->SetRect(104, 4, 300, 23, false);
	this->chkPubDUP = ui->NewCheckBox(this->pnlPubTopic, CSTR("DUP"), false);
	this->chkPubDUP->SetRect(104, 28, 100, 23, false);
	this->chkPubRetain = ui->NewCheckBox(this->pnlPubTopic, CSTR("Retain"), false);
	this->chkPubRetain->SetRect(104, 52, 100, 23, false);
	this->lblPubQoS = ui->NewLabel(this->pnlPubTopic, CSTR("QoS"));
	this->lblPubQoS->SetRect(4, 76, 100, 23, false);
	this->cboPubQoS = ui->NewComboBox(this->pnlPubTopic, false);
	this->cboPubQoS->SetRect(104, 76, 300, 23, false);
	this->cboPubQoS->AddItem(CSTR("At most once delivery"), 0);
	this->cboPubQoS->AddItem(CSTR("At least once delivery"), (void*)1);
	this->cboPubQoS->AddItem(CSTR("Exactly once delivery"), (void*)2);
	this->cboPubQoS->SetSelectedIndex(0);
	this->pnlPubCtrl = ui->NewPanel(this->tpPublish);
	this->pnlPubCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlPubCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnPublish = ui->NewButton(this->pnlPubCtrl, CSTR("Publish"));
	this->btnPublish->SetRect(4, 4, 75, 23, false);
	this->btnPublish->HandleButtonClick(OnPublishClicked, this);
	this->txtPubContent = ui->NewTextBox(this->tpPublish, CSTR(""), true);
	this->txtPubContent->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpLog = this->tcDetail->AddTabPage(CSTR("Log"));
	this->logger = UI::ListBoxLogger::CreateUI(*this, this->ui, this->tpLog, 500, false);
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);

	this->client = 0;

	this->AddTimer(30000, OnPingTimerTick, this);
	this->AddTimer(1000, OnTimerTick, this);
	this->UpdateTopicChart();
}

SSWR::AVIRead::AVIRMQTTExplorerForm::~AVIRMQTTExplorerForm()
{
	this->log.RemoveLogHandler(this->logger);
	this->ServerStop();
	this->ClearTopics();
	this->cliCert.Delete();
	this->cliKey.Delete();
	this->logger.Delete();
	NN<Media::DrawImage> img;
	if (this->dispImg.SetTo(img))
	{
		this->core->GetDrawEngine()->DeleteImage(img);
		this->dispImg = 0;
	}
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRMQTTExplorerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
