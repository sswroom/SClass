#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "IO/PowerInfo.h"
#include "Manage/HiResClock.h"
#include "Math/Math_C.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRWifiCaptureForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureForm>();
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt64 imac;
	NN<BSSStatus> bsss;
	UInt8 id[8];
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UInt8> country;
	NN<IO::MotionDetectorAccelerometer> motion;
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	UInt64 maxIMAC;
	Int32 maxRSSI;
	Text::StringBuilderUTF8 sb;
	NN<IO::Writer> captureWriter;
	IO::PowerInfo::PowerStatus power;
	if (IO::PowerInfo::GetPowerStatus(&power))
	{
		if (power.hasBattery)
		{
			sptr = Text::StrConcatC(Text::StrUInt32(sbuff, power.batteryPercent), UTF8STRC("%"));
			me->txtBattery->SetText(CSTRP(sbuff, sptr));
			if (power.batteryPercent <= 15 && me->captureWriter.NotNull())
			{
				Sync::MutexUsage mutUsage(me->captureMut);
				me->captureWriter.Delete();
				me->captureFS.Delete();
				mutUsage.EndUse();
				me->btnGPS->SetEnabled(true);
			}
		}
		else
		{
			me->txtBattery->SetText(CSTR("Battery not found"));
		}
	}
	else
	{
		me->txtBattery->SetText(CSTR("Cannot read battery status"));
	}
	if (me->motion.SetTo(motion))
	{
		motion->UpdateStatus();
		if (motion->IsMovving())
		{
			me->txtMotion->SetText(CSTR("Moving"));
			Sync::MutexUsage mutUsage(me->captureMut);
			if (me->captureWriter.SetTo(captureWriter) && me->lastMotion != 1)
			{
				me->lastMotion = 1;
				sb.ClearStr();
				sb.AppendTSNoZone(ts);
				sb.AppendC(UTF8STRC("\tMotion:Moving"));
				captureWriter->WriteLine(sb.ToCString());
			}
			mutUsage.EndUse();
		}
		else
		{
			me->txtMotion->SetText(CSTR("Stopped"));
			Sync::MutexUsage mutUsage(me->captureMut);
			if (me->captureWriter.SetTo(captureWriter) && me->lastMotion != 0)
			{
				me->lastMotion = 0;
				sb.ClearStr();
				sb.AppendTSNoZone(ts);
				sb.AppendC(UTF8STRC("\tMotion:Stopped"));
				captureWriter->WriteLine(sb.ToCString());
			}
			mutUsage.EndUse();
		}
	}
	if (me->gpsChg)
	{
		me->gpsChg = false;
		sptr = Text::StrDouble(sbuff, me->currPos.GetLat());
		me->txtGPSLat->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->currPos.GetLon());
		me->txtGPSLon->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->currAlt);
		me->txtGPSAlt->SetText(CSTRP(sbuff, sptr));
		sptr = Data::Timestamp(me->currGPSTime, 0).ToStringNoZone(sbuff);
		me->txtGPSTime->SetText(CSTRP(sbuff, sptr));
		if (me->currActive)
		{
			me->txtGPSActive->SetText(CSTR("Active"));
		}
		else
		{
			me->txtGPSActive->SetText(CSTR("Void"));
		}
	}
	NN<Net::WirelessLAN::Interface> wlanInterf;
	if (me->wlanInterf.SetTo(wlanInterf))
	{
		ts = Data::Timestamp::UtcNow();
		if (ts.ToTicks() - me->lastTimeTick > 900)
		{
			if (me->wlanScan-- <= 0)
			{
				NN<Text::String> s;
				NN<Text::String> ssid;
				Bool bssListUpd = false;
				Data::ArrayListNN<Net::WirelessLAN::BSSInfo> bssList;
				NN<Net::WirelessLAN::BSSInfo> bss;
				wlanInterf->GetBSSList(bssList);
				me->lvCurrWifi->ClearItems();

				maxIMAC = 0;
				maxRSSI = -128;
				Sync::MutexUsage mutUsage(me->captureMut);
				if (me->captureWriter.SetTo(captureWriter))
				{
					sb.ClearStr();
					sb.AppendTSNoZone(ts);
					sb.AppendC(UTF8STRC("\tWLAN:"));
					i = 0;
					j = bssList.GetCount();
					while (i < j)
					{
						bss = bssList.GetItemNoCheck(i);

						sb.AppendUTF8Char('|');
						sb.AppendHexBuff(bss->GetMAC(), 6, ':', Text::LineBreakType::None);
						sb.AppendUTF8Char('|');
						sb.Append(bss->GetSSID());
						sb.AppendUTF8Char('|');
						sb.AppendDouble(bss->GetRSSI());
						sb.AppendUTF8Char('|');
						sb.AppendDouble(bss->GetFreq() / 1000000.0);
						i++;
					}
					captureWriter->WriteLine(sb.ToCString());
					me->ui->UseDevice(true, false);
				}
				mutUsage.EndUse();

				i = 0;
				j = bssList.GetCount();
				while (i < j)
				{
					bss = bssList.GetItemNoCheck(i);
					ssid = bss->GetSSID();
					MemCopyNO(&id[0], bss->GetMAC().Ptr(), 6);
					id[6] = 0;
					id[7] = 0;
					imac = ReadMUInt64(id);

					k = me->lvCurrWifi->AddItem(ssid, 0);
					sptr = Text::StrUInt32(sbuff, bss->GetPHYId());
					me->lvCurrWifi->SetSubItem(k, 1, CSTRP(sbuff, sptr));
					sptr = Text::StrHexBytes(sbuff, &id[0], 6, ':');
					me->lvCurrWifi->SetSubItem(k, 2, CSTRP(sbuff, sptr));
					sptr = Text::StrInt32(sbuff, bss->GetBSSType());
					NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMAC64Info(imac);
					me->lvCurrWifi->SetSubItem(k, 3, {entry->name, entry->nameLen});
					me->lvCurrWifi->SetSubItem(k, 4, CSTRP(sbuff, sptr));
					sptr = Text::StrInt32(sbuff, bss->GetPHYType());
					me->lvCurrWifi->SetSubItem(k, 5, CSTRP(sbuff, sptr));
					sptr = Text::StrDouble(sbuff, bss->GetRSSI());
					me->lvCurrWifi->SetSubItem(k, 6, CSTRP(sbuff, sptr));
					sptr = Text::StrUInt32(sbuff, bss->GetLinkQuality());
					me->lvCurrWifi->SetSubItem(k, 7, CSTRP(sbuff, sptr));
					sptr = Text::StrDouble(sbuff, bss->GetFreq());
					me->lvCurrWifi->SetSubItem(k, 8, CSTRP(sbuff, sptr));
					if (bss->GetManuf().SetTo(s))
						me->lvCurrWifi->SetSubItem(k, 9, s);
					if (bss->GetModel().SetTo(s))
						me->lvCurrWifi->SetSubItem(k, 10, s);
					if (bss->GetSN().SetTo(s))
						me->lvCurrWifi->SetSubItem(k, 11, s);
					if (maxRSSI < bss->GetRSSI())
					{
						maxRSSI = Double2Int32(bss->GetRSSI());
						maxIMAC = imac;
					}

					NN<SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog> wifiLog;
					const UInt8 tmpOUI[] = {0, 0, 0};
					UnsafeArray<const UInt8> oui1 = bss->GetChipsetOUI(0).Or(tmpOUI);
					UnsafeArray<const UInt8> oui2 = bss->GetChipsetOUI(1).Or(tmpOUI);
					UnsafeArray<const UInt8> oui3 = bss->GetChipsetOUI(2).Or(tmpOUI);
					if (!me->wifiLogMap.Get(imac).SetTo(wifiLog))
					{
						wifiLog = MemAllocNN(SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog);
						MemCopyNO(wifiLog->mac, &id[0], 6);
						wifiLog->ssid = ssid->Clone();
						wifiLog->phyType = bss->GetPHYType();
						wifiLog->freq = bss->GetFreq();
						wifiLog->manuf = Text::String::CopyOrNull(bss->GetManuf());
						wifiLog->model = Text::String::CopyOrNull(bss->GetModel());
						wifiLog->serialNum = Text::String::CopyOrNull(bss->GetSN());
						wifiLog->country = Text::String::NewOrNullSlow(bss->GetCountry());
						wifiLog->ouis[0][0] = oui1[0];
						wifiLog->ouis[0][1] = oui1[1];
						wifiLog->ouis[0][2] = oui1[2];
						wifiLog->ouis[1][0] = oui2[0];
						wifiLog->ouis[1][1] = oui2[1];
						wifiLog->ouis[1][2] = oui2[2];
						wifiLog->ouis[2][0] = oui3[0];
						wifiLog->ouis[2][1] = oui3[1];
						wifiLog->ouis[2][2] = oui3[2];
						me->wifiLogMap.Put(imac, wifiLog);

						sptr = Text::StrHexBytes(sbuff, &id[0], 6, ':');
						k = me->lvLogWifi->InsertItem((UOSInt)me->wifiLogMap.GetIndex(imac), CSTRP(sbuff, sptr), wifiLog);
						NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMAC64Info(imac);
						me->lvLogWifi->SetSubItem(k, 1, {entry->name, entry->nameLen});
						me->lvLogWifi->SetSubItem(k, 2, wifiLog->ssid);
						sptr = Text::StrInt32(sbuff, wifiLog->phyType);
						me->lvLogWifi->SetSubItem(k, 3, CSTRP(sbuff, sptr));
						sptr = Text::StrDouble(sbuff, wifiLog->freq);
						me->lvLogWifi->SetSubItem(k, 4, CSTRP(sbuff, sptr));
						if (wifiLog->manuf.SetTo(s))
							me->lvLogWifi->SetSubItem(k, 5, s);
						if (wifiLog->model.SetTo(s))
							me->lvLogWifi->SetSubItem(k, 6, s);
						if (wifiLog->serialNum.SetTo(s))
							me->lvLogWifi->SetSubItem(k, 7, s);
						if (wifiLog->country.SetTo(s))
							me->lvLogWifi->SetSubItem(k, 8, s);
						if (wifiLog->ouis[0][0] != 0 || wifiLog->ouis[0][1] != 0 || wifiLog->ouis[0][2] != 0)
						{
							NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[0]);
							me->lvLogWifi->SetSubItem(k, 9, {entry->name, entry->nameLen});
						}
						if (wifiLog->ouis[1][0] != 0 || wifiLog->ouis[1][1] != 0 || wifiLog->ouis[1][2] != 0)
						{
							NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[1]);
							me->lvLogWifi->SetSubItem(k, 10, {entry->name, entry->nameLen});
						}
						if (wifiLog->ouis[2][0] != 0 || wifiLog->ouis[2][1] != 0 || wifiLog->ouis[2][2] != 0)
						{
							NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[2]);
							me->lvLogWifi->SetSubItem(k, 11, {entry->name, entry->nameLen});
						}
					}
					else
					{
						k = (UOSInt)me->wifiLogMap.GetIndex(imac);
						if (wifiLog->manuf.IsNull() && bss->GetManuf().SetTo(s))
						{
							wifiLog->manuf = s->Clone();
							me->lvLogWifi->SetSubItem(k, 5, s);
						}
						if (wifiLog->model.IsNull() && bss->GetModel().SetTo(s))
						{
							wifiLog->model = s->Clone();
							me->lvLogWifi->SetSubItem(k, 6, s);
						}
						if (wifiLog->serialNum.IsNull() && bss->GetSN().SetTo(s))
						{
							wifiLog->serialNum = s->Clone();
							me->lvLogWifi->SetSubItem(k, 7, s);
						}
						if (wifiLog->country.IsNull() && bss->GetCountry().SetTo(country))
						{
							s = Text::String::NewNotNullSlow(country);
							wifiLog->country = s;
							me->lvLogWifi->SetSubItem(k, 8, s);
						}
						OSInt l;
						UnsafeArray<const UInt8> oui;
						oui = oui1;
						if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
						{
							l = 0;
							while (l < 3)
							{
								if (wifiLog->ouis[l][0] == oui[0] && wifiLog->ouis[l][1] == oui[1] && wifiLog->ouis[l][2] == oui[2])
								{
									break;
								}
								else if (wifiLog->ouis[l][0] == 0 && wifiLog->ouis[l][1] == 0 && wifiLog->ouis[l][2] == 0)
								{
									wifiLog->ouis[l][0] = oui[0];
									wifiLog->ouis[l][1] = oui[1];
									wifiLog->ouis[l][2] = oui[2];
									NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[l]);
									me->lvLogWifi->SetSubItem(k, 9, {entry->name, entry->nameLen});
								}
								l++;
							}
						}

						oui = oui2;
						if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
						{
							l = 0;
							while (l < 3)
							{
								if (wifiLog->ouis[l][0] == oui[0] && wifiLog->ouis[l][1] == oui[1] && wifiLog->ouis[l][2] == oui[2])
								{
									break;
								}
								else if (wifiLog->ouis[l][0] == 0 && wifiLog->ouis[l][1] == 0 && wifiLog->ouis[l][2] == 0)
								{
									wifiLog->ouis[l][0] = oui[0];
									wifiLog->ouis[l][1] = oui[1];
									wifiLog->ouis[l][2] = oui[2];
									NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[l]);
									me->lvLogWifi->SetSubItem(k, 10, {entry->name, entry->nameLen});
								}
								l++;
							}
						}

						oui = oui3;
						if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
						{
							l = 0;
							while (l < 3)
							{
								if (wifiLog->ouis[l][0] == oui[0] && wifiLog->ouis[l][1] == oui[1] && wifiLog->ouis[l][2] == oui[2])
								{
									break;
								}
								else if (wifiLog->ouis[l][0] == 0 && wifiLog->ouis[l][1] == 0 && wifiLog->ouis[l][2] == 0)
								{
									wifiLog->ouis[l][0] = oui[0];
									wifiLog->ouis[l][1] = oui[1];
									wifiLog->ouis[l][2] = oui[2];
									NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[l]);
									me->lvLogWifi->SetSubItem(k, 11, {entry->name, entry->nameLen});
								}
								l++;
							}
						}
					}

					WriteInt16(&id[6], Double2Int32(bss->GetFreq() / 1000000.0));
					if (!me->bssMap.Get(ReadUInt64(id)).SetTo(bsss))
					{
						bssListUpd = true;
						bsss = MemAllocNN(BSSStatus);
						bsss->bssType = bss->GetBSSType();
						bsss->phyType = bss->GetPHYType();
						bsss->freq = bss->GetFreq();
						MemCopyNO(bsss->mac, &id[0], 6);
						bsss->ssid = ssid->Clone();
						me->bssMap.Put(ReadUInt64(id), bsss);
					}

					i++;
				}

				NN<SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog> wifiLog;
				if (maxRSSI >= -60 && maxRSSI < 0 && me->wifiLogMap.Get(maxIMAC).SetTo(wifiLog))
				{
					i = 0;
					j = bssList.GetCount();
					while (i < j)
					{
						bss = bssList.GetItemNoCheck(i);
						ssid = bss->GetSSID();
						MemCopyNO(&id[0], bss->GetMAC().Ptr(), 6);
						id[6] = 0;
						id[7] = 0;
						imac = ReadMUInt64(id);
						if (imac != maxIMAC)
						{
							Bool found = false;
							Int32 minRSSI;
							UOSInt minIndex;
							Int32 rssi1 = Double2Int32(bss->GetRSSI());
							minRSSI = 0;
							minIndex = 0;
							k = 0;
							while (k < 20)
							{
								Int8 rssi2 = (Int8)(wifiLog->neighbour[k] & 0xff);
								if ((wifiLog->neighbour[k] & 0xffffffffffff0000LL) == imac)
								{
									found = true;
									if (rssi1 > rssi2)
									{
										wifiLog->neighbour[k] = imac | ((UInt64)rssi1 & 0xff) | (((UInt64)bss->GetLinkQuality()) << 8);
									}
									break;
								}
								else if (wifiLog->neighbour[k] == 0)
								{
									wifiLog->neighbour[k] = imac | ((UInt64)rssi1 & 0xff) | (((UInt64)bss->GetLinkQuality()) << 8);
									found = true;
									break;
								}
								else if (rssi2 < minRSSI)
								{
									minRSSI = rssi2;
									minIndex = k;
								}
								
								k++;
							}

							if (!found && minRSSI < rssi1)
							{
								wifiLog->neighbour[minIndex] = imac | ((UInt64)rssi1 & 0xff) | (((UInt64)bss->GetLinkQuality()) << 8);
							}
						}
						i++;
					}
				}

				i = bssList.GetCount();
				while (i-- > 0)
				{
					bss = bssList.GetItemNoCheck(i);
					bss.Delete();
				}

				sptr = Text::StrUOSInt(sbuff, j);
				me->txtCurrWifiCnt->SetText(CSTRP(sbuff, sptr));
				wlanInterf->Scan();

				me->wlanScan = 10;

				if (bssListUpd)
				{
					sptr = Text::StrUOSInt(sbuff, me->bssMap.GetCount());
					me->txtBSSCount->SetText(CSTRP(sbuff, sptr));
				}
			}
			me->lastTimeTick = Data::DateTimeUtil::GetCurrTimeMillis();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnGPSClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureForm>();
	NN<Map::LocationService> locSvc;
	if (me->locSvc.SetTo(locSvc))
	{
		locSvc->UnregisterLocationHandler(OnGPSData, me);
		if (me->locSvcRel)
		{
			locSvc.Delete();
		}
		me->locSvc = 0;
		me->locSvcRel = false;
		me->txtGPS->SetText(CSTR("Not opened"));
	}
	else
	{
		IO::StreamType st;
		NN<IO::Stream> stm;
		if (me->core->OpenStream(st, me, 0, true).SetTo(stm))
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(stm->GetSourceNameObj());
			me->txtGPS->SetText(sb.ToCString());
			me->locSvcRel = true;
			NEW_CLASSNN(locSvc, IO::GPSNMEA(stm, true));
			me->locSvc = locSvc;
			locSvc->RegisterLocationHandler(OnGPSData, me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnCaptureClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureForm>();
	if (me->captureWriter.NotNull())
	{
		Sync::MutexUsage mutUsage(me->captureMut);
		me->captureWriter.Delete();
		me->captureFS.Delete();
		me->captureWriter = 0;
		me->captureFS = 0;
		mutUsage.EndUse();
		me->btnGPS->SetEnabled(true);
	}
	else
	{
		if (me->motion.IsNull())
		{
			me->ui->ShowMsgOK(CSTR("Accelerator not found"), CSTR("Error"), me);
			return;
		}
		else if (me->locSvc.IsNull())
		{
			me->ui->ShowMsgOK(CSTR("GPS not connected"), CSTR("Error"), me);
			return;
		}
		else if (me->wlanInterf.IsNull())
		{
			me->ui->ShowMsgOK(CSTR("Wifi adapter not found"), CSTR("Error"), me);
			return;
		}

		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		Bool isError;
		Data::DateTime dt;
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("Wifi"));
		IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		dt.SetCurrTimeUTC();
		sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));
		Sync::MutexUsage mutUsage(me->captureMut);
		NN<IO::FileStream> fs;
		NEW_CLASSNN(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		me->captureFS = fs;
		if (fs->IsError())
		{
			fs.Delete();
			me->captureFS = 0;
			isError = true;
		}
		else
		{
			NEW_CLASSOPT(me->captureWriter, Text::UTF8Writer(fs));
			me->lastMotion = -1;
			me->currActive = true;
			isError = false;
		}
		mutUsage.EndUse();
		if (!isError)
		{
			me->btnGPS->SetEnabled(false);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnLogWifiDblClicked(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureForm>();
	NN<Text::String> s;
	if (me->lvLogWifi->GetItemTextNew(index).SetTo(s))
	{
		UI::Clipboard::SetString(me->GetHandle(), s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnLogWifiSaveClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureForm>();
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Data::DateTime dt;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];
	dt.SetCurrTime();
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));
	Bool succ = false;
	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			succ = true;
			Text::UTF8Writer writer(fs);
			writer.WriteSignature();
			NN<SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog> wifiLog;
			i = 0;
			j = me->wifiLogMap.GetCount();
			while (i < j)
			{
				wifiLog = me->wifiLogMap.GetItemNoCheck(i);
				sb.ClearStr();
				sb.AppendHexBuff(wifiLog->mac, 6, ':', Text::LineBreakType::None);
				sb.AppendC(UTF8STRC("\t"));
				sb.Append(wifiLog->ssid);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendI32(wifiLog->phyType);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendDouble(wifiLog->freq);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendOpt(wifiLog->manuf);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendOpt(wifiLog->model);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendOpt(wifiLog->serialNum);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendHexBuff(wifiLog->ouis[0], 3, 0, Text::LineBreakType::None);
				sb.AppendUTF8Char(',');
				sb.AppendHexBuff(wifiLog->ouis[1], 3, 0, Text::LineBreakType::None);
				sb.AppendUTF8Char(',');
				sb.AppendHexBuff(wifiLog->ouis[2], 3, 0, Text::LineBreakType::None);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendOpt(wifiLog->country);
				sb.AppendC(UTF8STRC("\t"));
				k = 0;
				while (k < 20)
				{
					if (wifiLog->neighbour[k] == 0)
						break;
					if (k > 0)
					{
						sb.AppendUTF8Char(',');
					}
					sb.AppendHex64(wifiLog->neighbour[k]);
					k++;
				}
				if (!writer.WriteLine(sb.ToCString()))
				{
					succ = false;
				}

				i++;
			}
		}
	}
	if (succ)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("File saved to "));
		sb.AppendP(sbuff, sptr);
		me->ui->ShowMsgOK(sb.ToCString(), CSTR("Save"), me);
	}
	else
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Error in saving to "));
		sb.AppendP(sbuff, sptr);
		me->ui->ShowMsgOK(sb.ToCString(), CSTR("Save"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnLogWifiSaveFClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureForm>();
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UInt8 macBuff[8];
	Data::DateTime dt;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];
	dt.SetCurrTime();
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));
	Bool succ = false;
	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			succ = true;
			Text::UTF8Writer writer(fs);
			writer.WriteSignature();
			NN<SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog> wifiLog;
			i = 0;
			j = me->wifiLogMap.GetCount();
			while (i < j)
			{
				wifiLog = me->wifiLogMap.GetItemNoCheck(i);
				MemCopyNO(&macBuff[0], wifiLog->mac, 6);
				macBuff[6] = 0;
				macBuff[7] = 0;
				NN<const Net::MACInfo::MACEntry> ent = Net::MACInfo::GetMAC64Info(ReadMUInt64(macBuff));
				if (Text::StrEqualsC(ent->name, ent->nameLen, UTF8STRC("Unknown")))
				{
					sb.ClearStr();
					sb.AppendHexBuff(wifiLog->mac, 6, ':', Text::LineBreakType::None);
					sb.AppendC(UTF8STRC("\t"));
					sb.Append(wifiLog->ssid);
					sb.AppendC(UTF8STRC("\t"));
					sb.AppendI32(wifiLog->phyType);
					sb.AppendC(UTF8STRC("\t"));
					sb.AppendDouble(wifiLog->freq);
					if (!writer.WriteLine(sb.ToCString()))
					{
						succ = false;
					}
				}

				i++;
			}
		}
	}
	if (succ)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("File saved to "));
		sb.AppendP(sbuff, sptr);
		me->ui->ShowMsgOK(sb.ToCString(), CSTR("Save Unk"), me);
	}
	else
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Error in saving to "));
		sb.AppendP(sbuff, sptr);
		me->ui->ShowMsgOK(sb.ToCString(), CSTR("Save Unk"), me);
	}
}

