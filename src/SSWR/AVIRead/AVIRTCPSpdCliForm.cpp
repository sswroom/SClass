#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "SSWR/AVIRead/AVIRTCPSpdCliForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

#define SENDBUFFSIZE 65536//9000
#define RECVBUFFSIZE 65536//9000

void __stdcall SSWR::AVIRead::AVIRTCPSpdCliForm::OnConnClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPSpdCliForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPSpdCliForm>();
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
		me->btnConn->SetText(CSTR("Start"));
		return;
	}
	me->txtHost->GetText(sb);
	if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Host is not valid"), CSTR("Error"), me);
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(sb);
	if (!sb.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Port is not a number"), CSTR("Error"), me);
		return;
	}
	if (port <= 0 || port > 65535)
	{
		me->ui->ShowMsgOK(CSTR("Port is out of range"), CSTR("Error"), me);
		return;
	}
	Net::TCPClient *cli;
	NEW_CLASS(cli, Net::TCPClient(me->core->GetSocketFactory(), addr, port, 10000));
	if (cli->IsConnectError())
	{
		DEL_CLASS(cli);
		me->ui->ShowMsgOK(CSTR("Error in connect to server"), CSTR("Error"), me);
		return;
	}
	else
	{
		cli->SetNoDelay(true);
		me->connected = true;
		me->clk.Start();
		me->recvSize = 0;
		me->sendSize = 0;
		me->lastRecvSize = 0;
		me->lastSendSize = 0;
		me->lastTime = 0;
		me->txtHost->SetReadOnly(true);
		me->txtPort->SetReadOnly(true);
		me->btnConn->SetText(CSTR("Stop"));
		me->cli = cli;
		me->procEvt.Set();
		me->recvEvt.Set();
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdCliForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPSpdCliForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPSpdCliForm>();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (me->connected)
	{
		if (me->cli == 0)
		{
			me->connected = false;
			me->txtHost->SetReadOnly(false);
			me->txtPort->SetReadOnly(false);
		}
		Double currTime = me->clk.GetTimeDiff();
		UInt64 currRecvSize = me->recvSize;
		UInt64 currSendSize = me->sendSize;
		if (currTime > me->lastTime)
		{
			sptr = Text::StrDouble(sbuff, (Double)(currSendSize - me->lastSendSize) / (currTime - me->lastTime));
			me->txtSendSpeed->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, (Double)(currRecvSize - me->lastRecvSize) / (currTime - me->lastTime));
			me->txtRecvSpeed->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			me->txtSendSpeed->SetText(CSTR("0"));
			me->txtRecvSpeed->SetText(CSTR("0"));
		}
		me->lastTime = currTime;
		me->lastRecvSize = currRecvSize;
		me->lastSendSize = currSendSize;
	}

}

UInt32 __stdcall SSWR::AVIRead::AVIRTCPSpdCliForm::ProcThread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPSpdCliForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPSpdCliForm>();
	UInt8 *sendBuff;
	UOSInt sendSize;
	UOSInt sendBuffSize = SENDBUFFSIZE;
	me->procRunning = true;
	me->mainEvt.Set();
	sendBuff = MemAlloc(UInt8, sendBuffSize);
	while (!me->toStop)
	{
		Sync::MutexUsage mutUsage(me->cliMut);
		if (me->cli)
		{
			sendSize = me->cli->Write(Data::ByteArrayR(sendBuff, sendBuffSize));
			mutUsage.EndUse();
			if (sendSize > 0)
			{
				Sync::Interlocked::AddU64(me->sendSize, sendSize);
			}
			else
			{
				me->procEvt.Wait(1000);
			}
		}
		else
		{
			mutUsage.EndUse();
			me->procEvt.Wait(1000);
		}
	}
	MemFree(sendBuff);
	me->procRunning = false;
	me->mainEvt.Set();
	return 0;
}

UInt32 __stdcall SSWR::AVIRead::AVIRTCPSpdCliForm::RecvThread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPSpdCliForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPSpdCliForm>();
	UOSInt recvSize;
	me->recvRunning = true;
	me->mainEvt.Set();
	{
		Data::ByteBuffer recvBuff(RECVBUFFSIZE);
		while (!me->toStop)
		{
			if (me->cli)
			{
				recvSize = me->cli->Read(recvBuff);
				if (recvSize > 0)
				{
					Sync::Interlocked::AddU64(me->recvSize, recvSize);
				}
				else
				{
					Sync::MutexUsage mutUsage(me->cliMut);
					DEL_CLASS(me->cli);
					me->cli = 0;
					mutUsage.EndUse();
					me->recvEvt.Wait(1000);
				}
			}
			else
			{
				me->recvEvt.Wait(1000);
			}
		}
	}
	me->recvRunning = false;
	me->mainEvt.Set();
	return 0;
}

SSWR::AVIRead::AVIRTCPSpdCliForm::AVIRTCPSpdCliForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 320, 240, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("TCP Speed Client"));
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
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblHost = ui->NewLabel(*this, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(*this, CSTR("127.0.0.1"));
	this->txtHost->SetRect(104, 4, 100, 23, false);
	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("1234"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	this->btnConn = ui->NewButton(*this, CSTR("Start"));
	this->btnConn->SetRect(104, 52, 75, 23, false);
	this->btnConn->HandleButtonClick(OnConnClick, this);

	this->lblSendSpeed = ui->NewLabel(*this, CSTR("Send Speed"));
	this->lblSendSpeed->SetRect(4, 100, 100, 23, false);
	this->txtSendSpeed = ui->NewTextBox(*this, CSTR(""));
	this->txtSendSpeed->SetRect(104, 100, 150, 23, false);
	this->txtSendSpeed->SetReadOnly(true);
	this->lblRecvSpeed = ui->NewLabel(*this, CSTR("Recv Speed"));
	this->lblRecvSpeed->SetRect(4, 124, 100, 23, false);
	this->txtRecvSpeed = ui->NewTextBox(*this, CSTR(""));
	this->txtRecvSpeed->SetRect(104, 124, 150, 23, false);
	this->txtRecvSpeed->SetReadOnly(true);

	this->AddTimer(1000, OnTimerTick, this);
	Sync::ThreadUtil::Create(ProcThread, this);
	Sync::ThreadUtil::Create(RecvThread, this);
	while (!this->recvRunning || !this->procRunning)
	{
		this->mainEvt.Wait(100);
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
		this->mainEvt.Wait(100);
	}
}

void SSWR::AVIRead::AVIRTCPSpdCliForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
