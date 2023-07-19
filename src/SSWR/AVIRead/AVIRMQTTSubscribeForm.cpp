#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/LineChart.h"
#include "Math/Math.h"
#include "Media/ColorProfile.h"
#include "SSWR/AVIRead/AVIRMQTTSubscribeForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTSubscribeForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeForm*)userObj;
	if (me->client)
	{
		me->ServerStop();
		me->txtHost->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		me->txtUsername->SetReadOnly(false);
		me->txtPassword->SetReadOnly(false);
		me->lbSTopic->ClearItems();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		Net::SocketUtil::AddressInfo addr;
		Int32 port;
		me->txtPort->GetText(&sb);
		if (!sb.ToInt32(&port))
		{
			UI::MessageDialog::ShowDialog(CSTR("Port is not valid"), CSTR("Error"), me);
			return;
		}
		else if (port <= 0 || port >= 65536)
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

		NEW_CLASS(me->client, Net::MQTTConn(me->core->GetSocketFactory(), 0, sb.ToCString(), (UInt16)port, 0, 0, 30000));
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
			username.v = Text::StrCopyNewC(sb.ToString(), sb.GetLength()).Ptr();
			username.leng = sb.GetLength();
		}
		sb.ClearStr();
		me->txtPassword->GetText(&sb);
		if (sb.GetLength() > 0)
		{
			password.v = Text::StrCopyNewC(sb.ToString(), sb.GetLength()).Ptr();
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
			Net::MQTTConn::ConnectStatus status = me->client->WaitConnAck(30000);
			succ = (status == Net::MQTTConn::CS_ACCEPTED);
		}
		
		if (succ)
		{
			me->txtHost->SetReadOnly(true);
			me->txtPort->SetReadOnly(true);
			me->txtUsername->SetReadOnly(true);
			me->txtPassword->SetReadOnly(true);
			me->lbSTopic->ClearItems();
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

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnSTopicClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTSubscribeForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeForm*)userObj;
	if (me->client)
	{
		Text::StringBuilderUTF8 sb;
		me->txtSTopic->GetText(&sb);
		if (sb.GetLength() > 0)
		{
			if (me->client->SendSubscribe(1, sb.ToCString()))
			{
				if (me->client->WaitSubAck(1, 30000) <= 2)
				{
					me->txtSTopic->SetText(CSTR(""));
					me->lbSTopic->AddItem(sb.ToCString(), 0);
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTSubscribeForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeForm*)userObj;
	Text::String *s = me->lbLog->GetSelectedItemTextNew();
	if (s)
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnTopicSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTSubscribeForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeForm*)userObj;
	me->currTopic = (TopicStatus*)me->lvTopic->GetSelectedItem();
	me->UpdateTopicChart();
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnPublishClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTSubscribeForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeForm*)userObj;
	if (me->client)
	{
		Text::StringBuilderUTF8 sbTopic;
		Text::StringBuilderUTF8 sbMsg;
		me->txtPublishTopic->GetText(&sbTopic);
		me->txtPublishMessage->GetText(&sbMsg);
		if (sbTopic.GetLength() > 0 && sbMsg.GetLength() > 0)
		{
			if (me->client->SendPublish(sbTopic.ToCString(), sbMsg.ToCString()))
			{
				me->txtPublishMessage->SetText(CSTR(""));
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnPingTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTSubscribeForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeForm*)userObj;
	if (me->client)
	{
		if (me->client->IsError())
		{
			me->ServerStop();
			me->txtHost->SetReadOnly(false);
			me->txtPort->SetReadOnly(false);
			me->txtUsername->SetReadOnly(false);
			me->txtPassword->SetReadOnly(false);
		}
		else
		{
			me->client->ClearPackets();
			me->client->SendPing();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTSubscribeForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeForm*)userObj;
	const Data::ArrayList<TopicStatus*> *topicList;
	Data::DateTime dt;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	TopicStatus *topicSt;
	UOSInt i;
	UOSInt j;
	Sync::MutexUsage mutUsage(&me->topicMut);
	topicList = me->topicMap.GetValues();
	i = 0;
	j = topicList->GetCount();
	if (me->topicListChanged)
	{
		me->topicListChanged = false;
		me->lvTopic->ClearItems();
		while (i < j)
		{
			topicSt = topicList->GetItem(i);
			if (topicSt->updated)
			{
				topicSt->updated = false;
				if (topicSt == me->currTopic)
				{
					me->UpdateTopicChart();
				}
			}
			me->lvTopic->AddItem(topicSt->topic, topicSt);
			me->lvTopic->SetSubItem(i, 1, {topicSt->currValue, topicSt->currValueLen});
			sptr = Text::StrUOSInt(sbuff, topicSt->recvCnt);
			me->lvTopic->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			dt.SetTicks(topicSt->lastRecvTime);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->lvTopic->SetSubItem(i, 3, CSTRP(sbuff, sptr));
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
				me->lvTopic->SetSubItem(i, 1, {topicSt->currValue, topicSt->currValueLen});
				sptr = Text::StrUOSInt(sbuff, topicSt->recvCnt);
				me->lvTopic->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				dt.SetTicks(topicSt->lastRecvTime);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				me->lvTopic->SetSubItem(i, 3, CSTRP(sbuff, sptr));

				if (topicSt == me->currTopic)
				{
					me->UpdateTopicChart();
				}
			}
			i++;
		}
	}
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnPublishMessage(void *userObj, Text::CString topic, const Data::ByteArrayR &message)
{
	SSWR::AVIRead::AVIRMQTTSubscribeForm *me = (SSWR::AVIRead::AVIRMQTTSubscribeForm*)userObj;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Received message, topic = "));
	sb.Append(topic);
	sb.AppendC(UTF8STRC(", message = "));
	sb.AppendC((const UTF8Char*)message.Ptr(), message.GetSize());
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);

	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	TopicStatus *topicSt;
	Sync::MutexUsage mutUsage(&me->topicMut);
	topicSt = me->topicMap.Get(topic);
	if (topicSt == 0)
	{
		topicSt = MemAlloc(TopicStatus, 1);
		topicSt->topic = Text::String::New(topic);
		topicSt->currValue = MemAlloc(UTF8Char, message.GetSize() + 1);
		Text::StrConcatC(topicSt->currValue, message.Ptr(), message.GetSize());
		topicSt->currValueLen = message.GetSize();
		topicSt->updated = true;
		topicSt->recvCnt = 1;
		topicSt->lastRecvTime = dt.ToTicks();
		me->topicMap.Put(topic, topicSt);
		me->topicListChanged = true;
	}
	else
	{
		MemFree(topicSt->currValue);
		topicSt->currValue = MemAlloc(UTF8Char, message.GetSize() + 1);
		Text::StrConcatC(topicSt->currValue, message.Ptr(), message.GetSize());	
		topicSt->currValueLen = message.GetSize();
		topicSt->updated = true;
		topicSt->recvCnt++;
		topicSt->lastRecvTime = dt.ToTicks();
	}
	Double dVal;
	UOSInt i;
	topicSt->dateList[(topicSt->recvCnt - 1) & 255] = topicSt->lastRecvTime;
	if (Text::StrToDouble(topicSt->currValue, &dVal))
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
			Text::StrToDouble(sb.ToString(), &dVal);
		}
		topicSt->valueList[(topicSt->recvCnt - 1) & 255] = dVal;
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRMQTTSubscribeForm::UpdateTopicChart()
{
	Media::DrawImage *dimg;
	Media::DrawEngine *deng = this->core->GetDrawEngine();
	Math::Size2D<UOSInt> sz = this->pbTopic->GetSizeP();
	if (sz.x > 0 && sz.y > 0)
	{
		if (this->dispImg == 0 || this->dispImg->GetSize() != sz)
		{
			if (this->dispImg)
			{
				deng->DeleteImage(this->dispImg);
			}
			dimg = deng->CreateImage32(sz, Media::AT_NO_ALPHA);
			this->dispImg = dimg;
		}
		if (this->currTopic == 0 || this->currTopic->recvCnt <= 1)
		{
			Media::DrawBrush *b;
			b = this->dispImg->NewBrushARGB(0xffffffff);
			this->dispImg->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), 0, b);
			this->dispImg->DelBrush(b);
		}
		else
		{
			if (this->currTopic->recvCnt < 256)
			{
				UOSInt recvCnt = this->currTopic->recvCnt;
				Data::LineChart *chart;
				NEW_CLASS(chart, Data::LineChart(CSTR_NULL));
				chart->AddXDataDate(this->currTopic->dateList, recvCnt);
				chart->AddYData(this->currTopic->topic.Ptr(), this->currTopic->valueList, recvCnt, 0xFFFF0000, Data::LineChart::LS_LINE);
				chart->Plot(this->dispImg, 0, 0, UOSInt2Double(sz.x), UOSInt2Double(sz.y));
				DEL_CLASS(chart);
			}
			else
			{
				UOSInt recvCnt = this->currTopic->recvCnt;
				Int64 *dateList = MemAlloc(Int64, 256);
				Double *valueList = MemAlloc(Double, 256);
				if (recvCnt & 255)
				{
					MemCopyNO(dateList, &this->currTopic->dateList[recvCnt & 255], sizeof(Int64) * (256 - (recvCnt & 255)));
					MemCopyNO(valueList, &this->currTopic->valueList[recvCnt & 255], sizeof(Double) * (256 - (recvCnt & 255)));
					MemCopyNO(&dateList[256 - (recvCnt & 255)], this->currTopic->dateList, sizeof(Int64) * (recvCnt & 255));
					MemCopyNO(&valueList[256 - (recvCnt & 255)], this->currTopic->valueList, sizeof(Double) * (recvCnt & 255));
				}
				else
				{
					MemCopyNO(dateList, this->currTopic->dateList, sizeof(Int64) * 256);
					MemCopyNO(valueList, this->currTopic->valueList, sizeof(Double) * 256);
				}
				
				Data::LineChart *chart;
				NEW_CLASS(chart, Data::LineChart(CSTR_NULL));
				chart->AddXDataDate(this->currTopic->dateList, 256);
				chart->AddYData(this->currTopic->topic.Ptr(), this->currTopic->valueList, 256, 0xFFFF0000, Data::LineChart::LS_LINE);
				chart->Plot(this->dispImg, 0, 0, UOSInt2Double(sz.x), UOSInt2Double(sz.y));
				DEL_CLASS(chart);
				MemFree(dateList);
				MemFree(valueList);
			}
			
		}
		
		this->pbTopic->SetImageDImg(this->dispImg);
	}
}

