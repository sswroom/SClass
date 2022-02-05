#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "IO/StmData/FileData.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRCertUtilForm.h"
#include "Text/StringTool.h"
#include "UI/MessageDialog.h"

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
		UOSInt fileNameLen = Text::StrCharCnt(files[i]);
		NEW_CLASS(fd, IO::StmData::FileData({files[i], fileNameLen}, false));
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
					Crypto::Cert::CertExtensions exts;
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
						MemClear(&exts, sizeof(exts));
						if (cert->GetExtensions(&exts))
						{
							me->UpdateExtensions(&exts);
							Crypto::Cert::CertExtensions::FreeExtensions(&exts);
						}
						else
						{
							me->ClearExtensions();
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
						MemClear(&exts, sizeof(exts));
						if (csr->GetExtensions(&exts))
						{
							me->UpdateExtensions(&exts);
							Crypto::Cert::CertExtensions::FreeExtensions(&exts);
						}
						else
						{
							me->ClearExtensions();
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
					case Crypto::Cert::X509File::FileType::PKCS12:
						DEL_CLASS(x509);
						break;
					case Crypto::Cert::X509File::FileType::PKCS7:
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

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnSANAddClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCertUtilForm *me = (SSWR::AVIRead::AVIRCertUtilForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtSAN->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		me->sanList->Add(Text::String::New(sb.ToString(), sb.GetLength()));
		me->txtSAN->SetText((const UTF8Char*)"");
		me->lbSAN->AddItem(sb.ToCString(), 0);
	}
}

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnSANClearClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCertUtilForm *me = (SSWR::AVIRead::AVIRCertUtilForm*)userObj;
	LIST_FREE_STRING(me->sanList);
	me->sanList->Clear();
	me->lbSAN->ClearItems();
}

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnCSRGenerateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCertUtilForm *me = (SSWR::AVIRead::AVIRCertUtilForm*)userObj;
	if (me->key == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Key not exist", (const UTF8Char*)"Cert Util", me);
		return;
	}
	Crypto::Cert::CertNames names;
	MemClear(&names, sizeof(names));
	if (!me->GetNames(&names))
	{
		return;
	}
	Crypto::Cert::CertExtensions ext;
	MemClear(&ext, sizeof(ext));
	ext.subjectAltName = me->sanList;
	Crypto::Cert::X509CertReq *csr = Crypto::Cert::CertUtil::CertReqCreate(me->ssl, &names, me->key, &ext);
	if (csr)
	{
		me->core->OpenObject(csr);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in creating cert request", (const UTF8Char*)"Cert Util", me);
	}
	Crypto::Cert::CertNames::FreeNames(&names);
}

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnSelfSignedCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCertUtilForm *me = (SSWR::AVIRead::AVIRCertUtilForm*)userObj;
	if (me->key == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Key not exist", (const UTF8Char*)"Cert Util", me);
		return;
	}
	UOSInt validDays;
	Text::StringBuilderUTF8 sb;
	me->txtValidDays->GetText(&sb);
	if (!sb.ToUOSInt(&validDays))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Valid days not valid", (const UTF8Char*)"Cert Util", me);
		return;
	}
	Crypto::Cert::CertNames names;
	MemClear(&names, sizeof(names));
	if (!me->GetNames(&names))
	{
		return;
	}
	Crypto::Cert::CertExtensions ext;
	MemClear(&ext, sizeof(ext));
	ext.subjectAltName = me->sanList;
	ext.useSubjKeyId = true;
	me->key->GetKeyId(ext.subjKeyId);
	ext.useAuthKeyId = true;
	me->key->GetKeyId(ext.authKeyId);
	ext.caCert = me->chkCACert->IsChecked();
	Crypto::Cert::X509Cert *cert = Crypto::Cert::CertUtil::SelfSignedCertCreate(me->ssl, &names, me->key, validDays, &ext);
	if (cert)
	{
		me->core->OpenObject(cert);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in creating self-signed certificate", (const UTF8Char*)"Cert Util", me);
	}
	Crypto::Cert::CertNames::FreeNames(&names);
}

