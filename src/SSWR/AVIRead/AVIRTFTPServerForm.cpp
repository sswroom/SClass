#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRTFTPServerForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRTFTPServerForm::OnStartClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTFTPServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTFTPServerForm>();
	if (me->svr)
	{
		DEL_CLASS(me->svr);
		me->txtFilePath->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		return;
	}
	UInt16 port = 0;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(sb);
	if (!Text::StrToUInt16(sb.ToString(), port))
	{
		return;
	}
	if (port == 0)
	{
		return;
	}
	sb.ClearStr();
	me->txtFilePath->GetText(sb);
	if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Directory)
	{
		return;
	}
	NEW_CLASS(me->svr, Net::TFTPServer(me->core->GetSocketFactory(), port, me->log, sb.ToCString()));
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

void __stdcall SSWR::AVIRead::AVIRTFTPServerForm::OnLogSel(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTFTPServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTFTPServerForm>();
	Optional<Text::String> s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(Text::String::OrEmpty(s)->ToCString());
	OPTSTR_DEL(s);
}

void __stdcall SSWR::AVIRead::AVIRTFTPServerForm::OnTimerTick(AnyType userObj)
{
//	NN<SSWR::AVIRead::AVIRTFTPServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTFTPServerForm>();
}

SSWR::AVIRead::AVIRTFTPServerForm::AVIRTFTPServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	this->core = core;
	this->SetText(CSTR("TFTP Server"));
	this->SetFont(0, 0, 8.25, false);
	this->svr = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->lblPort = ui->NewLabel(this->tpControl, CSTR("Port"));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->tpControl, CSTR("69"));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	this->lblFilePath = ui->NewLabel(this->tpControl, CSTR("File Path"));
	this->lblFilePath->SetRect(8, 32, 100, 23, false);
	sptr = IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		sbuff[i] = 0;
		sptr = &sbuff[i];
	}
	this->txtFilePath = ui->NewTextBox(this->tpControl, CSTRP(sbuff, sptr));
	this->txtFilePath->SetRect(108, 32, 500, 23, false);
	this->btnStart = ui->NewButton(this->tpControl, CSTR("Start"));
	this->btnStart->SetRect(100, 56, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	this->AddTimer(1000, OnTimerTick, this);
	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 300, true));
	this->logger->SetTimeFormat("yyyy-MM-dd HH:mm:ss.fff");
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Command);
}

SSWR::AVIRead::AVIRTFTPServerForm::~AVIRTFTPServerForm()
{
	SDEL_CLASS(this->svr);
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
}

void SSWR::AVIRead::AVIRTFTPServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
