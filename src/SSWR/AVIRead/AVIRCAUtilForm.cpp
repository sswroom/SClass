#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Data/RandomBytesGenerator.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/DefaultSSLEngine.h"
#include "SSWR/AVIRead/AVIRCAUtilForm.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRCAUtilForm *me = (SSWR::AVIRead::AVIRCAUtilForm*)userObj;
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
					Crypto::Cert::CertExtensions exts;
					switch (x509->GetFileType())
					{
					case Crypto::Cert::X509File::FileType::Cert:
						cert = (Crypto::Cert::X509Cert*)x509;
						MemClear(&names, sizeof(names));
						if (cert->GetSubjNames(&names))
						{
							SDEL_CLASS(me->caCert);
							me->caCert = cert;
							me->txtCACert->SetText(names.commonName);
							Crypto::Cert::CertNames::FreeNames(&names);
							if (me->key)
							{
								if (!me->caCert->IsSignatureKey(me->ssl, me->key))
								{
									DEL_CLASS(me->key);
									me->key = 0;
									me->txtKey->SetText((const UTF8Char*)"-");
								}
							}
						}
						else
						{
							DEL_CLASS(x509);
						}
						break;
					case Crypto::Cert::X509File::FileType::CertRequest:
						csr = (Crypto::Cert::X509CertReq*)x509;
						MemClear(&names, sizeof(names));
						if (csr->GetNames(&names))
						{
							SDEL_CLASS(me->csr);
							me->csr = csr;
							me->UpdateNames(&names);
							me->txtCSR->SetText(names.commonName);
							Crypto::Cert::CertNames::FreeNames(&names);

							me->lbSAN->ClearItems();
							MemClear(&exts, sizeof(exts));
							if (csr->GetExtensions(&exts))
							{
								if (exts.subjectAltName)
								{
									UOSInt j = 0;
									UOSInt k = exts.subjectAltName->GetCount();
									while (j < k)
									{
										me->lbSAN->AddItem(exts.subjectAltName->GetItem(j), 0);
										j++;
									}
								}
								Crypto::Cert::CertExtensions::FreeExtensions(&exts);
							}
						}
						else
						{
							DEL_CLASS(x509);
						}
						break;
					case Crypto::Cert::X509File::FileType::Jks:
						DEL_CLASS(x509);
						break;
					case Crypto::Cert::X509File::FileType::Key:
						key = (Crypto::Cert::X509Key*)x509;
						if (key->IsPrivateKey())
						{
							if (me->caCert && !me->caCert->IsSignatureKey(me->ssl, key))
							{
								UI::MessageDialog::ShowDialog((const UTF8Char*)"Key is not match with CA cert", (const UTF8Char*)"CA Util", me);
								DEL_CLASS(key);
							}
							else
							{
								SDEL_CLASS(me->key);
								me->key = key;
								me->UpdateKeyDetail();
							}
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

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnKeyViewClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCAUtilForm *me = (SSWR::AVIRead::AVIRCAUtilForm*)userObj;
	if (me->key)
	{
		me->core->OpenObject(me->key->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnCACertViewClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCAUtilForm *me = (SSWR::AVIRead::AVIRCAUtilForm*)userObj;
	if (me->caCert)
	{
		me->core->OpenObject(me->caCert->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnCSRViewClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCAUtilForm *me = (SSWR::AVIRead::AVIRCAUtilForm*)userObj;
	if (me->csr)
	{
		me->core->OpenObject(me->csr->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnIssueClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCAUtilForm *me = (SSWR::AVIRead::AVIRCAUtilForm*)userObj;
	if (me->key == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Key not exist", (const UTF8Char*)"CA Util", me);
		return;
	}
	if (me->caCert == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"CA Cert not exist", (const UTF8Char*)"CA Util", me);
		return;
	}
	if (me->csr == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"CSR not exist", (const UTF8Char*)"CA Util", me);
		return;
	}
	UOSInt validDays;
	Text::StringBuilderUTF8 sb;
	me->txtValidDays->GetText(&sb);
	if (!sb.ToUOSInt(&validDays))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Valid Days not valid", (const UTF8Char*)"CA Util", me);
		return;
	}
	Crypto::Cert::X509Cert *cert = Crypto::Cert::CertUtil::IssueCert(me->ssl, me->caCert, me->key, validDays, me->csr);
	if (cert)
	{
		me->core->OpenObject(cert);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in issuing certificate", (const UTF8Char*)"CA Util", me);
	}
}

void SSWR::AVIRead::AVIRCAUtilForm::UpdateKeyDetail()
{
	if (this->key == 0)
	{
		this->txtKey->SetText((const UTF8Char*)"-");
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(Crypto::Cert::X509File::KeyTypeGetName(this->key->GetKeyType()));
		sb.Append((const UTF8Char*)", ");
		sb.AppendUOSInt(key->GetKeySizeBits());
		sb.Append((const UTF8Char*)" bits");
		this->txtKey->SetText(sb.ToString());
	}
}

void SSWR::AVIRead::AVIRCAUtilForm::UpdateNames(Crypto::Cert::CertNames *names)
{
	this->txtCountryName->SetText(Text::StringTool::Null2Empty(names->countryName));
	this->txtStateOrProvinceName->SetText(Text::StringTool::Null2Empty(names->stateOrProvinceName));
	this->txtLocalityName->SetText(Text::StringTool::Null2Empty(names->localityName));
	this->txtOrganizationName->SetText(Text::StringTool::Null2Empty(names->organizationName));
	this->txtOrganizationUnitName->SetText(Text::StringTool::Null2Empty(names->organizationUnitName));
	this->txtCommonName->SetText(Text::StringTool::Null2Empty(names->commonName));
	this->txtEmailAddress->SetText(Text::StringTool::Null2Empty(names->emailAddress));
}

SSWR::AVIRead::AVIRCAUtilForm::AVIRCAUtilForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"CA Utility");
	this->SetFont(0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::DefaultSSLEngine::Create(this->core->GetSocketFactory(), true);
	NEW_CLASS(this->sanList, Data::ArrayList<const UTF8Char*>());
	this->caCert = 0;
	this->key = 0;
	this->csr = 0;

	NEW_CLASS(this->lblKey, UI::GUILabel(ui, this, (const UTF8Char*)"Key"));
	this->lblKey->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtKey, UI::GUITextBox(ui, this, (const UTF8Char*)"-"));
	this->txtKey->SetRect(104, 4, 200, 23, false);
	this->txtKey->SetReadOnly(true);
	NEW_CLASS(this->btnKeyView, UI::GUIButton(ui, this, (const UTF8Char*)"View"));
	this->btnKeyView->SetRect(304, 4, 75, 23, false);
	this->btnKeyView->HandleButtonClick(OnKeyViewClicked, this);
	NEW_CLASS(this->lblCACert, UI::GUILabel(ui, this, (const UTF8Char*)"CA Cert"));
	this->lblCACert->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtCACert, UI::GUITextBox(ui, this, (const UTF8Char*)"-"));
	this->txtCACert->SetRect(104, 28, 200, 23, false);
	this->txtCACert->SetReadOnly(true);
	NEW_CLASS(this->btnCACertView, UI::GUIButton(ui, this, (const UTF8Char*)"View"));
	this->btnCACertView->SetRect(304, 28, 75, 23, false);
	this->btnCACertView->HandleButtonClick(OnKeyViewClicked, this);
	NEW_CLASS(this->lblCSR, UI::GUILabel(ui, this, (const UTF8Char*)"CSR"));
	this->lblCSR->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtCSR, UI::GUITextBox(ui, this, (const UTF8Char*)"-"));
	this->txtCSR->SetRect(104, 52, 200, 23, false);
	this->txtCSR->SetReadOnly(true);
	NEW_CLASS(this->btnCSRView, UI::GUIButton(ui, this, (const UTF8Char*)"View"));
	this->btnCSRView->SetRect(304, 52, 75, 23, false);
	this->btnCSRView->HandleButtonClick(OnKeyViewClicked, this);
	NEW_CLASS(this->lblCountryName, UI::GUILabel(ui, this, (const UTF8Char*)"C"));
	this->lblCountryName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtCountryName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtCountryName->SetRect(104, 76, 200, 23, false);
	this->txtCountryName->SetReadOnly(true);
	NEW_CLASS(this->lblStateOrProvinceName, UI::GUILabel(ui, this, (const UTF8Char*)"ST"));
	this->lblStateOrProvinceName->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtStateOrProvinceName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtStateOrProvinceName->SetRect(104, 100, 200, 23, false);
	this->txtStateOrProvinceName->SetReadOnly(true);
	NEW_CLASS(this->lblLocalityName, UI::GUILabel(ui, this, (const UTF8Char*)"L"));
	this->lblLocalityName->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtLocalityName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtLocalityName->SetRect(104, 124, 200, 23, false);
	this->txtLocalityName->SetReadOnly(true);
	NEW_CLASS(this->lblOrganizationName, UI::GUILabel(ui, this, (const UTF8Char*)"O"));
	this->lblOrganizationName->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtOrganizationName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtOrganizationName->SetRect(104, 148, 200, 23, false);
	this->txtOrganizationName->SetReadOnly(true);
	NEW_CLASS(this->lblOrganizationUnitName, UI::GUILabel(ui, this, (const UTF8Char*)"OU"));
	this->lblOrganizationUnitName->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtOrganizationUnitName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtOrganizationUnitName->SetRect(104, 172, 200, 23, false);
	this->txtOrganizationUnitName->SetReadOnly(true);
	NEW_CLASS(this->lblCommonName, UI::GUILabel(ui, this, (const UTF8Char*)"CN"));
	this->lblCommonName->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtCommonName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtCommonName->SetRect(104, 196, 200, 23, false);
	this->txtCommonName->SetReadOnly(true);
	NEW_CLASS(this->lblEmailAddress, UI::GUILabel(ui, this, (const UTF8Char*)"Email"));
	this->lblEmailAddress->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtEmailAddress, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtEmailAddress->SetRect(104, 220, 200, 23, false);
	this->txtEmailAddress->SetReadOnly(true);
	NEW_CLASS(this->lblSAN, UI::GUILabel(ui, this, (const UTF8Char*)"SubjAltName"));
	this->lblSAN->SetRect(4, 244, 100, 23, false);
	NEW_CLASS(this->lbSAN, UI::GUIListBox(ui, this, false));
	this->lbSAN->SetRect(104, 244, 200, 95, false);
	NEW_CLASS(this->lblValidDays, UI::GUILabel(ui, this, (const UTF8Char*)"Valid Days"));
	this->lblValidDays->SetRect(4, 340, 100, 23, false);
	NEW_CLASS(this->txtValidDays, UI::GUITextBox(ui, this, (const UTF8Char*)"365"));
	this->txtValidDays->SetRect(104, 340, 200, 23, false);
	NEW_CLASS(this->btnIssue, UI::GUIButton(ui, this, (const UTF8Char *)"Issue"));
	this->btnIssue->SetRect(104, 364, 150, 23, false);
	this->btnIssue->HandleButtonClick(OnIssueClicked, this);

	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRCAUtilForm::~AVIRCAUtilForm()
{
	LIST_FREE_FUNC(this->sanList, Text::StrDelNew);
	DEL_CLASS(this->sanList);
	SDEL_CLASS(this->caCert);
	SDEL_CLASS(this->key);
	SDEL_CLASS(this->csr);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRCAUtilForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}