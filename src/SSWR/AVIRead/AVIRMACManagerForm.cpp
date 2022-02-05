#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/MACInfo.h"
#include "Net/WirelessLANIE.h"
#include "SSWR/AVIRead/AVIRMACManagerEntryForm.h"
#include "SSWR/AVIRead/AVIRMACManagerForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

#include <stdio.h>
void __stdcall SSWR::AVIRead::AVIRMACManagerForm::OnFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMACManagerForm *me = (SSWR::AVIRead::AVIRMACManagerForm*)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"MACManagerFile", false));
	dlg->SetAllowMultiSel(false);
	dlg->AddFilter((const UTF8Char*)"*.txt", (const UTF8Char*)"Log File");
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->LogFileLoad(dlg->GetFileName()->ToCString());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRMACManagerForm::OnStoreClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMACManagerForm *me = (SSWR::AVIRead::AVIRMACManagerForm*)userObj;
	if (me->macList->Store())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Data Stored", (const UTF8Char*)"MAC Manager", me);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in storing data", (const UTF8Char*)"MAC Manager", me);
	}
}

void __stdcall SSWR::AVIRead::AVIRMACManagerForm::OnContentDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRMACManagerForm *me = (SSWR::AVIRead::AVIRMACManagerForm*)userObj;
	SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry *log = me->logList->GetItem(index);
	if (log == 0)
		return;
	const Net::MACInfo::MACEntry *entry = me->macList->GetEntry(log->macInt);
	SSWR::AVIRead::AVIRMACManagerEntryForm *frm;
	if (entry)
	{
		NEW_CLASS(frm, SSWR::AVIRead::AVIRMACManagerEntryForm(0, me->ui, me->core, log->mac, entry->name));
	}
	else
	{
		NEW_CLASS(frm, SSWR::AVIRead::AVIRMACManagerEntryForm(0, me->ui, me->core, log->mac, 0));
	}
	if (frm->ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		Text::String *name = frm->GetNameNew();
		UOSInt i = me->macList->SetEntry(log->macInt, name->ToCString());
		name->Release();
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
				me->lvContent->SetSubItem(i, 1, entry->name);
			}
			i++;
		}
	}
	DEL_CLASS(frm);

}

void __stdcall SSWR::AVIRead::AVIRMACManagerForm::OnContentSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRMACManagerForm *me = (SSWR::AVIRead::AVIRMACManagerForm*)userObj;
	SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry *log = (SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry*)me->lvContent->GetSelectedItem();
	if (log == 0 || log->ieLen <= 0)
	{
		me->txtFileIE->SetText((const UTF8Char*)"");
		return;
	}
	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	while (i < log->ieLen)
	{
		Net::WirelessLANIE::ToString(&log->ieBuff[i], &sb);
		sb.AppendC(UTF8STRC("\r\n"));
		i += (UOSInt)log->ieBuff[i + 1] + 2;
	}
	me->txtFileIE->SetText(sb.ToString());
}

