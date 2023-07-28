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
#include "UI/Clipboard.h"
#include "UI/MessageDialog.h"
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
	UTF8Char *sptr;
	Data::DateTime dt;
	IO::BTDevLog btLog;
	dt.SetCurrTimeUTC();
	sptr = Text::StrConcatC(Text::StrInt64(sbuff, dt.ToTicks()), UTF8STRC("bt.txt"));
	Sync::MutexUsage mutUsage;
	btLog.AppendList(me->bt->GetPublicMap(mutUsage));
	mutUsage.EndUse();
	btLog.AppendList(me->bt->GetRandomMap(mutUsage));
	mutUsage.EndUse();
	if (btLog.StoreFile(CSTRP(sbuff, sptr)))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Stored as "));
		sb.AppendP(sbuff, sptr);
		UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Bluetooth Ctrl"), me);
	}
	else
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in storing file"), CSTR("Bluetooth Ctrl"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnDevicesDblClick(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRBluetoothCtlForm *me = (SSWR::AVIRead::AVIRBluetoothCtlForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	IO::BTScanLog::ScanRecord3 *dev = (IO::BTScanLog::ScanRecord3*)me->lvDevices->GetItem(index);
	if (dev)
	{
		sptr = Text::StrHexBytes(sbuff, dev->mac, 6, ':');
		UI::Clipboard::SetString(me->GetHandle(), CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothCtlForm *me = (SSWR::AVIRead::AVIRBluetoothCtlForm*)userObj;
	Sync::MutexUsage mutUsage;
	UOSInt i = me->UpdateList(me->bt->GetPublicMap(mutUsage), &me->pubDevMap, 0);
	me->UpdateList(me->bt->GetRandomMap(mutUsage), &me->randDevMap, i);
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnDeviceUpdated(IO::BTScanLog::ScanRecord3 *dev, IO::BTScanner::UpdateType updateType, void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothCtlForm *me = (SSWR::AVIRead::AVIRBluetoothCtlForm*)userObj;
	Sync::MutexUsage mutUsage(me->devMut);
	if (dev->addrType == IO::BTScanLog::AT_RANDOM)
	{
		if (me->randDevMap.GetIndex(dev->macInt) >= 0)
		{
			me->randDevMap.Put(dev->macInt, 1);
		}
	}
	else
	{
		if (me->pubDevMap.GetIndex(dev->macInt) >= 0)
		{
			me->pubDevMap.Put(dev->macInt, 1);
		}
	}
}

UOSInt SSWR::AVIRead::AVIRBluetoothCtlForm::UpdateList(NotNullPtr<Data::FastMap<UInt64, IO::BTScanLog::ScanRecord3*>> devMap, Data::FastMap<UInt64, UInt32> *statusMap, UOSInt baseIndex)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Data::DateTime dt;
	NotNullPtr<Text::String> s;
	Sync::MutexUsage mutUsage;
	IO::BTScanLog::ScanRecord3 *dev;

	j = 0;
	k = devMap->GetCount();
	while (j < k)
	{
		dev = devMap->GetItem(j);
		i = j + baseIndex;
		Sync::MutexUsage devMutUsage(this->devMut);
		if (statusMap->GetIndex(dev->macInt) < 0)
		{
			sptr = Text::StrHexBytes(sbuff, dev->mac, 6, ':');
			this->lvDevices->InsertItem(i, CSTRP(sbuff, sptr), dev);
			this->lvDevices->SetSubItem(i, 1, IO::BTScanLog::RadioTypeGetName(dev->radioType));
			this->lvDevices->SetSubItem(i, 2, IO::BTScanLog::AddressTypeGetName(dev->addrType));
			if (dev->addrType == IO::BTScanLog::AT_RANDOM)
			{
				switch (dev->mac[0] & 0xC0)
				{
				case 0x00:
					this->lvDevices->SetSubItem(i, 4, CSTR("Non-resolvable Random"));
					break;
				case 0x40:
					this->lvDevices->SetSubItem(i, 4, CSTR("Resolvable Random"));
					break;
				case 0xC0:
					this->lvDevices->SetSubItem(i, 4, CSTR("Static Random"));
					break;
				default:
					this->lvDevices->SetSubItem(i, 4, CSTR("-"));
					break;
				}
			}
			else
			{
				const Net::MACInfo::MACEntry *mac = Net::MACInfo::GetMACInfo(dev->macInt);
				this->lvDevices->SetSubItem(i, 4, {mac->name, mac->nameLen});
			}
			statusMap->Put(dev->macInt, 1);
		}
		if (statusMap->Get(dev->macInt) != 0)
		{
			if (s.Set(dev->name))
			{
				this->lvDevices->SetSubItem(i, 3, s);
			}
			sptr = Text::StrInt32(sbuff, dev->rssi);
			this->lvDevices->SetSubItem(i, 5, CSTRP(sbuff, sptr));
			dt.SetTicks(dev->lastSeenTime);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			this->lvDevices->SetSubItem(i, 6, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, dev->txPower);
			this->lvDevices->SetSubItem(i, 7, CSTRP(sbuff, sptr));
			this->lvDevices->SetSubItem(i, 8, (dev->inRange?CSTR("Y"):CSTR("N")));
			this->lvDevices->SetSubItem(i, 9, (dev->connected?CSTR("Y"):CSTR("N")));
			if (dev->company == 0)
			{
				this->lvDevices->SetSubItem(i, 10, CSTR("-"));
			}
			else
			{
				Text::CString cstr = Net::PacketAnalyzerBluetooth::CompanyGetName(dev->company);
				if (cstr.v)
				{
					this->lvDevices->SetSubItem(i, 10, cstr);
				}
				else
				{
					this->lvDevices->SetSubItem(i, 10, CSTR("?"));
				}
			}
			this->lvDevices->SetSubItem(i, 11, IO::BTScanLog::AdvTypeGetName(dev->advType));
			statusMap->Put(dev->macInt, 0);
		}
		j++;
	}
	return k;
}

SSWR::AVIRead::AVIRBluetoothCtlForm::AVIRBluetoothCtlForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Bluetooth Ctrl"));
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->bt = IO::BTScanner::CreateScanner();

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, CSTR("Scan/Stop")));
	this->btnStart->SetRect(4, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->btnStoreList, UI::GUIButton(ui, this->pnlControl, CSTR("Store Devices")));
	this->btnStoreList->SetRect(84, 4, 100, 23, false);
	this->btnStoreList->HandleButtonClick(OnStoreListClicked, this);
	NEW_CLASS(this->lvDevices, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 12));
	this->lvDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevices->SetShowGrid(true);
	this->lvDevices->SetFullRowSelect(true);
	this->lvDevices->HandleDblClk(OnDevicesDblClick, this);
	this->lvDevices->AddColumn(CSTR("MAC"), 120);
	this->lvDevices->AddColumn(CSTR("Type"), 60);
	this->lvDevices->AddColumn(CSTR("AddrType"), 80);
	this->lvDevices->AddColumn(CSTR("Name"), 150);
	this->lvDevices->AddColumn(CSTR("Vendor"), 150);
	this->lvDevices->AddColumn(CSTR("RSSI"), 60);
	this->lvDevices->AddColumn(CSTR("RecvTime"), 150);
	this->lvDevices->AddColumn(CSTR("TX Power"), 60);
	this->lvDevices->AddColumn(CSTR("Range"), 60);
	this->lvDevices->AddColumn(CSTR("Connect"), 60);
	this->lvDevices->AddColumn(CSTR("Company"), 200);
	this->lvDevices->AddColumn(CSTR("AdvType"), 80);

	if (this->bt)
	{
		this->bt->HandleRecordUpdate(OnDeviceUpdated, this);
		this->bt->ScanOn();
	}
	else
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in starting bluetoothctl"), CSTR("Bluetooth Ctrl"), this);
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
}

void SSWR::AVIRead::AVIRBluetoothCtlForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
