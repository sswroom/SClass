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
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"

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
				const UTF8Char *csptr;
				const UTF8Char *ssid;
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
					Text::StrInt32(sbuff, bss->GetLinkQuality());
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

					SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog *wifiLog = me->wifiLogMap->Get(imac);
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
						OSInt sk = me->wifiLogMap->GetIndex(imac);
						if (sk >= 0)
						{
							if (wifiLog->manuf == 0 && bss->GetManuf())
							{
								wifiLog->manuf = Text::StrCopyNew(bss->GetManuf());
								me->lvLogWifi->SetSubItem((UOSInt)sk, 5, wifiLog->manuf);
							}
							if (wifiLog->model == 0 && bss->GetModel())
							{
								wifiLog->model = Text::StrCopyNew(bss->GetModel());
								me->lvLogWifi->SetSubItem((UOSInt)sk, 6, wifiLog->model);
							}
							if (wifiLog->serialNum == 0 && bss->GetSN())
							{
								wifiLog->serialNum = Text::StrCopyNew(bss->GetSN());
								me->lvLogWifi->SetSubItem((UOSInt)sk, 7, wifiLog->serialNum);
							}
							if (wifiLog->country == 0 && bss->GetCountry())
							{
								wifiLog->country = Text::StrCopyNew(bss->GetCountry());
								me->lvLogWifi->SetSubItem((UOSInt)sk, 8, wifiLog->country);
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

					WriteInt16(id, Math::Double2Int32(bss->GetFreq() / 1000000.0));
					bsss = me->bssMap->Get(ReadInt64(id));
					if (bsss == 0)
					{
						bsss = MemAlloc(SSWR::AVIRead::AVIRWifiCaptureLiteForm::BSSStatus, 1);
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
						me->bssMap->Put(ReadInt64(id), bsss);
					}
					else if (ssid && bsss->ssid == 0)
					{
						bsss->ssid = Text::StrCopyNew(ssid);
					}

					i++;
				}

				if (maxRSSI >= -60 && maxRSSI < 0)
				{
					SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog *wifiLog = me->wifiLogMap->Get(maxIMAC);
					i = 0;
					j = bssList.GetCount();
					while (i < j)
					{
						bss = bssList.GetItem(i);
						ssid = bss->GetSSID();
						MemCopyNO(&id[2], bss->GetMAC(), 6);
						id[0] = 0;
						id[1] = 0;
						imac = ReadMInt64(id);
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
										wifiLog->neighbour[k] = imac | (((Int64)rssi1 & 0xff) << 48) | (((Int64)bss->GetLinkQuality()) << 56);
									}
									break;
								}
								else if (wifiLog->neighbour[k] == 0)
								{
									wifiLog->neighbour[k] = imac | (((Int64)rssi1 & 0xff) << 48) | (((Int64)bss->GetLinkQuality()) << 56);
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
								wifiLog->neighbour[minIndex] = imac | (((Int64)rssi1 & 0xff) << 48) | (((Int64)bss->GetLinkQuality()) << 56);
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

void __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnLogWifiDblClicked(void *userObj, OSInt index)
{
	SSWR::AVIRead::AVIRWifiCaptureLiteForm *me = (SSWR::AVIRead::AVIRWifiCaptureLiteForm*)userObj;
	const UTF8Char *csptr = me->lvLogWifi->GetItemTextNew(index);
	if (csptr)
	{
		Win32::Clipboard::SetString(me->GetHandle(), csptr);
		me->lvLogWifi->DelTextNew(csptr);
	}
}

void __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnLogWifiSaveClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWifiCaptureLiteForm *me = (SSWR::AVIRead::AVIRWifiCaptureLiteForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt ui;
	UOSInt uj;
	UOSInt uk;
	OSInt si;
	Data::DateTime dt;
	IO::Path::GetProcessFileName(sbuff);
	si = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[si + 1];
	dt.SetCurrTime();
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".txt");
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Bool succ = false;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		succ = true;
		NEW_CLASS(writer, Text::UTF8Writer(fs));
		writer->WriteSignature();
		SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog *wifiLog;
		Data::ArrayList<SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog*> *wifiLogList = me->wifiLogMap->GetValues();
		ui = 0;
		uj = wifiLogList->GetCount();
		while (ui < uj)
		{
			wifiLog = wifiLogList->GetItem(ui);
			sb.ClearStr();
			sb.AppendHexBuff(wifiLog->mac, 6, ':', Text::LBT_NONE);
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
			sb.AppendHexBuff(wifiLog->ouis[0], 3, 0, Text::LBT_NONE);
			sb.AppendChar(',', 1);
			sb.AppendHexBuff(wifiLog->ouis[1], 3, 0, Text::LBT_NONE);
			sb.AppendChar(',', 1);
			sb.AppendHexBuff(wifiLog->ouis[2], 3, 0, Text::LBT_NONE);
			sb.Append((const UTF8Char*)"\t");
			if (wifiLog->country)
			{
				sb.Append(wifiLog->country);
			}
			sb.Append((const UTF8Char*)"\t");
			uk = 0;
			while (uk < 20)
			{
				if (wifiLog->neighbour[uk] == 0)
					break;
				if (uk > 0)
				{
					sb.AppendChar(',', 1);
				}
				sb.AppendHex64(wifiLog->neighbour[uk]);
				uk++;
			}
			sb.Append((const UTF8Char*)"\t");
			if (wifiLog->ieLen > 0)
			{
				sb.AppendHexBuff(wifiLog->ieBuff, wifiLog->ieLen, 0, Text::LBT_NONE);
			}
			if (!writer->WriteLine(sb.ToString()))
			{
				succ = false;
			}

			ui++;
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

void __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnLogWifiSaveFClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWifiCaptureLiteForm *me = (SSWR::AVIRead::AVIRWifiCaptureLiteForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	OSInt si;
	UOSInt ui;
	UOSInt uj;
	UInt8 macBuff[8];
	Data::DateTime dt;
	IO::Path::GetProcessFileName(sbuff);
	si = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[si + 1];
	dt.SetCurrTime();
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".txt");
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Bool succ = false;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		succ = true;
		NEW_CLASS(writer, Text::UTF8Writer(fs));
		writer->WriteSignature();
		SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog *wifiLog;
		Data::ArrayList<SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog*> *wifiLogList = me->wifiLogMap->GetValues();
		ui = 0;
		uj = wifiLogList->GetCount();
		while (ui < uj)
		{
			wifiLog = wifiLogList->GetItem(ui);
			MemCopyNO(&macBuff[2], wifiLog->mac, 6);
			macBuff[0] = 0;
			macBuff[1] = 0;
			if (Text::StrEquals(Net::MACInfo::GetMACInfo(ReadMInt64(macBuff))->name, "Unknown"))
			{
				sb.ClearStr();
				sb.AppendHexBuff(wifiLog->mac, 6, ':', Text::LBT_NONE);
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

			ui++;
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

Bool __stdcall SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnFormClosing(void *userObj, CloseReason reason)
{
	SSWR::AVIRead::AVIRWifiCaptureLiteForm *me = (SSWR::AVIRead::AVIRWifiCaptureLiteForm*)userObj;
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

SSWR::AVIRead::AVIRWifiCaptureLiteForm::AVIRWifiCaptureLiteForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Wifi Capture Lite");

	this->core = core;
	this->lastTimeTick = 0;
	NEW_CLASS(this->bssMap, Data::Int64Map<SSWR::AVIRead::AVIRWifiCaptureLiteForm::BSSStatus*>());
	NEW_CLASS(this->wifiLogMap, Data::Int64Map<SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog*>());
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

SSWR::AVIRead::AVIRWifiCaptureLiteForm::~AVIRWifiCaptureLiteForm()
{
	SDEL_CLASS(this->wlanInterf);
	DEL_CLASS(this->wlan);
	UOSInt i;
	SSWR::AVIRead::AVIRWifiCaptureLiteForm::BSSStatus *bss;
	Data::ArrayList<SSWR::AVIRead::AVIRWifiCaptureLiteForm::BSSStatus*> *bssList = this->bssMap->GetValues();
	i = bssList->GetCount();
	while (i-- > 0)
	{
		bss = bssList->GetItem(i);
		SDEL_TEXT(bss->ssid);
		MemFree(bss);
	}
	DEL_CLASS(this->bssMap);

	Data::ArrayList<SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog*> *wifiLogList = this->wifiLogMap->GetValues();
	SSWR::AVIRead::AVIRWifiCaptureLiteForm::WifiLog *wifiLog;
	i = wifiLogList->GetCount();
	while (i-- > 0)
	{
		wifiLog = wifiLogList->GetItem(i);
		Text::StrDelNew(wifiLog->ssid);
		SDEL_TEXT(wifiLog->manuf);
		SDEL_TEXT(wifiLog->model);
		SDEL_TEXT(wifiLog->serialNum);
		SDEL_TEXT(wifiLog->country);
		if (wifiLog->ieBuff)
		{
			MemFree(wifiLog->ieBuff);
		}
		MemFree(wifiLog);
	}
	DEL_CLASS(this->wifiLogMap);
}

void SSWR::AVIRead::AVIRWifiCaptureLiteForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
