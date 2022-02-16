#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRThreadSpeedForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"

UInt32 __stdcall SSWR::AVIRead::AVIRThreadSpeedForm::TestThread(void *userObj)
{
	SSWR::AVIRead::AVIRThreadSpeedForm *me = (SSWR::AVIRead::AVIRThreadSpeedForm*)userObj;
	me->t = me->clk->GetTimeDiff();
	me->clk->Start();
	me->mainEvt->Set();
	Sync::MutexUsage mutUsage(me->mut);
	Sync::Thread::Sleep(500);
	me->clk->Start();
	mutUsage.EndUse();
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRThreadSpeedForm::OnTestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRThreadSpeedForm *me = (SSWR::AVIRead::AVIRThreadSpeedForm*)userObj;
	UTF8Char sbuff[64];
	Double t;
	UOSInt i;
	me->lvResult->ClearItems();

	NEW_CLASS(me->clk, Manage::HiResClock());
	NEW_CLASS(me->mut, Sync::Mutex());
	NEW_CLASS(me->threadEvt, Sync::Event(true));
	NEW_CLASS(me->mainEvt, Sync::Event(true));
	me->t = 0;

	i = me->lvResult->AddItem(CSTR("Thread Count"), 0);
	Text::StrUOSInt(sbuff, Sync::Thread::GetThreadCnt());
	me->lvResult->SetSubItem(i, 1, sbuff);

	me->clk->Start();
	t = me->clk->GetTimeDiff();
	i = me->lvResult->AddItem(CSTR("Check Time"), 0);
	Text::StrDouble(sbuff, t);
	me->lvResult->SetSubItem(i, 1, sbuff);

	me->clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		Sync::Thread::GetThreadId();
	}
	t = me->clk->GetTimeDiff();
	i = me->lvResult->AddItem(CSTR("GetThreadId"), 0);
	Text::StrDouble(sbuff, t / 1000.0);
	me->lvResult->SetSubItem(i, 1, sbuff);

	me->clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		Sync::MutexUsage mutUsage(me->mut);
		mutUsage.EndUse();
	}
	t = me->clk->GetTimeDiff();
	i = me->lvResult->AddItem(CSTR("Mutex Lock Unlock"), 0);
	Text::StrDouble(sbuff, t / 1000.0);
	me->lvResult->SetSubItem(i, 1, sbuff);

	me->clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		Sync::Interlocked::Increment(&me->tmpVal);
	}
	t = me->clk->GetTimeDiff();
	i = me->lvResult->AddItem(CSTR("Interlocked Increment"), 0);
	Text::StrDouble(sbuff, t / 1000.0);
	me->lvResult->SetSubItem(i, 1, sbuff);

	me->clk->Start();
	i = 1000;
	while (i-- > 0)
	{
		me->mainEvt->Set();
	}
	t = me->clk->GetTimeDiff();
	i = me->lvResult->AddItem(CSTR("Event.Set"), 0);
	Text::StrDouble(sbuff, t / 1000.0);
	me->lvResult->SetSubItem(i, 1, sbuff);

	me->mainEvt->Clear();
	me->t = 0;
	me->clk->Start();
	Sync::Thread::Create(TestThread, me);
	me->mainEvt->Wait(1000);
	t = me->clk->GetTimeDiff();
	i = me->lvResult->AddItem(CSTR("Thread Create"), 0);
	Text::StrDouble(sbuff, me->t);
	me->lvResult->SetSubItem(i, 1, sbuff);
	i = me->lvResult->AddItem(CSTR("Event Wake"), 0);
	Text::StrDouble(sbuff, t);
	me->lvResult->SetSubItem(i, 1, sbuff);

	Sync::Thread::Sleep(100);
	{
		Sync::MutexUsage mutUsage(me->mut);
		t = me->clk->GetTimeDiff();
		mutUsage.EndUse();
	}
	i = me->lvResult->AddItem(CSTR("Mutex Lock Relase"), 0);
	Text::StrDouble(sbuff, t);
	me->lvResult->SetSubItem(i, 1, sbuff);

	DEL_CLASS(me->mainEvt);
	DEL_CLASS(me->threadEvt);
	DEL_CLASS(me->mut);
	DEL_CLASS(me->clk);
}

SSWR::AVIRead::AVIRThreadSpeedForm::AVIRThreadSpeedForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("Thread Speed"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnTest, UI::GUIButton(ui, this->pnlCtrl, CSTR("Test")));
	this->btnTest->SetRect(4, 4, 75, 23, false);
	this->btnTest->HandleButtonClick(OnTestClicked, this);
	NEW_CLASS(this->lvResult, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResult->AddColumn((const UTF8Char*)"Thread Function", 200);
	this->lvResult->AddColumn((const UTF8Char*)"Time (sec)", 200);
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
