#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRNetPingForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRNetPingForm::OnPingClicked(void *userObj)
{
	SSWR::AVIRead::AVIRNetPingForm *me = (SSWR::AVIRead::AVIRNetPingForm*)userObj;
	if (me->targetAddr.addrType != Net::AddrType::Unknown)
	{
		me->targetAddr.addrType = Net::AddrType::Unknown;
		me->chkRepeat->SetEnabled(true);
		me->txtTarget->SetReadOnly(false);
	}
	else
	{
		Net::SocketUtil::AddressInfo addr;
		Text::StringBuilderUTF8 sb;
		me->txtTarget->GetText(&sb);
		if (!me->sockf->DNSResolveIP(sb.ToCString(), &addr))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error, target name is not valid"), CSTR("Error"), me);
			return;
		}
		if (me->chkRepeat->IsChecked())
		{
			me->targetAddr = addr;
			me->chkRepeat->SetEnabled(false);
			me->txtTarget->SetReadOnly(true);
			me->rlcPing->ClearChart();
		}
		else
		{
			UInt32 t1;
			UInt32 ttl;
			Double t2;
			sb.ClearStr();
			Manage::HiResClock clk;
			if (me->sockf->IcmpSendEcho2(&addr, &t1, &ttl))
			{
				t2 = clk.GetTimeDiff();
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Ping time: round trip = "));
				Text::SBAppendF64(&sb, t1 / 1000.0);
				sb.AppendC(UTF8STRC("ms, calc time = "));
				Text::SBAppendF64(&sb, t2 * 1000);
				sb.AppendC(UTF8STRC("ms, ttl = "));
				sb.AppendU32(ttl);
				me->log->LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::Command);
			}
			else
			{
				me->log->LogMessage(CSTR("Ping: no response from target"), IO::ILogHandler::LogLevel::Command);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRNetPingForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRNetPingForm *me = (SSWR::AVIRead::AVIRNetPingForm*)userObj;
	if (me->targetAddr.addrType != Net::AddrType::Unknown)
	{
		UInt32 t1;
		UInt32 ttl;
		Double t2;
		Double t[2];
		Manage::HiResClock clk;
		if (me->sockf->IcmpSendEcho2(&me->targetAddr, &t1, &ttl))
		{
			t2 = clk.GetTimeDiff();
			Text::StringBuilderUTF8 sb;
			t[0] = t1 / 1000.0;
			t[1] = t2 * 1000;
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Ping time: round trip = "));
			Text::SBAppendF64(&sb, t1 / 1000.0);
			sb.AppendC(UTF8STRC("ms, calc time = "));
			Text::SBAppendF64(&sb, t2 * 1000);
			sb.AppendC(UTF8STRC("ms, ttl = "));
			sb.AppendU32(ttl);
			me->log->LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::Command);
			me->rlcPing->AddSample(t);
		}
		else
		{
			t[0] = 1000;
			t[1] = 1000;
			me->log->LogMessage(CSTR("Ping: no response from target"), IO::ILogHandler::LogLevel::Command);
			me->rlcPing->AddSample(t);
		}
	}
}

SSWR::AVIRead::AVIRNetPingForm::AVIRNetPingForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Network Ping"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->targetAddr.addrType = Net::AddrType::Unknown;
	NEW_CLASS(this->log, IO::LogTool());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 80, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->vspRequest, UI::GUIVSplitter(ui, this, 3, false));
	NEW_CLASS(this->lblTarget, UI::GUILabel(ui, this->pnlRequest, CSTR("Target")));
	this->lblTarget->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtTarget, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtTarget->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblRepeat, UI::GUILabel(ui, this->pnlRequest, CSTR("Repeat")));
	this->lblRepeat->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->chkRepeat, UI::GUICheckBox(ui, this->pnlRequest, CSTR(""), false));
	this->chkRepeat->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->btnPing, UI::GUIButton(ui, this->pnlRequest, CSTR("&Ping")));
	this->btnPing->SetRect(104, 52, 75, 23, false);
	this->btnPing->HandleButtonClick(OnPingClicked, this);
	NEW_CLASS(this->rlcPing, UI::GUIRealtimeLineChart(ui, this, this->core->GetDrawEngine(), 2, 600, 1000));
	this->rlcPing->SetRect(0, 0, 100, 100, false);
	this->rlcPing->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->rlcPing->SetUnit(CSTR("ms"));
	NEW_CLASS(this->vspPing, UI::GUIVSplitter(ui, this, 3, true));
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 500, true));
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LogLevel::Raw);
	this->SetDefaultButton(this->btnPing);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRNetPingForm::~AVIRNetPingForm()
{
	DEL_CLASS(this->log);
	DEL_CLASS(this->logger);
}

void SSWR::AVIRead::AVIRNetPingForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
