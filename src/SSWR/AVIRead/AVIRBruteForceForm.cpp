#include "Stdafx.h"
#include "Crypto/Hash/BatchHashValidator.h"
#include "Crypto/Hash/BcryptValidator.h"
#include "Crypto/Hash/BruteForceAttack.h"
#include "Crypto/Hash/HashCreator.h"
#include "Crypto/Hash/IHash.h"
#include "SSWR/AVIRead/AVIRBruteForceForm.h"
#include "Text/MyString.h"

/*
Test Keys:
CB8E9D52EB0C2DFB39B5D82A5315E433
19A2854144B63A8F7617A6F225019B12
*/

void __stdcall SSWR::AVIRead::AVIRBruteForceForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBruteForceForm *me = (SSWR::AVIRead::AVIRBruteForceForm*)userObj;
	Crypto::Hash::IHash *hash;
	Text::StringBuilderUTF8 sb;
	UInt32 minLeng;
	UInt32 maxLeng;
	if (me->bforce)
	{
		DEL_CLASS(me->bforce);
		me->bforce = 0;
		me->txtStatus->SetText(CSTR("Stopped"));
		return;
	}
	me->txtMinLen->GetText(sb);
	minLeng = 0;
	sb.ToUInt32(minLeng);
	sb.ClearStr();
	me->txtMaxLen->GetText(sb);
	maxLeng = 0;
	sb.ToUInt32(maxLeng);
	if (minLeng <= 0 || maxLeng < minLeng)
	{
		me->ui->ShowMsgOK(CSTR("MinLeng or MaxLeng is not valid"), CSTR("Brute Force"), me);
		return;
	}
	sb.ClearStr();
	me->txtHashValue->GetText(sb);
	Crypto::Hash::HashValidator *validator = 0;
	OSInt hashType = (OSInt)me->cboHashType->GetSelectedItem();
	if (hashType < 1000)
	{
		hash = Crypto::Hash::HashCreator::CreateHash((Crypto::Hash::HashType)hashType);
		if (hash == 0)
		{
			me->ui->ShowMsgOK(CSTR("Unsupported Hash Type"), CSTR("Brute Force"), me);
			return;
		}
		NEW_CLASS(validator, Crypto::Hash::BatchHashValidator(hash, true));
	}
	else if (hashType == 1000)
	{
		NEW_CLASS(validator, Crypto::Hash::BcryptValidator());
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Unsupported Hash Type"), CSTR("Brute Force"), me);
		return;
	}
	Crypto::Hash::BruteForceAttack *bforce;
	NEW_CLASS(bforce, Crypto::Hash::BruteForceAttack(validator, (Crypto::Hash::BruteForceAttack::CharEncoding)(OSInt)me->cboEncoding->GetSelectedItem()));
	bforce->SetCharLimit((Crypto::Hash::BruteForceAttack::CharLimit)(OSInt)me->cboCharType->GetSelectedItem());
	me->lastCnt = 0;
	me->lastTime = Data::DateTimeUtil::GetCurrTimeMillis();
	if (!bforce->Start(sb.ToString(), sb.GetLength(), minLeng, maxLeng))
	{
		DEL_CLASS(bforce);
		me->ui->ShowMsgOK(CSTR("Hash value is not valid for this Hash Type"), CSTR("Brute Force"), me);
	}
	else
	{
		me->txtStatus->SetText(CSTR("Processing"));
		me->bforce = bforce;
	}
}

void __stdcall SSWR::AVIRead::AVIRBruteForceForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRBruteForceForm *me = (SSWR::AVIRead::AVIRBruteForceForm*)userObj;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if (me->bforce)
	{
		if (me->bforce->IsProcessing())
		{
			UInt64 thisCnt = me->bforce->GetTestCnt();
			Int64 thisTime = Data::DateTimeUtil::GetCurrTimeMillis();
			
			sptr = me->bforce->GetCurrKey(Text::StrConcatC(Text::StrInt64(Text::StrConcatC(sbuff, UTF8STRC("Processing, spd=")), (Int64)(thisCnt - me->lastCnt) * 1000 / (thisTime - me->lastTime)), UTF8STRC(", key=")));
			me->lastCnt = thisCnt;
			me->lastTime = thisTime;
			me->txtStatus->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			if ((sptr = me->bforce->GetResult(Text::StrConcatC(sbuff, UTF8STRC("Pwd=")))) != 0)
			{
				me->txtStatus->SetText(CSTRP(sbuff, sptr));
			}
			else
			{
				me->txtStatus->SetText(CSTR("Finished, pwd not found"));
			}
			DEL_CLASS(me->bforce);
			me->bforce = 0;			
		}
	}
}

