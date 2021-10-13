#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRTFTPServerForm.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRTFTPServerForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRTFTPServerForm *me = (SSWR::AVIRead::AVIRTFTPServerForm*)userObj;
	if (me->svr)
	{
		DEL_CLASS(me->svr);
		me->txtFilePath->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		return;
	}
	UInt16 port = 0;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(&sb);
	if (!Text::StrToUInt16(sb.ToString(), &port))
	{
		return;
	}
	if (port == 0)
	{
		return;
	}
	sb.ClearStr();
	me->txtFilePath->GetText(&sb);
	if (IO::Path::GetPathType(sb.ToString()) != IO::Path::PathType::Directory)
	{
		return;
	}
	NEW_CLASS(me->svr, Net::TFTPServer(me->core->GetSocketFactory(), port, me->log, sb.ToString()));
	if (me->svr->IsError())
	{
		SDEL_CLASS(me->svr);
	}
	else
	{
		me->txtFilePath->SetReadOnly(true);
		me->txtPort->SetReadOnly(true);
	}
}

void __stdcall SSWR::AVIRead::AVIRTFTPServerForm::OnLogSel(void *userObj)
{
	const UTF8Char *t;
	SSWR::AVIRead::AVIRTFTPServerForm *me = (SSWR::AVIRead::AVIRTFTPServerForm*)userObj;
	t = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(t);
	me->lbLog->DelTextNew(t);
}

void __stdcall SSWR::AVIRead::AVIRTFTPServerForm::OnTimerTick(void *userObj)
{
//	SSWR::AVIRead::AVIRTFTPServerForm *me = (SSWR::AVIRead::AVIRTFTPServerForm*)userObj;
}

SSWR::AVIRead::AVIRTFTPServerForm::AVIRTFTPServerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UOSInt i;
	this->core = core;
	this->SetText((const UTF8Char*)"TFTP Server");
	this->SetFont(0, 8.25, false);
	this->svr = 0;
	this->log = 0;
	this->logger = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage((const UTF8Char*)"Control");
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->tpControl, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->tpControl, (const UTF8Char*)"69"));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	NEW_CLASS(this->lblFilePath, UI::GUILabel(ui, this->tpControl, (const UTF8Char*)"File Path"));
	this->lblFilePath->SetRect(8, 32, 100, 23, false);
	IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		sbuff[i] = 0;
	}
	NEW_CLASS(this->txtFilePath, UI::GUITextBox(ui, this->tpControl, sbuff));
	this->txtFilePath->SetRect(108, 32, 500, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->tpControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(100, 56, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);

	this->tpLog = this->tcMain->AddTabPage((const UTF8Char*)"Log");
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, (const UTF8Char*)""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	this->AddTimer(1000, OnTimerTick, this);
	NEW_CLASS(this->log, IO::LogTool());
	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 300, true));
	this->logger->SetTimeFormat("yyyy-MM-dd HH:mm:ss.fff");
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_COMMAND);
}

SSWR::AVIRead::AVIRTFTPServerForm::~AVIRTFTPServerForm()
{
	SDEL_CLASS(this->svr);
	SDEL_CLASS(this->log);
	SDEL_CLASS(this->logger);
}

void SSWR::AVIRead::AVIRTFTPServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
