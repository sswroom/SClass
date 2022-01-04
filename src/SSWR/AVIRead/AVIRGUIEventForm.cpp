#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGUIEventForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRGUIEventForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRGUIEventForm *me = (SSWR::AVIRead::AVIRGUIEventForm*)userObj;
	Text::String *s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(s->v);
	s->Release();
}

void __stdcall SSWR::AVIRead::AVIRGUIEventForm::OnDisplayOffClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGUIEventForm *me = (SSWR::AVIRead::AVIRGUIEventForm*)userObj;
	me->log->LogMessageC(UTF8STRC("DisplayOff"), IO::ILogHandler::LOG_LEVEL_ACTION);
	me->ui->DisplayOff();
}

void __stdcall SSWR::AVIRead::AVIRGUIEventForm::OnKeyDown(void *userObj, UOSInt keyCode, Bool extendedKey)
{
	SSWR::AVIRead::AVIRGUIEventForm *me = (SSWR::AVIRead::AVIRGUIEventForm*)userObj;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Key Down - "));
	sb.Append(GUIKeyGetName(OSKey2GUIKey((UInt32)keyCode)));
	me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_ACTION);
}

SSWR::AVIRead::AVIRGUIEventForm::AVIRGUIEventForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"GUI Event");
	
	this->core = core;
	NEW_CLASS(this->log, IO::LogTool());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlMain, UI::GUIPanel(ui, this));
	this->pnlMain->SetRect(0, 0, 100, 31, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDisplayOff, UI::GUIButton(ui, this->pnlMain, (const UTF8Char*)"Display Off"));
	this->btnDisplayOff->SetRect(4, 4, 75, 23, false);
	this->btnDisplayOff->HandleButtonClick(OnDisplayOffClicked, this);
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);
	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 300, false));
	this->logger->SetTimeFormat("yyyy-MM-dd HH:mm:ss.fff");
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_RAW);
	this->HandleKeyDown(OnKeyDown, this);
}

SSWR::AVIRead::AVIRGUIEventForm::~AVIRGUIEventForm()
{
	DEL_CLASS(this->log);
	DEL_CLASS(this->logger);
}

Bool SSWR::AVIRead::AVIRGUIEventForm::OnPaint()
{
	this->log->LogMessageC(UTF8STRC("Paint"), IO::ILogHandler::LOG_LEVEL_ACTION);
	return false;
}

void SSWR::AVIRead::AVIRGUIEventForm::OnMonitorChanged()
{
	this->log->LogMessageC(UTF8STRC("MonitorChanged"), IO::ILogHandler::LOG_LEVEL_ACTION);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGUIEventForm::OnFocus()
{
	this->log->LogMessageC(UTF8STRC("Focus"), IO::ILogHandler::LOG_LEVEL_ACTION);
}

void SSWR::AVIRead::AVIRGUIEventForm::OnFocusLost()
{
	this->log->LogMessageC(UTF8STRC("FocusLost"), IO::ILogHandler::LOG_LEVEL_ACTION);
}

void SSWR::AVIRead::AVIRGUIEventForm::OnDisplaySizeChange(UOSInt dispWidth, UOSInt dispHeight)
{
	this->log->LogMessageC(UTF8STRC("DisplaySizeChange"), IO::ILogHandler::LOG_LEVEL_ACTION);
}
