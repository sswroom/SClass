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
	NN<IO::ParsedObject> pobj;
	while (i < nFiles)
	{
		IO::StmData::FileData fd(files[i]->ToCString(), false);
		if (parsers->ParseFile(fd).SetTo(pobj))
		{
			if (pobj->GetParserType() == IO::ParserType::ASN1Data)
			{
				NN<Net::ASN1Data> asn1 = NN<Net::ASN1Data>::ConvertFrom(pobj);
				if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
				{
					NN<Crypto::Cert::X509File> x509 = NN<Crypto::Cert::X509File>::ConvertFrom(asn1);
					NN<Crypto::Cert::X509Key> key;
					NN<Text::String> s;
					NN<Crypto::Cert::X509PrivKey> privKey;
					NN<Crypto::Cert::X509CertReq> csr;
					NN<Crypto::Cert::X509Cert> cert;
					Crypto::Cert::CertNames names;
					Crypto::Cert::CertExtensions exts;
					switch (x509->GetFileType())
					{
					case Crypto::Cert::X509File::FileType::Cert:
						cert = NN<Crypto::Cert::X509Cert>::ConvertFrom(x509);
						MemClear(&names, sizeof(names));
						if (cert->GetSubjNames(names))
						{
							me->caCert.Delete();
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
							if (me->key.SetTo(key) && me->ssl.SetTo(ssl))
							{
								if (!cert->IsSignatureKey(ssl, key))
								{
									me->key.Delete();
									me->txtKey->SetText(CSTR("-"));
								}
							}
						}
						else
						{
							x509.Delete();
						}
						break;
					case Crypto::Cert::X509File::FileType::CertRequest:
						csr = NN<Crypto::Cert::X509CertReq>::ConvertFrom(x509);
						MemClear(&names, sizeof(names));
						if (csr->GetNames(names))
						{
							me->csr.Delete();
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
							x509.Delete();
						}
						break;
					case Crypto::Cert::X509File::FileType::Key:
						key = NN<Crypto::Cert::X509Key>::ConvertFrom(x509);
						if (key->IsPrivateKey())
						{
							NN<Net::SSLEngine> ssl;
							if (me->caCert.SetTo(cert) && (!me->ssl.SetTo(ssl) || !cert->IsSignatureKey(ssl, key)))
							{
								me->ui->ShowMsgOK(CSTR("Key is not match with CA cert"), CSTR("CA Util"), me);
								key.Delete();
							}
							else
							{
								me->key.Delete();
								me->key = key;
								me->DisplayKeyDetail();
							}
						}
						else
						{
							key.Delete();
						}
						break;
					case Crypto::Cert::X509File::FileType::PrivateKey:
						privKey = NN<Crypto::Cert::X509PrivKey>::ConvertFrom(x509);
						if (privKey->CreateKey().SetTo(key))
						{
							me->key.Delete();
							me->key = key;
							me->DisplayKeyDetail();
						}
						x509.Delete();
						break;
					case Crypto::Cert::X509File::FileType::FileList:
						if (Optional<Crypto::Cert::X509Cert>::ConvertFrom(NN<Crypto::Cert::X509FileList>::ConvertFrom(x509)->GetFile(0)).SetTo(cert))
						{
							MemClear(&names, sizeof(names));
							if (cert->GetSubjNames(names))
							{
								me->caCert.Delete();
								me->caCert = cert = NN<Crypto::Cert::X509Cert>::ConvertFrom(cert->Clone());
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
								if (me->key.SetTo(key) && me->ssl.SetTo(ssl))
								{
									if (!cert->IsSignatureKey(ssl, key))
									{
										me->key.Delete();
										me->txtKey->SetText(CSTR("-"));
									}
								}
							}
						}
						x509.Delete();
						break;
					case Crypto::Cert::X509File::FileType::EPrivateKey:
						x509.Delete();
						break;
					case Crypto::Cert::X509File::FileType::PublicKey:
						x509.Delete();
						break;
					case Crypto::Cert::X509File::FileType::PKCS12:
						x509.Delete();
						break;
					case Crypto::Cert::X509File::FileType::PKCS7:
						x509.Delete();
						break;
					case Crypto::Cert::X509File::FileType::CRL:
						x509.Delete();
						break;
					}
				}
				else
				{
					asn1.Delete();
				}
			}
			else
			{
				pobj.Delete();
			}
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnKeyViewClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCAUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCAUtilForm>();
	NN<Crypto::Cert::X509Key> key;
	if (me->key.SetTo(key))
	{
		me->core->OpenObject(key->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnCACertViewClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCAUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCAUtilForm>();
	NN<Crypto::Cert::X509Cert> caCert;
	if (me->caCert.SetTo(caCert))
	{
		me->core->OpenObject(caCert->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnCSRViewClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCAUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCAUtilForm>();
	NN<Crypto::Cert::X509CertReq> csr;
	if (me->csr.SetTo(csr))
	{
		me->core->OpenObject(csr->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRCAUtilForm::OnIssueClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCAUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCAUtilForm>();
	NN<Crypto::Cert::X509Key> key;
	NN<Crypto::Cert::X509Cert> caCert;
	NN<Crypto::Cert::X509CertReq> csr;
	if (!me->key.SetTo(key))
	{
		me->ui->ShowMsgOK(CSTR("Key not exist"), CSTR("CA Util"), me);
		return;
	}
	if (!me->caCert.SetTo(caCert))
	{
		me->ui->ShowMsgOK(CSTR("CA Cert not exist"), CSTR("CA Util"), me);
		return;
	}
	if (!me->csr.SetTo(csr))
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
	if (Crypto::Cert::CertUtil::IssueCert(ssl, caCert, key, validDays, csr).SetTo(cert))
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
	NN<Crypto::Cert::X509Key> key;
	if (!this->key.SetTo(key))
	{
		this->txtKey->SetText(CSTR("-"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(Crypto::Cert::X509File::KeyTypeGetName(key->GetKeyType()));
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
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
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
	this->caCert.Delete();
	this->key.Delete();
	this->csr.Delete();
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRCAUtilForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
