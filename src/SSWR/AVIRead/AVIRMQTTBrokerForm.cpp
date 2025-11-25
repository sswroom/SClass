#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Core/ByteTool_C.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRMQTTBrokerForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRMQTTBrokerForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTBrokerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTBrokerForm>();
	if (me->broker.NotNull())
	{
		me->ServerStop();
		me->txtPort->SetReadOnly(false);
		me->btnStart->SetText(CSTR("Start"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		Bool sslEnable = me->chkSSL->IsChecked();
		UInt16 port;
		me->txtPort->GetText(sb);
		if (!sb.ToUInt16(port))
		{
			me->ui->ShowMsgOK(CSTR("Port is not valid"), CSTR("Error"), me);
		}
		else if (port <= 0)
		{
			me->ui->ShowMsgOK(CSTR("Port is out of range"), CSTR("Error"), me);
		}
		else
		{
			Optional<Net::SSLEngine> ssl = 0;
			if (sslEnable)
			{
				NN<Crypto::Cert::X509Cert> sslCert;
				NN<Crypto::Cert::X509File> sslKey;
				NN<Net::SSLEngine> nnssl;
				if (!me->ssl.SetTo(nnssl))
				{
					me->ui->ShowMsgOK(CSTR("Error in initializing SSL engine"), CSTR("MQTT Broker"), me);
					return;
				}
				if (!me->sslCert.SetTo(sslCert) || !me->sslKey.SetTo(sslKey))
				{
					me->ui->ShowMsgOK(CSTR("Please select SSL Cert/Key to enable SSL"), CSTR("MQTT Broker"), me);
					return;
				}
				ssl = me->ssl;
				nnssl->ServerSetCertsASN1(sslCert, sslKey, me->caCerts);
			}
			NN<Net::MQTTBroker> broker;
			NEW_CLASSNN(broker, Net::MQTTBroker(me->core->GetTCPClientFactory(), ssl, port, me->log, true, false));
			if (broker->IsError())
			{
				me->ui->ShowMsgOK(CSTR("Error in initing server"), CSTR("Error"), me);
				broker.Delete();
			}
			else
			{
				me->broker = broker;
				me->totalCount = 0;
				broker->HandleTopicUpdate(OnTopicUpdate, me);
				if (broker->Start())
				{
					me->txtPort->SetReadOnly(true);
					me->btnStart->SetText(CSTR("Stop"));
				}
				else
				{
					me->ui->ShowMsgOK(CSTR("Error in starting server"), CSTR("Error"), me);
					broker.Delete();
					me->broker = 0;
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTBrokerForm::OnSSLCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTBrokerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTBrokerForm>();
	if (me->broker.NotNull())
	{
		me->ui->ShowMsgOK(CSTR("You cannot change cert when server is started"), CSTR("MQTT Broker"), me);
		return;
	}
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->ssl, me->sslCert, me->sslKey, me->caCerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		NN<Crypto::Cert::X509Cert> nnCert;
		NN<Crypto::Cert::X509File> nnKey;
		me->sslCert.Delete();
		me->sslKey.Delete();
		me->ClearCACerts();
		me->sslCert = frm.GetCert();
		me->sslKey = frm.GetKey();
		frm.GetCACerts(me->caCerts);
		Text::StringBuilderUTF8 sb;
		if (me->sslCert.SetTo(nnCert)) nnCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		if (me->sslKey.SetTo(nnKey)) nnKey->ToShortString(sb);
		me->lblSSLCert->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTBrokerForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTBrokerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTBrokerForm>();
	NN<Text::String> s;
	if (me->lbLog->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTBrokerForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTBrokerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTBrokerForm>();
	NN<const Data::ArrayListNN<SSWR::AVIRead::AVIRMQTTBrokerForm::TopicStatus>> topicList;
	NN<SSWR::AVIRead::AVIRMQTTBrokerForm::TopicStatus> topicSt;
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	UInt64 thisCount = me->totalCount;
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	if (thisCount >= me->dispCount)
	{
		Double rate = (Double)(thisCount - me->dispCount) / ts.DiffSecDbl(me->dispTime);
		sb.AppendDouble(rate);
		me->txtDataRate->SetText(sb.ToCString());
	}
	else
	{
		me->txtDataRate->SetText(CSTR("0.0"));
	}
	if (thisCount != me->dispCount)
	{
		sb.ClearStr();
		sb.AppendU64(thisCount);
		me->txtTotalCount->SetText(sb.ToCString());
	}
	me->dispCount = thisCount;
	me->dispTime = ts;
	
	Sync::MutexUsage mutUsage(me->topicMut);
	topicList = me->topicMap.GetValues();
	i = 0;
	j = topicList->GetCount();
	if (me->topicListUpdated)
	{
		me->topicListUpdated = false;
		me->lvTopic->ClearItems();
		while (i < j)
		{
			topicSt = topicList->GetItemNoCheck(i);
			topicSt->updated = false;
			me->lvTopic->AddItem(topicSt->topic, topicSt);
			sb.ClearStr();
			sb.AppendC(topicSt->message, topicSt->msgSize);
			me->lvTopic->SetSubItem(i, 1, sb.ToCString());
			sb.ClearStr();
			sb.AppendTSNoZone(Data::Timestamp(topicSt->updateTime, Data::DateTimeUtil::GetLocalTzQhr()));
			me->lvTopic->SetSubItem(i, 2, sb.ToCString());
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
				sb.ClearStr();
				sb.AppendC(topicSt->message, topicSt->msgSize);
				me->lvTopic->SetSubItem(i, 1, sb.ToCString());
				sb.ClearStr();
				sb.AppendTSNoZone(Data::Timestamp(topicSt->updateTime, Data::DateTimeUtil::GetLocalTzQhr()));
				me->lvTopic->SetSubItem(i, 2, sb.ToCString());
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTBrokerForm::OnTopicUpdate(AnyType userObj, Text::CStringNN topic, UnsafeArray<const UInt8> message, UOSInt msgSize)
{
	NN<SSWR::AVIRead::AVIRMQTTBrokerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTBrokerForm>();
	NN<SSWR::AVIRead::AVIRMQTTBrokerForm::TopicStatus> topicSt;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Sync::Interlocked::IncrementU64(me->totalCount);
	Sync::MutexUsage mutUsage(me->topicMut);
	if (me->topicMap.GetC(topic).SetTo(topicSt))
	{
		if (topicSt->msgSize != msgSize)
		{
			MemFreeArr(topicSt->message);
			topicSt->message = MemAllocArr(UInt8, msgSize);
			topicSt->msgSize = msgSize;
		}
		MemCopyNO(topicSt->message.Ptr(), message.Ptr(), msgSize);
		topicSt->updated = true;
		topicSt->updateTime = dt.ToTicks();
	}
	else
	{
		topicSt = MemAllocNN(SSWR::AVIRead::AVIRMQTTBrokerForm::TopicStatus);
		topicSt->topic = Text::String::New(topic);
		topicSt->message = MemAllocArr(UInt8, msgSize);
		topicSt->msgSize = msgSize;
		topicSt->updated = true;
		MemCopyNO(topicSt->message.Ptr(), message.Ptr(), msgSize);
		topicSt->updateTime = dt.ToTicks();
		me->topicMap.PutC(topic, topicSt);
		me->topicListUpdated = true;
	}
}

void SSWR::AVIRead::AVIRMQTTBrokerForm::ServerStop()
{
	this->broker.Delete();
}

void SSWR::AVIRead::AVIRMQTTBrokerForm::ClearCACerts()
{
	this->caCerts.DeleteAll();
}

SSWR::AVIRead::AVIRMQTTBrokerForm::AVIRMQTTBrokerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MQTT Broker"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->sslCert = 0;
	this->sslKey = 0;
	this->topicListUpdated = false;
	this->totalCount = 0;
	this->dispCount = 0;
	this->dispTime = Data::Timestamp::UtcNow();

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->lblSSL = ui->NewLabel(this->tpStatus, CSTR("SSL"));
	this->lblSSL->SetRect(4, 4, 100, 23, false);
	this->chkSSL = ui->NewCheckBox(this->tpStatus, CSTR("Enable"), false);
	this->chkSSL->SetRect(104, 4, 100, 23, false);
	this->btnSSLCert = ui->NewButton(this->tpStatus, CSTR("Cert/Key"));
	this->btnSSLCert->SetRect(204, 4, 75, 23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	this->lblSSLCert = ui->NewLabel(this->tpStatus, CSTR(""));
	this->lblSSLCert->SetRect(284, 4, 200, 23, false);
	this->lblPort = ui->NewLabel(this->tpStatus, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->tpStatus, CSTR("1883"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	this->btnStart = ui->NewButton(this->tpStatus, CSTR("Start"));
	this->btnStart->SetRect(204, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lblTotalCount = ui->NewLabel(this->tpStatus, CSTR("Total Count"));
	this->lblTotalCount->SetRect(4, 52, 100, 23, false);
	this->txtTotalCount = ui->NewTextBox(this->tpStatus, CSTR("0"));
	this->txtTotalCount->SetReadOnly(true);
	this->txtTotalCount->SetRect(104, 52, 200, 23, false);
	this->lblDataRate = ui->NewLabel(this->tpStatus, CSTR("Data Rate"));
	this->lblDataRate->SetRect(4, 76, 100, 23, false);
	this->txtDataRate = ui->NewTextBox(this->tpStatus, CSTR("0.0"));
	this->txtDataRate->SetReadOnly(true);
	this->txtDataRate->SetRect(104, 76, 200, 23, false);

	this->tpTopic = this->tcMain->AddTabPage(CSTR("Topic"));
	this->lvTopic = ui->NewListView(this->tpTopic, UI::ListViewStyle::Table, 3);
	this->lvTopic->SetDockType(UI::GUIClientControl::DOCK_FILL);
	this->lvTopic->SetFullRowSelect(true);
	this->lvTopic->SetShowGrid(true);
	this->lvTopic->AddColumn(CSTR("Topic"), 200);
	this->lvTopic->AddColumn(CSTR("Message"), 200);
	this->lvTopic->AddColumn(CSTR("Update Time"), 150);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtLog->SetReadOnly(true);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 100, false));
	this->logger->SetTimeFormat("yyyy-MM-dd HH:mm:ss.fff");
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);
	this->broker = 0;
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRMQTTBrokerForm::~AVIRMQTTBrokerForm()
{
	this->ServerStop();
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	NN<const Data::ArrayListNN<SSWR::AVIRead::AVIRMQTTBrokerForm::TopicStatus>> topicList = this->topicMap.GetValues();
	NN<SSWR::AVIRead::AVIRMQTTBrokerForm::TopicStatus> topic;
	UOSInt i = topicList->GetCount();
	while (i-- > 0)
	{
		topic = topicList->GetItemNoCheck(i);
		topic->topic->Release();
		MemFreeArr(topic->message);
		MemFreeNN(topic);
	}
	this->ssl.Delete();
	this->sslCert.Delete();
	this->sslKey.Delete();
	this->ClearCACerts();
}

void SSWR::AVIRead::AVIRMQTTBrokerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
