#include "Stdafx.h"
#include "Crypto/Cert/X509FileList.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Crypto/Cert/X509PubKey.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/FileAnalyse/ASN1FileAnalyse.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "Net/ASN1Util.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRASN1DataForm.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/HexTextBinEnc.h"
#include "UI/GUIComboBoxUtil.h"

#include <stdio.h>

enum MenuItem
{
	MNU_SAVE = 100,
	MNU_VIEW_HEX = 101,
	MNU_CERT_0 = 500,
	MNU_KEY_CREATE = 600,
	MNU_CERT_EXT_KEY = 601
};

UOSInt SSWR::AVIRead::AVIRASN1DataForm::AddHash(NotNullPtr<UI::GUIComboBox> cbo, Crypto::Hash::HashType hashType, Crypto::Hash::HashType targetType)
{
	UOSInt i = cbo->AddItem(Crypto::Hash::HashTypeGetName(hashType), (void*)(OSInt)hashType);
	if (hashType == targetType)
		cbo->SetSelectedIndex(i);
	return i;
}

void SSWR::AVIRead::AVIRASN1DataForm::AddHashTypes(NotNullPtr<UI::GUIComboBox> cbo, Crypto::Hash::HashType hashType)
{
	AddHash(cbo, Crypto::Hash::HashType::MD5, hashType);
	AddHash(cbo, Crypto::Hash::HashType::SHA1, hashType);
	AddHash(cbo, Crypto::Hash::HashType::SHA224, hashType);
	AddHash(cbo, Crypto::Hash::HashType::SHA256, hashType);
	AddHash(cbo, Crypto::Hash::HashType::SHA384, hashType);
	AddHash(cbo, Crypto::Hash::HashType::SHA512, hashType);
}

Bool SSWR::AVIRead::AVIRASN1DataForm::FileIsSign(NotNullPtr<Text::String> fileName)
{
	UInt8 fileCont[172];
	UInt8 decCont[256];
	UInt64 fileLen;
	UOSInt decSize;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	fileLen = fs.GetLength();
	if (fileLen >= 172 && fileLen <= 174)
	{
		if (fs.Read(Data::ByteArray(fileCont, (UOSInt)fileLen)) == (UOSInt)fileLen)
		{
			Text::TextBinEnc::Base64Enc enc;
			decSize = enc.DecodeBin(fileCont, (UOSInt)fileLen, decCont);
			if (decSize == 128)
			{
				return true;
			}
		}
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRASN1DataForm::OnVerifyClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRASN1DataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRASN1DataForm>();
	UInt8 signBuff[256];
	UOSInt signLen = 128;
	Text::StringBuilderUTF8 sb;
	me->txtVerifySignature->GetText(sb);
	signLen = me->ParseSignature(&sb, signBuff);
	if (signLen == 0)
	{
		return;
	}

	sb.ClearStr();
	me->txtVerifyPayloadFile->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter Payload"), CSTR("Verify Signature"), me);
		return;
	}

	IO::FileStream fs(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Payload Invalid"), CSTR("Verify Signature"), me);
		return;
	}
	IO::MemoryStream mstm;
	fs.ReadToEnd(mstm, 65536);
	NotNullPtr<Crypto::Cert::X509Key> key;
	if (!key.Set(me->GetNewKey()))
	{
		me->ui->ShowMsgOK(CSTR("Error in extracting key"), CSTR("Verify Signature"), me);
		return;
	}
	NotNullPtr<Net::SSLEngine> ssl;
	if (Net::SSLEngineFactory::Create(me->core->GetSocketFactory(), true).SetTo(ssl))
	{
		if (ssl->SignatureVerify(key, (Crypto::Hash::HashType)me->cboVerifyHash->GetSelectedItem().GetOSInt(), mstm.GetBuff(), (UOSInt)mstm.GetLength(), signBuff, signLen))
		{
			me->txtVerifyStatus->SetText(CSTR("Valid"));
		}
		else
		{
			me->txtVerifyStatus->SetText(CSTR("Invalid"));
		}
		ssl.Delete();
	}
	else
	{
		me->txtVerifyStatus->SetText(CSTR("SSL Engine Error"));
	}
	key.Delete();
}

