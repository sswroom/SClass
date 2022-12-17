#include "Stdafx.h"
#include "Crypto/Encrypt/AES128.h"
#include "Crypto/Encrypt/AES192.h"
#include "Crypto/Encrypt/AES256.h"
#include "SSWR/AVIRead/AVIREncryptMsgForm.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/HexTextBinEnc.h"
#include "Text/TextBinEnc/UTF8TextBinEnc.h"
#include "UI/GUIComboBoxUtil.h"
#include "UI/MessageDialog.h"

Crypto::Encrypt::ICrypto *SSWR::AVIRead::AVIREncryptMsgForm::InitCrypto()
{
	UOSInt blockSize;
	UOSInt keySize;
	UOSInt algType = this->cboAlgorithm->GetSelectedIndex();
	switch (algType)
	{
	case 0:
		keySize = 16;
		blockSize = 16;
		break;
	case 1:
		keySize = 24;
		blockSize = 16;
		break;
	case 2:
		keySize = 32;
		blockSize = 16;
		break;
	default:
		UI::MessageDialog::ShowDialog(CSTR("Unknown Algorithm"), CSTR("Encrypt Message"), this);
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	this->txtKey->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Key cannot be empty"), CSTR("Encrypt Message"), this);
		return 0;
	}
	Text::TextBinEnc::ITextBinEnc *enc = GetTextEncType(this->cboKeyIVType);
	UOSInt buffSize = enc->CalcBinSize(sb.ToString(), sb.GetLength());
	if (buffSize == 0)
	{
		DEL_CLASS(enc);
		UI::MessageDialog::ShowDialog(CSTR("Error in parsing key"), CSTR("Encrypt Message"), this);
		return 0;
	}
	else if (buffSize > keySize)
	{
		DEL_CLASS(enc);
		UI::MessageDialog::ShowDialog(CSTR("Key size is too long"), CSTR("Encrypt Message"), this);
		return 0;
	}
	UInt8 key[32];
	UInt8 iv[32];
	Bool hasIV = false;
	MemClear(key, keySize);
	enc->DecodeBin(sb.ToString(), sb.GetLength(), key);
	sb.ClearStr();
	this->txtIV->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		hasIV = true;
		buffSize = enc->CalcBinSize(sb.ToString(), sb.GetLength());
		if (buffSize == 0)
		{
			DEL_CLASS(enc);
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing IV"), CSTR("Encrypt Message"), this);
			return 0;
		}
		else if (buffSize > blockSize)
		{
			DEL_CLASS(enc);
			UI::MessageDialog::ShowDialog(CSTR("IV size is too long"), CSTR("Encrypt Message"), this);
			return 0;
		}
		MemClear(iv, blockSize);
		enc->DecodeBin(sb.ToString(), sb.GetLength(), iv);
	}
	DEL_CLASS(enc);
	Crypto::Encrypt::BlockCipher *crypto;
	switch (algType)
	{
	case 0:
		NEW_CLASS(crypto, Crypto::Encrypt::AES128(key));
		break;
	case 1:
		NEW_CLASS(crypto, Crypto::Encrypt::AES192(key));
		break;
	case 2:
		NEW_CLASS(crypto, Crypto::Encrypt::AES256(key));
		break;
	default:
		return 0;
	}
	if (hasIV)
	{
		crypto->SetIV(iv);
	}
	crypto->SetChainMode((Crypto::Encrypt::ChainMode)(OSInt)this->cboChainMode->GetSelectedItem());
	return crypto;
}

