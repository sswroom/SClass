#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/WriteCacheStream.h"
#include "Net/MACInfo.h"
#include "Net/WirelessLANIE.h"
#include "SSWR/AVIRead/AVIRMACManagerEntryForm.h"
#include "SSWR/AVIRead/AVIRWiFiLogManagerForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm *me = (SSWR::AVIRead::AVIRWiFiLogManagerForm*)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"WiFiLoganagerFile", false));
	dlg->SetAllowMultiSel(true);
	dlg->AddFilter((const UTF8Char*)"*.txt", (const UTF8Char*)"Log File");
	if (dlg->ShowDialog(me->GetHandle()))
	{
		UOSInt i = 0;
		UOSInt j = dlg->GetFileNameCount();
		while (i < j)
		{
			me->LogFileLoad(dlg->GetFileNames(i));
			i++;
		}
		me->LogFileStore();
		me->LogUIUpdate();
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnStoreClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm *me = (SSWR::AVIRead::AVIRWiFiLogManagerForm*)userObj;
	if (me->macList->Store())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Data Stored", (const UTF8Char*)"MAC Manager", me);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in storing data", (const UTF8Char*)"MAC Manager", me);
	}
}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnContentDblClicked(void *userObj, OSInt index)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm *me = (SSWR::AVIRead::AVIRWiFiLogManagerForm*)userObj;
	SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry *log = (SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry*)me->logList->GetItem((UOSInt)index);
	if (log == 0)
		return;
	const Net::MACInfo::MACEntry *entry = me->macList->GetEntry(log->macInt);
	SSWR::AVIRead::AVIRMACManagerEntryForm *frm;
	if (entry)
	{
		NEW_CLASS(frm, SSWR::AVIRead::AVIRMACManagerEntryForm(0, me->ui, me->core, log->mac, (const UTF8Char*)entry->name));
	}
	else
	{
		NEW_CLASS(frm, SSWR::AVIRead::AVIRMACManagerEntryForm(0, me->ui, me->core, log->mac, 0));
	}
	if (frm->ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		const UTF8Char *name = frm->GetNameNew();
		UOSInt i = me->macList->SetEntry(log->macInt, name);
		Text::StrDelNew(name);
		entry = me->macList->GetItem(i);
		me->UpdateStatus();

		UOSInt j;
		i = 0;
		j = me->logList->GetCount();
		while (i < j)
		{
			log = me->logList->GetItem(i);
			if (log->macInt >= entry->rangeStart && log->macInt <= entry->rangeEnd)
			{
				me->lvContent->SetSubItem(i, 1, (const UTF8Char*)entry->name);
			}
			i++;
		}
	}
	DEL_CLASS(frm);

}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnContentSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm *me = (SSWR::AVIRead::AVIRWiFiLogManagerForm*)userObj;
	SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry *log = (SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry*)me->lvContent->GetSelectedItem();
	if (log == 0 || log->ieLen <= 0)
	{
		me->txtFileIE->SetText((const UTF8Char*)"");
		return;
	}
	Text::StringBuilderUTF8 sb;
	OSInt i = 0;
	while (i < log->ieLen)
	{
		Net::WirelessLANIE::ToString(&log->ieBuff[i], &sb);
		sb.Append((const UTF8Char*)"\r\n");
		i += log->ieBuff[i + 1] + 2;
	}
	me->txtFileIE->SetText(sb.ToString());
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileLoad(const UTF8Char *fileName)
{
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		UTF8Char *sarr[12];
		UTF8Char *sarr2[7];
		UInt8 buff[8];
		UOSInt i;
		UOSInt j;
		OSInt k;
		SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry *log;
		Text::StringBuilderUTF8 sb;
		UInt64 iMAC;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		buff[0] = 0;
		buff[1] = 0;
		while (reader->ReadLine(&sb, 4096))
		{
			i = Text::StrSplit(sarr, 12, sb.ToString(), '\t');
			if (i == 4 || i == 7 || i == 9 || i == 10 || i == 11)
			{
				if (Text::StrSplit(sarr2, 7, sarr[0], ':') == 6)
				{
					buff[2] = Text::StrHex2Byte(sarr2[0]);
					buff[3] = Text::StrHex2Byte(sarr2[1]);
					buff[4] = Text::StrHex2Byte(sarr2[2]);
					buff[5] = Text::StrHex2Byte(sarr2[3]);
					buff[6] = Text::StrHex2Byte(sarr2[4]);
					buff[7] = Text::StrHex2Byte(sarr2[5]);
					iMAC = ReadMUInt64(buff);
					log = this->LogGet(iMAC);
					if (log)
					{
						if (i >= 7)
						{
							if ((log->manuf == 0 || log->manuf[0] == 0) && sarr[4][0] != 0)
							{
								SDEL_TEXT(log->manuf);
								log->manuf = Text::StrCopyNew(sarr[4]);
							}
							if ((log->model == 0 || log->model[0] == 0) && sarr[5][0] != 0)
							{
								SDEL_TEXT(log->model);
								log->model = Text::StrCopyNew(sarr[5]);
							}
							if ((log->serialNum == 0 || log->serialNum[0] == 0) && sarr[6][0] != 0)
							{
								SDEL_TEXT(log->serialNum);
								log->serialNum = Text::StrCopyNew(sarr[6]);
							}
						}
						if (i >= 9)
						{
							if ((log->country == 0 || log->country[0] == 0) && sarr[8][0] != 0)
							{
								SDEL_TEXT(log->country);
								log->country = Text::StrCopyNew(sarr[8]);
							}
							j = Text::StrSplit(sarr2, 3, sarr[7], ',');
							while (j-- > 0)
							{
								if (Text::StrCharCnt(sarr2[j]) == 6)
								{
									Text::StrHex2Bytes(sarr2[j], &buff[2]);
									k = 0;
									while (k < 3)
									{
										if (buff[2] == log->ouis[k][0] && buff[3] == log->ouis[k][1] && buff[4] == log->ouis[k][2])
										{
											break;
										}
										else if (log->ouis[k][0] == 0 && log->ouis[k][1] == 0 && log->ouis[k][2] == 2)
										{
											log->ouis[k][0] = buff[2];
											log->ouis[k][1] = buff[3];
											log->ouis[k][2] = buff[4];
											break;
										}
										k++;
									}
								}
							}
						}
						if (i >= 10)
						{
							sarr2[1] = sarr[9];
							if (sarr2[1][0])
							{
								UInt64 iMAC;
								while (true)
								{
									j = Text::StrSplit(sarr2, 2, sarr2[1], ',');
									iMAC = Text::StrHex2UInt64(sarr2[0]);
									k = 0;
									while (k < 20)
									{
										if (log->neighbour[k] == 0)
										{
											log->neighbour[k] = iMAC;
											break;
										}
										else if ((log->neighbour[k] & 0xFFFFFFFFFFFFLL) == (iMAC & 0xFFFFFFFFFFFFLL))
										{
											if ((Int8)((log->neighbour[k] >> 48) & 0xff) < (Int8)((iMAC >> 48) & 0xff))
											{
												log->neighbour[k] = iMAC;
											}
											break;
										}
										k++;
									}
									if (j != 2)
									{
										break;
									}
								}
							}
						}
						if (i >= 11)
						{
							UInt32 ieLen = (UInt32)(Text::StrCharCnt(sarr[10]) >> 1);
							if (ieLen > log->ieLen)
							{
								log->ieLen = ieLen;
								if (log->ieBuff)
								{
									MemFree(log->ieBuff);
								}
								log->ieBuff = MemAlloc(UInt8, log->ieLen);
								Text::StrHex2Bytes(sarr[10], log->ieBuff);
							}
						}
					}
					else
					{
						log = MemAlloc(SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry, 1);
						MemClear(log->neighbour, sizeof(log->neighbour));
						log->mac[0] = buff[2];
						log->mac[1] = buff[3];
						log->mac[2] = buff[4];
						log->mac[3] = buff[5];
						log->mac[4] = buff[6];
						log->mac[5] = buff[7];
						log->macInt = iMAC;
						log->ssid = Text::StrCopyNew(sarr[1]);
						log->phyType = Text::StrToInt32(sarr[2]);
						log->freq = Text::StrToDouble(sarr[3]);
						if (i >= 7)
						{
							log->manuf = Text::StrCopyNew(sarr[4]);
							log->model = Text::StrCopyNew(sarr[5]);
							log->serialNum = Text::StrCopyNew(sarr[6]);
						}
						else
						{
							log->manuf = 0;
							log->model = 0;
							log->serialNum = 0;
						}
						j = 3;
						while (j-- > 0)
						{
							log->ouis[j][0] = 0;
							log->ouis[j][1] = 0;
							log->ouis[j][2] = 0;
						}
						if (i >= 9)
						{
							log->country = Text::StrCopyNew(sarr[8]);
							j = Text::StrSplit(sarr2, 3, sarr[7], ',');
							while (j-- > 0)
							{
								if (Text::StrCharCnt(sarr2[j]) == 6)
								{
									Text::StrHex2Bytes(sarr2[j], log->ouis[j]);
								}
							}
						}
						else
						{
							log->country = 0;
						}
						if (i >= 10)
						{
							sarr2[1] = sarr[9];
							if (sarr2[1][0])
							{
								j = 0;
								while (Text::StrSplit(sarr2, 2, sarr2[1], ',') == 2)
								{
									log->neighbour[j] = Text::StrHex2UInt64(sarr2[0]);
									j++;
								}
								log->neighbour[j] = Text::StrHex2UInt64(sarr2[0]);
							}
						}
						if (i >= 11)
						{
							log->ieLen = (UInt32)(Text::StrCharCnt(sarr[10]) >> 1);
							if (log->ieLen > 0)
							{
								log->ieBuff = MemAlloc(UInt8, log->ieLen);
								Text::StrHex2Bytes(sarr[10], log->ieBuff);
							}
							else
							{
								log->ieBuff = 0;
							}
						}
						else
						{
							log->ieBuff = 0;
							log->ieLen = 0;
						}
						
						this->LogInsert(log);
					}
				}
			}
			sb.ClearStr();
		}

		DEL_CLASS(reader);

	}
	DEL_CLASS(fs);
}

