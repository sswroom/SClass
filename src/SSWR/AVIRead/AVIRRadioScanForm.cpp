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
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall SSWR::AVIRead::AVIRRadioScanForm::CellularThread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	Data::Timestamp currTime;
	Data::Timestamp nextSignalTime;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Bool init = false;
	IO::GSMModemController::BER ber;
	NN<IO::ATCommandChannel> channel;
	NN<IO::GSMModemController> modem;
	NN<IO::HuaweiGSMModemController> huawei;
	NN<Text::String> s;

	nextSignalTime = Data::Timestamp::UtcNow();
	me->cellularRunning = true;
	if (me->cellularModem.SetTo(modem))
	{
		while (!me->cellularToStop)
		{
			if (!init)
			{
				init = true;
				OPTSTR_DEL(me->cellularModemManu);
				OPTSTR_DEL(me->cellularModemModel);
				OPTSTR_DEL(me->cellularModemVer);
				OPTSTR_DEL(me->cellularIMEI);
				OPTSTR_DEL(me->cellularHuaweiICCID);
				if (modem->GSMGetManufacturer(sbuff).SetTo(sptr))
				{
					s = Text::String::NewP(sbuff, sptr);
					me->cellularModemManu = s;
					if (s->StartsWith(UTF8STRC("Huawei")) && me->cellularChannel.SetTo(channel))
					{
						NN<IO::GSMModemController> oldModem;
						NEW_CLASSNN(huawei, IO::HuaweiGSMModemController(channel, false));
						me->cellularHuawei = huawei;
						oldModem = modem;
						me->cellularModem = huawei;
						oldModem.Delete();
//						huawei->HuaweiGetCardMode(&me->huaweiSIMType);
					}
				}
				if (modem->GSMGetModelIdent(sbuff).SetTo(sptr))
					me->cellularModemModel = Text::String::NewP(sbuff, sptr);
				if (modem->GSMGetModemVer(sbuff).SetTo(sptr))
					me->cellularModemVer = Text::String::NewP(sbuff, sptr);
				if (modem->GSMGetIMEI(sbuff).SetTo(sptr))
					me->cellularIMEI = Text::String::NewP(sbuff, sptr);
				if (me->cellularHuawei.SetTo(huawei) && huawei->HuaweiGetICCID(sbuff).SetTo(sptr))
					me->cellularHuaweiICCID = Text::String::NewP(sbuff, sptr);
				me->cellularInitStrs = true;

				if (modem->GSMGetTECharset(sbuff).SetTo(sptr))
				{
					OPTSTR_DEL(me->cellularTECharset);
					me->cellularTECharset = Text::String::NewP(sbuff, sptr);
					me->cellularTECharsetUpd = true;
				}
			}
			if (me->cellularSIMChanged)
			{
				me->cellularSIMChanged = false;
				if (modem->GSMGetIMSI(sbuff).SetTo(sptr))
				{
					OPTSTR_DEL(me->cellularIMSI);
					me->cellularIMSI = Text::String::NewP(sbuff, sptr);
				}
				me->cellularSIMInfoUpdated = true;
			}

			currTime = Data::Timestamp::UtcNow();
			if (currTime >= me->cellularOperNextTime)
			{
				me->cellularOperNextTime = me->cellularOperNextTime.AddSecond(30);
				if (modem->GSMGetCurrPLMN(sbuff).SetTo(sptr))
				{
					OPTSTR_DEL(me->cellularOperName);
					me->cellularOperName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
					me->cellularOperUpdated = true;
				}
				if (modem->GSMGetRegisterNetwork(me->cellularRegNetN, me->cellularRegNetStat, me->cellularRegNetLAC, me->cellularRegNetCI, me->cellularRegNetACT))
				{
					me->cellularRegNetUpdated = true;
				}
/*				if (me->huawei)
				{
					me->huaweiSysInfoUpdated = me->huawei->HuaweiGetSysInfoEx(&me->huaweiSysInfoSrvStatus,
						&me->huaweiSysInfoSrvDomain,
						&me->huaweiSysInfoRoamStatus,
						&me->huaweiSysInfoSIMState,
						&me->huaweiSysInfoLockState,
						&me->huaweiSysInfoSysMode,
						&me->huaweiSysInfoSubMode);
				}*/
			}
			if (currTime >= nextSignalTime)
			{
				nextSignalTime = nextSignalTime.AddSecond(10);
				modem->GSMGetSignalQuality(me->cellularSignalQuality, ber);
/*				if (me->huawei)
				{
					me->huaweiCSQUpdated = me->huawei->HuaweiGetSignalStrength(&me->huaweiCSQ);
				}*/
				me->cellularSignalUpdated = true;
			}
			me->cellularEvt.Wait(1000);
		}
	}
	me->cellularRunning = false;
	return 0;
}

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
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
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

	if (me->gnssRecUpdated)
	{
		me->gnssLastUpdateTime = Data::DateTimeUtil::GetCurrTimeMillis();
		Sync::MutexUsage mutUsage(me->gnssRecMut);
		sptr = Data::Timestamp(me->gnssRecCurr.recTime, Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
		me->txtGNSSTime->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->gnssRecCurr.pos.GetLat());
		me->txtGNSSLatitude->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->gnssRecCurr.pos.GetLon());
		me->txtGNSSLongitude->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->gnssRecCurr.altitude);
		me->txtGNSSAltitude->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->gnssRecCurr.speed);
		me->txtGNSSSpeed->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->gnssRecCurr.heading);
		me->txtGNSSHeading->SetText(CSTRP(sbuff, sptr));
		if (me->gnssRecCurr.valid)
			me->txtGNSSValid->SetText(CSTR("Valid"));
		else
			me->txtGNSSValid->SetText(CSTR("Invalid"));
		sptr = Text::StrInt32(sbuff, me->gnssRecCurr.nSateUsed);
		me->txtGNSSNSateUsed->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gnssRecCurr.nSateUsedGPS);
		me->txtGNSSNSateUsedGPS->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gnssRecCurr.nSateUsedSBAS);
		me->txtGNSSNSateUsedSBAS->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gnssRecCurr.nSateUsedGLO);
		me->txtGNSSNSateUsedGLO->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gnssRecCurr.nSateViewGPS);
		me->txtGNSSNSateViewGPS->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gnssRecCurr.nSateViewGLO);
		me->txtGNSSNSateViewGLO->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gnssRecCurr.nSateViewGA);
		me->txtGNSSNSateViewGA->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gnssRecCurr.nSateViewQZSS);
		me->txtGNSSNSateViewQZSS->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->gnssRecCurr.nSateViewBD);
		me->txtGNSSNSateViewBD->SetText(CSTRP(sbuff, sptr));

		me->lvGNSSSatellite->ClearItems();
		UOSInt actCnt = 0;
		UOSInt i = 0;
		while (i < me->gnssRecSateCnt)
		{
			me->lvGNSSSatellite->AddItem(Map::ILocationService::SateTypeGetName(me->gnssRecSates[i].sateType), 0);
			sptr = Text::StrUInt16(sbuff, me->gnssRecSates[i].prn);
			me->lvGNSSSatellite->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, me->gnssRecSates[i].elev);
			me->lvGNSSSatellite->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, me->gnssRecSates[i].azimuth);
			me->lvGNSSSatellite->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			if (me->gnssRecSates[i].snr >= 0)
			{
				sptr = Text::StrInt16(sbuff, me->gnssRecSates[i].snr);
				me->lvGNSSSatellite->SetSubItem(i, 4, CSTRP(sbuff, sptr));
				actCnt++;
			}
			i++;
		}
		sptr = Text::StrUOSInt(sbuff, actCnt);
		*sptr++ = '/';
		sptr = Text::StrUOSInt(sptr, me->gnssRecSateCnt);
		me->lvDashboard->SetSubItem(2, 2, CSTRP(sbuff, sptr));

		me->gnssRecUpdated = false;
	}
	else
	{
		NN<Map::ILocationService> locSvc;
		currTime = Data::DateTimeUtil::GetCurrTimeMillis();
		if (me->locSvc.SetTo(locSvc) && (currTime - me->gnssLastUpdateTime) >= 20000)
		{
			me->gnssLastUpdateTime = currTime;
			locSvc->ErrorRecover();
		}
	}

	if (me->cellularInitStrs)
	{
		me->cellularInitStrs = false;
		if (me->cellularModemManu.SetTo(s))
		{
			me->txtCellularManu->SetText(s->ToCString());
		}
		if (me->cellularModemModel.SetTo(s))
		{
			me->txtCellularModel->SetText(s->ToCString());
		}
		if (me->cellularModemVer.SetTo(s))
		{
			me->txtCellularVer->SetText(s->ToCString());
		}
		if (me->cellularIMEI.SetTo(s))
		{
			me->txtCellularIMEI->SetText(s->ToCString());
		}
/*		if (me->cellularHuawei.NotNull())
		{
			if (me->cellularHuaweiICCID.SetTo(s))
			{
				me->txtCellularHuaweiICCID->SetText(s->ToCString());
			}
			me->txtCellularHuaweiSIMType->SetText(IO::HuaweiGSMModemController::SIMCardTypeGetName(me->cellularHuaweiSIMType));
		}*/
	}
	if (me->cellularSIMInfoUpdated)
	{
		me->cellularSIMInfoUpdated = false;
		me->txtCellularIMSI->SetText(Text::String::OrEmpty(me->cellularIMSI)->ToCString());
	}
	if (me->cellularTECharsetUpd && me->cellularTECharset.SetTo(s))
	{
		me->cellularTECharsetUpd = false;
		me->txtCellularTECharset->SetText(s->ToCString());
	}

	if (me->cellularOperUpdated && me->cellularOperName.SetTo(s))
	{
		me->cellularOperUpdated = false;
		me->txtCellularOperator->SetText(s->ToCString());
	}

	if (me->cellularRegNetUpdated)
	{
		me->cellularRegNetUpdated = false;
		me->txtCellularRegStatus->SetText(IO::GSMModemController::RegisterStatusGetName(me->cellularRegNetStat));
		if (me->cellularRegNetN == IO::GSMModemController::NetworkResult::Enable_w_Location)
		{
			sptr = Text::StrUInt16(sbuff, me->cellularRegNetLAC);
			me->txtCellularLAC->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, me->cellularRegNetCI);
			me->txtCellularCI->SetText(CSTRP(sbuff, sptr));
			me->txtCellularACT->SetText(IO::GSMModemController::AccessTechGetName(me->cellularRegNetACT));
		}
	}
