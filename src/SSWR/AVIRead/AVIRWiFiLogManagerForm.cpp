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

#define MAX_ROW 5000

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
			me->wifiLogFile->LoadFile(dlg->GetFileNames(i));
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

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnContentDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm *me = (SSWR::AVIRead::AVIRWiFiLogManagerForm*)userObj;
	const Net::WiFiLogFile::LogFileEntry *log = (const Net::WiFiLogFile::LogFileEntry*)me->lvContent->GetItem(index);
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
		j = me->lvContent->GetCount();
		while (i < j)
		{
			log = (const Net::WiFiLogFile::LogFileEntry*)me->lvContent->GetItem(i);
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
	const Net::WiFiLogFile::LogFileEntry *log = (const Net::WiFiLogFile::LogFileEntry*)me->lvContent->GetSelectedItem();
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
		sb.Append((const UTF8Char*)"\r\n");
		i += (UOSInt)log->ieBuff[i + 1] + 2;
	}
	me->txtFileIE->SetText(sb.ToString());
}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnUnkOnlyChkChg(void *userObj, Bool checked)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm *me = (SSWR::AVIRead::AVIRWiFiLogManagerForm*)userObj;
	me->LogUIUpdate();	
}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnFilterClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm *me = (SSWR::AVIRead::AVIRWiFiLogManagerForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtFilter->GetText(&sb);
	SDEL_TEXT(me->filterText);
	if (sb.GetLength() > 0)
	{
		me->filterText = Text::StrCopyNew(sb.ToString());
	}
	me->LogUIUpdate();
}

