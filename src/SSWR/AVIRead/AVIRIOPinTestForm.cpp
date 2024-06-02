#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRIOPinTestForm.h"

void __stdcall SSWR::AVIRead::AVIRIOPinTestForm::OnPullUpClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRIOPinTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIOPinTestForm>();
	me->pin->SetPullType(IO::IOPin::PT_UP);
}

void __stdcall SSWR::AVIRead::AVIRIOPinTestForm::OnPullDownClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRIOPinTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIOPinTestForm>();
	me->pin->SetPullType(IO::IOPin::PT_DOWN);
}

void __stdcall SSWR::AVIRead::AVIRIOPinTestForm::OnPinModeInputClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRIOPinTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIOPinTestForm>();
	me->pin->SetPinOutput(false);
	me->isOutput = false;
}

void __stdcall SSWR::AVIRead::AVIRIOPinTestForm::OnPinModeOutputClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRIOPinTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIOPinTestForm>();
	me->pin->SetPinOutput(true);
	me->isOutput = true;
}

void __stdcall SSWR::AVIRead::AVIRIOPinTestForm::OnPinHighClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRIOPinTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIOPinTestForm>();
	if (me->isOutput)
	{
		me->pin->SetPinState(true);
		me->txtPinState->SetText(CSTR("High"));
	}
}

void __stdcall SSWR::AVIRead::AVIRIOPinTestForm::OnPinLowClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRIOPinTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIOPinTestForm>();
	if (me->isOutput)
	{
		me->pin->SetPinState(false);
		me->txtPinState->SetText(CSTR("Low"));
	}
}

void __stdcall SSWR::AVIRead::AVIRIOPinTestForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRIOPinTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRIOPinTestForm>();
	if (!me->isOutput)
	{
		if (me->pin->IsPinHigh())
		{
			me->txtPinState->SetText(CSTR("High"));
		}
		else
		{
			me->txtPinState->SetText(CSTR("Low"));
		}
	}
}

SSWR::AVIRead::AVIRIOPinTestForm::AVIRIOPinTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::IOPin> pin) : UI::GUIForm(parent, 480, 160, ui)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	this->SetFont(0, 0, 8.25, false);
	this->pin = pin;
	this->core = core;
	this->isOutput = false;
	sptr = this->pin->GetName(Text::StrConcatC(sbuff, UTF8STRC("IO Pin Test - ")));
	this->SetText(CSTRP(sbuff, sptr));
	this->pin->SetPinOutput(false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblPullType = ui->NewLabel(*this, CSTR("Pull Type"));
	this->lblPullType->SetRect(4, 4, 100, 23, false);
	this->btnPullUp = ui->NewButton(*this, CSTR("Pull Up"));
	this->btnPullUp->SetRect(104, 4, 75, 23, false);
	this->btnPullUp->HandleButtonClick(OnPullUpClicked, this);
	this->btnPullDown = ui->NewButton(*this, CSTR("Pull Down"));
	this->btnPullDown->SetRect(184, 4, 75, 23, false);
	this->btnPullDown->HandleButtonClick(OnPullDownClicked, this);
	this->lblPinMode = ui->NewLabel(*this, CSTR("Pin Mode"));
	this->lblPinMode->SetRect(4, 28, 100, 23, false);
	this->btnPinModeInput = ui->NewButton(*this, CSTR("Input"));
	this->btnPinModeInput->SetRect(104, 28, 75, 23, false);
	this->btnPinModeInput->HandleButtonClick(OnPinModeInputClicked, this);
	this->btnPinModeOutput = ui->NewButton(*this, CSTR("Output"));
	this->btnPinModeOutput->SetRect(184, 28, 75, 23, false);
	this->btnPinModeOutput->HandleButtonClick(OnPinModeOutputClicked, this);
	this->lblPinState = ui->NewLabel(*this, CSTR("Pin State"));
	this->lblPinState->SetRect(4, 52, 100, 23, false);
	this->txtPinState = ui->NewTextBox(*this, CSTR(""));
	this->txtPinState->SetRect(104, 52, 100, 23, false);
	this->txtPinState->SetReadOnly(true);
	this->btnPinHigh = ui->NewButton(*this, CSTR("High"));
	this->btnPinHigh->SetRect(204, 52, 75, 23, false);
	this->btnPinHigh->HandleButtonClick(OnPinHighClicked, this);
	this->btnPinLow = ui->NewButton(*this, CSTR("Low"));
	this->btnPinLow->SetRect(284, 52, 75, 23, false);
	this->btnPinLow->HandleButtonClick(OnPinLowClicked, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRIOPinTestForm::~AVIRIOPinTestForm()
{
	this->pin.Delete();
}

void SSWR::AVIRead::AVIRIOPinTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
