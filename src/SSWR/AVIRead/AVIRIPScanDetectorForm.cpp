#include "Stdafx.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRIPScanDetectorForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRIPScanDetectorForm::OnIPScanEvent(const UInt8 *hwAddr, void *userObj)
{
	SSWR::AVIRead::AVIRIPScanDetectorForm *me = (SSWR::AVIRead::AVIRIPScanDetectorForm *)userObj;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("IP Scan detected from "));
	sb.AppendHexBuff(hwAddr, 6, ':', Text::LineBreakType::None);
	sb.AppendC(UTF8STRC(", vendor = "));
	const Net::MACInfo::MACEntry *macEntry = Net::MACInfo::GetMACInfoBuff(hwAddr);
	if (macEntry)
	{
		sb.AppendC(macEntry->name, macEntry->nameLen);
	}
	me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_COMMAND);
}

void __stdcall SSWR::AVIRead::AVIRIPScanDetectorForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRIPScanDetectorForm *me = (SSWR::AVIRead::AVIRIPScanDetectorForm *)userObj;
	Text::String *s = Text::String::OrEmpty(me->lbLog->GetSelectedItemTextNew());
	me->txtLog->SetText(s->v);
	s->Release();
}

SSWR::AVIRead::AVIRIPScanDetectorForm::AVIRIPScanDetectorForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"IP Scan Detector");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->log, IO::LogTool());
	
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 300, true));
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_RAW);
	NEW_CLASS(this->ipScanDetect, Net::IPScanDetector(this->core->GetSocketFactory(), OnIPScanEvent, this, 2));
	if (this->ipScanDetect->IsError())
	{
		this->log->LogMessageC(UTF8STRC("Error in monitoring to IP Scan"), IO::ILogHandler::LOG_LEVEL_ERROR);
	}
}

SSWR::AVIRead::AVIRIPScanDetectorForm::~AVIRIPScanDetectorForm()
{
	DEL_CLASS(this->ipScanDetect);
	DEL_CLASS(this->log);
}

void SSWR::AVIRead::AVIRIPScanDetectorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
