#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
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
#include "Win32/Clipboard.h"

#define MAX_ROW 5000

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm *me = (SSWR::AVIRead::AVIRWiFiLogManagerForm*)userObj;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"WiFiLoganagerFile", false);
	dlg.SetAllowMultiSel(true);
	dlg.AddFilter(CSTR("*.txt"), CSTR("Log File"));
	if (dlg.ShowDialog(me->GetHandle()))
	{
		UOSInt i = 0;
		UOSInt j = dlg.GetFileNameCount();
		while (i < j)
		{
			const UTF8Char *fileName = dlg.GetFileNames(i);
			me->wifiLogFile->LoadFile({fileName, Text::StrCharCnt(fileName)});
			i++;
		}
		me->LogFileStore();
		me->LogUIUpdate();
	}
}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnStoreClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm *me = (SSWR::AVIRead::AVIRWiFiLogManagerForm*)userObj;
	if (me->macList->Store())
	{
		UI::MessageDialog::ShowDialog(CSTR("Data Stored"), CSTR("MAC Manager"), me);
	}
	else
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in storing data"), CSTR("MAC Manager"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnContentDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm *me = (SSWR::AVIRead::AVIRWiFiLogManagerForm*)userObj;
	const Net::WiFiLogFile::LogFileEntry *log = (const Net::WiFiLogFile::LogFileEntry*)me->lvContent->GetItem(index);
	if (log == 0)
		return;
	
	UOSInt dblType = me->cboDblClk->GetSelectedIndex();
	if (dblType == 1)
	{
		Text::StringBuilderUTF8 sb;
		if (Win32::Clipboard::GetString(me->GetHandle(), &sb))
		{
			UOSInt i = me->macList->SetEntry(log->macInt, sb.ToCString());
			me->UpdateStatus();
			me->EntryUpdated(me->macList->GetItem(i));
		}
	}
	else
	{
		const Net::MACInfo::MACEntry *entry = me->macList->GetEntry(log->macInt);
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
			Text::String *name = frm.GetNameNew();
			UOSInt i = me->macList->SetEntry(log->macInt, name->ToCString());
			name->Release();
			entry = me->macList->GetItem(i);
			me->UpdateStatus();
			me->EntryUpdated(entry);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnContentSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRWiFiLogManagerForm *me = (SSWR::AVIRead::AVIRWiFiLogManagerForm*)userObj;
	const Net::WiFiLogFile::LogFileEntry *log = (const Net::WiFiLogFile::LogFileEntry*)me->lvContent->GetSelectedItem();
	if (log == 0 || log->ieLen <= 0)
	{
		me->txtFileIE->SetText(CSTR(""));
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
	me->txtFileIE->SetText(sb.ToCString());
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
	SDEL_STRING(me->filterText);
	if (sb.GetLength() > 0)
	{
		me->filterText = Text::String::New(sb.ToString(), sb.GetLength());
	}
	me->LogUIUpdate();
}

Bool SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileStore()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("WiFiLog.txt"));
	return this->wifiLogFile->StoreFile(CSTRP(sbuff, sptr));
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::LogUIUpdate()
{
	const Net::MACInfo::MACEntry *entry;
	const Net::MACInfo::MACEntry *entry2;
	Net::WiFiLogFile::LogFileEntry *log;
	Data::ArrayList<Net::WiFiLogFile::LogFileEntry*> *logList = this->wifiLogFile->GetLogList();
	Bool unkOnly = this->chkUnkOnly->IsChecked();
	UTF8Char sbuff[64];
	UTF8Char *sptr;
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
		if (unkOnly && (entry != 0 && entry->nameLen != 0))
		{
			valid = false;
		}
		else if (this->filterText)
		{
			valid = false;
			if (log->ssid && log->ssid->IndexOfICase(this->filterText) != INVALID_INDEX)
			{
				valid = true;
			}
			else if (log->manuf && log->manuf->IndexOfICase(this->filterText) != INVALID_INDEX)
			{
				valid = true;
			}
			else if (entry && Text::StrIndexOfICase(entry->name, this->filterText->v) != INVALID_INDEX)
			{
				valid = true;
			}
			else
			{
				if (!valid && (log->ouis[0][0] != 0 || log->ouis[0][1] != 0 || log->ouis[0][2] != 0))
				{
					entry2 = this->macList->GetEntryOUI(log->ouis[0]);
					if (entry2 && Text::StrIndexOfICase(entry2->name, this->filterText->v) != INVALID_INDEX)
					{
						valid = true;
					}
				}
				if (!valid && (log->ouis[1][0] != 0 || log->ouis[1][1] != 0 || log->ouis[1][2] != 0))
				{
					entry2 = this->macList->GetEntryOUI(log->ouis[1]);
					if (entry2 && Text::StrIndexOfICase(entry2->name, this->filterText->v) != INVALID_INDEX)
					{
						valid = true;
					}
				}
				if (!valid && (log->ouis[2][0] != 0 || log->ouis[2][1] != 0 || log->ouis[2][2] != 0))
				{
					entry2 = this->macList->GetEntryOUI(log->ouis[2]);
					if (entry2 && Text::StrIndexOfICase(entry2->name, this->filterText->v) != INVALID_INDEX)
					{
						valid = true;
					}
				}
			}
		}

		if (valid)
		{
			sptr = Text::StrHexBytes(sbuff, log->mac, 6, ':');
			l = this->lvContent->AddItem(CSTRP(sbuff, sptr), log);
			if (entry)
			{
				this->lvContent->SetSubItem(l, 1, {entry->name, entry->nameLen});
			}
			else
			{
				this->lvContent->SetSubItem(l, 1, CSTR("?"));
			}
			this->lvContent->SetSubItem(l, 2, log->ssid);
			sptr = Text::StrInt32(sbuff, log->phyType);
			this->lvContent->SetSubItem(l, 3, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, log->freq);
			this->lvContent->SetSubItem(l, 4, CSTRP(sbuff, sptr));
			Text::String *manuf = 0;
			Text::String *model = 0;
			Text::String *serialNum = 0;
			Net::WirelessLANIE::GetWPSInfo(log->ieBuff, log->ieLen, &manuf, &model, &serialNum);
			if (manuf)
				this->lvContent->SetSubItem(l, 5, manuf);
			else if (log->manuf)
				this->lvContent->SetSubItem(l, 5, log->manuf);
			if (model)
				this->lvContent->SetSubItem(l, 6, model);
			else if (log->model)
				this->lvContent->SetSubItem(l, 6, log->model);
			if (serialNum)
				this->lvContent->SetSubItem(l, 7, serialNum);
			else if (log->serialNum)
				this->lvContent->SetSubItem(l, 7, log->serialNum);
			SDEL_STRING(manuf);
			SDEL_STRING(model);
			SDEL_STRING(serialNum);
			if (log->ouis[0][0] != 0 || log->ouis[0][1] != 0 || log->ouis[0][2] != 0)
			{
				const Net::MACInfo::MACEntry *entry = Net::MACInfo::GetMACInfoOUI(log->ouis[0]);
				this->lvContent->SetSubItem(l, 8, {entry->name, entry->nameLen});
			}
			if (log->ouis[1][0] != 0 || log->ouis[1][1] != 0 || log->ouis[1][2] != 0)
			{
				const Net::MACInfo::MACEntry *entry = Net::MACInfo::GetMACInfoOUI(log->ouis[1]);
				this->lvContent->SetSubItem(l, 9, {entry->name, entry->nameLen});
			}
			if (log->ouis[2][0] != 0 || log->ouis[2][1] != 0 || log->ouis[2][2] != 0)
			{
				const Net::MACInfo::MACEntry *entry = Net::MACInfo::GetMACInfoOUI(log->ouis[2]);
				this->lvContent->SetSubItem(l, 10, {entry->name, entry->nameLen});
			}
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
			sptr = Text::StrUOSInt(sbuff, cnt);
			this->lvContent->SetSubItem(l, 12, CSTRP(sbuff, sptr));

			recCnt++;
			if (recCnt >= MAX_ROW)
			{
				break;
			}
		}

		i++;
	}
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::EntryUpdated(const Net::MACInfo::MACEntry *entry)
{
	const Net::WiFiLogFile::LogFileEntry *log;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->lvContent->GetCount();
	while (i < j)
	{
		log = (const Net::WiFiLogFile::LogFileEntry*)this->lvContent->GetItem(i);
		if (log->macInt >= entry->rangeStart && log->macInt <= entry->rangeEnd)
		{
			this->lvContent->SetSubItem(i, 1, {entry->name, entry->nameLen});
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::UpdateStatus()
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(this->macList->GetCount());
	sb.AppendC(UTF8STRC(" Records"));
	this->lblInfo->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRWiFiLogManagerForm::AVIRWiFiLogManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("WiFi Log Manager"));

	this->core = core;
	NEW_CLASS(this->wifiLogFile, Net::WiFiLogFile());
	NEW_CLASS(this->macList, Net::MACInfoList());
	this->filterText = 0;

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnFile, UI::GUIButton(ui, this->pnlControl, CSTR("Open Log")));
	this->btnFile->SetRect(4, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	NEW_CLASS(this->chkUnkOnly, UI::GUICheckBox(ui, this->pnlControl, CSTR("Unknown Only"), true));
	this->chkUnkOnly->SetRect(84, 4, 100, 23, false);
	this->chkUnkOnly->HandleCheckedChange(OnUnkOnlyChkChg, this);
	NEW_CLASS(this->txtFilter, UI::GUITextBox(ui, this->pnlControl, CSTR("")));
	this->txtFilter->SetRect(184, 4, 150, 23, false);
	NEW_CLASS(this->btnFilter, UI::GUIButton(ui, this->pnlControl, CSTR("Filter")));
	this->btnFilter->SetRect(334, 4, 75, 23, false);
	this->btnFilter->HandleButtonClick(OnFilterClicked, this);
	NEW_CLASS(this->btnStore, UI::GUIButton(ui, this->pnlControl, CSTR("Store MACList")));
	this->btnStore->SetRect(414, 4, 75, 23, false);
	this->btnStore->HandleButtonClick(OnStoreClicked, this);
	NEW_CLASS(this->lblInfo, UI::GUILabel(ui, this->pnlControl, CSTR("")));
	this->lblInfo->SetRect(494, 4, 200, 23, false);
	NEW_CLASS(this->lblDblClk, UI::GUILabel(ui, this->pnlControl, CSTR("Dbl-Clk Action")));
	this->lblDblClk->SetRect(694, 4, 100, 23, false);
	NEW_CLASS(this->cboDblClk, UI::GUIComboBox(ui, this->pnlControl, false));
	this->cboDblClk->SetRect(794, 4, 100, 23, false);
	this->cboDblClk->AddItem(CSTR("Edit"), 0);
	this->cboDblClk->AddItem(CSTR("Paste"), 0);
	this->cboDblClk->SetSelectedIndex(0);
	NEW_CLASS(this->txtFileIE, UI::GUITextBox(ui, this, CSTR(""), true));
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
	this->lvContent->AddColumn(CSTR("Neigbour Cnt"), 50);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->UpdateStatus();

	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("WiFiLog.txt"));
	this->wifiLogFile->LoadFile(CSTRP(sbuff, sptr));
	this->LogUIUpdate();
}

SSWR::AVIRead::AVIRWiFiLogManagerForm::~AVIRWiFiLogManagerForm()
{
	DEL_CLASS(this->wifiLogFile);
	DEL_CLASS(this->macList);
	SDEL_STRING(this->filterText);
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
