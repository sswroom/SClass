#include "Stdafx.h"
#include "Core/ByteTool_C.h"
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
#include "UI/Clipboard.h"
#include "UI/GUIFileDialog.h"

#define MAX_ROW 5000

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWiFiLogManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWiFiLogManagerForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"WiFiLoganagerFile", false);
	dlg->SetAllowMultiSel(true);
	dlg->AddFilter(CSTR("*.txt"), CSTR("Log File"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		UOSInt i = 0;
		UOSInt j = dlg->GetFileNameCount();
		while (i < j)
		{
			NN<Text::String> fileName;
			if (dlg->GetFileNames(i).SetTo(fileName))
			{
				me->wifiLogFile->LoadFile(fileName->ToCString());
			}
			i++;
		}
		me->LogFileStore();
		me->LogUIUpdate();
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnStoreClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWiFiLogManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWiFiLogManagerForm>();
	if (me->macList->Store())
	{
		me->ui->ShowMsgOK(CSTR("Data Stored"), CSTR("MAC Manager"), me);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in storing data"), CSTR("MAC Manager"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnContentDblClicked(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRWiFiLogManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWiFiLogManagerForm>();
	NN<const Net::WiFiLogFile::LogFileEntry> log;
	if (!me->lvContent->GetItem(index).GetOpt<const Net::WiFiLogFile::LogFileEntry>().SetTo(log))
		return;
	
	UOSInt dblType = me->cboDblClk->GetSelectedIndex();
	if (dblType == 1)
	{
		Text::StringBuilderUTF8 sb;
		if (UI::Clipboard::GetString(me->GetHandle(), sb))
		{
			UOSInt i = me->macList->SetEntry(log->mac64Int, sb.ToCString());
			me->UpdateStatus();
			me->EntryUpdated(me->macList->GetItemNoCheck(i));
		}
	}
	else
	{
		NN<const Net::MACInfo::MACEntry> entry;
		Text::CString name;
		if (me->macList->GetEntry(log->mac64Int).SetTo(entry))
		{
			name = {entry->name, entry->nameLen};
		}
		else
		{
			name = nullptr;
		}
		SSWR::AVIRead::AVIRMACManagerEntryForm frm(0, me->ui, me->core, log->mac, name);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			NN<Text::String> name = frm.GetNameNew();
			UOSInt i = me->macList->SetEntry(log->mac64Int, name->ToCString());
			name->Release();
			entry = me->macList->GetItemNoCheck(i);
			me->UpdateStatus();
			me->EntryUpdated(entry);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnContentSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWiFiLogManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWiFiLogManagerForm>();
	NN<const Net::WiFiLogFile::LogFileEntry> log;
	UnsafeArray<UInt8> ieBuff;
	if (!me->lvContent->GetSelectedItem().GetOpt<const Net::WiFiLogFile::LogFileEntry>().SetTo(log) || log->ieLen <= 0 || !log->ieBuff.SetTo(ieBuff))
	{
		me->txtFileIE->SetText(CSTR(""));
		return;
	}
	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	while (i < log->ieLen)
	{
		Net::WirelessLANIE::ToString(&ieBuff[i], sb);
		sb.AppendC(UTF8STRC("\r\n"));
		i += (UOSInt)ieBuff[i + 1] + 2;
	}
	me->txtFileIE->SetText(sb.ToCString());
}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnUnkOnlyChkChg(AnyType userObj, Bool checked)
{
	NN<SSWR::AVIRead::AVIRWiFiLogManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWiFiLogManagerForm>();
	me->LogUIUpdate();	
}

void __stdcall SSWR::AVIRead::AVIRWiFiLogManagerForm::OnFilterClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWiFiLogManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWiFiLogManagerForm>();
	Text::StringBuilderUTF8 sb;
	me->txtFilter->GetText(sb);
	OPTSTR_DEL(me->filterText);
	if (sb.GetLength() > 0)
	{
		me->filterText = Text::String::New(sb.ToCString());
	}
	me->LogUIUpdate();
}

Bool SSWR::AVIRead::AVIRWiFiLogManagerForm::LogFileStore()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("WiFiLog.txt"));
	return this->wifiLogFile->StoreFile(CSTRP(sbuff, sptr));
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::LogUIUpdate()
{
	Optional<const Net::MACInfo::MACEntry> entry;
	NN<const Net::MACInfo::MACEntry> entry2;
	NN<Net::WiFiLogFile::LogFileEntry> log;
	NN<Data::ArrayListNN<Net::WiFiLogFile::LogFileEntry>> logList = this->wifiLogFile->GetLogList();
	Bool unkOnly = this->chkUnkOnly->IsChecked();
	Bool local = this->chkLocal->IsChecked();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt cnt;
	UOSInt recCnt = 0;
	Bool valid;
	UnsafeArray<UInt8> ieBuff;
	NN<Text::String> s;
	NN<Text::String> filterText;
	this->lvContent->ClearItems();
	i = 0;
	j = logList->GetCount();
	while (i < j)
	{
		log = logList->GetItemNoCheck(i);
		entry = this->macList->GetEntry(log->mac64Int);
		valid = true;
		Net::MACInfo::AddressType addrType = Net::MACInfo::GetAddressType(log->mac);
		if (unkOnly && (entry.SetTo(entry2) && entry2->nameLen != 0))
		{
			valid = false;
		}
		else if ((unkOnly || !local) && (addrType == Net::MACInfo::AddressType::LocalMulticast || addrType == Net::MACInfo::AddressType::LocalUnicast))
		{
			valid = false;
		}
		else if (this->filterText.SetTo(filterText))
		{
			valid = false;
			if (log->ssid->IndexOfICase(filterText) != INVALID_INDEX)
			{
				valid = true;
			}
			else if (log->manuf.SetTo(s) && s->IndexOfICase(filterText) != INVALID_INDEX)
			{
				valid = true;
			}
			else if (entry.SetTo(entry2) && Text::StrIndexOfICase(entry2->name, filterText->v) != INVALID_INDEX)
			{
				valid = true;
			}
			else
			{
				if (!valid && (log->ouis[0][0] != 0 || log->ouis[0][1] != 0 || log->ouis[0][2] != 0))
				{
					if (this->macList->GetEntryOUI(log->ouis[0]).SetTo(entry2) && Text::StrIndexOfICase(entry2->name, filterText->v) != INVALID_INDEX)
					{
						valid = true;
					}
				}
				if (!valid && (log->ouis[1][0] != 0 || log->ouis[1][1] != 0 || log->ouis[1][2] != 0))
				{
					if (this->macList->GetEntryOUI(log->ouis[1]).SetTo(entry2) && Text::StrIndexOfICase(entry2->name, filterText->v) != INVALID_INDEX)
					{
						valid = true;
					}
				}
				if (!valid && (log->ouis[2][0] != 0 || log->ouis[2][1] != 0 || log->ouis[2][2] != 0))
				{
					if (this->macList->GetEntryOUI(log->ouis[2]).SetTo(entry2) && Text::StrIndexOfICase(entry2->name, filterText->v) != INVALID_INDEX)
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
			if (addrType == Net::MACInfo::AddressType::UniversalMulticast || addrType == Net::MACInfo::AddressType::UniversalUnicast)
			{
				if (entry.SetTo(entry2))
				{
					this->lvContent->SetSubItem(l, 1, {entry2->name, entry2->nameLen});
				}
				else
				{
					this->lvContent->SetSubItem(l, 1, CSTR("?"));
				}
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
			if (log->ieBuff.SetTo(ieBuff))
			{
				Optional<Text::String> manuf = 0;
				Optional<Text::String> model = 0;
				Optional<Text::String> serialNum = 0;
				Net::WirelessLANIE::GetWPSInfo(ieBuff, log->ieLen, manuf, model, serialNum);
				if (manuf.SetTo(s))
					this->lvContent->SetSubItem(l, 5, s->ToCString());
				else if (log->manuf.SetTo(s))
					this->lvContent->SetSubItem(l, 5, s->ToCString());
				if (model.SetTo(s))
					this->lvContent->SetSubItem(l, 6, s->ToCString());
				else if (log->model.SetTo(s))
					this->lvContent->SetSubItem(l, 6, s->ToCString());
				if (serialNum.SetTo(s))
					this->lvContent->SetSubItem(l, 7, s->ToCString());
				else if (log->serialNum.SetTo(s))
					this->lvContent->SetSubItem(l, 7, s->ToCString());
				OPTSTR_DEL(manuf);
				OPTSTR_DEL(model);
				OPTSTR_DEL(serialNum);
			}
			if (log->ouis[0][0] != 0 || log->ouis[0][1] != 0 || log->ouis[0][2] != 0)
			{
				NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfoOUI(log->ouis[0]);
				this->lvContent->SetSubItem(l, 8, {entry->name, entry->nameLen});
			}
			if (log->ouis[1][0] != 0 || log->ouis[1][1] != 0 || log->ouis[1][2] != 0)
			{
				NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfoOUI(log->ouis[1]);
				this->lvContent->SetSubItem(l, 9, {entry->name, entry->nameLen});
			}
			if (log->ouis[2][0] != 0 || log->ouis[2][1] != 0 || log->ouis[2][2] != 0)
			{
				NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfoOUI(log->ouis[2]);
				this->lvContent->SetSubItem(l, 10, {entry->name, entry->nameLen});
			}
			if (log->country.SetTo(s))
				this->lvContent->SetSubItem(l, 11, s->ToCString());
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

void SSWR::AVIRead::AVIRWiFiLogManagerForm::EntryUpdated(NN<const Net::MACInfo::MACEntry> entry)
{
	NN<const Net::WiFiLogFile::LogFileEntry> log;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->lvContent->GetCount();
	while (i < j)
	{
		log = this->lvContent->GetItem(i).GetNN<const Net::WiFiLogFile::LogFileEntry>();
		if (log->mac64Int >= entry->rangeStart && log->mac64Int <= entry->rangeEnd)
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

SSWR::AVIRead::AVIRWiFiLogManagerForm::AVIRWiFiLogManagerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("WiFi Log Manager"));

	this->core = core;
	NEW_CLASSNN(this->wifiLogFile, Net::WiFiLogFile());
	NEW_CLASSNN(this->macList, Net::MACInfoList());
	this->filterText = 0;

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnFile = ui->NewButton(this->pnlControl, CSTR("Open Log"));
	this->btnFile->SetRect(4, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	this->chkUnkOnly = ui->NewCheckBox(this->pnlControl, CSTR("Unknown Only"), true);
	this->chkUnkOnly->SetRect(84, 4, 100, 23, false);
	this->chkUnkOnly->HandleCheckedChange(OnUnkOnlyChkChg, this);
	this->chkLocal = ui->NewCheckBox(this->pnlControl, CSTR("Local"), false);
	this->chkLocal->SetRect(184, 4, 100, 23, false);
	this->chkLocal->HandleCheckedChange(OnUnkOnlyChkChg, this);
	this->txtFilter = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtFilter->SetRect(284, 4, 150, 23, false);
	this->btnFilter = ui->NewButton(this->pnlControl, CSTR("Filter"));
	this->btnFilter->SetRect(434, 4, 75, 23, false);
	this->btnFilter->HandleButtonClick(OnFilterClicked, this);
	this->btnStore = ui->NewButton(this->pnlControl, CSTR("Store MACList"));
	this->btnStore->SetRect(514, 4, 75, 23, false);
	this->btnStore->HandleButtonClick(OnStoreClicked, this);
	this->lblInfo = ui->NewLabel(this->pnlControl, CSTR(""));
	this->lblInfo->SetRect(594, 4, 200, 23, false);
	this->lblDblClk = ui->NewLabel(this->pnlControl, CSTR("Dbl-Clk Action"));
	this->lblDblClk->SetRect(794, 4, 100, 23, false);
	this->cboDblClk = ui->NewComboBox(this->pnlControl, false);
	this->cboDblClk->SetRect(894, 4, 100, 23, false);
	this->cboDblClk->AddItem(CSTR("Edit"), 0);
	this->cboDblClk->AddItem(CSTR("Paste"), 0);
	this->cboDblClk->SetSelectedIndex(0);
	this->txtFileIE = ui->NewTextBox(*this, CSTR(""), true);
	this->txtFileIE->SetRect(0, 0, 100, 255, false);
	this->txtFileIE->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtFileIE->SetReadOnly(true);
	this->vspFile = ui->NewVSplitter(*this, 3, true);
	this->lvContent = ui->NewListView(*this, UI::ListViewStyle::Table, 13);
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
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("WiFiLog.txt"));
	this->wifiLogFile->LoadFile(CSTRP(sbuff, sptr));
	this->LogUIUpdate();
}

SSWR::AVIRead::AVIRWiFiLogManagerForm::~AVIRWiFiLogManagerForm()
{
	this->wifiLogFile.Delete();
	this->macList.Delete();
	OPTSTR_DEL(this->filterText);
}

void SSWR::AVIRead::AVIRWiFiLogManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
