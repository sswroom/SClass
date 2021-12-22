#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "IO/PowerInfo.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRWifiCaptureForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRWifiCaptureForm *me = (SSWR::AVIRead::AVIRWifiCaptureForm*)userObj;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt64 imac;
	BSSStatus *bsss;
	UInt8 id[8];
	UTF8Char sbuff[64];
	Data::DateTime dt;
	UInt64 maxIMAC;
	Int32 maxRSSI;
	Text::StringBuilderUTF8 sb;
	IO::PowerInfo::PowerStatus power;
	dt.SetCurrTimeUTC();
	if (IO::PowerInfo::GetPowerStatus(&power))
	{
		if (power.hasBattery)
		{
			Text::StrConcat(Text::StrUInt32(sbuff, power.batteryPercent), (const UTF8Char*)"%");
			me->txtBattery->SetText(sbuff);
			if (power.batteryPercent <= 15 && me->captureWriter)
			{
				Sync::MutexUsage mutUsage(me->captureMut);
				DEL_CLASS(me->captureWriter);
				DEL_CLASS(me->captureFS);
				me->captureWriter = 0;
				me->captureFS = 0;
				mutUsage.EndUse();
				me->btnGPS->SetEnabled(true);
			}
		}
		else
		{
			me->txtBattery->SetText((const UTF8Char*)"Battery not found");
		}
	}
	else
	{
		me->txtBattery->SetText((const UTF8Char*)"Cannot read battery status");
	}
	if (me->motion)
	{
		me->motion->UpdateStatus();
		if (me->motion->IsMovving())
		{
			me->txtMotion->SetText((const UTF8Char*)"Moving");
			Sync::MutexUsage mutUsage(me->captureMut);
			if (me->captureWriter && me->lastMotion != 1)
			{
				me->lastMotion = 1;
				sb.ClearStr();
				sb.AppendDate(&dt);
				sb.Append((const UTF8Char*)"\tMotion:Moving");
				me->captureWriter->WriteLine(sb.ToString());
			}
			mutUsage.EndUse();
		}
		else
		{
			me->txtMotion->SetText((const UTF8Char*)"Stopped");
			Sync::MutexUsage mutUsage(me->captureMut);
			if (me->captureWriter && me->lastMotion != 0)
			{
				me->lastMotion = 0;
				sb.ClearStr();
				sb.AppendDate(&dt);
				sb.Append((const UTF8Char*)"\tMotion:Stopped");
				me->captureWriter->WriteLine(sb.ToString());
			}
			mutUsage.EndUse();
		}
	}
	if (me->gpsChg)
	{
		Data::DateTime dt;
		me->gpsChg = false;
		Text::StrDouble(sbuff, me->currLat);
		me->txtGPSLat->SetText(sbuff);
		Text::StrDouble(sbuff, me->currLon);
		me->txtGPSLon->SetText(sbuff);
		Text::StrDouble(sbuff, me->currAlt);
		me->txtGPSAlt->SetText(sbuff);
		dt.SetTicks(me->currGPSTimeTick);
		dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		me->txtGPSTime->SetText(sbuff);
		if (me->currActive)
		{
			me->txtGPSActive->SetText((const UTF8Char*)"Active");
		}
		else
		{
			me->txtGPSActive->SetText((const UTF8Char*)"Void");
		}
	}
	if (me->wlanInterf)
	{
		dt.SetCurrTimeUTC();
		if (dt.ToTicks() - me->lastTimeTick > 900)
		{
			if (me->wlanScan-- <= 0)
			{
				const UTF8Char *csptr;
				const UTF8Char *ssid;
				Bool bssListUpd = false;
				Data::ArrayList<Net::WirelessLAN::BSSInfo*> bssList;
				Net::WirelessLAN::BSSInfo *bss;
				me->wlanInterf->GetBSSList(&bssList);
				me->lvCurrWifi->ClearItems();

				maxIMAC = 0;
				maxRSSI = -128;
				Sync::MutexUsage mutUsage(me->captureMut);
				if (me->captureWriter)
				{
					sb.ClearStr();
					sb.AppendDate(&dt);
					sb.Append((const UTF8Char*)"\tWLAN:");
					i = 0;
					j = bssList.GetCount();
					while (i < j)
					{
						bss = bssList.GetItem(i);

						sb.AppendChar('|', 1);
						sb.AppendHex(bss->GetMAC(), 6, ':', Text::LineBreakType::None);
						sb.AppendChar('|', 1);
						sb.Append(bss->GetSSID());
						sb.AppendChar('|', 1);
						Text::SBAppendF64(&sb, bss->GetRSSI());
						sb.AppendChar('|', 1);
						Text::SBAppendF64(&sb, bss->GetFreq() / 1000000.0);
						i++;
					}
					me->captureWriter->WriteLine(sb.ToString());
					me->ui->UseDevice(true, false);
				}
				mutUsage.EndUse();

				i = 0;
				j = bssList.GetCount();
				while (i < j)
				{
					bss = bssList.GetItem(i);
					ssid = bss->GetSSID();
					MemCopyNO(&id[2], bss->GetMAC(), 6);
					id[0] = 0;
					id[1] = 0;
					imac = ReadMUInt64(id);

					k = me->lvCurrWifi->AddItem(ssid, 0);
					Text::StrUInt32(sbuff, bss->GetPHYId());
					me->lvCurrWifi->SetSubItem(k, 1, sbuff);
					Text::StrHexBytes(sbuff, &id[2], 6, ':');
					me->lvCurrWifi->SetSubItem(k, 2, sbuff);
					Text::StrInt32(sbuff, bss->GetBSSType());
					me->lvCurrWifi->SetSubItem(k, 3, (const UTF8Char*)Net::MACInfo::GetMACInfo(imac)->name);
					me->lvCurrWifi->SetSubItem(k, 4, sbuff);
					Text::StrInt32(sbuff, bss->GetPHYType());
					me->lvCurrWifi->SetSubItem(k, 5, sbuff);
					Text::StrDouble(sbuff, bss->GetRSSI());
					me->lvCurrWifi->SetSubItem(k, 6, sbuff);
					Text::StrUInt32(sbuff, bss->GetLinkQuality());
					me->lvCurrWifi->SetSubItem(k, 7, sbuff);
					Text::StrDouble(sbuff, bss->GetFreq());
					me->lvCurrWifi->SetSubItem(k, 8, sbuff);
					if ((csptr = bss->GetManuf()) != 0)
						me->lvCurrWifi->SetSubItem(k, 9, csptr);
					if ((csptr = bss->GetModel()) != 0)
						me->lvCurrWifi->SetSubItem(k, 10, csptr);
					if ((csptr = bss->GetSN()) != 0)
						me->lvCurrWifi->SetSubItem(k, 11, csptr);
					if (maxRSSI < bss->GetRSSI())
					{
						maxRSSI = Math::Double2Int32(bss->GetRSSI());
						maxIMAC = imac;
					}

					SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog *wifiLog = me->wifiLogMap->Get(imac);
					const UInt8 *oui1 = bss->GetChipsetOUI(0);
					const UInt8 *oui2 = bss->GetChipsetOUI(1);
					const UInt8 *oui3 = bss->GetChipsetOUI(2);
					if (wifiLog == 0)
					{
						wifiLog = MemAlloc(SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog, 1);
						MemCopyNO(wifiLog->mac, &id[2], 6);
						wifiLog->ssid = Text::StrCopyNew(ssid);
						wifiLog->phyType = bss->GetPHYType();
						wifiLog->freq = bss->GetFreq();
						csptr = bss->GetManuf();
						wifiLog->manuf = csptr?Text::StrCopyNew(csptr):0;
						csptr = bss->GetModel();
						wifiLog->model = csptr?Text::StrCopyNew(csptr):0;
						csptr = bss->GetSN();
						wifiLog->serialNum = csptr?Text::StrCopyNew(csptr):0;
						csptr = bss->GetCountry();
						wifiLog->country = csptr?Text::StrCopyNew(csptr):0;
						wifiLog->ouis[0][0] = oui1[0];
						wifiLog->ouis[0][1] = oui1[1];
						wifiLog->ouis[0][2] = oui1[2];
						wifiLog->ouis[1][0] = oui2[0];
						wifiLog->ouis[1][1] = oui2[1];
						wifiLog->ouis[1][2] = oui2[2];
						wifiLog->ouis[2][0] = oui3[0];
						wifiLog->ouis[2][1] = oui3[1];
						wifiLog->ouis[2][2] = oui3[2];
						me->wifiLogMap->Put(imac, wifiLog);

						Text::StrHexBytes(sbuff, &id[2], 6, ':');
						k = me->lvLogWifi->InsertItem((UOSInt)me->wifiLogMap->GetIndex(imac), sbuff, wifiLog);
						me->lvLogWifi->SetSubItem(k, 1, (const UTF8Char*)Net::MACInfo::GetMACInfo(imac)->name);
						me->lvLogWifi->SetSubItem(k, 2, wifiLog->ssid);
						Text::StrInt32(sbuff, wifiLog->phyType);
						me->lvLogWifi->SetSubItem(k, 3, sbuff);
						Text::StrDouble(sbuff, wifiLog->freq);
						me->lvLogWifi->SetSubItem(k, 4, sbuff);
						if (wifiLog->manuf)
							me->lvLogWifi->SetSubItem(k, 5, wifiLog->manuf);
						if (wifiLog->model)
							me->lvLogWifi->SetSubItem(k, 6, wifiLog->model);
						if (wifiLog->serialNum)
							me->lvLogWifi->SetSubItem(k, 7, wifiLog->serialNum);
						if (wifiLog->country)
							me->lvLogWifi->SetSubItem(k, 8, wifiLog->country);
						if (wifiLog->ouis[0][0] != 0 || wifiLog->ouis[0][1] != 0 || wifiLog->ouis[0][2] != 0)
							me->lvLogWifi->SetSubItem(k, 9, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[0])->name);
						if (wifiLog->ouis[1][0] != 0 || wifiLog->ouis[1][1] != 0 || wifiLog->ouis[1][2] != 0)
							me->lvLogWifi->SetSubItem(k, 10, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[1])->name);
						if (wifiLog->ouis[2][0] != 0 || wifiLog->ouis[2][1] != 0 || wifiLog->ouis[2][2] != 0)
							me->lvLogWifi->SetSubItem(k, 11, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[2])->name);
					}
					else
					{
						k = (UOSInt)me->wifiLogMap->GetIndex(imac);
						if (wifiLog->manuf == 0 && bss->GetManuf())
						{
							wifiLog->manuf = Text::StrCopyNew(bss->GetManuf());
							me->lvLogWifi->SetSubItem(k, 5, wifiLog->manuf);
						}
						if (wifiLog->model == 0 && bss->GetModel())
						{
							wifiLog->model = Text::StrCopyNew(bss->GetModel());
							me->lvLogWifi->SetSubItem(k, 6, wifiLog->model);
						}
						if (wifiLog->serialNum == 0 && bss->GetSN())
						{
							wifiLog->serialNum = Text::StrCopyNew(bss->GetSN());
							me->lvLogWifi->SetSubItem(k, 7, wifiLog->serialNum);
						}
						if (wifiLog->country == 0 && bss->GetCountry())
						{
							wifiLog->country = Text::StrCopyNew(bss->GetCountry());
							me->lvLogWifi->SetSubItem(k, 8, wifiLog->country);
						}
						OSInt l;
						const UInt8 *oui;
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
									me->lvLogWifi->SetSubItem(k, 9, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[l])->name);
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
									me->lvLogWifi->SetSubItem(k, 10, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[l])->name);
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
									me->lvLogWifi->SetSubItem(k, 11, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[l])->name);
								}
								l++;
							}
						}
					}

					WriteInt16(id, Math::Double2Int32(bss->GetFreq() / 1000000.0));
					bsss = me->bssMap->Get(ReadUInt64(id));
					if (bsss == 0)
					{
						bssListUpd = true;
						bsss = MemAlloc(BSSStatus, 1);
						bsss->bssType = bss->GetBSSType();
						bsss->phyType = bss->GetPHYType();
						bsss->freq = bss->GetFreq();
						MemCopyNO(bsss->mac, &id[2], 6);
						if (ssid)
						{
							bsss->ssid = Text::StrCopyNew(ssid);
						}
						else
						{
							bsss->ssid = 0;
						}
						me->bssMap->Put(ReadUInt64(id), bsss);
					}
					else if (ssid && bsss->ssid == 0)
					{
						bsss->ssid = Text::StrCopyNew(ssid);
						bssListUpd = true;
					}

					i++;
				}

				if (maxRSSI >= -60 && maxRSSI < 0)
				{
					SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog *wifiLog = me->wifiLogMap->Get(maxIMAC);
					i = 0;
					j = bssList.GetCount();
					while (i < j)
					{
						bss = bssList.GetItem(i);
						ssid = bss->GetSSID();
						MemCopyNO(&id[2], bss->GetMAC(), 6);
						id[0] = 0;
						id[1] = 0;
						imac = ReadMUInt64(id);
						if (imac != maxIMAC)
						{
							Bool found = false;
							Int32 minRSSI;
							UOSInt minIndex;
							Int32 rssi1 = Math::Double2Int32(bss->GetRSSI());
							minRSSI = 0;
							minIndex = 0;
							k = 0;
							while (k < 20)
							{
								Int8 rssi2 = (Int8)((wifiLog->neighbour[k] >> 48) & 0xff);
								if ((wifiLog->neighbour[k] & 0xffffffffffff) == imac)
								{
									found = true;
									if (rssi1 > rssi2)
									{
										wifiLog->neighbour[k] = imac | (((UInt64)rssi1 & 0xff) << 48) | (((UInt64)bss->GetLinkQuality()) << 56);
									}
									break;
								}
								else if (wifiLog->neighbour[k] == 0)
								{
									wifiLog->neighbour[k] = imac | (((UInt64)rssi1 & 0xff) << 48) | (((UInt64)bss->GetLinkQuality()) << 56);
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
								wifiLog->neighbour[minIndex] = imac | (((UInt64)rssi1 & 0xff) << 48) | (((UInt64)bss->GetLinkQuality()) << 56);
							}
						}
						i++;
					}
				}

				i = bssList.GetCount();
				while (i-- > 0)
				{
					bss = bssList.GetItem(i);
					DEL_CLASS(bss);
				}

				Text::StrUOSInt(sbuff, j);
				me->txtCurrWifiCnt->SetText(sbuff);
				me->wlanInterf->Scan();

				me->wlanScan = 10;

				if (bssListUpd)
				{
					Text::StrUOSInt(sbuff, me->bssMap->GetCount());
					me->txtBSSCount->SetText(sbuff);
				}
			}
			dt.SetCurrTimeUTC();
			me->lastTimeTick = dt.ToTicks();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnGPSClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWifiCaptureForm *me = (SSWR::AVIRead::AVIRWifiCaptureForm*)userObj;
	if (me->locSvc)
	{
		me->locSvc->UnregisterLocationHandler(OnGPSData, me);
		if (me->locSvcRel)
		{
			DEL_CLASS(me->locSvc);
		}
		me->locSvc = 0;
		me->locSvcRel = false;
		me->txtGPS->SetText((const UTF8Char*)"Not opened");
	}
	else
	{
		SSWR::AVIRead::AVIRCore::StreamType st;
		IO::Stream *stm = me->core->OpenStream(&st, me, 0, true);
		if (stm)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(stm->GetSourceNameObj());
			me->txtGPS->SetText(sb.ToString());
			me->locSvcRel = true;
			NEW_CLASS(me->locSvc, IO::GPSNMEA(stm, true));
			me->locSvc->RegisterLocationHandler(OnGPSData, me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnCaptureClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWifiCaptureForm *me = (SSWR::AVIRead::AVIRWifiCaptureForm*)userObj;
	if (me->captureWriter)
	{
		Sync::MutexUsage mutUsage(me->captureMut);
		DEL_CLASS(me->captureWriter);
		DEL_CLASS(me->captureFS);
		me->captureWriter = 0;
		me->captureFS = 0;
		mutUsage.EndUse();
		me->btnGPS->SetEnabled(true);
	}
	else
	{
		if (me->motion == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Accelerator not found", (const UTF8Char *)"Error", me);
			return;
		}
		else if (me->locSvc == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"GPS not connected", (const UTF8Char *)"Error", me);
			return;
		}
		else if (me->wlanInterf == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Wifi adapter not found", (const UTF8Char *)"Error", me);
			return;
		}

		UTF8Char sbuff[512];
		UTF8Char *sptr;
		Bool isError;
		Data::DateTime dt;
		IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"Wifi");
		IO::Path::CreateDirectory(sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		dt.SetCurrTimeUTC();
		sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
		sptr = Text::StrConcat(sptr, (const UTF8Char*)".txt");
		Sync::MutexUsage mutUsage(me->captureMut);
		NEW_CLASS(me->captureFS, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (me->captureFS->IsError())
		{
			DEL_CLASS(me->captureFS);
			me->captureFS = 0;
			isError = true;
		}
		else
		{
			NEW_CLASS(me->captureWriter, Text::UTF8Writer(me->captureFS));
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

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnLogWifiDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRWifiCaptureForm *me = (SSWR::AVIRead::AVIRWifiCaptureForm*)userObj;
	Text::String *s = me->lvLogWifi->GetItemTextNew(index);
	if (s)
	{
		Win32::Clipboard::SetString(me->GetHandle(), s->v);
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnLogWifiSaveClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWifiCaptureForm *me = (SSWR::AVIRead::AVIRWifiCaptureForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Data::DateTime dt;
	IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];
	dt.SetCurrTime();
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".txt");
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Bool succ = false;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		succ = true;
		NEW_CLASS(writer, Text::UTF8Writer(fs));
		writer->WriteSignature();
		SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog *wifiLog;
		Data::ArrayList<SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog*> *wifiLogList = me->wifiLogMap->GetValues();
		i = 0;
		j = wifiLogList->GetCount();
		while (i < j)
		{
			wifiLog = wifiLogList->GetItem(i);
			sb.ClearStr();
			sb.AppendHexBuff(wifiLog->mac, 6, ':', Text::LineBreakType::None);
			sb.Append((const UTF8Char*)"\t");
			sb.Append(wifiLog->ssid);
			sb.Append((const UTF8Char*)"\t");
			sb.AppendI32(wifiLog->phyType);
			sb.Append((const UTF8Char*)"\t");
			Text::SBAppendF64(&sb, wifiLog->freq);
			sb.Append((const UTF8Char*)"\t");
			if (wifiLog->manuf)
			{
				sb.Append(wifiLog->manuf);
			}
			sb.Append((const UTF8Char*)"\t");
			if (wifiLog->model)
			{
				sb.Append(wifiLog->model);
			}
			sb.Append((const UTF8Char*)"\t");
			if (wifiLog->serialNum)
			{
				sb.Append(wifiLog->serialNum);
			}
			sb.Append((const UTF8Char*)"\t");
			sb.AppendHexBuff(wifiLog->ouis[0], 3, 0, Text::LineBreakType::None);
			sb.AppendChar(',', 1);
			sb.AppendHexBuff(wifiLog->ouis[1], 3, 0, Text::LineBreakType::None);
			sb.AppendChar(',', 1);
			sb.AppendHexBuff(wifiLog->ouis[2], 3, 0, Text::LineBreakType::None);
			sb.Append((const UTF8Char*)"\t");
			if (wifiLog->country)
			{
				sb.Append(wifiLog->country);
			}
			sb.Append((const UTF8Char*)"\t");
			k = 0;
			while (k < 20)
			{
				if (wifiLog->neighbour[k] == 0)
					break;
				if (k > 0)
				{
					sb.AppendChar(',', 1);
				}
				sb.AppendHex64(wifiLog->neighbour[k]);
				k++;
			}
			if (!writer->WriteLine(sb.ToString()))
			{
				succ = false;
			}

			i++;
		}
		DEL_CLASS(writer);
	}
	DEL_CLASS(fs);
	if (succ)
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"File saved to ");
		sb.Append(sbuff);
		UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"Save", me);
	}
	else
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Error in saving to ");
		sb.Append(sbuff);
		UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"Save", me);
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnLogWifiSaveFClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWifiCaptureForm *me = (SSWR::AVIRead::AVIRWifiCaptureForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UInt8 macBuff[8];
	Data::DateTime dt;
	IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];
	dt.SetCurrTime();
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".txt");
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Bool succ = false;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		succ = true;
		NEW_CLASS(writer, Text::UTF8Writer(fs));
		writer->WriteSignature();
		SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog *wifiLog;
		Data::ArrayList<SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog*> *wifiLogList = me->wifiLogMap->GetValues();
		i = 0;
		j = wifiLogList->GetCount();
		while (i < j)
		{
			wifiLog = wifiLogList->GetItem(i);
			MemCopyNO(&macBuff[2], wifiLog->mac, 6);
			macBuff[0] = 0;
			macBuff[1] = 0;
			if (Text::StrEquals(Net::MACInfo::GetMACInfo(ReadMUInt64(macBuff))->name, "Unknown"))
			{
				sb.ClearStr();
				sb.AppendHexBuff(wifiLog->mac, 6, ':', Text::LineBreakType::None);
				sb.Append((const UTF8Char*)"\t");
				sb.Append(wifiLog->ssid);
				sb.Append((const UTF8Char*)"\t");
				sb.AppendI32(wifiLog->phyType);
				sb.Append((const UTF8Char*)"\t");
				Text::SBAppendF64(&sb, wifiLog->freq);
				if (!writer->WriteLine(sb.ToString()))
				{
					succ = false;
				}
			}

			i++;
		}
		DEL_CLASS(writer);
	}
	DEL_CLASS(fs);
	if (succ)
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"File saved to ");
		sb.Append(sbuff);
		UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"Save Unk", me);
	}
	else
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Error in saving to ");
		sb.Append(sbuff);
		UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"Save Unk", me);
	}
}

