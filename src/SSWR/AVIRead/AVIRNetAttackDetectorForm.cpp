#include "Stdafx.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRNetAttackDetectorForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRNetAttackDetectorForm::OnNetAttackEvent(NN<Net::SocketUtil::AddressInfo> addr, Net::NetAttackDetector::AttackType attackType, AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRNetAttackDetectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRNetAttackDetectorForm>();
	Text::StringBuilderUTF8 sb;
	switch (attackType)
	{
	case Net::NetAttackDetector::AttackType::IPScan:
		sb.AppendC(UTF8STRC("IP Scan detected from "));
		break;
	case Net::NetAttackDetector::AttackType::TCPPortScan:
		sb.AppendC(UTF8STRC("TCP Port Scan detected from "));
		break;
	case Net::NetAttackDetector::AttackType::CVE_2026_98432:
		sb.AppendC(UTF8STRC("CVE_2026_98432 detected from "));
		break;
	}
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
	sb.AppendP(sbuff, sptr);
	if (addr->addrType == Net::AddrType::MAC)
	{
		sb.AppendC(UTF8STRC(", vendor = "));
		NN<const Net::MACInfo::MACEntry> macEntry = Net::MACInfo::GetMACInfoBuff(addr->addr);
		sb.AppendC(macEntry->name, macEntry->nameLen);
	}
	me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
}

void __stdcall SSWR::AVIRead::AVIRNetAttackDetectorForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRNetAttackDetectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRNetAttackDetectorForm>();
	NN<Text::String> s = Text::String::OrEmpty(me->lbLog->GetSelectedItemTextNew());
	me->txtLog->SetText(s->ToCString());
	s->Release();
}

SSWR::AVIRead::AVIRNetAttackDetectorForm::AVIRNetAttackDetectorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Net Attack Detector"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASSNN(this->log, IO::LogTool());
	
	this->txtLog = ui->NewTextBox(*this, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(*this, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 300, true));
	this->log->AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);
	NEW_CLASSNN(this->netAttackDetect, Net::NetAttackDetector(this->core->GetSocketFactory(), OnNetAttackEvent, this, 2));
	if (this->netAttackDetect->IsError())
	{
		this->log->LogMessage(CSTR("Error in monitoring to Net Attack"), IO::LogHandler::LogLevel::Error);
	}
}

SSWR::AVIRead::AVIRNetAttackDetectorForm::~AVIRNetAttackDetectorForm()
{
	this->netAttackDetect.Delete();
	this->log.Delete();
	this->logger.Delete();
}

void SSWR::AVIRead::AVIRNetAttackDetectorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
