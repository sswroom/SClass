#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRThreadSpeedForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"

UInt32 __stdcall SSWR::AVIRead::AVIRThreadSpeedForm::TestThread(void *userObj)
{
	SSWR::AVIRead::AVIRThreadSpeedForm *me = (SSWR::AVIRead::AVIRThreadSpeedForm*)userObj;
	me->t = me->clk.GetTimeDiff();
	me->clk.Start();
	me->mainEvt.Set();
	Sync::MutexUsage mutUsage(me->mut);
	Sync::SimpleThread::Sleep(500);
	me->clk.Start();
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRThreadSpeedForm::OnTestClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRThreadSpeedForm> me = userObj.GetNN<SSWR::AVIRead::AVIRThreadSpeedForm>();
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Double t;
	UOSInt i;
	me->lvResult->ClearItems();

	me->t = 0;

	i = me->lvResult->AddItem(CSTR("Thread Count"), 0);
	sptr = Text::StrUOSInt(sbuff, Sync::ThreadUtil::GetThreadCnt());
	me->lvResult->SetSubItem(i, 1, CSTRP(sbuff, sptr));

	me->clk.Start();
	t = me->clk.GetTimeDiff();
	i = me->lvResult->AddItem(CSTR("Check Time"), 0);
	sptr = Text::StrDouble(sbuff, t);
	me->lvResult->SetSubItem(i, 1, CSTRP(sbuff, sptr));

	me->clk.Start();
	i = 1000;
	while (i-- > 0)
	{
		Sync::ThreadUtil::GetThreadId();
	}
	t = me->clk.GetTimeDiff();
	i = me->lvResult->AddItem(CSTR("GetThreadId"), 0);
	sptr = Text::StrDouble(sbuff, t / 1000.0);
	me->lvResult->SetSubItem(i, 1, CSTRP(sbuff, sptr));

	me->clk.Start();
	i = 1000;
	while (i-- > 0)
	{
		Sync::MutexUsage mutUsage(me->mut);
	}
	t = me->clk.GetTimeDiff();
	i = me->lvResult->AddItem(CSTR("Mutex Lock Unlock"), 0);
	sptr = Text::StrDouble(sbuff, t / 1000.0);
	me->lvResult->SetSubItem(i, 1, CSTRP(sbuff, sptr));

	me->clk.Start();
	i = 1000;
	while (i-- > 0)
	{
		Sync::Interlocked::IncrementI32(me->tmpVal);
	}
	t = me->clk.GetTimeDiff();
	i = me->lvResult->AddItem(CSTR("Interlocked Increment"), 0);
	sptr = Text::StrDouble(sbuff, t / 1000.0);
	me->lvResult->SetSubItem(i, 1, CSTRP(sbuff, sptr));

	me->clk.Start();
	i = 1000;
	while (i-- > 0)
	{
		me->mainEvt.Set();
	}
	t = me->clk.GetTimeDiff();
	i = me->lvResult->AddItem(CSTR("Event.Set"), 0);
	sptr = Text::StrDouble(sbuff, t / 1000.0);
	me->lvResult->SetSubItem(i, 1, CSTRP(sbuff, sptr));

	me->mainEvt.Clear();
	me->t = 0;
	me->clk.Start();
	Sync::ThreadUtil::Create(TestThread, me.Ptr());
	me->mainEvt.Wait(1000);
	t = me->clk.GetTimeDiff();
	i = me->lvResult->AddItem(CSTR("Thread Create"), 0);
	sptr = Text::StrDouble(sbuff, me->t);
	me->lvResult->SetSubItem(i, 1, CSTRP(sbuff, sptr));
	i = me->lvResult->AddItem(CSTR("Event Wake"), 0);
	sptr = Text::StrDouble(sbuff, t);
	me->lvResult->SetSubItem(i, 1, CSTRP(sbuff, sptr));

	Sync::SimpleThread::Sleep(100);
	{
		Sync::MutexUsage mutUsage(me->mut);
		t = me->clk.GetTimeDiff();
	}
	i = me->lvResult->AddItem(CSTR("Mutex Lock Relase"), 0);
	sptr = Text::StrDouble(sbuff, t);
	me->lvResult->SetSubItem(i, 1, CSTRP(sbuff, sptr));
}

SSWR::AVIRead::AVIRThreadSpeedForm::AVIRThreadSpeedForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("Thread Speed"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnTest = ui->NewButton(this->pnlCtrl, CSTR("Test"));
	this->btnTest->SetRect(4, 4, 75, 23, false);
	this->btnTest->HandleButtonClick(OnTestClicked, this);
	this->lvResult = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResult->AddColumn(CSTR("Thread Function"), 200);
	this->lvResult->AddColumn(CSTR("Time (sec)"), 200);
	this->lvResult->SetShowGrid(true);
	this->lvResult->SetFullRowSelect(true);

}

SSWR::AVIRead::AVIRThreadSpeedForm::~AVIRThreadSpeedForm()
{
}

void SSWR::AVIRead::AVIRThreadSpeedForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
