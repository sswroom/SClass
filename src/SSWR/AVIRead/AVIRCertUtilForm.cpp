#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509FileList.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "IO/StmData/FileData.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRCertUtilForm.h"
#include "Text/StringTool.h"

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRCertUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCertUtilForm>();
	NN<Parser::ParserList> parsers = me->core->GetParserList();

	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
	NN<IO::ParsedObject> pobj;
	while (i < nFiles)
	{
		IO::StmData::FileData fd(files[i], false);
		if (parsers->ParseFile(fd).SetTo(pobj))
		{
			if (pobj->GetParserType() == IO::ParserType::ASN1Data)
			{
				NN<Net::ASN1Data> asn1 = NN<Net::ASN1Data>::ConvertFrom(pobj);
				if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
				{
					NN<Crypto::Cert::X509File> x509 = NN<Crypto::Cert::X509File>::ConvertFrom(asn1);
					NN<Crypto::Cert::X509Key> key;
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
							me->DisplayNames(names);
							Crypto::Cert::CertNames::FreeNames(names);
						}
						MemClear(&exts, sizeof(exts));
						if (cert->GetExtensions(exts))
						{
							me->DisplayExtensions(exts);
							Crypto::Cert::CertExtensions::FreeExtensions(exts);
						}
						else
						{
							me->ClearExtensions();
						}
						x509.Delete();
						break;
					case Crypto::Cert::X509File::FileType::CertRequest:
						csr = NN<Crypto::Cert::X509CertReq>::ConvertFrom(x509);
						MemClear(&names, sizeof(names));
						if (csr->GetNames(names))
						{
							me->DisplayNames(names);
							Crypto::Cert::CertNames::FreeNames(names);
						}
						MemClear(&exts, sizeof(exts));
						if (csr->GetExtensions(exts))
						{
							me->DisplayExtensions(exts);
							Crypto::Cert::CertExtensions::FreeExtensions(exts);
						}
						else
						{
							me->ClearExtensions();
						}
						x509.Delete();
						break;
					case Crypto::Cert::X509File::FileType::Key:
						key = NN<Crypto::Cert::X509Key>::ConvertFrom(x509);
						if (key->IsPrivateKey())
						{
							me->key.Delete();
							me->key = key;
							me->DisplayKeyDetail();
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
								me->DisplayNames(names);
								Crypto::Cert::CertNames::FreeNames(names);
							}
							MemClear(&exts, sizeof(exts));
							if (cert->GetExtensions(exts))
							{
								me->DisplayExtensions(exts);
								Crypto::Cert::CertExtensions::FreeExtensions(exts);
							}
							else
							{
								me->ClearExtensions();
							}
						}
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

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnKeyGenerateClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCertUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCertUtilForm>();
	NN<Net::SSLEngine> ssl;
	if (me->ssl.SetTo(ssl))
	{
		NN<Crypto::Cert::X509Key> key;
		if (ssl->GenerateRSAKey().SetTo(key))
		{
			me->key.Delete();
			me->key = key;
			me->DisplayKeyDetail();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnKeyViewClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCertUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCertUtilForm>();
	NN<Crypto::Cert::X509Key> key;
	if (me->key.SetTo(key))
	{
		me->core->OpenObject(key->Clone());
	}
}

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnKeySaveClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCertUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCertUtilForm>();
	NN<Crypto::Cert::X509Key> key;
	if (me->key.SetTo(key))
	{
		me->core->SaveData(me.Ptr(), key, L"CertUtilSaveKey");
	}
}

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnSANAddClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCertUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCertUtilForm>();
	Text::StringBuilderUTF8 sb;
	me->txtSAN->GetText(sb);
	if (sb.GetLength() > 0)
	{
		me->sanList->Add(Text::String::New(sb.ToString(), sb.GetLength()));
		me->txtSAN->SetText(CSTR(""));
		me->lbSAN->AddItem(sb.ToCString(), 0);
	}
}

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnSANClearClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCertUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCertUtilForm>();
	me->sanList->FreeAll();
	me->lbSAN->ClearItems();
}

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnCSRGenerateClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCertUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCertUtilForm>();
	NN<Net::SSLEngine> ssl;
	if (!me->ssl.SetTo(ssl))
	{
		me->ui->ShowMsgOK(CSTR("SSL engine is not initiated"), CSTR("Cert Util"), me);
		return;
	}
	NN<Crypto::Cert::X509Key> key;
	if (!me->key.SetTo(key))
	{
		me->ui->ShowMsgOK(CSTR("Key not exist"), CSTR("Cert Util"), me);
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
	ext.caCert = me->chkCACert->IsChecked();
	ext.digitalSign = me->chkDigitalSign->IsChecked();
	Crypto::Cert::X509CertReq *csr;
	NN<Crypto::Cert::X509CertReq> nncsr;
	if (me->sanList->GetCount() > 0 || ext.caCert || ext.digitalSign)
	{
		csr = Crypto::Cert::CertUtil::CertReqCreate(ssl, names, key, &ext);
	}
	else
	{
		csr = Crypto::Cert::CertUtil::CertReqCreate(ssl, names, key, 0);
	}
	if (nncsr.Set(csr))
	{
		me->core->OpenObject(nncsr);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in creating cert request"), CSTR("Cert Util"), me);
	}
	Crypto::Cert::CertNames::FreeNames(names);
}

