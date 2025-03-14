#include "Stdafx.h"
#include "Crypto/Token/JWToken.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRJWTParserForm.h"
#include "Text/JSON.h"

void __stdcall SSWR::AVIRead::AVIRJWTParserForm::OnParseClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRJWTParserForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJWTParserForm>();
	Text::StringBuilderUTF8 sbJWT;
	Text::StringBuilderUTF8 sbErr;
	Bool succ = false;
	NN<Crypto::Token::JWToken> token;
	Int64 t;
	me->txtJWT->GetText(sbJWT);
	me->token.Delete();
	me->token = Crypto::Token::JWToken::Parse(sbJWT.ToCString(), sbErr);
	me->verifyType = Crypto::Token::JWToken::VerifyType::Unknown;
	if (me->token.SetTo(token))
	{
		Crypto::Token::JWTParam param;
		NN<Data::StringMap<Text::String*>> result;
		if (token->ParsePayload(param, true, sbErr).SetTo(result))
		{
			me->verifyType = token->GetVerifyType(param);
			succ = true;
			me->txtParseStatus->SetText(CSTR("Success"));
			me->txtVerifyStatus->SetText(CSTR("Not Verified"));
			me->txtIssuer->SetText(Text::String::OrEmpty(param.GetIssuer())->ToCString());
			me->txtSubject->SetText(Text::String::OrEmpty(param.GetSubject())->ToCString());
			me->txtAudience->SetText(Text::String::OrEmpty(param.GetAudience())->ToCString());
			if ((t = param.GetExpirationTime()) != 0)
			{
				sbErr.ClearStr();
				sbErr.AppendTSNoZone(Data::Timestamp(Data::TimeInstant(t, 0), Data::DateTimeUtil::GetLocalTzQhr()));
				me->txtExpTime->SetText(sbErr.ToCString());
			}
			else
			{
				me->txtExpTime->SetText(CSTR("-"));
			}
			if ((t = param.GetNotBefore()) != 0)
			{
				sbErr.ClearStr();
				sbErr.AppendTSNoZone(Data::Timestamp(Data::TimeInstant(t, 0), Data::DateTimeUtil::GetLocalTzQhr()));
				me->txtNotBefore->SetText(sbErr.ToCString());
			}
			else
			{
				me->txtNotBefore->SetText(CSTR("-"));
			}
			if ((t = param.GetIssuedAt()) != 0)
			{
				sbErr.ClearStr();
				sbErr.AppendTSNoZone(Data::Timestamp(Data::TimeInstant(t, 0), Data::DateTimeUtil::GetLocalTzQhr()));
				me->txtIssueAt->SetText(sbErr.ToCString());
			}
			else
			{
				me->txtIssueAt->SetText(CSTR("-"));
			}
			me->txtJWTId->SetText(Text::String::OrEmpty(param.GetJWTId())->ToCString());

			me->lvPayload->ClearItems();
			NN<Text::String> name;
			UOSInt i = 0;
			UOSInt j = result->GetCount();
			while (i < j)
			{
				name = Text::String::OrEmpty(result->GetKey(i));
				me->lvPayload->AddItem(name, 0);
				me->lvPayload->SetSubItem(i, 1, Crypto::Token::JWToken::PayloadName(name->ToCString()));
				me->lvPayload->SetSubItem(i, 2, Text::String::OrEmpty(result->GetItem(i)));
				i++;
			}
			token->FreeResult(result);

			if (me->verifyType == Crypto::Token::JWToken::VerifyType::Azure)
			{
				NN<Text::JSONBase> json;
				if (!Text::JSONBase::ParseJSONStr(Text::String::OrEmpty(token->GetHeader())->ToCString()).SetTo(json))
				{
					me->txtVerifyStatus->SetText(CSTR("Cannot parse JWT header"));
				}
				else
				{
					NN<Text::String> kid;
					if (!json->GetValueString(CSTR("kid")).SetTo(kid))
					{
						me->txtVerifyStatus->SetText(CSTR("kid not found"));
					}
					else
					{
						NN<Crypto::Cert::X509Key> key;
						if (me->azure->CreateKey(kid->ToCString()).SetTo(key))
						{
							if (token->SignatureValid(me->ssl, key->GetASN1Buff(), key->GetASN1BuffSize(), key->GetKeyType()))
							{
								me->txtVerifyStatus->SetText(CSTR("Signature Valid"));
							}
							else
							{
								me->txtVerifyStatus->SetText(CSTR("Signature not valid"));
							}
							key.Delete();
						}
						else
						{
							me->txtVerifyStatus->SetText(CSTR("Cannot found key from Azure"));
						}
						kid->Release();
					}
					json->EndUse();
				}
			}
		}
	}
	if (!succ)
	{
		me->txtParseStatus->SetText(sbErr.ToCString());
		me->txtVerifyStatus->SetText(CSTR(""));
		me->txtIssuer->SetText(CSTR(""));
		me->txtSubject->SetText(CSTR(""));
		me->txtAudience->SetText(CSTR(""));
		me->txtExpTime->SetText(CSTR(""));
		me->txtNotBefore->SetText(CSTR(""));
		me->txtIssueAt->SetText(CSTR(""));
		me->txtJWTId->SetText(CSTR(""));
		me->lvPayload->ClearItems();
	}
	me->txtVerifyType->SetText(Crypto::Token::JWToken::VerifyTypeGetName(me->verifyType));
}

