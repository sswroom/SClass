#include "Stdafx.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "IO/StmData/FileData.h"
#include "Net/SAMLUtil.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRSAMLDecryptForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSAMLDecryptForm::OnFormFiles(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRSAMLDecryptForm *me = (SSWR::AVIRead::AVIRSAMLDecryptForm*)userObj;
	Parser::ParserList *parsers = me->core->GetParserList();

	UOSInt i = 0;
	IO::ParsedObject *pobj;
	while (i < nFiles)
	{
		{
			IO::StmData::FileData fd(files[i], false);
			pobj = parsers->ParseFileType(&fd, IO::ParserType::ASN1Data);
		}
		if (pobj)
		{
			Net::ASN1Data *asn1 = (Net::ASN1Data*)pobj;
			Crypto::Cert::X509Key *key;
			if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
			{
				Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn1;
				switch (x509->GetFileType())
				{
				case Crypto::Cert::X509File::FileType::Cert:
					break;
				case Crypto::Cert::X509File::FileType::CertRequest:
					break;
				case Crypto::Cert::X509File::FileType::Key:
					key = (Crypto::Cert::X509Key*)x509;
					if (key->IsPrivateKey())
					{
						me->txtKey->SetText(files[i]->ToCString());
					}
					break;
				case Crypto::Cert::X509File::FileType::PrivateKey:
					me->txtKey->SetText(files[i]->ToCString());
					break;
				case Crypto::Cert::X509File::FileType::FileList:
				case Crypto::Cert::X509File::FileType::PublicKey:
				case Crypto::Cert::X509File::FileType::PKCS12:
				case Crypto::Cert::X509File::FileType::PKCS7:
				case Crypto::Cert::X509File::FileType::CRL:
					break;
				}
			}
			DEL_CLASS(pobj);
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRSAMLDecryptForm::OnDecryptClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSAMLDecryptForm *me = (SSWR::AVIRead::AVIRSAMLDecryptForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Net::SSLEngine *ssl;
	Crypto::Cert::X509Key *key = 0;
	IO::ParsedObject *pobj;
	me->txtKey->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please input private key file"), CSTR("SAML Response Decrypt"), me);
		return;
	}
	{
		IO::StmData::FileData fd(sb.ToCString(), false);
		pobj = me->core->GetParserList()->ParseFileType(&fd, IO::ParserType::ASN1Data);
	}
	if (pobj == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Key file is not in ASN1 Format"), CSTR("SAML Response Decrypt"), me);
		return;
	}
	Net::ASN1Data *asn1 = (Net::ASN1Data*)pobj;
	if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
	{
		Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn1;
		switch (x509->GetFileType())
		{
		case Crypto::Cert::X509File::FileType::Key:
			key = (Crypto::Cert::X509Key*)x509;
			if (!key->IsPrivateKey())
			{
				DEL_CLASS(key);
				UI::MessageDialog::ShowDialog(CSTR("Key file is not a private key"), CSTR("SAML Response Decrypt"), me);
				return;
			}
			break;
		case Crypto::Cert::X509File::FileType::PrivateKey:
			key = ((Crypto::Cert::X509PrivKey*)x509)->CreateKey();
			DEL_CLASS(x509);
			if (key == 0)
			{
				UI::MessageDialog::ShowDialog(CSTR("Error in converting key file to private key"), CSTR("SAML Response Decrypt"), me);
				return;
			}
			break;
		case Crypto::Cert::X509File::FileType::Cert:
		case Crypto::Cert::X509File::FileType::CertRequest:
		case Crypto::Cert::X509File::FileType::FileList:
		case Crypto::Cert::X509File::FileType::PublicKey:
		case Crypto::Cert::X509File::FileType::PKCS12:
		case Crypto::Cert::X509File::FileType::PKCS7:
		case Crypto::Cert::X509File::FileType::CRL:
		default:
			DEL_CLASS(asn1);
			UI::MessageDialog::ShowDialog(CSTR("Key file is not a key file"), CSTR("SAML Response Decrypt"), me);
			return;
		}
	}
	else
	{
		DEL_CLASS(asn1);
		UI::MessageDialog::ShowDialog(CSTR("Key file is in X.509 format"), CSTR("SAML Response Decrypt"), me);
		return;
	}
	sb.ClearStr();
	me->txtRAWResponse->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		DEL_CLASS(key);
		UI::MessageDialog::ShowDialog(CSTR("Please input raw response"), CSTR("SAML Response Decrypt"), me);
		return;
	}
	Text::StringBuilderUTF8 sbResult;
	ssl = Net::SSLEngineFactory::Create(me->core->GetSocketFactory(), false);
	Net::SAMLUtil::DecryptResponse(ssl, me->core->GetEncFactory(), key, sb.ToCString(), &sbResult);
	DEL_CLASS(ssl);
	DEL_CLASS(key);
	me->txtResult->SetText(sbResult.ToCString());
}

SSWR::AVIRead::AVIRSAMLDecryptForm::AVIRSAMLDecryptForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("SAML Response Decrypt"));
	this->SetFont(0, 0, 8.25, false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblKey, UI::GUILabel(ui, this, CSTR("Key File")));
	this->lblKey->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtKey, UI::GUITextBox(ui, this, CSTR("")));
	this->txtKey->SetRect(100, 0, 600, 23, false);
	NEW_CLASS(this->lblRAWResponse, UI::GUILabel(ui, this, CSTR("RAW Response")));
	this->lblRAWResponse->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtRAWResponse, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtRAWResponse->SetRect(100, 24, 500, 119, false);
	NEW_CLASS(this->btnDecrypt, UI::GUIButton(ui, this, CSTR("Decrypt")));
	this->btnDecrypt->SetRect(100, 144, 75, 23, false);
	this->btnDecrypt->HandleButtonClick(OnDecryptClicked, this);
	NEW_CLASS(this->lblResult, UI::GUILabel(ui, this, CSTR("Result")));
	this->lblResult->SetRect(0, 168, 100, 23, false);
	NEW_CLASS(this->txtResult, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtResult->SetRect(100, 168, 500, 119, false);

	this->HandleDropFiles(OnFormFiles, this);
}

SSWR::AVIRead::AVIRSAMLDecryptForm::~AVIRSAMLDecryptForm()
{
}

void SSWR::AVIRead::AVIRSAMLDecryptForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