void __stdcall SSWR::AVIRead::AVIRASN1DataForm::OnVerifySignInfoClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRASN1DataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRASN1DataForm>();
	UInt8 signBuff[384];
	UOSInt signLen = 128;
	UInt8 decBuff[256];
	UOSInt decLen = 0;
	Text::StringBuilderUTF8 sb;
	me->txtVerifySignature->GetText(sb);
	signLen = me->ParseSignature(&sb, signBuff);
	if (signLen == 0)
	{
		return;
	}
	NotNullPtr<Crypto::Cert::X509Key> key;
	if (!key.Set(me->GetNewKey()))
	{
		me->ui->ShowMsgOK(CSTR("Error in extracting key"), CSTR("Verify Signature"), me);
		return;
	}
	NotNullPtr<Net::SSLEngine> ssl;
	if (Net::SSLEngineFactory::Create(me->core->GetSocketFactory(), true).SetTo(ssl))
	{
		decLen = ssl->Decrypt(key, decBuff, signBuff, signLen, Crypto::Encrypt::RSACipher::Padding::PKCS1);
		if (decLen > 0)
		{
			Crypto::Cert::DigestInfo digestInfo;
			if (Crypto::Cert::X509File::ParseDigestType(&digestInfo, decBuff, decBuff + decLen))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Hash Type: "));
				sb.Append(Crypto::Hash::HashTypeGetName(digestInfo.hashType));
				sb.AppendC(UTF8STRC("\r\nHash Value: "));
				sb.AppendHexBuff(digestInfo.hashVal, digestInfo.hashLen, ' ', Text::LineBreakType::None);
				me->txtVerifyStatus->SetText(sb.ToCString());
			}
			else
			{
				me->txtVerifyStatus->SetText(CSTR("Signature is not a valid DigestInfo"));
			}
		}
		else
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Cannot decrypt signature, sign len = "));
			sb.AppendUOSInt(signLen);
			me->txtVerifyStatus->SetText(sb.ToCString());
		}
		ssl.Delete();
	}
	else
	{
		me->txtVerifyStatus->SetText(CSTR("SSL Engine problem"));
	}
	key.Delete();
}

