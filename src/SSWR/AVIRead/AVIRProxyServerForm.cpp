#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRProxyServerForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRProxyServerForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProxyServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProxyServerForm>();
	NN<Net::ProxyServer> svr;
	if (!me->svr.SetTo(svr))
	{
		Text::StringBuilderUTF8 sb;
		me->txtPort->GetText(sb);
		Int32 port = sb.ToInt32();
		if (port <= 0 || port >= 65536)
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid port number"), CSTR("Start"), me);
			return;
		}
		NEW_CLASSNN(svr, Net::ProxyServer(me->sockf, (UInt16)port, me->log, true));
		if (svr->IsError())
		{
			svr.Delete();
			me->ui->ShowMsgOK(CSTR("Error in starting proxy server. Maybe port is in use."), CSTR("Start"), me);
		}
		else
		{
			me->svr = svr;
			me->txtPort->SetReadOnly(true);
		}
	}
	else
	{
		svr.Delete();
		me->svr = 0;
		me->txtPort->SetReadOnly(false);
	}
}

void __stdcall SSWR::AVIRead::AVIRProxyServerForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProxyServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProxyServerForm>();
	NN<Text::String> s;
	if (me->lbLog->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
}

SSWR::AVIRead::AVIRProxyServerForm::AVIRProxyServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Proxy Server"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->svr = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 55, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPort = ui->NewLabel(this->pnlRequest, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlRequest, CSTR("8080"));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	this->btnStart = ui->NewButton(this->pnlRequest, CSTR("&Start"));
	this->btnStart->SetRect(204, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->txtLog = ui->NewTextBox(*this, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(*this, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);
	
	this->SetDefaultButton(this->btnStart);
	this->txtPort->Focus();
	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);
}

SSWR::AVIRead::AVIRProxyServerForm::~AVIRProxyServerForm()
{
	this->svr.Delete();
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
}

void SSWR::AVIRead::AVIRProxyServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