UInt8 *SSWR::AVIRead::AVIREncryptMsgForm::InitInput(UOSInt blockSize, UOSInt *dataSize)
{
	Text::StringBuilderUTF8 sb;
	this->txtInputMsg->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Input Msg is empty"), CSTR("Encrypt Message"), this);
	}
	Text::TextBinEnc::ITextBinEnc *enc = GetTextEncType(this->cboInputType);
	UOSInt buffSize = enc->CalcBinSize(sb.ToString(), sb.GetLength());
	if (buffSize == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in parsing Input Msg"), CSTR("Encrypt Message"), this);
		DEL_CLASS(enc);
		return 0;
	}
	UOSInt nBlock = (buffSize + blockSize - 1) / blockSize;
	*dataSize = buffSize;
	buffSize = nBlock * blockSize;
	UInt8 *input = MemAlloc(UInt8, buffSize);
	enc->DecodeBin(sb.ToString(), sb.GetLength(), input);
	DEL_CLASS(enc);
	return input;
}

void SSWR::AVIRead::AVIREncryptMsgForm::ShowOutput(const UInt8 *buff, UOSInt buffSize)
{
	Text::TextBinEnc::ITextBinEnc *enc = GetTextEncType(this->cboOutputType);
	Text::StringBuilderUTF8 sb;
	enc->EncodeBin(&sb, buff, buffSize);
	this->txtOutputMsg->SetText(sb.ToCString());
	DEL_CLASS(enc);
}

void __stdcall SSWR::AVIRead::AVIREncryptMsgForm::OnEncryptClicked(void *userObj)
{
	SSWR::AVIRead::AVIREncryptMsgForm *me = (SSWR::AVIRead::AVIREncryptMsgForm *)userObj;
	Crypto::Encrypt::ICrypto *crypto = me->InitCrypto();
	if (crypto)
	{
		UOSInt buffSize;
		UInt8 *buff = me->InitInput(crypto->GetEncBlockSize(), &buffSize);
		if (buff)
		{
			buffSize = crypto->Encrypt(buff, buffSize, buff, 0);
			me->ShowOutput(buff, buffSize);
			MemFree(buff);
		}
		DEL_CLASS(crypto);
	}
}

void __stdcall SSWR::AVIRead::AVIREncryptMsgForm::OnDecryptClicked(void *userObj)
{
	SSWR::AVIRead::AVIREncryptMsgForm *me = (SSWR::AVIRead::AVIREncryptMsgForm *)userObj;
	Crypto::Encrypt::ICrypto *crypto = me->InitCrypto();
	if (crypto)
	{
		UOSInt buffSize;
		UInt8 *buff = me->InitInput(crypto->GetDecBlockSize(), &buffSize);
		if (buff)
		{
			buffSize = crypto->Decrypt(buff, buffSize, buff, 0);
			me->ShowOutput(buff, buffSize);
			MemFree(buff);
		}
		DEL_CLASS(crypto);
	}
}

Text::TextBinEnc::ITextBinEnc *SSWR::AVIRead::AVIREncryptMsgForm::GetTextEncType(UI::GUIComboBox *cbo)
{
	switch (cbo->GetSelectedIndex())
	{
	case 0:
	default:
		return NEW_CLASS_D(Text::TextBinEnc::UTF8TextBinEnc());
	case 1:
		return NEW_CLASS_D(Text::TextBinEnc::HexTextBinEnc());
	case 2:
		return NEW_CLASS_D(Text::TextBinEnc::Base64Enc());
	}
}
			
void SSWR::AVIRead::AVIREncryptMsgForm::AddTextEncType(UI::GUIComboBox *cbo)
{
	cbo->AddItem(CSTR("UTF-8"), 0);
	cbo->AddItem(CSTR("Hex"), 0);
	cbo->AddItem(CSTR("Base64"), 0);
	cbo->SetSelectedIndex(0);
}

