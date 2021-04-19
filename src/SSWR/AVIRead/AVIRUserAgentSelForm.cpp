#include "Stdafx.h"
#include "IO/AndroidDB.h"
#include "Manage/CPUDB.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRUserAgentSelForm.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRUserAgentSelForm::OnOkClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentSelForm *me = (SSWR::AVIRead::AVIRUserAgentSelForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRUserAgentSelForm::OnFilterChg(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentSelForm *me = (SSWR::AVIRead::AVIRUserAgentSelForm*)userObj;
	SSWR::AVIRead::AVIRUserAgentSelForm::OSItem *osItem = (SSWR::AVIRead::AVIRUserAgentSelForm::OSItem*)me->cboFilterOS->GetSelectedItem();
	me->UpdateUAList(osItem->os, osItem->osVer, (Net::BrowserInfo::BrowserType)(OSInt)me->cboFilterBrowser->GetSelectedItem());
}

void __stdcall SSWR::AVIRead::AVIRUserAgentSelForm::OnUserAgentSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentSelForm *me = (SSWR::AVIRead::AVIRUserAgentSelForm*)userObj;
	Net::UserAgentDB::UAEntry *uaList = (Net::UserAgentDB::UAEntry*)me->lvUserAgent->GetSelectedItem();
	me->currUserAgent = (const UTF8Char*)uaList->userAgent;
}

void __stdcall SSWR::AVIRead::AVIRUserAgentSelForm::OnUserAgentDblClk(void *userObj, OSInt itemIndex)
{
	SSWR::AVIRead::AVIRUserAgentSelForm *me = (SSWR::AVIRead::AVIRUserAgentSelForm*)userObj;
	Net::UserAgentDB::UAEntry *uaList = (Net::UserAgentDB::UAEntry*)me->lvUserAgent->GetItem(itemIndex);
	me->currUserAgent = (const UTF8Char*)uaList->userAgent;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void SSWR::AVIRead::AVIRUserAgentSelForm::UpdateUAList(Manage::OSInfo::OSType os, const Char *osVer, Net::BrowserInfo::BrowserType browser)
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
			if (osVer == 0 || (uaList[i].osVer != 0 && Text::StrEquals(uaList[i].osVer, osVer)))
			{
				if (browser == Net::BrowserInfo::BT_UNKNOWN || browser == uaList[i].browser)
				{
					sb.ClearStr();
					Manage::OSInfo::GetCommonName(&sb, uaList[i].os, (const UTF8Char*)uaList[i].osVer);
					k = this->lvUserAgent->AddItem(sb.ToString(), &uaList[i]);
					this->lvUserAgent->SetSubItem(k, 1, Net::BrowserInfo::GetName(uaList[i].browser));
					if (uaList[i].browserVer)
					{
						this->lvUserAgent->SetSubItem(k, 2, (const UTF8Char*)uaList[i].browserVer);
					}
					if (uaList[i].devName)
					{
						const IO::AndroidDB::AndroidInfo *android = IO::AndroidDB::GetAndroidInfo((const UTF8Char*)uaList[i].devName);
						if (android)
						{
							sb.ClearStr();
							sb.Append((const UTF8Char*)android->brandName);
							sb.AppendChar(' ', 1);
							sb.Append((const UTF8Char*)android->modelName);
							this->lvUserAgent->SetSubItem(k, 3, sb.ToString());
							this->lvUserAgent->SetSubItem(k, 4, (const UTF8Char*)android->cpuName);
							const Manage::CPUDB::CPUSpec *cpu = Manage::CPUDB::GetCPUSpec((const UTF8Char*)android->cpuName);
							if (cpu)
							{
								sb.ClearStr();
								sb.AppendI32(cpu->nm);
								this->lvUserAgent->SetSubItem(k, 5, sb.ToString());
							}
						}
						else
						{
							this->lvUserAgent->SetSubItem(k, 3, (const UTF8Char*)uaList[i].devName);
						}						
					}
					this->lvUserAgent->SetSubItem(k, 6, (const UTF8Char*)uaList[i].userAgent);
					if (Text::StrEquals(this->currUserAgent, (const UTF8Char*)uaList[i].userAgent))
					{
						this->lvUserAgent->SetSelectedIndex(k);
					}
				}
			}
		}
		i++;
	}
}

