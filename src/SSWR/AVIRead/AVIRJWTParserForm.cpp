#include "Stdafx.h"
#include "Crypto/Token/JWTHandler.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRJWTParserForm.h"

void __stdcall SSWR::AVIRead::AVIRJWTParserForm::OnParseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRJWTParserForm *me = (SSWR::AVIRead::AVIRJWTParserForm*)userObj;
	Text::StringBuilderUTF8 sbJWT;
	Text::StringBuilderUTF8 sbErr;
	Int64 t;
	me->txtJWT->GetText(sbJWT);
	Crypto::Token::JWTHandler jwt(me->ssl);
	Crypto::Token::JWTParam param;
	Data::StringMap<Text::String*> *result = jwt.Parse(sbJWT.ToCString(), param, me->chkIgnoreSignValid->IsChecked(), &sbErr);
	if (result)
	{
		me->txtStatus->SetText(CSTR("Success"));
		me->txtIssuer->SetText(Text::String::OrEmpty(param.GetIssuer())->ToCString());
		me->txtSubject->SetText(Text::String::OrEmpty(param.GetSubject())->ToCString());
		me->txtAudience->SetText(Text::String::OrEmpty(param.GetAudience())->ToCString());
		if ((t = param.GetExpirationTime()) != 0)
		{
			sbErr.ClearStr();
			sbErr.AppendTS(Data::Timestamp(Data::TimeInstant(t, 0), Data::DateTimeUtil::GetLocalTzQhr()));
			me->txtExpTime->SetText(sbErr.ToCString());
		}
		else
		{
			me->txtExpTime->SetText(CSTR("-"));
		}
		if ((t = param.GetNotBefore()) != 0)
		{
			sbErr.ClearStr();
			sbErr.AppendTS(Data::Timestamp(Data::TimeInstant(t, 0), Data::DateTimeUtil::GetLocalTzQhr()));
			me->txtNotBefore->SetText(sbErr.ToCString());
		}
		else
		{
			me->txtNotBefore->SetText(CSTR("-"));
		}
		if ((t = param.GetIssuedAt()) != 0)
		{
			sbErr.ClearStr();
			sbErr.AppendTS(Data::Timestamp(Data::TimeInstant(t, 0), Data::DateTimeUtil::GetLocalTzQhr()));
			me->txtIssueAt->SetText(sbErr.ToCString());
		}
		else
		{
			me->txtIssueAt->SetText(CSTR("-"));
		}
		me->txtJWTId->SetText(Text::String::OrEmpty(param.GetJWTId())->ToCString());

		me->lvPayload->ClearItems();
		NotNullPtr<Text::String> name;
		UOSInt i = 0;
		UOSInt j = result->GetCount();
		while (i < j)
		{
			name = Text::String::OrEmpty(result->GetKey(i));
			me->lvPayload->AddItem(name, 0);
			me->lvPayload->SetSubItem(i, 1, Crypto::Token::JWTHandler::PayloadName(name->ToCString()));
			me->lvPayload->SetSubItem(i, 2, Text::String::OrEmpty(result->GetItem(i)));
			i++;
		}
		jwt.FreeResult(result);
	}
	else
	{
		me->txtStatus->SetText(sbErr.ToCString());
		me->txtIssuer->SetText(CSTR(""));
		me->txtSubject->SetText(CSTR(""));
		me->txtAudience->SetText(CSTR(""));
		me->txtExpTime->SetText(CSTR(""));
		me->txtNotBefore->SetText(CSTR(""));
		me->txtIssueAt->SetText(CSTR(""));
		me->txtJWTId->SetText(CSTR(""));
		me->lvPayload->ClearItems();
	}
}

