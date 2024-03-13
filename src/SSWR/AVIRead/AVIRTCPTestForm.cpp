#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/TCPClient.h"
#include "SSWR/AVIRead/AVIRTCPTestForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRTCPTestForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTCPTestForm *me = (SSWR::AVIRead::AVIRTCPTestForm*)userObj;
	if (me->threadCurrCnt > 0)
	{
		me->StopThreads();
		return;
	}
	me->StopThreads();
	Text::StringBuilderUTF8 sb;
	me->txtIP->GetText(sb);
	me->svrIP = Net::SocketUtil::GetIPAddr(sb.ToCString());
	if (me->svrIP == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid IP"), CSTR("Start"), me);
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(sb);
	if (!sb.ToUInt16(me->svrPort) || me->svrPort <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid Port"), CSTR("Start"), me);
		return;
	}
	sb.ClearStr();
	me->txtConcurrCnt->GetText(sb);
	if (!sb.ToUInt32(me->threadCnt) || me->threadCnt <= 0 || me->threadCnt >= 1000)
	{
		me->threadCnt = 0;
		me->ui->ShowMsgOK(CSTR("Please enter valid Concurrent Count"), CSTR("Start"), me);
		return;
	}
	sb.ClearStr();
	me->txtTotalConnCnt->GetText(sb);
	if (!sb.ToUInt32(me->connLeftCnt) || me->connLeftCnt <= 10 || me->connLeftCnt >= 1000000000)
	{
		me->threadCnt = 0;
		me->connLeftCnt = 0;
		me->ui->ShowMsgOK(CSTR("Please enter valid Total Connection Count"), CSTR("Start"), me);
		return;
	}
	
	UOSInt i;
	me->connCnt = 0;
	me->failCnt = 0;
	me->threadCurrCnt = 0;
	me->threads = MemAlloc(Sync::Thread*, me->threadCnt);
	i = me->threadCnt;
	while (i-- > 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("AVIRTCPTest"));
		sb.AppendUOSInt(i);
		NEW_CLASS(me->threads[i], Sync::Thread(ProcessThread, me, sb.ToCString()));
		me->threads[i]->Start();
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPTestForm::ProcessThread(NotNullPtr<Sync::Thread> thread)
{
	SSWR::AVIRead::AVIRTCPTestForm *me = (SSWR::AVIRead::AVIRTCPTestForm*)thread->GetUserObj();
	Net::TCPClient *cli = 0;
//	UInt8 buff[2048];
//	void *reqData;
	Sync::Interlocked::IncrementU32(me->threadCurrCnt);
	while (!thread->IsStopping())
	{
		Sync::MutexUsage mutUsage(me->connMut);
		if (me->connLeftCnt <= 0)
		{
			break;
		}
		me->connLeftCnt -= 1;
		mutUsage.EndUse();
		
		NEW_CLASS(cli, Net::TCPClient(me->sockf, me->svrIP, me->svrPort, 10000));
		if (cli->IsConnectError())
		{
			Sync::Interlocked::IncrementU32(me->failCnt);
			DEL_CLASS(cli);
		}
		else
		{
			Sync::Interlocked::IncrementU32(me->connCnt);
			cli->ShutdownSend();
/*			reqData = cli->BeginRead(buff, 2048, status->evt);
			while (true)
			{
				if (reqData == 0)
					break;
				status->evt->Wait();
				if (status->threadToStop)
				{
					cli->CancelRead(reqData);
					break;
				}
				if (cli->EndRead(reqData, true) == 0)
					break;
				reqData = cli->BeginRead(buff, 2048, status->evt);
			}*/
			DEL_CLASS(cli);
		}
		
	}
	Sync::Interlocked::DecrementU32(me->threadCurrCnt);
}

void __stdcall SSWR::AVIRead::AVIRTCPTestForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRTCPTestForm *me = (SSWR::AVIRead::AVIRTCPTestForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrUInt32(sbuff, me->connLeftCnt);
	me->txtConnLeftCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt32(sbuff, me->threadCurrCnt);
	me->txtThreadCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt32(sbuff, me->connCnt);
	me->txtSuccCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt32(sbuff, me->failCnt);
	me->txtFailCnt->SetText(CSTRP(sbuff, sptr));
}

void SSWR::AVIRead::AVIRTCPTestForm::StopThreads()
{
	if (this->threadCurrCnt > 0)
	{
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			this->threads[i]->BeginStop();
		}
		while (this->threadCurrCnt > 0)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}
	if (this->threads)
	{
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			DEL_CLASS(this->threads[i]);
		}
		MemFree(this->threads);
		this->threads = 0;
		this->threadCnt = 0;
	}
}