SSWR::AVIRead::AVIRJWTParserForm::AVIRJWTParserForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("JWT Parser"));

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), false);
	NEW_CLASSNN(this->azure, Net::AzureManager(this->core->GetTCPClientFactory(), this->ssl));
	this->token = 0;
	this->verifyType = Crypto::Token::JWToken::VerifyType::Unknown;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->txtJWT = ui->NewTextBox(*this, CSTR(""), true);
	this->txtJWT->SetRect(0, 0, 300, 23, false);
	this->txtJWT->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->hspJWT = ui->NewHSplitter(*this, 3, false);
	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlResult = ui->NewPanel(this->pnlMain);
	this->pnlResult->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnParse = ui->NewButton(this->pnlResult, CSTR("Parse"));
	this->btnParse->SetRect(4, 4, 75, 23, false);
	this->btnParse->HandleButtonClick(OnParseClicked, this);
	this->lblParseStatus = ui->NewLabel(this->pnlResult, CSTR("Parse Status"));
	this->lblParseStatus->SetRect(4, 28, 100, 23, false);
	this->txtParseStatus = ui->NewTextBox(this->pnlResult, CSTR(""));
	this->txtParseStatus->SetRect(104, 28, 300, 23, false);
	this->txtParseStatus->SetReadOnly(true);
	this->lblVerifyType = ui->NewLabel(this->pnlResult, CSTR("Verify Type"));
	this->lblVerifyType->SetRect(4, 52, 100, 23, false);
	this->txtVerifyType = ui->NewTextBox(this->pnlResult, CSTR(""));
	this->txtVerifyType->SetRect(104, 52, 300, 23, false);
	this->txtVerifyType->SetReadOnly(true);
	this->lblVerifyStatus = ui->NewLabel(this->pnlResult, CSTR("Verify Status"));
	this->lblVerifyStatus->SetRect(4, 76, 100, 23, false);
	this->txtVerifyStatus = ui->NewTextBox(this->pnlResult, CSTR(""));
	this->txtVerifyStatus->SetRect(104, 76, 300, 23, false);
	this->txtVerifyStatus->SetReadOnly(true);

	Double y = 100;
	this->lblIssuer = ui->NewLabel(this->pnlResult, CSTR("Issuer"));
	this->lblIssuer->SetRect(4, y, 100, 23, false);
	this->txtIssuer = ui->NewTextBox(this->pnlResult, CSTR(""));
	this->txtIssuer->SetRect(104, y, 200, 23, false);
	this->txtIssuer->SetReadOnly(true);
	y += 24;
	this->lblSubject = ui->NewLabel(this->pnlResult, CSTR("Subject"));
	this->lblSubject->SetRect(4, y, 100, 23, false);
	this->txtSubject = ui->NewTextBox(this->pnlResult, CSTR(""));
	this->txtSubject->SetRect(104, y, 200, 23, false);
	this->txtSubject->SetReadOnly(true);
	y += 24;
	this->lblAudience = ui->NewLabel(this->pnlResult, CSTR("Audience"));
	this->lblAudience->SetRect(4, y, 100, 23, false);
	this->txtAudience = ui->NewTextBox(this->pnlResult, CSTR(""));
	this->txtAudience->SetRect(104, y, 200, 23, false);
	this->txtAudience->SetReadOnly(true);
	y += 24;
	this->lblExpTime = ui->NewLabel(this->pnlResult, CSTR("Expiration Time"));
	this->lblExpTime->SetRect(4, y, 100, 23, false);
	this->txtExpTime = ui->NewTextBox(this->pnlResult, CSTR(""));
	this->txtExpTime->SetRect(104, y, 200, 23, false);
	this->txtExpTime->SetReadOnly(true);
	y += 24;
	this->lblNotBefore = ui->NewLabel(this->pnlResult, CSTR("Not Before"));
	this->lblNotBefore->SetRect(4, y, 100, 23, false);
	this->txtNotBefore = ui->NewTextBox(this->pnlResult, CSTR(""));
	this->txtNotBefore->SetRect(104, y, 200, 23, false);
	this->txtNotBefore->SetReadOnly(true);
	y += 24;
	this->lblIssueAt = ui->NewLabel(this->pnlResult, CSTR("Issued At"));
	this->lblIssueAt->SetRect(4, y, 100, 23, false);
	this->txtIssueAt = ui->NewTextBox(this->pnlResult, CSTR(""));
	this->txtIssueAt->SetRect(104, y, 200, 23, false);
	this->txtIssueAt->SetReadOnly(true);
	y += 24;
	this->lblJWTId = ui->NewLabel(this->pnlResult, CSTR("JWT Id"));
	this->lblJWTId->SetRect(4, y, 100, 23, false);
	this->txtJWTId = ui->NewTextBox(this->pnlResult, CSTR(""));
	this->txtJWTId->SetRect(104, y, 200, 23, false);
	this->txtJWTId->SetReadOnly(true);
	y += 24;
	this->pnlResult->SetRect(0, 0, 100, y + 8, false);

	this->lvPayload = ui->NewListView(this->pnlMain, UI::ListViewStyle::Table, 3);
	this->lvPayload->SetFullRowSelect(true);
	this->lvPayload->SetShowGrid(true);
	this->lvPayload->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPayload->AddColumn(CSTR("Name"), 120);
	this->lvPayload->AddColumn(CSTR("Desc"), 120);
	this->lvPayload->AddColumn(CSTR("Value"), 300);
}

SSWR::AVIRead::AVIRJWTParserForm::~AVIRJWTParserForm()
{
	this->ssl.Delete();
	this->token.Delete();
	this->azure.Delete();
}

void SSWR::AVIRead::AVIRJWTParserForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