Bool SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileStore()
{
	UTF8Char sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"WiFiLog.txt");
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	IO::WriteCacheStream *cstm;
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry *log;
	Bool succ = false;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(cstm, IO::WriteCacheStream(fs));
	NEW_CLASS(writer, Text::UTF8Writer(cstm));
	succ = true;
	i = 0;
	j = this->logList->GetCount();
	while (i < j)
	{
		log = this->logList->GetItem(i);
		sb.ClearStr();
		sb.AppendHexBuff(log->mac, 6, ':', Text::LBT_NONE);
		sb.Append((const UTF8Char*)"\t");
		sb.Append(log->ssid);
		sb.Append((const UTF8Char*)"\t");
		sb.AppendI32(log->phyType);
		sb.Append((const UTF8Char*)"\t");
		Text::SBAppendF64(&sb, log->freq);
		sb.Append((const UTF8Char*)"\t");
		if (log->manuf)
		{
			sb.Append(log->manuf);
		}
		sb.Append((const UTF8Char*)"\t");
		if (log->model)
		{
			sb.Append(log->model);
		}
		sb.Append((const UTF8Char*)"\t");
		if (log->serialNum)
		{
			sb.Append(log->serialNum);
		}
		sb.Append((const UTF8Char*)"\t");
		sb.AppendHexBuff(log->ouis[0], 3, 0, Text::LBT_NONE);
		sb.AppendChar(',', 1);
		sb.AppendHexBuff(log->ouis[1], 3, 0, Text::LBT_NONE);
		sb.AppendChar(',', 1);
		sb.AppendHexBuff(log->ouis[2], 3, 0, Text::LBT_NONE);
		sb.Append((const UTF8Char*)"\t");
		if (log->country)
		{
			sb.Append(log->country);
		}
		sb.AppendChar('\t', 1);
		k = 0;
		while (k < 20)
		{
			if (log->neighbour[k] == 0)
			{
				break;
			}
			if (k > 0)
			{
				sb.AppendChar(',', 1);
			}
			sb.AppendHex64(log->neighbour[k]);
			k++;
		}
		sb.AppendChar('\t', 1);
		if (log->ieLen > 0)
		{
			sb.AppendHexBuff(log->ieBuff, log->ieLen, 0, Text::LBT_NONE);
		}
		if (!writer->WriteLine(sb.ToString()))
		{
			succ = false;
		}
		i++;
	}
	DEL_CLASS(writer);
	DEL_CLASS(cstm);
	DEL_CLASS(fs);
	return succ;
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::LogClear()
{
	UOSInt i = this->logList->GetCount();
	SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry *log;
	while (i-- > 0)
	{
		log = this->logList->GetItem(i);
		SDEL_TEXT(log->ssid);
		SDEL_TEXT(log->manuf);
		SDEL_TEXT(log->model);
		SDEL_TEXT(log->serialNum);
		SDEL_TEXT(log->country);
		if (log->ieBuff)
		{
			MemFree(log->ieBuff);
		}
		MemFree(log);
	}
	this->logList->Clear();
}

SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry *SSWR::AVIRead::AVIRWiFiLogManagerForm::LogGet(UInt64 iMAC)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry *log;
	OSInt i;
	OSInt j;
	OSInt k;
	i = 0;
	j = (OSInt)this->logList->GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		log = this->logList->GetItem((UOSInt)k);
		if (iMAC > log->macInt)
		{
			i = k + 1;
		}
		else if (iMAC < log->macInt)
		{
			j = k - 1;
		}
		else
		{
			return log;
		}
	}
	return 0;
}

OSInt SSWR::AVIRead::AVIRWiFiLogManagerForm::LogInsert(SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry *newLog)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry *log;
	OSInt i;
	OSInt j;
	OSInt k;
	i = 0;
	j = (OSInt)this->logList->GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		log = this->logList->GetItem((UOSInt)k);
		if (newLog->macInt > log->macInt)
		{
			i = k + 1;
		}
		else if (newLog->macInt < log->macInt)
		{
			j = k - 1;
		}
		else
		{
			this->logList->Insert((UOSInt)k, newLog);
			return k;
		}
	}
	this->logList->Insert((UOSInt)i, newLog);
	return i;
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::LogUIUpdate()
{
	const Net::MACInfo::MACEntry *entry;
	SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry *log;
	UTF8Char sbuff[64];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt cnt;
	this->lvContent->ClearItems();
	i = 0;
	j = this->logList->GetCount();
	while (i < j)
	{
		log = this->logList->GetItem(i);
		Text::StrHexBytes(sbuff, log->mac, 6, ':');
		this->lvContent->AddItem(sbuff, log);
		entry = this->macList->GetEntry(log->macInt);
		if (entry)
		{
			this->lvContent->SetSubItem(i, 1, (const UTF8Char*)entry->name);
		}
		else
		{
			this->lvContent->SetSubItem(i, 1, (const UTF8Char*)"?");
		}
		this->lvContent->SetSubItem(i, 2, log->ssid);
		Text::StrInt32(sbuff, log->phyType);
		this->lvContent->SetSubItem(i, 3, sbuff);
		Text::StrDouble(sbuff, log->freq);
		this->lvContent->SetSubItem(i, 4, sbuff);
		if (log->manuf)
			this->lvContent->SetSubItem(i, 5, log->manuf);
		if (log->model)
			this->lvContent->SetSubItem(i, 6, log->model);
		if (log->serialNum)
			this->lvContent->SetSubItem(i, 7, log->serialNum);
		if (log->ouis[0][0] != 0 || log->ouis[0][1] != 0 || log->ouis[0][2] != 0)
			this->lvContent->SetSubItem(i, 8, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(log->ouis[0])->name);
		if (log->ouis[1][0] != 0 || log->ouis[1][1] != 0 || log->ouis[1][2] != 0)
			this->lvContent->SetSubItem(i, 9, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(log->ouis[1])->name);
		if (log->ouis[2][0] != 0 || log->ouis[2][1] != 0 || log->ouis[2][2] != 0)
			this->lvContent->SetSubItem(i, 10, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(log->ouis[2])->name);
		if (log->country)
			this->lvContent->SetSubItem(i, 11, log->country);
		cnt = 0;
		k = 0;
		while (k < 20)
		{
			if (log->neighbour[k])
				cnt++;
			k++;
		}
		Text::StrUOSInt(sbuff, cnt);
		this->lvContent->SetSubItem(i, 12, sbuff);
		i++;
	}
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::UpdateStatus()
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(this->macList->GetCount());
	sb.Append((const UTF8Char*)" Records");
	this->lblInfo->SetText(sb.ToString());
}

