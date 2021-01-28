#include "Stdafx.h"
#include "Net/MySQLTCPClient.h"
#include "SSWR/AVIRead/AVIRMySQLConnForm.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRMySQLConnForm::OnOKClicked(void *userObj)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	Text::StringBuilderUTF8 sb4;
	SSWR::AVIRead::AVIRMySQLConnForm *me = (SSWR::AVIRead::AVIRMySQLConnForm*)userObj;
	me->txtServer->GetText(&sb);
	me->txtUID->GetText(&sb2);
	me->txtPWD->GetText(&sb3);
	me->txtDatabase->GetText(&sb4);

	Net::MySQLTCPClient *conn;
	Net::SocketFactory *sockf = me->core->GetSocketFactory();
	Net::SocketUtil::AddressInfo addr;
	if (!sockf->DNSResolveIP(sb.ToString(), &addr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in resolving server host", (const UTF8Char *)"MySQL Connection", me);
		return;
	}
	NEW_CLASS(conn, Net::MySQLTCPClient(sockf, &addr, 3306, sb2.ToString(), sb3.ToString(), sb4.ToString()));
	if (conn->IsError())
	{
		DEL_CLASS(conn);
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in opening MySQL connection", (const UTF8Char *)"MySQL Connection", me);
		return;
	}
	me->conn = conn;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRMySQLConnForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMySQLConnForm *me = (SSWR::AVIRead::AVIRMySQLConnForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRMySQLConnForm::AVIRMySQLConnForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 320, 140, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"MySQL Connection");
	this->SetNoResize(true);

	this->core = core;
	this->conn = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblServer, UI::GUILabel(ui, this, (const UTF8Char*)"Server"));
	this->lblServer->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtServer, UI::GUITextBox(ui, this, (const UTF8Char*)"127.0.0.1"));
	this->txtServer->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblUID, UI::GUILabel(ui, this, (const UTF8Char*)"User Name"));
	this->lblUID->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtUID, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtUID->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblPWD, UI::GUILabel(ui, this, (const UTF8Char*)"Password"));
	this->lblPWD->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtPWD, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtPWD->SetRect(104, 52, 200, 23, false);
	this->txtPWD->SetPasswordChar('*');
	NEW_CLASS(this->lblDatabase, UI::GUILabel(ui, this, (const UTF8Char*)"Database"));
	this->lblDatabase->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtDatabase, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtDatabase->SetRect(104, 76, 200, 23, false);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, (const UTF8Char*)"OK"));
	this->btnOK->SetRect(104, 104, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"Cancel"));
	this->btnCancel->SetRect(184, 104, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->txtServer->Focus();
}

SSWR::AVIRead::AVIRMySQLConnForm::~AVIRMySQLConnForm()
{
}

void SSWR::AVIRead::AVIRMySQLConnForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

DB::DBConn *SSWR::AVIRead::AVIRMySQLConnForm::GetDBConn()
{
	return this->conn;
}