Bool SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileStore()
{
	UTF8Char sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"WiFiLog.txt");
	return this->wifiLogFile->StoreFile(sbuff);
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::LogUIUpdate()
{
	const Net::MACInfo::MACEntry *entry;
	const Net::MACInfo::MACEntry *entry2;
	Net::WiFiLogFile::LogFileEntry *log;
	Data::ArrayList<Net::WiFiLogFile::LogFileEntry*> *logList = this->wifiLogFile->GetLogList();
	Bool unkOnly = this->chkUnkOnly->IsChecked();
	UTF8Char sbuff[64];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt cnt;
	UOSInt recCnt = 0;
	Bool valid;
	this->lvContent->ClearItems();
	i = 0;
	j = logList->GetCount();
	while (i < j)
	{
		log = logList->GetItem(i);
		entry = this->macList->GetEntry(log->macInt);
		valid = true;
		if (unkOnly && (entry != 0 && entry->name != 0 && entry->name[0] != 0))
		{
			valid = false;
		}
		else if (this->filterText)
		{
			valid = false;
			if (log->ssid && Text::StrIndexOfICase(log->ssid, this->filterText) != INVALID_INDEX)
			{
				valid = true;
			}
			else if (log->manuf && Text::StrIndexOfICase(log->manuf, this->filterText) != INVALID_INDEX)
			{
				valid = true;
			}
			else if (entry && Text::StrIndexOfICase((const UTF8Char*)entry->name, this->filterText) != INVALID_INDEX)
			{
				valid = true;
			}
			else
			{
				if (!valid && (log->ouis[0][0] != 0 || log->ouis[0][1] != 0 || log->ouis[0][2] != 0))
				{
					entry2 = this->macList->GetEntryOUI(log->ouis[0]);
					if (entry2 && Text::StrIndexOfICase((const UTF8Char*)entry2->name, this->filterText) != INVALID_INDEX)
					{
						valid = true;
					}
				}
				if (!valid && (log->ouis[1][0] != 0 || log->ouis[1][1] != 0 || log->ouis[1][2] != 0))
				{
					entry2 = this->macList->GetEntryOUI(log->ouis[1]);
					if (entry2 && Text::StrIndexOfICase((const UTF8Char*)entry2->name, this->filterText) != INVALID_INDEX)
					{
						valid = true;
					}
				}
				if (!valid && (log->ouis[2][0] != 0 || log->ouis[2][1] != 0 || log->ouis[2][2] != 0))
				{
					entry2 = this->macList->GetEntryOUI(log->ouis[2]);
					if (entry2 && Text::StrIndexOfICase((const UTF8Char*)entry2->name, this->filterText) != INVALID_INDEX)
					{
						valid = true;
					}
				}
			}
		}

		if (valid)
		{
			Text::StrHexBytes(sbuff, log->mac, 6, ':');
			l = this->lvContent->AddItem(sbuff, log);
			if (entry)
			{
				this->lvContent->SetSubItem(l, 1, (const UTF8Char*)entry->name);
			}
			else
			{
				this->lvContent->SetSubItem(l, 1, (const UTF8Char*)"?");
			}
			this->lvContent->SetSubItem(l, 2, log->ssid);
			Text::StrInt32(sbuff, log->phyType);
			this->lvContent->SetSubItem(l, 3, sbuff);
			Text::StrDouble(sbuff, log->freq);
			this->lvContent->SetSubItem(l, 4, sbuff);
			if (log->manuf)
				this->lvContent->SetSubItem(l, 5, log->manuf);
			if (log->model)
				this->lvContent->SetSubItem(l, 6, log->model);
			if (log->serialNum)
				this->lvContent->SetSubItem(l, 7, log->serialNum);
			if (log->ouis[0][0] != 0 || log->ouis[0][1] != 0 || log->ouis[0][2] != 0)
				this->lvContent->SetSubItem(l, 8, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(log->ouis[0])->name);
			if (log->ouis[1][0] != 0 || log->ouis[1][1] != 0 || log->ouis[1][2] != 0)
				this->lvContent->SetSubItem(l, 9, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(log->ouis[1])->name);
			if (log->ouis[2][0] != 0 || log->ouis[2][1] != 0 || log->ouis[2][2] != 0)
				this->lvContent->SetSubItem(l, 10, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(log->ouis[2])->name);
			if (log->country)
				this->lvContent->SetSubItem(l, 11, log->country);
			cnt = 0;
			k = 0;
			while (k < 20)
			{
				if (log->neighbour[k])
					cnt++;
				k++;
			}
			Text::StrUOSInt(sbuff, cnt);
			this->lvContent->SetSubItem(l, 12, sbuff);

			recCnt++;
			if (recCnt >= MAX_ROW)
			{
				break;
			}
		}

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
	NEW_CLASS(this->wifiLogFile, Net::WiFiLogFile());
	NEW_CLASS(this->macList, Net::MACInfoList());
	this->filterText = 0;

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnFile, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Open Log"));
	this->btnFile->SetRect(4, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	NEW_CLASS(this->chkUnkOnly, UI::GUICheckBox(ui, this->pnlControl, (const UTF8Char*)"Unknown Only", true));
	this->chkUnkOnly->SetRect(84, 4, 100, 23, false);
	this->chkUnkOnly->HandleCheckedChange(OnUnkOnlyChkChg, this);
	NEW_CLASS(this->txtFilter, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)""));
	this->txtFilter->SetRect(184, 4, 150, 23, false);
	NEW_CLASS(this->btnFilter, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Filter"));
	this->btnFilter->SetRect(334, 4, 75, 23, false);
	this->btnFilter->HandleButtonClick(OnFilterClicked, this);
	NEW_CLASS(this->btnStore, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Store MACList"));
	this->btnStore->SetRect(414, 4, 75, 23, false);
	this->btnStore->HandleButtonClick(OnStoreClicked, this);
	NEW_CLASS(this->lblInfo, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)""));
	this->lblInfo->SetRect(494, 4, 200, 23, false);
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
	this->wifiLogFile->LoadFile(sbuff);
	this->LogUIUpdate();
}

SSWR::AVIRead::AVIRWiFiLogManagerForm::~AVIRWiFiLogManagerForm()
{
	DEL_CLASS(this->wifiLogFile);
	DEL_CLASS(this->macList);
	SDEL_TEXT(this->filterText);
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
