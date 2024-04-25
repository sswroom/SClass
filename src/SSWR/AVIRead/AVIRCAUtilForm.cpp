#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Crypto/Cert/X509FileList.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Data/RandomBytesGenerator.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRCAUtilForm.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRCAUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCAUtilForm>();
	NN<Parser::ParserList> parsers = me->core->GetParserList();

	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
	IO::ParsedObject *pobj;
	while (i < nFiles)
	{
		{
			IO::StmData::FileData fd(files[i]->ToCString(), false);
			pobj = parsers->ParseFile(fd);
		}
		if (pobj)
		{
			if (pobj->GetParserType() == IO::ParserType::ASN1Data)
			{
				Net::ASN1Data *asn1 = (Net::ASN1Data*)pobj;
				if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
				{
					Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn1;
					NN<Crypto::Cert::X509Key> key;
					NN<Text::String> s;
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
						if (cert->GetSubjNames(names))
						{
							SDEL_CLASS(me->caCert);
							me->caCert = cert;
							if (names.commonName.SetTo(s))
							{
								me->txtCACert->SetText(s->ToCString());
							}
							else
							{
								me->txtCACert->SetText(CSTR("Unnamed"));
							}
							Crypto::Cert::CertNames::FreeNames(names);
							NN<Net::SSLEngine> ssl;
							if (key.Set(me->key) && me->ssl.SetTo(ssl))
							{
								if (!me->caCert->IsSignatureKey(ssl, key))
								{
									DEL_CLASS(me->key);
									me->key = 0;
									me->txtKey->SetText(CSTR("-"));
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
						if (csr->GetNames(names))
						{
							SDEL_CLASS(me->csr);
							me->csr = csr;
							me->DisplayNames(names);
							if (names.commonName.SetTo(s))
							{
								me->txtCSR->SetText(s->ToCString());
							}
							else
							{
								me->txtCSR->SetText(CSTR("Unnamed"));
							}
							Crypto::Cert::CertNames::FreeNames(names);

							me->lbSAN->ClearItems();
							MemClear(&exts, sizeof(exts));
							if (csr->GetExtensions(exts))
							{
								NN<Data::ArrayListStringNN> nameList;
								if (exts.subjectAltName.SetTo(nameList))
								{
									UOSInt j = 0;
									UOSInt k = nameList->GetCount();
									while (j < k)
									{
										me->lbSAN->AddItem(nameList->GetItemNoCheck(j), 0);
										j++;
									}
								}
								Crypto::Cert::CertExtensions::FreeExtensions(exts);
							}
						}
						else
						{
							DEL_CLASS(x509);
						}
						break;
					case Crypto::Cert::X509File::FileType::Key:
						if (key.Set((Crypto::Cert::X509Key*)x509))
						{
							if (key->IsPrivateKey())
							{
								NN<Net::SSLEngine> ssl;
								if (me->caCert && (!me->ssl.SetTo(ssl) || !me->caCert->IsSignatureKey(ssl, key)))
								{
									me->ui->ShowMsgOK(CSTR("Key is not match with CA cert"), CSTR("CA Util"), me);
									key.Delete();
								}
								else
								{
									SDEL_CLASS(me->key);
									me->key = key.Ptr();
									me->DisplayKeyDetail();
								}
							}
							else
							{
								key.Delete();
							}
						}
						break;
					case Crypto::Cert::X509File::FileType::PrivateKey:
						privKey = (Crypto::Cert::X509PrivKey*)x509;
						if (privKey->CreateKey().SetTo(key))
						{
							SDEL_CLASS(me->key);
							me->key = key.Ptr();
							me->DisplayKeyDetail();
						}
						DEL_CLASS(x509);
						break;
					case Crypto::Cert::X509File::FileType::FileList:
						cert = (Crypto::Cert::X509Cert*)((Crypto::Cert::X509FileList*)x509)->GetFile(0).OrNull();
						MemClear(&names, sizeof(names));
						if (cert->GetSubjNames(names))
						{
							SDEL_CLASS(me->caCert);
							me->caCert = (Crypto::Cert::X509Cert*)cert->Clone().Ptr();
							if (names.commonName.SetTo(s))
							{
								me->txtCACert->SetText(s->ToCString());
							}
							else
							{
								me->txtCACert->SetText(CSTR("Unnamed"));
							}
							Crypto::Cert::CertNames::FreeNames(names);
							NN<Net::SSLEngine> ssl;
							if (key.Set(me->key) && me->ssl.SetTo(ssl))
							{
								if (!me->caCert->IsSignatureKey(ssl, key))
								{
									DEL_CLASS(me->key);
									me->key = 0;
									me->txtKey->SetText(CSTR("-"));
								}
							}
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
					case Crypto::Cert::X509File::FileType::CRL:
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

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnKeyViewClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCAUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCAUtilForm>();
	if (me->key)
	{
		me->core->OpenObject(me->key->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnCACertViewClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCAUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCAUtilForm>();
	if (me->caCert)
	{
		me->core->OpenObject(me->caCert->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnCSRViewClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCAUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCAUtilForm>();
	if (me->csr)
	{
		me->core->OpenObject(me->csr->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnIssueClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCAUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCAUtilForm>();
	NN<Crypto::Cert::X509Key> key;
	if (!key.Set(me->key))
	{
		me->ui->ShowMsgOK(CSTR("Key not exist"), CSTR("CA Util"), me);
		return;
	}
	if (me->caCert == 0)
	{
		me->ui->ShowMsgOK(CSTR("CA Cert not exist"), CSTR("CA Util"), me);
		return;
	}
	if (me->csr == 0)
	{
		me->ui->ShowMsgOK(CSTR("CSR not exist"), CSTR("CA Util"), me);
		return;
	}
	UOSInt validDays;
	Text::StringBuilderUTF8 sb;
	me->txtValidDays->GetText(sb);
	if (!sb.ToUOSInt(validDays))
	{
		me->ui->ShowMsgOK(CSTR("Valid Days not valid"), CSTR("CA Util"), me);
		return;
	}
	NN<Net::SSLEngine> ssl;
	if (!me->ssl.SetTo(ssl))
	{
		me->ui->ShowMsgOK(CSTR("SSL Engine is not initiated"), CSTR("CA Util"), me);
		return;
	}
	NN<Crypto::Cert::X509Cert> cert;
	if (cert.Set(Crypto::Cert::CertUtil::IssueCert(ssl, me->caCert, key, validDays, me->csr)))
	{
		me->core->OpenObject(cert);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in issuing certificate"), CSTR("CA Util"), me);
	}
}

void SSWR::AVIRead::AVIRCAUtilForm::DisplayKeyDetail()
{
	if (this->key == 0)
	{
		this->txtKey->SetText(CSTR("-"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(Crypto::Cert::X509File::KeyTypeGetName(this->key->GetKeyType()));
		sb.AppendC(UTF8STRC(", "));
		sb.AppendUOSInt(key->GetKeySizeBits());
		sb.AppendC(UTF8STRC(" bits"));
		this->txtKey->SetText(sb.ToCString());
	}
}

void SSWR::AVIRead::AVIRCAUtilForm::DisplayNames(NN<Crypto::Cert::CertNames> names)
{
	this->txtCountryName->SetText(Text::String::OrEmpty(names->countryName)->ToCString());
	this->txtStateOrProvinceName->SetText(Text::String::OrEmpty(names->stateOrProvinceName)->ToCString());
	this->txtLocalityName->SetText(Text::String::OrEmpty(names->localityName)->ToCString());
	this->txtOrganizationName->SetText(Text::String::OrEmpty(names->organizationName)->ToCString());
	this->txtOrganizationUnitName->SetText(Text::String::OrEmpty(names->organizationUnitName)->ToCString());
	this->txtCommonName->SetText(Text::String::OrEmpty(names->commonName)->ToCString());
	this->txtEmailAddress->SetText(Text::String::OrEmpty(names->emailAddress)->ToCString());
}

SSWR::AVIRead::AVIRCAUtilForm::AVIRCAUtilForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("CA Utility"));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	NEW_CLASS(this->sanList, Data::ArrayList<const UTF8Char*>());
	this->caCert = 0;
	this->key = 0;
	this->csr = 0;

	this->lblKey = ui->NewLabel(*this, CSTR("Key"));
	this->lblKey->SetRect(4, 4, 100, 23, false);
	this->txtKey = ui->NewTextBox(*this, CSTR("-"));
	this->txtKey->SetRect(104, 4, 200, 23, false);
	this->txtKey->SetReadOnly(true);
	this->btnKeyView = ui->NewButton(*this, CSTR("View"));
	this->btnKeyView->SetRect(304, 4, 75, 23, false);
	this->btnKeyView->HandleButtonClick(OnKeyViewClicked, this);
	this->lblCACert = ui->NewLabel(*this, CSTR("CA Cert"));
	this->lblCACert->SetRect(4, 28, 100, 23, false);
	this->txtCACert = ui->NewTextBox(*this, CSTR("-"));
	this->txtCACert->SetRect(104, 28, 200, 23, false);
	this->txtCACert->SetReadOnly(true);
	this->btnCACertView = ui->NewButton(*this, CSTR("View"));
	this->btnCACertView->SetRect(304, 28, 75, 23, false);
	this->btnCACertView->HandleButtonClick(OnKeyViewClicked, this);
	this->lblCSR = ui->NewLabel(*this, CSTR("CSR"));
	this->lblCSR->SetRect(4, 52, 100, 23, false);
	this->txtCSR = ui->NewTextBox(*this, CSTR("-"));
	this->txtCSR->SetRect(104, 52, 200, 23, false);
	this->txtCSR->SetReadOnly(true);
	this->btnCSRView = ui->NewButton(*this, CSTR("View"));
	this->btnCSRView->SetRect(304, 52, 75, 23, false);
	this->btnCSRView->HandleButtonClick(OnKeyViewClicked, this);
	this->lblCountryName = ui->NewLabel(*this, CSTR("C"));
	this->lblCountryName->SetRect(4, 76, 100, 23, false);
	this->txtCountryName = ui->NewTextBox(*this, CSTR(""));
	this->txtCountryName->SetRect(104, 76, 200, 23, false);
	this->txtCountryName->SetReadOnly(true);
	this->lblStateOrProvinceName = ui->NewLabel(*this, CSTR("ST"));
	this->lblStateOrProvinceName->SetRect(4, 100, 100, 23, false);
	this->txtStateOrProvinceName = ui->NewTextBox(*this, CSTR(""));
	this->txtStateOrProvinceName->SetRect(104, 100, 200, 23, false);
	this->txtStateOrProvinceName->SetReadOnly(true);
	this->lblLocalityName = ui->NewLabel(*this, CSTR("L"));
	this->lblLocalityName->SetRect(4, 124, 100, 23, false);
	this->txtLocalityName = ui->NewTextBox(*this, CSTR(""));
	this->txtLocalityName->SetRect(104, 124, 200, 23, false);
	this->txtLocalityName->SetReadOnly(true);
	this->lblOrganizationName = ui->NewLabel(*this, CSTR("O"));
	this->lblOrganizationName->SetRect(4, 148, 100, 23, false);
	this->txtOrganizationName = ui->NewTextBox(*this, CSTR(""));
	this->txtOrganizationName->SetRect(104, 148, 200, 23, false);
	this->txtOrganizationName->SetReadOnly(true);
	this->lblOrganizationUnitName = ui->NewLabel(*this, CSTR("OU"));
	this->lblOrganizationUnitName->SetRect(4, 172, 100, 23, false);
	this->txtOrganizationUnitName = ui->NewTextBox(*this, CSTR(""));
	this->txtOrganizationUnitName->SetRect(104, 172, 200, 23, false);
	this->txtOrganizationUnitName->SetReadOnly(true);
	this->lblCommonName = ui->NewLabel(*this, CSTR("CN"));
	this->lblCommonName->SetRect(4, 196, 100, 23, false);
	this->txtCommonName = ui->NewTextBox(*this, CSTR(""));
	this->txtCommonName->SetRect(104, 196, 200, 23, false);
	this->txtCommonName->SetReadOnly(true);
	this->lblEmailAddress = ui->NewLabel(*this, CSTR("Email"));
	this->lblEmailAddress->SetRect(4, 220, 100, 23, false);
	this->txtEmailAddress = ui->NewTextBox(*this, CSTR(""));
	this->txtEmailAddress->SetRect(104, 220, 200, 23, false);
	this->txtEmailAddress->SetReadOnly(true);
	this->lblSAN = ui->NewLabel(*this, CSTR("SubjAltName"));
	this->lblSAN->SetRect(4, 244, 100, 23, false);
	this->lbSAN = ui->NewListBox(*this, false);
	this->lbSAN->SetRect(104, 244, 200, 95, false);
	this->lblValidDays = ui->NewLabel(*this, CSTR("Valid Days"));
	this->lblValidDays->SetRect(4, 340, 100, 23, false);
	this->txtValidDays = ui->NewTextBox(*this, CSTR("365"));
	this->txtValidDays->SetRect(104, 340, 200, 23, false);
	this->btnIssue = ui->NewButton(*this, CSTR("Issue"));
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
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRCAUtilForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