Bool SSWR::AVIRead::AVIRCertUtilForm::GetNames(Crypto::Cert::CertNames *names)
{
	Text::StringBuilderUTF8 sb;
	this->txtCountryName->GetText(&sb);
	if (sb.GetLength() != 0)
	{
		if (sb.GetLength() != 2)
		{
			return false;
		}
		SDEL_STRING(names->countryName);
		names->countryName = Text::String::New(sb.ToString(), sb.GetLength());
	}
	sb.ClearStr();
	this->txtStateOrProvinceName->GetText(&sb);
	if (sb.GetLength() != 0)
	{
		SDEL_STRING(names->stateOrProvinceName);
		names->stateOrProvinceName = Text::String::New(sb.ToString(), sb.GetLength());
	}
	sb.ClearStr();
	this->txtLocalityName->GetText(&sb);
	if (sb.GetLength() != 0)
	{
		SDEL_STRING(names->localityName);
		names->localityName = Text::String::New(sb.ToString(), sb.GetLength());
	}
	sb.ClearStr();
	this->txtOrganizationName->GetText(&sb);
	if (sb.GetLength() != 0)
	{
		SDEL_STRING(names->organizationName);
		names->organizationName = Text::String::New(sb.ToString(), sb.GetLength());
	}
	sb.ClearStr();
	this->txtOrganizationUnitName->GetText(&sb);
	if (sb.GetLength() != 0)
	{
		SDEL_STRING(names->organizationUnitName);
		names->organizationUnitName = Text::String::New(sb.ToString(), sb.GetLength());
	}
	sb.ClearStr();
	this->txtCommonName->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		return false;
	}
	SDEL_STRING(names->commonName);
	names->commonName = Text::String::New(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	this->txtEmailAddress->GetText(&sb);
	if (sb.GetLength() != 0)
	{
		if (!Text::StringTool::IsEmailAddress(sb.ToString()))
		{
			return false;
		}
		SDEL_STRING(names->emailAddress);
		names->emailAddress = Text::String::New(sb.ToString(), sb.GetLength());
	}
	return true;
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
		sb.AppendC(UTF8STRC(", "));
		sb.AppendUOSInt(key->GetKeySizeBits());
		sb.AppendC(UTF8STRC(" bits"));
		this->lblKeyDetail->SetText(sb.ToString());
	}
}

void SSWR::AVIRead::AVIRCertUtilForm::UpdateNames(Crypto::Cert::CertNames *names)
{
	this->txtCountryName->SetText(Text::String::OrEmpty(names->countryName)->v);
	this->txtStateOrProvinceName->SetText(Text::String::OrEmpty(names->stateOrProvinceName)->v);
	this->txtLocalityName->SetText(Text::String::OrEmpty(names->localityName)->v);
	this->txtOrganizationName->SetText(Text::String::OrEmpty(names->organizationName)->v);
	this->txtOrganizationUnitName->SetText(Text::String::OrEmpty(names->organizationUnitName)->v);
	this->txtCommonName->SetText(Text::String::OrEmpty(names->commonName)->v);
	this->txtEmailAddress->SetText(Text::String::OrEmpty(names->emailAddress)->v);
}

void SSWR::AVIRead::AVIRCertUtilForm::UpdateExtensions(Crypto::Cert::CertExtensions *exts)
{
	this->ClearExtensions();
	if (exts->subjectAltName)
	{
		UOSInt i = 0;
		UOSInt j = exts->subjectAltName->GetCount();
		Text::String *s;
		while (i < j)
		{
			s = exts->subjectAltName->GetItem(i);
			this->sanList->Add(s->Clone());
			this->lbSAN->AddItem(s, 0);
			i++;
		}
	}
}

void SSWR::AVIRead::AVIRCertUtilForm::ClearExtensions()
{
	LIST_FREE_STRING(this->sanList);
	this->sanList->Clear();
	this->lbSAN->ClearItems();
}