Bool __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnFormClosing(AnyType userObj, CloseReason reason)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureForm>();
	Manage::HiResClock clk;
	if (me->ui->ShowMsgYesNo(CSTR("Are you sure to close?"), CSTR("Question"), me))
	{
		if (clk.GetTimeDiff() < 30)
		{
			return false;
		}
	}
	return true;
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnGPSData(AnyType userObj, NN<Map::GPSTrack::GPSRecord3> record, Data::DataArray<Map::LocationService::SateStatus> sates)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureForm>();
	NN<IO::Writer> captureWriter;
	if (me->currActive || record->valid != 0)
	{
		Sync::MutexUsage mutUsage(me->captureMut);
		if (me->captureWriter.SetTo(captureWriter))
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendTSNoZone(Data::Timestamp::UtcNow());
			sb.AppendC(UTF8STRC("\tGPS:"));
			if (record->valid == 0)
			{
				sb.AppendC(UTF8STRC("Void"));
			}
			else
			{
				sb.AppendC(UTF8STRC("Active,"));
				sb.AppendDouble(record->pos.GetLat());
				sb.AppendC(UTF8STRC(","));
				sb.AppendDouble(record->pos.GetLon());
				sb.AppendC(UTF8STRC(","));
				sb.AppendDouble(record->altitude);
			}
			captureWriter->WriteLine(sb.ToCString());
		}
		mutUsage.EndUse();
	}

	me->currGPSTime = record->recTime;
	me->currPos = record->pos;
	me->currAlt = record->altitude;
	me->currActive = record->valid != 0;
	me->gpsChg = true;
}

