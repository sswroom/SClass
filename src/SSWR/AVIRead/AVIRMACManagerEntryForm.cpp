#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRMACManagerEntryForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRMACManagerEntryForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMACManagerEntryForm *me = (SSWR::AVIRead::AVIRMACManagerEntryForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->cboName->GetText(&sb);
	sb.Trim();
	UOSInt i = sb.GetCharCnt();
	while (i-- > 0)
	{
		if (sb.ToString()[i] >= 0x80)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Non-ASCII Char found", (const UTF8Char*)"MAC Entry", me);
			return;
		}
	}
	me->name = Text::String::NewNotNull(sb.ToString());
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRMACManagerEntryForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMACManagerEntryForm *me = (SSWR::AVIRead::AVIRMACManagerEntryForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

OSInt __stdcall SSWR::AVIRead::AVIRMACManagerEntryForm::MACCompare(void *obj1, void *obj2)
{
	Net::MACInfo::MACEntry *mac1 = (Net::MACInfo::MACEntry *)obj1;
	Net::MACInfo::MACEntry *mac2 = (Net::MACInfo::MACEntry *)obj2;
	return Text::StrCompareFastC(mac1->name, mac1->nameLen, mac2->name, mac2->nameLen);
}

SSWR::AVIRead::AVIRMACManagerEntryForm::AVIRMACManagerEntryForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, const UInt8 *mac, Text::CString name) : UI::GUIForm(parent, 480, 104, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MAC Entry"));
	this->SetNoResize(true);

	this->core = core;
	this->name = 0;

	NEW_CLASS(this->lblMAC, UI::GUILabel(ui, this, (const UTF8Char*)"MAC"));
	this->lblMAC->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtMAC, UI::GUITextBox(ui, this, CSTR("")));
	this->txtMAC->SetRect(104, 4, 200, 23, false);
	this->txtMAC->SetReadOnly(true);
	NEW_CLASS(this->lblName, UI::GUILabel(ui, this, (const UTF8Char*)"Name"));
	this->lblName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboName, UI::GUIComboBox(ui, this, true));
	this->cboName->SetRect(104, 28, 400, 23, false);
	this->cboName->SetAutoComplete(true);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("Cancel")));
	this->btnCancel->SetRect(104, 52, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("Ok")));
	this->btnOK->SetRect(204, 52, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	UOSInt i;
	UOSInt j;
	Net::MACInfo::MACEntry *macList = Net::MACInfo::GetMACEntryList(&i);
	Net::MACInfo::MACEntry **macListSort = MemAlloc(Net::MACInfo::MACEntry*, i);
	Net::MACInfo::MACEntry *lastMAC;
	j = 0;
	while (i-- > 0)
	{
		if (macList[i].nameLen > 0)
		{
			macListSort[j++] = &macList[i];;
		}
	}
	ArtificialQuickSort_SortCmp((void**)macListSort, MACCompare, 0, (OSInt)j - 1);
	this->cboName->BeginUpdate();
	lastMAC = 0;
	i = 0;
	while (i < j)
	{
		if (lastMAC == 0 || !Text::StrEqualsC(macListSort[i]->name, macListSort[i]->nameLen, lastMAC->name, lastMAC->nameLen))
		{
			lastMAC = macListSort[i];
			this->cboName->AddItem({lastMAC->name, lastMAC->nameLen}, 0);
		}
		i++;
	}
	this->cboName->EndUpdate();
	MemFree(macListSort);
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrHexBytes(sbuff, mac, 6, ':');
	this->txtMAC->SetText(CSTRP(sbuff, sptr));
	if (name.leng > 0)
	{
		this->cboName->SetText(name);
		this->cboName->Focus();
	}
	else
	{
		this->txtMAC->Focus();
		this->txtMAC->SelectAll();
	}

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::AVIRead::AVIRMACManagerEntryForm::~AVIRMACManagerEntryForm()
{
	SDEL_STRING(this->name);
}

void SSWR::AVIRead::AVIRMACManagerEntryForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Text::String *SSWR::AVIRead::AVIRMACManagerEntryForm::GetNameNew()
{
	return this->name->Clone();
}
