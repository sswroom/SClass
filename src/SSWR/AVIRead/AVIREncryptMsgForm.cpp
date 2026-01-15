#include "Stdafx.h"
#include "Crypto/Encrypt/AES128.h"
#include "Crypto/Encrypt/AES192.h"
#include "Crypto/Encrypt/AES256.h"
#include "Data/RandomBytesGenerator.h"
#include "SSWR/AVIRead/AVIREncryptMsgForm.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/HexTextBinEnc.h"
#include "Text/TextBinEnc/UTF8TextBinEnc.h"
#include "UI/GUIComboBoxUtil.h"

Optional<Crypto::Encrypt::Encryption> SSWR::AVIRead::AVIREncryptMsgForm::InitCrypto()
{
	UIntOS keySize;
	UIntOS algType = this->cboAlgorithm->GetSelectedIndex();
	switch (algType)
	{
	case 0:
		keySize = 16;
		break;
	case 1:
		keySize = 24;
		break;
	case 2:
		keySize = 32;
		break;
	default:
		this->ui->ShowMsgOK(CSTR("Unknown Algorithm"), CSTR("Encrypt Message"), this);
		return nullptr;
	}
	Text::StringBuilderUTF8 sb;
	this->txtKey->GetText(sb);
	if (sb.GetLength() == 0)
	{
		this->ui->ShowMsgOK(CSTR("Key cannot be empty"), CSTR("Encrypt Message"), this);
		return nullptr;
	}
	NN<Text::TextBinEnc::TextBinEnc> enc = GetTextEncType(this->cboKeyType);
	UIntOS buffSize = enc->CalcBinSize(sb.ToCString());
	if (buffSize == 0)
	{
		enc.Delete();
		this->ui->ShowMsgOK(CSTR("Error in parsing key"), CSTR("Encrypt Message"), this);
		return nullptr;
	}
	else if (buffSize > keySize)
	{
		enc.Delete();
		this->ui->ShowMsgOK(CSTR("Key size is too long"), CSTR("Encrypt Message"), this);
		return nullptr;
	}
	UInt8 key[32];
	MemClear(key, keySize);
	enc->DecodeBin(sb.ToCString(), key);
	enc.Delete();
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
		return nullptr;
	}
	crypto->SetChainMode((Crypto::Encrypt::ChainMode)this->cboChainMode->GetSelectedItem().GetIntOS());
	return crypto;
}

UnsafeArrayOpt<UInt8> SSWR::AVIRead::AVIREncryptMsgForm::InitInput(UIntOS blockSize, OutParam<UIntOS> dataSize)
{
	Text::StringBuilderUTF8 sb;
	this->txtInputMsg->GetText(sb);
	if (sb.GetLength() == 0)
	{
		this->ui->ShowMsgOK(CSTR("Input Msg is empty"), CSTR("Encrypt Message"), this);
	}
	NN<Text::TextBinEnc::TextBinEnc> enc = GetTextEncType(this->cboInputType);
	UIntOS buffSize = enc->CalcBinSize(sb.ToCString());
	if (buffSize == 0)
	{
		this->ui->ShowMsgOK(CSTR("Error in parsing Input Msg"), CSTR("Encrypt Message"), this);
		enc.Delete();
		return nullptr;
	}
	UIntOS nBlock = (buffSize + blockSize - 1) / blockSize;
	dataSize.Set(buffSize);
	buffSize = nBlock * blockSize;
	UnsafeArray<UInt8> input = MemAllocArr(UInt8, buffSize);
	enc->DecodeBin(sb.ToCString(), input);
	enc.Delete();
	return input;
}