SSWR::AVIRead::AVIRWifiCaptureForm::AVIRWifiCaptureForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Wifi Capture"));

	this->core = core;
	this->motion = 0;
	this->gpsChg = false;
	this->lastTimeTick = 0;
	this->currGPSTime = Data::TimeInstant(0, 0);
	this->currPos = Math::Coord2DDbl(0, 0);
	this->currAlt = 0;
	this->locSvc = 0;
	this->locSvcRel = false;
	this->captureFS = 0;
	this->captureWriter = 0;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->sensorMgr.GetSensorCnt();
	while (i < j)
	{
		if (this->sensorMgr.GetSensorType(i) == IO::Sensor::SensorType::Accelerometer)
		{
			NN<IO::Sensor> sensor;
			NN<IO::SensorAccelerometer> acc;
			if (this->sensorMgr.CreateSensor(i).SetTo(sensor) && sensor->GetSensorAccelerator().SetTo(acc))
			{
				NEW_CLASSOPT(this->motion, IO::MotionDetectorAccelerometer(acc, true));
				break;
			}
		}
		i++;
	}
	NEW_CLASSNN(this->wlan, Net::WirelessLAN());
	Data::ArrayListNN<Net::WirelessLAN::Interface> interfList;
	this->wlan->GetInterfaces(interfList);
	this->wlanInterf = interfList.GetItem(0);
	i = interfList.GetCount();
	while (i-- > 1)
	{
		NN<Net::WirelessLAN::Interface> interf = interfList.GetItemNoCheck(i);
		interf.Delete();
	}
	this->wlanScan = 0;

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->lblMotion = ui->NewLabel(this->tpStatus, CSTR("Motion"));
	this->lblMotion->SetRect(4, 4, 100, 23, false);
	this->txtMotion = ui->NewTextBox(this->tpStatus, CSTR("No Accelerator found"));
	this->txtMotion->SetReadOnly(true);
	this->txtMotion->SetRect(104, 4, 200, 23, false);
	this->lblGPS = ui->NewLabel(this->tpStatus, CSTR("GPS"));
	this->lblGPS->SetRect(4, 28, 100, 23, false);
	this->txtGPS = ui->NewTextBox(this->tpStatus, CSTR("Not opened"));
	this->txtGPS->SetReadOnly(true);
	this->txtGPS->SetRect(104, 28, 200, 23, false);
	this->btnGPS = ui->NewButton(this->tpStatus, CSTR("Open"));
	this->btnGPS->SetRect(304, 28, 75, 23, false);
	this->btnGPS->HandleButtonClick(OnGPSClicked, this);
	this->lblGPSTime = ui->NewLabel(this->tpStatus, CSTR("GPS Time"));
	this->lblGPSTime->SetRect(4, 52, 100, 23, false);
	this->txtGPSTime = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtGPSTime->SetReadOnly(true);
	this->txtGPSTime->SetRect(104, 52, 200, 23, false);
	this->lblGPSLat = ui->NewLabel(this->tpStatus, CSTR("Latitude"));
	this->lblGPSLat->SetRect(4, 76, 100, 23, false);
	this->txtGPSLat = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtGPSLat->SetReadOnly(true);
	this->txtGPSLat->SetRect(104, 76, 200, 23, false);
	this->lblGPSLon = ui->NewLabel(this->tpStatus, CSTR("Longitude"));
	this->lblGPSLon->SetRect(4, 100, 100, 23, false);
	this->txtGPSLon = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtGPSLon->SetReadOnly(true);
	this->txtGPSLon->SetRect(104, 100, 200, 23, false);
	this->lblGPSAlt = ui->NewLabel(this->tpStatus, CSTR("Altitude"));
	this->lblGPSAlt->SetRect(4, 124, 100, 23, false);
	this->txtGPSAlt = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtGPSAlt->SetReadOnly(true);
	this->txtGPSAlt->SetRect(104, 124, 200, 23, false);
	this->lblGPSActive = ui->NewLabel(this->tpStatus, CSTR("GPS Active"));
	this->lblGPSActive->SetRect(4, 148, 100, 23, false);
	this->txtGPSActive = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtGPSActive->SetReadOnly(true);
	this->txtGPSActive->SetRect(104, 148, 200, 23, false);
	this->lblBattery = ui->NewLabel(this->tpStatus, CSTR("Battery"));
	this->lblBattery->SetRect(4, 172, 100, 23, false);
	this->txtBattery = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtBattery->SetReadOnly(true);
	this->txtBattery->SetRect(104, 172, 200, 23, false);
	this->lblCurrWifiCnt = ui->NewLabel(this->tpStatus, CSTR("Wifi Count"));
	this->lblCurrWifiCnt->SetRect(4, 196, 100, 23, false);
	this->txtCurrWifiCnt = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtCurrWifiCnt->SetReadOnly(true);
	this->txtCurrWifiCnt->SetRect(104, 196, 200, 23, false);
	this->lblBSSCount = ui->NewLabel(this->tpStatus, CSTR("BSS Count"));
	this->lblBSSCount->SetRect(4, 220, 100, 23, false);
	this->txtBSSCount = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtBSSCount->SetReadOnly(true);
	this->txtBSSCount->SetRect(104, 220, 200, 23, false);
	this->btnCapture = ui->NewButton(this->tpStatus, CSTR("Start Capture"));
	this->btnCapture->SetRect(104, 244, 120, 23, false);
	this->btnCapture->HandleButtonClick(OnCaptureClicked, this);

	this->tpCurr = this->tcMain->AddTabPage(CSTR("Current"));
	this->lvCurrWifi = ui->NewListView(this->tpCurr, UI::ListViewStyle::Table, 12);
	this->lvCurrWifi->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCurrWifi->SetShowGrid(true);
	this->lvCurrWifi->SetFullRowSelect(true);
	this->lvCurrWifi->AddColumn(CSTR("SSID"), 200);
	this->lvCurrWifi->AddColumn(CSTR("PhyId"), 40);
	this->lvCurrWifi->AddColumn(CSTR("MAC"), 120);
	this->lvCurrWifi->AddColumn(CSTR("Vendor"), 120);
	this->lvCurrWifi->AddColumn(CSTR("BSSType"), 100);
	this->lvCurrWifi->AddColumn(CSTR("PHYType"), 100);
	this->lvCurrWifi->AddColumn(CSTR("RSSI"), 100);
	this->lvCurrWifi->AddColumn(CSTR("Link Quality"), 100);
	this->lvCurrWifi->AddColumn(CSTR("Frequency"), 100);
	this->lvCurrWifi->AddColumn(CSTR("Manufacturer"), 100);
	this->lvCurrWifi->AddColumn(CSTR("Model"), 100);
	this->lvCurrWifi->AddColumn(CSTR("S/N"), 100);

	this->tpLogWifi = this->tcMain->AddTabPage(CSTR("Wifi Log"));
	this->pnlLogWifi = ui->NewPanel(this->tpLogWifi);
	this->pnlLogWifi->SetRect(0, 0, 100, 31, false);
	this->pnlLogWifi->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnLogWifiSave = ui->NewButton(this->pnlLogWifi, CSTR("Save"));
	this->btnLogWifiSave->SetRect(4, 4, 75, 23, false);
	this->btnLogWifiSave->HandleButtonClick(OnLogWifiSaveClicked, this);
	this->btnLogWifiSaveF = ui->NewButton(this->pnlLogWifi, CSTR("Save Unk only"));
	this->btnLogWifiSaveF->SetRect(84, 4, 75, 23, false);
	this->btnLogWifiSaveF->HandleButtonClick(OnLogWifiSaveFClicked, this);
	this->lvLogWifi = ui->NewListView(this->tpLogWifi, UI::ListViewStyle::Table, 12);
	this->lvLogWifi->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvLogWifi->SetShowGrid(true);
	this->lvLogWifi->SetFullRowSelect(true);
	this->lvLogWifi->HandleDblClk(OnLogWifiDblClicked, this);
	this->lvLogWifi->AddColumn(CSTR("MAC"), 120);
	this->lvLogWifi->AddColumn(CSTR("Vendor"), 120);
	this->lvLogWifi->AddColumn(CSTR("SSID"), 200);
	this->lvLogWifi->AddColumn(CSTR("PHYType"), 60);
	this->lvLogWifi->AddColumn(CSTR("Frequency"), 80);
	this->lvLogWifi->AddColumn(CSTR("Manufactorer"), 150);
	this->lvLogWifi->AddColumn(CSTR("Model"), 150);
	this->lvLogWifi->AddColumn(CSTR("S/N"), 100);
	this->lvLogWifi->AddColumn(CSTR("Country"), 50);
	this->lvLogWifi->AddColumn(CSTR("Vendor1"), 120);
	this->lvLogWifi->AddColumn(CSTR("Vendor2"), 120);
	this->lvLogWifi->AddColumn(CSTR("Vendor3"), 120);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetClosingHandler(OnFormClosing, this);
	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRWifiCaptureForm::~AVIRWifiCaptureForm()
{
	NN<Map::LocationService> locSvc;
	this->motion.Delete();
	this->wlanInterf.Delete();
	if (this->locSvc.SetTo(locSvc))
	{
		locSvc->UnregisterLocationHandler(OnGPSData, this);
		if (this->locSvcRel)
		{
			locSvc.Delete();
		}
		this->locSvc = 0;
		this->locSvcRel = false;
	}
	this->captureWriter.Delete();
	this->captureFS.Delete();
	this->wlan.Delete();
	UOSInt i;
	NN<BSSStatus> bss;
	i = this->bssMap.GetCount();
	while (i-- > 0)
	{
		bss = this->bssMap.GetItemNoCheck(i);
		bss->ssid->Release();
		MemFreeNN(bss);
	}

	NN<SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog> wifiLog;
	i = this->wifiLogMap.GetCount();
	while (i-- > 0)
	{
		wifiLog = this->wifiLogMap.GetItemNoCheck(i);
		wifiLog->ssid->Release();
		OPTSTR_DEL(wifiLog->manuf);
		OPTSTR_DEL(wifiLog->model);
		OPTSTR_DEL(wifiLog->serialNum);
		OPTSTR_DEL(wifiLog->country);
		MemFreeNN(wifiLog);
	}
}

void SSWR::AVIRead::AVIRWifiCaptureForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
