#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRBluetoothLEForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"
#include <stdio.h>

void __stdcall SSWR::AVIRead::AVIRBluetoothLEForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothLEForm *me = (SSWR::AVIRead::AVIRBluetoothLEForm*)userObj;
	if (me->btCtrl)
	{
		me->btCtrl->LEScanEnd();
		me->btCtrl = 0;
		me->cboInterface->SetEnabled(true);
		return;
	}
	me->ClearDevices();
	me->lvDevices->ClearItems();

	IO::BTController *btCtrl = (IO::BTController*)me->cboInterface->GetSelectedItem();
	if (btCtrl == 0)
		return;
	btCtrl->LEScanHandleResult(OnLEScanItem, me);
	if (btCtrl->LEScanBegin())
	{
		me->btCtrl = btCtrl;
		me->cboInterface->SetEnabled(false);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in starting LE Scan", (const UTF8Char*)"Bluetooth LE", me);
	}
	
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLEForm::OnStoreListClicked(void *userObj)
{
//	SSWR::AVIRead::AVIRBluetoothLEForm *me = (SSWR::AVIRead::AVIRBluetoothLEForm*)userObj;

}

void __stdcall SSWR::AVIRead::AVIRBluetoothLEForm::OnDevicesDblClick(void *userObj, OSInt index)
{
	SSWR::AVIRead::AVIRBluetoothLEForm *me = (SSWR::AVIRead::AVIRBluetoothLEForm*)userObj;
	UTF8Char sbuff[32];
	UInt8 mac[8];
	BTDevice *dev = (BTDevice*)me->lvDevices->GetItem(index);
	if (dev)
	{
		WriteMInt64(mac, dev->mac);
		Text::StrHexBytes(sbuff, &mac[2], 6, ':');
		Win32::Clipboard::SetString(me->GetHandle(), sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLEForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothLEForm *me = (SSWR::AVIRead::AVIRBluetoothLEForm*)userObj;
	OSInt i;
	OSInt j;
	UInt8 buff[8];
	UTF8Char sbuff[32];
	Data::ArrayList<BTDevice*> *devList;
	BTDevice *dev;

	me->devMut->Lock();
	devList = me->devMap->GetValues();
	i = 0;
	j = devList->GetCount();
	while (i < j)
	{
		dev = devList->GetItem(i);
		if (!dev->shown)
		{
			dev->shown = true;
			dev->updated = false;
			WriteMInt64(buff, dev->mac);
			Text::StrHexBytes(sbuff, &buff[2], 6, ':');
			me->lvDevices->InsertItem(i, sbuff, dev);
			if (dev->name)
			{
				me->lvDevices->SetSubItem(i, 1, dev->name);
			}
			me->lvDevices->SetSubItem(i, 2, (const UTF8Char*)Net::MACInfo::GetMACInfo(dev->mac)->name);
			Text::StrInt32(sbuff, dev->rssi);
			me->lvDevices->SetSubItem(i, 3, sbuff);
		}
		else if (dev->updated)
		{
			dev->updated = false;
			if (dev->name)
			{
				me->lvDevices->SetSubItem(i, 1, dev->name);
			}
			Text::StrInt32(sbuff, dev->rssi);
			me->lvDevices->SetSubItem(i, 3, sbuff);
		}
		i++;
	}
	me->devMut->Unlock();
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLEForm::OnLEScanItem(void *userObj, Int64 mac, Int32 rssi, const Char *name)
{
	SSWR::AVIRead::AVIRBluetoothLEForm *me = (SSWR::AVIRead::AVIRBluetoothLEForm*)userObj;
	BTDevice *dev;
	me->devMut->Lock();
	dev = me->devMap->Get(mac);
	if (dev)
	{
		dev->rssi = rssi;
		if (name)
		{
			if (dev->name == 0)
			{
				dev->name = Text::StrCopyNew((const UTF8Char*)name);
			}
			else if (Text::StrCharCnt(name) > Text::StrCharCnt(dev->name))
			{
				Text::StrDelNew(dev->name);
				dev->name = Text::StrCopyNew((const UTF8Char*)name);
			}
		}
		dev->updated = true;
	}
	else
	{
		dev = MemAlloc(BTDevice, 1);
		dev->mac = mac;
		dev->rssi = rssi;
		if (name)
		{
			dev->name = Text::StrCopyNew((const UTF8Char*)name);
		}
		else
		{
			dev->name = 0;
		}
		dev->shown = false;
		dev->updated = true;	
		me->devMap->Put(mac, dev);
	}
	me->devMut->Unlock();
}

void SSWR::AVIRead::AVIRBluetoothLEForm::ClearDevices()
{
	OSInt i;
	Data::ArrayList<BTDevice*> *devList = this->devMap->GetValues();
	BTDevice *dev;
	i = devList->GetCount();
	while (i-- > 0)
	{
		dev = devList->GetItem(i);
		SDEL_TEXT(dev->name);
		MemFree(dev);
	}
	this->devMap->Clear();
}

SSWR::AVIRead::AVIRBluetoothLEForm::AVIRBluetoothLEForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Bluetooth LE");
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->btMgr, IO::BTManager());
	NEW_CLASS(this->btList, Data::ArrayList<IO::BTController*>());
	this->btMgr->CreateControllers(this->btList);
	this->btCtrl = 0;
	NEW_CLASS(this->devMut, Sync::Mutex());
	NEW_CLASS(this->devMap, Data::Integer64Map<BTDevice*>());

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblInterface, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Interface"));
	this->lblInterface->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboInterface, UI::GUIComboBox(ui, this->pnlControl, false));
	this->cboInterface->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Scan/Stop"));
	this->btnStart->SetRect(254, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->btnStoreList, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Store Devices"));
	this->btnStoreList->SetRect(404, 4, 100, 23, false);
	this->btnStoreList->HandleButtonClick(OnStoreListClicked, this);
	NEW_CLASS(this->lvDevices, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevices->SetShowGrid(true);
	this->lvDevices->SetFullRowSelect(true);
	this->lvDevices->HandleDblClk(OnDevicesDblClick, this);
	this->lvDevices->AddColumn((const UTF8Char*)"MAC", 80);
	this->lvDevices->AddColumn((const UTF8Char*)"Name", 150);
	this->lvDevices->AddColumn((const UTF8Char*)"Vendor", 150);
	this->lvDevices->AddColumn((const UTF8Char*)"RSSI", 60);

	OSInt i = 0;
	OSInt j = this->btList->GetCount();
	IO::BTController *btCtrl;
	while (i < j)
	{
		btCtrl = this->btList->GetItem(i);
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
	OSInt i;
	IO::BTController *btCtrl;
	if (this->btCtrl)
	{
		this->btCtrl->LEScanEnd();
		this->btCtrl = 0;
	}

	i = this->btList->GetCount();
	while (i-- > 0)
	{
		btCtrl = this->btList->GetItem(i);
		DEL_CLASS(btCtrl);
	}
	DEL_CLASS(this->btMgr);
	DEL_CLASS(this->btList);

	this->ClearDevices();
	DEL_CLASS(this->devMap);
	DEL_CLASS(this->devMut);
}

void SSWR::AVIRead::AVIRBluetoothLEForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
