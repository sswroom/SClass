#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGPIOStatusForm.h"

void __stdcall SSWR::AVIRead::AVIRGPIOStatusForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPIOStatusForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPIOStatusForm>();
	UnsafeArray<PinState> states;
	if (me->states.SetTo(states))
	{
		Bool isHigh;
		UOSInt pinMode;
		UOSInt i = 0;
		UOSInt j = me->ctrl->GetPinCount();
		while (i < j)
		{
			pinMode = me->ctrl->GetPinMode(i);
			isHigh = me->ctrl->IsPinHigh(i);
			if (states[i].pinMode != pinMode)
			{
				me->lvStatus->SetSubItem(i, 1, IO::GPIOControl::PinModeGetName(i, pinMode));
				states[i].pinMode = pinMode;
			}
			if (states[i].isHigh != isHigh)
			{
				me->lvStatus->SetSubItem(i, 2, (isHigh?CSTR("1"):CSTR("0")));
				states[i].isHigh = isHigh;
			}
			i++;
		}
	}
}

SSWR::AVIRead::AVIRGPIOStatusForm::AVIRGPIOStatusForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("GPIO Status"));

	this->core = core;
	this->states = 0;
	NEW_CLASSNN(this->ctrl, IO::GPIOControl());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lvStatus = ui->NewListView(*this, UI::ListViewStyle::Table, 3);
	this->lvStatus->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvStatus->SetFullRowSelect(true);
	this->lvStatus->SetShowGrid(true);
	this->lvStatus->AddColumn(CSTR("Name"), 200);
	this->lvStatus->AddColumn(CSTR("Pin Mode"), 100);
	this->lvStatus->AddColumn(CSTR("State"), 80);

	if (!this->ctrl->IsError())
	{
		UTF8Char sbuff[32];
		UnsafeArray<PinState> states;
		UnsafeArray<UTF8Char> sptr;
		UnsafeArray<UTF8Char> sptr2;
		UOSInt i = 0;
		UOSInt j = this->ctrl->GetPinCount();
		this->states = states = MemAllocArr(SSWR::AVIRead::AVIRGPIOStatusForm::PinState, j);
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Pin "));
		while (i < j)
		{
			sptr2 = Text::StrUOSInt(sptr, i);
			this->lvStatus->AddItem(CSTRP(sbuff, sptr2), 0);
			states[i].pinMode = this->ctrl->GetPinMode(i);
			states[i].isHigh = this->ctrl->IsPinHigh(i);
			this->lvStatus->SetSubItem(i, 1, IO::GPIOControl::PinModeGetName(i, states[i].pinMode));
			this->lvStatus->SetSubItem(i, 2, (states[i].isHigh?CSTR("1"):CSTR("0")));
			i++;
		}
	}

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRGPIOStatusForm::~AVIRGPIOStatusForm()
{
	UnsafeArray<PinState> states;
	this->ctrl.Delete();
	if (this->states.SetTo(states))
	{
		MemFreeArr(states);
		this->states = 0;
	}
}

void SSWR::AVIRead::AVIRGPIOStatusForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
