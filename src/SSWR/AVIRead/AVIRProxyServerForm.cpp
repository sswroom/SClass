#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRProxyServerForm.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRProxyServerForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProxyServerForm *me = (SSWR::AVIRead::AVIRProxyServerForm*)userObj;
	if (me->svr == 0)
	{
		Text::StringBuilderUTF8 sb;
		me->txtPort->GetText(&sb);
		Int32 port = sb.ToInt32();
		if (port <= 0 || port >= 65536)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Please enter valid port number", (const UTF8Char *)"Start", me);
			return;
		}
		NEW_CLASS(me->svr, Net::ProxyServer(me->sockf, (UInt16)port, me->log));
		if (me->svr->IsError())
		{
			DEL_CLASS(me->svr);
			me->svr = 0;
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in starting proxy server. Maybe port is in use.", (const UTF8Char *)"Start", me);
		}
		else
		{
			me->txtPort->SetReadOnly(true);
		}
	}
	else
	{
		DEL_CLASS(me->svr);
		me->svr = 0;
		me->txtPort->SetReadOnly(false);
	}
}

void __stdcall SSWR::AVIRead::AVIRProxyServerForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRProxyServerForm *me = (SSWR::AVIRead::AVIRProxyServerForm*)userObj;
	Text::String *s = me->lbLog->GetSelectedItemTextNew();
	if (s)
	{
		me->txtLog->SetText(s->v);
		s->Release();
	}
}

SSWR::AVIRead::AVIRProxyServerForm::AVIRProxyServerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Proxy Server");

	this->core = core;
	this->sockf = core->GetSocketFactory();
	NEW_CLASS(this->log, IO::LogTool());
	this->svr = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 55, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"8080"));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"&Start"));
	this->btnStart->SetRect(204, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);
	
	this->SetDefaultButton(this->btnStart);
	this->txtPort->Focus();
	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 500, true));
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_RAW);
}

SSWR::AVIRead::AVIRProxyServerForm::~AVIRProxyServerForm()
{
	SDEL_CLASS(this->svr);
	DEL_CLASS(this->logger);
	DEL_CLASS(this->log);
}

void SSWR::AVIRead::AVIRProxyServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
