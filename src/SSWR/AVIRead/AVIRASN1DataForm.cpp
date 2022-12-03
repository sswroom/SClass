#include "Stdafx.h"
#include "Crypto/Cert/X509FileList.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Crypto/Cert/X509PubKey.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Net/ASN1Util.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRASN1DataForm.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "UI/MessageDialog.h"

#include <stdio.h>

enum MenuItem
{
	MNU_SAVE = 100,
	MNU_CERT_0 = 500,
	MNU_KEY_CREATE = 600,
	MNU_CERT_EXT_KEY = 601
};

UOSInt SSWR::AVIRead::AVIRASN1DataForm::AddHash(UI::GUIComboBox *cbo, Crypto::Hash::HashType hashType, Crypto::Hash::HashType targetType)
{
	UOSInt i = cbo->AddItem(Crypto::Hash::HashTypeGetName(hashType), (void*)(OSInt)hashType);
	if (hashType == targetType)
		cbo->SetSelectedIndex(i);
	return i;
}

void SSWR::AVIRead::AVIRASN1DataForm::AddHashTypes(UI::GUIComboBox *cbo, Crypto::Hash::HashType hashType)
{
	AddHash(cbo, Crypto::Hash::HashType::MD5, hashType);
	AddHash(cbo, Crypto::Hash::HashType::SHA1, hashType);
	AddHash(cbo, Crypto::Hash::HashType::SHA224, hashType);
	AddHash(cbo, Crypto::Hash::HashType::SHA256, hashType);
	AddHash(cbo, Crypto::Hash::HashType::SHA384, hashType);
	AddHash(cbo, Crypto::Hash::HashType::SHA512, hashType);
}

Bool SSWR::AVIRead::AVIRASN1DataForm::FileIsSign(Text::String *fileName)
{
	UInt8 fileCont[172];
	UInt8 decCont[256];
	UInt64 fileLen;
	UOSInt decSize;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	fileLen = fs.GetLength();
	if (fileLen >= 172 && fileLen <= 174)
	{
		if (fs.Read(fileCont, (UOSInt)fileLen) == (UOSInt)fileLen)
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

void __stdcall SSWR::AVIRead::AVIRASN1DataForm::OnVerifyClicked(void *userObj)
{
	SSWR::AVIRead::AVIRASN1DataForm *me = (SSWR::AVIRead::AVIRASN1DataForm*)userObj;
	UInt8 signBuff[256];
	UOSInt signLen = 128;
	Text::StringBuilderUTF8 sb;
	me->txtVerifySignature->GetText(&sb);
	signLen = me->ParseSignature(&sb, signBuff);
	if (signLen == 0)
	{
		return;
	}

	sb.ClearStr();
	me->txtVerifyPayloadFile->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter Payload"), CSTR("Verify Signature"), me);
		return;
	}

	IO::FileStream fs(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Payload Invalid"), CSTR("Verify Signature"), me);
		return;
	}
	IO::MemoryStream mstm(UTF8STRC("SSWR.AVIRead.AVIRASN1DataForm.OnVerifyClicked.mstm"));
	fs.ReadToEnd(&mstm, 65536);
	Crypto::Cert::X509Key *key = me->GetNewKey();
	if (key == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in extracting key"), CSTR("Verify Signature"), me);
		return;
	}
	UOSInt buffSize;
	Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(me->core->GetSocketFactory(), true);
	if (ssl->SignatureVerify(key, (Crypto::Hash::HashType)(OSInt)me->cboVerifyHash->GetSelectedItem(), mstm.GetBuff(&buffSize), (UOSInt)mstm.GetLength(), signBuff, signLen))
	{
		me->txtVerifyStatus->SetText(CSTR("Valid"));
	}
	else
	{
		me->txtVerifyStatus->SetText(CSTR("Invalid"));
	}
	DEL_CLASS(ssl);
	DEL_CLASS(key);
}

void __stdcall SSWR::AVIRead::AVIRASN1DataForm::OnVerifySignInfoClicked(void *userObj)
{
	SSWR::AVIRead::AVIRASN1DataForm *me = (SSWR::AVIRead::AVIRASN1DataForm*)userObj;
	UInt8 signBuff[384];
	UOSInt signLen = 128;
	UInt8 decBuff[256];
	UOSInt decLen = 0;
	Text::StringBuilderUTF8 sb;
	me->txtVerifySignature->GetText(&sb);
	signLen = me->ParseSignature(&sb, signBuff);
	if (signLen == 0)
	{
		return;
	}
	Crypto::Cert::X509Key *key = me->GetNewKey();
	if (key == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in extracting key"), CSTR("Verify Signature"), me);
		return;
	}
	Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(me->core->GetSocketFactory(), true);
	decLen = ssl->Decrypt(key, decBuff, signBuff, signLen);
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
		me->txtVerifyStatus->SetText(CSTR("Cannot decrypt signature"));
	}
	DEL_CLASS(ssl);
	DEL_CLASS(key);
}

