#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "SSWR/AVIRead/AVIRTCPSpdSvrForm.h"
#include "Sync/ThreadUtil.h"

struct ClientStatus
{
	Bool echo;
	NN<Net::TCPClient> cli;
};

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnStartClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPSpdSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPSpdSvrForm>();
	Text::StringBuilderUTF8 sb;
	UInt16 port;
	if (me->svr)
	{
		DEL_CLASS(me->svr);
		DEL_CLASS(me->cliMgr);
		me->svr = 0;
		me->cliMgr = 0;
		me->txtPort->SetReadOnly(false);
		me->btnStart->SetText(CSTR("Start"));
		return;
	}
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
	me->echo = me->chkEcho->IsChecked();
	if (!me->chkMultiThread->IsChecked())
	{
		NEW_CLASS(me->cliMgr, Net::TCPClientMgr(60, OnClientEvent, OnClientData, me, 4, OnClientTimeout));
	}
	NEW_CLASS(me->svr, Net::TCPServer(me->core->GetSocketFactory(), 0, port, me->log, OnClientConn, me, CSTR_NULL, true));
	if (me->svr->IsV4Error())
	{
		DEL_CLASS(me->svr);
		SDEL_CLASS(me->cliMgr);
		me->svr = 0;
		me->cliMgr = 0;
		me->ui->ShowMsgOK(CSTR("Error in listening to the port"), CSTR("Error"), me);
		return;
	}
	me->txtPort->SetReadOnly(true);
	me->btnStart->SetText(CSTR("Stop"));
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnClientConn(NN<Socket> s, AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPSpdSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPSpdSvrForm>();
	NN<Net::TCPClient> cli;
	NEW_CLASSNN(cli, Net::TCPClient(me->core->GetSocketFactory(), s));
	cli->SetNoDelay(true);
	if (me->cliMgr)
	{
		me->cliMgr->AddClient(cli, 0);
	}
	else
	{
		ClientStatus *cliStatus = MemAlloc(ClientStatus, 1);
		cliStatus->echo = me->echo;
		cliStatus->cli = cli;
		Sync::ThreadUtil::Create(RecvThread, cliStatus);
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
//	NN<SSWR::AVIRead::AVIRTCPSpdSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPSpdSvrForm>();
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		cli.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff)
{
	NN<SSWR::AVIRead::AVIRTCPSpdSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPSpdSvrForm>();
	if (me->echo)
	{
		cli->Write(buff.Arr().Ptr(), buff.GetSize());
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{
}

UInt32 __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::RecvThread(AnyType userObj)
{
	NN<ClientStatus> cliStatus = userObj.GetNN<ClientStatus>();
	Data::ByteBuffer recvBuff(9000);
	UOSInt recvSize;
	while (true)
	{
		recvSize = cliStatus->cli->Read(recvBuff);
		if (recvSize <= 0)
			break;
		if (cliStatus->echo)
			cliStatus->cli->Write(recvBuff.Arr().Ptr(), recvSize);
	}
	cliStatus->cli.Delete();
	MemFreeNN(cliStatus);
	return 0;
}

SSWR::AVIRead::AVIRTCPSpdSvrForm::AVIRTCPSpdSvrForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 320, 144, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("TCP Speed Server"));

	this->core = core;
	this->svr = 0;
	this->cliMgr = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("1234"));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	this->chkMultiThread = ui->NewCheckBox(*this, CSTR("Multi-Thread"), false);
	this->chkMultiThread->SetRect(104, 28, 100, 23, false);
	this->chkEcho = ui->NewCheckBox(*this, CSTR("Echo"), true);
	this->chkEcho->SetRect(104, 52, 100, 23, false);
	this->btnStart = ui->NewButton(*this, CSTR("Start"));
	this->btnStart->SetRect(104, 76, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
}

SSWR::AVIRead::AVIRTCPSpdSvrForm::~AVIRTCPSpdSvrForm()
{
	if (this->svr)
	{
		DEL_CLASS(this->svr);
		this->svr = 0;
	}
	if (this->cliMgr)
	{
		DEL_CLASS(this->cliMgr);
		this->cliMgr = 0;
	}
}

void SSWR::AVIRead::AVIRTCPSpdSvrForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
