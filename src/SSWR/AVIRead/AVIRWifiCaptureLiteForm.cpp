#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRWifiCaptureLiteForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/Clipboard.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRWifiCaptureLiteForm *me = (SSWR::AVIRead::AVIRWifiCaptureLiteForm*)userObj;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	UInt64 imac;
	SSWR::AVIRead::AVIRWifiCaptureLiteForm::BSSStatus *bsss;
	UInt8 id[8];
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Data::DateTime dt;
	UInt64 maxIMAC;
	Int32 maxRSSI;
	Text::StringBuilderUTF8 sb;
	UOSInt ieLen;
	Net::WirelessLANIE *ie;
	const UInt8 *ieBuff;
	dt.SetCurrTimeUTC();
	if (me->wlanInterf)
	{
		dt.SetCurrTimeUTC();
		if (dt.ToTicks() - me->lastTimeTick > 900)
		{
			if (me->wlanScan-- <= 0)
			{
				NotNullPtr<Text::String> s;
				Text::String *str;
				NotNullPtr<Text::String> ssid;
				Data::ArrayList<Net::WirelessLAN::BSSInfo*> bssList;
				Net::WirelessLAN::BSSInfo *bss;
				me->wlanInterf->GetBSSList(&bssList);
				me->lvCurrWifi->ClearItems();

				maxIMAC = 0;
				maxRSSI = -128;
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
					sptr = Text::StrUInt32(sbuff, bss->GetPHYId());
					me->lvCurrWifi->SetSubItem(k, 1, CSTRP(sbuff, sptr));
					sptr = Text::StrHexBytes(sbuff, &id[2], 6, ':');
					me->lvCurrWifi->SetSubItem(k, 2, CSTRP(sbuff, sptr));
					sptr = Text::StrInt32(sbuff, bss->GetBSSType());
					const Net::MACInfo::MACEntry *entry = Net::MACInfo::GetMACInfo(imac);
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
					if (s.Set(bss->GetManuf()))
						me->lvCurrWifi->SetSubItem(k, 9, s);
					if (s.Set(bss->GetModel()))
						me->lvCurrWifi->SetSubItem(k, 10, s);
					if (s.Set(bss->GetSN()))
						me->lvCurrWifi->SetSubItem(k, 11, s);
					if (maxRSSI < bss->GetRSSI())
					{
						maxRSSI = Double2Int32(bss->GetRSSI());
						maxIMAC = imac;
					}

					SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog *wifiLog = me->wifiLogMap.Get(imac);
					const UInt8 *oui1 = bss->GetChipsetOUI(0);
					const UInt8 *oui2 = bss->GetChipsetOUI(1);
					const UInt8 *oui3 = bss->GetChipsetOUI(2);
					ieLen = 0;
					k = bss->GetIECount();
					while (k-- > 0)
					{
						ie = bss->GetIE(k);
						ieLen += (UOSInt)ie->GetIEBuff()[1] + 2;
					}
					if (wifiLog == 0)
					{
						wifiLog = MemAlloc(SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog, 1);
						MemClear(wifiLog->neighbour, sizeof(wifiLog->neighbour));
						MemCopyNO(wifiLog->mac, &id[2], 6);
						wifiLog->ssid = ssid->Clone();
						wifiLog->phyType = bss->GetPHYType();
						wifiLog->freq = bss->GetFreq();
						str = bss->GetManuf();
						wifiLog->manuf = SCOPY_STRING(str);
						str = bss->GetModel();
						wifiLog->model = SCOPY_STRING(str);
						str = bss->GetSN();
						wifiLog->serialNum = SCOPY_STRING(str);
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
							wifiLog->ieBuff = MemAlloc(UInt8, ieLen);
							k = 0;
							l = bss->GetIECount();
							m = 0;
							while (k < l)
							{
								ie = bss->GetIE(k);
								ieBuff = ie->GetIEBuff();
								MemCopyNO(&wifiLog->ieBuff[m], ieBuff, (UOSInt)ieBuff[1] + 2);
								m += (UOSInt)ieBuff[1] + 2;
								k++;
							}
						}
						else
						{
							wifiLog->ieBuff = 0;
						}
						me->wifiLogMap.Put(imac, wifiLog);

						sptr = Text::StrHexBytes(sbuff, &id[2], 6, ':');
						k = me->lvLogWifi->InsertItem((UOSInt)me->wifiLogMap.GetIndex(imac), CSTRP(sbuff, sptr), wifiLog);
						const Net::MACInfo::MACEntry *entry = Net::MACInfo::GetMACInfo(imac);
						me->lvLogWifi->SetSubItem(k, 1, {entry->name, entry->nameLen});
						me->lvLogWifi->SetSubItem(k, 2, wifiLog->ssid);
						sptr = Text::StrInt32(sbuff, wifiLog->phyType);
						me->lvLogWifi->SetSubItem(k, 3, CSTRP(sbuff, sptr));
						sptr = Text::StrDouble(sbuff, wifiLog->freq);
						me->lvLogWifi->SetSubItem(k, 4, CSTRP(sbuff, sptr));
						if (s.Set(wifiLog->manuf))
							me->lvLogWifi->SetSubItem(k, 5, s);
						if (s.Set(wifiLog->model))
							me->lvLogWifi->SetSubItem(k, 6, s);
						if (s.Set(wifiLog->serialNum))
							me->lvLogWifi->SetSubItem(k, 7, s);
						if (s.Set(wifiLog->country))
							me->lvLogWifi->SetSubItem(k, 8, s);
						if (wifiLog->ouis[0][0] != 0 || wifiLog->ouis[0][1] != 0 || wifiLog->ouis[0][2] != 0)
						{
							const Net::MACInfo::MACEntry *entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[0]);
							me->lvLogWifi->SetSubItem(k, 9, {entry->name, entry->nameLen});
						}
						if (wifiLog->ouis[1][0] != 0 || wifiLog->ouis[1][1] != 0 || wifiLog->ouis[1][2] != 0)
						{
							const Net::MACInfo::MACEntry *entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[1]);
							me->lvLogWifi->SetSubItem(k, 10, {entry->name, entry->nameLen});
						}
						if (wifiLog->ouis[2][0] != 0 || wifiLog->ouis[2][1] != 0 || wifiLog->ouis[2][2] != 0)
						{
							const Net::MACInfo::MACEntry *entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[2]);
							me->lvLogWifi->SetSubItem(k, 11, {entry->name, entry->nameLen});
						}
					}
					else
					{
						OSInt sk = me->wifiLogMap.GetIndex(imac);
						if (sk >= 0)
						{
							if (wifiLog->manuf == 0 && s.Set(bss->GetManuf()))
							{
								wifiLog->manuf = s->Clone().Ptr();
								me->lvLogWifi->SetSubItem((UOSInt)sk, 5, s);
							}
							if (wifiLog->model == 0 && s.Set(bss->GetModel()))
							{
								wifiLog->model = s->Clone().Ptr();
								me->lvLogWifi->SetSubItem((UOSInt)sk, 6, s);
							}
							if (wifiLog->serialNum == 0 && s.Set(bss->GetSN()))
							{
								wifiLog->serialNum = s->Clone().Ptr();
								me->lvLogWifi->SetSubItem((UOSInt)sk, 7, s);
							}
							if (wifiLog->country == 0 && bss->GetCountry())
							{
								s =Text::String::NewNotNullSlow(bss->GetCountry());
								wifiLog->country = s.Ptr();
								me->lvLogWifi->SetSubItem((UOSInt)sk, 8, s);
							}
						}
						UOSInt l;
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
									const Net::MACInfo::MACEntry *entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[l]);
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
									const Net::MACInfo::MACEntry *entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[l]);
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
									const Net::MACInfo::MACEntry *entry = Net::MACInfo::GetMACInfoOUI(wifiLog->ouis[l]);
									me->lvLogWifi->SetSubItem(k, 11, {entry->name, entry->nameLen});
								}
								l++;
							}
						}

						if (ieLen > wifiLog->ieLen)
						{
							if (wifiLog->ieBuff)
							{
								MemFree(wifiLog->ieBuff);
							}
							wifiLog->ieBuff = MemAlloc(UInt8, ieLen);
							k = 0;
							l = bss->GetIECount();
							m = 0;
							while (k < l)
							{
								ie = bss->GetIE(k);
								ieBuff = ie->GetIEBuff();
								MemCopyNO(&wifiLog->ieBuff[m], ieBuff, (UOSInt)ieBuff[1] + 2);
								m += (UOSInt)ieBuff[1] + 2;
								k++;
							}
						}
					}

					WriteInt16(id, Double2Int32(bss->GetFreq() / 1000000.0));
					bsss = me->bssMap.Get(ReadUInt64(id));
					if (bsss == 0)
					{
						bsss = MemAlloc(SSWR::AVIRead::AVIRWifiCaptureLiteForm::BSSStatus, 1);
						bsss->bssType = bss->GetBSSType();
						bsss->phyType = bss->GetPHYType();
						bsss->freq = bss->GetFreq();
						MemCopyNO(bsss->mac, &id[2], 6);
						bsss->ssid = ssid->Clone();
						me->bssMap.Put(ReadUInt64(id), bsss);
					}

					i++;
				}

				if (maxRSSI >= -60 && maxRSSI < 0)
				{
					SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog *wifiLog = me->wifiLogMap.Get(maxIMAC);
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
							Int32 rssi1 = Double2Int32(bss->GetRSSI());
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

				me->wlanInterf->Scan();

				me->wlanScan = 10;
			}
			dt.SetCurrTimeUTC();
			me->lastTimeTick = dt.ToTicks();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnLogWifiDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRWifiCaptureLiteForm *me = (SSWR::AVIRead::AVIRWifiCaptureLiteForm*)userObj;
	Text::String *s = me->lvLogWifi->GetItemTextNew(index);
	if (s)
	{
		UI::Clipboard::SetString(me->GetHandle(), s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnLogWifiSaveClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWifiCaptureLiteForm *me = (SSWR::AVIRead::AVIRWifiCaptureLiteForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Data::DateTime dt;
	sptr = IO::Path::GetProcessFileName(sbuff);
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
			Text::UTF8Writer writer(&fs);
			writer.WriteSignature();
			SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog *wifiLog;
			i = 0;
			j = me->wifiLogMap.GetCount();
			while (i < j)
			{
				wifiLog = me->wifiLogMap.GetItem(i);
				sb.ClearStr();
				sb.AppendHexBuff(wifiLog->mac, 6, ':', Text::LineBreakType::None);
				sb.AppendC(UTF8STRC("\t"));
				sb.Append(wifiLog->ssid);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendI32(wifiLog->phyType);
				sb.AppendC(UTF8STRC("\t"));
				Text::SBAppendF64(&sb, wifiLog->freq);
				sb.AppendC(UTF8STRC("\t"));
				if (wifiLog->manuf)
				{
					sb.Append(wifiLog->manuf);
				}
				sb.AppendC(UTF8STRC("\t"));
				if (wifiLog->model)
				{
					sb.Append(wifiLog->model);
				}
				sb.AppendC(UTF8STRC("\t"));
				if (wifiLog->serialNum)
				{
					sb.Append(wifiLog->serialNum);
				}
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendHexBuff(wifiLog->ouis[0], 3, 0, Text::LineBreakType::None);
				sb.AppendUTF8Char(',');
				sb.AppendHexBuff(wifiLog->ouis[1], 3, 0, Text::LineBreakType::None);
				sb.AppendUTF8Char(',');
				sb.AppendHexBuff(wifiLog->ouis[2], 3, 0, Text::LineBreakType::None);
				sb.AppendC(UTF8STRC("\t"));
				if (wifiLog->country)
				{
					sb.Append(wifiLog->country);
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
				if (wifiLog->ieLen > 0)
				{
					sb.AppendHexBuff(wifiLog->ieBuff, wifiLog->ieLen, 0, Text::LineBreakType::None);
				}
				if (!writer.WriteLineC(sb.ToString(), sb.GetLength()))
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
		UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Save"), me);
	}
	else
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Error in saving to "));
		sb.AppendP(sbuff, sptr);
		UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Save"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnLogWifiSaveFClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWifiCaptureLiteForm *me = (SSWR::AVIRead::AVIRWifiCaptureLiteForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UInt8 macBuff[8];
	Data::DateTime dt;
	sptr = IO::Path::GetProcessFileName(sbuff);
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
			Text::UTF8Writer writer(&fs);
			writer.WriteSignature();
			SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog *wifiLog;
			i = 0;
			j = me->wifiLogMap.GetCount();
			while (i < j)
			{
				wifiLog = me->wifiLogMap.GetItem(i);
				MemCopyNO(&macBuff[2], wifiLog->mac, 6);
				macBuff[0] = 0;
				macBuff[1] = 0;
				const Net::MACInfo::MACEntry *ent = Net::MACInfo::GetMACInfo(ReadMUInt64(macBuff));
				if (Text::StrEqualsC(ent->name, ent->nameLen, UTF8STRC("Unknown")))
				{
					sb.ClearStr();
					sb.AppendHexBuff(wifiLog->mac, 6, ':', Text::LineBreakType::None);
					sb.AppendC(UTF8STRC("\t"));
					sb.Append(wifiLog->ssid);
					sb.AppendC(UTF8STRC("\t"));
					sb.AppendI32(wifiLog->phyType);
					sb.AppendC(UTF8STRC("\t"));
					Text::SBAppendF64(&sb, wifiLog->freq);
					if (!writer.WriteLineC(sb.ToString(), sb.GetLength()))
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
		UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Save Unk"), me);
	}
	else
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Error in saving to "));
		sb.AppendP(sbuff, sptr);
		UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Save Unk"), me);
	}
}

Bool __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnFormClosing(void *userObj, CloseReason reason)
{
	SSWR::AVIRead::AVIRWifiCaptureLiteForm *me = (SSWR::AVIRead::AVIRWifiCaptureLiteForm*)userObj;
	Manage::HiResClock clk;
	if (UI::MessageDialog::ShowYesNoDialog(CSTR("Are you sure to close?"), CSTR("Question"), me))
	{
		if (clk.GetTimeDiff() < 30)
		{
			return false;
		}
	}
	return true;
}

SSWR::AVIRead::AVIRWifiCaptureLiteForm::AVIRWifiCaptureLiteForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Wifi Capture Lite"));

	this->core = core;
	this->lastTimeTick = 0;
	UOSInt i;
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
	
	this->tpCurr = this->tcMain->AddTabPage(CSTR("Current"));
	NEW_CLASS(this->lvCurrWifi, UI::GUIListView(ui, this->tpCurr, UI::GUIListView::LVSTYLE_TABLE, 12));
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
	NEW_CLASS(this->pnlLogWifi, UI::GUIPanel(ui, this->tpLogWifi));
	this->pnlLogWifi->SetRect(0, 0, 100, 31, false);
	this->pnlLogWifi->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnLogWifiSave, UI::GUIButton(ui, this->pnlLogWifi, CSTR("Save")));
	this->btnLogWifiSave->SetRect(4, 4, 75, 23, false);
	this->btnLogWifiSave->HandleButtonClick(OnLogWifiSaveClicked, this);
	NEW_CLASS(this->btnLogWifiSaveF, UI::GUIButton(ui, this->pnlLogWifi, CSTR("Save Unk only")));
	this->btnLogWifiSaveF->SetRect(84, 4, 75, 23, false);
	this->btnLogWifiSaveF->HandleButtonClick(OnLogWifiSaveFClicked, this);
	NEW_CLASS(this->lvLogWifi, UI::GUIListView(ui, this->tpLogWifi, UI::GUIListView::LVSTYLE_TABLE, 12));
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

SSWR::AVIRead::AVIRWifiCaptureLiteForm::~AVIRWifiCaptureLiteForm()
{
	SDEL_CLASS(this->wlanInterf);
	DEL_CLASS(this->wlan);
	UOSInt i;
	SSWR::AVIRead::AVIRWifiCaptureLiteForm::BSSStatus *bss;
	i = this->bssMap.GetCount();
	while (i-- > 0)
	{
		bss = this->bssMap.GetItem(i);
		bss->ssid->Release();
		MemFree(bss);
	}

	SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog *wifiLog;
	i = this->wifiLogMap.GetCount();
	while (i-- > 0)
	{
		wifiLog = this->wifiLogMap.GetItem(i);
		wifiLog->ssid->Release();
		SDEL_STRING(wifiLog->manuf);
		SDEL_STRING(wifiLog->model);
		SDEL_STRING(wifiLog->serialNum);
		SDEL_STRING(wifiLog->country);
		if (wifiLog->ieBuff)
		{
			MemFree(wifiLog->ieBuff);
		}
		MemFree(wifiLog);
	}
}

void SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
