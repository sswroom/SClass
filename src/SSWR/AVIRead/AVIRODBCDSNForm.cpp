#include "Stdafx.h"
#include "DB/ODBCConn.h"
#include "SSWR/AVIRead/AVIRODBCDSNForm.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRODBCDSNForm::OnOKClicked(void *userObj)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	SSWR::AVIRead::AVIRODBCDSNForm *me = (SSWR::AVIRead::AVIRODBCDSNForm*)userObj;
	me->txtDSN->GetText(&sb);
	me->txtUID->GetText(&sb2);
	me->txtPWD->GetText(&sb3);

	DB::ODBCConn *conn;
	NEW_CLASS(conn, DB::ODBCConn(sb.ToCString(), sb2.ToCString(), sb3.ToCString(), CSTR_NULL, me->core->GetLog()));
	if (conn->GetConnError() != DB::ODBCConn::CE_NONE)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Error in opening ODBC connection\r\n"));
		conn->GetErrorMsg(&sb);
		UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("ODBC DSN Connection"), me);
		DEL_CLASS(conn);
		return;
	}
	me->conn = conn;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRODBCDSNForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRODBCDSNForm *me = (SSWR::AVIRead::AVIRODBCDSNForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRODBCDSNForm::AVIRODBCDSNForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 320, 140, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("ODBC DSN Connection"));
	this->SetNoResize(true);

	this->core = core;
	this->conn = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblDSN, UI::GUILabel(ui, this, CSTR("DSN")));
	this->lblDSN->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtDSN, UI::GUITextBox(ui, this, CSTR("")));
	this->txtDSN->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblUID, UI::GUILabel(ui, this, CSTR("User Name")));
	this->lblUID->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtUID, UI::GUITextBox(ui, this, CSTR("")));
	this->txtUID->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblPWD, UI::GUILabel(ui, this, CSTR("Password")));
	this->lblPWD->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtPWD, UI::GUITextBox(ui, this, CSTR("")));
	this->txtPWD->SetRect(104, 52, 200, 23, false);
	this->txtPWD->SetPasswordChar('*');
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(104, 80, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("Cancel")));
	this->btnCancel->SetRect(184, 80, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->txtDSN->Focus();
}

SSWR::AVIRead::AVIRODBCDSNForm::~AVIRODBCDSNForm()
{
}

void SSWR::AVIRead::AVIRODBCDSNForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

DB::DBConn *SSWR::AVIRead::AVIRODBCDSNForm::GetDBConn()
{
	return this->conn;
}
