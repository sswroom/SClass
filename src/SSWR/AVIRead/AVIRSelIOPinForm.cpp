#include "Stdafx.h"
#include "IO/GPIOPin.h"
#include "SSWR/AVIRead/AVIRSelIOPinForm.h"

void __stdcall SSWR::AVIRead::AVIRSelIOPinForm::OnOKClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSelIOPinForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSelIOPinForm>();
	SSWR::AVIRead::AVIRCore::IOPinType iopt = (SSWR::AVIRead::AVIRCore::IOPinType)me->cboPinType->GetSelectedItem().GetOSInt();

	if (iopt == SSWR::AVIRead::AVIRCore::IOPT_GPIO)
	{
		UOSInt i = me->cboGPIO->GetSelectedIndex();
		UInt16 pinNum = (UInt16)me->cboGPIO->GetItem((UOSInt)i).GetUOSInt();
		if (i == INVALID_INDEX)
		{
			me->ui->ShowMsgOK(CSTR("Please select a GPIO"), CSTR("Select GPIO"), me);
			return;
		}
		NN<IO::GPIOControl> gpioPin;
		if (!gpioPin.Set(me->gpioPin))
		{
			me->ui->ShowMsgOK(CSTR("System does not have GPIO"), CSTR("Select GPIO"), me);
			return;
		}
		IO::GPIOPin *pin;
		NEW_CLASS(pin, IO::GPIOPin(gpioPin, pinNum));
		if (pin->IsError())
		{
			DEL_CLASS(pin);
			me->ui->ShowMsgOK(CSTR("Error in opening the pin"), CSTR("Select GPIO"), me);
			return;
		}
		me->ioPin = pin;
		me->ioPinType = iopt;
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else if (iopt == SSWR::AVIRead::AVIRCore::IOPT_VIOPIN)
	{
		UOSInt i = me->cboVirtualPin->GetSelectedIndex();
		UInt16 pinNum = (UInt16)me->cboVirtualPin->GetItem(i).GetUOSInt();
		if (i == INVALID_INDEX)
		{
			me->ui->ShowMsgOK(CSTR("Please select a VirtualPin"), CSTR("Select VirtualPin"), me);
			return;
		}
		NN<IO::IOPin> pin;
		if (!me->vioPinMgr->CreatePin(pinNum).SetTo(pin))
		{
			me->ui->ShowMsgOK(CSTR("Error in opening the pin"), CSTR("Select VirtualPin"), me);
			return;
		}
		me->ioPin = pin;
		me->ioPinType = iopt;
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRSelIOPinForm::OnCancelClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSelIOPinForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSelIOPinForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRSelIOPinForm::OnPinTypeChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSelIOPinForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSelIOPinForm>();
	UOSInt i = me->cboPinType->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		SSWR::AVIRead::AVIRCore::IOPinType iopt = (SSWR::AVIRead::AVIRCore::IOPinType)me->cboPinType->GetItem(i).GetOSInt();
		if (iopt == SSWR::AVIRead::AVIRCore::IOPT_GPIO)
		{
			me->tcConfig->SetSelectedPage(me->tpGPIO);
		}
		else if (iopt == SSWR::AVIRead::AVIRCore::IOPT_VIOPIN)
		{
			me->tcConfig->SetSelectedPage(me->tpVirtualPin);
		}
	}
}

SSWR::AVIRead::AVIRSelIOPinForm::AVIRSelIOPinForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	Data::ArrayListInt32 *ports;

	this->SetText(CSTR("Select IOPin"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->vioPinMgr = this->core->GetVirtualIOPinMgr();
	this->gpioPin = this->core->GetGPIOControl();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlPinType = ui->NewPanel(*this);
	this->pnlPinType->SetRect(0, 0, 100, 31, false);
	this->pnlPinType->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPinType = ui->NewLabel(this->pnlPinType, CSTR("Pin Type"));
	this->lblPinType->SetRect(4, 4, 100, 23, false);
	this->cboPinType = ui->NewComboBox(this->pnlPinType, false);
	this->cboPinType->SetRect(104, 4, 200, 23, false);
	this->cboPinType->AddItem(SSWR::AVIRead::AVIRCore::IOPinTypeGetName(SSWR::AVIRead::AVIRCore::IOPT_GPIO), (void*)SSWR::AVIRead::AVIRCore::IOPT_GPIO);
	this->cboPinType->AddItem(SSWR::AVIRead::AVIRCore::IOPinTypeGetName(SSWR::AVIRead::AVIRCore::IOPT_VIOPIN), (void*)SSWR::AVIRead::AVIRCore::IOPT_VIOPIN);
	this->cboPinType->HandleSelectionChange(OnPinTypeChg, this);

	this->pnlButtons = ui->NewPanel(*this);
	this->pnlButtons->SetRect(0, 0, 100, 31, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnOK = ui->NewButton(this->pnlButtons, CSTR("&OK"));
	this->btnOK->SetRect(49, 4, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(this->pnlButtons, CSTR("&Cancel"));
	this->btnCancel->SetRect(132, 4, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);

	this->tcConfig = ui->NewTabControl(*this);
	this->tcConfig->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpGPIO = this->tcConfig->AddTabPage(CSTR("GPIO"));
	this->lblGPIO = ui->NewLabel(this->tpGPIO, CSTR("GPIO Pin"));
	this->lblGPIO->SetRect(8, 8, 100, 23, false);
	this->cboGPIO = ui->NewComboBox(this->tpGPIO, false);
	this->cboGPIO->SetRect(108, 8, 100, 23, false);

	this->tpVirtualPin = this->tcConfig->AddTabPage(CSTR("VirtualPin"));
	this->lblVirtualPin = ui->NewLabel(this->tpVirtualPin, CSTR("VirtualPin"));
	this->lblVirtualPin->SetRect(8, 8, 100, 23, false);
	this->cboVirtualPin = ui->NewComboBox(this->tpVirtualPin, false);
	this->cboVirtualPin->SetRect(108, 8, 100, 23, false);

	Int32 currPort;
	if (this->gpioPin)
	{
		i = 0;
		j = this->gpioPin->GetPinCount();
		while (i < j)
		{
			currPort = (Int32)i;
			sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("GPIO")), currPort);
			this->cboGPIO->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)currPort);
			i++;
		}
		if (j > 0)
		{
			this->cboGPIO->SetSelectedIndex(0);
		}
	}
	NEW_CLASS(ports, Data::ArrayListInt32());
	ports->Clear();
	this->vioPinMgr->GetAvailablePins(ports);
	i = 0;
	j = ports->GetCount();
	while (i < j)
	{
		currPort = ports->GetItem(i);
		sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("VirtualIOPin")), currPort);
		this->cboVirtualPin->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)currPort);
		i++;
	}
	if (j > 0)
	{
		this->cboVirtualPin->SetSelectedIndex(0);
	}
	DEL_CLASS(ports);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->cboPinType->SetSelectedIndex(0);
}

SSWR::AVIRead::AVIRSelIOPinForm::~AVIRSelIOPinForm()
{
}

void SSWR::AVIRead::AVIRSelIOPinForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
