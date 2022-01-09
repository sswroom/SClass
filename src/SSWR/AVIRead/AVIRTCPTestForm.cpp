#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/TCPClient.h"
#include "SSWR/AVIRead/AVIRTCPTestForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"

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
	me->txtIP->GetText(&sb);
	me->svrIP = Net::SocketUtil::GetIPAddr(sb.ToString());
	if (me->svrIP == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid IP", (const UTF8Char*)"Start", me);
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(&sb);
	if (!sb.ToUInt16(&me->svrPort) || me->svrPort <= 0 || me->svrPort >= 65536)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid Port", (const UTF8Char*)"Start", me);
		return;
	}
	sb.ClearStr();
	me->txtConcurrCnt->GetText(&sb);
	if (!sb.ToUInt32(&me->threadCnt) || me->threadCnt <= 0 || me->threadCnt >= 1000)
	{
		me->threadCnt = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid Concurrent Count", (const UTF8Char*)"Start", me);
		return;
	}
	sb.ClearStr();
	me->txtTotalConnCnt->GetText(&sb);
	if (!sb.ToUInt32(&me->connLeftCnt) || me->connLeftCnt <= 10 || me->connLeftCnt >= 1000000000)
	{
		me->threadCnt = 0;
		me->connLeftCnt = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid Total Connection Count", (const UTF8Char*)"Start", me);
		return;
	}
	
	UOSInt i;
	me->connCnt = 0;
	me->failCnt = 0;
	me->threadCurrCnt = 0;
	me->threadStatus = MemAlloc(ThreadStatus, me->threadCnt);
	i = me->threadCnt;
	while (i-- > 0)
	{
		me->threadStatus[i].me = me;
		me->threadStatus[i].threadRunning = false;
		me->threadStatus[i].threadToStop = false;
		NEW_CLASS(me->threadStatus[i].evt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRTCPTestForm.threadStatus.evt"));
		Sync::Thread::Create(ProcessThread, &me->threadStatus[i]);
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRTCPTestForm::ProcessThread(void *userObj)
{
	ThreadStatus *status = (ThreadStatus*)userObj;
	Net::TCPClient *cli = 0;
//	UInt8 buff[2048];
//	void *reqData;
	Sync::Interlocked::Increment(&status->me->threadCurrCnt);
	status->threadRunning = true;
	while (!status->threadToStop)
	{
		Sync::MutexUsage mutUsage(status->me->connMut);
		if (status->me->connLeftCnt <= 0)
		{
			mutUsage.EndUse();
			break;
		}
		status->me->connLeftCnt -= 1;
		mutUsage.EndUse();
		
		NEW_CLASS(cli, Net::TCPClient(status->me->sockf, status->me->svrIP, status->me->svrPort));
		if (cli->IsConnectError())
		{
			Sync::Interlocked::Increment(&status->me->failCnt);
			DEL_CLASS(cli);
		}
		else
		{
			Sync::Interlocked::Increment(&status->me->connCnt);
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
	status->threadToStop = false;
	status->threadRunning = false;
	Sync::Interlocked::Decrement(&status->me->threadCurrCnt);
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRTCPTestForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRTCPTestForm *me = (SSWR::AVIRead::AVIRTCPTestForm*)userObj;
	UTF8Char sbuff[32];
	Text::StrUInt32(sbuff, me->connLeftCnt);
	me->txtConnLeftCnt->SetText(sbuff);
	Text::StrUInt32(sbuff, me->threadCurrCnt);
	me->txtThreadCnt->SetText(sbuff);
	Text::StrUInt32(sbuff, me->connCnt);
	me->txtSuccCnt->SetText(sbuff);
	Text::StrUInt32(sbuff, me->failCnt);
	me->txtFailCnt->SetText(sbuff);
}

void SSWR::AVIRead::AVIRTCPTestForm::StopThreads()
{
	if (this->threadCurrCnt > 0)
	{
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			this->threadStatus[i].threadToStop = true;
			this->threadStatus[i].evt->Set();
		}
		while (this->threadCurrCnt > 0)
		{
			Sync::Thread::Sleep(10);
		}
	}
	if (this->threadStatus)
	{
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			DEL_CLASS(this->threadStatus[i].evt);
		}
		MemFree(this->threadStatus);
		this->threadStatus = 0;
		this->threadCnt = 0;
	}
}

SSWR::AVIRead::AVIRTCPTestForm::AVIRTCPTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"TCP Conn Test");

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->threadStatus = 0;
	this->svrIP = 0;
	this->svrPort = 0;
	NEW_CLASS(this->connMut, Sync::Mutex());
	this->connLeftCnt = 0;
	this->threadCnt = 0;
	this->threadCurrCnt = 0;
	this->connCnt = 0;
	this->failCnt = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 127, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblIP, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"IP"));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtIP, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"127.0.0.1"));
	this->txtIP->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"80"));
	this->txtPort->SetRect(104, 28, 70, 23, false);
	NEW_CLASS(this->lblConcurrCnt, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Concurrent Count"));
	this->lblConcurrCnt->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtConcurrCnt, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"10"));
	this->txtConcurrCnt->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->lblTotalConnCnt, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Total Conn Count"));
	this->lblTotalConnCnt->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtTotalConnCnt, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"100000"));
	this->txtTotalConnCnt->SetRect(104, 76, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"Request"));
	this->btnStart->SetRect(104, 100, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->grpStatus, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Status"));
	this->grpStatus->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblConnLeftCnt, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Conn Left"));
	this->lblConnLeftCnt->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtConnLeftCnt, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtConnLeftCnt->SetRect(104, 4, 150, 23, false);
	this->txtConnLeftCnt->SetReadOnly(true);
	NEW_CLASS(this->lblThreadCnt, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Thread Count"));
	this->lblThreadCnt->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtThreadCnt, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtThreadCnt->SetRect(104, 28, 150, 23, false);
	this->txtThreadCnt->SetReadOnly(true);
	NEW_CLASS(this->lblSuccCnt, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Success Count"));
	this->lblSuccCnt->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtSuccCnt, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtSuccCnt->SetRect(104, 52, 150, 23, false);
	this->txtSuccCnt->SetReadOnly(true);
	NEW_CLASS(this->lblFailCnt, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Fail Count"));
	this->lblFailCnt->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtFailCnt, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtFailCnt->SetRect(104, 76, 150, 23, false);
	this->txtFailCnt->SetReadOnly(true);
	
	this->SetDefaultButton(this->btnStart);
	this->txtIP->Focus();
	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRTCPTestForm::~AVIRTCPTestForm()
{
	this->StopThreads();
	DEL_CLASS(this->connMut);
}

void SSWR::AVIRead::AVIRTCPTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