SSWR::AVIRead::AVIRWiFiLogManagerForm::AVIRWiFiLogManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"WiFi Log Manager");

	this->core = core;
	NEW_CLASS(this->logList, Data::ArrayList<SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileEntry*>());
	NEW_CLASS(this->macList, Net::MACInfoList());

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnFile, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Open Log"));
	this->btnFile->SetRect(4, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	NEW_CLASS(this->btnStore, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Store MACList"));
	this->btnStore->SetRect(84, 4, 75, 23, false);
	this->btnStore->HandleButtonClick(OnStoreClicked, this);
	NEW_CLASS(this->lblInfo, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)""));
	this->lblInfo->SetRect(164, 4, 200, 23, false);
	NEW_CLASS(this->txtFileIE, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtFileIE->SetRect(0, 0, 100, 255, false);
	this->txtFileIE->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtFileIE->SetReadOnly(true);
	NEW_CLASS(this->vspFile, UI::GUIVSplitter(ui, this, 3, true));
	NEW_CLASS(this->lvContent, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 13));
	this->lvContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvContent->SetShowGrid(true);
	this->lvContent->SetFullRowSelect(true);
	this->lvContent->HandleDblClk(OnContentDblClicked, this);
	this->lvContent->HandleSelChg(OnContentSelChg, this);
	this->lvContent->AddColumn((const UTF8Char*)"MAC", 120);
	this->lvContent->AddColumn((const UTF8Char*)"Vendor", 120);
	this->lvContent->AddColumn((const UTF8Char*)"SSID", 200);
	this->lvContent->AddColumn((const UTF8Char*)"PHYType", 60);
	this->lvContent->AddColumn((const UTF8Char*)"Frequency", 80);
	this->lvContent->AddColumn((const UTF8Char*)"Manufacturer", 100);
	this->lvContent->AddColumn((const UTF8Char*)"Model", 100);
	this->lvContent->AddColumn((const UTF8Char*)"S/N", 100);
	this->lvContent->AddColumn((const UTF8Char*)"Vendor1", 120);
	this->lvContent->AddColumn((const UTF8Char*)"Vendor2", 120);
	this->lvContent->AddColumn((const UTF8Char*)"Vendor3", 120);
	this->lvContent->AddColumn((const UTF8Char*)"Country", 50);
	this->lvContent->AddColumn((const UTF8Char*)"Neigbour Cnt", 50);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->UpdateStatus();

	UTF8Char sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"WiFiLog.txt");
	this->LogFileLoad(sbuff);
	this->LogUIUpdate();
}

SSWR::AVIRead::AVIRWiFiLogManagerForm::~AVIRWiFiLogManagerForm()
{
	this->LogClear();
	DEL_CLASS(this->logList);
	DEL_CLASS(this->macList);
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
