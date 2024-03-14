#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/MACInfo.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "SSWR/AVIRead/AVIRMACManagerEntryForm.h"
#include "SSWR/AVIRead/AVIRBTScanLogDevForm.h"
#include "SSWR/AVIRead/AVIRBTScanLogForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

void __stdcall SSWR::AVIRead::AVIRBTScanLogForm::OnFileClicked(AnyType userObj)
{
/*	SSWR::AVIRead::AVIRBTScanLogForm *me = (SSWR::AVIRead::AVIRBTScanLogForm*)userObj;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"BTScanLogFile", false);
	dlg.SetAllowMultiSel(true);
	dlg.AddFilter(CSTR("*.txt"), CSTR("Log File"));
	if (dlg.ShowDialog(me->GetHandle()))
	{
		UOSInt i = 0;
		UOSInt j = dlg.GetFileNameCount();
		while (i < j)
		{
			me->btLog->LoadFile(dlg.GetFileNames(i));
			i++;
		}
		me->LogFileStore();
		me->LogUIUpdate();
	}*/
}

void __stdcall SSWR::AVIRead::AVIRBTScanLogForm::OnStoreClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRBTScanLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBTScanLogForm>();
	if (me->macList->Store())
	{
		me->ui->ShowMsgOK(CSTR("Data Stored"), CSTR("MAC Manager"), me);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in storing data"), CSTR("MAC Manager"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRBTScanLogForm::OnContentDblClicked(AnyType userObj, UOSInt index)
{
	NotNullPtr<SSWR::AVIRead::AVIRBTScanLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBTScanLogForm>();
	const IO::BTScanLog::DevEntry *log = (const IO::BTScanLog::DevEntry*)me->lvContent->GetItem(index);
	if (log == 0)
		return;
	{
		SSWR::AVIRead::AVIRBTScanLogDevForm frm(0, me->ui, me->core, log);
		frm.ShowDialog(me);
	}
	
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

void __stdcall SSWR::AVIRead::AVIRBTScanLogForm::OnContentSelChg(AnyType userObj)
{
}

Bool SSWR::AVIRead::AVIRBTScanLogForm::LogFileStore()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, sptr, CSTR("BTDevLog.txt"));
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
	UTF8Char *sptr;
	Text::CString cstr;
	UInt8 mac[8];
	NotNullPtr<Text::String> s;
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
		sptr = Text::StrHexBytes(sbuff, &mac[2], 6, ':');
		l = this->lvContent->AddItem(CSTRP(sbuff, sptr), log);
		this->lvContent->SetSubItem(l, 1, IO::BTScanLog::RadioTypeGetName(log->radioType));
		this->lvContent->SetSubItem(l, 2, IO::BTScanLog::AddressTypeGetName(log->addrType));
		if (log->company == 0)
		{
			this->lvContent->SetSubItem(l, 3, CSTR("-"));
		}
		else
		{
			cstr = Net::PacketAnalyzerBluetooth::CompanyGetName(log->company);
			if (cstr.v)
			{
				this->lvContent->SetSubItem(l, 3, cstr.OrEmpty());
			}
			else
			{
				sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), log->company);
				this->lvContent->SetSubItem(l, 3, CSTRP(sbuff, sptr));
			}
		}
		if (log->addrType == IO::BTScanLog::AT_RANDOM)
		{
			switch ((log->macInt >> 40) & 0xc0)
			{
			case 0x00:
				this->lvContent->SetSubItem(i, 4, CSTR("Non-resolvable Random"));
				break;
			case 0x40:
				this->lvContent->SetSubItem(i, 4, CSTR("Resolvable Random"));
				break;
			case 0xC0:
				this->lvContent->SetSubItem(i, 4, CSTR("Static Random"));
				break;
			default:
				this->lvContent->SetSubItem(i, 4, CSTR("-"));
				break;
			}
		}
		else
		{
			entry = this->macList->GetEntry(log->macInt);
			if (entry)
			{
				this->lvContent->SetSubItem(l, 4, {entry->name, entry->nameLen});
			}
			else
			{
				this->lvContent->SetSubItem(l, 4, CSTR("?"));
			}
		}
		if (s.Set(log->name))
			this->lvContent->SetSubItem(l, 5, s);
		sptr = Text::StrUOSInt(sbuff, log->logs->GetCount());
		this->lvContent->SetSubItem(l, 6, CSTRP(sbuff, sptr));
		sptr = Text::StrInt16(sbuff, log->measurePower);
		this->lvContent->SetSubItem(l, 7, CSTRP(sbuff, sptr));
		this->lvContent->SetSubItem(l, 8, IO::BTScanLog::AdvTypeGetName(log->lastAdvType));

		i++;
	}
}

void SSWR::AVIRead::AVIRBTScanLogForm::UpdateStatus()
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(this->macList->GetCount());
	sb.AppendC(UTF8STRC(" Records"));
	this->lblInfo->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRBTScanLogForm::AVIRBTScanLogForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::BTScanLog *btLog) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Bluetooth Scan Log"));

	this->core = core;
	this->btLog = btLog;
	NEW_CLASS(this->macList, Net::MACInfoList());

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnFile = ui->NewButton(this->pnlControl, CSTR("Open Log"));
	this->btnFile->SetRect(4, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	this->btnStore = ui->NewButton(this->pnlControl, CSTR("Store MACList"));
	this->btnStore->SetRect(184, 4, 75, 23, false);
	this->btnStore->HandleButtonClick(OnStoreClicked, this);
	this->lblInfo = ui->NewLabel(this->pnlControl, CSTR(""));
	this->lblInfo->SetRect(264, 4, 200, 23, false);
	this->lvContent = ui->NewListView(*this, UI::ListViewStyle::Table, 9);
	this->lvContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvContent->SetShowGrid(true);
	this->lvContent->SetFullRowSelect(true);
	this->lvContent->HandleDblClk(OnContentDblClicked, this);
	this->lvContent->HandleSelChg(OnContentSelChg, this);
	this->lvContent->AddColumn(CSTR("MAC"), 120);
	this->lvContent->AddColumn(CSTR("Type"), 60);
	this->lvContent->AddColumn(CSTR("AddrType"), 80);
	this->lvContent->AddColumn(CSTR("Company"), 100);
	this->lvContent->AddColumn(CSTR("Vendor"), 160);
	this->lvContent->AddColumn(CSTR("Name"), 200);
	this->lvContent->AddColumn(CSTR("Count"), 60);
	this->lvContent->AddColumn(CSTR("Measure Power"), 60);
	this->lvContent->AddColumn(CSTR("AdvType"), 80);

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