void __stdcall SSWR::AVIRead::AVIRCertUtilForm::OnSelfSignedCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCertUtilForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCertUtilForm>();
	NN<Net::SSLEngine> ssl;
	if (!me->ssl.SetTo(ssl))
	{
		me->ui->ShowMsgOK(CSTR("SSL Engine is not initiated"), CSTR("Cert Util"), me);
		return;
	}
	NN<Crypto::Cert::X509Key> key;
	if (!me->key.SetTo(key))
	{
		me->ui->ShowMsgOK(CSTR("Key not exist"), CSTR("Cert Util"), me);
		return;
	}
	UOSInt validDays;
	Text::StringBuilderUTF8 sb;
	me->txtValidDays->GetText(sb);
	if (!sb.ToUOSInt(validDays))
	{
		me->ui->ShowMsgOK(CSTR("Valid days not valid"), CSTR("Cert Util"), me);
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
	key->GetKeyId(BYTEARR(ext.subjKeyId));
	ext.useAuthKeyId = true;
	key->GetKeyId(BYTEARR(ext.authKeyId));
	ext.caCert = me->chkCACert->IsChecked();
	ext.digitalSign = me->chkDigitalSign->IsChecked();
	NN<Crypto::Cert::X509Cert> cert;
	if (cert.Set(Crypto::Cert::CertUtil::SelfSignedCertCreate(ssl, names, key, validDays, &ext)))
	{
		me->core->OpenObject(cert);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in creating self-signed certificate"), CSTR("Cert Util"), me);
	}
	Crypto::Cert::CertNames::FreeNames(names);
}

Bool SSWR::AVIRead::AVIRCertUtilForm::GetNames(Crypto::Cert::CertNames *names)
{
	Text::StringBuilderUTF8 sb;
	this->txtCountryName->GetText(sb);
	if (sb.GetLength() != 0)
	{
		if (sb.GetLength() != 2)
		{
			this->ui->ShowMsgOK(CSTR("C must be 2 chars"), CSTR("Cert Util"), this);
			return false;
		}
		OPTSTR_DEL(names->countryName);
		names->countryName = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}
	sb.ClearStr();
	this->txtStateOrProvinceName->GetText(sb);
	if (sb.GetLength() != 0)
	{
		OPTSTR_DEL(names->stateOrProvinceName);
		names->stateOrProvinceName = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}
	sb.ClearStr();
	this->txtLocalityName->GetText(sb);
	if (sb.GetLength() != 0)
	{
		OPTSTR_DEL(names->localityName);
		names->localityName = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}
	sb.ClearStr();
	this->txtOrganizationName->GetText(sb);
	if (sb.GetLength() != 0)
	{
		OPTSTR_DEL(names->organizationName);
		names->organizationName = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}
	sb.ClearStr();
	this->txtOrganizationUnitName->GetText(sb);
	if (sb.GetLength() != 0)
	{
		OPTSTR_DEL(names->organizationUnitName);
		names->organizationUnitName = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}
	sb.ClearStr();
	this->txtCommonName->GetText(sb);
	if (sb.GetLength() == 0)
	{
		this->ui->ShowMsgOK(CSTR("CN cannot be null"), CSTR("Cert Util"), this);
		return false;
	}
	OPTSTR_DEL(names->commonName);
	names->commonName = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();

	sb.ClearStr();
	this->txtEmailAddress->GetText(sb);
	if (sb.GetLength() != 0)
	{
		if (!Text::StringTool::IsEmailAddress(sb.ToString()))
		{
			this->ui->ShowMsgOK(CSTR("Email address is not valid"), CSTR("Cert Util"), this);
			return false;
		}
		OPTSTR_DEL(names->emailAddress);
		names->emailAddress = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	}
	return true;
}

