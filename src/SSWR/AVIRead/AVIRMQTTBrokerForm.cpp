#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Data/ByteTool.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRMQTTBrokerForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRMQTTBrokerForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTBrokerForm *me = (SSWR::AVIRead::AVIRMQTTBrokerForm*)userObj;
	if (me->broker)
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
		me->txtPort->GetText(&sb);
		if (!sb.ToUInt16(&port))
		{
			UI::MessageDialog::ShowDialog(CSTR("Port is not valid"), CSTR("Error"), me);
		}
		else if (port <= 0 || port >= 65536)
		{
			UI::MessageDialog::ShowDialog(CSTR("Port is out of range"), CSTR("Error"), me);
		}
		else
		{
			Net::SSLEngine *ssl = 0;
			if (sslEnable)
			{
				if (me->ssl == 0)
				{
					UI::MessageDialog::ShowDialog(CSTR("Error in initializing SSL engine"), CSTR("MQTT Broker"), me);
					return;
				}
				if (me->sslCert == 0 || me->sslKey == 0)
				{
					UI::MessageDialog::ShowDialog(CSTR("Please select SSL Cert/Key to enable SSL"), CSTR("MQTT Broker"), me);
					return;
				}
				ssl = me->ssl;
				Crypto::Cert::X509Cert *issuerCert = Crypto::Cert::CertUtil::FindIssuer(me->sslCert);
				ssl->ServerSetCertsASN1(me->sslCert, me->sslKey, issuerCert);
				SDEL_CLASS(issuerCert);
			}
			NEW_CLASS(me->broker, Net::MQTTBroker(me->core->GetSocketFactory(), ssl, port, &me->log, true, false));
			if (me->broker->IsError())
			{
				UI::MessageDialog::ShowDialog(CSTR("Error in initing server"), CSTR("Error"), me);
				DEL_CLASS(me->broker);
				me->broker = 0;
			}
			else
			{
				me->totalCount = 0;
				me->broker->HandleTopicUpdate(OnTopicUpdate, me);
				if (me->broker->Start())
				{
					me->txtPort->SetReadOnly(true);
					me->btnStart->SetText(CSTR("Stop"));
				}
				else
				{
					UI::MessageDialog::ShowDialog(CSTR("Error in starting server"), CSTR("Error"), me);
					DEL_CLASS(me->broker);
					me->broker = 0;
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTBrokerForm::OnSSLCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTBrokerForm *me = (SSWR::AVIRead::AVIRMQTTBrokerForm*)userObj;
	if (me->broker)
	{
		UI::MessageDialog::ShowDialog(CSTR("You cannot change cert when server is started"), CSTR("MQTT Broker"), me);
		return;
	}
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->ssl, me->sslCert, me->sslKey);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->sslCert);
		SDEL_CLASS(me->sslKey);
		me->sslCert = frm.GetCert();
		me->sslKey = frm.GetKey();
		Text::StringBuilderUTF8 sb;
		me->sslCert->ToShortString(&sb);
		sb.AppendC(UTF8STRC(", "));
		me->sslKey->ToShortString(&sb);
		me->lblSSLCert->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTBrokerForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTBrokerForm *me = (SSWR::AVIRead::AVIRMQTTBrokerForm*)userObj;
	Text::String *s = me->lbLog->GetSelectedItemTextNew();
	if (s)
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTBrokerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTBrokerForm *me = (SSWR::AVIRead::AVIRMQTTBrokerForm*)userObj;
	const Data::ArrayList<SSWR::AVIRead::AVIRMQTTBrokerForm::TopicStatus*> *topicList;
	SSWR::AVIRead::AVIRMQTTBrokerForm::TopicStatus *topicSt;
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
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
	
	Sync::MutexUsage mutUsage(&me->topicMut);
	topicList = me->topicMap.GetValues();
	i = 0;
	j = topicList->GetCount();
	if (me->topicListUpdated)
	{
		me->topicListUpdated = false;
		me->lvTopic->ClearItems();
		while (i < j)
		{
			topicSt = topicList->GetItem(i);
			topicSt->updated = false;
			me->lvTopic->AddItem(topicSt->topic, topicSt);
			sb.ClearStr();
			sb.AppendC(topicSt->message, topicSt->msgSize);
			me->lvTopic->SetSubItem(i, 1, sb.ToCString());
			dt.SetTicks(topicSt->updateTime);
			dt.ToLocalTime();
			sb.ClearStr();
			sb.AppendDate(&dt);
			me->lvTopic->SetSubItem(i, 2, sb.ToCString());
			i++;
		}
	}
	else
	{
		while (i < j)
		{
			topicSt = topicList->GetItem(i);
			if (topicSt->updated)
			{
				topicSt->updated = false;
				sb.ClearStr();
				sb.AppendC(topicSt->message, topicSt->msgSize);
				me->lvTopic->SetSubItem(i, 1, sb.ToCString());
				dt.SetTicks(topicSt->updateTime);
				dt.ToLocalTime();
				sb.ClearStr();
				sb.AppendDate(&dt);
				me->lvTopic->SetSubItem(i, 2, sb.ToCString());
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTBrokerForm::OnTopicUpdate(void *userObj, Text::CString topic, const UInt8 *message, UOSInt msgSize)
{
	SSWR::AVIRead::AVIRMQTTBrokerForm *me = (SSWR::AVIRead::AVIRMQTTBrokerForm*)userObj;
	SSWR::AVIRead::AVIRMQTTBrokerForm::TopicStatus *topicSt;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Sync::Interlocked::Increment(&me->totalCount);
	Sync::MutexUsage mutUsage(&me->topicMut);
	topicSt = me->topicMap.Get(topic);
	if (topicSt)
	{
		if (topicSt->msgSize != msgSize)
		{
			MemFree(topicSt->message);
			topicSt->message = MemAlloc(UInt8, msgSize);
			topicSt->msgSize = msgSize;
		}
		MemCopyNO(topicSt->message, message, msgSize);
		topicSt->updated = true;
		topicSt->updateTime = dt.ToTicks();
	}
	else
	{
		topicSt = MemAlloc(SSWR::AVIRead::AVIRMQTTBrokerForm::TopicStatus, 1);
		topicSt->topic = Text::String::New(topic);
		topicSt->message = MemAlloc(UInt8, msgSize);
		topicSt->msgSize = msgSize;
		topicSt->updated = true;
		MemCopyNO(topicSt->message, message, msgSize);
		topicSt->updateTime = dt.ToTicks();
		me->topicMap.Put(topic, topicSt);
		me->topicListUpdated = true;
	}
}

void SSWR::AVIRead::AVIRMQTTBrokerForm::ServerStop()
{
	if (this->broker)
	{
		DEL_CLASS(this->broker);
		this->broker = 0;
	}
}

SSWR::AVIRead::AVIRMQTTBrokerForm::AVIRMQTTBrokerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MQTT Broker"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->sslCert = 0;
	this->sslKey = 0;
	this->topicListUpdated = false;
	this->totalCount = 0;
	this->dispCount = 0;
	this->dispTime = Data::Timestamp::UtcNow();

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	NEW_CLASS(this->lblSSL, UI::GUILabel(ui, this->tpStatus, CSTR("SSL")));
	this->lblSSL->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->chkSSL, UI::GUICheckBox(ui, this->tpStatus, CSTR("Enable"), false));
	this->chkSSL->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->btnSSLCert, UI::GUIButton(ui, this->tpStatus, CSTR("Cert/Key")));
	this->btnSSLCert->SetRect(204, 4, 75, 23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	NEW_CLASS(this->lblSSLCert, UI::GUILabel(ui, this->tpStatus, CSTR("")));
	this->lblSSLCert->SetRect(284, 4, 200, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->tpStatus, CSTR("Port")));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->tpStatus, CSTR("1883")));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->tpStatus, CSTR("Start")));
	this->btnStart->SetRect(204, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->lblTotalCount, UI::GUILabel(ui, this->tpStatus, CSTR("Total Count")));
	this->lblTotalCount->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtTotalCount, UI::GUITextBox(ui, this->tpStatus, CSTR("0")));
	this->txtTotalCount->SetReadOnly(true);
	this->txtTotalCount->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->lblDataRate, UI::GUILabel(ui, this->tpStatus, CSTR("Data Rate")));
	this->lblDataRate->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtDataRate, UI::GUITextBox(ui, this->tpStatus, CSTR("0.0")));
	this->txtDataRate->SetReadOnly(true);
	this->txtDataRate->SetRect(104, 76, 200, 23, false);

	this->tpTopic = this->tcMain->AddTabPage(CSTR("Topic"));
	NEW_CLASS(this->lvTopic, UI::GUIListView(ui, this->tpTopic, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvTopic->SetDockType(UI::GUIClientControl::DOCK_FILL);
	this->lvTopic->SetFullRowSelect(true);
	this->lvTopic->SetShowGrid(true);
	this->lvTopic->AddColumn(CSTR("Topic"), 200);
	this->lvTopic->AddColumn(CSTR("Message"), 200);
	this->lvTopic->AddColumn(CSTR("Update Time"), 150);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtLog->SetReadOnly(true);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 100, false));
	this->logger->SetTimeFormat("yyyy-MM-dd HH:mm:ss.fff");
	this->log.AddLogHandler(this->logger, IO::ILogHandler::LogLevel::Raw);
	this->broker = 0;
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRMQTTBrokerForm::~AVIRMQTTBrokerForm()
{
	this->ServerStop();
	this->log.RemoveLogHandler(this->logger);
	DEL_CLASS(this->logger);
	const Data::ArrayList<SSWR::AVIRead::AVIRMQTTBrokerForm::TopicStatus*> *topicList = this->topicMap.GetValues();
	SSWR::AVIRead::AVIRMQTTBrokerForm::TopicStatus *topic;
	UOSInt i = topicList->GetCount();
	while (i-- > 0)
	{
		topic = topicList->GetItem(i);
		topic->topic->Release();
		MemFree(topic->message);
		MemFree(topic);
	}
	SDEL_CLASS(this->ssl);
	SDEL_CLASS(this->sslCert);
	SDEL_CLASS(this->sslKey);
}

void SSWR::AVIRead::AVIRMQTTBrokerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