void __stdcall SSWR::AVIRead::AVIRMACManagerForm::OnInputClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMACManagerForm *me = (SSWR::AVIRead::AVIRMACManagerForm*)userObj;
	UInt8 buff[16];
	UTF8Char sbuff[2];
	UOSInt i;
	Text::StringBuilderUTF8 sb;
	me->txtInput->GetText(&sb);
	if (sb.GetLength() < 6 || sb.GetLength() > 28)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Invalid input", (const UTF8Char*)"MAC Manager", me);
		return;
	}
	sbuff[0] = sb.ToString()[2];
	sbuff[1] = 0;
	if (sbuff[0] >= '0' && sbuff[0] <= '9')
	{

	}
	else if (sbuff[0] >= 'A' && sbuff[0] >= 'F')
	{

	}
	else if (sbuff[0] >= 'a' && sbuff[0] >= 'f')
	{
		
	}
	else
	{
		sb.ReplaceStr(sbuff, 1, UTF8STRC(""));
	}
	buff[0] = 0;
	buff[1] = 0;
	i = sb.Hex2Bytes(&buff[2]);
	if (i < 3)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Invalid input", (const UTF8Char*)"MAC Manager", me);
		return;
	}
	while (i < 6)
	{
		buff[i + 2] = 0;
		i++;
	}
	UInt64 macInt = ReadMUInt64(buff);
	const Net::MACInfo::MACEntry *entry = me->macList->GetEntry(macInt);
	SSWR::AVIRead::AVIRMACManagerEntryForm *frm;
	if (entry)
	{
		NEW_CLASS(frm, SSWR::AVIRead::AVIRMACManagerEntryForm(0, me->ui, me->core, &buff[2], entry->name));
	}
	else
	{
		NEW_CLASS(frm, SSWR::AVIRead::AVIRMACManagerEntryForm(0, me->ui, me->core, &buff[2], 0));
	}
	if (frm->ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		Text::String *name = frm->GetNameNew();
		i = me->macList->SetEntry(macInt, name->ToCString());
		name->Release();
		me->UpdateStatus();
		entry = me->macList->GetItem(i);

		SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry *log;
		UOSInt j;
		i = 0;
		j = me->logList->GetCount();
		while (i < j)
		{
			log = me->logList->GetItem(i);
			if (log->macInt >= entry->rangeStart && log->macInt <= entry->rangeEnd)
			{
				me->lvContent->SetSubItem(i, 1, entry->name);
			}
			i++;
		}
	}
	DEL_CLASS(frm);
}

void __stdcall SSWR::AVIRead::AVIRMACManagerForm::OnWiresharkClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMACManagerForm *me = (SSWR::AVIRead::AVIRMACManagerForm*)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"MACManagerWiresharkFile", false));
	dlg->SetAllowMultiSel(false);
	dlg->AddFilter((const UTF8Char*)"manuf", (const UTF8Char*)"Wireshark manuf File");
	if (dlg->ShowDialog(me->GetHandle()))
	{
		IO::FileStream *fs;
		Text::UTF8Reader *reader;
		Text::StringBuilderUTF8 sb;
		Text::PString sarr[3];
		UOSInt i;
		UOSInt j;
		NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		while (true)
		{
			sb.ClearStr();
			if (!reader->ReadLine(&sb, 512))
			{
				break;
			}

			i = sb.IndexOf('#');
			if (i != INVALID_INDEX)
			{
				sb.TrimToLength(i);
			}
			sb.RTrim();
			j = Text::StrSplitP(sarr, 3, sb, '\t');
			if (j == 2 || j == 3)
			{
				UInt8 buff[8];
				UOSInt bitCnt;
				Bool succ;
				UInt64 startAddr;
				UInt64 endAddr;
				if (j == 2)
				{
					sarr[2] = sarr[1];
				}
				succ = false;
				j = sarr[0].leng;
				if (j == 8 && sarr[0].v[2] == ':' && sarr[0].v[5] == ':')
				{
					buff[0] = 0;
					buff[1] = 0;
					buff[2] = Text::StrHex2UInt8C(&sarr[0].v[0]);
					buff[3] = Text::StrHex2UInt8C(&sarr[0].v[3]);
					buff[4] = Text::StrHex2UInt8C(&sarr[0].v[6]);
					buff[5] = 0;
					buff[6] = 0;
					buff[7] = 0;
					startAddr = ReadMUInt64(buff);
					endAddr = startAddr | 0xffffff;
					succ = true;
				}
				else if (j > 18 && sarr[0].v[17] == '/' && Text::StrToUOSInt(&sarr[0].v[18], &bitCnt))
				{
					buff[0] = 0;
					buff[1] = 0;
					buff[2] = Text::StrHex2UInt8C(&sarr[0].v[0]);
					buff[3] = Text::StrHex2UInt8C(&sarr[0].v[3]);
					buff[4] = Text::StrHex2UInt8C(&sarr[0].v[6]);
					buff[5] = Text::StrHex2UInt8C(&sarr[0].v[9]);
					buff[6] = Text::StrHex2UInt8C(&sarr[0].v[12]);
					buff[7] = Text::StrHex2UInt8C(&sarr[0].v[15]);
					startAddr = ReadMUInt64(buff);
					endAddr = startAddr | (((UInt64)1 << (48 - bitCnt)) - 1);
					succ = true;
				}
				else
				{
					printf("Error in file2: %s\r\n", sarr[0].v);
				}

				if (succ)
				{
					const Net::MACInfo::MACEntry *entry = me->macList->GetEntry(startAddr);
					if (sarr[2].Equals(UTF8STRC("IEEE Registration Authority")))
					{

					}
					else if (entry && (entry->rangeStart != startAddr || entry->rangeEnd != endAddr))
					{
						printf("Range mismatch: %llx - %llx\r\n", startAddr, endAddr);
					}
					else
					{
						me->macList->SetEntry(startAddr, endAddr, sarr[2].ToCString());
					}
				}
			}
		}
		DEL_CLASS(reader);
		DEL_CLASS(fs);
		me->UpdateStatus();
	}
	DEL_CLASS(dlg);
	
}