void __stdcall SSWR::AVIRead::AVIRASN1DataForm::OnFileDrop(void *userObj, Text::String **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRASN1DataForm *me = (SSWR::AVIRead::AVIRASN1DataForm*)userObj;
	UI::GUITabPage *tp = me->tcMain->GetSelectedPage();
	UOSInt i;
	Bool isSign;
	if (tp == 0)
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
	else if (tp == me->tpSignature)
	{
		me->txtSignaturePayloadFile->SetText(files[0]->ToCString());
	}
}

UOSInt SSWR::AVIRead::AVIRASN1DataForm::ParseSignature(Text::PString *s, UInt8 *signBuff)
{
	UInt8 fileCont[346];
	UOSInt signLen = 0;
	if (s->leng == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter Signature"), CSTR("Verify Signature"), this);
		return 0;
	}
	if (IO::Path::GetPathType(s->ToCString()) == IO::Path::PathType::File)
	{
		IO::FileStream fs(s->ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		UInt64 fileLen = fs.GetLength();
		Bool succ = false;
		if (fileLen >= 172 && fileLen <= 174)
		{
			if (fs.Read(fileCont, (UOSInt)fileLen) == fileLen)
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
			if (fs.Read(fileCont, (UOSInt)fileLen) == fileLen)
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
			UI::MessageDialog::ShowDialog(CSTR("Please enter valid Signature (Invalid file format)"), CSTR("Verify Signature"), this);
			return 0;
		}
	}
	else if (s->leng == 172)
	{
		Text::TextBinEnc::Base64Enc enc;
		signLen = enc.DecodeBin(s->v, 172, signBuff);
		if (signLen != 128)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter valid Signature (Invalid base64 format)"), CSTR("Verify Signature"), this);
			return 0;
		}
	}
	else if (s->leng == 256)
	{
		if (Text::StrHex2Bytes(s->v, signBuff) != 128)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter valid Signature (Invalid hex format)"), CSTR("Verify Signature"), this);
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
			UI::MessageDialog::ShowDialog(CSTR("Please enter valid Signature (Invalid base64 format2)"), CSTR("Verify Signature"), this);
			return 0;
		}
	}
	else if (s->leng == 767)
	{
		if (Text::StrHex2BytesS(s->v, signBuff, s->v[2]) != 256)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter valid Signature (Invalid hex formats)"), CSTR("Verify Signature"), this);
			return 0;
		}
		signLen = 256;
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(CSTR("Please enter valid Signature (Unknown format), length = "));
		sb.AppendUOSInt(s->leng);
		UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Verify Signature"), this);
		return 0;
	}
	return signLen;
}

Crypto::Cert::X509Key *SSWR::AVIRead::AVIRASN1DataForm::GetNewKey()
{
	Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)this->asn1;
	switch (x509->GetFileType())
	{
	case Crypto::Cert::X509File::FileType::FileList:
		return ((Crypto::Cert::X509Cert*)((Crypto::Cert::X509FileList*)x509)->GetFile(0))->GetNewPublicKey();
	case Crypto::Cert::X509File::FileType::Cert:
		return ((Crypto::Cert::X509Cert*)x509)->GetNewPublicKey();
	case Crypto::Cert::X509File::FileType::Key:
		return (Crypto::Cert::X509Key*)x509->Clone();
	case Crypto::Cert::X509File::FileType::PrivateKey:
		return ((Crypto::Cert::X509PrivKey*)x509)->CreateKey();
	case Crypto::Cert::X509File::FileType::PublicKey:
		return ((Crypto::Cert::X509PubKey*)x509)->CreateKey();
	case Crypto::Cert::X509File::FileType::CertRequest:
	case Crypto::Cert::X509File::FileType::PKCS7:
	case Crypto::Cert::X509File::FileType::PKCS12:
	case Crypto::Cert::X509File::FileType::CRL:
	default:
		return 0;
	}
}

