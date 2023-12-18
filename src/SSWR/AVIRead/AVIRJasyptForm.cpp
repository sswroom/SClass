#include "Stdafx.h"
#include "Crypto/Encrypt/JasyptEncryptor.h"
#include "SSWR/AVIRead/AVIRJasyptForm.h"

void __stdcall SSWR::AVIRead::AVIRJasyptForm::OnDecryptClicked(void *userObj)
{
	SSWR::AVIRead::AVIRJasyptForm *me = (SSWR::AVIRead::AVIRJasyptForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	me->txtPassword->GetText(sb);
	me->txtMessage->GetText(sb2);
	if (sb.GetLength() > 0 && sb2.GetLength() > 0)
	{
		UInt8 *res = MemAlloc(UInt8, sb2.GetLength());
		UOSInt resSize;
		Crypto::Encrypt::JasyptEncryptor *enc;
		NEW_CLASS(enc, Crypto::Encrypt::JasyptEncryptor((Crypto::Encrypt::JasyptEncryptor::KeyAlgorithm)(OSInt)me->cboKeyAlg->GetSelectedItem(), (Crypto::Encrypt::JasyptEncryptor::CipherAlgorithm)(OSInt)me->cboEncAlg->GetSelectedItem(), sb.ToString(), sb.GetLength()));
		resSize = enc->DecryptB64(sb2.ToCString(), res);
		sb.ClearStr();
		sb.AppendC(res, resSize);
		me->txtResult->SetText(sb.ToCString());
		DEL_CLASS(enc);
		MemFree(res);
	}
}

void __stdcall SSWR::AVIRead::AVIRJasyptForm::OnEncryptClicked(void *userObj)
{
	SSWR::AVIRead::AVIRJasyptForm *me = (SSWR::AVIRead::AVIRJasyptForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	me->txtPassword->GetText(sb);
	me->txtMessage->GetText(sb2);
	if (sb.GetLength() > 0 && sb2.GetLength() > 0)
	{
		Crypto::Encrypt::JasyptEncryptor *enc;
		NEW_CLASS(enc, Crypto::Encrypt::JasyptEncryptor((Crypto::Encrypt::JasyptEncryptor::KeyAlgorithm)(OSInt)me->cboKeyAlg->GetSelectedItem(), (Crypto::Encrypt::JasyptEncryptor::CipherAlgorithm)(OSInt)me->cboEncAlg->GetSelectedItem(), sb.ToString(), sb.GetLength()));
		sb.ClearStr();
		enc->EncryptAsB64(sb, sb2.ToString(), sb2.GetLength());
		me->txtResult->SetText(sb.ToCString());
		DEL_CLASS(enc);
	}
}

SSWR::AVIRead::AVIRJasyptForm::AVIRJasyptForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 256, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Jasypt Encryptor"));
	
	UOSInt i;
	UOSInt j;

	this->lblKeyAlg = ui->NewLabel(*this, CSTR("Key Algorithm"));
	this->lblKeyAlg->SetRect(4, 4, 100, 23, false);
	this->cboKeyAlg = ui->NewComboBox(*this, false);
	this->cboKeyAlg->SetRect(104, 4, 150, 23, false);
	i = Crypto::Encrypt::JasyptEncryptor::KA_FIRST;
	j = Crypto::Encrypt::JasyptEncryptor::KA_LAST;
	while (i <= j)
	{
		this->cboKeyAlg->AddItem(Crypto::Encrypt::JasyptEncryptor::KeyAlgorithmGetName((Crypto::Encrypt::JasyptEncryptor::KeyAlgorithm)i), (void*)i);
		i++;
	}
	this->cboKeyAlg->SetSelectedIndex(0);

	this->lblEncAlg = ui->NewLabel(*this, CSTR("Encrypt Algorithm"));
	this->lblEncAlg->SetRect(4, 28, 100, 23, false);
	this->cboEncAlg = ui->NewComboBox(*this, false);
	this->cboEncAlg->SetRect(104, 28, 150, 23, false);
	i = Crypto::Encrypt::JasyptEncryptor::CA_FIRST;
	j = Crypto::Encrypt::JasyptEncryptor::CA_LAST;
	while (i <= j)
	{
		this->cboEncAlg->AddItem(Crypto::Encrypt::JasyptEncryptor::CipherAlgorithmGetName((Crypto::Encrypt::JasyptEncryptor::CipherAlgorithm)i), (void*)i);
		i++;
	}
	this->cboEncAlg->SetSelectedIndex(0);

	this->lblPassword = ui->NewLabel(*this, CSTR("Password"));
	this->lblPassword->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtPassword->SetRect(104, 52, 150, 23, false);
	this->lblMessage = ui->NewLabel(*this, CSTR("Message"));
	this->lblMessage->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtMessage, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtMessage->SetRect(104, 76, 400, 23, false);
	this->btnDecrypt = ui->NewButton(*this, CSTR("Decrypt"));
	this->btnDecrypt->SetRect(104, 100, 75, 23, false);
	this->btnDecrypt->HandleButtonClick(OnDecryptClicked, this);
	this->btnEncrypt = ui->NewButton(*this, CSTR("Encrypt"));
	this->btnEncrypt->SetRect(184, 100, 75, 23, false);
	this->btnEncrypt->HandleButtonClick(OnEncryptClicked, this);
	this->lblResult = ui->NewLabel(*this, CSTR("Result"));
	this->lblResult->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtResult, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtResult->SetRect(104, 124, 400, 23, false);
	this->txtResult->SetReadOnly(true);
}

SSWR::AVIRead::AVIRJasyptForm::~AVIRJasyptForm()
{
}

void SSWR::AVIRead::AVIRJasyptForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
