#include "Stdafx.h"
#include "IO/AndroidDB.h"
#include "Manage/CPUDB.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRUserAgentSelForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRUserAgentSelForm::OnOkClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUserAgentSelForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUserAgentSelForm>();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRUserAgentSelForm::OnFilterChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUserAgentSelForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUserAgentSelForm>();
	NN<SSWR::AVIRead::AVIRUserAgentSelForm::OSItem> osItem;
	if (me->cboFilterOS->GetSelectedItem().GetOpt<SSWR::AVIRead::AVIRUserAgentSelForm::OSItem>().SetTo(osItem))
	{
		me->UpdateUAList(osItem->os, {osItem->osVer, osItem->osVerLen}, (Net::BrowserInfo::BrowserType)me->cboFilterBrowser->GetSelectedItem().GetOSInt());
	}
}

void __stdcall SSWR::AVIRead::AVIRUserAgentSelForm::OnUserAgentSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUserAgentSelForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUserAgentSelForm>();
	NN<Net::UserAgentDB::UAEntry> uaList;
	if (me->lvUserAgent->GetSelectedItem().GetOpt<Net::UserAgentDB::UAEntry>().SetTo(uaList))
	{
		me->currUserAgent = uaList->userAgent;
		me->currUserAgentLen = uaList->userAgentLen;
	}
}

