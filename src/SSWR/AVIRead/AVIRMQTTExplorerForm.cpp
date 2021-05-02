#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/LineChart.h"
#include "Math/Math.h"
#include "Media/ColorProfile.h"
#include "SSWR/AVIRead/AVIRMQTTExplorerForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTExplorerForm *me = (SSWR::AVIRead::AVIRMQTTExplorerForm*)userObj;
	if (me->client)
	{
		me->ServerStop();
		me->txtHost->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		me->txtUsername->SetReadOnly(false);
		me->txtPassword->SetReadOnly(false);
		me->lvTopic->ClearItems();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		Net::SocketUtil::AddressInfo addr;
		Int32 port;
		me->txtPort->GetText(&sb);
		if (!sb.ToInt32(&port))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Port is not valid", (const UTF8Char*)"Error", me);
			return;
		}
		else if (port <= 0 || port >= 65536)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Port is out of range", (const UTF8Char*)"Error", me);
			return;
		}
		sb.ClearStr();
		me->txtHost->GetText(&sb);
		if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToString(), &addr))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in parsing host", (const UTF8Char*)"Error", me);
			return;
		}

		NEW_CLASS(me->client, Net::MQTTClient(me->core->GetSocketFactory(), &addr, (UInt16)port));
		if (me->client->IsError())
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in connecting to server", (const UTF8Char*)"Error", me);
			DEL_CLASS(me->client);
			me->client = 0;
			return;
		}
		me->client->HandlePublishMessage(OnPublishMessage, me);

		const UTF8Char *username = 0;
		const UTF8Char *password = 0;
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		sb.ClearStr();
		me->txtUsername->GetText(&sb);
		if (sb.GetLength() > 0)
		{
			username = Text::StrCopyNew(sb.ToString());
		}
		sb.ClearStr();
		me->txtPassword->GetText(&sb);
		if (sb.GetLength() > 0)
		{
			password = Text::StrCopyNew(sb.ToString());
		}
		sb.ClearStr();
		sb.Append((const UTF8Char*)"sswrMQTT/");
		sb.AppendI64(dt.ToTicks());
		Bool succ = me->client->SendConnect(4, 30, sb.ToString(), username, password);
		SDEL_TEXT(username);
		SDEL_TEXT(password);
		if (succ)
		{
			succ = (me->client->WaitConnAck(30000) == Net::MQTTClient::CS_ACCEPTED);
		}

		if (succ)
		{
			succ = false;
			if (me->client->SendSubscribe(1, (const UTF8Char*)"#"))
			{
				if (me->client->WaitSubAck(1, 30000) <= 2)
				{
					succ = true;
				}
			}
		}
		if (succ)
		{
			succ = false;
			if (me->client->SendSubscribe(2, (const UTF8Char*)"$SYS/#"))
			{
				if (me->client->WaitSubAck(2, 30000) <= 2)
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
			me->lvTopic->ClearItems();
		}
		else
		{
			DEL_CLASS(me->client);
			me->client = 0;
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in communicating with server", (const UTF8Char *)"Error", me);
			return;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnTopicSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTExplorerForm *me = (SSWR::AVIRead::AVIRMQTTExplorerForm*)userObj;
	me->currTopic = (SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus*)me->lvTopic->GetSelectedItem();
	me->UpdateTopicChart();
}

void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnPingTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTExplorerForm *me = (SSWR::AVIRead::AVIRMQTTExplorerForm*)userObj;
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

void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTExplorerForm *me = (SSWR::AVIRead::AVIRMQTTExplorerForm*)userObj;
	Data::ArrayList<SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus*> *topicList;
	Data::DateTime dt;
	UTF8Char sbuff[32];
	SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus *topicSt;
	UOSInt i;
	UOSInt j;
	Sync::MutexUsage mutUsage(me->topicMut);
	topicList = me->topicMap->GetValues();
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
			me->lvTopic->SetSubItem(i, 1, topicSt->currValue);
			Text::StrUOSInt(sbuff, topicSt->recvCnt);
			me->lvTopic->SetSubItem(i, 2, sbuff);
			dt.SetTicks(topicSt->lastRecvTime);
			dt.ToLocalTime();
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->lvTopic->SetSubItem(i, 3, sbuff);
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
				me->lvTopic->SetSubItem(i, 1, topicSt->currValue);
				Text::StrUOSInt(sbuff, topicSt->recvCnt);
				me->lvTopic->SetSubItem(i, 2, sbuff);
				dt.SetTicks(topicSt->lastRecvTime);
				dt.ToLocalTime();
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				me->lvTopic->SetSubItem(i, 3, sbuff);

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


void __stdcall SSWR::AVIRead::AVIRMQTTExplorerForm::OnPublishMessage(void *userObj, const UTF8Char *topic, const UInt8 *message, UOSInt msgSize)
{
	SSWR::AVIRead::AVIRMQTTExplorerForm *me = (SSWR::AVIRead::AVIRMQTTExplorerForm*)userObj;
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"Received message, topic = ");
	sb.Append(topic);
	sb.Append((const UTF8Char*)", message = ");
	sb.AppendC((const UTF8Char*)message, msgSize);
	me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_COMMAND);

	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus *topicSt;
	Sync::MutexUsage mutUsage(me->topicMut);
	topicSt = me->topicMap->Get(topic);
	if (topicSt == 0)
	{
		topicSt = MemAlloc(SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus, 1);
		topicSt->topic = Text::StrCopyNew(topic);
		topicSt->currValue = MemAlloc(UTF8Char, msgSize + 1);
		Text::StrConcatC(topicSt->currValue, message, msgSize);
		topicSt->updated = true;
		topicSt->recvCnt = 1;
		topicSt->lastRecvTime = dt.ToTicks();
		me->topicMap->Put(topic, topicSt);
		me->topicListChanged = true;
	}
	else
	{
		MemFree(topicSt->currValue);
		topicSt->currValue = MemAlloc(UTF8Char, msgSize + 1);
		Text::StrConcatC(topicSt->currValue, message, msgSize);	
		topicSt->updated = true;
		topicSt->recvCnt++;
		topicSt->lastRecvTime = dt.ToTicks();
	}
	Double dVal;
	OSInt i;
	topicSt->dateList[(topicSt->recvCnt - 1) & 255] = topicSt->lastRecvTime;
	if (Text::StrToDouble(topicSt->currValue, &dVal))
	{
		topicSt->valueList[(topicSt->recvCnt - 1) & 255] = dVal;
	}
	else
	{
		dVal = 0;
		i = Text::StrIndexOf(topicSt->currValue, (const UTF8Char*)" seconds");
		if (i > 0)
		{
			sb.ClearStr();
			sb.AppendC(topicSt->currValue, (UOSInt)i);
			Text::StrToDouble(sb.ToString(), &dVal);
		}
		topicSt->valueList[(topicSt->recvCnt - 1) & 255] = dVal;
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRMQTTExplorerForm::UpdateTopicChart()
{
	Media::DrawImage *dimg;
	Media::DrawEngine *deng = this->core->GetDrawEngine();
	UOSInt w;
	UOSInt h;
	this->pbTopic->GetSizeP(&w, &h);
	if (w > 0 && h > 0)
	{
		if (this->dispImg == 0 || this->dispImg->GetWidth() != w || this->dispImg->GetHeight() != h)
		{
			if (this->dispImg)
			{
				deng->DeleteImage(this->dispImg);
			}
			dimg = deng->CreateImage32(w, h, Media::AT_NO_ALPHA);
			this->dispImg = dimg;
		}
		if (this->currTopic == 0 || this->currTopic->recvCnt <= 1)
		{
			Media::DrawBrush *b;
			b = this->dispImg->NewBrushARGB(0xffffffff);
			this->dispImg->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h), 0, b);
			this->dispImg->DelBrush(b);
		}
		else
		{
			if (this->currTopic->recvCnt < 256)
			{
				UOSInt recvCnt = this->currTopic->recvCnt;
				Data::LineChart *chart;
				NEW_CLASS(chart, Data::LineChart(0));
				chart->AddXDataDate(this->currTopic->dateList, recvCnt);
				chart->AddYData(this->currTopic->topic, this->currTopic->valueList, recvCnt, 0xFFFF0000, Data::LineChart::LS_LINE);
				chart->Plot(this->dispImg, 0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h));
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
				NEW_CLASS(chart, Data::LineChart(0));
				chart->AddXDataDate(this->currTopic->dateList, 256);
				chart->AddYData(this->currTopic->topic, this->currTopic->valueList, 256, 0xFFFF0000, Data::LineChart::LS_LINE);
				chart->Plot(this->dispImg, 0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h));
				DEL_CLASS(chart);
				MemFree(dateList);
				MemFree(valueList);
			}
			
		}
		
		this->pbTopic->SetImageDImg(this->dispImg);
	}
}