/*	if (me->huaweiSysInfoUpdated)
	{
		me->huaweiSysInfoUpdated = false;
		me->txtHuaweiSrvStatus->SetText(IO::HuaweiGSMModemController::ServiceStatusGetName(me->huaweiSysInfoSrvStatus));
		me->txtHuaweiSrvDomain->SetText(IO::HuaweiGSMModemController::ServiceDomainGetName(me->huaweiSysInfoSrvDomain));
		me->txtHuaweiRoamStatus->SetText(me->huaweiSysInfoRoamStatus?CSTR("Roaming"):CSTR("Not Roaming"));
		me->txtHuaweiSIMState->SetText(IO::HuaweiGSMModemController::SIMStateGetName(me->huaweiSysInfoSIMState));
		me->txtHuaweiLockState->SetText(me->huaweiSysInfoLockState?CSTR("Locked"):CSTR("Not locked"));
		me->txtHuaweiSysMode->SetText(IO::HuaweiGSMModemController::SysModeGetName(me->huaweiSysInfoSysMode));
		me->txtHuaweiSubMode->SetText(IO::HuaweiGSMModemController::SubModeGetName(me->huaweiSysInfoSubMode));
	}*/
	if (me->cellularSignalUpdated)
	{
		me->cellularSignalUpdated = false;
		sptr = IO::GSMModemController::RSSIGetName(sbuff, me->cellularSignalQuality);
		me->txtCellularSignalQuality->SetText(CSTRP(sbuff, sptr));
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
		me->ToggleGNSS();
	}
	else if (index == 3)
	{
		me->ToggleCellular();
	}
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnDeviceUpdated(NN<IO::BTScanLog::ScanRecord3> dev, IO::BTScanner::UpdateType updateType, AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	me->btUpdated = true;
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnGNSSPortClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	me->cboGNSSPort->ClearItems();
	IO::SerialPortUtil::FillPortSelector(me->cboGNSSPort, IO::Device::MTKGPSNMEA::GetMTKSerialPort());
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnGNSSClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	me->ToggleGNSS();
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnGNSSLocationUpdated(AnyType userObj, NN<Map::GPSTrack::GPSRecord3> record, Data::DataArray<Map::ILocationService::SateStatus> sates)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	Sync::MutexUsage mutUsage(me->gnssRecMut);
	MemCopyNO(&me->gnssRecCurr, record.Ptr(), sizeof(Map::GPSTrack::GPSRecord3));
	me->gnssRecSateCnt = sates.GetCount();
	MemCopyNO(me->gnssRecSates, sates.Arr().Ptr(), sates.GetCount() * sizeof(Map::ILocationService::SateStatus));
	me->gnssRecUpdated = true;
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnCellularPortClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	me->cboGNSSPort->ClearItems();
	IO::SerialPortUtil::FillPortSelector(me->cboGNSSPort, IO::Device::MTKGPSNMEA::GetMTKSerialPort());
}

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnCellularClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	me->ToggleCellular();
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