SSWR::AVIRead::AVIREncryptMsgForm::AVIREncryptMsgForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Encrypt Message"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblAlgorithm, UI::GUILabel(ui, this, CSTR("Algorithm")));
	this->lblAlgorithm->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboAlgorithm, UI::GUIComboBox(ui, this, false));
	this->cboAlgorithm->SetRect(104, 4, 150, 23, false);
	this->cboAlgorithm->AddItem(CSTR("AES 128-bit"), 0);
	this->cboAlgorithm->AddItem(CSTR("AES 192-bit"), 0);
	this->cboAlgorithm->AddItem(CSTR("AES 256-bit"), 0);
	this->cboAlgorithm->SetSelectedIndex(0);
	NEW_CLASS(this->lblKeyIVType, UI::GUILabel(ui, this, CSTR("Key/IV Type")));
	this->lblKeyIVType->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboKeyIVType, UI::GUIComboBox(ui, this, false));
	this->cboKeyIVType->SetRect(104, 28, 100, 23, false);
	AddTextEncType(this->cboKeyIVType);
	NEW_CLASS(this->lblKey, UI::GUILabel(ui, this, CSTR("Key")));
	this->lblKey->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtKey, UI::GUITextBox(ui, this, CSTR("")));
	this->txtKey->SetRect(104, 52, 300, 23, false);
	NEW_CLASS(this->lblIV, UI::GUILabel(ui, this, CSTR("IV")));
	this->lblIV->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtIV, UI::GUITextBox(ui, this, CSTR("")));
	this->txtIV->SetRect(104, 76, 300, 23, false);
	NEW_CLASS(this->lblChainMode, UI::GUILabel(ui, this, CSTR("Chain Mode")));
	this->lblChainMode->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->cboChainMode, UI::GUIComboBox(ui, this, false));
	this->cboChainMode->SetRect(104, 100, 100, 23, false);
	CBOADDENUM(this->cboChainMode, Crypto::Encrypt::ChainMode, ECB);
	CBOADDENUM(this->cboChainMode, Crypto::Encrypt::ChainMode, CBC);
	CBOADDENUM(this->cboChainMode, Crypto::Encrypt::ChainMode, PCBC);
	CBOADDENUM(this->cboChainMode, Crypto::Encrypt::ChainMode, CFB);
	CBOADDENUM(this->cboChainMode, Crypto::Encrypt::ChainMode, OFB);
	this->cboChainMode->SetSelectedIndex(0);
	NEW_CLASS(this->lblInputType, UI::GUILabel(ui, this, CSTR("Input Type")));
	this->lblInputType->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->cboInputType, UI::GUIComboBox(ui, this, false));
	this->cboInputType->SetRect(104, 124, 100, 23, false);
	AddTextEncType(this->cboInputType);
	NEW_CLASS(this->lblInputMsg, UI::GUILabel(ui, this, CSTR("Input Msg")));
	this->lblInputMsg->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtInputMsg, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtInputMsg->SetRect(104, 148, 400, 71, false);
	NEW_CLASS(this->lblOutputType, UI::GUILabel(ui, this, CSTR("Output Type")));
	this->lblOutputType->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->cboOutputType, UI::GUIComboBox(ui, this, false));
	this->cboOutputType->SetRect(104, 220, 100, 23, false);
	AddTextEncType(this->cboOutputType);
	NEW_CLASS(this->btnEncrypt, UI::GUIButton(ui, this, CSTR("Encrypt")));
	this->btnEncrypt->SetRect(104, 244, 75, 23, false);
	this->btnEncrypt->HandleButtonClick(OnEncryptClicked, this);
	NEW_CLASS(this->btnDecrypt, UI::GUIButton(ui, this, CSTR("Decrypt")));
	this->btnDecrypt->SetRect(184, 244, 75, 23, false);
	this->btnDecrypt->HandleButtonClick(OnDecryptClicked, this);
	NEW_CLASS(this->lblOutputMsg, UI::GUILabel(ui, this, CSTR("Output Msg")));
	this->lblOutputMsg->SetRect(4, 268, 100, 23, false);
	NEW_CLASS(this->txtOutputMsg, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtOutputMsg->SetRect(104, 268, 400, 71, false);
	this->txtOutputMsg->SetReadOnly(true);
}

SSWR::AVIRead::AVIREncryptMsgForm::~AVIREncryptMsgForm()
{
}

void SSWR::AVIRead::AVIREncryptMsgForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