SSWR::AVIRead::AVIRTCPTestForm::AVIRTCPTestForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("TCP Conn Test"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->threads = 0;
	this->svrIP = 0;
	this->svrPort = 0;
	this->connLeftCnt = 0;
	this->threadCnt = 0;
	this->threadCurrCnt = 0;
	this->connCnt = 0;
	this->failCnt = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 127, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblIP = ui->NewLabel(this->pnlRequest, CSTR("IP"));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	this->txtIP = ui->NewTextBox(this->pnlRequest, CSTR("127.0.0.1"));
	this->txtIP->SetRect(104, 4, 150, 23, false);
	this->lblPort = ui->NewLabel(this->pnlRequest, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlRequest, CSTR("80"));
	this->txtPort->SetRect(104, 28, 70, 23, false);
	this->lblConcurrCnt = ui->NewLabel(this->pnlRequest, CSTR("Concurrent Count"));
	this->lblConcurrCnt->SetRect(4, 52, 100, 23, false);
	this->txtConcurrCnt = ui->NewTextBox(this->pnlRequest, CSTR("10"));
	this->txtConcurrCnt->SetRect(104, 52, 100, 23, false);
	this->lblTotalConnCnt = ui->NewLabel(this->pnlRequest, CSTR("Total Conn Count"));
	this->lblTotalConnCnt->SetRect(4, 76, 100, 23, false);
	this->txtTotalConnCnt = ui->NewTextBox(this->pnlRequest, CSTR("100000"));
	this->txtTotalConnCnt->SetRect(104, 76, 100, 23, false);
	this->btnStart = ui->NewButton(this->pnlRequest, CSTR("Request"));
	this->btnStart->SetRect(104, 100, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->grpStatus = ui->NewGroupBox(*this, CSTR("Status"));
	this->grpStatus->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblConnLeftCnt = ui->NewLabel(this->grpStatus, CSTR("Conn Left"));
	this->lblConnLeftCnt->SetRect(4, 4, 100, 23, false);
	this->txtConnLeftCnt = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtConnLeftCnt->SetRect(104, 4, 150, 23, false);
	this->txtConnLeftCnt->SetReadOnly(true);
	this->lblThreadCnt = ui->NewLabel(this->grpStatus, CSTR("Thread Count"));
	this->lblThreadCnt->SetRect(4, 28, 100, 23, false);
	this->txtThreadCnt = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtThreadCnt->SetRect(104, 28, 150, 23, false);
	this->txtThreadCnt->SetReadOnly(true);
	this->lblSuccCnt = ui->NewLabel(this->grpStatus, CSTR("Success Count"));
	this->lblSuccCnt->SetRect(4, 52, 100, 23, false);
	this->txtSuccCnt = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtSuccCnt->SetRect(104, 52, 150, 23, false);
	this->txtSuccCnt->SetReadOnly(true);
	this->lblFailCnt = ui->NewLabel(this->grpStatus, CSTR("Fail Count"));
	this->lblFailCnt->SetRect(4, 76, 100, 23, false);
	this->txtFailCnt = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtFailCnt->SetRect(104, 76, 150, 23, false);
	this->txtFailCnt->SetReadOnly(true);
	
	this->SetDefaultButton(this->btnStart);
	this->txtIP->Focus();
	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRTCPTestForm::~AVIRTCPTestForm()
{
	this->StopThreads();
}

void SSWR::AVIRead::AVIRTCPTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