SSWR::AVIRead::AVIRUserAgentSelForm::AVIRUserAgentSelForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, const UTF8Char *currUserAgent) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"User Agent Select");

	this->core = core;
	this->currUserAgent = currUserAgent;
	NEW_CLASS(this->osList, Data::ArrayList<SSWR::AVIRead::AVIRUserAgentSelForm::OSItem*>());

	NEW_CLASS(this->pnlFilter, UI::GUIPanel(ui, this));
	this->pnlFilter->SetRect(0, 0, 100, 31, false);
	this->pnlFilter->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFilterOS, UI::GUILabel(ui, this->pnlFilter, (const UTF8Char*)"OS"));
	this->lblFilterOS->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboFilterOS, UI::GUIComboBox(ui, this->pnlFilter, false));
	this->cboFilterOS->SetRect(104, 4, 150, 23, false);
	this->cboFilterOS->HandleSelectionChange(OnFilterChg, this);
	NEW_CLASS(this->lblFilterBrowser, UI::GUILabel(ui, this->pnlFilter, (const UTF8Char*)"Browser"));
	this->lblFilterBrowser->SetRect(304, 4, 100, 23, false);
	NEW_CLASS(this->cboFilterBrowser, UI::GUIComboBox(ui, this->pnlFilter, false));
	this->cboFilterBrowser->SetRect(404, 4, 150, 23, false);
	this->cboFilterBrowser->HandleSelectionChange(OnFilterChg, this);
	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnOk, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Ok"));
	this->btnOk->SetRect(4, 4, 75, 23, false);
	this->btnOk->HandleButtonClick(OnOkClicked, this);
	NEW_CLASS(this->lvUserAgent, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 7));
	this->lvUserAgent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvUserAgent->HandleSelChg(OnUserAgentSelChg, this);
	this->lvUserAgent->SetFullRowSelect(true);
	this->lvUserAgent->SetShowGrid(true);
	this->lvUserAgent->AddColumn((const UTF8Char*)"OS", 150);
	this->lvUserAgent->AddColumn((const UTF8Char*)"Browser", 150);
	this->lvUserAgent->AddColumn((const UTF8Char*)"Browser Ver", 150);
	this->lvUserAgent->AddColumn((const UTF8Char*)"Device Name", 150);
	this->lvUserAgent->AddColumn((const UTF8Char*)"CPU", 100);
	this->lvUserAgent->AddColumn((const UTF8Char*)"nm", 40);
	this->lvUserAgent->AddColumn((const UTF8Char*)"User Agent", 400);
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
	this->osList->Add(osItem);
	sb.ClearStr();
	Manage::OSInfo::GetCommonName(&sb, osItem->os, (const UTF8Char*)osItem->osVer);
	i = this->cboFilterOS->AddItem(sb.ToString(), osItem);
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
			this->osList->Add(osItem);
			sb.ClearStr();
			Manage::OSInfo::GetCommonName(&sb, osItem->os, (const UTF8Char*)osItem->osVer);
			this->cboFilterOS->AddItem(sb.ToString(), osItem);
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
				this->osList->Add(osItem);
				sb.ClearStr();
				Manage::OSInfo::GetCommonName(&sb, osItem->os, (const UTF8Char*)osItem->osVer);
				this->cboFilterOS->AddItem(sb.ToString(), osItem);
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
	this->UpdateUAList(Manage::OSInfo::OT_UNKNOWN, 0, Net::BrowserInfo::BT_UNKNOWN);
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

const UTF8Char *SSWR::AVIRead::AVIRUserAgentSelForm::GetUserAgent()
{
	return this->currUserAgent;
}
