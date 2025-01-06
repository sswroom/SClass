#include "Stdafx.h"
#include "IO/GPSNMEA.h"
#include "IO/Path.h"
#include "IO/SerialPortUtil.h"
#include "IO/Device/MTKGPSNMEA.h"
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

	if (me->gpsRecUpdated)
	{
		me->gpsLastUpdateTime = Data::DateTimeUtil::GetCurrTimeMillis();
		Sync::MutexUsage mutUsage(me->gpsRecMut);
		sptr = Data::Timestamp(me->gpsRecCurr.recTime, Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
		me->txtGPSTime->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->gpsRecCurr.pos.GetLat());
		me->txtGPSLatitude->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->gpsRecCurr.pos.GetLon());
		me->txtGPSLongitude->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->gpsRecCurr.altitude);
		me->txtGPSAltitude->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->gpsRecCurr.speed);
		me->txtGPSSpeed->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->gpsRecCurr.heading);
		me->txtGPSHeading->SetText(CSTRP(sbuff, sptr));
		if (me->gpsRecCurr.valid)
			me->txtGPSValid->SetText(CSTR("Valid"));
		else
			me->txtGPSValid->SetText(CSTR("Invalid"));
		sptr = Text::StrInt32(sbuff, me->gpsRecCurr.nSateUsed);
		me->txtGPSNSateUsed->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gpsRecCurr.nSateUsedGPS);
		me->txtGPSNSateUsedGPS->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gpsRecCurr.nSateUsedSBAS);
		me->txtGPSNSateUsedSBAS->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gpsRecCurr.nSateUsedGLO);
		me->txtGPSNSateUsedGLO->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gpsRecCurr.nSateViewGPS);
		me->txtGPSNSateViewGPS->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gpsRecCurr.nSateViewGLO);
		me->txtGPSNSateViewGLO->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gpsRecCurr.nSateViewGA);
		me->txtGPSNSateViewGA->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gpsRecCurr.nSateViewQZSS);
		me->txtGPSNSateViewQZSS->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gpsRecCurr.nSateViewBD);
		me->txtGPSNSateViewBD->SetText(CSTRP(sbuff, sptr));

		me->lvGPSSatellite->ClearItems();
		UOSInt actCnt = 0;
		UOSInt i = 0;
		while (i < me->gpsRecSateCnt)
		{
			me->lvGPSSatellite->AddItem(Map::ILocationService::SateTypeGetName(me->gpsRecSates[i].sateType), 0);
			sptr = Text::StrUInt16(sbuff, me->gpsRecSates[i].prn);
			me->lvGPSSatellite->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, me->gpsRecSates[i].elev);
			me->lvGPSSatellite->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, me->gpsRecSates[i].azimuth);
			me->lvGPSSatellite->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			if (me->gpsRecSates[i].snr >= 0)
			{
				sptr = Text::StrInt16(sbuff, me->gpsRecSates[i].snr);
				me->lvGPSSatellite->SetSubItem(i, 4, CSTRP(sbuff, sptr));
				actCnt++;
			}
			i++;
		}
		sptr = Text::StrUOSInt(sbuff, actCnt);
		*sptr++ = '/';
		sptr = Text::StrUOSInt(sptr, me->gpsRecSateCnt);
		me->lvDashboard->SetSubItem(2, 2, CSTRP(sbuff, sptr));

		me->gpsRecUpdated = false;
	}
	else
	{
		NN<Map::ILocationService> locSvc;
		currTime = Data::DateTimeUtil::GetCurrTimeMillis();
		if (me->locSvc.SetTo(locSvc) && (currTime - me->gpsLastUpdateTime) >= 20000)
		{
			me->gpsLastUpdateTime = currTime;
			locSvc->ErrorRecover();
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
	else if (index == 2)
	{
		me->ToggleGPS();
	}
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnDeviceUpdated(NN<IO::BTScanLog::ScanRecord3> dev, IO::BTScanner::UpdateType updateType, AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	me->btUpdated = true;
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnGPSPortClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	me->cboGPSPort->ClearItems();
	IO::SerialPortUtil::FillPortSelector(me->cboGPSPort, IO::Device::MTKGPSNMEA::GetMTKSerialPort());
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnGPSClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	me->ToggleGPS();
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnGPSLocationUpdated(AnyType userObj, NN<Map::GPSTrack::GPSRecord3> record, Data::DataArray<Map::ILocationService::SateStatus> sates)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	Sync::MutexUsage mutUsage(me->gpsRecMut);
	MemCopyNO(&me->gpsRecCurr, record.Ptr(), sizeof(Map::GPSTrack::GPSRecord3));
	me->gpsRecSateCnt = sates.GetCount();
	MemCopyNO(me->gpsRecSates, sates.Arr().Ptr(), sates.GetCount() * sizeof(Map::ILocationService::SateStatus));
	me->gpsRecUpdated = true;
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

void SSWR::AVIRead::AVIRRadioScanForm::ToggleGPS()
{
	if (this->locSvc.NotNull())
	{
		this->locSvc.Delete();
		this->btnGPS->SetText(CSTR("Start"));
		this->lvDashboard->SetSubItem(2, 1, CSTR("Idle"));
		this->cboGPSPort->SetEnabled(true);
		this->cboGPSBaudRate->SetEnabled(true);
		this->cboGPSParity->SetEnabled(true);
		this->chkGPSFlowControl->SetEnabled(true);
		this->btnGPSPort->SetEnabled(true);
	}
	else
	{
		NN<IO::SerialPort> stm;
		if (IO::SerialPortUtil::OpenSerialPort(this->cboGPSPort, this->cboGPSBaudRate, this->cboGPSParity, this->chkGPSFlowControl->IsChecked()).SetTo(stm))
		{
			NN<Map::ILocationService> locSvc;
			NEW_CLASSNN(locSvc, IO::GPSNMEA(stm, true));
			locSvc->RegisterLocationHandler(OnGPSLocationUpdated, this);
			this->locSvc = locSvc;
			this->gpsLastUpdateTime = Data::DateTimeUtil::GetCurrTimeMillis();
			this->lvDashboard->SetSubItem(2, 1, CSTR("Scanning"));
			this->btnGPS->SetText(CSTR("Stop"));
			this->cboGPSPort->SetEnabled(false);
			this->cboGPSBaudRate->SetEnabled(false);
			this->cboGPSParity->SetEnabled(false);
			this->chkGPSFlowControl->SetEnabled(false);
			this->btnGPSPort->SetEnabled(false);
		}
		else
		{
			this->ui->ShowMsgOK(CSTR("Error in opening GPS Serial Port"), CSTR("Radio Scan"), this);
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

	this->tpGPS = this->tcMain->AddTabPage(CSTR("GPS"));
	this->pnlGPS = ui->NewPanel(this->tpGPS);
	this->pnlGPS->SetRect(0, 0, 100, 127, false);
	this->pnlGPS->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblGPSPort = ui->NewLabel(this->pnlGPS, CSTR("Port"));
	this->lblGPSPort->SetRect(4, 4, 100, 23, false);
	this->cboGPSPort = ui->NewComboBox(this->pnlGPS, false);
	this->cboGPSPort->SetRect(104, 4, 150, 23, false);
	IO::SerialPortUtil::FillPortSelector(this->cboGPSPort, IO::Device::MTKGPSNMEA::GetMTKSerialPort());
	this->btnGPSPort = ui->NewButton(this->pnlGPS, CSTR("Refresh"));
	this->btnGPSPort->SetRect(254, 4, 75, 23, false);
	this->btnGPSPort->HandleButtonClick(OnGPSPortClicked, this);
	this->lblGPSBaudRate = ui->NewLabel(this->pnlGPS, CSTR("Baud Rate"));
	this->lblGPSBaudRate->SetRect(4, 28, 100, 23, false);
	this->cboGPSBaudRate = ui->NewComboBox(this->pnlGPS, false);
	this->cboGPSBaudRate->SetRect(104, 28, 100, 23, false);
	IO::SerialPortUtil::FillBaudRateSelector(this->cboGPSBaudRate);
	this->lblGPSParity = ui->NewLabel(this->pnlGPS, CSTR("Parity"));
	this->lblGPSParity->SetRect(4, 52, 100, 23, false);
	this->cboGPSParity = ui->NewComboBox(this->pnlGPS, false);
	this->cboGPSParity->SetRect(104, 52, 100, 23, false);
	IO::SerialPortUtil::FillParitySelector(this->cboGPSParity);
	this->lblGPSFlowControl = ui->NewLabel(this->pnlGPS, CSTR("Flow Control"));
	this->lblGPSFlowControl->SetRect(4, 76, 100, 23, false);
	this->chkGPSFlowControl = ui->NewCheckBox(this->pnlGPS, CSTR("Enabled"), false);
	this->chkGPSFlowControl->SetRect(104, 76, 100, 23, false);
	this->btnGPS = ui->NewButton(this->pnlGPS, CSTR("Start"));
	this->btnGPS->SetRect(104, 100, 75, 23, false);
	this->btnGPS->HandleButtonClick(OnGPSClicked, this);
	this->tcGPS = ui->NewTabControl(this->tpGPS);
	this->tcGPS->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpGPSSatellite = this->tcGPS->AddTabPage(CSTR("Satellite"));
	this->lvGPSSatellite = ui->NewListView(this->tpGPSSatellite, UI::ListViewStyle::Table, 5);
	this->lvGPSSatellite->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvGPSSatellite->AddColumn(CSTR("Type"), 80);
	this->lvGPSSatellite->AddColumn(CSTR("PRN"), 60);
	this->lvGPSSatellite->AddColumn(CSTR("Elevation"), 60);
	this->lvGPSSatellite->AddColumn(CSTR("Azimuth"), 60);
	this->lvGPSSatellite->AddColumn(CSTR("SNR"), 60);
	this->lvGPSSatellite->SetShowGrid(true);
	this->lvGPSSatellite->SetFullRowSelect(true);
	this->tpGPSLocation = this->tcGPS->AddTabPage(CSTR("Location"));
	this->lblGPSTime = ui->NewLabel(this->tpGPSLocation, CSTR("GPS Time"));
	this->lblGPSTime->SetRect(4, 4, 100, 23, false);
	this->txtGPSTime = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSTime->SetReadOnly(true);
	this->txtGPSTime->SetRect(104, 4, 200, 23, false);
	this->lblGPSLatitude = ui->NewLabel(this->tpGPSLocation, CSTR("Latitude"));
	this->lblGPSLatitude->SetRect(4, 28, 100, 23, false);
	this->txtGPSLatitude = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSLatitude->SetReadOnly(true);
	this->txtGPSLatitude->SetRect(104, 28, 120, 23, false);
	this->lblGPSLongitude = ui->NewLabel(this->tpGPSLocation, CSTR("Longitude"));
	this->lblGPSLongitude->SetRect(4, 52, 100, 23, false);
	this->txtGPSLongitude = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSLongitude->SetReadOnly(true);
	this->txtGPSLongitude->SetRect(104, 52, 120, 23, false);
	this->lblGPSAltitude = ui->NewLabel(this->tpGPSLocation, CSTR("Altitude"));
	this->lblGPSAltitude->SetRect(4, 76, 100, 23, false);
	this->txtGPSAltitude = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSAltitude->SetReadOnly(true);
	this->txtGPSAltitude->SetRect(104, 76, 100, 23, false);
	this->lblGPSSpeed = ui->NewLabel(this->tpGPSLocation, CSTR("Speed(knot)"));
	this->lblGPSSpeed->SetRect(4, 100, 100, 23, false);
	this->txtGPSSpeed = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSSpeed->SetReadOnly(true);
	this->txtGPSSpeed->SetRect(104, 100, 100, 23, false);
	this->lblGPSHeading = ui->NewLabel(this->tpGPSLocation, CSTR("Heading"));
	this->lblGPSHeading->SetRect(4, 124, 100, 23, false);
	this->txtGPSHeading = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSHeading->SetReadOnly(true);
	this->txtGPSHeading->SetRect(104, 124, 100, 23, false);
	this->lblGPSValid = ui->NewLabel(this->tpGPSLocation, CSTR("Valid"));
	this->lblGPSValid->SetRect(4, 148, 100, 23, false);
	this->txtGPSValid = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSValid->SetReadOnly(true);
	this->txtGPSValid->SetRect(104, 148, 100, 23, false);
	this->lblGPSNSateUsed = ui->NewLabel(this->tpGPSLocation, CSTR("Sate Used"));
	this->lblGPSNSateUsed->SetRect(4, 172, 100, 23, false);
	this->txtGPSNSateUsed = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSNSateUsed->SetReadOnly(true);
	this->txtGPSNSateUsed->SetRect(104, 172, 100, 23, false);
	this->lblGPSNSateUsedGPS = ui->NewLabel(this->tpGPSLocation, CSTR("GPS Used"));
	this->lblGPSNSateUsedGPS->SetRect(4, 196, 100, 23, false);
	this->txtGPSNSateUsedGPS = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSNSateUsedGPS->SetReadOnly(true);
	this->txtGPSNSateUsedGPS->SetRect(104, 196, 100, 23, false);
	this->lblGPSNSateUsedSBAS = ui->NewLabel(this->tpGPSLocation, CSTR("SBAS Used"));
	this->lblGPSNSateUsedSBAS->SetRect(4, 220, 100, 23, false);
	this->txtGPSNSateUsedSBAS = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSNSateUsedSBAS->SetReadOnly(true);
	this->txtGPSNSateUsedSBAS->SetRect(104, 220, 100, 23, false);
	this->lblGPSNSateUsedGLO = ui->NewLabel(this->tpGPSLocation, CSTR("GLO Used"));
	this->lblGPSNSateUsedGLO->SetRect(4, 244, 100, 23, false);
	this->txtGPSNSateUsedGLO = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSNSateUsedGLO->SetReadOnly(true);
	this->txtGPSNSateUsedGLO->SetRect(104, 244, 100, 23, false);
	this->lblGPSNSateViewGPS = ui->NewLabel(this->tpGPSLocation, CSTR("GPS In-view"));
	this->lblGPSNSateViewGPS->SetRect(4, 268, 100, 23, false);
	this->txtGPSNSateViewGPS = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSNSateViewGPS->SetReadOnly(true);
	this->txtGPSNSateViewGPS->SetRect(104, 268, 100, 23, false);
	this->lblGPSNSateViewGLO = ui->NewLabel(this->tpGPSLocation, CSTR("GLO In-view"));
	this->lblGPSNSateViewGLO->SetRect(4, 292, 100, 23, false);
	this->txtGPSNSateViewGLO = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSNSateViewGLO->SetReadOnly(true);
	this->txtGPSNSateViewGLO->SetRect(104, 292, 100, 23, false);
	this->lblGPSNSateViewGA = ui->NewLabel(this->tpGPSLocation, CSTR("GA In-view"));
	this->lblGPSNSateViewGA->SetRect(4, 316, 100, 23, false);
	this->txtGPSNSateViewGA = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSNSateViewGA->SetReadOnly(true);
	this->txtGPSNSateViewGA->SetRect(104, 316, 100, 23, false);
	this->lblGPSNSateViewQZSS = ui->NewLabel(this->tpGPSLocation, CSTR("QZSS In-view"));
	this->lblGPSNSateViewQZSS->SetRect(4, 340, 100, 23, false);
	this->txtGPSNSateViewQZSS = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSNSateViewQZSS->SetReadOnly(true);
	this->txtGPSNSateViewQZSS->SetRect(104, 340, 100, 23, false);
	this->lblGPSNSateViewBD = ui->NewLabel(this->tpGPSLocation, CSTR("BD In-view"));
	this->lblGPSNSateViewBD->SetRect(4, 364, 100, 23, false);
	this->txtGPSNSateViewBD = ui->NewTextBox(this->tpGPSLocation, CSTR(""));
	this->txtGPSNSateViewBD->SetReadOnly(true);
	this->txtGPSNSateViewBD->SetRect(104, 364, 100, 23, false);
	this->lvDashboard->AddItem(CSTR("GPS"), 0);
	this->lvDashboard->SetSubItem(2, 1, CSTR("Idle"));
	this->lvDashboard->SetSubItem(2, 2, CSTR("0/0"));

	this->wlanInterf = 0;
	this->wlanScan = 0;
	this->wlanLastTime = 0;
	this->bt = 0;
	this->btUpdated = false;
	this->btMinTime = 0;
	this->locSvc = 0;
	this->gpsRecUpdated = false;
	this->gpsLastUpdateTime = Data::DateTimeUtil::GetCurrTimeMillis();
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

	this->locSvc.Delete();
}

void SSWR::AVIRead::AVIRRadioScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
