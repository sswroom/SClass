#include "Stdafx.h"
#include "Crypto/Hash/HashCreator.h"
#include "SSWR/AVIRead/AVIRPasswordHashForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRPasswordHashForm::OnGenerateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRPasswordHashForm *me = (SSWR::AVIRead::AVIRPasswordHashForm*)userObj;
	UInt8 buff[64];
	Text::StringBuilderUTF8 sb;
	me->txtPassword->GetText(&sb);
	if (sb.GetLength() <= 0)
	{
		me->txtPassword->Focus();
		return;
	}
	UOSInt i = me->cboHashType->GetSelectedIndex();
	if (i == INVALID_INDEX)
	{
		me->cboHashType->Focus();
		return;
	}
	Crypto::Hash::IHash *hash;
	hash = Crypto::Hash::HashCreator::CreateHash((Crypto::Hash::HashType)(OSInt)me->cboHashType->GetItem(i));
	hash->Calc(sb.ToString(), sb.GetLength());
	hash->GetValue(buff);
	sb.ClearStr();
	sb.AppendHexBuff(buff, hash->GetResultSize(), 0, Text::LineBreakType::None);
	DEL_CLASS(hash);
	me->txtHashValue->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRPasswordHashForm::AVIRPasswordHashForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 136, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Password Hash"));
	this->SetNoResize(true);

	this->core = core;

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this, (const UTF8Char*)"Password"));
	this->lblPassword->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this, CSTR("")));
	this->txtPassword->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblHashType, UI::GUILabel(ui, this, (const UTF8Char*)"Hash Type"));
	this->lblHashType->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboHashType, UI::GUIComboBox(ui, this, false));
	this->cboHashType->SetRect(104, 28, 300, 23, false);
	NEW_CLASS(this->btnGenerate, UI::GUIButton(ui, this, CSTR("Generate")));
	this->btnGenerate->SetRect(104, 52, 75, 23, false);
	this->btnGenerate->HandleButtonClick(OnGenerateClicked, this);
	NEW_CLASS(this->lblHashValue, UI::GUILabel(ui, this, (const UTF8Char*)"Hash Value"));
	this->lblHashValue->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtHashValue, UI::GUITextBox(ui, this, CSTR("")));
	this->txtHashValue->SetRect(104, 76, 400, 23, false);
	this->txtHashValue->SetReadOnly(true);

	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Crypto::Hash::IHash *hash;
	OSInt i = Crypto::Hash::HT_FIRST;
	while (i <= Crypto::Hash::HT_LAST)
	{
		hash = Crypto::Hash::HashCreator::CreateHash((Crypto::Hash::HashType)i);
		sptr = hash->GetName(sbuff);
		this->cboHashType->AddItem(CSTRP(sbuff, sptr), (void*)i);
		DEL_CLASS(hash);
		i++;
	}
}

SSWR::AVIRead::AVIRPasswordHashForm::~AVIRPasswordHashForm()
{
}

void SSWR::AVIRead::AVIRPasswordHashForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
