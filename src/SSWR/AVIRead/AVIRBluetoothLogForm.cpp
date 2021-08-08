#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/WriteCacheStream.h"
#include "Net/MACInfo.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "Net/WirelessLANIE.h"
#include "SSWR/AVIRead/AVIRMACManagerEntryForm.h"
#include "SSWR/AVIRead/AVIRBluetoothLogForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRBluetoothLogForm::OnFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothLogForm *me = (SSWR::AVIRead::AVIRBluetoothLogForm*)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"BluetoothLogFile", false));
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
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLogForm::OnStoreClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothLogForm *me = (SSWR::AVIRead::AVIRBluetoothLogForm*)userObj;
	if (me->macList->Store())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Data Stored", (const UTF8Char*)"MAC Manager", me);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in storing data", (const UTF8Char*)"MAC Manager", me);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLogForm::OnContentDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRBluetoothLogForm *me = (SSWR::AVIRead::AVIRBluetoothLogForm*)userObj;
	const IO::BTDevLog::DevEntry2 *log = (const IO::BTDevLog::DevEntry2*)me->lvContent->GetItem(index);
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
			log = (const IO::BTDevLog::DevEntry2*)me->lvContent->GetItem(i);
			if (log->macInt >= entry->rangeStart && log->macInt <= entry->rangeEnd)
			{
				me->lvContent->SetSubItem(i, 1, (const UTF8Char*)entry->name);
			}
			i++;
		}
	}
	DEL_CLASS(frm);
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLogForm::OnContentSelChg(void *userObj)
{
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLogForm::OnUnkOnlyChkChg(void *userObj, Bool checked)
{
	SSWR::AVIRead::AVIRBluetoothLogForm *me = (SSWR::AVIRead::AVIRBluetoothLogForm*)userObj;
	me->LogUIUpdate();	
}

Bool SSWR::AVIRead::AVIRBluetoothLogForm::LogFileStore()
{
	UTF8Char sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"BTDevLog.txt");
	return this->btLog->StoreFile(sbuff);
}

void SSWR::AVIRead::AVIRBluetoothLogForm::LogUIUpdate()
{
	const Net::MACInfo::MACEntry *entry;
	IO::BTDevLog::DevEntry2 *log;
	Data::ArrayList<IO::BTDevLog::DevEntry2*> *logList = this->btLog->GetLogList();
	Bool unkOnly = this->chkUnkOnly->IsChecked();
	UTF8Char sbuff[64];
	UOSInt i;
	UOSInt j;
	UOSInt l;
	this->lvContent->ClearItems();
	i = 0;
	j = logList->GetCount();
	while (i < j)
	{
		log = logList->GetItem(i);
		entry = this->macList->GetEntry(log->macInt);
		if (unkOnly && (entry != 0 && entry->name != 0 && entry->name[0] != 0) && log->addrType != IO::BTScanLog::AT_RANDOM)
		{

		}
		else
		{
			Text::StrHexBytes(sbuff, log->mac, 6, ':');
			l = this->lvContent->AddItem(sbuff, log);
			this->lvContent->SetSubItem(l, 1, IO::BTScanLog::RadioTypeGetName(log->radioType));
			this->lvContent->SetSubItem(l, 2, IO::BTScanLog::AddressTypeGetName(log->addrType));
			if (log->addrType == IO::BTScanLog::AT_RANDOM)
			{
				switch (log->mac[0] & 0xc0)
				{
				case 0x00:
					this->lvContent->SetSubItem(i, 3, (const UTF8Char*)"Non-resolvable Random");
					break;
				case 0x40:
					this->lvContent->SetSubItem(i, 3, (const UTF8Char*)"Resolvable Random");
					break;
				case 0xC0:
					this->lvContent->SetSubItem(i, 3, (const UTF8Char*)"Static Random");
					break;
				default:
					this->lvContent->SetSubItem(i, 3, (const UTF8Char*)"-");
					break;
				}
			}
			else
			{
				if (entry)
				{
					this->lvContent->SetSubItem(l, 3, (const UTF8Char*)entry->name);
				}
				else
				{
					this->lvContent->SetSubItem(l, 3, (const UTF8Char*)"?");
				}
			}
			if (log->name)
				this->lvContent->SetSubItem(l, 4, log->name);
			Text::StrInt32(sbuff, log->txPower);
			this->lvContent->SetSubItem(l, 5, sbuff);
			if (log->company == 0)
			{
				this->lvContent->SetSubItem(l, 6, (const UTF8Char*)"-");
			}
			else
			{
				const UTF8Char *csptr = Net::PacketAnalyzerBluetooth::CompanyGetName(log->company);
				if (csptr == 0)
				{
					this->lvContent->SetSubItem(l, 6, (const UTF8Char*)"?");
				}
				else
				{
					this->lvContent->SetSubItem(l, 6, csptr);
				}
			}
		}

		i++;
	}
}

void SSWR::AVIRead::AVIRBluetoothLogForm::UpdateStatus()
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(this->macList->GetCount());
	sb.Append((const UTF8Char*)" Records");
	this->lblInfo->SetText(sb.ToString());
}

SSWR::AVIRead::AVIRBluetoothLogForm::AVIRBluetoothLogForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Bluetooth Log");

	this->core = core;
	NEW_CLASS(this->btLog, IO::BTDevLog());
	NEW_CLASS(this->macList, Net::MACInfoList());

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnFile, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Open Log"));
	this->btnFile->SetRect(4, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	NEW_CLASS(this->chkUnkOnly, UI::GUICheckBox(ui, this->pnlControl, (const UTF8Char*)"Unknown Only", true));
	this->chkUnkOnly->SetRect(84, 4, 100, 23, false);
	this->chkUnkOnly->HandleCheckedChange(OnUnkOnlyChkChg, this);
	NEW_CLASS(this->btnStore, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Store MACList"));
	this->btnStore->SetRect(184, 4, 75, 23, false);
	this->btnStore->HandleButtonClick(OnStoreClicked, this);
	NEW_CLASS(this->lblInfo, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)""));
	this->lblInfo->SetRect(264, 4, 200, 23, false);
	NEW_CLASS(this->lvContent, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 7));
	this->lvContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvContent->SetShowGrid(true);
	this->lvContent->SetFullRowSelect(true);
	this->lvContent->HandleDblClk(OnContentDblClicked, this);
	this->lvContent->HandleSelChg(OnContentSelChg, this);
	this->lvContent->AddColumn((const UTF8Char*)"MAC", 120);
	this->lvContent->AddColumn((const UTF8Char*)"Type", 60);
	this->lvContent->AddColumn((const UTF8Char*)"AddrType", 80);
	this->lvContent->AddColumn((const UTF8Char*)"Vendor", 120);
	this->lvContent->AddColumn((const UTF8Char*)"Name", 200);
	this->lvContent->AddColumn((const UTF8Char*)"TX Power", 60);
	this->lvContent->AddColumn((const UTF8Char*)"Company", 150);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->UpdateStatus();

	UTF8Char sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"BTDevLog.txt");
	this->btLog->LoadFile(sbuff);
	this->LogUIUpdate();
}

SSWR::AVIRead::AVIRBluetoothLogForm::~AVIRBluetoothLogForm()
{
	DEL_CLASS(this->btLog);
	DEL_CLASS(this->macList);
}

void SSWR::AVIRead::AVIRBluetoothLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
