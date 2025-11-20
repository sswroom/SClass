#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/MACInfo.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "Net/WirelessLANIE.h"
#include "SSWR/AVIRead/AVIRMACManagerEntryForm.h"
#include "SSWR/AVIRead/AVIRBluetoothLogForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRBluetoothLogForm::OnFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothLogForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"BluetoothLogFile", false);
	dlg->SetAllowMultiSel(true);
	dlg->AddFilter(CSTR("*.txt"), CSTR("Log File"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		UOSInt i = 0;
		UOSInt j = dlg->GetFileNameCount();
		while (i < j)
		{
			NN<Text::String> name;
			if (dlg->GetFileNames(i).SetTo(name))
			{
				me->btLog.LoadFile(name->ToCString());
			}
			i++;
		}
		me->LogFileStore();
		me->LogUIUpdate();
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLogForm::OnStoreClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothLogForm>();
	if (me->macList.Store())
	{
		me->ui->ShowMsgOK(CSTR("Data Stored"), CSTR("MAC Manager"), me);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in storing data"), CSTR("MAC Manager"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLogForm::OnContentDblClicked(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRBluetoothLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothLogForm>();
	NN<const IO::BTDevLog::DevEntry> log;
	if (!me->lvContent->GetItem(index).GetOpt<const IO::BTDevLog::DevEntry>().SetTo(log))
		return;
	NN<const Net::MACInfo::MACEntry> entry;
	Text::CString name = me->macList.GetEntry(log->mac64Int).SetTo(entry)?Text::CString(entry->name, entry->nameLen):nullptr;
	SSWR::AVIRead::AVIRMACManagerEntryForm frm(0, me->ui, me->core, log->mac, name);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		NN<Text::String> name = frm.GetNameNew();
		UOSInt i = me->macList.SetEntry(log->mac64Int, name->ToCString());
		name->Release();
		entry = me->macList.GetItemNoCheck(i);
		me->UpdateStatus();

		UOSInt j;
		i = 0;
		j = me->lvContent->GetCount();
		while (i < j)
		{
			log = me->lvContent->GetItem(i).GetNN<const IO::BTDevLog::DevEntry>();
			if (log->mac64Int >= entry->rangeStart && log->mac64Int <= entry->rangeEnd)
			{
				me->lvContent->SetSubItem(i, 1, {entry->name, entry->nameLen});
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLogForm::OnContentSelChg(AnyType userObj)
{
}

void __stdcall SSWR::AVIRead::AVIRBluetoothLogForm::OnUnkOnlyChkChg(AnyType userObj, Bool checked)
{
	NN<SSWR::AVIRead::AVIRBluetoothLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothLogForm>();
	me->LogUIUpdate();	
}

Bool SSWR::AVIRead::AVIRBluetoothLogForm::LogFileStore()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("BTDevLog.txt"));
	return this->btLog.StoreFile(CSTRP(sbuff, sptr));
}

void SSWR::AVIRead::AVIRBluetoothLogForm::LogUIUpdate()
{
	Optional<const Net::MACInfo::MACEntry> entry;
	NN<const Net::MACInfo::MACEntry> nnentry;
	NN<IO::BTDevLog::DevEntry> log;
	Data::ArrayListNN<IO::BTDevLog::DevEntry> logList;
	logList.AddAll(this->btLog.GetPublicList());
	logList.AddAll(this->btLog.GetRandomList());
	Bool unkOnly = this->chkUnkOnly->IsChecked();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	UOSInt i;
	UOSInt j;
	UOSInt l;
	this->lvContent->ClearItems();
	i = 0;
	j = logList.GetCount();
	while (i < j)
	{
		log = logList.GetItemNoCheck(i);
		entry = this->macList.GetEntry(log->mac64Int);
		if (unkOnly && (entry.SetTo(nnentry) && nnentry->nameLen != 0) && log->addrType != IO::BTScanLog::AT_RANDOM)
		{

		}
		else
		{
			sptr = Text::StrHexBytes(sbuff, log->mac, 6, ':');
			l = this->lvContent->AddItem(CSTRP(sbuff, sptr), log);
			this->lvContent->SetSubItem(l, 1, IO::BTScanLog::RadioTypeGetName(log->radioType));
			this->lvContent->SetSubItem(l, 2, IO::BTScanLog::AddressTypeGetName(log->addrType));
			if (log->addrType == IO::BTScanLog::AT_RANDOM)
			{
				switch (log->mac[0] & 0xc0)
				{
				case 0x00:
					this->lvContent->SetSubItem(i, 3, CSTR("Non-resolvable Random"));
					break;
				case 0x40:
					this->lvContent->SetSubItem(i, 3, CSTR("Resolvable Random"));
					break;
				case 0xC0:
					this->lvContent->SetSubItem(i, 3, CSTR("Static Random"));
					break;
				default:
					this->lvContent->SetSubItem(i, 3, CSTR("-"));
					break;
				}
			}
			else
			{
				if (entry.SetTo(nnentry))
				{
					this->lvContent->SetSubItem(l, 3, {nnentry->name, nnentry->nameLen});
				}
				else
				{
					this->lvContent->SetSubItem(l, 3, CSTR("?"));
				}
			}
			if (log->name.SetTo(s))
				this->lvContent->SetSubItem(l, 4, s);
			sptr = Text::StrInt32(sbuff, log->txPower);
			this->lvContent->SetSubItem(l, 5, CSTRP(sbuff, sptr));
			if (log->company == 0)
			{
				this->lvContent->SetSubItem(l, 6, CSTR("-"));
			}
			else
			{
				Text::CString cstr = Net::PacketAnalyzerBluetooth::CompanyGetName(log->company);
				if (cstr.v == 0)
				{
					this->lvContent->SetSubItem(l, 6, CSTR("?"));
				}
				else
				{
					this->lvContent->SetSubItem(l, 6, cstr.OrEmpty());
				}
			}
			sptr = Text::StrInt32(sbuff, log->measurePower);
			this->lvContent->SetSubItem(l, 7, CSTRP(sbuff, sptr));
		}

		i++;
	}
}

void SSWR::AVIRead::AVIRBluetoothLogForm::UpdateStatus()
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(this->macList.GetCount());
	sb.AppendC(UTF8STRC(" Records"));
	this->lblInfo->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRBluetoothLogForm::AVIRBluetoothLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Bluetooth Log"));

	this->core = core;

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnFile = ui->NewButton(this->pnlControl, CSTR("Open Log"));
	this->btnFile->SetRect(4, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	this->chkUnkOnly = ui->NewCheckBox(this->pnlControl, CSTR("Unknown Only"), true);
	this->chkUnkOnly->SetRect(84, 4, 100, 23, false);
	this->chkUnkOnly->HandleCheckedChange(OnUnkOnlyChkChg, this);
	this->btnStore = ui->NewButton(this->pnlControl, CSTR("Store MACList"));
	this->btnStore->SetRect(184, 4, 75, 23, false);
	this->btnStore->HandleButtonClick(OnStoreClicked, this);
	this->lblInfo = ui->NewLabel(this->pnlControl, CSTR(""));
	this->lblInfo->SetRect(264, 4, 200, 23, false);
	this->lvContent = ui->NewListView(*this, UI::ListViewStyle::Table, 8);
	this->lvContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvContent->SetShowGrid(true);
	this->lvContent->SetFullRowSelect(true);
	this->lvContent->HandleDblClk(OnContentDblClicked, this);
	this->lvContent->HandleSelChg(OnContentSelChg, this);
	this->lvContent->AddColumn(CSTR("MAC"), 120);
	this->lvContent->AddColumn(CSTR("Type"), 60);
	this->lvContent->AddColumn(CSTR("AddrType"), 80);
	this->lvContent->AddColumn(CSTR("Vendor"), 120);
	this->lvContent->AddColumn(CSTR("Name"), 200);
	this->lvContent->AddColumn(CSTR("TX Power"), 60);
	this->lvContent->AddColumn(CSTR("Company"), 150);
	this->lvContent->AddColumn(CSTR("Measure Power"), 60);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->UpdateStatus();

	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("BTDevLog.txt"));
	this->btLog.LoadFile(CSTRP(sbuff, sptr));
	this->LogUIUpdate();
}

SSWR::AVIRead::AVIRBluetoothLogForm::~AVIRBluetoothLogForm()
{
}

void SSWR::AVIRead::AVIRBluetoothLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
