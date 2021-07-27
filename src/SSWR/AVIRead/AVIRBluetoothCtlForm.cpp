#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/BTLog.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRBluetoothCtlForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"
#include <stdio.h>

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothCtlForm *me = (SSWR::AVIRead::AVIRBluetoothCtlForm*)userObj;
	if (me->bt)
	{
		if (me->bt->IsScanOn())
		{
			me->bt->ScanOff();
			return;
		}

		me->bt->ScanOn();
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnStoreListClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothCtlForm *me = (SSWR::AVIRead::AVIRBluetoothCtlForm*)userObj;
	UTF8Char sbuff[128];
	Data::DateTime dt;
	IO::BTLog btLog;
	dt.SetCurrTimeUTC();
	Text::StrConcat(Text::StrInt64(sbuff, dt.ToTicks()), (const UTF8Char*)"bt.txt");
	Sync::MutexUsage mutUsage;
	btLog.AppendList(me->bt->GetRecordMap(&mutUsage));
	mutUsage.EndUse();
	if (btLog.StoreFile(sbuff))
	{
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"Stored as ");
		sb.Append(sbuff);
		UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"Bluetooth Ctrl", me);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in storing file", (const UTF8Char*)"Bluetooth Ctrl", me);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnDevicesDblClick(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRBluetoothCtlForm *me = (SSWR::AVIRead::AVIRBluetoothCtlForm*)userObj;
	UTF8Char sbuff[32];
	IO::BTScanner::ScanRecord *dev = (IO::BTScanner::ScanRecord*)me->lvDevices->GetItem(index);
	if (dev)
	{
		Text::StrHexBytes(sbuff, dev->mac, 6, ':');
		Win32::Clipboard::SetString(me->GetHandle(), sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothCtlForm *me = (SSWR::AVIRead::AVIRBluetoothCtlForm*)userObj;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UTF8Char sbuff[32];
	Sync::MutexUsage mutUsage;
	Data::UInt64Map<IO::BTScanner::ScanRecord*> *devMap = me->bt->GetRecordMap(&mutUsage);
	Data::ArrayList<IO::BTScanner::ScanRecord*> *devList = devMap->GetValues();
	IO::BTScanner::ScanRecord *dev;
	Text::StringBuilderUTF8 sb;

	i = 0;
	j = devList->GetCount();
	while (i < j)
	{
		dev = devList->GetItem(i);
		Sync::MutexUsage devMutUsage(me->devMut);
		if (me->devMap->GetIndex(dev->macInt) < 0)
		{
			Text::StrHexBytes(sbuff, dev->mac, 6, ':');
			me->lvDevices->InsertItem(i, sbuff, dev);
			me->lvDevices->SetSubItem(i, 2, (const UTF8Char*)Net::MACInfo::GetMACInfo(dev->macInt)->name);
			me->devMap->Put(dev->macInt, 1);
		}
		if (me->devMap->Get(dev->macInt) != 0)
		{
			if (dev->name)
			{
				me->lvDevices->SetSubItem(i, 1, dev->name);
			}
			Text::StrInt32(sbuff, dev->rssi);
			me->lvDevices->SetSubItem(i, 3, sbuff);
			Text::StrInt32(sbuff, dev->txPower);
			me->lvDevices->SetSubItem(i, 4, sbuff);
			me->lvDevices->SetSubItem(i, 5, (const UTF8Char*)(dev->inRange?"Y":"N"));
			me->lvDevices->SetSubItem(i, 6, (const UTF8Char*)(dev->connected?"Y":"N"));
			k = 0;
			l = dev->keys->GetCount();
			if (l > 0)
			{
				sb.ClearStr();
				while (k < l)
				{
					if (k > 0)
					{
						sb.Append((const UTF8Char*)", ");
					}
					sb.AppendHex16((UInt16)dev->keys->GetItem(k));
					k++;
				}
				me->lvDevices->SetSubItem(i, 7, sb.ToString());
			}
			me->devMap->Put(dev->macInt, 0);
		}
		i++;
	}
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnDeviceUpdated(IO::BTScanner::ScanRecord *dev, IO::BTScanner::UpdateType updateType, void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothCtlForm *me = (SSWR::AVIRead::AVIRBluetoothCtlForm*)userObj;
	Sync::MutexUsage mutUsage(me->devMut);
	if (me->devMap->GetIndex(dev->macInt) >= 0)
	{
		me->devMap->Put(dev->macInt, 1);
	}
}

SSWR::AVIRead::AVIRBluetoothCtlForm::AVIRBluetoothCtlForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Bluetooth Ctrl");
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->bt = IO::BTScanner::CreateScanner();
	NEW_CLASS(this->devMut, Sync::Mutex());
	NEW_CLASS(this->devMap, Data::UInt64Map<UInt32>());

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Scan/Stop"));
	this->btnStart->SetRect(4, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->btnStoreList, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Store Devices"));
	this->btnStoreList->SetRect(84, 4, 100, 23, false);
	this->btnStoreList->HandleButtonClick(OnStoreListClicked, this);
	NEW_CLASS(this->lvDevices, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 8));
	this->lvDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevices->SetShowGrid(true);
	this->lvDevices->SetFullRowSelect(true);
	this->lvDevices->HandleDblClk(OnDevicesDblClick, this);
	this->lvDevices->AddColumn((const UTF8Char*)"MAC", 120);
	this->lvDevices->AddColumn((const UTF8Char*)"Name", 150);
	this->lvDevices->AddColumn((const UTF8Char*)"Vendor", 150);
	this->lvDevices->AddColumn((const UTF8Char*)"RSSI", 60);
	this->lvDevices->AddColumn((const UTF8Char*)"TX Power", 60);
	this->lvDevices->AddColumn((const UTF8Char*)"Range", 60);
	this->lvDevices->AddColumn((const UTF8Char*)"Connect", 60);
	this->lvDevices->AddColumn((const UTF8Char*)"Keys", 200);

	if (this->bt)
	{
		this->bt->HandleRecordUpdate(OnDeviceUpdated, this);
		this->bt->ScanOn();
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in starting bluetoothctl", (const UTF8Char*)"Bluetooth Ctrl", this);
	}
	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRBluetoothCtlForm::~AVIRBluetoothCtlForm()
{
	if (this->bt)
	{
		this->bt->ScanOff();

		DEL_CLASS(this->bt);
	}
	DEL_CLASS(this->devMap);
	DEL_CLASS(this->devMut);
}

void SSWR::AVIRead::AVIRBluetoothCtlForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
