#include "Stdafx.h"
#include "Crypto/Encrypt/AES128.h"
#include "Crypto/Encrypt/AES192.h"
#include "Crypto/Encrypt/AES256.h"
#include "Data/RandomBytesGenerator.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIREncryptFileForm.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/HexTextBinEnc.h"
#include "Text/TextBinEnc/UTF8TextBinEnc.h"
#include "UI/GUIComboBoxUtil.h"

Optional<Crypto::Encrypt::ICrypto> SSWR::AVIRead::AVIREncryptFileForm::InitCrypto()
{
	UOSInt keySize;
	UOSInt algType = this->cboAlgorithm->GetSelectedIndex();
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
		this->ui->ShowMsgOK(CSTR("Unknown Algorithm"), CSTR("Encrypt File"), this);
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	this->txtKey->GetText(sb);
	if (sb.GetLength() == 0)
	{
		this->ui->ShowMsgOK(CSTR("Key cannot be empty"), CSTR("Encrypt File"), this);
		return 0;
	}
	NN<Text::TextBinEnc::ITextBinEnc> enc = GetTextEncType(this->cboKeyType);
	UOSInt buffSize = enc->CalcBinSize(sb.ToCString());
	if (buffSize == 0)
	{
		enc.Delete();
		this->ui->ShowMsgOK(CSTR("Error in parsing key"), CSTR("Encrypt File"), this);
		return 0;
	}
	else if (buffSize > keySize)
	{
		enc.Delete();
		this->ui->ShowMsgOK(CSTR("Key size is too long"), CSTR("Encrypt File"), this);
		return 0;
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
		return 0;
	}
	crypto->SetChainMode((Crypto::Encrypt::ChainMode)this->cboChainMode->GetSelectedItem().GetOSInt());
	crypto->SetPaddingMode((Crypto::Encrypt::PaddingMode)this->cboPaddingMode->GetSelectedItem().GetOSInt());
	return crypto;
}

UnsafeArrayOpt<UInt8> SSWR::AVIRead::AVIREncryptFileForm::InitInput(UOSInt blockSize, OutParam<UOSInt> dataSize)
{
	Text::StringBuilderUTF8 sb;
	this->txtInputFile->GetText(sb);
	if (sb.GetLength() == 0)
	{
		this->ui->ShowMsgOK(CSTR("Please input Input File"), CSTR("Encrypt File"), this);
		return 0;
	}
	IO::FileStream fs(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		this->ui->ShowMsgOK(CSTR("Error in opening Input File"), CSTR("Encrypt File"), this);
		return 0;
	}
	UInt64 fileSize = fs.GetLength();
	if (fileSize > 0x7fffffff)
	{
		this->ui->ShowMsgOK(CSTR("Input File is too large to encrypt"), CSTR("Encrypt File"), this);
		return 0;
	}
	else if (fileSize == 0)
	{
		this->ui->ShowMsgOK(CSTR("Input File is empty"), CSTR("Encrypt File"), this);
		return 0;
	}
	UOSInt buffSize = (UOSInt)fileSize;
	UOSInt nBlock = (buffSize + blockSize) / blockSize;
	dataSize.Set(buffSize);
	buffSize = nBlock * blockSize;
	UnsafeArray<UInt8> input = MemAllocArr(UInt8, buffSize);
	fs.Read(Data::ByteArray(input, (UOSInt)fileSize));
	return input;
}

