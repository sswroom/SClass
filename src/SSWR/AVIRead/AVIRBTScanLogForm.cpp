#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/WriteCacheStream.h"
#include "Net/MACInfo.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "SSWR/AVIRead/AVIRMACManagerEntryForm.h"
#include "SSWR/AVIRead/AVIRBTScanLogDevForm.h"
#include "SSWR/AVIRead/AVIRBTScanLogForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRBTScanLogForm::OnFileClicked(void *userObj)
{
/*	SSWR::AVIRead::AVIRBTScanLogForm *me = (SSWR::AVIRead::AVIRBTScanLogForm*)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"BTScanLogFile", false));
	dlg->SetAllowMultiSel(true);
	dlg->AddFilter((const UTF8Char*)"*.txt", (const UTF8Char*)"Log File");
	if (dlg->ShowDialog(me->GetHandle()))
	{
		UOSInt i = 0;
		UOSInt j = dlg->GetFileNameCount();
		while (i < j)
		{
			me->btLog->LoadFile(dlg->GetFileNames(i));
			i++;
		}
		me->LogFileStore();
		me->LogUIUpdate();
	}
	DEL_CLASS(dlg);*/
}

void __stdcall SSWR::AVIRead::AVIRBTScanLogForm::OnStoreClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBTScanLogForm *me = (SSWR::AVIRead::AVIRBTScanLogForm*)userObj;
	if (me->macList->Store())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Data Stored", (const UTF8Char*)"MAC Manager", me);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in storing data", (const UTF8Char*)"MAC Manager", me);
	}
}

void __stdcall SSWR::AVIRead::AVIRBTScanLogForm::OnContentDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRBTScanLogForm *me = (SSWR::AVIRead::AVIRBTScanLogForm*)userObj;
	const IO::BTScanLog::DevEntry *log = (const IO::BTScanLog::DevEntry*)me->lvContent->GetItem(index);
	if (log == 0)
		return;
	SSWR::AVIRead::AVIRBTScanLogDevForm *frm;
	NEW_CLASS(frm, SSWR::AVIRead::AVIRBTScanLogDevForm(0, me->ui, me->core, log));
	frm->ShowDialog(me);
	DEL_CLASS(frm);
	
/*	const Net::MACInfo::MACEntry *entry = me->macList->GetEntry(log->macInt);
	SSWR::AVIRead::AVIRMACManagerEntryForm *frm;
	UInt8 mac[8];
	WriteMUInt64(mac, log->macInt);
	if (entry)
	{
		NEW_CLASS(frm, SSWR::AVIRead::AVIRMACManagerEntryForm(0, me->ui, me->core, &mac[2], (const UTF8Char*)entry->name));
	}
	else
	{
		NEW_CLASS(frm, SSWR::AVIRead::AVIRMACManagerEntryForm(0, me->ui, me->core, &mac[2], 0));
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
			log = (const IO::BTScanLog::DevEntry*)me->lvContent->GetItem(i);
			if (log->macInt >= entry->rangeStart && log->macInt <= entry->rangeEnd)
			{
				me->lvContent->SetSubItem(i, 1, (const UTF8Char*)entry->name);
			}
			i++;
		}
	}
	DEL_CLASS(frm);*/
}

void __stdcall SSWR::AVIRead::AVIRBTScanLogForm::OnContentSelChg(void *userObj)
{
}

Bool SSWR::AVIRead::AVIRBTScanLogForm::LogFileStore()
{
	UTF8Char sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"BTDevLog.txt");
	return false; //this->btLog->StoreFile(sbuff);
}

