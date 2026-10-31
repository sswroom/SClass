#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "IO/BTAdvData.h"
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
	NN<SSWR::AVIRead::AVIRBluetoothCtlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothCtlForm>();
	NN<IO::BTScanner> bt;
	if (me->bt.SetTo(bt))
	{
		if (bt->IsScanOn())
		{
			bt->ScanOff();
			me->lblStatus->SetText(CSTR("Idle"));
			return;
		}
		bt->ScanOn();
		me->lblStatus->SetText(bt->IsScanOn() ? CSTR("Scanning") : CSTR("Idle"));
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnStoreListClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothCtlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothCtlForm>();
	NN<IO::BTScanner> bt;
	if (me->bt.SetTo(bt))
	{
		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
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

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnDevicesDblClick(AnyType userObj, UIntOS index)
{
	NN<SSWR::AVIRead::AVIRBluetoothCtlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothCtlForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::BTScanLog::ScanRecord5> dev;
	if (me->lvDevices->GetItem(index).GetOpt<IO::BTScanLog::ScanRecord5>().SetTo(dev))
	{
		sptr = Text::StrHexBytes(sbuff, dev->mac, 6, ':');
		UI::Clipboard::SetString(me->GetHandle(), CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothCtlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothCtlForm>();
	NN<IO::BTScanner> bt;
	if (me->bt.SetTo(bt))
	{
		Sync::MutexUsage mutUsage;
		UIntOS i = me->UpdateList(bt->GetPublicMap(mutUsage), me->pubDevMap, 0);
		me->UpdateList(bt->GetRandomMap(mutUsage), me->randDevMap, i);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothCtlForm::OnDeviceUpdated(NN<IO::BTScanLog::ScanRecord5> dev, IO::BTScanner::UpdateType updateType, AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothCtlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothCtlForm>();
	Sync::MutexUsage mutUsage(me->devMut);
	if (dev->addrType == IO::BTScanLog::AT_RANDOM)
	{
		if (me->randDevMap.GetIndex(dev->mac64Int) >= 0)
		{
			me->randDevMap.Put(dev->mac64Int, 1);
		}
	}
	else
	{
		if (me->pubDevMap.GetIndex(dev->mac64Int) >= 0)
		{
			me->pubDevMap.Put(dev->mac64Int, 1);
		}
	}
}

UIntOS SSWR::AVIRead::AVIRBluetoothCtlForm::UpdateList(NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord5>> devMap, NN<Data::FastMapNative<UInt64, UInt32>> statusMap, UIntOS baseIndex)
{
	UIntOS i;
	UIntOS j;
	UIntOS k;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Data::DateTime dt;
	NN<Text::String> s;
	Sync::MutexUsage mutUsage;
	NN<IO::BTScanLog::ScanRecord5> dev;

	j = 0;
	k = devMap->GetCount();
	while (j < k)
	{
		dev = devMap->GetItemNoCheck(j);
		i = j + baseIndex;
		Sync::MutexUsage devMutUsage(this->devMut);
		if (statusMap->GetIndex(dev->mac64Int) < 0)
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
				NN<const Net::MACInfo::MACEntry> mac = Net::MACInfo::GetMAC64Info(dev->mac64Int);
				this->lvDevices->SetSubItem(i, 4, {mac->name, mac->nameLen});
			}
			statusMap->Put(dev->mac64Int, 1);
		}
		if (statusMap->Get(dev->mac64Int) != 0)
		{
			if (dev->name.SetTo(s))
			{
				this->lvDevices->SetSubItem(i, 3, s);
			}
			sptr = Text::StrHexByte(Text::StrConcatC(sbuff, UTF8STRC("0x")), dev->flags);
			this->lvDevices->SetSubItem(i, 5, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, dev->rssi);
			this->lvDevices->SetSubItem(i, 6, CSTRP(sbuff, sptr));
			dt.SetTicks(dev->lastSeenTime);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			this->lvDevices->SetSubItem(i, 7, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, dev->txPower);
			this->lvDevices->SetSubItem(i, 8, CSTRP(sbuff, sptr));
			this->lvDevices->SetSubItem(i, 9, (dev->inRange?CSTR("Y"):CSTR("N")));
			this->lvDevices->SetSubItem(i, 10, (dev->connected?CSTR("Y"):CSTR("N")));
			if (dev->company == 0)
			{
				this->lvDevices->SetSubItem(i, 11, CSTR("-"));
			}
			else
			{
				Text::CStringNN cstr;
				if (Net::PacketAnalyzerBluetooth::CompanyGetName(dev->company).SetTo(cstr))
				{
					this->lvDevices->SetSubItem(i, 11, cstr);
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					sb.Append(CSTR("? (0x"));
					sb.AppendHex16(dev->company);
					sb.AppendC(UTF8STRC(")"));
					this->lvDevices->SetSubItem(i, 11, sb.ToCString());
				}
			}
			this->lvDevices->SetSubItem(i, 12, IO::BTScanLog::AdvTypeGetName(dev->advType));
			if (dev->advData[0] == 0 || dev->advData[0] > 30)
			{
				this->lvDevices->SetSubItem(i, 13, CSTR(""));
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				if (IO::BTAdvData::ToString(dev->serviceUUID, Data::ByteArrayR(dev->serviceData, dev->serviceDataLen), dev->advData, sb))
				{
					this->lvDevices->SetSubItem(i, 13, sb.ToCString());
				}
				else
				{
					sb.AppendC(UTF8STRC("Unknown: "));
					sb.AppendHexBuff(Data::ByteArrayR(dev->advData, dev->advData[0] + 1), ' ', Text::LineBreakType::None);
					printf("AVIRBluetoothCtlForm: Unknown advData: %s\r\n", sb.v.Ptr());
					this->lvDevices->SetSubItem(i, 13, sb.ToCString());
				}
			}
			statusMap->Put(dev->mac64Int, 0);
		}
		j++;
	}
	return k;
}

SSWR::AVIRead::AVIRBluetoothCtlForm::AVIRBluetoothCtlForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
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
	this->lblStatus = ui->NewLabel(this->pnlControl, CSTR("Idle"));
	this->lblStatus->SetRect(190, 4, 200, 23, false);
	this->lvDevices = ui->NewListView(*this, UI::ListViewStyle::Table, 14);
	this->lvDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevices->SetShowGrid(true);
	this->lvDevices->SetFullRowSelect(true);
	this->lvDevices->HandleDblClk(OnDevicesDblClick, this);
	this->lvDevices->AddColumn(CSTR("MAC"), 120);
	this->lvDevices->AddColumn(CSTR("Type"), 60);
	this->lvDevices->AddColumn(CSTR("AddrType"), 80);
	this->lvDevices->AddColumn(CSTR("Name"), 150);
	this->lvDevices->AddColumn(CSTR("Vendor"), 150);
	this->lvDevices->AddColumn(CSTR("Flags"), 30);
	this->lvDevices->AddColumn(CSTR("RSSI"), 60);
	this->lvDevices->AddColumn(CSTR("RecvTime"), 150);
	this->lvDevices->AddColumn(CSTR("TX Power"), 60);
	this->lvDevices->AddColumn(CSTR("Range"), 60);
	this->lvDevices->AddColumn(CSTR("Connect"), 60);
	this->lvDevices->AddColumn(CSTR("Company"), 200);
	this->lvDevices->AddColumn(CSTR("AdvType"), 80);
	this->lvDevices->AddColumn(CSTR("AdvData"), 200);

	NN<IO::BTScanner> bt;
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
	NN<IO::BTScanner> bt;
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
