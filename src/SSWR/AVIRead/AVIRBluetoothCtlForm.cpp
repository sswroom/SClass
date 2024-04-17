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
#include <stdio.h>

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnStartClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRBluetoothCtlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothCtlForm>();
	NotNullPtr<IO::BTScanner> bt;
	if (me->bt.SetTo(bt))
	{
		if (bt->IsScanOn())
		{
			bt->ScanOff();
			return;
		}

		bt->ScanOn();
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnStoreListClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRBluetoothCtlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothCtlForm>();
	NotNullPtr<IO::BTScanner> bt;
	if (me->bt.SetTo(bt))
	{
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		Data::DateTime dt;
		IO::BTDevLog btLog;
		dt.SetCurrTimeUTC();
		sptr = Text::StrConcatC(Text::StrInt64(sbuff, dt.ToTicks()), UTF8STRC("bt.txt"));
		Sync::MutexUsage mutUsage;
		btLog.AppendList(bt->GetPublicMap(mutUsage));
		mutUsage.EndUse();
		btLog.AppendList(bt->GetRandomMap(mutUsage));
		mutUsage.EndUse();
		if (btLog.StoreFile(CSTRP(sbuff, sptr)))
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Stored as "));
			sb.AppendP(sbuff, sptr);
			me->ui->ShowMsgOK(sb.ToCString(), CSTR("Bluetooth Ctrl"), me);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Error in storing file"), CSTR("Bluetooth Ctrl"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnDevicesDblClick(AnyType userObj, UOSInt index)
{
	NotNullPtr<SSWR::AVIRead::AVIRBluetoothCtlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothCtlForm>();
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<IO::BTScanLog::ScanRecord3> dev;
	if (me->lvDevices->GetItem(index).GetOpt<IO::BTScanLog::ScanRecord3>().SetTo(dev))
	{
		sptr = Text::StrHexBytes(sbuff, dev->mac, 6, ':');
		UI::Clipboard::SetString(me->GetHandle(), CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnTimerTick(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRBluetoothCtlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothCtlForm>();
	NotNullPtr<IO::BTScanner> bt;
	if (me->bt.SetTo(bt))
	{
		Sync::MutexUsage mutUsage;
		UOSInt i = me->UpdateList(bt->GetPublicMap(mutUsage), &me->pubDevMap, 0);
		me->UpdateList(bt->GetRandomMap(mutUsage), &me->randDevMap, i);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnDeviceUpdated(NotNullPtr<IO::BTScanLog::ScanRecord3> dev, IO::BTScanner::UpdateType updateType, AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRBluetoothCtlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothCtlForm>();
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

UOSInt SSWR::AVIRead::AVIRBluetoothCtlForm::UpdateList(NotNullPtr<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> devMap, Data::FastMap<UInt64, UInt32> *statusMap, UOSInt baseIndex)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Data::DateTime dt;
	NotNullPtr<Text::String> s;
	Sync::MutexUsage mutUsage;
	NotNullPtr<IO::BTScanLog::ScanRecord3> dev;

	j = 0;
	k = devMap->GetCount();
	while (j < k)
	{
		dev = devMap->GetItemNoCheck(j);
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
					this->lvDevices->SetSubItem(i, 10, cstr.OrEmpty());
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

SSWR::AVIRead::AVIRBluetoothCtlForm::AVIRBluetoothCtlForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Bluetooth Ctrl"));
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->bt = IO::BTScanner::CreateScanner();

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnStart = ui->NewButton(this->pnlControl, CSTR("Scan/Stop"));
	this->btnStart->SetRect(4, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->btnStoreList = ui->NewButton(this->pnlControl, CSTR("Store Devices"));
	this->btnStoreList->SetRect(84, 4, 100, 23, false);
	this->btnStoreList->HandleButtonClick(OnStoreListClicked, this);
	this->lvDevices = ui->NewListView(*this, UI::ListViewStyle::Table, 12);
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

	NotNullPtr<IO::BTScanner> bt;
	if (this->bt.SetTo(bt))
	{
		bt->HandleRecordUpdate(OnDeviceUpdated, this);
		bt->ScanOn();
	}
	else
	{
		this->ui->ShowMsgOK(CSTR("Error in starting bluetoothctl"), CSTR("Bluetooth Ctrl"), this);
	}
	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRBluetoothCtlForm::~AVIRBluetoothCtlForm()
{
	NotNullPtr<IO::BTScanner> bt;
	if (this->bt.SetTo(bt))
	{
		bt->ScanOff();
		this->bt.Delete();
	}
}

void SSWR::AVIRead::AVIRBluetoothCtlForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
