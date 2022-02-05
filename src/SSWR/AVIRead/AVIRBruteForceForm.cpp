#include "Stdafx.h"
#include "Crypto/Hash/BruteForceAttack.h"
#include "Crypto/Hash/HashCreator.h"
#include "Crypto/Hash/IHash.h"
#include "SSWR/AVIRead/AVIRBruteForceForm.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"

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
	UInt8 hashBuff[64];
	UOSInt hashSize;
	if (me->bforce)
	{
		DEL_CLASS(me->bforce);
		me->bforce = 0;
		me->txtStatus->SetText((const UTF8Char*)"Stopped");
		return;
	}
	me->txtMinLen->GetText(&sb);
	minLeng = 0;
	sb.ToUInt32(&minLeng);
	sb.ClearStr();
	me->txtMaxLen->GetText(&sb);
	maxLeng = 0;
	sb.ToUInt32(&maxLeng);
	if (minLeng <= 0 || maxLeng < minLeng)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"MinLeng or MaxLeng is not valid", (const UTF8Char*)"Brute Force", me);
		return;
	}
	sb.ClearStr();
	me->txtHashValue->GetText(&sb);
	hashSize = sb.Hex2Bytes(hashBuff);
	hash = Crypto::Hash::HashCreator::CreateHash((Crypto::Hash::HashType)(OSInt)me->cboHashType->GetSelectedItem());
	if (hash == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Unsupported Hash Type", (const UTF8Char*)"Brute Force", me);
		return;
	}
	if (hash->GetResultSize() != hashSize)
	{
		DEL_CLASS(hash);
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Hash value length does not match with Hash Type", (const UTF8Char*)"Brute Force", me);
		return;
	}
	Crypto::Hash::BruteForceAttack *bforce;
	NEW_CLASS(bforce, Crypto::Hash::BruteForceAttack(hash, true, (Crypto::Hash::BruteForceAttack::CharEncoding)(OSInt)me->cboEncoding->GetSelectedItem()));
	bforce->SetCharLimit((Crypto::Hash::BruteForceAttack::CharLimit)(OSInt)me->cboCharType->GetSelectedItem());
	me->txtStatus->SetText((const UTF8Char*)"Processing");
	me->bforce = bforce;
	me->lastCnt = 0;
	bforce->Start(hashBuff, minLeng, maxLeng);
}

void __stdcall SSWR::AVIRead::AVIRBruteForceForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRBruteForceForm *me = (SSWR::AVIRead::AVIRBruteForceForm*)userObj;
	UTF8Char sbuff[64];
	if (me->bforce)
	{
		if (me->bforce->IsProcessing())
		{
			UInt64 thisCnt = me->bforce->GetTestCnt();
			
			me->bforce->GetCurrKey(Text::StrConcatC(Text::StrUInt64(Text::StrConcatC(sbuff, UTF8STRC("Processing, spd=")), thisCnt - me->lastCnt), UTF8STRC(", key=")));
			me->lastCnt = thisCnt;
			me->txtStatus->SetText(sbuff);
		}
		else
		{
			if (me->bforce->GetResult(Text::StrConcatC(sbuff, UTF8STRC("Pwd="))))
			{
				me->txtStatus->SetText(sbuff);
			}
			else
			{
				me->txtStatus->SetText((const UTF8Char*)"Finished, pwd not found");
			}
			DEL_CLASS(me->bforce);
			me->bforce = 0;			
		}
	}
}

SSWR::AVIRead::AVIRBruteForceForm::AVIRBruteForceForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 480, 240, ui)
{
	this->SetText((const UTF8Char*)"Brute Force");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->bforce = 0;
	this->lastCnt = 0;

	OSInt i;
	OSInt j;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	NEW_CLASS(this->lblHashType, UI::GUILabel(ui, this, (const UTF8Char*)"Hash Type"));
	this->lblHashType->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboHashType, UI::GUIComboBox(ui, this, false));
	this->cboHashType->SetRect(104, 4, 150, 23, false);

	Crypto::Hash::IHash *hash;
	i = Crypto::Hash::HT_FIRST;
	j = Crypto::Hash::HT_LAST;
	while (i <= j)
	{
		hash = Crypto::Hash::HashCreator::CreateHash((Crypto::Hash::HashType)i);
		sptr = hash->GetName(sbuff);
		this->cboHashType->AddItem({sbuff, (UOSInt)(sptr - sbuff)}, (void*)i);
		DEL_CLASS(hash);
		i++;
	}
	this->cboHashType->SetSelectedIndex(0);
	NEW_CLASS(this->lblHashValue, UI::GUILabel(ui, this, (const UTF8Char*)"Hash Value"));
	this->lblHashValue->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtHashValue, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtHashValue->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblEncoding, UI::GUILabel(ui, this, (const UTF8Char*)"Encoding"));
	this->lblEncoding->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->cboEncoding, UI::GUIComboBox(ui, this, false));
	this->cboEncoding->SetRect(104, 52, 150, 23, false);
	this->cboEncoding->AddItem(CSTR("UTF-8"), (void*)Crypto::Hash::BruteForceAttack::CE_UTF8);
	this->cboEncoding->AddItem(CSTR("UTF-16LE"), (void*)Crypto::Hash::BruteForceAttack::CE_UTF16LE);
	this->cboEncoding->AddItem(CSTR("UTF-32LE"), (void*)Crypto::Hash::BruteForceAttack::CE_UTF32LE);
	this->cboEncoding->SetSelectedIndex(0);
	NEW_CLASS(this->lblMinLen, UI::GUILabel(ui, this, (const UTF8Char*)"Min Length"));
	this->lblMinLen->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtMinLen, UI::GUITextBox(ui, this, (const UTF8Char*)"4"));
	this->txtMinLen->SetRect(104, 76, 100, 23, false);
	NEW_CLASS(this->lblMaxLen, UI::GUILabel(ui, this, (const UTF8Char*)"Max Length"));
	this->lblMaxLen->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtMaxLen, UI::GUITextBox(ui, this, (const UTF8Char*)"16"));
	this->txtMaxLen->SetRect(104, 100, 100, 23, false);
	NEW_CLASS(this->lblCharType, UI::GUILabel(ui, this, (const UTF8Char*)"Char Type"));
	this->lblCharType->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->cboCharType, UI::GUIComboBox(ui, this, false));
	this->cboCharType->SetRect(104, 124, 200, 23, false);
	i = Crypto::Hash::BruteForceAttack::CL_FIRST;
	j = Crypto::Hash::BruteForceAttack::CL_LAST;
	while (i <= j)
	{
		this->cboCharType->AddItem(Crypto::Hash::BruteForceAttack::CharLimitGetName((Crypto::Hash::BruteForceAttack::CharLimit)i), (void*)i);
		i++;
	}
	this->cboCharType->SetSelectedIndex(0);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(104, 148, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, (const UTF8Char*)"Status"));
	this->lblStatus->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, (const UTF8Char*)""));
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
