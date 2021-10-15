#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "IO/StmData/FileData.h"
#include "Net/DefaultSSLEngine.h"
#include "SSWR/AVIRead/AVIRCertUtilForm.h"
#include "Text/StringTool.h"

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRCertUtilForm *me = (SSWR::AVIRead::AVIRCertUtilForm*)userObj;
	Parser::ParserList *parsers = me->core->GetParserList();

	UOSInt i = 0;
	IO::StmData::FileData *fd;
	IO::ParserType t;
	IO::ParsedObject *pobj;
	while (i < nFiles)
	{
		NEW_CLASS(fd, IO::StmData::FileData(files[i], false));
		pobj = parsers->ParseFile(fd, &t);
		DEL_CLASS(fd);
		if (pobj)
		{
			if (t == IO::ParserType::ASN1Data)
			{
				Net::ASN1Data *asn1 = (Net::ASN1Data*)pobj;
				if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
				{
					Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn1;
					Crypto::Cert::X509Key *key;
					Crypto::Cert::X509PrivKey *privKey;
					Crypto::Cert::X509CertReq *csr;
					Crypto::Cert::X509Cert *cert;
					Crypto::Cert::CertNames names;
					switch (x509->GetFileType())
					{
					case Crypto::Cert::X509File::FileType::Cert:
						cert = (Crypto::Cert::X509Cert*)x509;
						MemClear(&names, sizeof(names));
						if (cert->GetSubjNames(&names))
						{
							me->UpdateNames(&names);
							Crypto::Cert::CertNames::FreeNames(&names);
						}
						DEL_CLASS(x509);
						break;
					case Crypto::Cert::X509File::FileType::CertRequest:
						csr = (Crypto::Cert::X509CertReq*)x509;
						MemClear(&names, sizeof(names));
						if (csr->GetNames(&names))
						{
							me->UpdateNames(&names);
							Crypto::Cert::CertNames::FreeNames(&names);
						}
						DEL_CLASS(x509);
						break;
					case Crypto::Cert::X509File::FileType::Jks:
						DEL_CLASS(x509);
						break;
					case Crypto::Cert::X509File::FileType::Key:
						key = (Crypto::Cert::X509Key*)x509;
						if (key->IsPrivateKey())
						{
							SDEL_CLASS(me->key);
							me->key = key;
							me->UpdateKeyDetail();
						}
						else
						{
							DEL_CLASS(key);
						}
						break;
					case Crypto::Cert::X509File::FileType::PrivateKey:
						privKey = (Crypto::Cert::X509PrivKey*)x509;
						key = privKey->CreateKey();
						if (key)
						{
							SDEL_CLASS(me->key);
							me->key = key;
							me->UpdateKeyDetail();
						}
						DEL_CLASS(x509);
						break;
					case Crypto::Cert::X509File::FileType::PublicKey:
						DEL_CLASS(x509);
						break;
					}
				}
				else
				{
					DEL_CLASS(asn1);
				}
			}
			else
			{
				DEL_CLASS(pobj);
			}
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnKeyGenerateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCertUtilForm *me = (SSWR::AVIRead::AVIRCertUtilForm*)userObj;
	if (me->ssl)
	{
		Crypto::Cert::X509Key *key = me->ssl->GenerateRSAKey();
		if (key)
		{
			SDEL_CLASS(me->key);
			me->key = key;
			me->UpdateKeyDetail();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnKeyViewClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCertUtilForm *me = (SSWR::AVIRead::AVIRCertUtilForm*)userObj;
	if (me->key)
	{
		me->core->OpenObject(me->key->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnKeySaveClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCertUtilForm *me = (SSWR::AVIRead::AVIRCertUtilForm*)userObj;
	if (me->key)
	{
		me->core->SaveData(me, me->key, L"CertUtilSaveKey");
	}
}

void SSWR::AVIRead::AVIRCertUtilForm::UpdateKeyDetail()
{
	if (this->key == 0)
	{
		this->lblKeyDetail->SetText((const UTF8Char*)"-");
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(Crypto::Cert::X509File::KeyTypeGetName(this->key->GetKeyType()));
		sb.Append((const UTF8Char*)", ");
		sb.AppendUOSInt(key->GetKeySizeBits());
		sb.Append((const UTF8Char*)" bits");
		this->lblKeyDetail->SetText(sb.ToString());
	}
}

void SSWR::AVIRead::AVIRCertUtilForm::UpdateNames(Crypto::Cert::CertNames *names)
{
	this->txtCountryName->SetText(Text::StringTool::Null2Empty(names->countryName));
	this->txtStateOrProvinceName->SetText(Text::StringTool::Null2Empty(names->stateOrProvinceName));
	this->txtLocalityName->SetText(Text::StringTool::Null2Empty(names->localityName));
	this->txtOrganizationName->SetText(Text::StringTool::Null2Empty(names->organizationName));
	this->txtOrganizationUnitName->SetText(Text::StringTool::Null2Empty(names->organizationUnitName));
	this->txtCommonName->SetText(Text::StringTool::Null2Empty(names->commonName));
	this->txtEmailAddress->SetText(Text::StringTool::Null2Empty(names->emailAddress));
}

SSWR::AVIRead::AVIRCertUtilForm::AVIRCertUtilForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Cert Utility");
	this->SetFont(0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::DefaultSSLEngine::Create(this->core->GetSocketFactory(), true);
	NEW_CLASS(this->sanList, Data::ArrayList<const UTF8Char*>());
	this->key = 0;

	NEW_CLASS(this->lblKey, UI::GUILabel(ui, this, (const UTF8Char*)"Key"));
	this->lblKey->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->lblKeyDetail, UI::GUILabel(ui, this, (const UTF8Char*)"-"));
	this->lblKeyDetail->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->btnKeyGenerate, UI::GUIButton(ui, this, (const UTF8Char*)"Generate"));
	this->btnKeyGenerate->SetRect(304, 4, 75, 23, false);
	this->btnKeyGenerate->HandleButtonClick(OnKeyGenerateClicked, this);
	NEW_CLASS(this->btnKeyView, UI::GUIButton(ui, this, (const UTF8Char*)"View"));
	this->btnKeyView->SetRect(384, 4, 75, 23, false);
	this->btnKeyView->HandleButtonClick(OnKeyViewClicked, this);
	NEW_CLASS(this->btnKeySave, UI::GUIButton(ui, this, (const UTF8Char*)"Save"));
	this->btnKeySave->SetRect(464, 4, 75, 23, false);
	this->btnKeySave->HandleButtonClick(OnKeySaveClicked, this);
	NEW_CLASS(this->lblCountryName, UI::GUILabel(ui, this, (const UTF8Char*)"C"));
	this->lblCountryName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtCountryName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtCountryName->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblStateOrProvinceName, UI::GUILabel(ui, this, (const UTF8Char*)"ST"));
	this->lblStateOrProvinceName->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtStateOrProvinceName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtStateOrProvinceName->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->lblLocalityName, UI::GUILabel(ui, this, (const UTF8Char*)"L"));
	this->lblLocalityName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtLocalityName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtLocalityName->SetRect(104, 76, 200, 23, false);
	NEW_CLASS(this->lblOrganizationName, UI::GUILabel(ui, this, (const UTF8Char*)"O"));
	this->lblOrganizationName->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtOrganizationName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtOrganizationName->SetRect(104, 100, 200, 23, false);
	NEW_CLASS(this->lblOrganizationUnitName, UI::GUILabel(ui, this, (const UTF8Char*)"OU"));
	this->lblOrganizationUnitName->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtOrganizationUnitName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtOrganizationUnitName->SetRect(104, 124, 200, 23, false);
	NEW_CLASS(this->lblCommonName, UI::GUILabel(ui, this, (const UTF8Char*)"CN"));
	this->lblCommonName->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtCommonName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtCommonName->SetRect(104, 148, 200, 23, false);
	NEW_CLASS(this->lblEmailAddress, UI::GUILabel(ui, this, (const UTF8Char*)"Email"));
	this->lblEmailAddress->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtEmailAddress, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtEmailAddress->SetRect(104, 172, 200, 23, false);
	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRCertUtilForm::~AVIRCertUtilForm()
{
	LIST_FREE_FUNC(this->sanList, Text::StrDelNew);
	DEL_CLASS(this->sanList);
	SDEL_CLASS(this->key);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRCertUtilForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
