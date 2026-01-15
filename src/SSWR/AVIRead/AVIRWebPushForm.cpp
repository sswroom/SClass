#include "Stdafx.h"
#include "Net/ASN1Data.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebPushClient.h"
#include "Parser/FileParser/X509Parser.h"
#include "SSWR/AVIRead/AVIRWebPushForm.h"
#include "Text/StringTool.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRWebPushForm::OnPrivateKeyClicked(AnyType userData)
{
	NN<SSWR::AVIRead::AVIRWebPushForm> me = userData.GetNN<SSWR::AVIRead::AVIRWebPushForm>();
	Text::StringBuilderUTF8 sb;
	me->txtPrivateKey->GetText(sb);
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"WebPushPrivateKey", false);
	if (sb.leng > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	dlg->SetAllowMultiSel(false);
	dlg->AddFilter(CSTR("*.key"), CSTR("Key File"));
	dlg->AddFilter(CSTR("*.pem"), CSTR("PEM File"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->OpenFile(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRWebPushForm::OnPushClicked(AnyType userData)
{
	NN<SSWR::AVIRead::AVIRWebPushForm> me = userData.GetNN<SSWR::AVIRead::AVIRWebPushForm>();
	NN<Net::SSLEngine> ssl;
	NN<Crypto::Cert::X509Key> key;
	if (me->ssl.SetTo(ssl) && me->key.SetTo(key))
	{
		Text::StringBuilderUTF8 sbEndPoint;
		me->txtEndPoint->GetText(sbEndPoint);
		if (!sbEndPoint.StartsWith(CSTR("https://")))
		{
			me->ui->ShowMsgOK(CSTR("EndPoint is not valid"), CSTR("Web Push"), me);
			return;
		}
		Text::StringBuilderUTF8 sbEmail;
		me->txtEmail->GetText(sbEmail);
		if (!Text::StringTool::IsEmailAddress(sbEmail.v))
		{
			me->ui->ShowMsgOK(CSTR("Email is not valid"), CSTR("Web Push"), me);
			return;
		}
		Net::WebPushClient cli(me->clif, ssl);
		if (cli.Push(sbEndPoint.ToCString(), key, sbEmail.ToCString(), 60))
		{
			me->lblPushStatus->SetText(CSTR("Push successfully"));
		}
		else
		{
			me->lblPushStatus->SetText(CSTR("Push failed"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWebPushForm::OnFiles(AnyType userData, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRWebPushForm> me = userData.GetNN<SSWR::AVIRead::AVIRWebPushForm>();
	UIntOS i = 0;
	UIntOS j = files.GetCount();
	while (i < j)
	{
		if (me->OpenFile(files[i]->ToCString()))
		{
			break;
		}
		i++;
	}
}

Bool SSWR::AVIRead::AVIRWebPushForm::OpenFile(Text::CStringNN fileName)
{
	Parser::FileParser::X509Parser parser;
	NN<IO::ParsedObject> pobj;
	if (parser.ParseFilePath(fileName).SetTo(pobj))
	{
		if (pobj->GetParserType() != IO::ParserType::ASN1Data)
		{
			pobj.Delete();
			return false;
		}
		NN<Net::ASN1Data> asn1 = NN<Net::ASN1Data>::ConvertFrom(pobj);
		if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
		{
			asn1.Delete();
			return false;
		}
		NN<Crypto::Cert::X509File> x509 = NN<Crypto::Cert::X509File>::ConvertFrom(asn1);
		if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Key)
		{
			NN<Crypto::Cert::X509Key> key = NN<Crypto::Cert::X509Key>::ConvertFrom(x509);
			if (key->GetKeyType() == Crypto::Cert::X509File::KeyType::ECDSA && key->GetECName() == Crypto::Cert::X509File::ECName::secp256r1)
			{
				this->key.Delete();
				this->key = key;
				this->txtPrivateKey->SetText(fileName);
				return true;
			}
		}
		x509.Delete();
	}
	return false;
}

SSWR::AVIRead::AVIRWebPushForm::AVIRWebPushForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Web Push"));
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->clif = this->core->GetTCPClientFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->key = nullptr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblPrivateKey = ui->NewLabel(*this, CSTR("Private Key"));
	this->lblPrivateKey->SetRect(4, 4, 100, 23, false);
	this->txtPrivateKey = ui->NewTextBox(*this, CSTR(""));
	this->txtPrivateKey->SetRect(104, 4, 500, 23, false);
	this->txtPrivateKey->SetReadOnly(true);
	this->btnPrivateKey = ui->NewButton(*this, CSTR("Browse"));
	this->btnPrivateKey->SetRect(604, 4, 75, 23, false);
	this->btnPrivateKey->HandleButtonClick(OnPrivateKeyClicked, this);
	this->lblEndPoint = ui->NewLabel(*this, CSTR("End Point"));
	this->lblEndPoint->SetRect(4, 28, 100, 23, false);
	this->txtEndPoint = ui->NewTextBox(*this, CSTR(""));
	this->txtEndPoint->SetRect(104, 28, 500, 23, false);
	this->lblEmail = ui->NewLabel(*this, CSTR("Email"));
	this->lblEmail->SetRect(4, 52, 100, 23, false);
	this->txtEmail = ui->NewTextBox(*this, CSTR(""));
	this->txtEmail->SetRect(104, 52, 200, 23, false);
	this->btnPush = ui->NewButton(*this, CSTR("Push"));
	this->btnPush->SetRect(104, 76, 75, 23, false);
	this->btnPush->HandleButtonClick(OnPushClicked, this);
	this->lblPushStatus = ui->NewLabel(*this, CSTR(""));
	this->lblPushStatus->SetRect(184, 76, 200, 23, false);
	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIRWebPushForm::~AVIRWebPushForm()
{
	this->ssl.Delete();
	this->key.Delete();
}

void SSWR::AVIRead::AVIRWebPushForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
