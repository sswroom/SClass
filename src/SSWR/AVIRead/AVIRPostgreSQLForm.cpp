#include "Stdafx.h"
#include "DB/PostgreSQLConn.h"
#include "SSWR/AVIRead/AVIRPostgreSQLForm.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRPostgreSQLForm::OnOKClicked(void *userObj)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	Text::StringBuilderUTF8 sb4;
	Text::StringBuilderUTF8 sbPort;
	SSWR::AVIRead::AVIRPostgreSQLForm *me = (SSWR::AVIRead::AVIRPostgreSQLForm*)userObj;
	me->txtServer->GetText(&sb);
	me->txtUID->GetText(&sb2);
	me->txtPWD->GetText(&sb3);
	me->txtDatabase->GetText(&sb4);
	me->txtPort->GetText(&sbPort);
	UInt16 port;

	DB::PostgreSQLConn *conn;
	if (!sbPort.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog(CSTR("Port is not valid"), CSTR("PostgreSQL Connection"), me);
		return;
	}
	NEW_CLASS(conn, DB::PostgreSQLConn(sb.ToCString(), port, sb2.ToCString(), sb3.ToCString(), sb4.ToCString(), me->core->GetLog()));
	if (conn->IsConnError())
	{
		DEL_CLASS(conn);
		UI::MessageDialog::ShowDialog(CSTR("Error in opening PostgreSQL connection"), CSTR("PostgreSQL Connection"), me);
		return;
	}
	me->conn = conn;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRPostgreSQLForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRPostgreSQLForm *me = (SSWR::AVIRead::AVIRPostgreSQLForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRPostgreSQLForm::AVIRPostgreSQLForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 340, 188, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("PostgreSQL Connection"));

	this->core = core;
	this->conn = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetNoResize(true);

	NEW_CLASS(this->lblServer, UI::GUILabel(ui, this, CSTR("Server")));
	this->lblServer->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtServer, UI::GUITextBox(ui, this, CSTR("127.0.0.1")));
	this->txtServer->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, CSTR("Port")));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, CSTR("5432")));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblUID, UI::GUILabel(ui, this, CSTR("User Name")));
	this->lblUID->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtUID, UI::GUITextBox(ui, this, CSTR("postgre")));
	this->txtUID->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->lblPWD, UI::GUILabel(ui, this, CSTR("Password")));
	this->lblPWD->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtPWD, UI::GUITextBox(ui, this, CSTR("")));
	this->txtPWD->SetRect(104, 76, 200, 23, false);
	this->txtPWD->SetPasswordChar('*');
	NEW_CLASS(this->lblDatabase, UI::GUILabel(ui, this, CSTR("Database")));
	this->lblDatabase->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtDatabase, UI::GUITextBox(ui, this, CSTR("")));
	this->txtDatabase->SetRect(104, 100, 200, 23, false);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(104, 128, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("Cancel")));
	this->btnCancel->SetRect(184, 128, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->txtServer->Focus();
}

SSWR::AVIRead::AVIRPostgreSQLForm::~AVIRPostgreSQLForm()
{
}

void SSWR::AVIRead::AVIRPostgreSQLForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

DB::DBConn *SSWR::AVIRead::AVIRPostgreSQLForm::GetDBConn()
{
	return this->conn;
}