void SSWR::AVIRead::AVIRBTScanLogForm::LogUIUpdate()
{
	const Net::MACInfo::MACEntry *entry;
	Text::StringBuilderUTF8 sb;
	IO::BTScanLog::DevEntry *log;
	Data::ArrayList<IO::BTScanLog::DevEntry*> logList;
	logList.AddAll(this->btLog->GetPublicList());
	logList.AddAll(this->btLog->GetRandomList());
	UTF8Char sbuff[64];
	const UTF8Char *csptr;
	UInt8 mac[8];
	UOSInt i;
	UOSInt j;
	UOSInt l;
	this->lvContent->ClearItems();
	i = 0;
	j = logList.GetCount();
	while (i < j)
	{
		log = logList.GetItem(i);
		WriteMUInt64(mac, log->macInt);
		Text::StrHexBytes(sbuff, &mac[2], 6, ':');
		l = this->lvContent->AddItem(sbuff, log);
		this->lvContent->SetSubItem(l, 1, IO::BTScanLog::RadioTypeGetName(log->radioType));
		this->lvContent->SetSubItem(l, 2, IO::BTScanLog::AddressTypeGetName(log->addrType));
		if (log->company == 0)
		{
			this->lvContent->SetSubItem(l, 3, (const UTF8Char*)"-");
		}
		else
		{
			csptr = Net::PacketAnalyzerBluetooth::CompanyGetName(log->company);
			if (csptr)
			{
				this->lvContent->SetSubItem(l, 3, csptr);
			}
			else
			{
				Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), log->company);
				this->lvContent->SetSubItem(l, 3, sbuff);
			}
		}
		if (log->addrType == IO::BTScanLog::AT_RANDOM)
		{
			switch ((log->macInt >> 40) & 0xc0)
			{
			case 0x00:
				this->lvContent->SetSubItem(i, 4, (const UTF8Char*)"Non-resolvable Random");
				break;
			case 0x40:
				this->lvContent->SetSubItem(i, 4, (const UTF8Char*)"Resolvable Random");
				break;
			case 0xC0:
				this->lvContent->SetSubItem(i, 4, (const UTF8Char*)"Static Random");
				break;
			default:
				this->lvContent->SetSubItem(i, 4, (const UTF8Char*)"-");
				break;
			}
		}
		else
		{
			entry = this->macList->GetEntry(log->macInt);
			if (entry)
			{
				this->lvContent->SetSubItem(l, 4, (const UTF8Char*)entry->name);
			}
			else
			{
				this->lvContent->SetSubItem(l, 4, (const UTF8Char*)"?");
			}
		}
		if (log->name)
			this->lvContent->SetSubItem(l, 5, log->name);
		Text::StrUOSInt(sbuff, log->logs->GetCount());
		this->lvContent->SetSubItem(l, 6, sbuff);
		Text::StrInt16(sbuff, log->measurePower);
		this->lvContent->SetSubItem(l, 7, sbuff);
		this->lvContent->SetSubItem(l, 8, IO::BTScanLog::AdvTypeGetName(log->lastAdvType));

		i++;
	}
}

void SSWR::AVIRead::AVIRBTScanLogForm::UpdateStatus()
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(this->macList->GetCount());
	sb.Append((const UTF8Char*)" Records");
	this->lblInfo->SetText(sb.ToString());
}

SSWR::AVIRead::AVIRBTScanLogForm::AVIRBTScanLogForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::BTScanLog *btLog) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Bluetooth Scan Log");

	this->core = core;
	this->btLog = btLog;
	NEW_CLASS(this->macList, Net::MACInfoList());

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnFile, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Open Log"));
	this->btnFile->SetRect(4, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	NEW_CLASS(this->btnStore, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Store MACList"));
	this->btnStore->SetRect(184, 4, 75, 23, false);
	this->btnStore->HandleButtonClick(OnStoreClicked, this);
	NEW_CLASS(this->lblInfo, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)""));
	this->lblInfo->SetRect(264, 4, 200, 23, false);
	NEW_CLASS(this->lvContent, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 9));
	this->lvContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvContent->SetShowGrid(true);
	this->lvContent->SetFullRowSelect(true);
	this->lvContent->HandleDblClk(OnContentDblClicked, this);
	this->lvContent->HandleSelChg(OnContentSelChg, this);
	this->lvContent->AddColumn((const UTF8Char*)"MAC", 120);
	this->lvContent->AddColumn((const UTF8Char*)"Type", 60);
	this->lvContent->AddColumn((const UTF8Char*)"AddrType", 80);
	this->lvContent->AddColumn((const UTF8Char*)"Company", 100);
	this->lvContent->AddColumn((const UTF8Char*)"Vendor", 160);
	this->lvContent->AddColumn((const UTF8Char*)"Name", 200);
	this->lvContent->AddColumn((const UTF8Char*)"Count", 60);
	this->lvContent->AddColumn((const UTF8Char*)"Measure Power", 60);
	this->lvContent->AddColumn((const UTF8Char*)"AdvType", 80);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->UpdateStatus();

	this->LogUIUpdate();
}

SSWR::AVIRead::AVIRBTScanLogForm::~AVIRBTScanLogForm()
{
	DEL_CLASS(this->btLog);
	DEL_CLASS(this->macList);
}

void SSWR::AVIRead::AVIRBTScanLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