void SSWR::AVIRead::AVIRRadioScanForm::ToggleGNSS()
{
	if (this->locSvc.NotNull())
	{
		this->locSvc.Delete();
		this->btnGNSS->SetText(CSTR("Start"));
		this->lvDashboard->SetSubItem(2, 1, CSTR("Idle"));
		this->cboGNSSPort->SetEnabled(true);
		this->cboGNSSBaudRate->SetEnabled(true);
		this->cboGNSSParity->SetEnabled(true);
		this->chkGNSSFlowControl->SetEnabled(true);
		this->btnGNSSPort->SetEnabled(true);
	}
	else
	{
		NN<IO::SerialPort> stm;
		if (IO::SerialPortUtil::OpenSerialPort(this->cboGNSSPort, this->cboGNSSBaudRate, this->cboGNSSParity, this->chkGNSSFlowControl->IsChecked()).SetTo(stm))
		{
			NN<Map::ILocationService> locSvc;
			NEW_CLASSNN(locSvc, IO::GPSNMEA(stm, true));
			locSvc->RegisterLocationHandler(OnGNSSLocationUpdated, this);
			this->locSvc = locSvc;
			this->gnssLastUpdateTime = Data::DateTimeUtil::GetCurrTimeMillis();
			this->lvDashboard->SetSubItem(2, 1, CSTR("Scanning"));
			this->btnGNSS->SetText(CSTR("Stop"));
			this->cboGNSSPort->SetEnabled(false);
			this->cboGNSSBaudRate->SetEnabled(false);
			this->cboGNSSParity->SetEnabled(false);
			this->chkGNSSFlowControl->SetEnabled(false);
			this->btnGNSSPort->SetEnabled(false);
		}
		else
		{
			this->ui->ShowMsgOK(CSTR("Error in opening GPS Serial Port"), CSTR("Radio Scan"), this);
		}
	}
}

