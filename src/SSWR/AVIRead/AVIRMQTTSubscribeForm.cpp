#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Data/ChartPlotter.h"
#include "Math/Math_C.h"
#include "Media/ColorProfile.h"
#include "SSWR/AVIRead/AVIRMQTTSubscribeForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeForm>();
	if (me->client.NotNull())
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
		me->txtPort->GetText(sb);
		if (!sb.ToInt32(port))
		{
			me->ui->ShowMsgOK(CSTR("Port is not valid"), CSTR("Error"), me);
			return;
		}
		else if (port <= 0 || port >= 65536)
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

		NN<Net::MQTTConn> client;
		NEW_CLASSNN(client, Net::MQTTConn(me->core->GetTCPClientFactory(), nullptr, sb.ToCString(), (UInt16)port, 0, 0, 30000));
		if (client->IsError())
		{
			me->ui->ShowMsgOK(CSTR("Error in connecting to server"), CSTR("Error"), me);
			client.Delete();
			return;
		}
		me->client = client;
		client->HandlePublishMessage(OnPublishMessage, me);

		Text::CString username = nullptr;
		Text::CString password = nullptr;
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
			Net::MQTTConn::ConnectStatus status = client->WaitConnAck(30000);
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
			client.Delete();
			me->client = nullptr;
			me->ui->ShowMsgOK(CSTR("Error in communicating with server"), CSTR("Error"), me);
			return;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnSTopicClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeForm>();
	NN<Net::MQTTConn> client;
	if (me->client.SetTo(client))
	{
		Text::StringBuilderUTF8 sb;
		me->txtSTopic->GetText(sb);
		if (sb.GetLength() > 0)
		{
			if (client->SendSubscribe(1, sb.ToCString()))
			{
				if (client->WaitSubAck(1, 30000) <= 2)
				{
					me->txtSTopic->SetText(CSTR(""));
					me->lbSTopic->AddItem(sb.ToCString(), 0);
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeForm>();
	NN<Text::String> s;
	if (me->lbLog->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnTopicSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeForm>();
	me->currTopic = me->lvTopic->GetSelectedItem().GetOpt<TopicStatus>();
	me->UpdateTopicChart();
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnPublishClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeForm>();
	NN<Net::MQTTConn> client;
	if (me->client.SetTo(client))
	{
		Text::StringBuilderUTF8 sbTopic;
		Text::StringBuilderUTF8 sbMsg;
		me->txtPublishTopic->GetText(sbTopic);
		me->txtPublishMessage->GetText(sbMsg);
		if (sbTopic.GetLength() > 0 && sbMsg.GetLength() > 0)
		{
			if (client->SendPublish(sbTopic.ToCString(), sbMsg.ToCString(), me->chkPublishDUP->IsChecked(), 0, me->chkPublishRetain->IsChecked()))
			{
				me->txtPublishMessage->SetText(CSTR(""));
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnPingTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeForm>();
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
		}
		else
		{
			client->ClearPackets();
			client->SendPing();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeForm>();
	NN<const Data::ArrayListNN<TopicStatus>> topicList;
	Data::DateTime dt;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<TopicStatus> topicSt;
	UOSInt i;
	UOSInt j;
	Sync::MutexUsage mutUsage(me->topicMut);
	topicList = me->topicMap.GetValues();
	i = 0;
	j = topicList->GetCount();
	if (me->topicListChanged)
	{
		me->topicListChanged = false;
		me->lvTopic->ClearItems();
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
			topicSt = topicList->GetItemNoCheck(i);
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

				if (topicSt.Ptr() == me->currTopic.OrNull())
				{
					me->UpdateTopicChart();
				}
			}
			i++;
		}
	}
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRMQTTSubscribeForm::OnPublishMessage(AnyType userObj, Text::CStringNN topic, const Data::ByteArrayR &message)
{
	NN<SSWR::AVIRead::AVIRMQTTSubscribeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTSubscribeForm>();
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Received message, topic = "));
	sb.Append(topic);
	sb.AppendC(UTF8STRC(", message = "));
	sb.AppendC(message.Arr(), message.GetSize());
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);

	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	NN<TopicStatus> topicSt;
	Sync::MutexUsage mutUsage(me->topicMut);
	if (!me->topicMap.GetC(topic).SetTo(topicSt))
	{
		topicSt = MemAllocNN(TopicStatus);
		topicSt->topic = Text::String::New(topic);
		topicSt->currValue = MemAllocArr(UTF8Char, message.GetSize() + 1);
		Text::StrConcatC(topicSt->currValue, message.Arr(), message.GetSize());
		topicSt->currValueLen = message.GetSize();
		topicSt->updated = true;
		topicSt->recvCnt = 1;
		topicSt->lastRecvTime = dt.ToTicks();
		me->topicMap.PutC(topic, topicSt);
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
		topicSt->lastRecvTime = dt.ToTicks();
	}
	Double dVal;
	UOSInt i;
	topicSt->dateList[(topicSt->recvCnt - 1) & 255] = topicSt->lastRecvTime;
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
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRMQTTSubscribeForm::UpdateTopicChart()
{
	Optional<Media::DrawImage> dimg;
	NN<Media::DrawEngine> deng = this->core->GetDrawEngine();
	Math::Size2D<UOSInt> sz = this->pbTopic->GetSizeP();
	NN<Media::DrawImage> gimg;
	if (sz.x > 0 && sz.y > 0)
	{
		if (!this->dispImg.SetTo(gimg) || gimg->GetSize() != sz)
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
				gimg->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), nullptr, b);
				gimg->DelBrush(b);
			}
			else
			{
				if (currTopic->recvCnt < 256)
				{
					UOSInt recvCnt = currTopic->recvCnt;
					Data::ChartPlotter *chart;
					NEW_CLASS(chart, Data::ChartPlotter(nullptr));
					chart->AddLineChart(currTopic->topic, Data::ChartPlotter::NewData(currTopic->valueList, recvCnt), Data::ChartPlotter::NewDataDate(currTopic->dateList, recvCnt), 0xFFFF0000);
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
					NEW_CLASS(chart, Data::ChartPlotter(nullptr));
					chart->AddLineChart(currTopic->topic, Data::ChartPlotter::NewData(currTopic->valueList, 256), Data::ChartPlotter::NewDataDate(currTopic->dateList, 256), 0xFFFF0000);
					chart->Plot(gimg, 0, 0, UOSInt2Double(sz.x), UOSInt2Double(sz.y));
					DEL_CLASS(chart);
					MemFree(dateList);
					MemFree(valueList);
				}
			}
		}
		
		this->pbTopic->SetImageDImg(this->dispImg);
	}
}

void SSWR::AVIRead::AVIRMQTTSubscribeForm::ServerStop()
{
	this->client.Delete();
}

SSWR::AVIRead::AVIRMQTTSubscribeForm::AVIRMQTTSubscribeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("MQTT Subsribe"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->currTopic = nullptr;
	this->dispImg = nullptr;

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->lblHost = ui->NewLabel(this->tpStatus, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(this->tpStatus, CSTR("127.0.0.1"));
	this->txtHost->SetRect(104, 4, 100, 23, false);
	this->lblPort = ui->NewLabel(this->tpStatus, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->tpStatus, CSTR("1883"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	this->lblUsername = ui->NewLabel(this->tpStatus, CSTR("User Name"));
	this->lblUsername->SetRect(4, 52, 100, 23, false);
	this->txtUsername = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtUsername->SetRect(104, 52, 100, 23, false);
	this->lblPassword = ui->NewLabel(this->tpStatus, CSTR("Password"));
	this->lblPassword->SetRect(4, 76, 100, 23, false);
	this->txtPassword = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtPassword->SetRect(104, 76, 100, 23, false);
	this->btnStart = ui->NewButton(this->tpStatus, CSTR("Start"));
	this->btnStart->SetRect(104, 100, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);

	this->tpSTopic = this->tcMain->AddTabPage(CSTR("Subscribe Topic"));
	this->lblSTopic = ui->NewLabel(this->tpSTopic, CSTR("Topic"));
	this->lblSTopic->SetRect(4, 4, 100, 23, false);
	this->txtSTopic = ui->NewTextBox(this->tpSTopic, CSTR(""));
	this->txtSTopic->SetRect(104, 4, 150, 23, false);
	this->btnSTopic = ui->NewButton(this->tpSTopic, CSTR("Subscribe"));
	this->btnSTopic->SetRect(254, 4, 75, 23, false);
	this->btnSTopic->HandleButtonClick(OnSTopicClicked, this);
	this->lbSTopic = ui->NewListBox(this->tpSTopic, false);
	this->lbSTopic->SetRect(104, 28, 150, 119, false);

	this->tpPublish = this->tcMain->AddTabPage(CSTR("Publish"));
	this->lblPublishTopic = ui->NewLabel(this->tpPublish, CSTR("Topic"));
	this->lblPublishTopic->SetRect(4, 4, 100, 23, false);
	this->txtPublishTopic = ui->NewTextBox(this->tpPublish, CSTR(""));
	this->txtPublishTopic->SetRect(104, 4, 200, 23, false);
	this->lblPublishMessage = ui->NewLabel(this->tpPublish, CSTR("Message"));
	this->lblPublishMessage->SetRect(4, 28, 100, 23, false);
	this->txtPublishMessage = ui->NewTextBox(this->tpPublish, CSTR(""));
	this->txtPublishMessage->SetRect(104, 28, 200, 23, false);
	this->chkPublishDUP = ui->NewCheckBox(this->tpPublish, CSTR("DUP"), false);
	this->chkPublishDUP->SetRect(104, 52, 100, 23, false);
	this->chkPublishRetain = ui->NewCheckBox(this->tpPublish, CSTR("DUP"), false);
	this->chkPublishRetain->SetRect(104, 76, 100, 23, false);
	this->btnPublish = ui->NewButton(this->tpPublish, CSTR("Publish"));
	this->btnPublish->SetRect(104, 100, 75, 23, false);
	this->btnPublish->HandleButtonClick(OnPublishClicked, this);

	this->tpTopic = this->tcMain->AddTabPage(CSTR("Topics"));
	this->pbTopic = ui->NewPictureBoxSimple(this->tpTopic, this->core->GetDrawEngine(), false);
	this->pbTopic->SetRect(0, 0, 100, 300, false);
	this->pbTopic->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->vspTopic = ui->NewVSplitter(this->tpTopic, 3, true);
	this->lvTopic = ui->NewListView(this->tpTopic, UI::ListViewStyle::Table, 4);
	this->lvTopic->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTopic->SetShowGrid(true);
	this->lvTopic->SetFullRowSelect(true);
	this->lvTopic->AddColumn(CSTR("Topic"), 200);
	this->lvTopic->AddColumn(CSTR("Message"), 200);
	this->lvTopic->AddColumn(CSTR("Count"), 60);
	this->lvTopic->AddColumn(CSTR("Update Time"), 150);
	this->lvTopic->HandleSelChg(OnTopicSelChg, this);

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
	this->client = nullptr;

	this->AddTimer(30000, OnPingTimerTick, this);
	this->AddTimer(1000, OnTimerTick, this);
	this->UpdateTopicChart();
}

SSWR::AVIRead::AVIRMQTTSubscribeForm::~AVIRMQTTSubscribeForm()
{
	this->ServerStop();
	UOSInt i;
	NN<TopicStatus> topicSt;
	NN<const Data::ArrayListNN<TopicStatus>> topicList;
	NN<Media::DrawImage> img;
	topicList = this->topicMap.GetValues();
	i = topicList->GetCount();
	while (i-- > 0)
	{
		topicSt = topicList->GetItemNoCheck(i);
		topicSt->topic->Release();
		MemFreeArr(topicSt->currValue);
		MemFreeNN(topicSt);
	}
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	if (this->dispImg.SetTo(img))
	{
		this->core->GetDrawEngine()->DeleteImage(img);
		this->dispImg = nullptr;
	}
}

void SSWR::AVIRead::AVIRMQTTSubscribeForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
