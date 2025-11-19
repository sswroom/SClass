#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Math/Math_C.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRWifiCaptureLiteForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureLiteForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureLiteForm>();
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	UInt64 imac;
	NN<SSWR::AVIRead::AVIRWifiCaptureLiteForm::BSSStatus> bsss;
	UInt8 id[8];
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Data::DateTime dt;
	UInt64 maxIMAC;
	Int32 maxRSSI;
	Text::StringBuilderUTF8 sb;
	UOSInt ieLen;
	NN<Net::WirelessLANIE> ie;
	UnsafeArray<const UInt8> ieBuff;
	UnsafeArray<UInt8> wieBuff;
	dt.SetCurrTimeUTC();
	NN<Net::WirelessLAN::Interface> wlanInterf;
	if (me->wlanInterf.SetTo(wlanInterf))
	{
		dt.SetCurrTimeUTC();
		if (dt.ToTicks() - me->lastTimeTick > 900)
		{
			if (me->wlanScan-- <= 0)
			{
				NN<Text::String> s;
				NN<Text::String> ssid;
				Data::ArrayListNN<Net::WirelessLAN::BSSInfo> bssList;
				NN<Net::WirelessLAN::BSSInfo> bss;
				wlanInterf->GetBSSList(bssList);
				me->lvCurrWifi->ClearItems();

				maxIMAC = 0;
				maxRSSI = -128;
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

					NN<SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog> wifiLog;
					const UInt8 tmpOUI[] = {0, 0, 0};
					UnsafeArray<const UInt8> oui1 = bss->GetChipsetOUI(0).Or(tmpOUI);
					UnsafeArray<const UInt8> oui2 = bss->GetChipsetOUI(1).Or(tmpOUI);
					UnsafeArray<const UInt8> oui3 = bss->GetChipsetOUI(2).Or(tmpOUI);
					ieLen = 0;
					k = bss->GetIECount();
					while (k-- > 0)
					{
						if (bss->GetIE(k).SetTo(ie))
						{
							ieLen += (UOSInt)ie->GetIEBuff()[1] + 2;
						}
					}
					if (!me->wifiLogMap.Get(imac).SetTo(wifiLog))
					{
						wifiLog = MemAllocNN(SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog);
						MemClear(wifiLog->neighbour, sizeof(wifiLog->neighbour));
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
						wifiLog->ieLen = ieLen;
						if (ieLen > 0)
						{
							wifiLog->ieBuff = wieBuff = MemAllocArr(UInt8, ieLen);
							k = 0;
							l = bss->GetIECount();
							m = 0;
							while (k < l)
							{
								if (bss->GetIE(k).SetTo(ie))
								{
									ieBuff = ie->GetIEBuff();
									MemCopyNO(&wieBuff[m], ieBuff.Ptr(), (UOSInt)ieBuff[1] + 2);
									m += (UOSInt)ieBuff[1] + 2;
								}
								k++;
							}
						}
						else
						{
							wifiLog->ieBuff = 0;
						}
						me->wifiLogMap.Put(imac, wifiLog);

						Net::MACInfo::AddressType addrType;
						sptr = Text::StrHexBytes(sbuff, &id[0], 6, ':');
						k = me->lvLogWifi->InsertItem((UOSInt)me->wifiLogMap.GetIndex(imac), CSTRP(sbuff, sptr), wifiLog);
						me->lvLogWifi->SetSubItem(k, 1, Net::MACInfo::AddressTypeGetName(addrType = Net::MACInfo::GetAddressType(id)));
						if (addrType == Net::MACInfo::AddressType::UniversalUnicast || addrType == Net::MACInfo::AddressType::UniversalMulticast)
						{
							NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMAC64Info(imac);
							me->lvLogWifi->SetSubItem(k, 2, {entry->name, entry->nameLen});
						}
						else
						{
							me->lvLogWifi->SetSubItem(k, 2, CSTR("-"));
						}
						me->lvLogWifi->SetSubItem(k, 3, wifiLog->ssid);
						sptr = Text::StrInt32(sbuff, wifiLog->phyType);
						me->lvLogWifi->SetSubItem(k, 4, CSTRP(sbuff, sptr));
						sptr = Text::StrDouble(sbuff, wifiLog->freq);
						me->lvLogWifi->SetSubItem(k, 5, CSTRP(sbuff, sptr));
						if (wifiLog->manuf.SetTo(s))
							me->lvLogWifi->SetSubItem(k, 6, s);
						if (wifiLog->model.SetTo(s))
							me->lvLogWifi->SetSubItem(k, 7, s);
						if (wifiLog->serialNum.SetTo(s))
							me->lvLogWifi->SetSubItem(k, 8, s);
						if (wifiLog->country.SetTo(s))
							me->lvLogWifi->SetSubItem(k, 9, s);
						if (wifiLog->ouis[0][0] != 0 || wifiLog->ouis[0][1] != 0 || wifiLog->ouis[0][2] != 0)
						{
							NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[0]);
							me->lvLogWifi->SetSubItem(k, 10, {entry->name, entry->nameLen});
						}
						if (wifiLog->ouis[1][0] != 0 || wifiLog->ouis[1][1] != 0 || wifiLog->ouis[1][2] != 0)
						{
							NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[1]);
							me->lvLogWifi->SetSubItem(k, 11, {entry->name, entry->nameLen});
						}
						if (wifiLog->ouis[2][0] != 0 || wifiLog->ouis[2][1] != 0 || wifiLog->ouis[2][2] != 0)
						{
							NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[2]);
							me->lvLogWifi->SetSubItem(k, 12, {entry->name, entry->nameLen});
						}
					}
					else
					{
						OSInt sk = me->wifiLogMap.GetIndex(imac);
						if (sk >= 0)
						{
							if (wifiLog->manuf.IsNull() && bss->GetManuf().SetTo(s))
							{
								wifiLog->manuf = s->Clone();
								me->lvLogWifi->SetSubItem((UOSInt)sk, 6, s);
							}
							if (wifiLog->model.IsNull() && bss->GetModel().SetTo(s))
							{
								wifiLog->model = s->Clone();
								me->lvLogWifi->SetSubItem((UOSInt)sk, 7, s);
							}
							if (wifiLog->serialNum.IsNull() && bss->GetSN().SetTo(s))
							{
								wifiLog->serialNum = s->Clone();
								me->lvLogWifi->SetSubItem((UOSInt)sk, 8, s);
							}
							if (wifiLog->country.IsNull() && bss->GetCountry().NotNull())
							{
								s = Text::String::OrEmpty(Text::String::NewOrNullSlow(bss->GetCountry()));
								wifiLog->country = s;
								me->lvLogWifi->SetSubItem((UOSInt)sk, 9, s);
							}
						}
						UOSInt l;
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
									me->lvLogWifi->SetSubItem(k, 10, {entry->name, entry->nameLen});
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
									me->lvLogWifi->SetSubItem(k, 11, {entry->name, entry->nameLen});
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
									me->lvLogWifi->SetSubItem(k, 12, {entry->name, entry->nameLen});
								}
								l++;
							}
						}

						if (ieLen > wifiLog->ieLen)
						{
							if (wifiLog->ieBuff.SetTo(wieBuff))
							{
								MemFreeArr(wieBuff);
							}
							wifiLog->ieBuff = wieBuff = MemAllocArr(UInt8, ieLen);
							k = 0;
							l = bss->GetIECount();
							m = 0;
							while (k < l)
							{
								if (bss->GetIE(k).SetTo(ie))
								{
									ieBuff = ie->GetIEBuff();
									MemCopyNO(&wieBuff[m], ieBuff.Ptr(), (UOSInt)ieBuff[1] + 2);
									m += (UOSInt)ieBuff[1] + 2;
								}
								k++;
							}
						}
					}

					WriteInt16(&id[6], Double2Int32(bss->GetFreq() / 1000000.0));
					if (!me->bssMap.Get(ReadUInt64(id)).SetTo(bsss))
					{
						bsss = MemAllocNN(SSWR::AVIRead::AVIRWifiCaptureLiteForm::BSSStatus);
						bsss->bssType = bss->GetBSSType();
						bsss->phyType = bss->GetPHYType();
						bsss->freq = bss->GetFreq();
						MemCopyNO(bsss->mac, &id[0], 6);
						bsss->ssid = ssid->Clone();
						me->bssMap.Put(ReadUInt64(id), bsss);
					}

					i++;
				}

				NN<SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog> wifiLog;
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
								if ((wifiLog->neighbour[k] & 0xffffffffffff0000) == imac)
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

				wlanInterf->Scan();

				me->wlanScan = 10;
			}
			dt.SetCurrTimeUTC();
			me->lastTimeTick = dt.ToTicks();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnLogWifiDblClicked(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureLiteForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureLiteForm>();
	NN<Text::String> s;
	if (me->lvLogWifi->GetItemTextNew(index).SetTo(s))
	{
		UI::Clipboard::SetString(me->GetHandle(), s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnLogWifiSaveClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureLiteForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureLiteForm>();
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UInt8> wieBuff;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	NN<Text::String> s;
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
			NN<SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog> wifiLog;
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
				if (wifiLog->country.SetTo(s))
				{
					sb.Append(s);
				}
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
				sb.AppendC(UTF8STRC("\t"));
				if (wifiLog->ieLen > 0 && wifiLog->ieBuff.SetTo(wieBuff))
				{
					sb.AppendHexBuff(wieBuff, wifiLog->ieLen, 0, Text::LineBreakType::None);
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

void __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnLogWifiSaveFClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureLiteForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureLiteForm>();
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
			NN<SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog> wifiLog;
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

Bool __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnFormClosing(AnyType userObj, CloseReason reason)
{
	NN<SSWR::AVIRead::AVIRWifiCaptureLiteForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiCaptureLiteForm>();
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

SSWR::AVIRead::AVIRWifiCaptureLiteForm::AVIRWifiCaptureLiteForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Wifi Capture Lite"));

	this->core = core;
	this->lastTimeTick = 0;
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

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
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
	this->lvLogWifi = ui->NewListView(this->tpLogWifi, UI::ListViewStyle::Table, 13);
	this->lvLogWifi->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvLogWifi->SetShowGrid(true);
	this->lvLogWifi->SetFullRowSelect(true);
	this->lvLogWifi->HandleDblClk(OnLogWifiDblClicked, this);
	this->lvLogWifi->AddColumn(CSTR("MAC"), 120);
	this->lvLogWifi->AddColumn(CSTR("AddrType"), 120);
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

SSWR::AVIRead::AVIRWifiCaptureLiteForm::~AVIRWifiCaptureLiteForm()
{
	this->wlanInterf.Delete();
	UOSInt i;
	UnsafeArray<UInt8> wieBuff;
	NN<SSWR::AVIRead::AVIRWifiCaptureLiteForm::BSSStatus> bss;
	i = this->bssMap.GetCount();
	while (i-- > 0)
	{
		bss = this->bssMap.GetItemNoCheck(i);
		bss->ssid->Release();
		MemFreeNN(bss);
	}

	NN<SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog> wifiLog;
	i = this->wifiLogMap.GetCount();
	while (i-- > 0)
	{
		wifiLog = this->wifiLogMap.GetItemNoCheck(i);
		wifiLog->ssid->Release();
		OPTSTR_DEL(wifiLog->manuf);
		OPTSTR_DEL(wifiLog->model);
		OPTSTR_DEL(wifiLog->serialNum);
		OPTSTR_DEL(wifiLog->country);
		if (wifiLog->ieBuff.SetTo(wieBuff))
		{
			MemFreeArr(wieBuff);
		}
		MemFreeNN(wifiLog);
	}
}

void SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