void __stdcall SSWR::AVIRead::AVIRUserAgentSelForm::OnUserAgentDblClk(AnyType userObj, UOSInt itemIndex)
{
	NN<SSWR::AVIRead::AVIRUserAgentSelForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUserAgentSelForm>();
	NN<Net::UserAgentDB::UAEntry> uaList;
	if (me->lvUserAgent->GetItem(itemIndex).GetOpt<Net::UserAgentDB::UAEntry>().SetTo(uaList))
	{
		me->currUserAgent = uaList->userAgent;
		me->currUserAgentLen = uaList->userAgentLen;
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void SSWR::AVIRead::AVIRUserAgentSelForm::UpdateUAList(Manage::OSInfo::OSType os, Text::CString osVer, Net::BrowserInfo::BrowserType browser)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Text::StringBuilderUTF8 sb;
	Text::CStringNN nnosVer;
	UnsafeArray<const UTF8Char> nns;
	this->lvUserAgent->ClearItems();
	i = 0;
	Net::UserAgentDB::UAEntry *uaList = Net::UserAgentDB::GetUAEntryList(&j);
	while (i < j)
	{
		if (os == Manage::OSInfo::OT_UNKNOWN || os == uaList[i].os)
		{
			if (!osVer.SetTo(nnosVer) || nnosVer.leng == 0 || (uaList[i].osVerLen != 0 && nnosVer.Equals(uaList[i].osVer.Or(U8STR("")), uaList[i].osVerLen)))
			{
				if (browser == Net::BrowserInfo::BT_UNKNOWN || browser == uaList[i].browser)
				{
					sb.ClearStr();
					Manage::OSInfo::GetCommonName(sb, uaList[i].os, {uaList[i].osVer, uaList[i].osVerLen});
					k = this->lvUserAgent->AddItem(sb.ToCString(), &uaList[i]);
					this->lvUserAgent->SetSubItem(k, 1, Net::BrowserInfo::GetName(uaList[i].browser));
					if (uaList[i].browserVer.SetTo(nns))
					{
						this->lvUserAgent->SetSubItem(k, 2, {nns, uaList[i].browserVerLen});
					}
					if (uaList[i].devName.SetTo(nns))
					{
						const IO::AndroidDB::AndroidInfo *android = IO::AndroidDB::GetAndroidInfo({nns, uaList[i].devNameLen});
						if (android)
						{
							sb.ClearStr();
							sb.AppendSlow((const UTF8Char*)android->brandName);
							sb.AppendUTF8Char(' ');
							sb.AppendSlow((const UTF8Char*)android->modelName);
							this->lvUserAgent->SetSubItem(k, 3, sb.ToCString());
							this->lvUserAgent->SetSubItem(k, 4, {android->cpuName, android->cpuNameLen});
							NN<const Manage::CPUDB::CPUSpec> cpu;
							if (Manage::CPUDB::GetCPUSpec({android->cpuName, android->cpuNameLen}).SetTo(cpu))
							{
								sb.ClearStr();
								sb.AppendI32(cpu->nm);
								this->lvUserAgent->SetSubItem(k, 5, sb.ToCString());
							}
						}
						else
						{
							this->lvUserAgent->SetSubItem(k, 3, {nns, uaList[i].devNameLen});
						}						
					}
					this->lvUserAgent->SetSubItem(k, 6, {uaList[i].userAgent, uaList[i].userAgentLen});
					if (Text::StrEqualsC(this->currUserAgent, this->currUserAgentLen, uaList[i].userAgent, uaList[i].userAgentLen))
					{
						this->lvUserAgent->SetSelectedIndex(k);
					}
				}
			}
		}
		i++;
	}
}

SSWR::AVIRead::AVIRUserAgentSelForm::AVIRUserAgentSelForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Text::CStringNN currUserAgent) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("User Agent Select"));

	this->core = core;
	this->currUserAgent = currUserAgent.v;
	this->currUserAgentLen = currUserAgent.leng;

	this->pnlFilter = ui->NewPanel(*this);
	this->pnlFilter->SetRect(0, 0, 100, 31, false);
	this->pnlFilter->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFilterOS = ui->NewLabel(this->pnlFilter, CSTR("OS"));
	this->lblFilterOS->SetRect(4, 4, 100, 23, false);
	this->cboFilterOS = ui->NewComboBox(this->pnlFilter, false);
	this->cboFilterOS->SetRect(104, 4, 150, 23, false);
	this->cboFilterOS->HandleSelectionChange(OnFilterChg, this);
	this->lblFilterBrowser = ui->NewLabel(this->pnlFilter, CSTR("Browser"));
	this->lblFilterBrowser->SetRect(304, 4, 100, 23, false);
	this->cboFilterBrowser = ui->NewComboBox(this->pnlFilter, false);
	this->cboFilterBrowser->SetRect(404, 4, 150, 23, false);
	this->cboFilterBrowser->HandleSelectionChange(OnFilterChg, this);
	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnOk = ui->NewButton(this->pnlControl, CSTR("Ok"));
	this->btnOk->SetRect(4, 4, 75, 23, false);
	this->btnOk->HandleButtonClick(OnOkClicked, this);
	this->lvUserAgent = ui->NewListView(*this, UI::ListViewStyle::Table, 7);
	this->lvUserAgent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvUserAgent->HandleSelChg(OnUserAgentSelChg, this);
	this->lvUserAgent->SetFullRowSelect(true);
	this->lvUserAgent->SetShowGrid(true);
	this->lvUserAgent->AddColumn(CSTR("OS"), 150);
	this->lvUserAgent->AddColumn(CSTR("Browser"), 150);
	this->lvUserAgent->AddColumn(CSTR("Browser Ver"), 150);
	this->lvUserAgent->AddColumn(CSTR("Device Name"), 150);
	this->lvUserAgent->AddColumn(CSTR("CPU"), 100);
	this->lvUserAgent->AddColumn(CSTR("nm"), 40);
	this->lvUserAgent->AddColumn(CSTR("User Agent"), 400);
	this->lvUserAgent->HandleDblClk(OnUserAgentDblClk, this);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	UOSInt i;
	UOSInt j;
	UOSInt k;
	Bool found;
	NN<SSWR::AVIRead::AVIRUserAgentSelForm::OSItem> osItem;
	Text::StringBuilderUTF8 sb;
	UnsafeArray<const UTF8Char> nns;
	UnsafeArray<const UTF8Char> nns2;
	Net::UserAgentDB::UAEntry *uaList = Net::UserAgentDB::GetUAEntryList(&j);
	osItem = MemAllocNN(SSWR::AVIRead::AVIRUserAgentSelForm::OSItem);
	osItem->os = Manage::OSInfo::OT_UNKNOWN;
	osItem->osVer = 0;
	osItem->osVerLen = 0 ;
	this->osList.Add(osItem);
	sb.ClearStr();
	Manage::OSInfo::GetCommonName(sb, osItem->os, {osItem->osVer, osItem->osVerLen});
	i = this->cboFilterOS->AddItem(sb.ToCString(), osItem);
	this->cboFilterOS->SetSelectedIndex(i);
	i = 0;
	while (i < j)
	{
		found = false;
		k = this->osList.GetCount();
		while (k-- > 0)
		{
			osItem = this->osList.GetItemNoCheck(k);
			if (osItem->os == uaList[i].os && osItem->osVer == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			osItem = MemAllocNN(SSWR::AVIRead::AVIRUserAgentSelForm::OSItem);
			osItem->os = uaList[i].os;
			osItem->osVer = 0;
			osItem->osVerLen = 0;
			this->osList.Add(osItem);
			sb.ClearStr();
			Manage::OSInfo::GetCommonName(sb, osItem->os, {osItem->osVer, osItem->osVerLen});
			this->cboFilterOS->AddItem(sb.ToCString(), osItem);
		}

		if (uaList[i].osVer.SetTo(nns))
		{
			found = false;
			k = this->osList.GetCount();
			while (k-- > 0)
			{
				osItem = this->osList.GetItemNoCheck(k);
				if (osItem->os == uaList[i].os && osItem->osVer.SetTo(nns2) && Text::StrEquals(nns2, nns))
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				osItem = MemAllocNN(SSWR::AVIRead::AVIRUserAgentSelForm::OSItem);
				osItem->os = uaList[i].os;
				osItem->osVer = nns;
				osItem->osVerLen = uaList[i].osVerLen;
				this->osList.Add(osItem);
				sb.ClearStr();
				Manage::OSInfo::GetCommonName(sb, osItem->os, {osItem->osVer, osItem->osVerLen});
				this->cboFilterOS->AddItem(sb.ToCString(), osItem);
			}
		}
		i++;
	}
	i = Net::BrowserInfo::BT_FIRST;
	j = Net::BrowserInfo::BT_LAST;
	while (i <= j)
	{
		this->cboFilterBrowser->AddItem(Net::BrowserInfo::GetName((Net::BrowserInfo::BrowserType)i), (void*)i);
		i++;
	}
	this->cboFilterBrowser->SetSelectedIndex(0);
	this->UpdateUAList(Manage::OSInfo::OT_UNKNOWN, CSTR_NULL, Net::BrowserInfo::BT_UNKNOWN);
}

SSWR::AVIRead::AVIRUserAgentSelForm::~AVIRUserAgentSelForm()
{
	this->osList.MemFreeAll();
}

void SSWR::AVIRead::AVIRUserAgentSelForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Text::CStringNN SSWR::AVIRead::AVIRUserAgentSelForm::GetUserAgent()
{
	return {this->currUserAgent, this->currUserAgentLen};
}