SSWR::AVIRead::AVIRJWTParserForm::AVIRJWTParserForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("JWT Parser"));

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->txtJWT, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtJWT->SetRect(0, 0, 300, 23, false);
	this->txtJWT->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->hspJWT, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlMain, UI::GUIPanel(ui, this));
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlResult, UI::GUIPanel(ui, this->pnlMain));
	this->pnlResult->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnParse, UI::GUIButton(ui, this->pnlResult, CSTR("Parse")));
	this->btnParse->SetRect(4, 4, 75, 23, false);
	this->btnParse->HandleButtonClick(OnParseClicked, this);
	NEW_CLASS(this->chkIgnoreSignValid, UI::GUICheckBox(ui, this->pnlResult, CSTR("Ignore Signature Validation"), false));
	this->chkIgnoreSignValid->SetRect(84, 4, 200, 23, false);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this->pnlResult, CSTR("Status")));
	this->lblStatus->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this->pnlResult, CSTR("")));
	this->txtStatus->SetRect(104, 28, 300, 23, false);
	this->txtStatus->SetReadOnly(true);

	Double y = 76;
	NEW_CLASS(this->lblIssuer, UI::GUILabel(ui, this->pnlResult, CSTR("Issuer")));
	this->lblIssuer->SetRect(4, y, 100, 23, false);
	NEW_CLASS(this->txtIssuer, UI::GUITextBox(ui, this->pnlResult, CSTR("")));
	this->txtIssuer->SetRect(104, y, 200, 23, false);
	this->txtIssuer->SetReadOnly(true);
	y += 24;
	NEW_CLASS(this->lblSubject, UI::GUILabel(ui, this->pnlResult, CSTR("Subject")));
	this->lblSubject->SetRect(4, y, 100, 23, false);
	NEW_CLASS(this->txtSubject, UI::GUITextBox(ui, this->pnlResult, CSTR("")));
	this->txtSubject->SetRect(104, y, 200, 23, false);
	this->txtSubject->SetReadOnly(true);
	y += 24;
	NEW_CLASS(this->lblAudience, UI::GUILabel(ui, this->pnlResult, CSTR("Audience")));
	this->lblAudience->SetRect(4, y, 100, 23, false);
	NEW_CLASS(this->txtAudience, UI::GUITextBox(ui, this->pnlResult, CSTR("")));
	this->txtAudience->SetRect(104, y, 200, 23, false);
	this->txtAudience->SetReadOnly(true);
	y += 24;
	NEW_CLASS(this->lblExpTime, UI::GUILabel(ui, this->pnlResult, CSTR("Expiration Time")));
	this->lblExpTime->SetRect(4, y, 100, 23, false);
	NEW_CLASS(this->txtExpTime, UI::GUITextBox(ui, this->pnlResult, CSTR("")));
	this->txtExpTime->SetRect(104, y, 200, 23, false);
	this->txtExpTime->SetReadOnly(true);
	y += 24;
	NEW_CLASS(this->lblNotBefore, UI::GUILabel(ui, this->pnlResult, CSTR("Not Before")));
	this->lblNotBefore->SetRect(4, y, 100, 23, false);
	NEW_CLASS(this->txtNotBefore, UI::GUITextBox(ui, this->pnlResult, CSTR("")));
	this->txtNotBefore->SetRect(104, y, 200, 23, false);
	this->txtNotBefore->SetReadOnly(true);
	y += 24;
	NEW_CLASS(this->lblIssueAt, UI::GUILabel(ui, this->pnlResult, CSTR("Issued At")));
	this->lblIssueAt->SetRect(4, y, 100, 23, false);
	NEW_CLASS(this->txtIssueAt, UI::GUITextBox(ui, this->pnlResult, CSTR("")));
	this->txtIssueAt->SetRect(104, y, 200, 23, false);
	this->txtIssueAt->SetReadOnly(true);
	y += 24;
	NEW_CLASS(this->lblJWTId, UI::GUILabel(ui, this->pnlResult, CSTR("JWT Id")));
	this->lblJWTId->SetRect(4, y, 100, 23, false);
	NEW_CLASS(this->txtJWTId, UI::GUITextBox(ui, this->pnlResult, CSTR("")));
	this->txtJWTId->SetRect(104, y, 200, 23, false);
	this->txtJWTId->SetReadOnly(true);
	y += 24;
	this->pnlResult->SetRect(0, 0, 100, y + 8, false);

	NEW_CLASS(this->lvPayload, UI::GUIListView(ui, this->pnlMain, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvPayload->SetFullRowSelect(true);
	this->lvPayload->SetShowGrid(true);
	this->lvPayload->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPayload->AddColumn(CSTR("Name"), 120);
	this->lvPayload->AddColumn(CSTR("Desc"), 120);
	this->lvPayload->AddColumn(CSTR("Value"), 300);
}

SSWR::AVIRead::AVIRJWTParserForm::~AVIRJWTParserForm()
{
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRJWTParserForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
