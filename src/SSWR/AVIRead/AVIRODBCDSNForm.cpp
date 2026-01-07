#include "Stdafx.h"
#include "DB/ODBCConn.h"
#include "SSWR/AVIRead/AVIRODBCDSNForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRODBCDSNForm::OnOKClicked(AnyType userObj)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	NN<SSWR::AVIRead::AVIRODBCDSNForm> me = userObj.GetNN<SSWR::AVIRead::AVIRODBCDSNForm>();
	me->txtDSN->GetText(sb);
	me->txtUID->GetText(sb2);
	me->txtPWD->GetText(sb3);

	DB::ODBCConn *conn;
	NEW_CLASS(conn, DB::ODBCConn(sb.ToCString(), sb2.ToCString(), sb3.ToCString(), nullptr, me->core->GetLog()));
	if (conn->GetConnError() != DB::ODBCConn::CE_NONE)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Error in opening ODBC connection\r\n"));
		conn->GetLastErrorMsg(sb);
		me->ui->ShowMsgOK(sb.ToCString(), CSTR("ODBC DSN Connection"), me);
		DEL_CLASS(conn);
		return;
	}
	me->conn = conn;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRODBCDSNForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRODBCDSNForm> me = userObj.GetNN<SSWR::AVIRead::AVIRODBCDSNForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRODBCDSNForm::AVIRODBCDSNForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 320, 140, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("ODBC DSN Connection"));
	this->SetNoResize(true);

	this->core = core;
	this->conn = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblDSN = ui->NewLabel(*this, CSTR("DSN"));
	this->lblDSN->SetRect(4, 4, 100, 23, false);
	this->txtDSN = ui->NewTextBox(*this, CSTR(""));
	this->txtDSN->SetRect(104, 4, 200, 23, false);
	this->lblUID = ui->NewLabel(*this, CSTR("User Name"));
	this->lblUID->SetRect(4, 28, 100, 23, false);
	this->txtUID = ui->NewTextBox(*this, CSTR(""));
	this->txtUID->SetRect(104, 28, 200, 23, false);
	this->lblPWD = ui->NewLabel(*this, CSTR("Password"));
	this->lblPWD->SetRect(4, 52, 100, 23, false);
	this->txtPWD = ui->NewTextBox(*this, CSTR(""));
	this->txtPWD->SetRect(104, 52, 200, 23, false);
	this->txtPWD->SetPasswordChar('*');
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(104, 80, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
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

Optional<DB::DBConn> SSWR::AVIRead::AVIRODBCDSNForm::GetDBConn()
{
	return this->conn;
}