void SSWR::AVIRead::AVIRRadioScanForm::ToggleCellular()
{
	NN<IO::GSMModemController> modem;
	if (this->CloseCellular())
	{
		this->btnCellular->SetText(CSTR("Start"));
		this->lvDashboard->SetSubItem(3, 1, CSTR("Idle"));
		this->cboCellularPort->SetEnabled(true);
		this->cboCellularBaudRate->SetEnabled(true);
		this->cboCellularParity->SetEnabled(true);
		this->chkCellularFlowControl->SetEnabled(true);
		this->btnCellularPort->SetEnabled(true);
	}
	else
	{
		NN<IO::SerialPort> stm;
		if (IO::SerialPortUtil::OpenSerialPort(this->cboCellularPort, this->cboCellularBaudRate, this->cboCellularParity, this->chkCellularFlowControl->IsChecked()).SetTo(stm))
		{
			this->cellularPort = stm;

			NN<IO::ATCommandChannel> channel;
			NEW_CLASSNN(channel, IO::ATCommandChannel(stm, false));
			this->cellularChannel = channel;
			NEW_CLASSNN(modem, IO::GSMModemController(channel, false));
			this->cellularModem = modem;

			this->cellularSIMChanged = true;
			this->cellularToStop = false;
			this->cellularRunning = false;
			Sync::ThreadUtil::Create(CellularThread, this);
			while (!this->cellularRunning)
			{
				Sync::SimpleThread::Sleep(10);
			}

			this->lvDashboard->SetSubItem(3, 1, CSTR("Scanning"));
			this->btnCellular->SetText(CSTR("Stop"));
			this->cboCellularPort->SetEnabled(false);
			this->cboCellularBaudRate->SetEnabled(false);
			this->cboCellularParity->SetEnabled(false);
			this->chkCellularFlowControl->SetEnabled(false);
			this->btnCellularPort->SetEnabled(false);
		}
		else
		{
			this->ui->ShowMsgOK(CSTR("Error in opening Cellular Serial Port"), CSTR("Radio Scan"), this);
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

Bool SSWR::AVIRead::AVIRRadioScanForm::CloseCellular()
{
	NN<IO::Stream> port;
	NN<IO::ATCommandChannel> channel;
	NN<IO::GSMModemController> modem;
	if (this->cellularPort.SetTo(port) && this->cellularChannel.SetTo(channel) && this->cellularModem.SetTo(modem))
	{
		this->cellularToStop = true;
		this->cellularEvt.Set();
		port->Close();
		channel->Close();
		while (this->cellularRunning)
		{
			Sync::SimpleThread::Sleep(10);
		}

//		modem->SMSFreeMessages(this->msgList);

		this->cellularModem.Delete();
		this->cellularChannel.Delete();
		this->cellularPort.Delete();
		this->cellularHuawei = 0;
		return true;
	}
	return false;
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

	this->tpGNSS = this->tcMain->AddTabPage(CSTR("GNSS"));
	this->pnlGNSS = ui->NewPanel(this->tpGNSS);
	this->pnlGNSS->SetRect(0, 0, 100, 127, false);
	this->pnlGNSS->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblGNSSPort = ui->NewLabel(this->pnlGNSS, CSTR("Port"));
	this->lblGNSSPort->SetRect(4, 4, 100, 23, false);
	this->cboGNSSPort = ui->NewComboBox(this->pnlGNSS, false);
	this->cboGNSSPort->SetRect(104, 4, 150, 23, false);
	IO::SerialPortUtil::FillPortSelector(this->cboGNSSPort, IO::Device::MTKGPSNMEA::GetMTKSerialPort());
	this->btnGNSSPort = ui->NewButton(this->pnlGNSS, CSTR("Refresh"));
	this->btnGNSSPort->SetRect(254, 4, 75, 23, false);
	this->btnGNSSPort->HandleButtonClick(OnGNSSPortClicked, this);
	this->lblGNSSBaudRate = ui->NewLabel(this->pnlGNSS, CSTR("Baud Rate"));
	this->lblGNSSBaudRate->SetRect(4, 28, 100, 23, false);
	this->cboGNSSBaudRate = ui->NewComboBox(this->pnlGNSS, false);
	this->cboGNSSBaudRate->SetRect(104, 28, 100, 23, false);
	IO::SerialPortUtil::FillBaudRateSelector(this->cboGNSSBaudRate);
	this->lblGNSSParity = ui->NewLabel(this->pnlGNSS, CSTR("Parity"));
	this->lblGNSSParity->SetRect(4, 52, 100, 23, false);
	this->cboGNSSParity = ui->NewComboBox(this->pnlGNSS, false);
	this->cboGNSSParity->SetRect(104, 52, 100, 23, false);
	IO::SerialPortUtil::FillParitySelector(this->cboGNSSParity);
	this->lblGNSSFlowControl = ui->NewLabel(this->pnlGNSS, CSTR("Flow Control"));
	this->lblGNSSFlowControl->SetRect(4, 76, 100, 23, false);
	this->chkGNSSFlowControl = ui->NewCheckBox(this->pnlGNSS, CSTR("Enabled"), false);
	this->chkGNSSFlowControl->SetRect(104, 76, 100, 23, false);
	this->btnGNSS = ui->NewButton(this->pnlGNSS, CSTR("Start"));
	this->btnGNSS->SetRect(104, 100, 75, 23, false);
	this->btnGNSS->HandleButtonClick(OnGNSSClicked, this);
	this->tcGNSS = ui->NewTabControl(this->tpGNSS);
	this->tcGNSS->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpGNSSSatellite = this->tcGNSS->AddTabPage(CSTR("Satellite"));
	this->lvGNSSSatellite = ui->NewListView(this->tpGNSSSatellite, UI::ListViewStyle::Table, 5);
	this->lvGNSSSatellite->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvGNSSSatellite->AddColumn(CSTR("Type"), 80);
	this->lvGNSSSatellite->AddColumn(CSTR("PRN"), 60);
	this->lvGNSSSatellite->AddColumn(CSTR("Elevation"), 60);
	this->lvGNSSSatellite->AddColumn(CSTR("Azimuth"), 60);
	this->lvGNSSSatellite->AddColumn(CSTR("SNR"), 60);
	this->lvGNSSSatellite->SetShowGrid(true);
	this->lvGNSSSatellite->SetFullRowSelect(true);
	this->tpGNSSLocation = this->tcGNSS->AddTabPage(CSTR("Location"));
	this->lblGNSSTime = ui->NewLabel(this->tpGNSSLocation, CSTR("GPS Time"));
	this->lblGNSSTime->SetRect(4, 4, 100, 23, false);
	this->txtGNSSTime = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSTime->SetReadOnly(true);
	this->txtGNSSTime->SetRect(104, 4, 200, 23, false);
	this->lblGNSSLatitude = ui->NewLabel(this->tpGNSSLocation, CSTR("Latitude"));
	this->lblGNSSLatitude->SetRect(4, 28, 100, 23, false);
	this->txtGNSSLatitude = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSLatitude->SetReadOnly(true);
	this->txtGNSSLatitude->SetRect(104, 28, 120, 23, false);
	this->lblGNSSLongitude = ui->NewLabel(this->tpGNSSLocation, CSTR("Longitude"));
	this->lblGNSSLongitude->SetRect(4, 52, 100, 23, false);
	this->txtGNSSLongitude = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSLongitude->SetReadOnly(true);
	this->txtGNSSLongitude->SetRect(104, 52, 120, 23, false);
	this->lblGNSSAltitude = ui->NewLabel(this->tpGNSSLocation, CSTR("Altitude"));
	this->lblGNSSAltitude->SetRect(4, 76, 100, 23, false);
	this->txtGNSSAltitude = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSAltitude->SetReadOnly(true);
	this->txtGNSSAltitude->SetRect(104, 76, 100, 23, false);
	this->lblGNSSSpeed = ui->NewLabel(this->tpGNSSLocation, CSTR("Speed(knot)"));
	this->lblGNSSSpeed->SetRect(4, 100, 100, 23, false);
	this->txtGNSSSpeed = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSSpeed->SetReadOnly(true);
	this->txtGNSSSpeed->SetRect(104, 100, 100, 23, false);
	this->lblGNSSHeading = ui->NewLabel(this->tpGNSSLocation, CSTR("Heading"));
	this->lblGNSSHeading->SetRect(4, 124, 100, 23, false);
	this->txtGNSSHeading = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSHeading->SetReadOnly(true);
	this->txtGNSSHeading->SetRect(104, 124, 100, 23, false);
	this->lblGNSSValid = ui->NewLabel(this->tpGNSSLocation, CSTR("Valid"));
	this->lblGNSSValid->SetRect(4, 148, 100, 23, false);
	this->txtGNSSValid = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSValid->SetReadOnly(true);
	this->txtGNSSValid->SetRect(104, 148, 100, 23, false);
	this->lblGNSSNSateUsed = ui->NewLabel(this->tpGNSSLocation, CSTR("Sate Used"));
	this->lblGNSSNSateUsed->SetRect(4, 172, 100, 23, false);
	this->txtGNSSNSateUsed = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSNSateUsed->SetReadOnly(true);
	this->txtGNSSNSateUsed->SetRect(104, 172, 100, 23, false);
	this->lblGNSSNSateUsedGPS = ui->NewLabel(this->tpGNSSLocation, CSTR("GPS Used"));
	this->lblGNSSNSateUsedGPS->SetRect(4, 196, 100, 23, false);
	this->txtGNSSNSateUsedGPS = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSNSateUsedGPS->SetReadOnly(true);
	this->txtGNSSNSateUsedGPS->SetRect(104, 196, 100, 23, false);
	this->lblGNSSNSateUsedSBAS = ui->NewLabel(this->tpGNSSLocation, CSTR("SBAS Used"));
	this->lblGNSSNSateUsedSBAS->SetRect(4, 220, 100, 23, false);
	this->txtGNSSNSateUsedSBAS = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSNSateUsedSBAS->SetReadOnly(true);
	this->txtGNSSNSateUsedSBAS->SetRect(104, 220, 100, 23, false);
	this->lblGNSSNSateUsedGLO = ui->NewLabel(this->tpGNSSLocation, CSTR("GLO Used"));
	this->lblGNSSNSateUsedGLO->SetRect(4, 244, 100, 23, false);
	this->txtGNSSNSateUsedGLO = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSNSateUsedGLO->SetReadOnly(true);
	this->txtGNSSNSateUsedGLO->SetRect(104, 244, 100, 23, false);
	this->lblGNSSNSateViewGPS = ui->NewLabel(this->tpGNSSLocation, CSTR("GPS In-view"));
	this->lblGNSSNSateViewGPS->SetRect(4, 268, 100, 23, false);
	this->txtGNSSNSateViewGPS = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSNSateViewGPS->SetReadOnly(true);
	this->txtGNSSNSateViewGPS->SetRect(104, 268, 100, 23, false);
	this->lblGNSSNSateViewGLO = ui->NewLabel(this->tpGNSSLocation, CSTR("GLO In-view"));
	this->lblGNSSNSateViewGLO->SetRect(4, 292, 100, 23, false);
	this->txtGNSSNSateViewGLO = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSNSateViewGLO->SetReadOnly(true);
	this->txtGNSSNSateViewGLO->SetRect(104, 292, 100, 23, false);
	this->lblGNSSNSateViewGA = ui->NewLabel(this->tpGNSSLocation, CSTR("GA In-view"));
	this->lblGNSSNSateViewGA->SetRect(4, 316, 100, 23, false);
	this->txtGNSSNSateViewGA = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSNSateViewGA->SetReadOnly(true);
	this->txtGNSSNSateViewGA->SetRect(104, 316, 100, 23, false);
	this->lblGNSSNSateViewQZSS = ui->NewLabel(this->tpGNSSLocation, CSTR("QZSS In-view"));
	this->lblGNSSNSateViewQZSS->SetRect(4, 340, 100, 23, false);
	this->txtGNSSNSateViewQZSS = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSNSateViewQZSS->SetReadOnly(true);
	this->txtGNSSNSateViewQZSS->SetRect(104, 340, 100, 23, false);
	this->lblGNSSNSateViewBD = ui->NewLabel(this->tpGNSSLocation, CSTR("BD In-view"));
	this->lblGNSSNSateViewBD->SetRect(4, 364, 100, 23, false);
	this->txtGNSSNSateViewBD = ui->NewTextBox(this->tpGNSSLocation, CSTR(""));
	this->txtGNSSNSateViewBD->SetReadOnly(true);
	this->txtGNSSNSateViewBD->SetRect(104, 364, 100, 23, false);
	this->lvDashboard->AddItem(CSTR("GNSS"), 0);
	this->lvDashboard->SetSubItem(2, 1, CSTR("Idle"));
	this->lvDashboard->SetSubItem(2, 2, CSTR("0/0"));

	this->tpCellular = this->tcMain->AddTabPage(CSTR("Cellular"));
	this->pnlCellular = ui->NewPanel(this->tpCellular);
	this->pnlCellular->SetRect(0, 0, 100, 127, false);
	this->pnlCellular->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCellularPort = ui->NewLabel(this->pnlCellular, CSTR("Port"));
	this->lblCellularPort->SetRect(4, 4, 100, 23, false);
	this->cboCellularPort = ui->NewComboBox(this->pnlCellular, false);
	this->cboCellularPort->SetRect(104, 4, 150, 23, false);
	IO::SerialPortUtil::FillPortSelector(this->cboCellularPort, IO::Device::MTKGPSNMEA::GetMTKSerialPort());
	this->btnCellularPort = ui->NewButton(this->pnlCellular, CSTR("Refresh"));
	this->btnCellularPort->SetRect(254, 4, 75, 23, false);
	this->btnCellularPort->HandleButtonClick(OnCellularPortClicked, this);
	this->lblCellularBaudRate = ui->NewLabel(this->pnlCellular, CSTR("Baud Rate"));
	this->lblCellularBaudRate->SetRect(4, 28, 100, 23, false);
	this->cboCellularBaudRate = ui->NewComboBox(this->pnlCellular, false);
	this->cboCellularBaudRate->SetRect(104, 28, 100, 23, false);
	IO::SerialPortUtil::FillBaudRateSelector(this->cboCellularBaudRate);
	this->lblCellularParity = ui->NewLabel(this->pnlCellular, CSTR("Parity"));
	this->lblCellularParity->SetRect(4, 52, 100, 23, false);
	this->cboCellularParity = ui->NewComboBox(this->pnlCellular, false);
	this->cboCellularParity->SetRect(104, 52, 100, 23, false);
	IO::SerialPortUtil::FillParitySelector(this->cboCellularParity);
	this->lblCellularFlowControl = ui->NewLabel(this->pnlCellular, CSTR("Flow Control"));
	this->lblCellularFlowControl->SetRect(4, 76, 100, 23, false);
	this->chkCellularFlowControl = ui->NewCheckBox(this->pnlCellular, CSTR("Enabled"), false);
	this->chkCellularFlowControl->SetRect(104, 76, 100, 23, false);
	this->btnCellular = ui->NewButton(this->pnlCellular, CSTR("Start"));
	this->btnCellular->SetRect(104, 100, 75, 23, false);
	this->btnCellular->HandleButtonClick(OnCellularClicked, this);
	this->tcCellular = ui->NewTabControl(this->tpCellular);
	this->tcCellular->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpCellularInfo = this->tcCellular->AddTabPage(CSTR("Info"));
	this->lblCellularManu = ui->NewLabel(this->tpCellularInfo, CSTR("Manufacture"));
	this->lblCellularManu->SetRect(4, 4, 100, 23, false);
	this->txtCellularManu = ui->NewTextBox(this->tpCellularInfo, CSTR(""));
	this->txtCellularManu->SetReadOnly(true);
	this->txtCellularManu->SetRect(104, 4, 150, 23, false);
	this->lblCellularModel = ui->NewLabel(this->tpCellularInfo, CSTR("Model"));
	this->lblCellularModel->SetRect(4, 28, 100, 23, false);
	this->txtCellularModel = ui->NewTextBox(this->tpCellularInfo, CSTR(""));
	this->txtCellularModel->SetReadOnly(true);
	this->txtCellularModel->SetRect(104, 28, 150, 23, false);
	this->lblCellularVer = ui->NewLabel(this->tpCellularInfo, CSTR("Modem Ver"));
	this->lblCellularVer->SetRect(4, 52, 100, 23, false);
	this->txtCellularVer = ui->NewTextBox(this->tpCellularInfo, CSTR(""));
	this->txtCellularVer->SetReadOnly(true);
	this->txtCellularVer->SetRect(104, 52, 150, 23, false);
	this->lblCellularIMEI = ui->NewLabel(this->tpCellularInfo, CSTR("IMEI"));
	this->lblCellularIMEI->SetRect(4, 76, 100, 23, false);
	this->txtCellularIMEI = ui->NewTextBox(this->tpCellularInfo, CSTR(""));
	this->txtCellularIMEI->SetReadOnly(true);
	this->txtCellularIMEI->SetRect(104, 76, 150, 23, false);
	this->lblCellularIMSI = ui->NewLabel(this->tpCellularInfo, CSTR("IMSI"));
	this->lblCellularIMSI->SetRect(4, 100, 100, 23, false);
	this->txtCellularIMSI = ui->NewTextBox(this->tpCellularInfo, CSTR(""));
	this->txtCellularIMSI->SetReadOnly(true);
	this->txtCellularIMSI->SetRect(104, 100, 150, 23, false);
	this->lblCellularTECharset = ui->NewLabel(this->tpCellularInfo, CSTR("TE Charset"));
	this->lblCellularTECharset->SetRect(4, 124, 100, 23, false);
	this->txtCellularTECharset = ui->NewTextBox(this->tpCellularInfo, CSTR(""));
	this->txtCellularTECharset->SetReadOnly(true);
	this->txtCellularTECharset->SetRect(104, 124, 150, 23, false);
	this->lblCellularOperator = ui->NewLabel(this->tpCellularInfo, CSTR("Operator"));
	this->lblCellularOperator->SetRect(4, 148, 100, 23, false);
	this->txtCellularOperator = ui->NewTextBox(this->tpCellularInfo, CSTR(""));
	this->txtCellularOperator->SetReadOnly(true);
	this->txtCellularOperator->SetRect(104, 148, 150, 23, false);
	this->lblCellularRegStatus = ui->NewLabel(this->tpCellularInfo, CSTR("Register Status"));
	this->lblCellularRegStatus->SetRect(4, 172, 100, 23, false);
	this->txtCellularRegStatus = ui->NewTextBox(this->tpCellularInfo, CSTR(""));
	this->txtCellularRegStatus->SetReadOnly(true);
	this->txtCellularRegStatus->SetRect(104, 172, 150, 23, false);
	this->lblCellularLAC = ui->NewLabel(this->tpCellularInfo, CSTR("LAC"));
	this->lblCellularLAC->SetRect(4, 196, 100, 23, false);
	this->txtCellularLAC = ui->NewTextBox(this->tpCellularInfo, CSTR(""));
	this->txtCellularLAC->SetReadOnly(true);
	this->txtCellularLAC->SetRect(104, 196, 150, 23, false);
	this->lblCellularCI = ui->NewLabel(this->tpCellularInfo, CSTR("CI"));
	this->lblCellularCI->SetRect(4, 220, 100, 23, false);
	this->txtCellularCI = ui->NewTextBox(this->tpCellularInfo, CSTR(""));
	this->txtCellularCI->SetReadOnly(true);
	this->txtCellularCI->SetRect(104, 220, 150, 23, false);
	this->lblCellularACT = ui->NewLabel(this->tpCellularInfo, CSTR("Access Tech"));
	this->lblCellularACT->SetRect(4, 244, 100, 23, false);
	this->txtCellularACT = ui->NewTextBox(this->tpCellularInfo, CSTR(""));
	this->txtCellularACT->SetReadOnly(true);
	this->txtCellularACT->SetRect(104, 244, 150, 23, false);
	this->lblCellularSignalQuality = ui->NewLabel(this->tpCellularInfo, CSTR("Signal Quality"));
	this->lblCellularSignalQuality->SetRect(4, 268, 100, 23, false);
	this->txtCellularSignalQuality = ui->NewTextBox(this->tpCellularInfo, CSTR(""));
	this->txtCellularSignalQuality->SetReadOnly(true);
	this->txtCellularSignalQuality->SetRect(104, 268, 150, 23, false);
	this->lvDashboard->AddItem(CSTR("Cellular"), 0);
	this->lvDashboard->SetSubItem(3, 1, CSTR("Idle"));
	this->lvDashboard->SetSubItem(3, 2, CSTR("0"));

	this->wlanInterf = 0;
	this->wlanScan = 0;
	this->wlanLastTime = 0;
	this->bt = 0;
	this->btUpdated = false;
	this->btMinTime = 0;
	this->locSvc = 0;
	this->gnssRecUpdated = false;
	this->gnssLastUpdateTime = Data::DateTimeUtil::GetCurrTimeMillis();
	this->cellularPort = 0;
	this->cellularChannel = 0;
	this->cellularModem = 0;
	this->cellularHuawei = 0;
	this->cellularToStop = false;
	this->cellularRunning = false;
	this->cellularInitStrs = false;
	this->cellularModemManu = 0;
	this->cellularModemModel = 0;
	this->cellularModemVer = 0;
	this->cellularIMEI = 0;
	this->cellularHuaweiICCID = 0;
	this->cellularTECharsetUpd = false;
	this->cellularTECharset = 0;
	this->cellularSIMChanged = false;
	this->cellularSIMInfoUpdated = false;
	this->cellularIMSI = 0;
	this->cellularRegNetUpdated = false;
	this->cellularRegNetN = IO::GSMModemController::NetworkResult::Disable;
	this->cellularRegNetStat = IO::GSMModemController::RegisterStatus::Unknown;
	this->cellularRegNetLAC = 0;
	this->cellularRegNetCI = 0;
	this->cellularRegNetACT = IO::GSMModemController::AccessTech::GSM;
	this->cellularSignalUpdated = false;
	this->cellularSignalQuality = IO::GSMModemController::RSSI::RSSI_UNKNOWN;
	this->cellularOperUpdated = false;
	this->cellularOperName = 0;
	this->cellularOperNextTime = Data::Timestamp::UtcNow();

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
	this->CloseCellular();
	OPTSTR_DEL(this->cellularOperName);
	OPTSTR_DEL(this->cellularModemManu);
	OPTSTR_DEL(this->cellularModemModel);
	OPTSTR_DEL(this->cellularModemVer);
	OPTSTR_DEL(this->cellularIMEI);
	OPTSTR_DEL(this->cellularHuaweiICCID);
	OPTSTR_DEL(this->cellularIMSI);
	OPTSTR_DEL(this->cellularTECharset);
}

void SSWR::AVIRead::AVIRRadioScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
