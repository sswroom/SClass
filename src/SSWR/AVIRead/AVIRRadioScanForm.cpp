#include "Stdafx.h"
#include "IO/Path.h"
#include "Math/Unit/Count.h"
#include "Net/MACInfo.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WiFiUtil.h"
#include "SSWR/AVIRead/AVIRRadioScanForm.h"

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	NN<Net::WirelessLAN::Interface> wlanInterf;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int64 currTime;
	UInt8 id[8];
	UInt64 imac;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	if (me->wlanInterf.SetTo(wlanInterf))
	{
		currTime = Data::DateTimeUtil::GetCurrTimeMillis();
		if (currTime - me->wlanLastTime > 900)
		{
			if (me->wlanScan-- <= 0)
			{
				NN<Text::String> s;
				NN<Text::String> ssid;
				NN<Net::WirelessLAN::BSSInfo> bss;
				me->lvWiFi->ClearItems();
				me->wlanBSSList.DeleteAll();
				wlanInterf->GetBSSList(me->wlanBSSList);

				i = 0;
				j = me->wlanBSSList.GetCount();
				while (i < j)
				{
					bss = me->wlanBSSList.GetItemNoCheck(i);
					ssid = bss->GetSSID();
					MemCopyNO(&id[2], bss->GetMAC(), 6);
					id[0] = 0;
					id[1] = 0;
					imac = ReadMUInt64(id);

					sptr = Text::StrHexBytes(sbuff, &id[2], 6, ':');
					k = me->lvWiFi->AddItem(CSTRP(sbuff, sptr), bss);
					sptr = Math::Unit::Count::WellFormat(sbuff, bss->GetFreq(), "0.000");
					me->lvWiFi->SetSubItem(k, 1, CSTRP(sbuff, sptr));
					sptr = Text::StrDouble(sbuff, bss->GetRSSI());
					me->lvWiFi->SetSubItem(k, 2, CSTRP(sbuff, sptr));
					if (Net::WiFiUtil::GuessDeviceType(sbuff, bss).SetTo(sptr))
						me->lvWiFi->SetSubItem(k, 3, CSTRP(sbuff, sptr));
					me->lvWiFi->SetSubItem(k, 4, ssid);
					NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfo(imac);
					me->lvWiFi->SetSubItem(k, 5, {entry->name, entry->nameLen});
					if (s.Set(bss->GetManuf()))
						me->lvWiFi->SetSubItem(k, 6, s);
					if (s.Set(bss->GetModel()))
						me->lvWiFi->SetSubItem(k, 7, s);
					if (s.Set(bss->GetSN()))
						me->lvWiFi->SetSubItem(k, 8, s);

					i++;
				}
				sptr = Text::StrUOSInt(sbuff, j);
				me->lvDashboard->SetSubItem(0, 2, CSTRP(sbuff, sptr));

				wlanInterf->Scan();

				me->wlanScan = 10;
			}
			me->wlanLastTime = Data::DateTimeUtil::GetCurrTimeMillis();
		}
	}

	NN<IO::BTScanner> bt;
	if (me->bt.SetTo(bt))
	{
		currTime = Data::DateTimeUtil::GetCurrTimeMillis();
		if (me->btUpdated || (me->btMinTime != 0 && me->btMinTime + 60000 < currTime))
		{
			me->btUpdated = false;
			UOSInt cnt = 0;
			Int64 minTime;
			Int64 minTime2;
			me->lvBluetooth->ClearItems();
			{
				Sync::MutexUsage mutUsage;
				cnt = me->AppendBTList(bt->GetPublicMap(mutUsage), currTime, minTime);
				cnt += me->AppendBTList(bt->GetRandomMap(mutUsage), currTime, minTime2);
			}
			if (minTime == 0)
				minTime = minTime2;
			else if (minTime2 != 0 && minTime > minTime2)
				minTime = minTime2;
			me->btMinTime = minTime;
			sptr = Text::StrUOSInt(sbuff, cnt);
			me->lvDashboard->SetSubItem(1, 2, CSTRP(sbuff, sptr));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnWiFiClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	me->ToggleWiFi();	
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnWiFiSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	NN<Net::WirelessLAN::BSSInfo> bss;
	if (!me->lvWiFi->GetSelectedItem().GetOpt<Net::WirelessLAN::BSSInfo>().SetTo(bss))
	{
		me->txtWiFiDetail->SetText(CSTR(""));
	}
	else
	{
		UOSInt i;
		UOSInt j;
		NN<Net::WirelessLANIE> ie;
		Text::StringBuilderUTF8 sb;
		i = 0;
		j = bss->GetIECount();
		while (i < j)
		{
			if (bss->GetIE(i).SetTo(ie))
			{
				if (i > 0)
				{
					sb.AppendC(UTF8STRC("\r\n"));
				}
				Net::WirelessLANIE::ToString(ie->GetIEBuff(), sb);
			}
			i++;
		}
		me->txtWiFiDetail->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnBluetoothClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	me->ToggleBT();
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnDashboardDblClk(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	if (index == 0)
	{
		me->ToggleWiFi();
	}
	else if (index == 1)
	{
		me->ToggleBT();
	}
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnDeviceUpdated(NN<IO::BTScanLog::ScanRecord3> dev, IO::BTScanner::UpdateType updateType, AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	me->btUpdated = true;
}

void SSWR::AVIRead::AVIRRadioScanForm::ToggleWiFi()
{
	if (this->wlanInterf.NotNull())
	{
		this->wlanInterf.Delete();
		this->lvDashboard->SetSubItem(0, 1, CSTR("Idle"));
		this->btnWiFi->SetText(CSTR("Start"));
	}
	else
	{
		UOSInt i;
		Data::ArrayListNN<Net::WirelessLAN::Interface> interfList;
		this->wlan.GetInterfaces(interfList);
		this->wlanInterf = interfList.GetItem(0);
		i = interfList.GetCount();
		while (i-- > 1)
		{
			NN<Net::WirelessLAN::Interface> interf = interfList.GetItemNoCheck(i);
			interf.Delete();
		}
		this->wlanScan = 0;
		if (this->wlanInterf.IsNull())
		{
			this->ui->ShowMsgOK(CSTR("Wireless LAN Interface not found"), CSTR("Radio Scan"), this);
		}
		else
		{
			this->lvDashboard->SetSubItem(0, 1, CSTR("Scanning"));
			this->btnWiFi->SetText(CSTR("Stop"));
		}
	}
}

void SSWR::AVIRead::AVIRRadioScanForm::ToggleBT()
{
	NN<IO::BTScanner> bt;
	if (this->bt.SetTo(bt))
	{
		bt->ScanOff();
		this->bt.Delete();
		this->lvDashboard->SetSubItem(1, 1, CSTR("Idle"));
		this->lvDashboard->SetSubItem(1, 2, CSTR("0"));
		this->btnBluetooth->SetText(CSTR("Start"));
		this->lvBluetooth->ClearItems();
		this->btUpdated = false;
		this->btMinTime = 0;
	}
	else
	{
		this->bt = IO::BTScanner::CreateScanner();
		if (this->bt.SetTo(bt))
		{
			this->btUpdated = false;
			this->btMinTime = 0;
			bt->HandleRecordUpdate(OnDeviceUpdated, this);
			bt->ScanOn();
			this->lvDashboard->SetSubItem(1, 1, CSTR("Scanning"));
			this->btnBluetooth->SetText(CSTR("Stop"));
		}
		else
		{
			this->ui->ShowMsgOK(CSTR("Bluetooth Interface not found"), CSTR("Radio Scan"), this);
		}
	}
}

UOSInt SSWR::AVIRead::AVIRRadioScanForm::AppendBTList(NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> devMap, Int64 currTime, OutParam<Int64> minTime)
{
	Int64 mTime = 0;
	UOSInt cnt = 0;
	NN<IO::BTScanLog::ScanRecord3> rec;
	Int64 expTime = currTime - 60000;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	UOSInt k;
	UOSInt i = 0;
	UOSInt j = devMap->GetCount();
	while (i < j)
	{
		rec = devMap->GetItemNoCheck(i);
		if (rec->lastSeenTime >= expTime)
		{
			sptr = Text::StrHexBytes(sbuff, rec->mac, 6, ':');
			k = this->lvBluetooth->AddItem(CSTRP(sbuff, sptr), 0);
			this->lvBluetooth->SetSubItem(k, 1, IO::BTScanLog::AddressTypeGetName(rec->addrType));
			this->lvBluetooth->SetSubItem(k, 2, IO::BTScanLog::RadioTypeGetName(rec->radioType));
			sptr = Text::StrInt32(sbuff, rec->rssi);
			this->lvBluetooth->SetSubItem(k, 3, CSTRP(sbuff, sptr));
			sptr = Data::Timestamp(rec->lastSeenTime, Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			this->lvBluetooth->SetSubItem(k, 4, CSTRP(sbuff, sptr));
			if (rec->name.SetTo(s))
			{
				this->lvBluetooth->SetSubItem(k, 5, s);
			}
			if (rec->addrType == IO::BTScanLog::AT_RANDOM)
			{
				switch (rec->mac[0] & 0xC0)
				{
				case 0x00:
					this->lvBluetooth->SetSubItem(k, 6, CSTR("Non-resolvable Random"));
					break;
				case 0x40:
					this->lvBluetooth->SetSubItem(k, 6, CSTR("Resolvable Random"));
					break;
				case 0xC0:
					this->lvBluetooth->SetSubItem(k, 6, CSTR("Static Random"));
					break;
				default:
					this->lvBluetooth->SetSubItem(k, 6, CSTR("-"));
					break;
				}
			}
			else
			{
				NN<const Net::MACInfo::MACEntry> mac = Net::MACInfo::GetMACInfo(rec->macInt);
				this->lvBluetooth->SetSubItem(k, 6, {mac->name, mac->nameLen});
			}
			if (rec->company == 0)
			{
				this->lvBluetooth->SetSubItem(k, 7, CSTR("-"));
			}
			else
			{
				Text::CStringNN cstr;
				if (Net::PacketAnalyzerBluetooth::CompanyGetName(rec->company).SetTo(cstr))
				{
					this->lvBluetooth->SetSubItem(k, 7, cstr);
				}
				else
				{
					this->lvBluetooth->SetSubItem(k, 7, CSTR("?"));
				}
			}
			this->lvBluetooth->SetSubItem(k, 8, IO::BTScanLog::AdvTypeGetName(rec->advType));
			if (mTime == 0 || mTime > rec->lastSeenTime)
				mTime = rec->lastSeenTime;
			cnt++;
		}
		i++;
	}
	minTime.Set(mTime);
	return cnt;
}

SSWR::AVIRead::AVIRRadioScanForm::AVIRRadioScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Radio Scan"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDashboard = this->tcMain->AddTabPage(CSTR("Dashboard"));
	this->lvDashboard = ui->NewListView(this->tpDashboard, UI::ListViewStyle::Table, 3);
	this->lvDashboard->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDashboard->SetFullRowSelect(true);
	this->lvDashboard->SetShowGrid(true);
	this->lvDashboard->AddColumn(CSTR("Type"), 80);
	this->lvDashboard->AddColumn(CSTR("Status"), 80);
	this->lvDashboard->AddColumn(CSTR("Count"), 100);
	this->lvDashboard->HandleDblClk(OnDashboardDblClk, this);

	this->tpWiFi = this->tcMain->AddTabPage(CSTR("WiFi"));
	this->pnlWiFi = ui->NewPanel(this->tpWiFi);
	this->pnlWiFi->SetRect(0, 0, 100, 31, false);
	this->pnlWiFi->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnWiFi = ui->NewButton(this->pnlWiFi, CSTR("Start"));
	this->btnWiFi->SetRect(4, 4, 75, 23, false);
	this->btnWiFi->HandleButtonClick(OnWiFiClicked, this);
	this->txtWiFiDetail = ui->NewTextBox(this->tpWiFi, CSTR(""), true);
	this->txtWiFiDetail->SetReadOnly(true);
	this->txtWiFiDetail->SetRect(0, 0, 100, 100, false);
	this->txtWiFiDetail->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvWiFi = ui->NewListView(this->tpWiFi, UI::ListViewStyle::Table, 9);
	this->lvWiFi->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvWiFi->SetShowGrid(true);
	this->lvWiFi->SetFullRowSelect(true);
	this->lvWiFi->AddColumn(CSTR("MAC Address"), 120);
	this->lvWiFi->AddColumn(CSTR("Frequency"), 60);
	this->lvWiFi->AddColumn(CSTR("RSSI"), 50);
	this->lvWiFi->AddColumn(CSTR("Device"), 150);
	this->lvWiFi->AddColumn(CSTR("SSID"), 150);
	this->lvWiFi->AddColumn(CSTR("Vendor"), 150);
	this->lvWiFi->AddColumn(CSTR("Manufacturer"), 100);
	this->lvWiFi->AddColumn(CSTR("Model"), 100);
	this->lvWiFi->AddColumn(CSTR("S/N"), 100);
	this->lvWiFi->HandleSelChg(OnWiFiSelChg, this);
	this->lvDashboard->AddItem(CSTR("WiFi"), 0);
	this->lvDashboard->SetSubItem(0, 1, CSTR("Idle"));
	this->lvDashboard->SetSubItem(0, 2, CSTR("0"));

	this->tpBluetooth = this->tcMain->AddTabPage(CSTR("Bluetooth"));
	this->pnlBluetooth = ui->NewPanel(this->tpBluetooth);
	this->pnlBluetooth->SetRect(0, 0, 100, 31, false);
	this->pnlBluetooth->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnBluetooth = ui->NewButton(this->pnlBluetooth, CSTR("Start"));
	this->btnBluetooth->SetRect(4, 4, 75, 23, false);
	this->btnBluetooth->HandleButtonClick(OnBluetoothClicked, this);
	this->lvBluetooth = ui->NewListView(this->tpBluetooth, UI::ListViewStyle::Table, 9);
	this->lvBluetooth->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvBluetooth->SetShowGrid(true);
	this->lvBluetooth->SetFullRowSelect(true);
	this->lvBluetooth->AddColumn(CSTR("MAC Address"), 120);
	this->lvBluetooth->AddColumn(CSTR("AddrType"), 60);
	this->lvBluetooth->AddColumn(CSTR("Type"), 50);
	this->lvBluetooth->AddColumn(CSTR("RSSI"), 50);
	this->lvBluetooth->AddColumn(CSTR("RecvTime"), 130);
	this->lvBluetooth->AddColumn(CSTR("Name"), 150);
	this->lvBluetooth->AddColumn(CSTR("Vendor"), 150);
	this->lvBluetooth->AddColumn(CSTR("Company"), 150);
	this->lvBluetooth->AddColumn(CSTR("AdvType"), 100);
	this->lvDashboard->AddItem(CSTR("Bluetooth"), 0);
	this->lvDashboard->SetSubItem(1, 1, CSTR("Idle"));
	this->lvDashboard->SetSubItem(1, 2, CSTR("0"));

	this->wlanInterf = 0;
	this->wlanScan = 0;
	this->wlanLastTime = 0;
	this->bt = 0;
	this->btUpdated = false;
	this->btMinTime = 0;
	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRRadioScanForm::~AVIRRadioScanForm()
{
	this->wlanInterf.Delete();
	this->wlanBSSList.DeleteAll();

	NN<IO::BTScanner> bt;
	if (this->bt.SetTo(bt))
	{
		bt->ScanOff();
		this->bt.Delete();
	}
}

void SSWR::AVIRead::AVIRRadioScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
