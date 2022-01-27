#include "Stdafx.h"
#include "IO/GPIOPin.h"
#include "SSWR/AVIRead/AVIRSelIOPinForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSelIOPinForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIRSelIOPinForm *me = (SSWR::AVIRead::AVIRSelIOPinForm*)userObj;
	SSWR::AVIRead::AVIRCore::IOPinType iopt = (SSWR::AVIRead::AVIRCore::IOPinType)(OSInt)me->cboPinType->GetSelectedItem();

	if (iopt == SSWR::AVIRead::AVIRCore::IOPT_GPIO)
	{
		UOSInt i = me->cboGPIO->GetSelectedIndex();
		UInt16 pinNum = (UInt16)(UOSInt)me->cboGPIO->GetItem((UOSInt)i);
		if (i == INVALID_INDEX)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please select a GPIO", (const UTF8Char*)"Select GPIO", me);
			return;
		}
		IO::GPIOPin *pin;
		NEW_CLASS(pin, IO::GPIOPin(me->gpioPin, pinNum));
		if (pin->IsError())
		{
			DEL_CLASS(pin);
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in opening the pin", (const UTF8Char*)"Select GPIO", me);
			return;
		}
		me->ioPin = pin;
		me->ioPinType = iopt;
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else if (iopt == SSWR::AVIRead::AVIRCore::IOPT_VIOPIN)
	{
		UOSInt i = me->cboVirtualPin->GetSelectedIndex();
		UInt16 pinNum = (UInt16)(UOSInt)me->cboVirtualPin->GetItem(i);
		if (i == INVALID_INDEX)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please select a VirtualPin", (const UTF8Char*)"Select VirtualPin", me);
			return;
		}
		IO::IOPin *pin;
		pin = me->vioPinMgr->CreatePin(pinNum);
		if (pin == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in opening the pin", (const UTF8Char*)"Select VirtualPin", me);
			return;
		}
		me->ioPin = pin;
		me->ioPinType = iopt;
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRSelIOPinForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIRSelIOPinForm *me = (SSWR::AVIRead::AVIRSelIOPinForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRSelIOPinForm::OnPinTypeChg(void *userObj)
{
	SSWR::AVIRead::AVIRSelIOPinForm *me = (SSWR::AVIRead::AVIRSelIOPinForm*)userObj;
	UOSInt i = me->cboPinType->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		SSWR::AVIRead::AVIRCore::IOPinType iopt = (SSWR::AVIRead::AVIRCore::IOPinType)(OSInt)me->cboPinType->GetItem(i);
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

SSWR::AVIRead::AVIRSelIOPinForm::AVIRSelIOPinForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 480, ui)
{
	UTF8Char sbuff[32];
	UOSInt i;
	UOSInt j;
	Data::ArrayListInt32 *ports;

	this->SetText((const UTF8Char*)"Select IOPin");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->vioPinMgr = this->core->GetVirtualIOPinMgr();
	this->gpioPin = this->core->GetGPIOControl();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlPinType, UI::GUIPanel(ui, this));
	this->pnlPinType->SetRect(0, 0, 100, 31, false);
	this->pnlPinType->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPinType, UI::GUILabel(ui, this->pnlPinType, (const UTF8Char*)"Pin Type"));
	this->lblPinType->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboPinType, UI::GUIComboBox(ui, this->pnlPinType, false));
	this->cboPinType->SetRect(104, 4, 200, 23, false);
	this->cboPinType->AddItem(SSWR::AVIRead::AVIRCore::GetIOPinTypeName(SSWR::AVIRead::AVIRCore::IOPT_GPIO).v, (void*)SSWR::AVIRead::AVIRCore::IOPT_GPIO);
	this->cboPinType->AddItem(SSWR::AVIRead::AVIRCore::GetIOPinTypeName(SSWR::AVIRead::AVIRCore::IOPT_VIOPIN).v, (void*)SSWR::AVIRead::AVIRCore::IOPT_VIOPIN);
	this->cboPinType->HandleSelectionChange(OnPinTypeChg, this);

	NEW_CLASS(this->pnlButtons, UI::GUIPanel(ui, this));
	this->pnlButtons->SetRect(0, 0, 100, 31, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this->pnlButtons, (const UTF8Char*)"&OK"));
	this->btnOK->SetRect(49, 4, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlButtons, (const UTF8Char*)"&Cancel"));
	this->btnCancel->SetRect(132, 4, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);

	NEW_CLASS(this->tcConfig, UI::GUITabControl(ui, this));
	this->tcConfig->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpGPIO = this->tcConfig->AddTabPage((const UTF8Char*)"GPIO");
	NEW_CLASS(this->lblGPIO, UI::GUILabel(ui, this->tpGPIO, (const UTF8Char*)"GPIO Pin"));
	this->lblGPIO->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->cboGPIO, UI::GUIComboBox(ui, this->tpGPIO, false));
	this->cboGPIO->SetRect(108, 8, 100, 23, false);

	this->tpVirtualPin = this->tcConfig->AddTabPage((const UTF8Char*)"VirtualPin");
	NEW_CLASS(this->lblVirtualPin, UI::GUILabel(ui, this->tpVirtualPin, (const UTF8Char*)"VirtualPin"));
	this->lblVirtualPin->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->cboVirtualPin, UI::GUIComboBox(ui, this->tpVirtualPin, false));
	this->cboVirtualPin->SetRect(108, 8, 100, 23, false);

	Int32 currPort;
	if (this->gpioPin)
	{
		i = 0;
		j = this->gpioPin->GetPinCount();
		while (i < j)
		{
			currPort = (Int32)i;
			Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("GPIO")), currPort);
			this->cboGPIO->AddItem(sbuff, (void*)(OSInt)currPort);
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
		Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("VirtualIOPin")), currPort);
		this->cboVirtualPin->AddItem(sbuff, (void*)(OSInt)currPort);
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
