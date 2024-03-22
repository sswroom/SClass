#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "SSWR/AVIRead/AVIRUDPTestForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRUDPTestForm::OnUDPPacket(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData)
{
	NotNullPtr<SSWR::AVIRead::AVIRUDPTestForm> me = userData.GetNN<SSWR::AVIRead::AVIRUDPTestForm>();
	if (me->autoReply)
	{
		me->udp->SendTo(addr, port, buff, dataSize);
	}
	Sync::MutexUsage mutUsage(me->mut);
	me->recvCnt++;
	me->recvSize += dataSize;
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRUDPTestForm::OnAutoReplyChanged(AnyType userObj, Bool newVal)
{
	NotNullPtr<SSWR::AVIRead::AVIRUDPTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPTestForm>();
	me->autoReply = newVal;
}

void __stdcall SSWR::AVIRead::AVIRUDPTestForm::OnStartClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRUDPTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPTestForm>();
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
		me->txtServerPort->GetText(sb);
		port = 0;
		sb.ToUInt16(port);
		if (port > 0)
		{
			NEW_CLASS(me->udp, Net::UDPServer(me->sockf, 0, port, CSTR_NULL, OnUDPPacket, me, me->core->GetLog(), CSTR_NULL, 5, false));
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

void __stdcall SSWR::AVIRead::AVIRUDPTestForm::OnSendClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRUDPTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPTestForm>();
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	UInt32 cnt;
	Text::StringBuilderUTF8 sb;
	if (me->udp == 0)
	{
		me->ui->ShowMsgOK(CSTR("You should start server first"), CSTR("UDP Test"), me);
		return;
	}
	me->txtDestHost->GetText(sb);
	if (!me->sockf->DNSResolveIP(sb.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Host is not valid"), CSTR("UDP Test"), me);
		return;
	}
	sb.ClearStr();
	me->txtDestPort->GetText(sb);
	if (!sb.ToUInt16(port) || port <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Port is not valid"), CSTR("UDP Test"), me);
		return;
	}
	sb.ClearStr();
	me->txtDestCount->GetText(sb);
	if (!sb.ToUInt32(cnt) || cnt <= 0 || cnt > 10000000)
	{
		me->ui->ShowMsgOK(CSTR("Count is not valid"), CSTR("UDP Test"), me);
		return;
	}
	UOSInt i = me->threadCnt;
	UOSInt lastCnt = cnt;
	UOSInt thisCnt;
	while (i-- > 0)
	{
		thisCnt = (UOSInt)(i * (UInt64)cnt / me->threadCnt);
		me->threads[i].reqCnt = lastCnt - thisCnt;
		me->threads[i].destAddr = addr;
		me->threads[i].destPort = port;
		me->threads[i].taskType = 1;
		me->threads[i].evt->Set();
		lastCnt = thisCnt;
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPTestForm::OnTimerTick(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRUDPTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPTestForm>();
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UInt64 thisRecvCnt = me->recvCnt;
	UInt64 thisRecvSize = me->recvSize;
	UInt64 sendSuccCnt;
	UInt64 sendFailCnt;
	Data::DateTime dt;
	Int64 diffMS;
	UOSInt i;
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
	sptr = Text::StrUInt64(sbuff, thisRecvCnt);
	me->txtRecvCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt64(sbuff, thisRecvSize);
	me->txtRecvSize->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt64(sbuff, sendSuccCnt);
	me->txtSentSuccCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt64(sbuff, sendFailCnt);
	me->txtSentFailCnt->SetText(CSTRP(sbuff, sptr));
	if (diffMS == 0)
	{
		me->txtRecvCntRate->SetText(CSTR("0"));
		me->txtRecvSizeRate->SetText(CSTR("0"));
		me->txtSentSuccCntRate->SetText(CSTR("0"));
		me->txtSentFailCntRate->SetText(CSTR("0"));
	}
	else
	{
		sptr = Text::StrDouble(sbuff, (Double)(thisRecvCnt - me->lastRecvCnt) * 1000.0 / (Double)diffMS);
		me->txtRecvCntRate->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, (Double)(thisRecvSize - me->lastRecvSize) * 1000.0 / (Double)diffMS);
		me->txtRecvSizeRate->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, (Double)(sendSuccCnt - me->lastSentSuccCnt) * 1000.0 / (Double)diffMS);
		me->txtSentSuccCntRate->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, (Double)(sendFailCnt - me->lastSentFailCnt) * 1000.0 / (Double)diffMS);
		me->txtSentFailCntRate->SetText(CSTRP(sbuff, sptr));
	}
	me->lastRecvCnt = thisRecvCnt;
	me->lastRecvSize = thisRecvSize;
	me->lastSentSuccCnt = sendSuccCnt;
	me->lastSentFailCnt = sendFailCnt;
	me->lastTime.SetValue(dt);
}

UInt32 __stdcall SSWR::AVIRead::AVIRUDPTestForm::ProcThread(AnyType userObj)
{
	NotNullPtr<ThreadStatus> t = userObj.GetNN<ThreadStatus>();
	UInt8 buff[32];
	UOSInt i;
	Net::SocketUtil::AddressInfo destAddr;
	UInt16 destPort;
	t->status = 1;
	t->me->mainEvt.Set();
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
				if (t->me->udp->SendTo(destAddr, destPort, buff, 32))
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
	t->me->mainEvt.Set();
	return 0;
}

SSWR::AVIRead::AVIRUDPTestForm::AVIRUDPTestForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 384, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("UDP Test"));
	this->SetNoResize(true);

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->udp = 0;
	this->autoReply = false;
	this->recvCnt = 0;
	this->recvSize = 0;
	this->lastRecvCnt = 0;
	this->lastRecvSize = 0;
	this->lastSentFailCnt = 0;
	this->lastSentSuccCnt = 0;
	this->lastTime.SetCurrTimeUTC();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->grpServer = ui->NewGroupBox(*this, CSTR("Server"));
	this->grpServer->SetRect(0, 0, 200, 23, false);
	this->grpServer->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lblServerPort = ui->NewLabel(this->grpServer, CSTR("Port"));
	this->lblServerPort->SetRect(4, 4, 50, 23, false);
	this->txtServerPort = ui->NewTextBox(this->grpServer, CSTR(""));
	this->txtServerPort->SetRect(54, 4, 75, 23, false);
	this->btnStart = ui->NewButton(this->grpServer, CSTR("Start"));
	this->btnStart->SetRect(54, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->chkAutoReply = ui->NewCheckBox(this->grpServer, CSTR("Auto Reply"), false);
	this->chkAutoReply->SetRect(54, 52, 100, 23, false);
	this->chkAutoReply->HandleCheckedChange(OnAutoReplyChanged, this);
	this->grpDest = ui->NewGroupBox(*this, CSTR("Dest"));
	this->grpDest->SetRect(0, 0, 250, 23, false);
	this->grpDest->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lblDestHost = ui->NewLabel(this->grpDest, CSTR("Host"));
	this->lblDestHost->SetRect(4, 4, 50, 23, false);
	this->txtDestHost = ui->NewTextBox(this->grpDest, CSTR("127.0.0.1"));
	this->txtDestHost->SetRect(54, 4, 100, 23, false);
	this->lblDestPort = ui->NewLabel(this->grpDest, CSTR("Port"));
	this->lblDestPort->SetRect(4, 28, 50, 23, false);
	this->txtDestPort = ui->NewTextBox(this->grpDest, CSTR(""));
	this->txtDestPort->SetRect(54, 28, 50, 23, false);
	this->lblDestCount = ui->NewLabel(this->grpDest, CSTR("Count"));
	this->lblDestCount->SetRect(4, 52, 50, 23, false);
	this->txtDestCount = ui->NewTextBox(this->grpDest, CSTR("10000"));
	this->txtDestCount->SetRect(54, 52, 75, 23, false);
	this->btnSend = ui->NewButton(this->grpDest, CSTR("Send"));
	this->btnSend->SetRect(54, 76, 75, 23, false);
	this->btnSend->HandleButtonClick(OnSendClicked, this);
	this->grpStatus = ui->NewGroupBox(*this, CSTR("Status"));
	this->grpStatus->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblRecvCnt = ui->NewLabel(this->grpStatus, CSTR("Recv Count"));
	this->lblRecvCnt->SetRect(4, 4, 100, 23, false);
	this->txtRecvCnt = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtRecvCnt->SetReadOnly(true);
	this->txtRecvCnt->SetRect(104, 4, 150, 23, false);
	this->txtRecvCntRate = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtRecvCntRate->SetReadOnly(true);
	this->txtRecvCntRate->SetRect(254, 4, 150, 23, false);
	this->lblRecvSize = ui->NewLabel(this->grpStatus, CSTR("Recv Size"));
	this->lblRecvSize->SetRect(4, 28, 100, 23, false);
	this->txtRecvSize = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtRecvSize->SetReadOnly(true);
	this->txtRecvSize->SetRect(104, 28, 150, 23, false);
	this->txtRecvSizeRate = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtRecvSizeRate->SetReadOnly(true);
	this->txtRecvSizeRate->SetRect(254, 28, 150, 23, false);
	this->lblSentSuccCnt = ui->NewLabel(this->grpStatus, CSTR("Sent Succ Cnt"));
	this->lblSentSuccCnt->SetRect(4, 52, 100, 23, false);
	this->txtSentSuccCnt = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtSentSuccCnt->SetReadOnly(true);
	this->txtSentSuccCnt->SetRect(104, 52, 150, 23, false);
	this->txtSentSuccCntRate = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtSentSuccCntRate->SetReadOnly(true);
	this->txtSentSuccCntRate->SetRect(254, 52, 150, 23, false);
	this->lblSentFailCnt = ui->NewLabel(this->grpStatus, CSTR("Sent Fail Cnt"));
	this->lblSentFailCnt->SetRect(4, 76, 100, 23, false);
	this->txtSentFailCnt = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtSentFailCnt->SetReadOnly(true);
	this->txtSentFailCnt->SetRect(104, 76, 150, 23, false);
	this->txtSentFailCntRate = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtSentFailCntRate->SetReadOnly(true);
	this->txtSentFailCntRate->SetRect(254, 76, 150, 23, false);

	this->AddTimer(100, OnTimerTick, this);

	UOSInt i;
	this->threadCnt = Sync::ThreadUtil::GetThreadCnt();
	this->threads = MemAlloc(ThreadStatus, this->threadCnt);
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].destAddr.addrType = Net::AddrType::Unknown;
		this->threads[i].destPort = 0;
		NEW_CLASS(this->threads[i].evt, Sync::Event(true));
		this->threads[i].me = this;
		this->threads[i].reqCnt = 0;
		this->threads[i].status = 0;
		this->threads[i].taskType = 0;
		this->threads[i].sentSuccCnt = 0;
		this->threads[i].sentFailCnt = 0;
		Sync::ThreadUtil::Create(ProcThread, &this->threads[i]);
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
			this->mainEvt.Wait(100);
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
			this->mainEvt.Wait(100);
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
}

void SSWR::AVIRead::AVIRUDPTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
