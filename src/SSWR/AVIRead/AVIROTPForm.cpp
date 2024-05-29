#include "Stdafx.h"
#include "Crypto/HOTP.h"
#include "Crypto/TOTP.h"
#include "Data/RandomBytesGenerator.h"
#include "SSWR/AVIRead/AVIROTPForm.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Base32Enc.h"

void SSWR::AVIRead::AVIROTPForm::RandBytes(UOSInt len)
{
	Data::RandomBytesGenerator random;
	UInt8 buff[32];
	Text::StringBuilderUTF8 sb;
	Text::TextBinEnc::Base32Enc b32;
	random.NextBytes(buff, len);
	b32.EncodeBin(sb, buff, len);
	this->txtKey->SetText(sb.ToCString());
}

void __stdcall SSWR::AVIRead::AVIROTPForm::OnKeyRand80Clicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROTPForm> me = userObj.GetNN<SSWR::AVIRead::AVIROTPForm>();
	me->RandBytes(10);
}

void __stdcall SSWR::AVIRead::AVIROTPForm::OnKeyRand160Clicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROTPForm> me = userObj.GetNN<SSWR::AVIRead::AVIROTPForm>();
	me->RandBytes(20);
}

void __stdcall SSWR::AVIRead::AVIROTPForm::OnNewClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROTPForm> me = userObj.GetNN<SSWR::AVIRead::AVIROTPForm>();
	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbKey;
	me->txtName->GetText(sbName);
	me->txtKey->GetText(sbKey);
	if (sbName.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter name"), CSTR("One-Time Password (OTP)"), me);
		return;
	}
	if (sbKey.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter key"), CSTR("One-Time Password (OTP)"), me);
		return;
	}
	if (sbKey.GetLength() > 32)
	{
		me->ui->ShowMsgOK(CSTR("Key is too long"), CSTR("One-Time Password (OTP)"), me);
		return;
	}
	if (!Text::TextBinEnc::Base32Enc::IsValid(sbKey.ToString()))
	{
		me->ui->ShowMsgOK(CSTR("Key is not valid"), CSTR("One-Time Password (OTP)"), me);
		return;
	}

	UOSInt type = me->cboType->GetSelectedIndex();
	UInt8 buff[32];
	UOSInt keySize;
	Text::TextBinEnc::Base32Enc b32;
	keySize = b32.DecodeBin(sbKey.ToCString(), buff);
	NN<EntryInfo> entry;
	entry = MemAllocNN(EntryInfo);
	entry->name = Text::String::New(sbName.ToCString());
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
	me->entryList.Add(entry);
	UOSInt i = me->lvEntry->AddItem(entry->name, entry);
	me->lvEntry->SetSubItem(i, 1, CSTR("-"));
}

void __stdcall SSWR::AVIRead::AVIROTPForm::OnEntryDblClicked(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIROTPForm> me = userObj.GetNN<SSWR::AVIRead::AVIROTPForm>();
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NN<EntryInfo> entry;
	if (me->entryList.GetItem(index).SetTo(entry) && entry->otp->GetType() == Crypto::OTP::OTPType::HOTP)
	{
		sptr = entry->otp->CodeString(sbuff, entry->otp->NextCode());
		entry->lastCounter = entry->otp->GetCounter();
		me->lvEntry->SetSubItem(index, 1, CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIROTPForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROTPForm> me = userObj.GetNN<SSWR::AVIRead::AVIROTPForm>();
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	NN<EntryInfo> entry;
	UOSInt i = me->entryList.GetCount();
	while (i-- > 0)
	{
		entry = me->entryList.GetItemNoCheck(i);
		if (entry->lastCounter != entry->otp->GetCounter())
		{
			sptr = entry->otp->CodeString(sbuff, entry->otp->NextCode());
			entry->lastCounter = entry->otp->GetCounter();
			me->lvEntry->SetSubItem(i, 1, CSTRP(sbuff, sptr));
		}
	}
}

SSWR::AVIRead::AVIROTPForm::AVIROTPForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("One-Time Password (OTP)"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->grpNew = ui->NewGroupBox(*this, CSTR("New Entry"));
	this->grpNew->SetRect(0, 0, 100, 112, false);
	this->grpNew->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblName = ui->NewLabel(this->grpNew, CSTR("Name"));
	this->lblName->SetRect(4, 4, 100, 23, false);
	this->txtName = ui->NewTextBox(this->grpNew, CSTR(""));
	this->txtName->SetRect(104, 4, 150, 23, false);
	this->lblKey = ui->NewLabel(this->grpNew, CSTR("Key"));
	this->lblKey->SetRect(4, 28, 100, 23, false);
	this->txtKey = ui->NewTextBox(this->grpNew, CSTR(""));
	this->txtKey->SetRect(104, 28, 300, 23, false);
	this->btnKeyRand80 = ui->NewButton(this->grpNew, CSTR("Random 80-bit"));
	this->btnKeyRand80->SetRect(404, 28, 75, 23, false);
	this->btnKeyRand80->HandleButtonClick(OnKeyRand80Clicked, this);
	this->btnKeyRand160 = ui->NewButton(this->grpNew, CSTR("Random 160-bit"));
	this->btnKeyRand160->SetRect(484, 28, 75, 23, false);
	this->btnKeyRand160->HandleButtonClick(OnKeyRand160Clicked, this);
	this->lblType = ui->NewLabel(this->grpNew, CSTR("Type"));
	this->lblType->SetRect(4, 52, 100, 23, false);
	this->cboType = ui->NewComboBox(this->grpNew, false);
	this->cboType->SetRect(104, 52, 150, 23, false);
	this->cboType->AddItem(CSTR("Counter-Based (HOTP)"), 0);
	this->cboType->AddItem(CSTR("Time-Based (TOTP)"), 0);
	this->cboType->SetSelectedIndex(0);
	this->btnNew = ui->NewButton(this->grpNew, CSTR("New"));
	this->btnNew->SetRect(104, 76, 75, 23, false);
	this->btnNew->HandleButtonClick(OnNewClicked, this);
	this->lvEntry = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvEntry->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvEntry->AddColumn(CSTR("Name"), 150);
	this->lvEntry->AddColumn(CSTR("Code"), 100);
	this->lvEntry->HandleDblClk(OnEntryDblClicked, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIROTPForm::~AVIROTPForm()
{
	NN<EntryInfo> entry;
	UOSInt i = this->entryList.GetCount();
	while (i-- > 0)
	{
		entry = this->entryList.GetItemNoCheck(i);
		entry->name->Release();
		DEL_CLASS(entry->otp);
		MemFreeNN(entry);
	}
}

void SSWR::AVIRead::AVIROTPForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
