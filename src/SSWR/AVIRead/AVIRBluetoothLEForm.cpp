#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRBluetoothLEForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/Clipboard.h"
#include <stdio.h>

void __stdcall SSWR::AVIRead::AVIRBluetoothLEForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothLEForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothLEForm>();
	NN<IO::BTController> btCtrl;
	if (me->btCtrl.SetTo(btCtrl))
	{
		btCtrl->LEScanEnd();
		me->btCtrl = 0;
		me->cboInterface->SetEnabled(true);
		return;
	}
	me->ClearDevices();
	me->lvDevices->ClearItems();

	if (!me->cboInterface->GetSelectedItem().GetOpt<IO::BTController>().SetTo(btCtrl))
		return;
	btCtrl->LEScanHandleResult(OnLEScanItem, me);
	if (btCtrl->LEScanBegin())
	{
		me->btCtrl = btCtrl;
		me->cboInterface->SetEnabled(false);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in starting LE Scan"), CSTR("Bluetooth LE"), me);
	}
	
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLEForm::OnStoreListClicked(AnyType userObj)
{
//	SSWR::AVIRead::AVIRBluetoothLEForm *me = (SSWR::AVIRead::AVIRBluetoothLEForm*)userObj;

}

void __stdcall SSWR::AVIRead::AVIRBluetoothLEForm::OnDevicesDblClick(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRBluetoothLEForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothLEForm>();
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UInt8 mac[8];
	NN<BTDevice> dev;
	if (me->lvDevices->GetItem(index).GetOpt<BTDevice>().SetTo(dev))
	{
		WriteMUInt64(mac, dev->mac);
		sptr = Text::StrHexBytes(sbuff, &mac[2], 6, ':');
		UI::Clipboard::SetString(me->GetHandle(), CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLEForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothLEForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothLEForm>();
	UOSInt i;
	UOSInt j;
	UInt8 buff[8];
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NN<BTDevice> dev;
	NN<Text::String> s;

	Sync::MutexUsage mutUsage(me->devMut);
	i = 0;
	j = me->devMap.GetCount();
	while (i < j)
	{
		dev = me->devMap.GetItemNoCheck(i);
		if (!dev->shown)
		{
			dev->shown = true;
			dev->updated = false;
			WriteMUInt64(buff, dev->mac);
			sptr = Text::StrHexBytes(sbuff, &buff[2], 6, ':');
			me->lvDevices->InsertItem(i, CSTRP(sbuff, sptr), dev);
			if (s.Set(dev->name))
			{
				me->lvDevices->SetSubItem(i, 1, s);
			}
			const Net::MACInfo::MACEntry *mac = Net::MACInfo::GetMACInfo(dev->mac);
			me->lvDevices->SetSubItem(i, 2, {mac->name, mac->nameLen});
			sptr = Text::StrInt32(sbuff, dev->rssi);
			me->lvDevices->SetSubItem(i, 3, CSTRP(sbuff, sptr));
		}
		else if (dev->updated)
		{
			dev->updated = false;
			if (s.Set(dev->name))
			{
				me->lvDevices->SetSubItem(i, 1, s);
			}
			sptr = Text::StrInt32(sbuff, dev->rssi);
			me->lvDevices->SetSubItem(i, 3, CSTRP(sbuff, sptr));
		}
		i++;
	}
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLEForm::OnLEScanItem(AnyType userObj, UInt64 mac, Int32 rssi, Text::CString name)
{
	NN<SSWR::AVIRead::AVIRBluetoothLEForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothLEForm>();
	NN<BTDevice> dev;
	Sync::MutexUsage mutUsage(me->devMut);
	if (me->devMap.Get(mac).SetTo(dev))
	{
		dev->rssi = rssi;
		if (name.leng > 0)
		{
			if (dev->name == 0)
			{
				dev->name = Text::String::New(name).Ptr();
			}
			else if (name.leng > dev->name->leng)
			{
				dev->name->Release();
				dev->name = Text::String::New(name).Ptr();
			}
		}
		dev->updated = true;
	}
	else
	{
		dev = MemAllocNN(BTDevice);
		dev->mac = mac;
		dev->rssi = rssi;
		if (name.leng)
		{
			dev->name = Text::String::New(name).Ptr();
		}
		else
		{
			dev->name = 0;
		}
		dev->shown = false;
		dev->updated = true;	
		me->devMap.Put(mac, dev);
	}
}

void SSWR::AVIRead::AVIRBluetoothLEForm::ClearDevices()
{
	UOSInt i;
	NN<BTDevice> dev;
	i = this->devMap.GetCount();
	while (i-- > 0)
	{
		dev = this->devMap.GetItemNoCheck(i);
		SDEL_STRING(dev->name);
		MemFreeNN(dev);
	}
	this->devMap.Clear();
}

SSWR::AVIRead::AVIRBluetoothLEForm::AVIRBluetoothLEForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Bluetooth LE"));
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->btMgr.CreateControllers(this->btList);
	this->btCtrl = 0;

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblInterface = ui->NewLabel(this->pnlControl, CSTR("Interface"));
	this->lblInterface->SetRect(4, 4, 100, 23, false);
	this->cboInterface = ui->NewComboBox(this->pnlControl, false);
	this->cboInterface->SetRect(104, 4, 150, 23, false);
	this->btnStart = ui->NewButton(this->pnlControl, CSTR("Scan/Stop"));
	this->btnStart->SetRect(254, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->btnStoreList = ui->NewButton(this->pnlControl, CSTR("Store Devices"));
	this->btnStoreList->SetRect(404, 4, 100, 23, false);
	this->btnStoreList->HandleButtonClick(OnStoreListClicked, this);
	this->lvDevices = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
	this->lvDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevices->SetShowGrid(true);
	this->lvDevices->SetFullRowSelect(true);
	this->lvDevices->HandleDblClk(OnDevicesDblClick, this);
	this->lvDevices->AddColumn(CSTR("MAC"), 80);
	this->lvDevices->AddColumn(CSTR("Name"), 150);
	this->lvDevices->AddColumn(CSTR("Vendor"), 150);
	this->lvDevices->AddColumn(CSTR("RSSI"), 60);

	UOSInt i = 0;
	UOSInt j = this->btList.GetCount();
	NN<IO::BTController> btCtrl;
	while (i < j)
	{
		btCtrl = this->btList.GetItemNoCheck(i);
		this->cboInterface->AddItem(btCtrl->GetName(), btCtrl);
		i++;
	}
	if (j > 0)
	{
		this->cboInterface->SetSelectedIndex(0);
	}
	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRBluetoothLEForm::~AVIRBluetoothLEForm()
{
	UOSInt i;
	NN<IO::BTController> btCtrl;
	if (this->btCtrl.SetTo(btCtrl))
	{
		btCtrl->LEScanEnd();
		this->btCtrl = 0;
	}

	i = this->btList.GetCount();
	while (i-- > 0)
	{
		btCtrl = this->btList.GetItemNoCheck(i);
		btCtrl.Delete();
	}

	this->ClearDevices();
}

void SSWR::AVIRead::AVIRBluetoothLEForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