void SSWR::AVIRead::AVIRMACManagerForm::LogFileLoad(Text::CString fileName)
{
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	UTF8Char sbuff[32];
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		UTF8Char *sarr[12];
		UTF8Char *sarr2[7];
		UInt8 buff[8];
		UOSInt i;
		UOSInt j;
		SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry *log;
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		this->LogFileClear();
		buff[0] = 0;
		buff[1] = 0;
		while (reader->ReadLine(&sb, 4096))
		{
			i = Text::StrSplit(sarr, 12, sb.ToString(), '\t');
			if (i == 4 || i == 7 || i == 9 || i == 10 || i == 11)
			{
				if (Text::StrSplit(sarr2, 7, sarr[0], ':') == 6)
				{
					log = MemAlloc(SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry, 1);
					MemClear(log->neighbour, sizeof(log->neighbour));
					log->mac[0] = Text::StrHex2UInt8C(sarr2[0]);
					log->mac[1] = Text::StrHex2UInt8C(sarr2[1]);
					log->mac[2] = Text::StrHex2UInt8C(sarr2[2]);
					log->mac[3] = Text::StrHex2UInt8C(sarr2[3]);
					log->mac[4] = Text::StrHex2UInt8C(sarr2[4]);
					log->mac[5] = Text::StrHex2UInt8C(sarr2[5]);
					buff[2] = log->mac[0];
					buff[3] = log->mac[1];
					buff[4] = log->mac[2];
					buff[5] = log->mac[3];
					buff[6] = log->mac[4];
					buff[7] = log->mac[5];
					log->macInt = ReadMUInt64(buff);
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
								log->neighbour[j] = Text::StrHex2UInt64C(sarr2[0]);
								j++;
							}
							log->neighbour[j] = Text::StrHex2UInt64C(sarr2[0]);
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
						log->ieLen = 0;
						log->ieBuff = 0;
					}
					

					this->logList->Add(log);
				}
			}
			sb.ClearStr();
		}

		this->txtFile->SetText(fileName.v);
		DEL_CLASS(reader);

		const Net::MACInfo::MACEntry *entry;
		this->lvContent->BeginUpdate();
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
				this->lvContent->SetSubItem(i, 1, entry->name);
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
				this->lvContent->SetSubItem(i, 8, Net::MACInfo::GetMACInfoOUI(log->ouis[0])->name);
			if (log->ouis[1][0] != 0 || log->ouis[1][1] != 0 || log->ouis[1][2] != 0)
				this->lvContent->SetSubItem(i, 9, Net::MACInfo::GetMACInfoOUI(log->ouis[1])->name);
			if (log->ouis[2][0] != 0 || log->ouis[2][1] != 0 || log->ouis[2][2] != 0)
				this->lvContent->SetSubItem(i, 10, Net::MACInfo::GetMACInfoOUI(log->ouis[2])->name);
			if (log->country)
				this->lvContent->SetSubItem(i, 11, log->country);
			i++;
		}
		this->lvContent->EndUpdate();
	}
	DEL_CLASS(fs);
}

