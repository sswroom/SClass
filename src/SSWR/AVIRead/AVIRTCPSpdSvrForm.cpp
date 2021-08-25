#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRTCPSpdSvrForm.h"
#include "Sync/Thread.h"
#include "UI/MessageDialog.h"

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
		return;
	}
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
	if (!me->chkMultiThread->IsChecked())
	{
		NEW_CLASS(me->cliMgr, Net::TCPClientMgr(60, OnClientEvent, OnClientData, me, 4, OnClientTimeout));
	}
	NEW_CLASS(me->svr, Net::TCPServer(me->core->GetSocketFactory(), port, me->log, OnClientConn, me, 0));
	if (me->svr->IsV4Error())
	{
		DEL_CLASS(me->svr);
		SDEL_CLASS(me->cliMgr);
		me->svr = 0;
		me->cliMgr = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in listening to the port", (const UTF8Char *)"Error", me);
		return;
	}
	me->txtPort->SetReadOnly(true);
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnClientConn(Socket *s, void *userObj)
{
	SSWR::AVIRead::AVIRTCPSpdSvrForm *me = (SSWR::AVIRead::AVIRTCPSpdSvrForm*)userObj;
	Net::TCPClient *cli;
	NEW_CLASS(cli, Net::TCPClient(me->core->GetSocketFactory(), s));
	cli->SetNoDelay(true);
	if (me->cliMgr)
	{
		me->cliMgr->AddClient(cli, 0);
	}
	else
	{
		Sync::Thread::Create(RecvThread, cli);
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
//	SSWR::AVIRead::AVIRTCPSpdSvrForm *me = (SSWR::AVIRead::AVIRTCPSpdSvrForm*)userObj;
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		DEL_CLASS(cli);
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
//	SSWR::AVIRead::AVIRTCPSpdSvrForm *me = (SSWR::AVIRead::AVIRTCPSpdSvrForm*)userObj;
	cli->Write(buff, size);
}

void __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData)
{
}

UInt32 __stdcall SSWR::AVIRead::AVIRTCPSpdSvrForm::RecvThread(void *userObj)
{
	Net::TCPClient *cli = (Net::TCPClient*)userObj;
	UInt8 *recvBuff = MemAlloc(UInt8, 9000);
	UOSInt recvSize;
	while (true)
	{
		recvSize = cli->Read(recvBuff, 9000);
		if (recvSize <= 0)
			break;
		cli->Write(recvBuff, recvSize);
	}
	MemFree(recvBuff);
	return 0;
}

SSWR::AVIRead::AVIRTCPSpdSvrForm::AVIRTCPSpdSvrForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 320, 120, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"TCP Speed Server");

	this->core = core;
	this->svr = 0;
	this->cliMgr = 0;
	NEW_CLASS(this->log, IO::LogTool());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, (const UTF8Char*)"1234"));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->chkMultiThread, UI::GUICheckBox(ui, this, (const UTF8Char*)"Multi-Thread", false));
	this->chkMultiThread->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(104, 52, 75, 23, false);
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