void SSWR::AVIRead::AVIRCertUtilForm::DisplayKeyDetail()
{
	NN<Crypto::Cert::X509Key> key;
	if (!this->key.SetTo(key))
	{
		this->txtKeyDetail->SetText(CSTR("-"));
	}
	else
	{
		UInt8 keyId[20];
		Text::StringBuilderUTF8 sb;
		sb.Append(Crypto::Cert::X509File::KeyTypeGetName(key->GetKeyType()));
		sb.AppendC(UTF8STRC(", "));
		sb.AppendUOSInt(key->GetKeySizeBits());
		sb.AppendC(UTF8STRC(" bits"));
		if (key->GetKeyId(BYTEARR(keyId)))
		{
			sb.AppendC(UTF8STRC(", id="));
			sb.AppendHexBuff(BYTEARR(keyId), 0, Text::LineBreakType::None);
		}
		this->txtKeyDetail->SetText(sb.ToCString());
	}
}

void SSWR::AVIRead::AVIRCertUtilForm::DisplayNames(NN<Crypto::Cert::CertNames> names)
{
	this->txtCountryName->SetText(Text::String::OrEmpty(names->countryName)->ToCString());
	this->txtStateOrProvinceName->SetText(Text::String::OrEmpty(names->stateOrProvinceName)->ToCString());
	this->txtLocalityName->SetText(Text::String::OrEmpty(names->localityName)->ToCString());
	this->txtOrganizationName->SetText(Text::String::OrEmpty(names->organizationName)->ToCString());
	this->txtOrganizationUnitName->SetText(Text::String::OrEmpty(names->organizationUnitName)->ToCString());
	this->txtCommonName->SetText(Text::String::OrEmpty(names->commonName)->ToCString());
	this->txtEmailAddress->SetText(Text::String::OrEmpty(names->emailAddress)->ToCString());
}

void SSWR::AVIRead::AVIRCertUtilForm::DisplayExtensions(NN<Crypto::Cert::CertExtensions> exts)
{
	this->ClearExtensions();
	NN<Data::ArrayListStringNN> nameList;
	if (exts->subjectAltName.SetTo(nameList))
	{
		Data::ArrayIterator<NN<Text::String>> it = nameList->Iterator();
		NN<Text::String> s;
		while (it.HasNext())
		{
			s = it.Next()->Clone();
			this->sanList->Add(s);
			this->lbSAN->AddItem(s, 0);
		}
	}
}

void SSWR::AVIRead::AVIRCertUtilForm::ClearExtensions()
{
	this->sanList->FreeAll();
	this->lbSAN->ClearItems();
}

