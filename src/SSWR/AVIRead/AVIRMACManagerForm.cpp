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

#include <stdio.h>
void __stdcall SSWR::AVIRead::AVIRMACManagerForm::OnFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMACManagerForm *me = (SSWR::AVIRead::AVIRMACManagerForm*)userObj;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"MACManagerFile", false);
	dlg.SetAllowMultiSel(false);
	dlg.AddFilter(CSTR("*.txt"), CSTR("Log File"));
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->LogFileLoad(dlg.GetFileName()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRMACManagerForm::OnStoreClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMACManagerForm *me = (SSWR::AVIRead::AVIRMACManagerForm*)userObj;
	if (me->macList.Store())
	{
		me->ui->ShowMsgOK(CSTR("Data Stored"), CSTR("MAC Manager"), me);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in storing data"), CSTR("MAC Manager"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRMACManagerForm::OnContentDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRMACManagerForm *me = (SSWR::AVIRead::AVIRMACManagerForm*)userObj;
	SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry *log = me->logList.GetItem(index);
	if (log == 0)
		return;
	const Net::MACInfo::MACEntry *entry = me->macList.GetEntry(log->macInt);
	Text::CString name;
	if (entry)
	{
		name = {entry->name, entry->nameLen};
	}
	else
	{
		name = CSTR_NULL;
	}
	SSWR::AVIRead::AVIRMACManagerEntryForm frm(0, me->ui, me->core, log->mac, name);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		NotNullPtr<Text::String> name = frm.GetNameNew();
		UOSInt i = me->macList.SetEntry(log->macInt, name->ToCString());
		name->Release();
		entry = me->macList.GetItem(i);
		me->UpdateStatus();

		UOSInt j;
		i = 0;
		j = me->logList.GetCount();
		while (i < j)
		{
			log = me->logList.GetItem(i);
			if (log->macInt >= entry->rangeStart && log->macInt <= entry->rangeEnd)
			{
				me->lvContent->SetSubItem(i, 1, {entry->name, entry->nameLen});
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMACManagerForm::OnContentSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRMACManagerForm *me = (SSWR::AVIRead::AVIRMACManagerForm*)userObj;
	SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry *log = (SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry*)me->lvContent->GetSelectedItem();
	if (log == 0 || log->ieLen <= 0)
	{
		me->txtFileIE->SetText(CSTR(""));
		return;
	}
	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	while (i < log->ieLen)
	{
		Net::WirelessLANIE::ToString(&log->ieBuff[i], sb);
		sb.AppendC(UTF8STRC("\r\n"));
		i += (UOSInt)log->ieBuff[i + 1] + 2;
	}
	me->txtFileIE->SetText(sb.ToCString());
}

void __stdcall SSWR::AVIRead::AVIRMACManagerForm::OnInputClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMACManagerForm *me = (SSWR::AVIRead::AVIRMACManagerForm*)userObj;
	UInt8 buff[16];
	UTF8Char sbuff[2];
	UOSInt i;
	Text::StringBuilderUTF8 sb;
	me->txtInput->GetText(sb);
	if (sb.GetLength() < 6 || sb.GetLength() > 28)
	{
		me->ui->ShowMsgOK(CSTR("Invalid input"), CSTR("MAC Manager"), me);
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
		me->ui->ShowMsgOK(CSTR("Invalid input"), CSTR("MAC Manager"), me);
		return;
	}
	while (i < 6)
	{
		buff[i + 2] = 0;
		i++;
	}
	UInt64 macInt = ReadMUInt64(buff);
	const Net::MACInfo::MACEntry *entry = me->macList.GetEntry(macInt);
	Text::CString name;
	if (entry)
	{
		name = {entry->name, entry->nameLen};
	}
	else
	{
		name = CSTR_NULL;
	}
	SSWR::AVIRead::AVIRMACManagerEntryForm frm(0, me->ui, me->core, &buff[2], name);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		NotNullPtr<Text::String> name = frm.GetNameNew();
		i = me->macList.SetEntry(macInt, name->ToCString());
		name->Release();
		me->UpdateStatus();
		entry = me->macList.GetItem(i);

		SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry *log;
		UOSInt j;
		i = 0;
		j = me->logList.GetCount();
		while (i < j)
		{
			log = me->logList.GetItem(i);
			if (log->macInt >= entry->rangeStart && log->macInt <= entry->rangeEnd)
			{
				me->lvContent->SetSubItem(i, 1, {entry->name, entry->nameLen});
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMACManagerForm::OnWiresharkClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMACManagerForm *me = (SSWR::AVIRead::AVIRMACManagerForm*)userObj;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"MACManagerWiresharkFile", false);
	dlg.SetAllowMultiSel(false);
	dlg.AddFilter(CSTR("manuf"), CSTR("Wireshark manuf File"));
	if (dlg.ShowDialog(me->GetHandle()))
	{
		Text::StringBuilderUTF8 sb;
		Text::PString sarr[3];
		UOSInt i;
		UOSInt j;
		{
			IO::FileStream fs(dlg.GetFileName(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			Text::UTF8Reader reader(fs);
			while (true)
			{
				sb.ClearStr();
				if (!reader.ReadLine(sb, 512))
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
					else if (j > 18 && sarr[0].v[17] == '/' && Text::StrToUOSInt(&sarr[0].v[18], bitCnt))
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
						const Net::MACInfo::MACEntry *entry = me->macList.GetEntry(startAddr);
						if (sarr[2].Equals(UTF8STRC("IEEE Registration Authority")))
						{

						}
						else if (entry && (entry->rangeStart != startAddr || entry->rangeEnd != endAddr))
						{
							printf("Range mismatch: %llx - %llx\r\n", startAddr, endAddr);
						}
						else
						{
							me->macList.SetEntry(startAddr, endAddr, sarr[2].ToCString());
						}
					}
				}
			}
		}
		me->UpdateStatus();
	}
}

void SSWR::AVIRead::AVIRMACManagerForm::LogFileLoad(Text::CStringNN fileName)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::PString sarr[12];
		Text::PString sarr2[7];
		UInt8 buff[8];
		UOSInt i;
		UOSInt j;
		SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry *log;
		Text::StringBuilderUTF8 sb;
		{
			Text::UTF8Reader reader(fs);
			sb.ClearStr();
			this->LogFileClear();
			buff[0] = 0;
			buff[1] = 0;
			while (reader.ReadLine(sb, 4096))
			{
				i = Text::StrSplitP(sarr, 12, sb, '\t');
				if (i == 4 || i == 7 || i == 9 || i == 10 || i == 11)
				{
					if (Text::StrSplitP(sarr2, 7, sarr[0], ':') == 6)
					{
						log = MemAlloc(SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry, 1);
						MemClear(log->neighbour, sizeof(log->neighbour));
						log->mac[0] = Text::StrHex2UInt8C(sarr2[0].v);
						log->mac[1] = Text::StrHex2UInt8C(sarr2[1].v);
						log->mac[2] = Text::StrHex2UInt8C(sarr2[2].v);
						log->mac[3] = Text::StrHex2UInt8C(sarr2[3].v);
						log->mac[4] = Text::StrHex2UInt8C(sarr2[4].v);
						log->mac[5] = Text::StrHex2UInt8C(sarr2[5].v);
						buff[2] = log->mac[0];
						buff[3] = log->mac[1];
						buff[4] = log->mac[2];
						buff[5] = log->mac[3];
						buff[6] = log->mac[4];
						buff[7] = log->mac[5];
						log->macInt = ReadMUInt64(buff);
						log->ssid = Text::String::New(sarr[1].v, sarr[1].leng);
						log->phyType = Text::StrToInt32(sarr[2].v);
						log->freq = Text::StrToDouble(sarr[3].v);
						if (i >= 7)
						{
							log->manuf = Text::String::New(sarr[4].v, sarr[4].leng).Ptr();
							log->model = Text::String::New(sarr[5].v, sarr[5].leng).Ptr();
							log->serialNum = Text::String::New(sarr[6].v, sarr[6].leng).Ptr();
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
							log->country = Text::String::New(sarr[8].v, sarr[8].leng).Ptr();
							j = Text::StrSplitP(sarr2, 3, sarr[7], ',');
							while (j-- > 0)
							{
								if (sarr2[j].leng == 6)
								{
									Text::StrHex2Bytes(sarr2[j].v, log->ouis[j]);
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
							if (sarr2[1].v[0])
							{
								j = 0;
								while (Text::StrSplitP(sarr2, 2, sarr2[1], ',') == 2)
								{
									log->neighbour[j] = Text::StrHex2UInt64C(sarr2[0].v);
									j++;
								}
								log->neighbour[j] = Text::StrHex2UInt64C(sarr2[0].v);
							}
						}
						if (i >= 11)
						{
							log->ieLen = (UInt32)(sarr[10].leng >> 1);
							if (log->ieLen > 0)
							{
								log->ieBuff = MemAlloc(UInt8, log->ieLen);
								Text::StrHex2Bytes(sarr[10].v, log->ieBuff);
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
						

						this->logList.Add(log);
					}
				}
				sb.ClearStr();
			}

			this->txtFile->SetText(fileName);
		}

		const Net::MACInfo::MACEntry *entry;
		NotNullPtr<Text::String> s;
		this->lvContent->BeginUpdate();
		this->lvContent->ClearItems();
		i = 0;
		j = this->logList.GetCount();
		while (i < j)
		{
			log = this->logList.GetItem(i);
			sptr = Text::StrHexBytes(sbuff, log->mac, 6, ':');
			this->lvContent->AddItem(CSTRP(sbuff, sptr), log);
			entry = this->macList.GetEntry(log->macInt);
			if (entry)
			{
				this->lvContent->SetSubItem(i, 1, {entry->name, entry->nameLen});
			}
			else
			{
				this->lvContent->SetSubItem(i, 1, CSTR("?"));
			}
			this->lvContent->SetSubItem(i, 2, log->ssid);
			sptr = Text::StrInt32(sbuff, log->phyType);
			this->lvContent->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, log->freq);
			this->lvContent->SetSubItem(i, 4, CSTRP(sbuff, sptr));
			if (s.Set(log->manuf))
				this->lvContent->SetSubItem(i, 5, s);
			if (s.Set(log->model))
				this->lvContent->SetSubItem(i, 6, s);
			if (s.Set(log->serialNum))
				this->lvContent->SetSubItem(i, 7, s);
			if (log->ouis[0][0] != 0 || log->ouis[0][1] != 0 || log->ouis[0][2] != 0)
			{
				entry = Net::MACInfo::GetMACInfoOUI(log->ouis[0]);
				this->lvContent->SetSubItem(i, 8, {entry->name, entry->nameLen});
			}
			if (log->ouis[1][0] != 0 || log->ouis[1][1] != 0 || log->ouis[1][2] != 0)
			{
				entry = Net::MACInfo::GetMACInfoOUI(log->ouis[1]);
				this->lvContent->SetSubItem(i, 9, {entry->name, entry->nameLen});
			}
			if (log->ouis[2][0] != 0 || log->ouis[2][1] != 0 || log->ouis[2][2] != 0)
			{
				entry = Net::MACInfo::GetMACInfoOUI(log->ouis[2]);
				this->lvContent->SetSubItem(i, 10, {entry->name, entry->nameLen});
			}
			if (s.Set(log->country))
				this->lvContent->SetSubItem(i, 11, s);
			i++;
		}
		this->lvContent->EndUpdate();
	}
}

void SSWR::AVIRead::AVIRMACManagerForm::LogFileClear()
{
	UOSInt i = this->logList.GetCount();
	SSWR::AVIRead::AVIRMACManagerForm::LogFileEntry *log;
	while (i-- > 0)
	{
		log = this->logList.GetItem(i);
		log->ssid->Release();
		SDEL_STRING(log->manuf);
		SDEL_STRING(log->model);
		SDEL_STRING(log->serialNum);
		SDEL_STRING(log->country);
		if (log->ieBuff)
		{
			MemFree(log->ieBuff);
		}
		MemFree(log);
	}
	this->logList.Clear();
}

void SSWR::AVIRead::AVIRMACManagerForm::UpdateStatus()
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(this->macList.GetCount());
	sb.AppendC(UTF8STRC(" Records"));
	this->lblInfo->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRMACManagerForm::AVIRMACManagerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MAC Manager"));

	this->core = core;

	NEW_CLASSNN(this->pnlControl, UI::GUIPanel(ui, *this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnStore = ui->NewButton(this->pnlControl, CSTR("Store"));
	this->btnStore->SetRect(4, 4, 75, 23, false);
	this->btnStore->HandleButtonClick(OnStoreClicked, this);
	this->lblInfo = ui->NewLabel(this->pnlControl, CSTR(""));
	this->lblInfo->SetRect(84, 4, 200, 23, false);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpFile = this->tcMain->AddTabPage(CSTR("WiFi Log"));
	NEW_CLASSNN(this->pnlFile, UI::GUIPanel(ui, this->tpFile));
	this->pnlFile->SetRect(0, 0, 100, 31, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFile = ui->NewLabel(this->pnlFile, CSTR("Log File"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFile, UI::GUITextBox(ui, this->pnlFile, CSTR("")));
	this->txtFile->SetRect(104, 4, 400, 23, false);
	this->txtFile->SetReadOnly(true);
	this->btnFile = ui->NewButton(this->pnlFile, CSTR("Open"));
	this->btnFile->SetRect(504, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	NEW_CLASS(this->txtFileIE, UI::GUITextBox(ui, this->tpFile, CSTR(""), true));
	this->txtFileIE->SetRect(0, 0, 100, 256, false);
	this->txtFileIE->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtFileIE->SetReadOnly(true);
	this->vspFile = ui->NewVSplitter(this->tpFile, 3, true);
	NEW_CLASS(this->lvContent, UI::GUIListView(ui, this->tpFile, UI::GUIListView::LVSTYLE_TABLE, 12));
	this->lvContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvContent->SetShowGrid(true);
	this->lvContent->SetFullRowSelect(true);
	this->lvContent->HandleDblClk(OnContentDblClicked, this);
	this->lvContent->HandleSelChg(OnContentSelChg, this);
	this->lvContent->AddColumn(CSTR("MAC"), 120);
	this->lvContent->AddColumn(CSTR("Vendor"), 120);
	this->lvContent->AddColumn(CSTR("SSID"), 200);
	this->lvContent->AddColumn(CSTR("PHYType"), 60);
	this->lvContent->AddColumn(CSTR("Frequency"), 80);
	this->lvContent->AddColumn(CSTR("Manufacturer"), 100);
	this->lvContent->AddColumn(CSTR("Model"), 100);
	this->lvContent->AddColumn(CSTR("S/N"), 100);
	this->lvContent->AddColumn(CSTR("Vendor1"), 120);
	this->lvContent->AddColumn(CSTR("Vendor2"), 120);
	this->lvContent->AddColumn(CSTR("Vendor3"), 120);
	this->lvContent->AddColumn(CSTR("Country"), 50);

	this->tpInput = this->tcMain->AddTabPage(CSTR("Input"));
	this->lblInput = ui->NewLabel(this->tpInput, CSTR("MAC Input"));
	this->lblInput->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtInput, UI::GUITextBox(ui, this->tpInput, CSTR("")));
	this->txtInput->SetRect(104, 4, 150, 23, false);
	this->btnInput = ui->NewButton(this->tpInput, CSTR("&Edit"));
	this->btnInput->SetRect(104, 28, 75, 23, false);
	this->btnInput->HandleButtonClick(OnInputClicked, this);

	this->tpWireshark = this->tcMain->AddTabPage(CSTR("Wireshark"));
	this->btnWireshark = ui->NewButton(this->tpWireshark, CSTR("Load manuf"));
	this->btnWireshark->SetRect(4, 4, 75, 23, false);
	this->btnWireshark->HandleButtonClick(OnWiresharkClicked, this);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->UpdateStatus();
}

SSWR::AVIRead::AVIRMACManagerForm::~AVIRMACManagerForm()
{
	this->LogFileClear();
}

void SSWR::AVIRead::AVIRMACManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