Bool __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnFormClosing(void *userObj, CloseReason reason)
{
	SSWR::AVIRead::AVIRWifiCaptureForm *me = (SSWR::AVIRead::AVIRWifiCaptureForm*)userObj;
	Manage::HiResClock clk;
	if (UI::MessageDialog::ShowYesNoDialog((const UTF8Char*)"Are you sure to close?", (const UTF8Char*)"Question", me))
	{
		if (clk.GetTimeDiff() < 30)
		{
			return false;
		}
	}
	return true;
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureForm::OnGPSData(void *userObj, Map::GPSTrack::GPSRecord *record)
{
	SSWR::AVIRead::AVIRWifiCaptureForm *me = (SSWR::AVIRead::AVIRWifiCaptureForm*)userObj;
	if (me->currActive || record->valid != 0)
	{
		Sync::MutexUsage mutUsage(me->captureMut);
		if (me->captureWriter)
		{
			Text::StringBuilderUTF8 sb;
			Data::DateTime dt;
			dt.SetCurrTimeUTC();

			sb.AppendDate(&dt);
			sb.Append((const UTF8Char*)"\tGPS:");
			if (record->valid == 0)
			{
				sb.Append((const UTF8Char*)"Void");
			}
			else
			{
				sb.Append((const UTF8Char*)"Active,");
				Text::SBAppendF64(&sb, record->lat);
				sb.Append((const UTF8Char*)",");
				Text::SBAppendF64(&sb, record->lon);
				sb.Append((const UTF8Char*)",");
				Text::SBAppendF64(&sb, record->altitude);
			}
			me->captureWriter->WriteLine(sb.ToString());
		}
		mutUsage.EndUse();
	}

	me->currGPSTimeTick = record->utcTimeTicks;
	me->currLat = record->lat;
	me->currLon = record->lon;
	me->currAlt = record->altitude;
	me->currActive = record->valid != 0;
	me->gpsChg = true;
}

SSWR::AVIRead::AVIRWifiCaptureForm::AVIRWifiCaptureForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Wifi Capture");

	this->core = core;
	this->motion = 0;
	this->gpsChg = false;
	this->lastTimeTick = 0;
	this->currGPSTimeTick = 0;
	this->currLat = 0;
	this->currLon = 0;
	this->currAlt = 0;
	this->locSvc = 0;
	this->locSvcRel = false;
	this->captureFS = 0;
	this->captureWriter = 0;
	NEW_CLASS(this->captureMut, Sync::Mutex());
	NEW_CLASS(this->bssMap, Data::UInt64Map<BSSStatus*>());
	NEW_CLASS(this->wifiLogMap, Data::UInt64Map<SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog*>());
	NEW_CLASS(this->sensorMgr, IO::SensorManager());
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->sensorMgr->GetSensorCnt();
	while (i < j)
	{
		if (this->sensorMgr->GetSensorType(i) == IO::Sensor::SensorType::Accelerometer)
		{
			IO::Sensor *sensor = this->sensorMgr->CreateSensor(0);
			if (sensor)
			{
				NEW_CLASS(this->motion, IO::MotionDetectorAccelerometer(sensor->GetSensorAccelerator(), true));
				break;
			}
		}
		i++;
	}
	NEW_CLASS(this->wlan, Net::WirelessLAN());
	Data::ArrayList<Net::WirelessLAN::Interface*> interfList;
	this->wlan->GetInterfaces(&interfList);
	this->wlanInterf = interfList.GetItem(0);
	i = interfList.GetCount();
	while (i-- > 1)
	{
		Net::WirelessLAN::Interface *interf = interfList.GetItem(i);
		DEL_CLASS(interf);
	}
	this->wlanScan = 0;

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpStatus = this->tcMain->AddTabPage((const UTF8Char*)"Status");
	NEW_CLASS(this->lblMotion, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Motion"));
	this->lblMotion->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtMotion, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)"No Accelerator found"));
	this->txtMotion->SetReadOnly(true);
	this->txtMotion->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblGPS, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"GPS"));
	this->lblGPS->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtGPS, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)"Not opened"));
	this->txtGPS->SetReadOnly(true);
	this->txtGPS->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->btnGPS, UI::GUIButton(ui, this->tpStatus, (const UTF8Char*)"Open"));
	this->btnGPS->SetRect(304, 28, 75, 23, false);
	this->btnGPS->HandleButtonClick(OnGPSClicked, this);
	NEW_CLASS(this->lblGPSTime, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"GPS Time"));
	this->lblGPSTime->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtGPSTime, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)""));
	this->txtGPSTime->SetReadOnly(true);
	this->txtGPSTime->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->lblGPSLat, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Latitude"));
	this->lblGPSLat->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtGPSLat, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)""));
	this->txtGPSLat->SetReadOnly(true);
	this->txtGPSLat->SetRect(104, 76, 200, 23, false);
	NEW_CLASS(this->lblGPSLon, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Longitude"));
	this->lblGPSLon->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtGPSLon, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)""));
	this->txtGPSLon->SetReadOnly(true);
	this->txtGPSLon->SetRect(104, 100, 200, 23, false);
	NEW_CLASS(this->lblGPSAlt, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Altitude"));
	this->lblGPSAlt->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtGPSAlt, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)""));
	this->txtGPSAlt->SetReadOnly(true);
	this->txtGPSAlt->SetRect(104, 124, 200, 23, false);
	NEW_CLASS(this->lblGPSActive, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"GPS Active"));
	this->lblGPSActive->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtGPSActive, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)""));
	this->txtGPSActive->SetReadOnly(true);
	this->txtGPSActive->SetRect(104, 148, 200, 23, false);
	NEW_CLASS(this->lblBattery, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Battery"));
	this->lblBattery->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtBattery, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)""));
	this->txtBattery->SetReadOnly(true);
	this->txtBattery->SetRect(104, 172, 200, 23, false);
	NEW_CLASS(this->lblCurrWifiCnt, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Wifi Count"));
	this->lblCurrWifiCnt->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtCurrWifiCnt, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)""));
	this->txtCurrWifiCnt->SetReadOnly(true);
	this->txtCurrWifiCnt->SetRect(104, 196, 200, 23, false);
	NEW_CLASS(this->lblBSSCount, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"BSS Count"));
	this->lblBSSCount->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtBSSCount, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)""));
	this->txtBSSCount->SetReadOnly(true);
	this->txtBSSCount->SetRect(104, 220, 200, 23, false);
	NEW_CLASS(this->btnCapture, UI::GUIButton(ui, this->tpStatus, (const UTF8Char*)"Start Capture"));
	this->btnCapture->SetRect(104, 244, 120, 23, false);
	this->btnCapture->HandleButtonClick(OnCaptureClicked, this);

	this->tpCurr = this->tcMain->AddTabPage((const UTF8Char*)"Current");
	NEW_CLASS(this->lvCurrWifi, UI::GUIListView(ui, this->tpCurr, UI::GUIListView::LVSTYLE_TABLE, 12));
	this->lvCurrWifi->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCurrWifi->SetShowGrid(true);
	this->lvCurrWifi->SetFullRowSelect(true);
	this->lvCurrWifi->AddColumn((const UTF8Char*)"SSID", 200);
	this->lvCurrWifi->AddColumn((const UTF8Char*)"PhyId", 40);
	this->lvCurrWifi->AddColumn((const UTF8Char*)"MAC", 120);
	this->lvCurrWifi->AddColumn((const UTF8Char*)"Vendor", 120);
	this->lvCurrWifi->AddColumn((const UTF8Char*)"BSSType", 100);
	this->lvCurrWifi->AddColumn((const UTF8Char*)"PHYType", 100);
	this->lvCurrWifi->AddColumn((const UTF8Char*)"RSSI", 100);
	this->lvCurrWifi->AddColumn((const UTF8Char*)"Link Quality", 100);
	this->lvCurrWifi->AddColumn((const UTF8Char*)"Frequency", 100);
	this->lvCurrWifi->AddColumn((const UTF8Char*)"Manufacturer", 100);
	this->lvCurrWifi->AddColumn((const UTF8Char*)"Model", 100);
	this->lvCurrWifi->AddColumn((const UTF8Char*)"S/N", 100);

	this->tpLogWifi = this->tcMain->AddTabPage((const UTF8Char*)"Wifi Log");
	NEW_CLASS(this->pnlLogWifi, UI::GUIPanel(ui, this->tpLogWifi));
	this->pnlLogWifi->SetRect(0, 0, 100, 31, false);
	this->pnlLogWifi->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnLogWifiSave, UI::GUIButton(ui, this->pnlLogWifi, (const UTF8Char*)"Save"));
	this->btnLogWifiSave->SetRect(4, 4, 75, 23, false);
	this->btnLogWifiSave->HandleButtonClick(OnLogWifiSaveClicked, this);
	NEW_CLASS(this->btnLogWifiSaveF, UI::GUIButton(ui, this->pnlLogWifi, (const UTF8Char*)"Save Unk only"));
	this->btnLogWifiSaveF->SetRect(84, 4, 75, 23, false);
	this->btnLogWifiSaveF->HandleButtonClick(OnLogWifiSaveFClicked, this);
	NEW_CLASS(this->lvLogWifi, UI::GUIListView(ui, this->tpLogWifi, UI::GUIListView::LVSTYLE_TABLE, 12));
	this->lvLogWifi->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvLogWifi->SetShowGrid(true);
	this->lvLogWifi->SetFullRowSelect(true);
	this->lvLogWifi->HandleDblClk(OnLogWifiDblClicked, this);
	this->lvLogWifi->AddColumn((const UTF8Char*)"MAC", 120);
	this->lvLogWifi->AddColumn((const UTF8Char*)"Vendor", 120);
	this->lvLogWifi->AddColumn((const UTF8Char*)"SSID", 200);
	this->lvLogWifi->AddColumn((const UTF8Char*)"PHYType", 60);
	this->lvLogWifi->AddColumn((const UTF8Char*)"Frequency", 80);
	this->lvLogWifi->AddColumn((const UTF8Char*)"Manufactorer", 150);
	this->lvLogWifi->AddColumn((const UTF8Char*)"Model", 150);
	this->lvLogWifi->AddColumn((const UTF8Char*)"S/N", 100);
	this->lvLogWifi->AddColumn((const UTF8Char*)"Country", 50);
	this->lvLogWifi->AddColumn((const UTF8Char*)"Vendor1", 120);
	this->lvLogWifi->AddColumn((const UTF8Char*)"Vendor2", 120);
	this->lvLogWifi->AddColumn((const UTF8Char*)"Vendor3", 120);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetClosingHandler(OnFormClosing, this);
	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRWifiCaptureForm::~AVIRWifiCaptureForm()
{
	DEL_CLASS(this->sensorMgr);
	SDEL_CLASS(this->motion);
	SDEL_CLASS(this->wlanInterf);
	if (this->locSvc)
	{
		this->locSvc->UnregisterLocationHandler(OnGPSData, this);
		if (this->locSvcRel)
		{
			DEL_CLASS(this->locSvc);
		}
		this->locSvc = 0;
		this->locSvcRel = false;
	}
	if (this->captureWriter)
	{
		SDEL_CLASS(this->captureWriter);
		SDEL_CLASS(this->captureFS);
	}
	DEL_CLASS(this->captureMut);
	DEL_CLASS(this->wlan);
	UOSInt i;
	BSSStatus *bss;
	Data::ArrayList<BSSStatus*> *bssList = this->bssMap->GetValues();
	i = bssList->GetCount();
	while (i-- > 0)
	{
		bss = bssList->GetItem(i);
		SDEL_TEXT(bss->ssid);
		MemFree(bss);
	}
	DEL_CLASS(this->bssMap);

	Data::ArrayList<SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog*> *wifiLogList = this->wifiLogMap->GetValues();
	SSWR::AVIRead::AVIRWifiCaptureForm::WifiLog *wifiLog;
	i = wifiLogList->GetCount();
	while (i-- > 0)
	{
		wifiLog = wifiLogList->GetItem(i);
		Text::StrDelNew(wifiLog->ssid);
		SDEL_TEXT(wifiLog->manuf);
		SDEL_TEXT(wifiLog->model);
		SDEL_TEXT(wifiLog->serialNum);
		SDEL_TEXT(wifiLog->country);
		MemFree(wifiLog);
	}
	DEL_CLASS(this->wifiLogMap);
}

void SSWR::AVIRead::AVIRWifiCaptureForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