SSWR::AVIRead::AVIRCertUtilForm::AVIRCertUtilForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Cert Utility"));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	NEW_CLASS(this->sanList, Data::ArrayListStringNN());
	this->key = 0;

	this->lblKey = ui->NewLabel(*this, CSTR("Key"));
	this->lblKey->SetRect(4, 4, 100, 23, false);
	this->txtKeyDetail = ui->NewTextBox(*this, CSTR("-"));
	this->txtKeyDetail->SetReadOnly(true);
	this->txtKeyDetail->SetRect(104, 4, 200, 23, false);
	this->btnKeyGenerate = ui->NewButton(*this, CSTR("Generate"));
	this->btnKeyGenerate->SetRect(304, 4, 75, 23, false);
	this->btnKeyGenerate->HandleButtonClick(OnKeyGenerateClicked, this);
	this->btnKeyView = ui->NewButton(*this, CSTR("View"));
	this->btnKeyView->SetRect(384, 4, 75, 23, false);
	this->btnKeyView->HandleButtonClick(OnKeyViewClicked, this);
	this->btnKeySave = ui->NewButton(*this, CSTR("Save"));
	this->btnKeySave->SetRect(464, 4, 75, 23, false);
	this->btnKeySave->HandleButtonClick(OnKeySaveClicked, this);
	this->lblCountryName = ui->NewLabel(*this, CSTR("C"));
	this->lblCountryName->SetRect(4, 28, 100, 23, false);
	this->txtCountryName = ui->NewTextBox(*this, CSTR(""));
	this->txtCountryName->SetRect(104, 28, 200, 23, false);
	this->lblCountryName2 = ui->NewLabel(*this, CSTR("Country Name (2 Chars)"));
	this->lblCountryName2->SetRect(304, 28, 200, 23, false);
	this->lblStateOrProvinceName = ui->NewLabel(*this, CSTR("ST"));
	this->lblStateOrProvinceName->SetRect(4, 52, 100, 23, false);
	this->txtStateOrProvinceName = ui->NewTextBox(*this, CSTR(""));
	this->txtStateOrProvinceName->SetRect(104, 52, 200, 23, false);
	this->lblStateOrProvinceName2 = ui->NewLabel(*this, CSTR("State Or Province Name"));
	this->lblStateOrProvinceName2->SetRect(304, 52, 200, 23, false);
	this->lblLocalityName = ui->NewLabel(*this, CSTR("L"));
	this->lblLocalityName->SetRect(4, 76, 100, 23, false);
	this->txtLocalityName = ui->NewTextBox(*this, CSTR(""));
	this->txtLocalityName->SetRect(104, 76, 200, 23, false);
	this->lblLocalityName2 = ui->NewLabel(*this, CSTR("Locality Name"));
	this->lblLocalityName2->SetRect(304, 76, 200, 23, false);
	this->lblOrganizationName = ui->NewLabel(*this, CSTR("O"));
	this->lblOrganizationName->SetRect(4, 100, 100, 23, false);
	this->txtOrganizationName = ui->NewTextBox(*this, CSTR(""));
	this->txtOrganizationName->SetRect(104, 100, 200, 23, false);
	this->lblOrganizationName2 = ui->NewLabel(*this, CSTR("Organization Name"));
	this->lblOrganizationName2->SetRect(304, 100, 200, 23, false);
	this->lblOrganizationUnitName = ui->NewLabel(*this, CSTR("OU"));
	this->lblOrganizationUnitName->SetRect(4, 124, 100, 23, false);
	this->txtOrganizationUnitName = ui->NewTextBox(*this, CSTR(""));
	this->txtOrganizationUnitName->SetRect(104, 124, 200, 23, false);
	this->lblOrganizationUnitName2 = ui->NewLabel(*this, CSTR("Organization Unit Name"));
	this->lblOrganizationUnitName2->SetRect(304, 124, 200, 23, false);
	this->lblCommonName = ui->NewLabel(*this, CSTR("CN"));
	this->lblCommonName->SetRect(4, 148, 100, 23, false);
	this->txtCommonName = ui->NewTextBox(*this, CSTR(""));
	this->txtCommonName->SetRect(104, 148, 200, 23, false);
	this->lblCommonName2 = ui->NewLabel(*this, CSTR("Common Name"));
	this->lblCommonName2->SetRect(304, 148, 200, 23, false);
	this->lblEmailAddress = ui->NewLabel(*this, CSTR("Email"));
	this->lblEmailAddress->SetRect(4, 172, 100, 23, false);
	this->txtEmailAddress = ui->NewTextBox(*this, CSTR(""));
	this->txtEmailAddress->SetRect(104, 172, 200, 23, false);
	this->lblValidDays = ui->NewLabel(*this, CSTR("Valid Days"));
	this->lblValidDays->SetRect(4, 196, 100, 23, false);
	this->txtValidDays = ui->NewTextBox(*this, CSTR("365"));
	this->txtValidDays->SetRect(104, 196, 200, 23, false);
	this->chkCACert = ui->NewCheckBox(*this, CSTR("CA Cert"), false);
	this->chkCACert->SetRect(104, 220, 200, 23, false);
	this->chkDigitalSign = ui->NewCheckBox(*this, CSTR("Digital Sign"), false);
	this->chkDigitalSign->SetRect(104, 244, 200, 23, false);
	this->lblSAN = ui->NewLabel(*this, CSTR("SubjAltName"));
	this->lblSAN->SetRect(4, 268, 100, 23, false);
	this->txtSAN = ui->NewTextBox(*this, CSTR(""));
	this->txtSAN->SetRect(104, 268, 200, 23, false);
	this->btnSANAdd = ui->NewButton(*this, CSTR("Add"));
	this->btnSANAdd->SetRect(304, 268, 75, 23, false);
	this->btnSANAdd->HandleButtonClick(OnSANAddClicked, this);
	this->lbSAN = ui->NewListBox(*this, false);
	this->lbSAN->SetRect(104, 292, 200, 95, false);
	this->btnSANClear = ui->NewButton(*this, CSTR("Clear"));
	this->btnSANClear->SetRect(304, 292, 75, 23, false);
	this->btnSANClear->HandleButtonClick(OnSANClearClicked, this);
	this->btnCSRGenerate = ui->NewButton(*this, CSTR("Generate CSR"));
	this->btnCSRGenerate->SetRect(104, 388, 150, 23, false);
	this->btnCSRGenerate->HandleButtonClick(OnCSRGenerateClicked, this);
	this->btnSelfSignedCert = ui->NewButton(*this, CSTR("Self-Signed Cert"));
	this->btnSelfSignedCert->SetRect(254, 388, 150, 23, false);
	this->btnSelfSignedCert->HandleButtonClick(OnSelfSignedCertClicked, this);

	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRCertUtilForm::~AVIRCertUtilForm()
{
	this->sanList->FreeAll();
	DEL_CLASS(this->sanList);
	this->key.Delete();
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRCertUtilForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