UnsafeArrayOpt<UInt8> SSWR::AVIRead::AVIREncryptMsgForm::InitIV(NN<Crypto::Encrypt::Encryption> crypto, UnsafeArray<UInt8> dataBuff, InOutParam<UIntOS> buffSize, UIntOS blockSize, Bool enc)
{
	UInt8 tmpbuff[256];
	UIntOS ivType = this->cboIV->GetSelectedIndex();
	switch (ivType)
	{
	case 0:
		return dataBuff;
	case 1:
		{
			Text::StringBuilderUTF8 sb;
			this->txtIV->GetText(sb);
			if (sb.GetLength() > sizeof(tmpbuff) || sb.Hex2Bytes(tmpbuff) != blockSize)
			{
				this->ui->ShowMsgOK(CSTR("IV input length not valid"), CSTR("Encrypt Message"), this);
				return nullptr;
			}
			NN<Crypto::Encrypt::BlockCipher>::ConvertFrom(crypto)->SetIV(tmpbuff);
			return dataBuff;
		}
	case 2:
		{
			Text::StringBuilderUTF8 sb;
			this->txtIV->GetText(sb);
			if (sb.GetLength() > sizeof(tmpbuff))
			{
				this->ui->ShowMsgOK(CSTR("IV input length too long"), CSTR("Encrypt Message"), this);
				return nullptr;
			}
			Text::TextBinEnc::Base64Enc b64;
			if (b64.DecodeBin(sb.ToCString(), tmpbuff) != blockSize)
			{
				this->ui->ShowMsgOK(CSTR("IV input length not valid"), CSTR("Encrypt Message"), this);
				return nullptr;
			}
			NN<Crypto::Encrypt::BlockCipher>::ConvertFrom(crypto)->SetIV(tmpbuff);
			return dataBuff;
		}
	case 3:
		if (enc)
		{
			Data::RandomBytesGenerator byteGen;
			byteGen.NextBytes(dataBuff, blockSize);
			NN<Crypto::Encrypt::BlockCipher>::ConvertFrom(crypto)->SetIV(dataBuff);
			buffSize.Set(buffSize.Get() + blockSize);
			return dataBuff + blockSize;
		}
		else
		{
			if (buffSize.Get() < blockSize)
			{
				this->ui->ShowMsgOK(CSTR("Data input length too short"), CSTR("Encrypt Message"), this);
				return nullptr;
			}
			NN<Crypto::Encrypt::BlockCipher>::ConvertFrom(crypto)->SetIV(dataBuff);
			buffSize.Set(buffSize.Get() - blockSize);
			return dataBuff + blockSize;
		}
	}
	return nullptr;
}

void SSWR::AVIRead::AVIREncryptMsgForm::ShowOutput(UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	NN<Text::TextBinEnc::TextBinEnc> enc = GetTextEncType(this->cboOutputType);
	Text::StringBuilderUTF8 sb;
	enc->EncodeBin(sb, buff, buffSize);
	this->txtOutputMsg->SetText(sb.ToCString());
	enc.Delete();
}

void __stdcall SSWR::AVIRead::AVIREncryptMsgForm::OnEncryptClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREncryptMsgForm> me = userObj.GetNN<SSWR::AVIRead::AVIREncryptMsgForm>();
	NN<Crypto::Encrypt::Encryption> crypto;
	if (me->InitCrypto().SetTo(crypto))
	{
		UIntOS buffSize;
		UnsafeArray<UInt8> buff;
		if (me->InitInput(crypto->GetEncBlockSize(), buffSize).SetTo(buff))
		{
			UnsafeArray<UInt8> dataBuff;
			if (me->InitIV(crypto, buff, buffSize, crypto->GetEncBlockSize(), true).SetTo(dataBuff))
			{
				buffSize = crypto->Encrypt(dataBuff, buffSize, dataBuff);
				me->ShowOutput(buff, buffSize + (UIntOS)(dataBuff - buff));
			}
			MemFreeArr(buff);
		}
		crypto.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIREncryptMsgForm::OnDecryptClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREncryptMsgForm> me = userObj.GetNN<SSWR::AVIRead::AVIREncryptMsgForm>();
	NN<Crypto::Encrypt::Encryption> crypto;
	if (me->InitCrypto().SetTo(crypto))
	{
		UIntOS buffSize;
		UnsafeArray<UInt8> buff;
		if (me->InitInput(crypto->GetDecBlockSize(), buffSize).SetTo(buff))
		{
			UnsafeArray<UInt8> dataBuff;
			if (me->InitIV(crypto, buff, buffSize, crypto->GetDecBlockSize(), false).SetTo(dataBuff))
			{
				buffSize = crypto->Decrypt(dataBuff, buffSize, dataBuff);
				me->ShowOutput(dataBuff, buffSize);
			}
			MemFreeArr(buff);
		}
		crypto.Delete();
	}
}

NN<Text::TextBinEnc::TextBinEnc> SSWR::AVIRead::AVIREncryptMsgForm::GetTextEncType(NN<UI::GUIComboBox> cbo)
{
	NN<Text::TextBinEnc::TextBinEnc> enc;
	switch (cbo->GetSelectedIndex())
	{
	case 0:
	default:
		NEW_CLASSNN(enc, Text::TextBinEnc::UTF8TextBinEnc());
		return enc;
	case 1:
		NEW_CLASSNN(enc, Text::TextBinEnc::HexTextBinEnc());
		return enc;
	case 2:
		NEW_CLASSNN(enc, Text::TextBinEnc::Base64Enc());
		return enc;
	}
}
			
void SSWR::AVIRead::AVIREncryptMsgForm::AddTextEncType(NN<UI::GUIComboBox> cbo)
{
	cbo->AddItem(CSTR("UTF-8"), 0);
	cbo->AddItem(CSTR("Hex"), 0);
	cbo->AddItem(CSTR("Base64"), 0);
	cbo->SetSelectedIndex(0);
}