void SSWR::AVIRead::AVIRMACManagerForm::LogFileClear()
{
	UOSInt i = this->logList->GetCount();
	SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry *log;
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

void SSWR::AVIRead::AVIRMACManagerForm::UpdateStatus()
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(this->macList->GetCount());
	sb.AppendC(UTF8STRC(" Records"));
	this->lblInfo->SetText(sb.ToString());
}

SSWR::AVIRead::AVIRMACManagerForm::AVIRMACManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"MAC Manager");

	this->core = core;
	NEW_CLASS(this->logList, Data::ArrayList<SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry*>());
	NEW_CLASS(this->macList, Net::MACInfoList());

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnStore, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Store"));
	this->btnStore->SetRect(4, 4, 75, 23, false);
	this->btnStore->HandleButtonClick(OnStoreClicked, this);
	NEW_CLASS(this->lblInfo, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)""));
	this->lblInfo->SetRect(84, 4, 200, 23, false);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpFile = this->tcMain->AddTabPage((const UTF8Char*)"WiFi Log");
	NEW_CLASS(this->pnlFile, UI::GUIPanel(ui, this->tpFile));
	this->pnlFile->SetRect(0, 0, 100, 31, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFile, UI::GUILabel(ui, this->pnlFile, (const UTF8Char*)"Log File"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFile, UI::GUITextBox(ui, this->pnlFile, CSTR("")));
	this->txtFile->SetRect(104, 4, 400, 23, false);
	this->txtFile->SetReadOnly(true);
	NEW_CLASS(this->btnFile, UI::GUIButton(ui, this->pnlFile, (const UTF8Char*)"Open"));
	this->btnFile->SetRect(504, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	NEW_CLASS(this->txtFileIE, UI::GUITextBox(ui, this->tpFile, CSTR(""), true));
	this->txtFileIE->SetRect(0, 0, 100, 256, false);
	this->txtFileIE->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtFileIE->SetReadOnly(true);
	NEW_CLASS(this->vspFile, UI::GUIVSplitter(ui, this->tpFile, 3, true));
	NEW_CLASS(this->lvContent, UI::GUIListView(ui, this->tpFile, UI::GUIListView::LVSTYLE_TABLE, 12));
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

	this->tpInput = this->tcMain->AddTabPage((const UTF8Char*)"Input");
	NEW_CLASS(this->lblInput, UI::GUILabel(ui, this->tpInput, (const UTF8Char*)"MAC Input"));
	this->lblInput->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtInput, UI::GUITextBox(ui, this->tpInput, CSTR("")));
	this->txtInput->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->btnInput, UI::GUIButton(ui, this->tpInput, (const UTF8Char*)"&Edit"));
	this->btnInput->SetRect(104, 28, 75, 23, false);
	this->btnInput->HandleButtonClick(OnInputClicked, this);

	this->tpWireshark = this->tcMain->AddTabPage((const UTF8Char*)"Wireshark");
	NEW_CLASS(this->btnWireshark, UI::GUIButton(ui, this->tpWireshark, (const UTF8Char*)"Load manuf"));
	this->btnWireshark->SetRect(4, 4, 75, 23, false);
	this->btnWireshark->HandleButtonClick(OnWiresharkClicked, this);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->UpdateStatus();
}

SSWR::AVIRead::AVIRMACManagerForm::~AVIRMACManagerForm()
{
	this->LogFileClear();
	DEL_CLASS(this->logList);
	DEL_CLASS(this->macList);
}

void SSWR::AVIRead::AVIRMACManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
