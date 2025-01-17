#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/SystemInfo.h"
#include "Manage/Benchmark.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRPerformanceLogForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

#define TEST_SIZE (32768*1024)
#define LOOP_CNT 128

void __stdcall SSWR::AVIRead::AVIRPerformanceLogForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPerformanceLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPerformanceLogForm>();
	if (me->testBuff.NotNull())
	{
		me->Stop();
		me->btnStart->SetText(CSTR("Start"));
	}
	else if (me->Start())
	{
		me->btnStart->SetText(CSTR("Stop"));
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in starting logger"), CSTR("Error"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRPerformanceLogForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPerformanceLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPerformanceLogForm>();
	if (me->testBuff.NotNull())
	{
		if (Data::Timestamp::UtcNow().DiffMS(me->testTime) >= 600000)
		{
			me->TestSpeed();
		}
	}
}

Bool SSWR::AVIRead::AVIRPerformanceLogForm::Start()
{
	if (this->testBuff.NotNull())
		return false;

	NN<IO::FileStream> fs;
	NEW_CLASSNN(fs, IO::FileStream(CSTR("Performance.log"), IO::FileMode::Append, IO::FileShare::DenyWrite, IO::FileStream::BufferType::Normal));
	this->logStream = fs;
	if (fs->IsError())
	{
		fs.Delete();
		this->logStream = 0;
		return false;
	}
	NEW_CLASSOPT(this->writer, Text::UTF8Writer(fs));
	this->testBuff = MemAllocAArr(UInt8, TEST_SIZE);
	this->TestSpeed();
	return true;
}

void SSWR::AVIRead::AVIRPerformanceLogForm::Stop()
{
	UnsafeArray<UInt8> testBuff;
	if (this->testBuff.SetTo(testBuff))
	{
		this->writer.Delete();
		this->logStream.Delete();
		MemFreeAArr(testBuff);
		this->testBuff = 0;
	}
}

void SSWR::AVIRead::AVIRPerformanceLogForm::TestSpeed()
{
	UnsafeArray<UInt8> testBuff;
	NN<IO::Writer> writer;
	if (!this->testBuff.SetTo(testBuff) || !this->writer.SetTo(writer))
		return;
	Double t;
	Double spd;
	Manage::HiResClock clk;
	Benchmark_MemWriteTest(testBuff.Ptr(), testBuff.Ptr(), TEST_SIZE, LOOP_CNT);
	t = clk.GetTimeDiff();
	this->testTime = Data::Timestamp::UtcNow();
	Text::StringBuilderUTF8 sb;
	spd = TEST_SIZE / t * LOOP_CNT;
	sb.AppendTSNoZone(this->testTime);
	sb.AppendC(UTF8STRC("\t"));
	sb.AppendDouble(spd);
	writer->WriteLine(sb.ToCString());
	this->rlcWRate->AddSample(&spd);
	sb.ClearStr();
	sb.AppendDouble(spd);
	this->txtCurrWRate->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRPerformanceLogForm::AVIRPerformanceLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetText(CSTR("Performance Log"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->logStream = 0;
	this->writer = 0;
	this->testBuff = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 52, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnStart = ui->NewButton(this->pnlCtrl, CSTR("Start"));
	this->btnStart->SetRect(104, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lblCurrWRate = ui->NewLabel(this->pnlCtrl, CSTR("W Rate"));
	this->lblCurrWRate->SetRect(4, 28, 100, 23, false);
	this->txtCurrWRate = ui->NewTextBox(this->pnlCtrl, CSTR(""));
	this->txtCurrWRate->SetRect(104, 28, 150, 23, false);
	this->txtCurrWRate->SetReadOnly(true);
	this->rlcWRate = ui->NewRealtimeLineChart(*this, this->core->GetDrawEngine(), 1, 144, 1000);
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