SSWR::AVIRead::AVIRCertUtilForm::AVIRCertUtilForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Cert Utility");
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	NEW_CLASS(this->sanList, Data::ArrayList<Text::String*>());
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
	NEW_CLASS(this->txtCountryName, UI::GUITextBox(ui, this, CSTR("")));
	this->txtCountryName->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblStateOrProvinceName, UI::GUILabel(ui, this, (const UTF8Char*)"ST"));
	this->lblStateOrProvinceName->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtStateOrProvinceName, UI::GUITextBox(ui, this, CSTR("")));
	this->txtStateOrProvinceName->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->lblLocalityName, UI::GUILabel(ui, this, (const UTF8Char*)"L"));
	this->lblLocalityName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtLocalityName, UI::GUITextBox(ui, this, CSTR("")));
	this->txtLocalityName->SetRect(104, 76, 200, 23, false);
	NEW_CLASS(this->lblOrganizationName, UI::GUILabel(ui, this, (const UTF8Char*)"O"));
	this->lblOrganizationName->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtOrganizationName, UI::GUITextBox(ui, this, CSTR("")));
	this->txtOrganizationName->SetRect(104, 100, 200, 23, false);
	NEW_CLASS(this->lblOrganizationUnitName, UI::GUILabel(ui, this, (const UTF8Char*)"OU"));
	this->lblOrganizationUnitName->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtOrganizationUnitName, UI::GUITextBox(ui, this, CSTR("")));
	this->txtOrganizationUnitName->SetRect(104, 124, 200, 23, false);
	NEW_CLASS(this->lblCommonName, UI::GUILabel(ui, this, (const UTF8Char*)"CN"));
	this->lblCommonName->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtCommonName, UI::GUITextBox(ui, this, CSTR("")));
	this->txtCommonName->SetRect(104, 148, 200, 23, false);
	NEW_CLASS(this->lblEmailAddress, UI::GUILabel(ui, this, (const UTF8Char*)"Email"));
	this->lblEmailAddress->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtEmailAddress, UI::GUITextBox(ui, this, CSTR("")));
	this->txtEmailAddress->SetRect(104, 172, 200, 23, false);
	NEW_CLASS(this->lblValidDays, UI::GUILabel(ui, this, (const UTF8Char*)"Valid Days"));
	this->lblValidDays->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtValidDays, UI::GUITextBox(ui, this, CSTR("365")));
	this->txtValidDays->SetRect(104, 196, 200, 23, false);
	NEW_CLASS(this->chkCACert, UI::GUICheckBox(ui, this, (const UTF8Char*)"CA Cert", false));
	this->chkCACert->SetRect(104, 220, 200, 23, false);
	NEW_CLASS(this->lblSAN, UI::GUILabel(ui, this, (const UTF8Char*)"SubjAltName"));
	this->lblSAN->SetRect(4, 244, 100, 23, false);
	NEW_CLASS(this->txtSAN, UI::GUITextBox(ui, this, CSTR("")));
	this->txtSAN->SetRect(104, 244, 200, 23, false);
	NEW_CLASS(this->btnSANAdd, UI::GUIButton(ui, this, (const UTF8Char*)"Add"));
	this->btnSANAdd->SetRect(304, 244, 75, 23, false);
	this->btnSANAdd->HandleButtonClick(OnSANAddClicked, this);
	NEW_CLASS(this->lbSAN, UI::GUIListBox(ui, this, false));
	this->lbSAN->SetRect(104, 268, 200, 95, false);
	NEW_CLASS(this->btnSANClear, UI::GUIButton(ui, this, (const UTF8Char*)"Clear"));
	this->btnSANClear->SetRect(304, 268, 75, 23, false);
	this->btnSANClear->HandleButtonClick(OnSANClearClicked, this);
	NEW_CLASS(this->btnCSRGenerate, UI::GUIButton(ui, this, (const UTF8Char*)"Generate CSR"));
	this->btnCSRGenerate->SetRect(104, 364, 150, 23, false);
	this->btnCSRGenerate->HandleButtonClick(OnCSRGenerateClicked, this);
	NEW_CLASS(this->btnSelfSignedCert, UI::GUIButton(ui, this, (const UTF8Char*)"Self-Signed Cert"));
	this->btnSelfSignedCert->SetRect(254, 364, 150, 23, false);
	this->btnSelfSignedCert->HandleButtonClick(OnSelfSignedCertClicked, this);

	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRCertUtilForm::~AVIRCertUtilForm()
{
	LIST_FREE_STRING(this->sanList);
	DEL_CLASS(this->sanList);
	SDEL_CLASS(this->key);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRCertUtilForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