SSWR::AVIRead::AVIREncryptMsgForm::AVIREncryptMsgForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Encrypt Message"));
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblAlgorithm = ui->NewLabel(*this, CSTR("Algorithm"));
	this->lblAlgorithm->SetRect(4, 4, 100, 23, false);
	this->cboAlgorithm = ui->NewComboBox(*this, false);
	this->cboAlgorithm->SetRect(104, 4, 150, 23, false);
	this->cboAlgorithm->AddItem(CSTR("AES 128-bit"), 0);
	this->cboAlgorithm->AddItem(CSTR("AES 192-bit"), 0);
	this->cboAlgorithm->AddItem(CSTR("AES 256-bit"), 0);
	this->cboAlgorithm->SetSelectedIndex(0);
	this->lblKeyType = ui->NewLabel(*this, CSTR("Key Type"));
	this->lblKeyType->SetRect(4, 28, 100, 23, false);
	this->cboKeyType = ui->NewComboBox(*this, false);
	this->cboKeyType->SetRect(104, 28, 100, 23, false);
	AddTextEncType(this->cboKeyType);
	this->lblKey = ui->NewLabel(*this, CSTR("Key"));
	this->lblKey->SetRect(4, 52, 100, 23, false);
	this->txtKey = ui->NewTextBox(*this, CSTR(""));
	this->txtKey->SetRect(104, 52, 300, 23, false);
	this->lblIV = ui->NewLabel(*this, CSTR("IV"));
	this->lblIV->SetRect(4, 76, 100, 23, false);
	this->cboIV = ui->NewComboBox(*this, false);
	this->cboIV->SetRect(104, 76, 100, 23, false);
	this->cboIV->AddItem(CSTR("Zero IV"), 0);
	this->cboIV->AddItem(CSTR("Hex Input"), 0);
	this->cboIV->AddItem(CSTR("Base64 Input"), 0);
	this->cboIV->AddItem(CSTR("Data beginning"), 0);
	this->cboIV->SetSelectedIndex(0);
	this->txtIV = ui->NewTextBox(*this, CSTR(""));
	this->txtIV->SetRect(204, 76, 300, 23, false);
	this->lblChainMode = ui->NewLabel(*this, CSTR("Chain Mode"));
	this->lblChainMode->SetRect(4, 100, 100, 23, false);
	this->cboChainMode = ui->NewComboBox(*this, false);
	this->cboChainMode->SetRect(104, 100, 100, 23, false);
	CBOADDENUM(this->cboChainMode, Crypto::Encrypt::ChainMode, ECB);
	CBOADDENUM(this->cboChainMode, Crypto::Encrypt::ChainMode, CBC);
	CBOADDENUM(this->cboChainMode, Crypto::Encrypt::ChainMode, PCBC);
	CBOADDENUM(this->cboChainMode, Crypto::Encrypt::ChainMode, CFB);
	CBOADDENUM(this->cboChainMode, Crypto::Encrypt::ChainMode, OFB);
	this->cboChainMode->SetSelectedIndex(0);
	this->lblInputType = ui->NewLabel(*this, CSTR("Input Type"));
	this->lblInputType->SetRect(4, 124, 100, 23, false);
	this->cboInputType = ui->NewComboBox(*this, false);
	this->cboInputType->SetRect(104, 124, 100, 23, false);
	AddTextEncType(this->cboInputType);
	this->lblInputMsg = ui->NewLabel(*this, CSTR("Input Msg"));
	this->lblInputMsg->SetRect(4, 148, 100, 23, false);
	this->txtInputMsg = ui->NewTextBox(*this, CSTR(""), true);
	this->txtInputMsg->SetRect(104, 148, 400, 71, false);
	this->lblOutputType = ui->NewLabel(*this, CSTR("Output Type"));
	this->lblOutputType->SetRect(4, 220, 100, 23, false);
	this->cboOutputType = ui->NewComboBox(*this, false);
	this->cboOutputType->SetRect(104, 220, 100, 23, false);
	AddTextEncType(this->cboOutputType);
	this->btnEncrypt = ui->NewButton(*this, CSTR("Encrypt"));
	this->btnEncrypt->SetRect(104, 244, 75, 23, false);
	this->btnEncrypt->HandleButtonClick(OnEncryptClicked, this);
	this->btnDecrypt = ui->NewButton(*this, CSTR("Decrypt"));
	this->btnDecrypt->SetRect(184, 244, 75, 23, false);
	this->btnDecrypt->HandleButtonClick(OnDecryptClicked, this);
	this->lblOutputMsg = ui->NewLabel(*this, CSTR("Output Msg"));
	this->lblOutputMsg->SetRect(4, 268, 100, 23, false);
	this->txtOutputMsg = ui->NewTextBox(*this, CSTR(""), true);
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
