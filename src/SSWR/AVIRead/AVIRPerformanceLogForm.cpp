#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/SystemInfo.h"
#include "Manage/Benchmark.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRPerformanceLogForm.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

#define TEST_SIZE (32768*1024)
#define LOOP_CNT 128

void __stdcall SSWR::AVIRead::AVIRPerformanceLogForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRPerformanceLogForm *me = (SSWR::AVIRead::AVIRPerformanceLogForm*)userObj;
	if (me->testBuff)
	{
		me->Stop();
		me->btnStart->SetText((const UTF8Char*)"Start");
	}
	else if (me->Start())
	{
		me->btnStart->SetText((const UTF8Char*)"Stop");
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in starting logger", (const UTF8Char *)"Error", me);
	}
}

void __stdcall SSWR::AVIRead::AVIRPerformanceLogForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRPerformanceLogForm *me = (SSWR::AVIRead::AVIRPerformanceLogForm*)userObj;
	if (me->testBuff)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(me->testTime) >= 600000)
		{
			me->TestSpeed();
		}
	}
}

Bool SSWR::AVIRead::AVIRPerformanceLogForm::Start()
{
	if (this->testBuff)
		return false;

	NEW_CLASS(this->logStream, IO::FileStream((const UTF8Char*)"Performance.log", IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_WRITE, IO::FileStream::BT_NORMAL));
	if (this->logStream->IsError())
	{
		DEL_CLASS(this->logStream);
		this->logStream = 0;
		return false;
	}
	NEW_CLASS(this->writer, Text::UTF8Writer(this->logStream));
	this->testBuff = MemAllocA(UInt8, TEST_SIZE);
	NEW_CLASS(this->testTime, Data::DateTime());
	this->TestSpeed();
	return true;
}

void SSWR::AVIRead::AVIRPerformanceLogForm::Stop()
{
	if (this->testBuff)
	{
		DEL_CLASS(this->testTime);
		this->testTime = 0;
		DEL_CLASS(this->writer);
		this->writer = 0;
		DEL_CLASS(this->logStream);
		this->logStream = 0;
		MemFreeA(this->testBuff);
		this->testBuff = 0;
	}
}

void SSWR::AVIRead::AVIRPerformanceLogForm::TestSpeed()
{
	if (this->testBuff == 0)
		return;
	Double t;
	Double spd;
	Manage::HiResClock clk;
	Benchmark_MemWriteTest(this->testBuff, this->testBuff, TEST_SIZE, LOOP_CNT);
	t = clk.GetTimeDiff();
	this->testTime->SetCurrTimeUTC();
	Text::StringBuilderUTF8 sb;
	spd = TEST_SIZE / t * LOOP_CNT;
	sb.AppendDate(this->testTime);
	sb.Append((const UTF8Char*)"\t");
	Text::SBAppendF64(&sb, spd);
	this->writer->WriteLine(sb.ToString());
	this->rlcWRate->AddSample(&spd);
	sb.ClearStr();
	Text::SBAppendF64(&sb, spd);
	this->txtCurrWRate->SetText(sb.ToString());
}

SSWR::AVIRead::AVIRPerformanceLogForm::AVIRPerformanceLogForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetText((const UTF8Char*)"Performance Log");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->logStream = 0;
	this->writer = 0;
	this->testBuff = 0;
	this->testTime = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 52, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(104, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->lblCurrWRate, UI::GUILabel(ui, this->pnlCtrl, (const UTF8Char*)"W Rate"));
	this->lblCurrWRate->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtCurrWRate, UI::GUITextBox(ui, this->pnlCtrl, (const UTF8Char*)""));
	this->txtCurrWRate->SetRect(104, 28, 150, 23, false);
	this->txtCurrWRate->SetReadOnly(true);
	NEW_CLASS(this->rlcWRate, UI::GUIRealtimeLineChart(ui, this, this->core->GetDrawEngine(), 1, 144, 1000));
	this->rlcWRate->SetDockType(UI::GUIControl::DOCK_FILL);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRPerformanceLogForm::~AVIRPerformanceLogForm()
{
	this->Stop();
}

void SSWR::AVIRead::AVIRPerformanceLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