void SSWR::AVIRead::AVIRMQTTSubscribeForm::ServerStop()
{
	if (this->client)
	{
		DEL_CLASS(this->client);
		this->client = 0;
	}
}

SSWR::AVIRead::AVIRMQTTSubscribeForm::AVIRMQTTSubscribeForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MQTT Subsribe"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->currTopic = 0;
	this->dispImg = 0;

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this->tpStatus, CSTR("Host")));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this->tpStatus, CSTR("127.0.0.1")));
	this->txtHost->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->tpStatus, CSTR("Port")));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->tpStatus, CSTR("1883")));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblUsername, UI::GUILabel(ui, this->tpStatus, CSTR("User Name")));
	this->lblUsername->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtUsername, UI::GUITextBox(ui, this->tpStatus, CSTR("")));
	this->txtUsername->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this->tpStatus, CSTR("Password")));
	this->lblPassword->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this->tpStatus, CSTR("")));
	this->txtPassword->SetRect(104, 76, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->tpStatus, CSTR("Start")));
	this->btnStart->SetRect(104, 100, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);

	this->tpSTopic = this->tcMain->AddTabPage(CSTR("Subscribe Topic"));
	NEW_CLASS(this->lblSTopic, UI::GUILabel(ui, this->tpSTopic, CSTR("Topic")));
	this->lblSTopic->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSTopic, UI::GUITextBox(ui, this->tpSTopic, CSTR("")));
	this->txtSTopic->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->btnSTopic, UI::GUIButton(ui, this->tpSTopic, CSTR("Subscribe")));
	this->btnSTopic->SetRect(254, 4, 75, 23, false);
	this->btnSTopic->HandleButtonClick(OnSTopicClicked, this);
	NEW_CLASS(this->lbSTopic, UI::GUIListBox(ui, this->tpSTopic, false));
	this->lbSTopic->SetRect(104, 28, 150, 119, false);

	this->tpPublish = this->tcMain->AddTabPage(CSTR("Publish"));
	NEW_CLASS(this->lblPublishTopic, UI::GUILabel(ui, this->tpPublish, CSTR("Topic")));
	this->lblPublishTopic->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPublishTopic, UI::GUITextBox(ui, this->tpPublish, CSTR("")));
	this->txtPublishTopic->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblPublishMessage, UI::GUILabel(ui, this->tpPublish, CSTR("Message")));
	this->lblPublishMessage->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPublishMessage, UI::GUITextBox(ui, this->tpPublish, CSTR("")));
	this->txtPublishMessage->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->btnPublish, UI::GUIButton(ui, this->tpPublish, CSTR("Publish")));
	this->btnPublish->SetRect(104, 52, 75, 23, false);
	this->btnPublish->HandleButtonClick(OnPublishClicked, this);

	this->tpTopic = this->tcMain->AddTabPage(CSTR("Topics"));
	NEW_CLASS(this->pbTopic, UI::GUIPictureBoxSimple(ui, this->tpTopic, this->core->GetDrawEngine(), false));
	this->pbTopic->SetRect(0, 0, 100, 300, false);
	this->pbTopic->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->vspTopic, UI::GUIVSplitter(ui, this->tpTopic, 3, true));
	NEW_CLASS(this->lvTopic, UI::GUIListView(ui, this->tpTopic, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvTopic->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTopic->SetShowGrid(true);
	this->lvTopic->SetFullRowSelect(true);
	this->lvTopic->AddColumn(CSTR("Topic"), 200);
	this->lvTopic->AddColumn(CSTR("Message"), 200);
	this->lvTopic->AddColumn(CSTR("Count"), 60);
	this->lvTopic->AddColumn(CSTR("Update Time"), 150);
	this->lvTopic->HandleSelChg(OnTopicSelChg, this);

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
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);
	this->client = 0;

	this->AddTimer(30000, OnPingTimerTick, this);
	this->AddTimer(1000, OnTimerTick, this);
	this->UpdateTopicChart();
}

SSWR::AVIRead::AVIRMQTTSubscribeForm::~AVIRMQTTSubscribeForm()
{
	this->ServerStop();
	UOSInt i;
	TopicStatus *topicSt;
	const Data::ArrayList<TopicStatus*> *topicList;
	topicList = this->topicMap.GetValues();
	i = topicList->GetCount();
	while (i-- > 0)
	{
		topicSt = topicList->GetItem(i);
		topicSt->topic->Release();
		MemFree(topicSt->currValue);
		MemFree(topicSt);
	}
	this->log.RemoveLogHandler(this->logger);
	DEL_CLASS(this->logger);
	if (this->dispImg)
	{
		this->core->GetDrawEngine()->DeleteImage(this->dispImg);
		this->dispImg = 0;
	}
}

void SSWR::AVIRead::AVIRMQTTSubscribeForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
