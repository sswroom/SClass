#include "Stdafx.h"
#include "IO/AndroidDB.h"
#include "Manage/CPUDB.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRUserAgentSelForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRUserAgentSelForm::OnOkClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentSelForm *me = (SSWR::AVIRead::AVIRUserAgentSelForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRUserAgentSelForm::OnFilterChg(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentSelForm *me = (SSWR::AVIRead::AVIRUserAgentSelForm*)userObj;
	SSWR::AVIRead::AVIRUserAgentSelForm::OSItem *osItem = (SSWR::AVIRead::AVIRUserAgentSelForm::OSItem*)me->cboFilterOS->GetSelectedItem();
	me->UpdateUAList(osItem->os, {osItem->osVer, osItem->osVerLen}, (Net::BrowserInfo::BrowserType)(OSInt)me->cboFilterBrowser->GetSelectedItem());
}

void __stdcall SSWR::AVIRead::AVIRUserAgentSelForm::OnUserAgentSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentSelForm *me = (SSWR::AVIRead::AVIRUserAgentSelForm*)userObj;
	Net::UserAgentDB::UAEntry *uaList = (Net::UserAgentDB::UAEntry*)me->lvUserAgent->GetSelectedItem();
	me->currUserAgent = uaList->userAgent;
	me->currUserAgentLen = uaList->userAgentLen;
}

void __stdcall SSWR::AVIRead::AVIRUserAgentSelForm::OnUserAgentDblClk(void *userObj, UOSInt itemIndex)
{
	SSWR::AVIRead::AVIRUserAgentSelForm *me = (SSWR::AVIRead::AVIRUserAgentSelForm*)userObj;
	Net::UserAgentDB::UAEntry *uaList = (Net::UserAgentDB::UAEntry*)me->lvUserAgent->GetItem(itemIndex);
	me->currUserAgent = uaList->userAgent;
	me->currUserAgentLen = uaList->userAgentLen;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void SSWR::AVIRead::AVIRUserAgentSelForm::UpdateUAList(Manage::OSInfo::OSType os, Text::CString osVer, Net::BrowserInfo::BrowserType browser)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Text::StringBuilderUTF8 sb;	
	this->lvUserAgent->ClearItems();
	i = 0;
	Net::UserAgentDB::UAEntry *uaList = Net::UserAgentDB::GetUAEntryList(&j);
	while (i < j)
	{
		if (os == Manage::OSInfo::OT_UNKNOWN || os == uaList[i].os)
		{
			if (osVer.leng == 0 || (uaList[i].osVerLen != 0 && osVer.Equals(uaList[i].osVer, uaList[i].osVerLen)))
			{
				if (browser == Net::BrowserInfo::BT_UNKNOWN || browser == uaList[i].browser)
				{
					sb.ClearStr();
					Manage::OSInfo::GetCommonName(sb, uaList[i].os, {uaList[i].osVer, uaList[i].osVerLen});
					k = this->lvUserAgent->AddItem(sb.ToCString(), &uaList[i]);
					this->lvUserAgent->SetSubItem(k, 1, Net::BrowserInfo::GetName(uaList[i].browser));
					if (uaList[i].browserVer)
					{
						this->lvUserAgent->SetSubItem(k, 2, {uaList[i].browserVer, uaList[i].browserVerLen});
					}
					if (uaList[i].devName)
					{
						const IO::AndroidDB::AndroidInfo *android = IO::AndroidDB::GetAndroidInfo({uaList[i].devName, uaList[i].devNameLen});
						if (android)
						{
							sb.ClearStr();
							sb.AppendSlow((const UTF8Char*)android->brandName);
							sb.AppendUTF8Char(' ');
							sb.AppendSlow((const UTF8Char*)android->modelName);
							this->lvUserAgent->SetSubItem(k, 3, sb.ToCString());
							this->lvUserAgent->SetSubItem(k, 4, {android->cpuName, android->cpuNameLen});
							const Manage::CPUDB::CPUSpec *cpu = Manage::CPUDB::GetCPUSpec({android->cpuName, android->cpuNameLen});
							if (cpu)
							{
								sb.ClearStr();
								sb.AppendI32(cpu->nm);
								this->lvUserAgent->SetSubItem(k, 5, sb.ToCString());
							}
						}
						else
						{
							this->lvUserAgent->SetSubItem(k, 3, {uaList[i].devName, uaList[i].devNameLen});
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

SSWR::AVIRead::AVIRUserAgentSelForm::AVIRUserAgentSelForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Text::CString currUserAgent) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("User Agent Select"));

	this->core = core;
	this->currUserAgent = currUserAgent.v;
	this->currUserAgentLen = currUserAgent.leng;
	NEW_CLASS(this->osList, Data::ArrayList<SSWR::AVIRead::AVIRUserAgentSelForm::OSItem*>());

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
	SSWR::AVIRead::AVIRUserAgentSelForm::OSItem *osItem;
	Text::StringBuilderUTF8 sb;
	Net::UserAgentDB::UAEntry *uaList = Net::UserAgentDB::GetUAEntryList(&j);
	osItem = MemAlloc(SSWR::AVIRead::AVIRUserAgentSelForm::OSItem, 1);
	osItem->os = Manage::OSInfo::OT_UNKNOWN;
	osItem->osVer = 0;
	osItem->osVerLen = 0 ;
	this->osList->Add(osItem);
	sb.ClearStr();
	Manage::OSInfo::GetCommonName(sb, osItem->os, {osItem->osVer, osItem->osVerLen});
	i = this->cboFilterOS->AddItem(sb.ToCString(), osItem);
	this->cboFilterOS->SetSelectedIndex(i);
	i = 0;
	while (i < j)
	{
		found = false;
		k = this->osList->GetCount();
		while (k-- > 0)
		{
			osItem = this->osList->GetItem(k);
			if (osItem->os == uaList[i].os && osItem->osVer == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			osItem = MemAlloc(SSWR::AVIRead::AVIRUserAgentSelForm::OSItem, 1);
			osItem->os = uaList[i].os;
			osItem->osVer = 0;
			osItem->osVerLen = 0;
			this->osList->Add(osItem);
			sb.ClearStr();
			Manage::OSInfo::GetCommonName(sb, osItem->os, {osItem->osVer, osItem->osVerLen});
			this->cboFilterOS->AddItem(sb.ToCString(), osItem);
		}

		if (uaList[i].osVer)
		{
			found = false;
			k = this->osList->GetCount();
			while (k-- > 0)
			{
				osItem = this->osList->GetItem(k);
				if (osItem->os == uaList[i].os && osItem->osVer != 0 && Text::StrEquals(osItem->osVer, uaList[i].osVer))
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				osItem = MemAlloc(SSWR::AVIRead::AVIRUserAgentSelForm::OSItem, 1);
				osItem->os = uaList[i].os;
				osItem->osVer = uaList[i].osVer;
				osItem->osVerLen = uaList[i].osVerLen;
				this->osList->Add(osItem);
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
	UOSInt i;
	i = this->osList->GetCount();
	while (i-- > 0)
	{
		MemFree(this->osList->GetItem(i));
	}
	DEL_CLASS(this->osList);
}

void SSWR::AVIRead::AVIRUserAgentSelForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Text::CString SSWR::AVIRead::AVIRUserAgentSelForm::GetUserAgent()
{
	return {this->currUserAgent, this->currUserAgentLen};
}