UnsafeArrayOpt<UInt8> SSWR::AVIRead::AVIREncryptFileForm::InitIV(NN<Crypto::Encrypt::ICrypto> crypto, UnsafeArray<UInt8> dataBuff, InOutParam<UOSInt> buffSize, UOSInt blockSize, Bool enc)
{
	UInt8 tmpbuff[256];
	UOSInt ivType = this->cboIV->GetSelectedIndex();
	switch (ivType)
	{
	case 0:
		return dataBuff;
	case 1:
		{
			Text::StringBuilderUTF8 sb;
			this->txtIV->GetText(sb);
			if (sb.GetLength() > sizeof(tmpbuff))
			{
				this->ui->ShowMsgOK(CSTR("IV input length not valid"), CSTR("Encrypt File"), this);
				return 0;
			}
			UOSInt ivLeng = sb.Hex2Bytes(tmpbuff);
			if (ivLeng > blockSize)
			{
				this->ui->ShowMsgOK(CSTR("IV input length too long"), CSTR("Encrypt File"), this);
				return 0;
			}
			else if (ivLeng < blockSize)
			{
				this->ui->ShowMsgOK(CSTR("IV input length too short, will pad with zero bytes"), CSTR("Encrypt File"), this);
				MemClear(&tmpbuff[ivLeng], blockSize - ivLeng);
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
				this->ui->ShowMsgOK(CSTR("IV input length too long"), CSTR("Encrypt File"), this);
				return 0;
			}
			Text::TextBinEnc::Base64Enc b64;
			UOSInt ivLeng = b64.DecodeBin(sb.ToCString(), tmpbuff);
			if (ivLeng > blockSize)
			{
				this->ui->ShowMsgOK(CSTR("IV input length too long"), CSTR("Encrypt File"), this);
				return 0;
			}
			else if (ivLeng < blockSize)
			{
				this->ui->ShowMsgOK(CSTR("IV input length too short, will pad with zero bytes"), CSTR("Encrypt File"), this);
				MemClear(&tmpbuff[ivLeng], blockSize - ivLeng);
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
				this->ui->ShowMsgOK(CSTR("Data input length too short"), CSTR("Encrypt File"), this);
				return 0;
			}
			NN<Crypto::Encrypt::BlockCipher>::ConvertFrom(crypto)->SetIV(dataBuff);
			buffSize.Set(buffSize.Get() - blockSize);
			return dataBuff + blockSize;
		}
	}
	return 0;
}

void SSWR::AVIRead::AVIREncryptFileForm::SaveOutput(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	Text::StringBuilderUTF8 sb;
	this->txtOutputFile->GetText(sb);
	IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	fs.WriteCont(buff, buffSize);
}

void SSWR::AVIRead::AVIREncryptFileForm::SetInputFile(Text::CStringNN file)
{
	this->txtInputFile->SetText(file);
	Text::StringBuilderUTF8 sb;
	sb.Append(file);
	if (sb.EndsWith(CSTR(".enc")))
	{
		sb.RemoveChars(4);
	}
	else
	{
		sb.Append(CSTR(".enc"));
	}
	this->txtOutputFile->SetText(sb.ToCString());
	if (file.IndexOfICase(CSTR("aes128")) != INVALID_INDEX)
	{
		this->cboAlgorithm->SetSelectedIndex(0);
	}
	else if (file.IndexOfICase(CSTR("aes192")) != INVALID_INDEX)
	{
		this->cboAlgorithm->SetSelectedIndex(1);
	}
	else if (file.IndexOfICase(CSTR("aes256")) != INVALID_INDEX)
	{
		this->cboAlgorithm->SetSelectedIndex(2);
	}

	if (file.IndexOfICase(CSTR("ECB")) != INVALID_INDEX)
	{
		this->cboChainMode->SetSelectedIndex(0);
	}
	else if (file.IndexOfICase(CSTR("PCBC")) != INVALID_INDEX)
	{
		this->cboChainMode->SetSelectedIndex(2);
	}
	else if (file.IndexOfICase(CSTR("CBC")) != INVALID_INDEX)
	{
		this->cboChainMode->SetSelectedIndex(1);
	}
	else if (file.IndexOfICase(CSTR("CFB")) != INVALID_INDEX)
	{
		this->cboChainMode->SetSelectedIndex(3);
	}
	else if (file.IndexOfICase(CSTR("OFB")) != INVALID_INDEX)
	{
		this->cboChainMode->SetSelectedIndex(4);
	}
	else if (file.IndexOfICase(CSTR("CTR")) != INVALID_INDEX)
	{
		this->cboChainMode->SetSelectedIndex(5);
	}
}