SSWR::AVIRead::AVIRASN1DataForm::AVIRASN1DataForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::ASN1Data *asn1) : UI::GUIForm(parent, 1024, 768, ui)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("ASN1 Data - "));
	sb.Append(asn1->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->asn1 = asn1;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	UI::GUIMenu *mnu;
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("Save"), MNU_SAVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	if (this->asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
	{
		Crypto::Cert::X509File *x509 = (Crypto::Cert::X509Cert*)this->asn1;
		Text::StringBuilderUTF8 sb;
		UI::GUIMenu *mnu2;
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
				if (!x509->GetCertName(i, &sb))
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

	NEW_CLASS(this->pnlStatus, UI::GUIPanel(ui, this));
	this->pnlStatus->SetRect(0, 0, 100, 31, false);
	this->pnlStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this->pnlStatus, CSTR("Valid Status")));
	this->lblStatus->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
	this->txtStatus->SetRect(104, 4, 200, 23, false);
	this->txtStatus->SetReadOnly(true);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpDesc = this->tcMain->AddTabPage(CSTR("Desc"));
	NEW_CLASS(this->txtDesc, UI::GUITextBox(ui, this->tpDesc, CSTR(""), true));
	this->txtDesc->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpASN1 = this->tcMain->AddTabPage(CSTR("ASN1"));
	NEW_CLASS(this->txtASN1, UI::GUITextBox(ui, this->tpASN1, CSTR(""), true));
	this->txtASN1->SetDockType(UI::GUIControl::DOCK_FILL);

	sb.ClearStr();
	this->asn1->ToString(&sb);
	this->txtDesc->SetText(sb.ToCString());
	sb.ClearStr();
	this->asn1->ToASN1String(&sb);
	this->txtASN1->SetText(sb.ToCString());

	if (this->asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
	{
		Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)this->asn1;
		Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), false);
		this->txtStatus->SetText(Crypto::Cert::X509File::ValidStatusGetDesc(x509->IsValid(ssl, ssl->GetTrustStore())));
		SDEL_CLASS(ssl);

		Bool canSignature = false;
		Bool canVerify = false;
		Crypto::Hash::HashType hashType = Crypto::Hash::HashType::SHA256;
		switch (x509->GetFileType())
		{
		case Crypto::Cert::X509File::FileType::FileList:
		{
			Crypto::Cert::X509FileList *fileList = (Crypto::Cert::X509FileList*)x509;
			canVerify = true;
			sb.ClearStr();
			Crypto::Cert::X509Cert *cert = (Crypto::Cert::X509Cert*)fileList->GetFile(0);
			if (cert->GetSubjectCN(&sb))
			{
				this->tcMain->SetTabPageName(1, sb.ToCString());
			}
			Crypto::Cert::X509File::SignedInfo signedInfo;
			if (cert->GetSignedInfo(&signedInfo))
			{
				hashType = Crypto::Cert::X509File::GetAlgHash(signedInfo.algType);
			}
			UI::GUITabPage *tp;
			UI::GUITextBox *txt;
			Crypto::Cert::X509File *file;
			UOSInt i = 1;
			UOSInt j = fileList->GetFileCount();
			while (i < j)
			{
				file = fileList->GetFile(i);
				sb.ClearStr();
				file->ToShortName(&sb);
				tp = this->tcMain->AddTabPage(sb.ToCString());
				sb.ClearStr();
				file->ToASN1String(&sb);
				NEW_CLASS(txt, UI::GUITextBox(ui, tp, sb.ToCString(), true));
				txt->SetDockType(UI::GUIControl::DOCK_FILL);

				i++;
			}
			break;
		}
		case Crypto::Cert::X509File::FileType::Cert:
		{
			Crypto::Cert::X509Cert *cert = (Crypto::Cert::X509Cert*)x509;
			canVerify = true;
			Crypto::Cert::X509File::SignedInfo signedInfo;
			if (cert->GetSignedInfo(&signedInfo))
			{
				hashType = Crypto::Cert::X509File::GetAlgHash(signedInfo.algType);
			}
			break;
		}
		case Crypto::Cert::X509File::FileType::Key:
		case Crypto::Cert::X509File::FileType::PrivateKey:
			canVerify = true;
			canSignature = true;
			break;
		case Crypto::Cert::X509File::FileType::PublicKey:
			canVerify = true;
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
			NEW_CLASS(this->lblVerifyHash, UI::GUILabel(ui, this->tpVerify, CSTR("Hash Type")));
			this->lblVerifyHash->SetRect(4, 4, 100, 23, false);
			NEW_CLASS(this->cboVerifyHash, UI::GUIComboBox(ui, this->tpVerify, false));
			this->cboVerifyHash->SetRect(104, 4, 200, 23, false);
			AddHashTypes(this->cboVerifyHash, hashType);
			NEW_CLASS(this->lblVerifyPayloadFile, UI::GUILabel(ui, this->tpVerify, CSTR("Payload File")));
			this->lblVerifyPayloadFile->SetRect(4, 28, 100, 23, false);
			NEW_CLASS(this->txtVerifyPayloadFile, UI::GUITextBox(ui, this->tpVerify, CSTR("")));
			this->txtVerifyPayloadFile->SetRect(104, 28, 300, 23, false);
			NEW_CLASS(this->lblVerifySignature, UI::GUILabel(ui, this->tpVerify, CSTR("Signature")));
			this->lblVerifySignature->SetRect(4, 52, 100, 23, false);
			NEW_CLASS(this->txtVerifySignature, UI::GUITextBox(ui, this->tpVerify, CSTR("")));
			this->txtVerifySignature->SetRect(104, 52, 300, 23, false);
			NEW_CLASS(this->btnVerify, UI::GUIButton(ui, this->tpVerify, CSTR("Verify")));
			this->btnVerify->SetRect(104, 76, 75, 23, false);
			this->btnVerify->HandleButtonClick(OnVerifyClicked, this);
			NEW_CLASS(this->btnVerifySignInfo, UI::GUIButton(ui, this->tpVerify, CSTR("Sign Info")));
			this->btnVerifySignInfo->SetRect(184, 76, 75, 23, false);
			this->btnVerifySignInfo->HandleButtonClick(OnVerifySignInfoClicked, this);
			NEW_CLASS(this->lblVerifyStatus, UI::GUILabel(ui, this->tpVerify, CSTR("Status")));
			this->lblVerifyStatus->SetRect(4, 100, 100, 23, false);
			NEW_CLASS(this->txtVerifyStatus, UI::GUITextBox(ui, this->tpVerify, CSTR(""), true));
			this->txtVerifyStatus->SetRect(104, 100, 600, 47, false);
			this->txtVerifyStatus->SetReadOnly(true);
		}
		if (canSignature)
		{
/*
			UI::GUITabPage *tpSignature;
			UI::GUILabel *lblSignatureHash;
			UI::GUIComboBox *cboSignatureHash;
			UI::GUILabel *lblSignaturePayloadFile;
			UI::GUITextBox *txtSignaturePayloadFile;
			UI::GUIButton *btnSignature;
			UI::GUILabel *lblSiguatureValue;
			UI::GUITextBox *txtSignatureValue;*/
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
	DEL_CLASS(this->asn1);
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
	case MNU_KEY_CREATE:
		{
			Crypto::Cert::X509Key *key = ((Crypto::Cert::X509PrivKey*)this->asn1)->CreateKey();
			if (key)
			{
				this->core->OpenObject(key);
			}
		}
	case MNU_CERT_EXT_KEY:
		{
			Crypto::Cert::X509Key *key = ((Crypto::Cert::X509Cert*)this->asn1)->GetNewPublicKey();
			if (key)
			{
				this->core->OpenObject(key);
			}
		}
	default:
		if (cmdId >= MNU_CERT_0)
		{
			Crypto::Cert::X509Cert *cert = ((Crypto::Cert::X509File*)this->asn1)->GetNewCert((UOSInt)(cmdId - MNU_CERT_0));
			if (cert)
			{
				this->core->OpenObject(cert);
			}
		}
		break;
	}
}