SSWR::AVIRead::AVIRBruteForceForm::AVIRBruteForceForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 480, 240, ui)
{
	this->SetText(CSTR("Brute Force"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->bforce = 0;
	this->lastCnt = 0;
	this->lastTime = 0;

	Crypto::Hash::HashType currHash;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	this->lblHashType = ui->NewLabel(*this, CSTR("Hash Type"));
	this->lblHashType->SetRect(4, 4, 100, 23, false);
	this->cboHashType = ui->NewComboBox(*this, false);
	this->cboHashType->SetRect(104, 4, 150, 23, false);

	Crypto::Hash::IHash *hash;
	currHash = Crypto::Hash::HashType::First;
	while (currHash <= Crypto::Hash::HashType::Last)
	{
		hash = Crypto::Hash::HashCreator::CreateHash(currHash);
		sptr = hash->GetName(sbuff);
		this->cboHashType->AddItem(CSTRP(sbuff, sptr), (void*)currHash);
		DEL_CLASS(hash);
		currHash = (Crypto::Hash::HashType)((OSInt)currHash + 1);
	}
	this->cboHashType->AddItem(CSTR("Bcrypt"), (void*)1000);
	this->cboHashType->SetSelectedIndex(0);
	this->lblHashValue = ui->NewLabel(*this, CSTR("Hash Value"));
	this->lblHashValue->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtHashValue, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtHashValue->SetRect(104, 28, 200, 23, false);
	this->lblEncoding = ui->NewLabel(*this, CSTR("Encoding"));
	this->lblEncoding->SetRect(4, 52, 100, 23, false);
	this->cboEncoding = ui->NewComboBox(*this, false);
	this->cboEncoding->SetRect(104, 52, 150, 23, false);
	this->cboEncoding->AddItem(CSTR("UTF-8"), (void*)Crypto::Hash::BruteForceAttack::CharEncoding::UTF8);
	this->cboEncoding->AddItem(CSTR("UTF-16LE"), (void*)Crypto::Hash::BruteForceAttack::CharEncoding::UTF16LE);
	this->cboEncoding->AddItem(CSTR("UTF-32LE"), (void*)Crypto::Hash::BruteForceAttack::CharEncoding::UTF32LE);
	this->cboEncoding->SetSelectedIndex(0);
	this->lblMinLen = ui->NewLabel(*this, CSTR("Min Length"));
	this->lblMinLen->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtMinLen, UI::GUITextBox(ui, *this, CSTR("4")));
	this->txtMinLen->SetRect(104, 76, 100, 23, false);
	this->lblMaxLen = ui->NewLabel(*this, CSTR("Max Length"));
	this->lblMaxLen->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtMaxLen, UI::GUITextBox(ui, *this, CSTR("16")));
	this->txtMaxLen->SetRect(104, 100, 100, 23, false);
	this->lblCharType = ui->NewLabel(*this, CSTR("Char Type"));
	this->lblCharType->SetRect(4, 124, 100, 23, false);
	this->cboCharType = ui->NewComboBox(*this, false);
	this->cboCharType->SetRect(104, 124, 200, 23, false);
	Crypto::Hash::BruteForceAttack::CharLimit currLimit = Crypto::Hash::BruteForceAttack::CharLimit::First;
	while (currLimit <= Crypto::Hash::BruteForceAttack::CharLimit::Last)
	{
		this->cboCharType->AddItem(Crypto::Hash::BruteForceAttack::CharLimitGetName(currLimit), (void*)currLimit);
		currLimit = (Crypto::Hash::BruteForceAttack::CharLimit)((OSInt)currLimit + 1);
	}
	this->cboCharType->SetSelectedIndex(0);
	this->btnStart = ui->NewButton(*this, CSTR("Start"));
	this->btnStart->SetRect(104, 148, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtStatus->SetRect(104, 172, 250, 23, false);
	this->txtStatus->SetReadOnly(true);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRBruteForceForm::~AVIRBruteForceForm()
{
	SDEL_CLASS(this->bforce);
}

void SSWR::AVIRead::AVIRBruteForceForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