void SSWR::AVIRead::AVIRMQTTExplorerForm::ServerStop()
{
	if (this->client)
	{
		DEL_CLASS(this->client);
		this->client = 0;
	}
}

SSWR::AVIRead::AVIRMQTTExplorerForm::AVIRMQTTExplorerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"MQTT Explorer");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->topicMut, Sync::Mutex());
	NEW_CLASS(this->topicMap, Data::StringUTF8Map<SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus*>());
	this->currTopic = 0;
	this->dispImg = 0;

	NEW_CLASS(this->pnlConnect, UI::GUIPanel(ui, this));
	this->pnlConnect->SetRect(0, 0, 100, 55, false);
	this->pnlConnect->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this->pnlConnect, (const UTF8Char*)"Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this->pnlConnect, (const UTF8Char*)"127.0.0.1"));
	this->txtHost->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlConnect, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlConnect, (const UTF8Char*)"1883"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblUsername, UI::GUILabel(ui, this->pnlConnect, (const UTF8Char*)"User Name"));
	this->lblUsername->SetRect(254, 4, 100, 23, false);
	NEW_CLASS(this->txtUsername, UI::GUITextBox(ui, this->pnlConnect, (const UTF8Char*)""));
	this->txtUsername->SetRect(354, 4, 100, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this->pnlConnect, (const UTF8Char*)"Password"));
	this->lblPassword->SetRect(254, 28, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this->pnlConnect, (const UTF8Char*)""));
	this->txtPassword->SetRect(354, 28, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlConnect, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(504, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->pbTopic, UI::GUIPictureBoxSimple(ui, this, this->core->GetDrawEngine(), false));
	this->pbTopic->SetRect(0, 0, 100, 300, false);
	this->pbTopic->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->vspTopic, UI::GUIVSplitter(ui, this, 3, true));
	NEW_CLASS(this->lvTopic, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvTopic->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTopic->SetShowGrid(true);
	this->lvTopic->SetFullRowSelect(true);
	this->lvTopic->AddColumn((const UTF8Char*)"Topic", 200);
	this->lvTopic->AddColumn((const UTF8Char*)"Message", 200);
	this->lvTopic->AddColumn((const UTF8Char*)"Count", 60);
	this->lvTopic->AddColumn((const UTF8Char*)"Update Time", 150);
	this->lvTopic->HandleSelChg(OnTopicSelChg, this);


	NEW_CLASS(this->log, IO::LogTool());
	this->client = 0;

	this->AddTimer(30000, OnPingTimerTick, this);
	this->AddTimer(1000, OnTimerTick, this);
	this->UpdateTopicChart();
}

SSWR::AVIRead::AVIRMQTTExplorerForm::~AVIRMQTTExplorerForm()
{
	this->ServerStop();
	UOSInt i;
	SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus *topicSt;
	Data::ArrayList<SSWR::AVIRead::AVIRMQTTExplorerForm::TopicStatus*> *topicList;
	DEL_CLASS(this->topicMut);
	topicList = this->topicMap->GetValues();
	i = topicList->GetCount();
	while (i-- > 0)
	{
		topicSt = topicList->GetItem(i);
		Text::StrDelNew(topicSt->topic);
		MemFree(topicSt->currValue);
		MemFree(topicSt);
	}
	DEL_CLASS(this->topicMap);
	DEL_CLASS(this->log);
	if (this->dispImg)
	{
		this->core->GetDrawEngine()->DeleteImage(this->dispImg);
		this->dispImg = 0;
	}
}

void SSWR::AVIRead::AVIRMQTTExplorerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