void __stdcall SSWR::AVIRead::AVIREncryptFileForm::OnEncryptClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREncryptFileForm> me = userObj.GetNN<SSWR::AVIRead::AVIREncryptFileForm>();
	NN<Crypto::Encrypt::ICrypto> crypto;
	if (me->InitCrypto().SetTo(crypto))
	{
		UOSInt buffSize;
		UnsafeArray<UInt8> buff;
		if (me->InitInput(crypto->GetEncBlockSize(), buffSize).SetTo(buff))
		{
			UnsafeArray<UInt8> dataBuff;
			if (me->InitIV(crypto, buff, buffSize, crypto->GetEncBlockSize(), true).SetTo(dataBuff))
			{
				buffSize = crypto->Encrypt(dataBuff, buffSize, dataBuff);
				me->SaveOutput(buff, buffSize + (UOSInt)(dataBuff - buff));
			}
			MemFreeArr(buff);
		}
		crypto.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIREncryptFileForm::OnDecryptClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREncryptFileForm> me = userObj.GetNN<SSWR::AVIRead::AVIREncryptFileForm>();
	NN<Crypto::Encrypt::ICrypto> crypto;
	if (me->InitCrypto().SetTo(crypto))
	{
		UOSInt buffSize;
		UnsafeArray<UInt8> buff;
		if (me->InitInput(crypto->GetDecBlockSize(), buffSize).SetTo(buff))
		{
			UnsafeArray<UInt8> dataBuff;
			if (me->InitIV(crypto, buff, buffSize, crypto->GetDecBlockSize(), false).SetTo(dataBuff))
			{
				buffSize = crypto->Decrypt(dataBuff, buffSize, dataBuff);
				me->SaveOutput(dataBuff, buffSize);
			}
			MemFreeArr(buff);
		}
		crypto.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIREncryptFileForm::OnFilesDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIREncryptFileForm> me = userObj.GetNN<SSWR::AVIRead::AVIREncryptFileForm>();
	if (files.GetCount() > 0)
	{
		me->SetInputFile(files.GetItem(0)->ToCString());
	}
}

NN<Text::TextBinEnc::ITextBinEnc> SSWR::AVIRead::AVIREncryptFileForm::GetTextEncType(NN<UI::GUIComboBox> cbo)
{
	NN<Text::TextBinEnc::ITextBinEnc> enc;
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
			
void SSWR::AVIRead::AVIREncryptFileForm::AddTextEncType(NN<UI::GUIComboBox> cbo)
{
	cbo->AddItem(CSTR("UTF-8"), 0);
	cbo->AddItem(CSTR("Hex"), 0);
	cbo->AddItem(CSTR("Base64"), 0);
	cbo->SetSelectedIndex(0);
}

SSWR::AVIRead::AVIREncryptFileForm::AVIREncryptFileForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 256, ui)
{
	this->SetText(CSTR("Encrypt File"));
	this->SetFont(0, 0, 8.25, false);

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
	CBOADDENUM(this->cboChainMode, Crypto::Encrypt::ChainMode, CTR);
	this->cboChainMode->SetSelectedIndex(0);
	this->lblChainMode = ui->NewLabel(*this, CSTR("Padding Mode"));
	this->lblChainMode->SetRect(4, 124, 100, 23, false);
	this->cboPaddingMode = ui->NewComboBox(*this, false);
	this->cboPaddingMode->SetRect(104, 124, 100, 23, false);
	CBOADDENUM(this->cboPaddingMode, Crypto::Encrypt::PaddingMode, None);
	CBOADDENUM(this->cboPaddingMode, Crypto::Encrypt::PaddingMode, PKCS7);
	this->cboPaddingMode->SetSelectedIndex(1);
	this->lblInputFile = ui->NewLabel(*this, CSTR("Input File"));
	this->lblInputFile->SetRect(4, 148, 100, 23, false);
	this->txtInputFile = ui->NewTextBox(*this, CSTR(""), true);
	this->txtInputFile->SetRect(104, 148, 600, 23, false);
	this->lblOutputFile = ui->NewLabel(*this, CSTR("Output File"));
	this->lblOutputFile->SetRect(4, 172, 100, 23, false);
	this->txtOutputFile = ui->NewTextBox(*this, CSTR(""), true);
	this->txtOutputFile->SetRect(104, 172, 600, 23, false);
	this->btnEncrypt = ui->NewButton(*this, CSTR("Encrypt"));
	this->btnEncrypt->SetRect(104, 196, 75, 23, false);
	this->btnEncrypt->HandleButtonClick(OnEncryptClicked, this);
	this->btnDecrypt = ui->NewButton(*this, CSTR("Decrypt"));
	this->btnDecrypt->SetRect(184, 196, 75, 23, false);
	this->btnDecrypt->HandleButtonClick(OnDecryptClicked, this);

	this->HandleDropFiles(OnFilesDrop, this);
}

SSWR::AVIRead::AVIREncryptFileForm::~AVIREncryptFileForm()
{
}

void SSWR::AVIRead::AVIREncryptFileForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
