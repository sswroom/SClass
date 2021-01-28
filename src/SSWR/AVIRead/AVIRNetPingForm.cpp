#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRNetPingForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRNetPingForm::OnPingClicked(void *userObj)
{
	SSWR::AVIRead::AVIRNetPingForm *me = (SSWR::AVIRead::AVIRNetPingForm*)userObj;
	if (me->targetAddr.addrType != Net::SocketUtil::AT_UNKNOWN)
	{
		me->targetAddr.addrType = Net::SocketUtil::AT_UNKNOWN;
		me->chkRepeat->SetEnabled(true);
		me->txtTarget->SetReadOnly(false);
	}
	else
	{
		Net::SocketUtil::AddressInfo addr;
		Text::StringBuilderUTF8 sb;
		me->txtTarget->GetText(&sb);
		if (!me->sockf->DNSResolveIP(sb.ToString(), &addr))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Error, target name is not valid", (const UTF8Char *)"Error", me);
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
			Int32 t1;
			Int32 ttl;
			Double t2;
			sb.ClearStr();
			Manage::HiResClock clk;
			if (me->sockf->IcmpSendEcho2(&addr, &t1, &ttl))
			{
				t2 = clk.GetTimeDiff();
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Ping time: round trip = ");
				Text::SBAppendF64(&sb, t1 / 1000.0);
				sb.Append((const UTF8Char*)"ms, calc time = ");
				Text::SBAppendF64(&sb, t2 * 1000);
				sb.Append((const UTF8Char*)"ms, ttl = ");
				sb.AppendI32(ttl);
				me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_COMMAND);
			}
			else
			{
				me->log->LogMessage((const UTF8Char*)"Ping: no response from target", IO::ILogHandler::LOG_LEVEL_COMMAND);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRNetPingForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRNetPingForm *me = (SSWR::AVIRead::AVIRNetPingForm*)userObj;
	if (me->targetAddr.addrType != Net::SocketUtil::AT_UNKNOWN)
	{
		Int32 t1;
		Int32 ttl;
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
			sb.Append((const UTF8Char*)"Ping time: round trip = ");
			Text::SBAppendF64(&sb, t1 / 1000.0);
			sb.Append((const UTF8Char*)"ms, calc time = ");
			Text::SBAppendF64(&sb, t2 * 1000);
			sb.Append((const UTF8Char*)"ms, ttl = ");
			sb.AppendI32(ttl);
			me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_COMMAND);
			me->rlcPing->AddSample(t);
		}
		else
		{
			t[0] = 1000;
			t[1] = 1000;
			me->log->LogMessage((const UTF8Char*)"Ping: no response from target", IO::ILogHandler::LOG_LEVEL_COMMAND);
			me->rlcPing->AddSample(t);
		}
	}
}

SSWR::AVIRead::AVIRNetPingForm::AVIRNetPingForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Network Ping");

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->targetAddr.addrType = Net::SocketUtil::AT_UNKNOWN;
	NEW_CLASS(this->log, IO::LogTool());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 80, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->vspRequest, UI::GUIVSplitter(ui, this, 3, false));
	NEW_CLASS(this->lblTarget, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Target"));
	this->lblTarget->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtTarget, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)""));
	this->txtTarget->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblRepeat, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Repeat"));
	this->lblRepeat->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->chkRepeat, UI::GUICheckBox(ui, this->pnlRequest, (const UTF8Char*)"", false));
	this->chkRepeat->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->btnPing, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"&Ping"));
	this->btnPing->SetRect(104, 52, 75, 23, false);
	this->btnPing->HandleButtonClick(OnPingClicked, this);
	NEW_CLASS(this->rlcPing, UI::GUIRealtimeLineChart(ui, this, this->core->GetDrawEngine(), 2, 600, 1000));
	this->rlcPing->SetRect(0, 0, 100, 100, false);
	this->rlcPing->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->rlcPing->SetUnit((const UTF8Char*)"ms");
	NEW_CLASS(this->vspPing, UI::GUIVSplitter(ui, this, 3, true));
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 500, true));
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_RAW);
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