void __stdcall SSWR::AVIRead::AVIRASN1DataForm::OnEncryptEncryptClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRASN1DataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRASN1DataForm>();
	Text::StringBuilderUTF8 sb;
	me->txtEncryptInput->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Input is empty"), CSTR("Encrypt"), me);
		return;
	}
	UInt8 *buff;
	UOSInt buffSize;
	UOSInt type = me->cboEncryptInputType->GetSelectedIndex();
	if (type == 0)
	{
		Text::TextBinEnc::Base64Enc enc;
		buffSize = enc.CalcBinSize(sb.ToString(), sb.GetLength());
		buff = MemAlloc(UInt8, buffSize);
		enc.DecodeBin(sb.ToString(), sb.GetLength(), buff);
	}
	else if (type == 1)
	{
		Text::TextBinEnc::HexTextBinEnc enc;
		buffSize = enc.CalcBinSize(sb.ToString(), sb.GetLength());
		buff = MemAlloc(UInt8, buffSize);
		enc.DecodeBin(sb.ToString(), sb.GetLength(), buff);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Unknown Input Type"), CSTR("Encrypt"), me);
		return;
	}
	if (buffSize == 0)
	{
		MemFree(buff);
		me->ui->ShowMsgOK(CSTR("Binary value is empty"), CSTR("Encrypt"), me);
		return;
	}
	NotNullPtr<Crypto::Cert::X509Key> key;
	if (!key.Set(me->GetNewKey()))
	{
		MemFree(buff);
		me->ui->ShowMsgOK(CSTR("Error in getting key from file"), CSTR("Encrypt"), me);
		return;
	}
	NotNullPtr<Net::SSLEngine> ssl;
	if (Net::SSLEngineFactory::Create(me->core->GetSocketFactory(), true).SetTo(ssl))
	{
		UInt8 *outData = MemAlloc(UInt8, 512);
		UOSInt outSize = ssl->Encrypt(key, outData, buff, buffSize, (Crypto::Encrypt::RSACipher::Padding)me->cboEncryptRSAPadding->GetSelectedItem().GetOSInt());
		MemFree(buff);
		ssl.Delete();
		key.Delete();
		if (outSize == 0)
		{
			MemFree(outData);
			me->ui->ShowMsgOK(CSTR("Error in encrypting data"), CSTR("Encrypt"), me);
			return;
		}
		type = me->cboEncryptOutputType->GetSelectedIndex();
		sb.ClearStr();
		if (type == 0)
		{
			Text::TextBinEnc::Base64Enc enc;
			enc.EncodeBin(sb, outData, outSize);
			me->txtEncryptOutput->SetText(sb.ToCString());
		}
		else if (type == 1)
		{
			Text::TextBinEnc::HexTextBinEnc enc;
			enc.EncodeBin(sb, outData, outSize);
			me->txtEncryptOutput->SetText(sb.ToCString());
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Unknown Output Type"), CSTR("Encrypt"), me);
		}
		MemFree(outData);
	}
	else
	{
		key.Delete();
		MemFree(buff);
		me->ui->ShowMsgOK(CSTR("Error in SSL engine"), CSTR("Encrypt"), me);
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRASN1DataForm::OnEncryptDecryptClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRASN1DataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRASN1DataForm>();
	Text::StringBuilderUTF8 sb;
	me->txtEncryptInput->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Input is empty"), CSTR("Decrypt"), me);
		return;
	}
	UInt8 *buff;
	UOSInt buffSize;
	UOSInt type = me->cboEncryptInputType->GetSelectedIndex();
	if (type == 0)
	{
		Text::TextBinEnc::Base64Enc enc;
		buffSize = enc.CalcBinSize(sb.ToString(), sb.GetLength());
		buff = MemAlloc(UInt8, buffSize);
		enc.DecodeBin(sb.ToString(), sb.GetLength(), buff);
	}
	else if (type == 1)
	{
		Text::TextBinEnc::HexTextBinEnc enc;
		buffSize = enc.CalcBinSize(sb.ToString(), sb.GetLength());
		buff = MemAlloc(UInt8, buffSize);
		enc.DecodeBin(sb.ToString(), sb.GetLength(), buff);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Unknown Input Type"), CSTR("Decrypt"), me);
		return;
	}
	if (buffSize == 0)
	{
		MemFree(buff);
		me->ui->ShowMsgOK(CSTR("Binary value is empty"), CSTR("Decrypt"), me);
		return;
	}
	NotNullPtr<Crypto::Cert::X509Key> key;
	if (!key.Set(me->GetNewKey()))
	{
		MemFree(buff);
		me->ui->ShowMsgOK(CSTR("Error in getting key from file"), CSTR("Decrypt"), me);
		return;
	}
	NotNullPtr<Net::SSLEngine> ssl;
	if (Net::SSLEngineFactory::Create(me->core->GetSocketFactory(), true).SetTo(ssl))
	{
		UInt8 *outData = MemAlloc(UInt8, 512);
		UOSInt outSize = ssl->Decrypt(key, outData, buff, buffSize, (Crypto::Encrypt::RSACipher::Padding)me->cboEncryptRSAPadding->GetSelectedItem().GetOSInt());
		MemFree(buff);
		key.Delete();
		ssl.Delete();
		if (outSize == 0)
		{
			MemFree(outData);
			me->ui->ShowMsgOK(CSTR("Error in decrypting data"), CSTR("Decrypt"), me);
			return;
		}
		type = me->cboEncryptOutputType->GetSelectedIndex();
		sb.ClearStr();
		if (type == 0)
		{
			Text::TextBinEnc::Base64Enc enc;
			enc.EncodeBin(sb, outData, outSize);
			me->txtEncryptOutput->SetText(sb.ToCString());
		}
		else if (type == 1)
		{
			Text::TextBinEnc::HexTextBinEnc enc;
			enc.EncodeBin(sb, outData, outSize);
			me->txtEncryptOutput->SetText(sb.ToCString());
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Unknown Output Type"), CSTR("Decrypt"), me);
		}
		MemFree(outData);
	}
	else
	{
		MemFree(buff);
		key.Delete();
		me->ui->ShowMsgOK(CSTR("Error in SSL engine"), CSTR("Encrypt"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRASN1DataForm::OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files)
{
	NotNullPtr<SSWR::AVIRead::AVIRASN1DataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRASN1DataForm>();
	NotNullPtr<UI::GUITabPage> tp;
	UOSInt i;
	UOSInt nFiles = files.GetCount();
	Bool isSign;
	if (!me->tcMain->GetSelectedPage().SetTo(tp))
	{
		return;
	}
	if (tp == me->tpVerify)
	{
		i = 0;
		while (i < nFiles)
		{
			isSign = FileIsSign(files[i]);
			if (isSign)
			{
				me->txtVerifySignature->SetText(files[i]->ToCString());
			}
			else
			{
				me->txtVerifyPayloadFile->SetText(files[i]->ToCString());
			}
			i++;
		}
	}
//	else if (tp == me->tpSignature)
//	{
		//me->txtSignaturePayloadFile->SetText(files[0]->ToCString());
//	}
}

UOSInt SSWR::AVIRead::AVIRASN1DataForm::ParseSignature(Text::PString *s, UInt8 *signBuff)
{
	UInt8 fileCont[346];
	UOSInt signLen = 0;
	if (s->leng == 0)
	{
		this->ui->ShowMsgOK(CSTR("Please enter Signature"), CSTR("Verify Signature"), this);
		return 0;
	}
	if (IO::Path::GetPathType(s->ToCString()) == IO::Path::PathType::File)
	{
		IO::FileStream fs(s->ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		UInt64 fileLen = fs.GetLength();
		Bool succ = false;
		if (fileLen >= 172 && fileLen <= 174)
		{
			if (fs.Read(Data::ByteArray(fileCont, (UOSInt)fileLen)) == fileLen)
			{
				Text::TextBinEnc::Base64Enc enc;
				signLen = enc.DecodeBin(fileCont, (UOSInt)fileLen, signBuff);
				if (signLen == 128)
				{
					succ = true;
				}
			}
		}
		else if (fileLen >= 344 && fileLen <= 346)
		{
			if (fs.Read(Data::ByteArray(fileCont, (UOSInt)fileLen)) == fileLen)
			{
				Text::TextBinEnc::Base64Enc enc;
				signLen = enc.DecodeBin(fileCont, (UOSInt)fileLen, signBuff);
				if (signLen == 256)
				{
					succ = true;
				}
			}
		}
		if (!succ)
		{
			this->ui->ShowMsgOK(CSTR("Please enter valid Signature (Invalid file format)"), CSTR("Verify Signature"), this);
			return 0;
		}
	}
	else if (s->leng == 172)
	{
		Text::TextBinEnc::Base64Enc enc;
		signLen = enc.DecodeBin(s->v, 172, signBuff);
		if (signLen != 128)
		{
			this->ui->ShowMsgOK(CSTR("Please enter valid Signature (Invalid base64 format)"), CSTR("Verify Signature"), this);
			return 0;
		}
	}
	else if (s->leng == 256)
	{
		if (Text::StrHex2Bytes(s->v, signBuff) != 128)
		{
			this->ui->ShowMsgOK(CSTR("Please enter valid Signature (Invalid hex format)"), CSTR("Verify Signature"), this);
			return 0;
		}
		signLen = 128;
	}
	else if (s->leng == 344)
	{
		Text::TextBinEnc::Base64Enc enc;
		signLen = enc.DecodeBin(s->v, 344, signBuff);
		if (signLen != 256)
		{
			this->ui->ShowMsgOK(CSTR("Please enter valid Signature (Invalid base64 format2)"), CSTR("Verify Signature"), this);
			return 0;
		}
	}
	else if (s->leng == 767)
	{
		if (Text::StrHex2BytesS(s->v, signBuff, s->v[2]) != 256)
		{
			this->ui->ShowMsgOK(CSTR("Please enter valid Signature (Invalid hex formats)"), CSTR("Verify Signature"), this);
			return 0;
		}
		signLen = 256;
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(CSTR("Please enter valid Signature (Unknown format), length = "));
		sb.AppendUOSInt(s->leng);
		this->ui->ShowMsgOK(sb.ToCString(), CSTR("Verify Signature"), this);
		return 0;
	}
	return signLen;
}

Crypto::Cert::X509Key *SSWR::AVIRead::AVIRASN1DataForm::GetNewKey()
{
	NotNullPtr<Crypto::Cert::X509File> x509 = NotNullPtr<Crypto::Cert::X509File>::ConvertFrom(this->asn1);
	switch (x509->GetFileType())
	{
	case Crypto::Cert::X509File::FileType::FileList:
		return ((Crypto::Cert::X509Cert*)NotNullPtr<Crypto::Cert::X509FileList>::ConvertFrom(x509)->GetFile(0).OrNull())->GetNewPublicKey();
	case Crypto::Cert::X509File::FileType::Cert:
		return NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(x509)->GetNewPublicKey();
	case Crypto::Cert::X509File::FileType::Key:
		return (Crypto::Cert::X509Key*)x509->Clone().Ptr();
	case Crypto::Cert::X509File::FileType::PrivateKey:
		return NotNullPtr<Crypto::Cert::X509PrivKey>::ConvertFrom(x509)->CreateKey();
	case Crypto::Cert::X509File::FileType::PublicKey:
		return NotNullPtr<Crypto::Cert::X509PubKey>::ConvertFrom(x509)->CreateKey();
	case Crypto::Cert::X509File::FileType::CertRequest:
	case Crypto::Cert::X509File::FileType::PKCS7:
	case Crypto::Cert::X509File::FileType::PKCS12:
	case Crypto::Cert::X509File::FileType::CRL:
	default:
		return 0;
	}
}

SSWR::AVIRead::AVIRASN1DataForm::AVIRASN1DataForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Net::ASN1Data> asn1) : UI::GUIForm(parent, 1024, 768, ui)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("ASN1 Data - "));
	sb.Append(asn1->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->asn1 = asn1;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NotNullPtr<UI::GUIMenu> mnu;
	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("Save"), MNU_SAVE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu->AddItem(CSTR("View Hex"), MNU_VIEW_HEX, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_H);
	if (this->asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
	{
		NotNullPtr<Crypto::Cert::X509File> x509 = NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(this->asn1);
		Text::StringBuilderUTF8 sb;
		NotNullPtr<UI::GUIMenu> mnu2;
		UOSInt i;
		UOSInt j;
		mnu2 = mnu->AddSubMenu(CSTR("Certs"));
		i = 0;
		j = x509->GetCertCount();
		if (j == 0)
		{
			mnu2->AddItem(CSTR("None"), MNU_CERT_0, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
		}
		else
		{
			while (i < j)
			{
				sb.ClearStr();
				if (!x509->GetCertName(i, sb))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Cert "));
					sb.AppendUOSInt(i);
				}
				mnu2->AddItem(sb.ToCString(), (UInt16)(MNU_CERT_0 + i), UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
				i++;
			}
		}
		if (x509->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey)
		{
			mnu2 = mnu->AddSubMenu(CSTR("Private Key"));
			mnu2->AddItem(CSTR("Create Key"), MNU_KEY_CREATE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
		}
		else if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
		{
			mnu2 = mnu->AddSubMenu(CSTR("Key"));
			mnu2->AddItem(CSTR("Extract Key"), MNU_CERT_EXT_KEY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
		}
	}
	this->SetMenu(this->mnuMain);

	this->pnlStatus = ui->NewPanel(*this);
	this->pnlStatus->SetRect(0, 0, 100, 31, false);
	this->pnlStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lblStatus = ui->NewLabel(this->pnlStatus, CSTR("Valid Status"));
	this->lblStatus->SetRect(4, 4, 100, 23, false);
	this->txtStatus = ui->NewTextBox(this->pnlStatus, CSTR(""));
	this->txtStatus->SetRect(104, 4, 200, 23, false);
	this->txtStatus->SetReadOnly(true);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpDesc = this->tcMain->AddTabPage(CSTR("Desc"));
	this->txtDesc = ui->NewTextBox(this->tpDesc, CSTR(""), true);
	this->txtDesc->SetReadOnly(true);
	this->txtDesc->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpASN1 = this->tcMain->AddTabPage(CSTR("ASN1"));
	this->txtASN1 = ui->NewTextBox(this->tpASN1, CSTR(""), true);
	this->txtASN1->SetReadOnly(true);
	this->txtASN1->SetDockType(UI::GUIControl::DOCK_FILL);

	sb.ClearStr();
	this->asn1->ToString(sb);
	this->txtDesc->SetText(sb.ToCString());
	sb.ClearStr();
	this->asn1->ToASN1String(sb);
	this->txtASN1->SetText(sb.ToCString());

	if (this->asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
	{
		NotNullPtr<Crypto::Cert::X509File> x509 = NotNullPtr<Crypto::Cert::X509File>::ConvertFrom(this->asn1);
		NotNullPtr<Net::SSLEngine> ssl;
		if (Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), false).SetTo(ssl))
		{
			this->txtStatus->SetText(Crypto::Cert::X509File::ValidStatusGetDesc(x509->IsValid(ssl, ssl->GetTrustStore())));
			ssl.Delete();
		}
		else
		{
			this->txtStatus->SetText(CSTR("Error in SSL Engine"));
		}

		Bool canSignature = false;
		Bool canVerify = false;
		Bool hasPrivKey = false;
		Bool hasPubKey = false;
		Crypto::Hash::HashType hashType = Crypto::Hash::HashType::SHA256;
		switch (x509->GetFileType())
		{
		case Crypto::Cert::X509File::FileType::FileList:
		{
			NotNullPtr<Crypto::Cert::X509FileList> fileList = NotNullPtr<Crypto::Cert::X509FileList>::ConvertFrom(x509);
			canVerify = true;
			hasPubKey = true;
			sb.ClearStr();
			Crypto::Cert::X509Cert *cert = (Crypto::Cert::X509Cert*)fileList->GetFile(0).OrNull();
			if (cert->GetSubjectCN(sb))
			{
				this->tcMain->SetTabPageName(1, sb.ToCString());
			}
			Crypto::Cert::X509File::SignedInfo signedInfo;
			if (cert->GetSignedInfo(signedInfo))
			{
				hashType = Crypto::Cert::X509File::GetAlgHash(signedInfo.algType);
			}
			NotNullPtr<UI::GUITabPage> tp;
			NotNullPtr<UI::GUITextBox> txt;
			NotNullPtr<Crypto::Cert::X509File> file;
			UOSInt i = 1;
			UOSInt j = fileList->GetFileCount();
			while (i < j)
			{
				if (fileList->GetFile(i).SetTo(file))
				{
					sb.ClearStr();
					file->ToShortName(sb);
					tp = this->tcMain->AddTabPage(sb.ToCString());
					sb.ClearStr();
					file->ToASN1String(sb);
					txt = ui->NewTextBox(tp, sb.ToCString(), true);
					txt->SetDockType(UI::GUIControl::DOCK_FILL);
				}
				i++;
			}
			break;
		}
		case Crypto::Cert::X509File::FileType::Cert:
		{
			NotNullPtr<Crypto::Cert::X509Cert> cert = NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(x509);
			canVerify = true;
			hasPubKey = true;
			Crypto::Cert::X509File::SignedInfo signedInfo;
			if (cert->GetSignedInfo(signedInfo))
			{
				hashType = Crypto::Cert::X509File::GetAlgHash(signedInfo.algType);
			}
			break;
		}
		case Crypto::Cert::X509File::FileType::Key:
			canVerify = true;
			canSignature = true;
			hasPubKey = true;
			if (NotNullPtr<Crypto::Cert::X509Key>::ConvertFrom(x509)->IsPrivateKey())
			{
				hasPrivKey = true;
			}
			break;
		case Crypto::Cert::X509File::FileType::PrivateKey:
			canVerify = true;
			canSignature = true;
			hasPrivKey = true;
			break;
		case Crypto::Cert::X509File::FileType::PublicKey:
			canVerify = true;
			hasPubKey = true;
			break;
		case Crypto::Cert::X509File::FileType::CertRequest:
		case Crypto::Cert::X509File::FileType::PKCS7:
		case Crypto::Cert::X509File::FileType::PKCS12:
		case Crypto::Cert::X509File::FileType::CRL:
			break;
		}

		if (canVerify)
		{
			this->tpVerify = this->tcMain->AddTabPage(CSTR("Verify"));
			this->lblVerifyHash = ui->NewLabel(this->tpVerify, CSTR("Hash Type"));
			this->lblVerifyHash->SetRect(4, 4, 100, 23, false);
			this->cboVerifyHash = ui->NewComboBox(this->tpVerify, false);
			this->cboVerifyHash->SetRect(104, 4, 200, 23, false);
			AddHashTypes(this->cboVerifyHash, hashType);
			this->lblVerifyPayloadFile = ui->NewLabel(this->tpVerify, CSTR("Payload File"));
			this->lblVerifyPayloadFile->SetRect(4, 28, 100, 23, false);
			this->txtVerifyPayloadFile = ui->NewTextBox(this->tpVerify, CSTR(""));
			this->txtVerifyPayloadFile->SetRect(104, 28, 300, 23, false);
			this->lblVerifySignature = ui->NewLabel(this->tpVerify, CSTR("Signature"));
			this->lblVerifySignature->SetRect(4, 52, 100, 23, false);
			this->txtVerifySignature = ui->NewTextBox(this->tpVerify, CSTR(""));
			this->txtVerifySignature->SetRect(104, 52, 300, 23, false);
			this->btnVerify = ui->NewButton(this->tpVerify, CSTR("Verify"));
			this->btnVerify->SetRect(104, 76, 75, 23, false);
			this->btnVerify->HandleButtonClick(OnVerifyClicked, this);
			this->btnVerifySignInfo = ui->NewButton(this->tpVerify, CSTR("Sign Info"));
			this->btnVerifySignInfo->SetRect(184, 76, 75, 23, false);
			this->btnVerifySignInfo->HandleButtonClick(OnVerifySignInfoClicked, this);
			this->lblVerifyStatus = ui->NewLabel(this->tpVerify, CSTR("Status"));
			this->lblVerifyStatus->SetRect(4, 100, 100, 23, false);
			this->txtVerifyStatus = ui->NewTextBox(this->tpVerify, CSTR(""), true);
			this->txtVerifyStatus->SetRect(104, 100, 600, 47, false);
			this->txtVerifyStatus->SetReadOnly(true);
		}
		//this->tpSignature = 0;
		if (canSignature)
		{
/*
			NotNullPtr<UI::GUITabPage> tpSignature;
			UI::GUILabel *lblSignatureHash;
			UI::GUIComboBox *cboSignatureHash;
			UI::GUILabel *lblSignaturePayloadFile;
			UI::GUITextBox *txtSignaturePayloadFile;
			UI::GUIButton *btnSignature;
			UI::GUILabel *lblSiguatureValue;
			UI::GUITextBox *txtSignatureValue;*/
		}
		if (hasPrivKey || hasPubKey)
		{
			this->tpEncrypt = this->tcMain->AddTabPage(CSTR("Encrypt"));
			this->lblEncryptInputType = ui->NewLabel(this->tpEncrypt, CSTR("Input Type"));
			this->lblEncryptInputType->SetRect(4, 4, 100, 23, false);
			this->cboEncryptInputType = ui->NewComboBox(this->tpEncrypt, false);
			this->cboEncryptInputType->SetRect(104, 4, 100, 23, false);
			this->cboEncryptInputType->AddItem(CSTR("Base64"), 0);
			this->cboEncryptInputType->AddItem(CSTR("Hex"), 0);
			this->cboEncryptInputType->SetSelectedIndex(0);
			this->lblEncryptOutputType = ui->NewLabel(this->tpEncrypt, CSTR("Output Type"));
			this->lblEncryptOutputType->SetRect(4, 28, 100, 23, false);
			this->cboEncryptOutputType = ui->NewComboBox(this->tpEncrypt, false);
			this->cboEncryptOutputType->SetRect(104, 28, 100, 23, false);
			this->cboEncryptOutputType->AddItem(CSTR("Base64"), 0);
			this->cboEncryptOutputType->AddItem(CSTR("Hex"), 0);
			this->cboEncryptOutputType->SetSelectedIndex(0);
			this->lblEncryptRSAPadding = ui->NewLabel(this->tpEncrypt, CSTR("RSA Padding"));
			this->lblEncryptRSAPadding->SetRect(4, 52, 100, 23, false);
			this->cboEncryptRSAPadding = ui->NewComboBox(this->tpEncrypt, false);
			this->cboEncryptRSAPadding->SetRect(104, 52, 150, 23, false);
			CBOADDENUM(this->cboEncryptRSAPadding, Crypto::Encrypt::RSACipher::Padding, PKCS1);
			CBOADDENUM(this->cboEncryptRSAPadding, Crypto::Encrypt::RSACipher::Padding, NoPadding);
			CBOADDENUM(this->cboEncryptRSAPadding, Crypto::Encrypt::RSACipher::Padding, PKCS1_OAEP);
			CBOADDENUM(this->cboEncryptRSAPadding, Crypto::Encrypt::RSACipher::Padding, X931);
			CBOADDENUM(this->cboEncryptRSAPadding, Crypto::Encrypt::RSACipher::Padding, PKCS1_PSS);
			CBOADDENUM(this->cboEncryptRSAPadding, Crypto::Encrypt::RSACipher::Padding, PKCS1_WithTLS);
			this->cboEncryptRSAPadding->SetSelectedIndex(0);
			this->lblEncryptInput = ui->NewLabel(this->tpEncrypt, CSTR("Input"));
			this->lblEncryptInput->SetRect(4, 76, 100, 23, false);
			this->txtEncryptInput = ui->NewTextBox(this->tpEncrypt, CSTR(""), true);
			this->txtEncryptInput->SetRect(104, 76, 500, 95, false);
			this->btnEncryptEncrypt = ui->NewButton(this->tpEncrypt, CSTR("Encrypt"));
			this->btnEncryptEncrypt->SetRect(104, 172, 75, 23, false);
			this->btnEncryptEncrypt->HandleButtonClick(OnEncryptEncryptClicked, this);
			if (hasPrivKey)
			{
				this->btnEncryptDecrypt = ui->NewButton(this->tpEncrypt, CSTR("Decrypt"));
				this->btnEncryptDecrypt->SetRect(184, 172, 75, 23, false);
				this->btnEncryptDecrypt->HandleButtonClick(OnEncryptDecryptClicked, this);
			}
			this->lblEncryptOutput = ui->NewLabel(this->tpEncrypt, CSTR("Output"));
			this->lblEncryptOutput->SetRect(4, 196, 100, 23, false);
			this->txtEncryptOutput = ui->NewTextBox(this->tpEncrypt, CSTR(""), true);
			this->txtEncryptOutput->SetRect(104, 196, 500, 95, false);
			this->txtEncryptOutput->SetReadOnly(true);
		}
		this->HandleDropFiles(OnFileDrop, this);
	}
	else
	{
		this->txtStatus->SetText(CSTR("-"));
	}
}

SSWR::AVIRead::AVIRASN1DataForm::~AVIRASN1DataForm()
{
	this->asn1.Delete();
}

void SSWR::AVIRead::AVIRASN1DataForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRASN1DataForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_SAVE:
		this->core->SaveData(this, this->asn1, L"ASN1Data");
		break;
	case MNU_VIEW_HEX:
	{
		IO::StmData::MemoryDataCopy md(this->asn1->GetASN1Buff(), this->asn1->GetASN1BuffSize());
		IO::FileAnalyse::ASN1FileAnalyse *analyse;
		NEW_CLASS(analyse, IO::FileAnalyse::ASN1FileAnalyse(md, this->asn1->CreateNames()));
		this->core->OpenHex(md, analyse);
		break;
	}
	case MNU_KEY_CREATE:
		{
			NotNullPtr<Crypto::Cert::X509Key> key;
			if (key.Set(NotNullPtr<Crypto::Cert::X509PrivKey>::ConvertFrom(this->asn1)->CreateKey()))
			{
				this->core->OpenObject(key);
			}
		}
	case MNU_CERT_EXT_KEY:
		{
			NotNullPtr<Crypto::Cert::X509Key> key;
			if (key.Set(NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(this->asn1)->GetNewPublicKey()))
			{
				this->core->OpenObject(key);
			}
		}
	default:
		if (cmdId >= MNU_CERT_0)
		{
			NotNullPtr<Crypto::Cert::X509Cert> cert;
			if (cert.Set(NotNullPtr<Crypto::Cert::X509File>::ConvertFrom(this->asn1)->GetNewCert((UOSInt)(cmdId - MNU_CERT_0))))
			{
				this->core->OpenObject(cert);
			}
		}
		break;
	}
}
