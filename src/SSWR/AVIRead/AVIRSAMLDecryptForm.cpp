#include "Stdafx.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "IO/StmData/FileData.h"
#include "Net/SAMLUtil.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRSAMLDecryptForm.h"

void __stdcall SSWR::AVIRead::AVIRSAMLDecryptForm::OnFormFiles(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files)
{
	NotNullPtr<SSWR::AVIRead::AVIRSAMLDecryptForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSAMLDecryptForm>();
	NotNullPtr<Parser::ParserList> parsers = me->core->GetParserList();

	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
	IO::ParsedObject *pobj;
	while (i < nFiles)
	{
		{
			IO::StmData::FileData fd(files[i], false);
			pobj = parsers->ParseFileType(fd, IO::ParserType::ASN1Data);
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

void __stdcall SSWR::AVIRead::AVIRSAMLDecryptForm::OnDecryptClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRSAMLDecryptForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSAMLDecryptForm>();
	Text::StringBuilderUTF8 sb;
	NotNullPtr<Net::SSLEngine> ssl;
	Crypto::Cert::X509Key *key = 0;
	IO::ParsedObject *pobj;
	me->txtKey->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please input private key file"), CSTR("SAML Response Decrypt"), me);
		return;
	}
	{
		IO::StmData::FileData fd(sb.ToCString(), false);
		pobj = me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ASN1Data);
	}
	if (pobj == 0)
	{
		me->ui->ShowMsgOK(CSTR("Key file is not in ASN1 Format"), CSTR("SAML Response Decrypt"), me);
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
				me->ui->ShowMsgOK(CSTR("Key file is not a private key"), CSTR("SAML Response Decrypt"), me);
				return;
			}
			break;
		case Crypto::Cert::X509File::FileType::PrivateKey:
			key = ((Crypto::Cert::X509PrivKey*)x509)->CreateKey();
			DEL_CLASS(x509);
			if (key == 0)
			{
				me->ui->ShowMsgOK(CSTR("Error in converting key file to private key"), CSTR("SAML Response Decrypt"), me);
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
			me->ui->ShowMsgOK(CSTR("Key file is not a key file"), CSTR("SAML Response Decrypt"), me);
			return;
		}
	}
	else
	{
		DEL_CLASS(asn1);
		me->ui->ShowMsgOK(CSTR("Key file is in X.509 format"), CSTR("SAML Response Decrypt"), me);
		return;
	}
	sb.ClearStr();
	me->txtRAWResponse->GetText(sb);
	NotNullPtr<Crypto::Cert::X509Key> keyNN;
	if (sb.GetLength() == 0 || !keyNN.Set(key))
	{
		DEL_CLASS(key);
		me->ui->ShowMsgOK(CSTR("Please input raw response"), CSTR("SAML Response Decrypt"), me);
		return;
	}
	Text::StringBuilderUTF8 sbResult;
	if (Net::SSLEngineFactory::Create(me->core->GetSocketFactory(), false).SetTo(ssl))
	{
		Net::SAMLUtil::DecryptResponse(ssl, me->core->GetEncFactory(), keyNN, sb.ToCString(), sbResult);
		ssl.Delete();
	}
	else
	{
		sbResult.Append(CSTR("SSL Engine error"));
	}
	keyNN.Delete();
	me->txtResult->SetText(sbResult.ToCString());
}

SSWR::AVIRead::AVIRSAMLDecryptForm::AVIRSAMLDecryptForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("SAML Response Decrypt"));
	this->SetFont(0, 0, 8.25, false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblKey = ui->NewLabel(*this, CSTR("Key File"));
	this->lblKey->SetRect(0, 0, 100, 23, false);
	this->txtKey = ui->NewTextBox(*this, CSTR(""));
	this->txtKey->SetRect(100, 0, 600, 23, false);
	this->lblRAWResponse = ui->NewLabel(*this, CSTR("RAW Response"));
	this->lblRAWResponse->SetRect(0, 24, 100, 23, false);
	this->txtRAWResponse = ui->NewTextBox(*this, CSTR(""), true);
	this->txtRAWResponse->SetRect(100, 24, 500, 119, false);
	this->btnDecrypt = ui->NewButton(*this, CSTR("Decrypt"));
	this->btnDecrypt->SetRect(100, 144, 75, 23, false);
	this->btnDecrypt->HandleButtonClick(OnDecryptClicked, this);
	this->lblResult = ui->NewLabel(*this, CSTR("Result"));
	this->lblResult->SetRect(0, 168, 100, 23, false);
	this->txtResult = ui->NewTextBox(*this, CSTR(""), true);
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
