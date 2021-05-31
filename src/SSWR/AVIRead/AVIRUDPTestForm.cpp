#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "SSWR/AVIRead/AVIRUDPTestForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRUDPTestForm::OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	SSWR::AVIRead::AVIRUDPTestForm *me = (SSWR::AVIRead::AVIRUDPTestForm*)userData;
	if (me->autoReply)
	{
		me->udp->SendTo(addr, port, buff, dataSize);
	}
	Sync::MutexUsage mutUsage(me->mut);
	me->recvCnt++;
	me->recvSize += dataSize;
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRUDPTestForm::OnAutoReplyChanged(void *userObj, Bool newVal)
{
	SSWR::AVIRead::AVIRUDPTestForm *me = (SSWR::AVIRead::AVIRUDPTestForm*)userObj;
	me->autoReply = newVal;
}

void __stdcall SSWR::AVIRead::AVIRUDPTestForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUDPTestForm *me = (SSWR::AVIRead::AVIRUDPTestForm*)userObj;
	if (me->udp)
	{
		DEL_CLASS(me->udp);
		me->udp = 0;
		me->txtServerPort->SetReadOnly(false);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		UInt16 port;
		me->txtServerPort->GetText(&sb);
		port = 0;
		sb.ToUInt16(&port);
		if (port > 0 && port < 65536)
		{
			NEW_CLASS(me->udp, Net::UDPServer(me->sockf, 0, port, 0, OnUDPPacket, me, 0, 0, 5, false));
			if (me->udp->IsError())
			{
				DEL_CLASS(me->udp);
				me->udp = 0;
			}
			else
			{
				me->udp->SetBuffSize(10240);
				me->txtServerPort->SetReadOnly(true);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPTestForm::OnSendClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUDPTestForm *me = (SSWR::AVIRead::AVIRUDPTestForm*)userObj;
	Net::SocketUtil::AddressInfo addr;
	Int32 port;
	Int32 cnt;
	Text::StringBuilderUTF8 sb;
	if (me->udp == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"You should start server first", (const UTF8Char *)"UDP Test", me);
		return;
	}
	me->txtDestHost->GetText(&sb);
	if (!me->sockf->DNSResolveIP(sb.ToString(), &addr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Host is not valid", (const UTF8Char *)"UDP Test", me);
		return;
	}
	sb.ClearStr();
	me->txtDestPort->GetText(&sb);
	port = sb.ToInt32();
	if (port <= 0 || port >= 65536)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Port is not valid", (const UTF8Char *)"UDP Test", me);
		return;
	}
	sb.ClearStr();
	me->txtDestCount->GetText(&sb);
	cnt = sb.ToInt32();
	if (cnt <= 0 || cnt > 10000000)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Count is not valid", (const UTF8Char *)"UDP Test", me);
		return;
	}
	OSInt i = me->threadCnt;
	OSInt lastCnt = cnt;
	OSInt thisCnt;
	while (i-- > 0)
	{
		thisCnt = (Int32)(i * (Int64)cnt / me->threadCnt);
		me->threads[i].reqCnt = lastCnt - thisCnt;
		me->threads[i].destAddr = addr;
		me->threads[i].destPort = port;
		me->threads[i].taskType = 1;
		me->threads[i].evt->Set();
		lastCnt = thisCnt;
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPTestForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRUDPTestForm *me = (SSWR::AVIRead::AVIRUDPTestForm*)userObj;
	UTF8Char sbuff[32];
	UInt64 thisRecvCnt = me->recvCnt;
	UInt64 thisRecvSize = me->recvSize;
	UInt64 sendSuccCnt;
	UInt64 sendFailCnt;
	Data::DateTime dt;
	Int64 diffMS;
	OSInt i;
	sendSuccCnt = 0;
	sendFailCnt = 0;
	i = me->threadCnt;
	while (i-- > 0)
	{
		sendSuccCnt += me->threads[i].sentSuccCnt;
		sendFailCnt += me->threads[i].sentFailCnt;
	}
	dt.SetCurrTimeUTC();
	diffMS = dt.DiffMS(me->lastTime);
	Text::StrUInt64(sbuff, thisRecvCnt);
	me->txtRecvCnt->SetText(sbuff);
	Text::StrUInt64(sbuff, thisRecvSize);
	me->txtRecvSize->SetText(sbuff);
	Text::StrUInt64(sbuff, sendSuccCnt);
	me->txtSentSuccCnt->SetText(sbuff);
	Text::StrUInt64(sbuff, sendFailCnt);
	me->txtSentFailCnt->SetText(sbuff);
	if (diffMS == 0)
	{
		me->txtRecvCntRate->SetText((const UTF8Char*)"0");
		me->txtRecvSizeRate->SetText((const UTF8Char*)"0");
		me->txtSentSuccCntRate->SetText((const UTF8Char*)"0");
		me->txtSentFailCntRate->SetText((const UTF8Char*)"0");
	}
	else
	{
		Text::StrDouble(sbuff, (thisRecvCnt - me->lastRecvCnt) * 1000.0 / diffMS);
		me->txtRecvCntRate->SetText(sbuff);
		Text::StrDouble(sbuff, (thisRecvSize - me->lastRecvSize) * 1000.0 / diffMS);
		me->txtRecvSizeRate->SetText(sbuff);
		Text::StrDouble(sbuff, (sendSuccCnt - me->lastSentSuccCnt) * 1000.0 / diffMS);
		me->txtSentSuccCntRate->SetText(sbuff);
		Text::StrDouble(sbuff, (sendFailCnt - me->lastSentFailCnt) * 1000.0 / diffMS);
		me->txtSentFailCntRate->SetText(sbuff);
	}
	me->lastRecvCnt = thisRecvCnt;
	me->lastRecvSize = thisRecvSize;
	me->lastSentSuccCnt = sendSuccCnt;
	me->lastSentFailCnt = sendFailCnt;
	me->lastTime->SetValue(&dt);
}

UInt32 __stdcall SSWR::AVIRead::AVIRUDPTestForm::ProcThread(void *userObj)
{
	ThreadStatus *t = (ThreadStatus*)userObj;
	UInt8 buff[32];
	OSInt i;
	Net::SocketUtil::AddressInfo destAddr;
	UInt16 destPort;
	t->status = 1;
	t->me->mainEvt->Set();
	MemClear(buff, 32);
	while (true)
	{
		if (t->taskType == 1)
		{
			i = t->reqCnt;
			destAddr = t->destAddr;
			destPort = t->destPort;
			t->status = 2;
			t->taskType = 0;
			while (i-- > 0)
			{
				if (t->me->udp->SendTo(&destAddr, destPort, buff, 32))
				{
					t->sentSuccCnt++;
				}
				else
				{
					t->sentFailCnt++;
				}
			}
			t->status = 1;
		}
		else if (t->taskType == 2)
		{
			break;
		}
		else
		{
			t->evt->Wait(1000);
		}
	}
	t->status = 0;
	t->me->mainEvt->Set();
	return 0;
}

SSWR::AVIRead::AVIRUDPTestForm::AVIRUDPTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 384, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"UDP Test");
	this->SetNoResize(true);

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->udp = 0;
	this->autoReply = false;
	NEW_CLASS(this->mut, Sync::Mutex());
	this->recvCnt = 0;
	this->recvSize = 0;
	this->lastRecvCnt = 0;
	this->lastRecvSize = 0;
	this->lastSentFailCnt = 0;
	this->lastSentSuccCnt = 0;
	NEW_CLASS(this->lastTime, Data::DateTime());
	this->lastTime->SetCurrTimeUTC();
	NEW_CLASS(this->mainEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRUDPTestForm.mainEvt"));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->grpServer, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Server"));
	this->grpServer->SetRect(0, 0, 200, 23, false);
	this->grpServer->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->lblServerPort, UI::GUILabel(ui, this->grpServer, (const UTF8Char*)"Port"));
	this->lblServerPort->SetRect(4, 4, 50, 23, false);
	NEW_CLASS(this->txtServerPort, UI::GUITextBox(ui, this->grpServer, (const UTF8Char*)""));
	this->txtServerPort->SetRect(54, 4, 75, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->grpServer, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(54, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->chkAutoReply, UI::GUICheckBox(ui, this->grpServer, (const UTF8Char*)"Auto Reply", false));
	this->chkAutoReply->SetRect(54, 52, 100, 23, false);
	this->chkAutoReply->HandleCheckedChange(OnAutoReplyChanged, this);
	NEW_CLASS(this->grpDest, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Dest"));
	this->grpDest->SetRect(0, 0, 250, 23, false);
	this->grpDest->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->lblDestHost, UI::GUILabel(ui, this->grpDest, (const UTF8Char*)"Host"));
	this->lblDestHost->SetRect(4, 4, 50, 23, false);
	NEW_CLASS(this->txtDestHost, UI::GUITextBox(ui, this->grpDest, (const UTF8Char*)"127.0.0.1"));
	this->txtDestHost->SetRect(54, 4, 100, 23, false);
	NEW_CLASS(this->lblDestPort, UI::GUILabel(ui, this->grpDest, (const UTF8Char*)"Port"));
	this->lblDestPort->SetRect(4, 28, 50, 23, false);
	NEW_CLASS(this->txtDestPort, UI::GUITextBox(ui, this->grpDest, (const UTF8Char*)""));
	this->txtDestPort->SetRect(54, 28, 50, 23, false);
	NEW_CLASS(this->lblDestCount, UI::GUILabel(ui, this->grpDest, (const UTF8Char*)"Count"));
	this->lblDestCount->SetRect(4, 52, 50, 23, false);
	NEW_CLASS(this->txtDestCount, UI::GUITextBox(ui, this->grpDest, (const UTF8Char*)"10000"));
	this->txtDestCount->SetRect(54, 52, 75, 23, false);
	NEW_CLASS(this->btnSend, UI::GUIButton(ui, this->grpDest, (const UTF8Char*)"Send"));
	this->btnSend->SetRect(54, 76, 75, 23, false);
	this->btnSend->HandleButtonClick(OnSendClicked, this);
	NEW_CLASS(this->grpStatus, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Status"));
	this->grpStatus->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblRecvCnt, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Recv Count"));
	this->lblRecvCnt->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtRecvCnt, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtRecvCnt->SetReadOnly(true);
	this->txtRecvCnt->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->txtRecvCntRate, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtRecvCntRate->SetReadOnly(true);
	this->txtRecvCntRate->SetRect(254, 4, 150, 23, false);
	NEW_CLASS(this->lblRecvSize, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Recv Size"));
	this->lblRecvSize->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtRecvSize, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtRecvSize->SetReadOnly(true);
	this->txtRecvSize->SetRect(104, 28, 150, 23, false);
	NEW_CLASS(this->txtRecvSizeRate, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtRecvSizeRate->SetReadOnly(true);
	this->txtRecvSizeRate->SetRect(254, 28, 150, 23, false);
	NEW_CLASS(this->lblSentSuccCnt, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Sent Succ Cnt"));
	this->lblSentSuccCnt->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtSentSuccCnt, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtSentSuccCnt->SetReadOnly(true);
	this->txtSentSuccCnt->SetRect(104, 52, 150, 23, false);
	NEW_CLASS(this->txtSentSuccCntRate, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtSentSuccCntRate->SetReadOnly(true);
	this->txtSentSuccCntRate->SetRect(254, 52, 150, 23, false);
	NEW_CLASS(this->lblSentFailCnt, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Sent Fail Cnt"));
	this->lblSentFailCnt->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtSentFailCnt, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtSentFailCnt->SetReadOnly(true);
	this->txtSentFailCnt->SetRect(104, 76, 150, 23, false);
	NEW_CLASS(this->txtSentFailCntRate, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtSentFailCntRate->SetReadOnly(true);
	this->txtSentFailCntRate->SetRect(254, 76, 150, 23, false);

	this->AddTimer(100, OnTimerTick, this);

	UOSInt i;
	this->threadCnt = Sync::Thread::GetThreadCnt();
	this->threads = MemAlloc(ThreadStatus, this->threadCnt);
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].destAddr.addrType = Net::SocketUtil::AT_UNKNOWN;
		this->threads[i].destPort = 0;
		NEW_CLASS(this->threads[i].evt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRUDPTestForm.threads.evt"));
		this->threads[i].me = this;
		this->threads[i].reqCnt = 0;
		this->threads[i].status = 0;
		this->threads[i].taskType = 0;
		this->threads[i].sentSuccCnt = 0;
		this->threads[i].sentFailCnt = 0;
		Sync::Thread::Create(ProcThread, &this->threads[i]);
	}
	Bool found = true;
	while (found)
	{
		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threads[i].status == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			break;
		}
		else
		{
			this->mainEvt->Wait(100);
		}
	}
}

SSWR::AVIRead::AVIRUDPTestForm::~AVIRUDPTestForm()
{
	UOSInt i;
	Bool found = true;
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].taskType = 2;
		this->threads[i].evt->Set();
	}
	while (found)
	{
		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threads[i].status != 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			break;
		}
		else
		{
			this->mainEvt->Wait(100);
		}
	}
	if (this->udp)
	{
		SDEL_CLASS(this->udp);
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->threads[i].evt);
	}
	MemFree(this->threads);
	DEL_CLASS(this->mut);
	DEL_CLASS(this->mainEvt);
	DEL_CLASS(this->lastTime);
}

void SSWR::AVIRead::AVIRUDPTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
