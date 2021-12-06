#include "Stdafx.h"
#include "Crypto/HOTP.h"
#include "Crypto/TOTP.h"
#include "Data/RandomBytesGenerator.h"
#include "SSWR/AVIRead/AVIROTPForm.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Base32Enc.h"
#include "UI/MessageDialog.h"

void SSWR::AVIRead::AVIROTPForm::RandBytes(UOSInt len)
{
	Data::RandomBytesGenerator random;
	UInt8 buff[32];
	Text::StringBuilderUTF8 sb;
	Text::TextBinEnc::Base32Enc b32;
	random.NextBytes(buff, len);
	b32.EncodeBin(&sb, buff, len);
	this->txtKey->SetText(sb.ToString());
}

void __stdcall SSWR::AVIRead::AVIROTPForm::OnKeyRand80Clicked(void *userObj)
{
	SSWR::AVIRead::AVIROTPForm *me = (SSWR::AVIRead::AVIROTPForm*)userObj;
	me->RandBytes(10);
}

void __stdcall SSWR::AVIRead::AVIROTPForm::OnKeyRand160Clicked(void *userObj)
{
	SSWR::AVIRead::AVIROTPForm *me = (SSWR::AVIRead::AVIROTPForm*)userObj;
	me->RandBytes(20);
}

void __stdcall SSWR::AVIRead::AVIROTPForm::OnNewClicked(void *userObj)
{
	SSWR::AVIRead::AVIROTPForm *me = (SSWR::AVIRead::AVIROTPForm*)userObj;
	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbKey;
	me->txtName->GetText(&sbName);
	me->txtKey->GetText(&sbKey);
	if (sbName.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter name", (const UTF8Char*)"One-Time Password (OTP)", me);
		return;
	}
	if (sbKey.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter key", (const UTF8Char*)"One-Time Password (OTP)", me);
		return;
	}
	if (sbKey.GetLength() > 32)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Key is too long", (const UTF8Char*)"One-Time Password (OTP)", me);
		return;
	}
	if (!Text::TextBinEnc::Base32Enc::IsValid(sbKey.ToString()))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Key is not valid", (const UTF8Char*)"One-Time Password (OTP)", me);
		return;
	}

	UOSInt type = me->cboType->GetSelectedIndex();
	UInt8 buff[32];
	UOSInt keySize;
	Text::TextBinEnc::Base32Enc b32;
	keySize = b32.DecodeBin(sbKey.ToString(), buff);
	EntryInfo *entry;
	entry = MemAlloc(EntryInfo, 1);
	entry->name = Text::StrCopyNew(sbName.ToString());
	if (type == 0)
	{
		NEW_CLASS(entry->otp, Crypto::HOTP(buff, keySize, 1));
		entry->lastCounter = entry->otp->GetCounter();
	}
	else
	{
		NEW_CLASS(entry->otp, Crypto::TOTP(buff, keySize));
		entry->lastCounter = 0;
	}
	me->entryList->Add(entry);
	UOSInt i = me->lvEntry->AddItem(entry->name, entry);
	me->lvEntry->SetSubItem(i, 1, (const UTF8Char*)"-");
}

void __stdcall SSWR::AVIRead::AVIROTPForm::OnEntryDblClicked(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIROTPForm *me = (SSWR::AVIRead::AVIROTPForm*)userObj;
	UTF8Char sbuff[32];
	EntryInfo *entry = me->entryList->GetItem(index);
	if (entry->otp->GetType() == Crypto::OTP::OTPType::HOTP)
	{
		entry->otp->CodeString(sbuff, entry->otp->NextCode());
		entry->lastCounter = entry->otp->GetCounter();
		me->lvEntry->SetSubItem(index, 1, sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIROTPForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIROTPForm *me = (SSWR::AVIRead::AVIROTPForm*)userObj;
	UTF8Char sbuff[16];
	EntryInfo *entry;
	UOSInt i = me->entryList->GetCount();
	while (i-- > 0)
	{
		entry = me->entryList->GetItem(i);
		if (entry->lastCounter != entry->otp->GetCounter())
		{
			entry->otp->CodeString(sbuff, entry->otp->NextCode());
			entry->lastCounter = entry->otp->GetCounter();
			me->lvEntry->SetSubItem(i, 1, sbuff);
		}
	}
}

SSWR::AVIRead::AVIROTPForm::AVIROTPForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"One-Time Password (OTP)");
	this->SetFont(0, 8.25, false);

	this->core = core;
	NEW_CLASS(this->entryList, Data::ArrayList<EntryInfo*>());

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->grpNew, UI::GUIGroupBox(ui, this, (const UTF8Char*)"New Entry"));
	this->grpNew->SetRect(0, 0, 100, 112, false);
	this->grpNew->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblName, UI::GUILabel(ui, this->grpNew, (const UTF8Char*)"Name"));
	this->lblName->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtName, UI::GUITextBox(ui, this->grpNew, (const UTF8Char*)""));
	this->txtName->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblKey, UI::GUILabel(ui, this->grpNew, (const UTF8Char*)"Key"));
	this->lblKey->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtKey, UI::GUITextBox(ui, this->grpNew, (const UTF8Char*)""));
	this->txtKey->SetRect(104, 28, 300, 23, false);
	NEW_CLASS(this->btnKeyRand80, UI::GUIButton(ui, this->grpNew, (const UTF8Char*)"Random 80-bit"));
	this->btnKeyRand80->SetRect(404, 28, 75, 23, false);
	this->btnKeyRand80->HandleButtonClick(OnKeyRand80Clicked, this);
	NEW_CLASS(this->btnKeyRand160, UI::GUIButton(ui, this->grpNew, (const UTF8Char*)"Random 160-bit"));
	this->btnKeyRand160->SetRect(484, 28, 75, 23, false);
	this->btnKeyRand160->HandleButtonClick(OnKeyRand160Clicked, this);
	NEW_CLASS(this->lblType, UI::GUILabel(ui, this->grpNew, (const UTF8Char*)"Type"));
	this->lblType->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->cboType, UI::GUIComboBox(ui, this->grpNew, false));
	this->cboType->SetRect(104, 52, 150, 23, false);
	this->cboType->AddItem((const UTF8Char*)"Counter-Based (HOTP)", 0);
	this->cboType->AddItem((const UTF8Char*)"Time-Based (TOTP)", 0);
	this->cboType->SetSelectedIndex(0);
	NEW_CLASS(this->btnNew, UI::GUIButton(ui, this->grpNew, (const UTF8Char*)"New"));
	this->btnNew->SetRect(104, 76, 75, 23, false);
	this->btnNew->HandleButtonClick(OnNewClicked, this);
	NEW_CLASS(this->lvEntry, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvEntry->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvEntry->AddColumn((const UTF8Char*)"Name", 150);
	this->lvEntry->AddColumn((const UTF8Char*)"Code", 100);
	this->lvEntry->HandleDblClk(OnEntryDblClicked, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIROTPForm::~AVIROTPForm()
{
	EntryInfo *entry;
	UOSInt i = this->entryList->GetCount();
	while (i-- > 0)
	{
		entry = this->entryList->GetItem(i);
		Text::StrDelNew(entry->name);
		DEL_CLASS(entry->otp);
		MemFree(entry);
	}
	DEL_CLASS(this->entryList);
}

void SSWR::AVIRead::AVIROTPForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
