#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRTCPSpdSvrForm.h"
#include "Sync/ThreadUtil.h"
#include "UI/MessageDialog.h"

struct ClientStatus
{
	Bool echo;
	NotNullPtr<Net::TCPClient> cli;
};

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRTCPSpdSvrForm *me = (SSWR::AVIRead::AVIRTCPSpdSvrForm*)userObj;
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
	me->txtPort->GetText(&sb);
	if (!sb.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog(CSTR("Port is not a number"), CSTR("Error"), me);
		return;
	}
	if (port <= 0 || port > 65535)
	{
		UI::MessageDialog::ShowDialog(CSTR("Port is out of range"), CSTR("Error"), me);
		return;
	}
	me->echo = me->chkEcho->IsChecked();
	if (!me->chkMultiThread->IsChecked())
	{
		NEW_CLASS(me->cliMgr, Net::TCPClientMgr(60, OnClientEvent, OnClientData, me, 4, OnClientTimeout));
	}
	NEW_CLASS(me->svr, Net::TCPServer(me->core->GetSocketFactory(), port, me->log, OnClientConn, me, CSTR_NULL, true));
	if (me->svr->IsV4Error())
	{
		DEL_CLASS(me->svr);
		SDEL_CLASS(me->cliMgr);
		me->svr = 0;
		me->cliMgr = 0;
		UI::MessageDialog::ShowDialog(CSTR("Error in listening to the port"), CSTR("Error"), me);
		return;
	}
	me->txtPort->SetReadOnly(true);
	me->btnStart->SetText(CSTR("Stop"));
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnClientConn(Socket *s, void *userObj)
{
	SSWR::AVIRead::AVIRTCPSpdSvrForm *me = (SSWR::AVIRead::AVIRTCPSpdSvrForm*)userObj;
	NotNullPtr<Net::TCPClient> cli;
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

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
//	SSWR::AVIRead::AVIRTCPSpdSvrForm *me = (SSWR::AVIRead::AVIRTCPSpdSvrForm*)userObj;
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		cli.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
	SSWR::AVIRead::AVIRTCPSpdSvrForm *me = (SSWR::AVIRead::AVIRTCPSpdSvrForm*)userObj;
	if (me->echo)
	{
		cli->Write(buff, size);
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData)
{
}

UInt32 __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::RecvThread(void *userObj)
{
	ClientStatus *cliStatus = (ClientStatus*)userObj;
	UInt8 *recvBuff = MemAlloc(UInt8, 9000);
	UOSInt recvSize;
	while (true)
	{
		recvSize = cliStatus->cli->Read(recvBuff, 9000);
		if (recvSize <= 0)
			break;
		if (cliStatus->echo)
			cliStatus->cli->Write(recvBuff, recvSize);
	}
	MemFree(recvBuff);
	cliStatus->cli.Delete();
	MemFree(cliStatus);
	return 0;
}

SSWR::AVIRead::AVIRTCPSpdSvrForm::AVIRTCPSpdSvrForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 320, 144, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("TCP Speed Server"));

	this->core = core;
	this->svr = 0;
	this->cliMgr = 0;
	NEW_CLASS(this->log, IO::LogTool());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, CSTR("Port")));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, CSTR("1234")));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->chkMultiThread, UI::GUICheckBox(ui, this, CSTR("Multi-Thread"), false));
	this->chkMultiThread->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->chkEcho, UI::GUICheckBox(ui, this, CSTR("Echo"), true));
	this->chkEcho->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, CSTR("Start")));
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
	DEL_CLASS(this->log);
}

void SSWR::AVIRead::AVIRTCPSpdSvrForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
