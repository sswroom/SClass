#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGPIOStatusForm.h"

void __stdcall SSWR::AVIRead::AVIRGPIOStatusForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRGPIOStatusForm *me = (SSWR::AVIRead::AVIRGPIOStatusForm*)userObj;
	if (me->states)
	{
		Bool isHigh;
		UOSInt pinMode;
		UOSInt i = 0;
		UOSInt j = me->ctrl->GetPinCount();
		while (i < j)
		{
			pinMode = me->ctrl->GetPinMode(i);
			isHigh = me->ctrl->IsPinHigh(i);
			if (me->states[i].pinMode != pinMode)
			{
				me->lvStatus->SetSubItem(i, 1, IO::GPIOControl::PinModeGetName(i, pinMode));
				me->states[i].pinMode = pinMode;
			}
			if (me->states[i].isHigh != isHigh)
			{
				me->lvStatus->SetSubItem(i, 2, (const UTF8Char*)(isHigh?"1":"0"));
				me->states[i].isHigh = isHigh;
			}
			i++;
		}
	}
}

SSWR::AVIRead::AVIRGPIOStatusForm::AVIRGPIOStatusForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"GPIO Status");

	this->core = core;
	this->states = 0;
	NEW_CLASS(this->ctrl, IO::GPIOControl());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lvStatus, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvStatus->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvStatus->SetFullRowSelect(true);
	this->lvStatus->SetShowGrid(true);
	this->lvStatus->AddColumn((const UTF8Char*)"Name", 200);
	this->lvStatus->AddColumn((const UTF8Char*)"Pin Mode", 100);
	this->lvStatus->AddColumn((const UTF8Char*)"State", 80);

	if (!this->ctrl->IsError())
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		UOSInt i = 0;
		UOSInt j = this->ctrl->GetPinCount();
		this->states = MemAlloc(SSWR::AVIRead::AVIRGPIOStatusForm::PinState, j);
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Pin "));
		while (i < j)
		{
			Text::StrUOSInt(sptr, i);
			this->lvStatus->AddItem(sbuff, 0);
			this->states[i].pinMode = this->ctrl->GetPinMode(i);
			this->states[i].isHigh = this->ctrl->IsPinHigh(i);
			this->lvStatus->SetSubItem(i, 1, IO::GPIOControl::PinModeGetName(i, this->states[i].pinMode));
			this->lvStatus->SetSubItem(i, 2, (const UTF8Char*)(this->states[i].isHigh?"1":"0"));
			i++;
		}
	}

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRGPIOStatusForm::~AVIRGPIOStatusForm()
{
	DEL_CLASS(this->ctrl);
	if (this->states)
	{
		MemFree(this->states);
		this->states = 0;
	}
}

void SSWR::AVIRead::AVIRGPIOStatusForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
