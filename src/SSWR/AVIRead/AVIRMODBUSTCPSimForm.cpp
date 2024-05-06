#include "Stdafx.h"
#include "IO/ED516Sim.h"
#include "IO/ED527Sim.h"
#include "IO/ED538Sim.h"
#include "IO/ED588Sim.h"
#include "IO/PrintMODBUSDevSim.h"
#include "SSWR/AVIRead/AVIRMODBUSTCPSimForm.h"

void __stdcall SSWR::AVIRead::AVIRMODBUSTCPSimForm::OnListenClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMODBUSTCPSimForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMODBUSTCPSimForm>();
	if (me->listener)
	{
		DEL_CLASS(me->listener);
		me->listener = 0;
		me->btnListen->SetText(CSTR("Listen"));
		me->txtPort->SetReadOnly(false);
		me->lbDevice->ClearItems();
		me->currDev = 0;
	}
	else
	{
		UInt16 port;
		Text::StringBuilderUTF8 sb;
		me->txtPort->GetText(sb);
		if (!sb.ToUInt16(port))
		{
			me->ui->ShowMsgOK(CSTR("Error in parsing port number"), CSTR("MODBUS TCP Simulator"), me);
			return;
		}
		NEW_CLASS(me->listener, Net::MODBUSTCPListener(me->core->GetSocketFactory(), port, me->log, true));
		if (me->listener->IsError())
		{
			DEL_CLASS(me->listener);
			me->listener = 0;
			me->ui->ShowMsgOK(CSTR("Error in listening to the port"), CSTR("MODBUS TCP Simulator"), me);
			return;
		}
		else
		{
			me->btnListen->SetText(CSTR("Stop"));
			me->txtPort->SetReadOnly(true);
			me->UpdateDevList();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSTCPSimForm::OnDevAddClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMODBUSTCPSimForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMODBUSTCPSimForm>();
	DeviceType devType = (DeviceType)me->cboDevType->GetSelectedItem().GetOSInt();
	Text::StringBuilderUTF8 sb;
	UInt8 addr;
	if (me->listener == 0)
		return;
	me->txtAddr->GetText(sb);
	if (sb.ToUInt8(addr))
	{
		NN<IO::MODBUSDevSim> dev;
		switch (devType)
		{
		case DeviceType::Print:
			NEW_CLASSNN(dev, IO::PrintMODBUSDevSim());
			me->listener->AddDevice(addr, dev);
			me->UpdateDevList();
			break;
		case DeviceType::ED516:
			NEW_CLASSNN(dev, IO::ED516Sim());
			me->listener->AddDevice(addr, dev);
			me->UpdateDevList();
			break;
		case DeviceType::ED527:
			NEW_CLASSNN(dev, IO::ED527Sim());
			me->listener->AddDevice(addr, dev);
			me->UpdateDevList();
			break;
		case DeviceType::ED538:
			NEW_CLASSNN(dev, IO::ED538Sim());
			me->listener->AddDevice(addr, dev);
			me->UpdateDevList();
			break;
		case DeviceType::ED588:
			NEW_CLASSNN(dev, IO::ED588Sim());
			me->listener->AddDevice(addr, dev);
			me->UpdateDevList();
			break;
		}
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in parsing address"), CSTR("MODBUS TCP Simulator"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSTCPSimForm::OnDelayClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMODBUSTCPSimForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMODBUSTCPSimForm>();
	if (me->listener)
	{
		Text::StringBuilderUTF8 sb;
		UInt32 delay;
		me->txtDelay->GetText(sb);
		if (sb.ToUInt32(delay))
		{
			me->listener->SetDelay(delay);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSTCPSimForm::OnDeviceChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMODBUSTCPSimForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMODBUSTCPSimForm>();
	me->currDev = (IO::MODBUSDevSim*)me->lbDevice->GetSelectedItem().p;
	me->lvDeviceValues->ClearItems();
	if (me->currDev)
	{
		Text::StringBuilderUTF8 sb;
		UOSInt i = 0;
		UOSInt j = me->currDev->GetValueCount();
		while (i < j)
		{
			me->lvDeviceValues->AddItem(me->currDev->GetValueName(i).OrEmpty(), 0);
			sb.ClearStr();
			me->currDev->GetValue(i, sb);
			me->lvDeviceValues->SetSubItem(i, 1, sb.ToCString());
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSTCPSimForm::OnDeviceValuesDblClk(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRMODBUSTCPSimForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMODBUSTCPSimForm>();
	if (me->currDev && index != INVALID_INDEX)
	{
		me->currDev->ToggleValue(index);	
		me->UpdateDevValues();
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSTCPSimForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMODBUSTCPSimForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMODBUSTCPSimForm>();
	me->UpdateDevValues();
}

void SSWR::AVIRead::AVIRMODBUSTCPSimForm::UpdateDevList()
{
	this->lbDevice->ClearItems();
	if (this->listener)
	{
		Text::StringBuilderUTF8 sb;
		UOSInt i = 0;
		UOSInt j = this->listener->GetDeviceCount();
		while (i < j)
		{
			NN<IO::MODBUSDevSim> dev = this->listener->GetDeviceNoCheck(i);
			sb.ClearStr();
			sb.AppendU32(this->listener->GetDeviceAddr(i));
			sb.AppendC(UTF8STRC(", "));
			sb.Append(dev->GetName());
			this->lbDevice->AddItem(sb.ToCString(), dev);

			i++;
		}
	}
	this->currDev = 0;
	this->lvDeviceValues->ClearItems();
}

void SSWR::AVIRead::AVIRMODBUSTCPSimForm::UpdateDevValues()
{
	if (this->currDev)
	{
		Text::StringBuilderUTF8 sb;
		UOSInt i = 0;
		UOSInt j = this->currDev->GetValueCount();
		while (i < j)
		{
			sb.ClearStr();
			this->currDev->GetValue(i, sb);
			this->lvDeviceValues->SetSubItem(i, 1, sb.ToCString());
			i++;
		}
	}
}

SSWR::AVIRead::AVIRMODBUSTCPSimForm::AVIRMODBUSTCPSimForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 576, 480, ui)
{
	this->SetText(CSTR("MODBUS TCP Simulator"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->listener = 0;
	this->currDev = 0;

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 120, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPort = ui->NewLabel(this->pnlCtrl, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlCtrl, CSTR("502"));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	this->btnListen = ui->NewButton(this->pnlCtrl, CSTR("Listen"));
	this->btnListen->SetRect(204, 4, 75, 23, false);
	this->btnListen->HandleButtonClick(OnListenClicked, this);
	this->grpDev = ui->NewGroupBox(this->pnlCtrl, CSTR("Device"));
	this->grpDev->SetRect(0, 28, 400, 64, false);
	this->lblAddr = ui->NewLabel(this->grpDev, CSTR("Address"));
	this->lblAddr->SetRect(4, 4, 100, 23, false);
	this->txtAddr = ui->NewTextBox(this->grpDev, CSTR("1"));
	this->txtAddr->SetRect(104, 4, 100, 23, false);
	this->cboDevType = ui->NewComboBox(this->grpDev, false);
	this->cboDevType->SetRect(204, 4, 100, 23, false);
	this->btnDev = ui->NewButton(this->grpDev, CSTR("Add"));
	this->btnDev->SetRect(304, 4, 75, 23, false);
	this->btnDev->HandleButtonClick(OnDevAddClicked, this);
	this->lblDelay = ui->NewLabel(this->pnlCtrl, CSTR("Delay"));
	this->lblDelay->SetRect(4, 96, 100, 23, false);
	this->txtDelay = ui->NewTextBox(this->pnlCtrl, CSTR("0"));
	this->txtDelay->SetRect(104, 96, 100, 23, false);
	this->btnDelay = ui->NewButton(this->pnlCtrl, CSTR("Set"));
	this->btnDelay->SetRect(204, 96, 75, 23, false);
	this->btnDelay->HandleButtonClick(OnDelayClicked, this);
	this->lbDevice = ui->NewListBox(*this, false);
	this->lbDevice->SetRect(0, 0, 100, 23, false);
	this->lbDevice->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevice->HandleSelectionChange(OnDeviceChanged, this);
	this->lvDeviceValues = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvDeviceValues->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDeviceValues->SetFullRowSelect(true);
	this->lvDeviceValues->SetShowGrid(true);
	this->lvDeviceValues->AddColumn(CSTR("Name"), 150);
	this->lvDeviceValues->AddColumn(CSTR("Value"), 300);
	this->lvDeviceValues->HandleDblClk(OnDeviceValuesDblClk, this);

	DeviceType devType = DeviceType::First;
	while (devType <= DeviceType::Last)
	{
		this->cboDevType->AddItem(DeviceTypeGetName(devType), (void*)(OSInt)devType);
		devType = (DeviceType)((OSInt)devType + 1);
	}
	this->cboDevType->SetSelectedIndex(0);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRMODBUSTCPSimForm::~AVIRMODBUSTCPSimForm()
{
	SDEL_CLASS(this->listener);
}

void SSWR::AVIRead::AVIRMODBUSTCPSimForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Text::CStringNN SSWR::AVIRead::AVIRMODBUSTCPSimForm::DeviceTypeGetName(DeviceType devType)
{
	switch (devType)
	{
	case DeviceType::Print:
		return CSTR("Debug Print");
	case DeviceType::ED538:
		return CSTR("ED538");
	case DeviceType::ED588:
		return CSTR("ED588");
	case DeviceType::ED516:
		return CSTR("ED516");
	case DeviceType::ED527:
		return CSTR("ED527");
	default:
		return CSTR("Unknown");
	}
}
