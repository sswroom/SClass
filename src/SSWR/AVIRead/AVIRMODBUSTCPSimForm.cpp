#include "Stdafx.h"
#include "IO/ED516Sim.h"
#include "IO/ED527Sim.h"
#include "IO/ED538Sim.h"
#include "IO/ED588Sim.h"
#include "IO/PrintMODBUSDevSim.h"
#include "SSWR/AVIRead/AVIRMODBUSTCPSimForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRMODBUSTCPSimForm::OnListenClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSTCPSimForm *me = (SSWR::AVIRead::AVIRMODBUSTCPSimForm*)userObj;
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
		me->txtPort->GetText(&sb);
		if (!sb.ToUInt16(&port))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing port number"), CSTR("MODBUS TCP Simulator"), me);
			return;
		}
		NEW_CLASS(me->listener, Net::MODBUSTCPListener(me->core->GetSocketFactory(), port, &me->log));
		if (me->listener->IsError())
		{
			DEL_CLASS(me->listener);
			me->listener = 0;
			UI::MessageDialog::ShowDialog(CSTR("Error in listening to the port"), CSTR("MODBUS TCP Simulator"), me);
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

void __stdcall SSWR::AVIRead::AVIRMODBUSTCPSimForm::OnDevAddClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSTCPSimForm *me = (SSWR::AVIRead::AVIRMODBUSTCPSimForm*)userObj;
	DeviceType devType = (DeviceType)(OSInt)me->cboDevType->GetSelectedItem();
	Text::StringBuilderUTF8 sb;
	UInt8 addr;
	if (me->listener == 0)
		return;
	me->txtAddr->GetText(&sb);
	if (sb.ToUInt8(&addr))
	{
		IO::MODBUSDevSim *dev;
		switch (devType)
		{
		case DT_PRINT:
			NEW_CLASS(dev, IO::PrintMODBUSDevSim());
			me->listener->AddDevice(addr, dev);
			me->UpdateDevList();
			break;
		case DT_ED516:
			NEW_CLASS(dev, IO::ED516Sim());
			me->listener->AddDevice(addr, dev);
			me->UpdateDevList();
			break;
		case DT_ED527:
			NEW_CLASS(dev, IO::ED527Sim());
			me->listener->AddDevice(addr, dev);
			me->UpdateDevList();
			break;
		case DT_ED538:
			NEW_CLASS(dev, IO::ED538Sim());
			me->listener->AddDevice(addr, dev);
			me->UpdateDevList();
			break;
		case DT_ED588:
			NEW_CLASS(dev, IO::ED588Sim());
			me->listener->AddDevice(addr, dev);
			me->UpdateDevList();
			break;
		}
	}
	else
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in parsing address"), CSTR("MODBUS TCP Simulator"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSTCPSimForm::OnDeviceChanged(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSTCPSimForm *me = (SSWR::AVIRead::AVIRMODBUSTCPSimForm*)userObj;
	me->currDev = (IO::MODBUSDevSim*)me->lbDevice->GetSelectedItem();
	me->lvDeviceValues->ClearItems();
	if (me->currDev)
	{
		Text::StringBuilderUTF8 sb;
		UOSInt i = 0;
		UOSInt j = me->currDev->GetValueCount();
		while (i < j)
		{
			me->lvDeviceValues->AddItem(me->currDev->GetValueName(i), 0);
			sb.ClearStr();
			me->currDev->GetValue(i, &sb);
			me->lvDeviceValues->SetSubItem(i, 1, sb.ToCString());
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSTCPSimForm::OnDeviceValuesDblClk(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRMODBUSTCPSimForm *me = (SSWR::AVIRead::AVIRMODBUSTCPSimForm*)userObj;
	if (me->currDev && index != INVALID_INDEX)
	{
		me->currDev->ToggleValue(index);	
		me->UpdateDevValues();
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSTCPSimForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSTCPSimForm *me = (SSWR::AVIRead::AVIRMODBUSTCPSimForm*)userObj;
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
			IO::MODBUSDevSim *dev = this->listener->GetDevice(i);
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
			this->currDev->GetValue(i, &sb);
			this->lvDeviceValues->SetSubItem(i, 1, sb.ToCString());
			i++;
		}
	}
}

SSWR::AVIRead::AVIRMODBUSTCPSimForm::AVIRMODBUSTCPSimForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 576, 480, ui)
{
	this->SetText(CSTR("MODBUS TCP Simulator"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->listener = 0;
	this->currDev = 0;

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 96, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlCtrl, CSTR("Port")));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlCtrl, CSTR("1234")));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->btnListen, UI::GUIButton(ui, this->pnlCtrl, CSTR("Listen")));
	this->btnListen->SetRect(204, 4, 75, 23, false);
	this->btnListen->HandleButtonClick(OnListenClicked, this);
	NEW_CLASS(this->grpDev, UI::GUIGroupBox(ui, this->pnlCtrl, CSTR("Device")));
	this->grpDev->SetRect(0, 28, 400, 64, false);
	NEW_CLASS(this->lblAddr, UI::GUILabel(ui, this->grpDev, CSTR("Address")));
	this->lblAddr->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtAddr, UI::GUITextBox(ui, this->grpDev, CSTR("1")));
	this->txtAddr->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->cboDevType, UI::GUIComboBox(ui, this->grpDev, false));
	this->cboDevType->SetRect(204, 4, 100, 23, false);
	NEW_CLASS(this->btnDev, UI::GUIButton(ui, this->grpDev, CSTR("Add")));
	this->btnDev->SetRect(304, 4, 75, 23, false);
	this->btnDev->HandleButtonClick(OnDevAddClicked, this);
	NEW_CLASS(this->lbDevice, UI::GUIListBox(ui, this, false));
	this->lbDevice->SetRect(0, 0, 100, 23, false);
	this->lbDevice->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevice->HandleSelectionChange(OnDeviceChanged, this);
	NEW_CLASS(this->lvDeviceValues, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvDeviceValues->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDeviceValues->SetFullRowSelect(true);
	this->lvDeviceValues->SetShowGrid(true);
	this->lvDeviceValues->AddColumn(CSTR("Name"), 150);
	this->lvDeviceValues->AddColumn(CSTR("Value"), 300);
	this->lvDeviceValues->HandleDblClk(OnDeviceValuesDblClk, this);

	DeviceType devType = DT_FIRST;
	while (devType <= DT_LAST)
	{
		this->cboDevType->AddItem(DeviceTypeGetName(devType), (void*)(OSInt)devType);
		devType = (DeviceType)(devType + 1);
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

Text::CString SSWR::AVIRead::AVIRMODBUSTCPSimForm::DeviceTypeGetName(DeviceType devType)
{
	switch (devType)
	{
	case DT_PRINT:
		return CSTR("Debug Print");
	case DT_ED538:
		return CSTR("ED538");
	case DT_ED588:
		return CSTR("ED588");
	case DT_ED516:
		return CSTR("ED516");
	case DT_ED527:
		return CSTR("ED527");
	default:
		return CSTR("Unknown");
	}
}