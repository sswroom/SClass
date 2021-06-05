#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRTCPSpdCliForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRTCPSpdCliForm::OnConnClick(void *userObj)
{
	SSWR::AVIRead::AVIRTCPSpdCliForm *me = (SSWR::AVIRead::AVIRTCPSpdCliForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	if (me->connected)
	{
		me->connected = false;
		Sync::MutexUsage mutUsage(me->cliMut);
		if (me->cli)
		{
			me->cli->Close();
		}
		mutUsage.EndUse();
		me->txtHost->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		return;
	}
	me->txtHost->GetText(&sb);
	if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToString(), &addr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Host is not valid", (const UTF8Char *)"Error", me);
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(&sb);
	if (!sb.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Port is not a number", (const UTF8Char *)"Error", me);
		return;
	}
	if (port <= 0 || port > 65535)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Port is out of range", (const UTF8Char *)"Error", me);
		return;
	}
	Net::TCPClient *cli;
	NEW_CLASS(cli, Net::TCPClient(me->core->GetSocketFactory(), &addr, port));
	if (cli->IsConnectError())
	{
		DEL_CLASS(cli);
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in connect to server", (const UTF8Char *)"Error", me);
		return;
	}
	else
	{
		cli->SetNoDelay(true);
		me->connected = true;
		me->clk->Start();
		me->recvSize = 0;
		me->sendSize = 0;
		me->lastRecvSize = 0;
		me->lastSendSize = 0;
		me->lastTime = 0;
		me->txtHost->SetReadOnly(true);
		me->txtPort->SetReadOnly(true);
		me->cli = cli;
		me->procEvt->Set();
		me->recvEvt->Set();
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdCliForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRTCPSpdCliForm *me = (SSWR::AVIRead::AVIRTCPSpdCliForm*)userObj;
	UTF8Char sbuff[64];
	if (me->connected)
	{
		if (me->cli == 0)
		{
			me->connected = false;
			me->txtHost->SetReadOnly(false);
			me->txtPort->SetReadOnly(false);
		}
		Double currTime = me->clk->GetTimeDiff();
		UInt64 currRecvSize = me->recvSize;
		UInt64 currSendSize = me->sendSize;
		if (currTime > me->lastTime)
		{
			Text::StrDouble(sbuff, (Double)(currSendSize - me->lastSendSize) / (currTime - me->lastTime));
			me->txtSendSpeed->SetText(sbuff);
			Text::StrDouble(sbuff, (Double)(currRecvSize - me->lastRecvSize) / (currTime - me->lastTime));
			me->txtRecvSpeed->SetText(sbuff);
		}
		else
		{
			me->txtSendSpeed->SetText((const UTF8Char*)"0");
			me->txtRecvSpeed->SetText((const UTF8Char*)"0");
		}
		me->lastTime = currTime;
		me->lastRecvSize = currRecvSize;
		me->lastSendSize = currSendSize;
	}

}

UInt32 __stdcall SSWR::AVIRead::AVIRTCPSpdCliForm::ProcThread(void *userObj)
{
	SSWR::AVIRead::AVIRTCPSpdCliForm *me = (SSWR::AVIRead::AVIRTCPSpdCliForm*)userObj;
	UInt8 *sendBuff;
	UOSInt sendSize;
	UOSInt sendBuffSize = 9000;
	me->procRunning = true;
	me->mainEvt->Set();
	sendBuff = MemAlloc(UInt8, sendBuffSize);
	while (!me->toStop)
	{
		Sync::MutexUsage mutUsage(me->cliMut);
		if (me->cli)
		{
			sendSize = me->cli->Write(sendBuff, sendBuffSize);
			mutUsage.EndUse();
			if (sendSize > 0)
			{
				Sync::Interlocked::Add(&me->sendSize, sendSize);
			}
			else
			{
				me->procEvt->Wait(1000);
			}
		}
		else
		{
			mutUsage.EndUse();
			me->procEvt->Wait(1000);
		}
	}
	MemFree(sendBuff);
	me->procRunning = false;
	me->mainEvt->Set();
	return 0;
}

UInt32 __stdcall SSWR::AVIRead::AVIRTCPSpdCliForm::RecvThread(void *userObj)
{
	SSWR::AVIRead::AVIRTCPSpdCliForm *me = (SSWR::AVIRead::AVIRTCPSpdCliForm*)userObj;
	UInt8 *recvBuff;
	UOSInt recvSize;
	me->recvRunning = true;
	me->mainEvt->Set();
	recvBuff = MemAlloc(UInt8, 9000);
	while (!me->toStop)
	{
		if (me->cli)
		{
			recvSize = me->cli->Read(recvBuff, 9000);
			if (recvSize > 0)
			{
				Sync::Interlocked::Add(&me->recvSize, recvSize);
			}
			else
			{
				Sync::MutexUsage mutUsage(me->cliMut);
				DEL_CLASS(me->cli);
				me->cli = 0;
				mutUsage.EndUse();
				me->recvEvt->Wait(1000);
			}
		}
		else
		{
			me->recvEvt->Wait(1000);
		}
	}
	MemFree(recvBuff);
	me->recvRunning = false;
	me->mainEvt->Set();
	return 0;
}

SSWR::AVIRead::AVIRTCPSpdCliForm::AVIRTCPSpdCliForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 320, 240, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"TCP Speed Client");
	this->SetNoResize(true);

	this->core = core;
	this->sendSize = 0;
	this->recvSize = 0;
	this->lastTime = 0;
	this->lastSendSize = 0;
	this->lastRecvSize = 0;
	this->cli = 0;
	this->connected = false;
	this->toStop = false;
	this->procRunning = false;
	this->recvRunning = false;
	NEW_CLASS(this->cliMut, Sync::Mutex());
	NEW_CLASS(this->clk, Manage::HiResClock());
	NEW_CLASS(this->mainEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRTCPSpeedCliForm.mainEvt"));
	NEW_CLASS(this->recvEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRTCPSpeedCliForm.recvEvt"));
	NEW_CLASS(this->procEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRTCPSpeedCliForm.procEvt"));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this, (const UTF8Char*)"Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this, (const UTF8Char*)"127.0.0.1"));
	this->txtHost->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, (const UTF8Char*)"1234"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->btnConn, UI::GUIButton(ui, this, (const UTF8Char*)"Conn"));
	this->btnConn->SetRect(104, 52, 75, 23, false);
	this->btnConn->HandleButtonClick(OnConnClick, this);

	NEW_CLASS(this->lblSendSpeed, UI::GUILabel(ui, this, (const UTF8Char*)"Send Speed"));
	this->lblSendSpeed->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtSendSpeed, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtSendSpeed->SetRect(104, 100, 150, 23, false);
	this->txtSendSpeed->SetReadOnly(true);
	NEW_CLASS(this->lblRecvSpeed, UI::GUILabel(ui, this, (const UTF8Char*)"Recv Speed"));
	this->lblRecvSpeed->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtRecvSpeed, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtRecvSpeed->SetRect(104, 124, 150, 23, false);
	this->txtRecvSpeed->SetReadOnly(true);

	this->AddTimer(1000, OnTimerTick, this);
	Sync::Thread::Create(ProcThread, this);
	Sync::Thread::Create(RecvThread, this);
	while (!this->recvRunning || !this->procRunning)
	{
		this->mainEvt->Wait(100);
	}
}

SSWR::AVIRead::AVIRTCPSpdCliForm::~AVIRTCPSpdCliForm()
{
	if (this->cli)
	{
		this->cli->Close();
	}
	this->toStop = true;
	while (this->recvRunning || this->procRunning)
	{
		this->mainEvt->Wait(100);
	}
	DEL_CLASS(this->mainEvt);
	DEL_CLASS(this->procEvt);
	DEL_CLASS(this->recvEvt);
	DEL_CLASS(this->cliMut);
	DEL_CLASS(this->clk);
}

void SSWR::AVIRead::AVIRTCPSpdCliForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
