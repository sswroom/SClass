#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRMACManagerEntryForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRMACManagerEntryForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMACManagerEntryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMACManagerEntryForm>();
	Text::StringBuilderUTF8 sb;
	me->cboName->GetText(sb);
	sb.Trim();
	UOSInt i = sb.GetCharCnt();
	while (i-- > 0)
	{
		if (sb.ToString()[i] >= 0x80)
		{
			me->ui->ShowMsgOK(CSTR("Non-ASCII Char found"), CSTR("MAC Entry"), me);
			return;
		}
	}
	me->name->Release();
	me->name = Text::String::New(sb.ToCString());
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRMACManagerEntryForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMACManagerEntryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMACManagerEntryForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

OSInt __stdcall SSWR::AVIRead::AVIRMACManagerEntryForm::MACCompare(Net::MACInfo::MACEntry *obj1, Net::MACInfo::MACEntry *obj2)
{
	return Text::StrCompareFastC(obj1->name, obj1->nameLen, obj2->name, obj2->nameLen);
}

SSWR::AVIRead::AVIRMACManagerEntryForm::AVIRMACManagerEntryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, const UInt8 *mac, Text::CString name) : UI::GUIForm(parent, 480, 104, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MAC Entry"));
	this->SetNoResize(true);

	this->core = core;
	this->name = Text::String::NewEmpty();

	this->lblMAC = ui->NewLabel(*this, CSTR("MAC"));
	this->lblMAC->SetRect(4, 4, 100, 23, false);
	this->txtMAC = ui->NewTextBox(*this, CSTR(""));
	this->txtMAC->SetRect(104, 4, 200, 23, false);
	this->txtMAC->SetReadOnly(true);
	this->lblName = ui->NewLabel(*this, CSTR("Name"));
	this->lblName->SetRect(4, 28, 100, 23, false);
	this->cboName = ui->NewComboBox(*this, true);
	this->cboName->SetRect(104, 28, 400, 23, false);
	this->cboName->SetAutoComplete(true);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 52, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("Ok"));
	this->btnOK->SetRect(204, 52, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	UOSInt i;
	UOSInt j;
	UnsafeArray<Net::MACInfo::MACEntry> macList = Net::MACInfo::GetMACEntryList(i);
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
	Data::Sort::ArtificialQuickSortFunc<Net::MACInfo::MACEntry*>::Sort(macListSort, MACCompare, 0, (OSInt)j - 1);
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
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrHexBytes(sbuff, mac, 6, ':');
	this->txtMAC->SetText(CSTRP(sbuff, sptr));
	if (name.leng > 0)
	{
		this->cboName->SetText(name.OrEmpty());
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
	this->name->Release();
}

void SSWR::AVIRead::AVIRMACManagerEntryForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

NN<Text::String> SSWR::AVIRead::AVIRMACManagerEntryForm::GetNameNew() const
{
	return this->name->Clone();
}
