#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/BTDevLog.h"
#include "Net/MACInfo.h"
#include "Net/PacketAnalyzerBluetooth.h"
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
	IO::BTDevLog btLog;
	dt.SetCurrTimeUTC();
	Text::StrConcat(Text::StrInt64(sbuff, dt.ToTicks()), (const UTF8Char*)"bt.txt");
	Sync::MutexUsage mutUsage;
	btLog.AppendList(me->bt->GetPublicMap(&mutUsage));
	mutUsage.EndUse();
	btLog.AppendList(me->bt->GetRandomMap(&mutUsage));
	mutUsage.EndUse();
	if (btLog.StoreFile(sbuff))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Stored as "));
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
	IO::BTScanLog::ScanRecord3 *dev = (IO::BTScanLog::ScanRecord3*)me->lvDevices->GetItem(index);
	if (dev)
	{
		Text::StrHexBytes(sbuff, dev->mac, 6, ':');
		Win32::Clipboard::SetString(me->GetHandle(), sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothCtlForm *me = (SSWR::AVIRead::AVIRBluetoothCtlForm*)userObj;
	Sync::MutexUsage mutUsage;
	UOSInt i = me->UpdateList(me->bt->GetPublicMap(&mutUsage), me->pubDevMap, 0);
	me->UpdateList(me->bt->GetRandomMap(&mutUsage), me->randDevMap, i);
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnDeviceUpdated(IO::BTScanLog::ScanRecord3 *dev, IO::BTScanner::UpdateType updateType, void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothCtlForm *me = (SSWR::AVIRead::AVIRBluetoothCtlForm*)userObj;
	Sync::MutexUsage mutUsage(me->devMut);
	if (dev->addrType == IO::BTScanLog::AT_RANDOM)
	{
		if (me->randDevMap->GetIndex(dev->macInt) >= 0)
		{
			me->randDevMap->Put(dev->macInt, 1);
		}
	}
	else
	{
		if (me->pubDevMap->GetIndex(dev->macInt) >= 0)
		{
			me->pubDevMap->Put(dev->macInt, 1);
		}
	}
}

UOSInt SSWR::AVIRead::AVIRBluetoothCtlForm::UpdateList(Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *devMap, Data::UInt64Map<UInt32> *statusMap, UOSInt baseIndex)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char sbuff[32];
	Data::DateTime dt;
	Sync::MutexUsage mutUsage;
	Data::ArrayList<IO::BTScanLog::ScanRecord3*> *devList = devMap->GetValues();
	IO::BTScanLog::ScanRecord3 *dev;

	j = 0;
	k = devList->GetCount();
	while (j < k)
	{
		dev = devList->GetItem(j);
		i = j + baseIndex;
		Sync::MutexUsage devMutUsage(this->devMut);
		if (statusMap->GetIndex(dev->macInt) < 0)
		{
			Text::StrHexBytes(sbuff, dev->mac, 6, ':');
			this->lvDevices->InsertItem(i, sbuff, dev);
			this->lvDevices->SetSubItem(i, 1, IO::BTScanLog::RadioTypeGetName(dev->radioType));
			this->lvDevices->SetSubItem(i, 2, IO::BTScanLog::AddressTypeGetName(dev->addrType));
			if (dev->addrType == IO::BTScanLog::AT_RANDOM)
			{
				switch (dev->mac[0] & 0xC0)
				{
				case 0x00:
					this->lvDevices->SetSubItem(i, 4, (const UTF8Char*)"Non-resolvable Random");
					break;
				case 0x40:
					this->lvDevices->SetSubItem(i, 4, (const UTF8Char*)"Resolvable Random");
					break;
				case 0xC0:
					this->lvDevices->SetSubItem(i, 4, (const UTF8Char*)"Static Random");
					break;
				default:
					this->lvDevices->SetSubItem(i, 4, (const UTF8Char*)"-");
					break;
				}
			}
			else
			{
				this->lvDevices->SetSubItem(i, 4, (const UTF8Char*)Net::MACInfo::GetMACInfo(dev->macInt)->name);
			}
			statusMap->Put(dev->macInt, 1);
		}
		if (statusMap->Get(dev->macInt) != 0)
		{
			if (dev->name)
			{
				this->lvDevices->SetSubItem(i, 3, dev->name);
			}
			Text::StrInt32(sbuff, dev->rssi);
			this->lvDevices->SetSubItem(i, 5, sbuff);
			dt.SetTicks(dev->lastSeenTime);
			dt.ToLocalTime();
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			this->lvDevices->SetSubItem(i, 6, sbuff);
			Text::StrInt32(sbuff, dev->txPower);
			this->lvDevices->SetSubItem(i, 7, sbuff);
			this->lvDevices->SetSubItem(i, 8, (const UTF8Char*)(dev->inRange?"Y":"N"));
			this->lvDevices->SetSubItem(i, 9, (const UTF8Char*)(dev->connected?"Y":"N"));
			if (dev->company == 0)
			{
				this->lvDevices->SetSubItem(i, 10, (const UTF8Char*)"-");
			}
			else
			{
				const UTF8Char *csptr = Net::PacketAnalyzerBluetooth::CompanyGetName(dev->company);
				if (csptr)
				{
					this->lvDevices->SetSubItem(i, 10, csptr);
				}
				else
				{
					this->lvDevices->SetSubItem(i, 10, (const UTF8Char*)"?");
				}
			}
			this->lvDevices->SetSubItem(i, 11, IO::BTScanLog::AdvTypeGetName(dev->advType));
			statusMap->Put(dev->macInt, 0);
		}
		j++;
	}
	return k;
}

SSWR::AVIRead::AVIRBluetoothCtlForm::AVIRBluetoothCtlForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Bluetooth Ctrl");
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->bt = IO::BTScanner::CreateScanner();
	NEW_CLASS(this->devMut, Sync::Mutex());
	NEW_CLASS(this->randDevMap, Data::UInt64Map<UInt32>());
	NEW_CLASS(this->pubDevMap, Data::UInt64Map<UInt32>());

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Scan/Stop"));
	this->btnStart->SetRect(4, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->btnStoreList, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Store Devices"));
	this->btnStoreList->SetRect(84, 4, 100, 23, false);
	this->btnStoreList->HandleButtonClick(OnStoreListClicked, this);
	NEW_CLASS(this->lvDevices, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 12));
	this->lvDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevices->SetShowGrid(true);
	this->lvDevices->SetFullRowSelect(true);
	this->lvDevices->HandleDblClk(OnDevicesDblClick, this);
	this->lvDevices->AddColumn((const UTF8Char*)"MAC", 120);
	this->lvDevices->AddColumn((const UTF8Char*)"Type", 60);
	this->lvDevices->AddColumn((const UTF8Char*)"AddrType", 80);
	this->lvDevices->AddColumn((const UTF8Char*)"Name", 150);
	this->lvDevices->AddColumn((const UTF8Char*)"Vendor", 150);
	this->lvDevices->AddColumn((const UTF8Char*)"RSSI", 60);
	this->lvDevices->AddColumn((const UTF8Char*)"RecvTime", 150);
	this->lvDevices->AddColumn((const UTF8Char*)"TX Power", 60);
	this->lvDevices->AddColumn((const UTF8Char*)"Range", 60);
	this->lvDevices->AddColumn((const UTF8Char*)"Connect", 60);
	this->lvDevices->AddColumn((const UTF8Char*)"Company", 200);
	this->lvDevices->AddColumn((const UTF8Char*)"AdvType", 80);

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
	DEL_CLASS(this->randDevMap);
	DEL_CLASS(this->pubDevMap);
	DEL_CLASS(this->devMut);
}

void SSWR::AVIRead::AVIRBluetoothCtlForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
