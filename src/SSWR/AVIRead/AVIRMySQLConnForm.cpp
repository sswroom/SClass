#include "Stdafx.h"
#include "Net/MySQLTCPClient.h"
#include "SSWR/AVIRead/AVIRMySQLConnForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRMySQLConnForm::OnOKClicked(AnyType userObj)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	Text::StringBuilderUTF8 sb4;
	Text::StringBuilderUTF8 sbPort;
	NotNullPtr<SSWR::AVIRead::AVIRMySQLConnForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMySQLConnForm>();
	me->txtServer->GetText(sb);
	me->txtUID->GetText(sb2);
	me->txtPWD->GetText(sb3);
	me->txtDatabase->GetText(sb4);
	me->txtPort->GetText(sbPort);
	UInt16 port;

	Net::MySQLTCPClient *conn;
	NotNullPtr<Net::SocketFactory> sockf = me->core->GetSocketFactory();
	Net::SocketUtil::AddressInfo addr;
	if (!sbPort.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Port is not valid"), CSTR("MySQL Connection"), me);
		return;
	}
	else if (!sockf->DNSResolveIP(sb.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Error in resolving server host"), CSTR("MySQL Connection"), me);
		return;
	}
	NEW_CLASS(conn, Net::MySQLTCPClient(sockf, addr, port, sb2.ToCString(), sb3.ToCString(), sb4.ToCString()));
	if (conn->IsError())
	{
		DEL_CLASS(conn);
		me->ui->ShowMsgOK(CSTR("Error in opening MySQL connection"), CSTR("MySQL Connection"), me);
		return;
	}
	me->conn = conn;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRMySQLConnForm::OnCancelClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRMySQLConnForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMySQLConnForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRMySQLConnForm::AVIRMySQLConnForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 340, 188, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MySQL Connection"));

	this->core = core;
	this->conn = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetNoResize(true);

	this->lblServer = ui->NewLabel(*this, CSTR("Server"));
	this->lblServer->SetRect(4, 4, 100, 23, false);
	this->txtServer = ui->NewTextBox(*this, CSTR("127.0.0.1"));
	this->txtServer->SetRect(104, 4, 200, 23, false);
	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("3306"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	this->lblUID = ui->NewLabel(*this, CSTR("User Name"));
	this->lblUID->SetRect(4, 52, 100, 23, false);
	this->txtUID = ui->NewTextBox(*this, CSTR(""));
	this->txtUID->SetRect(104, 52, 200, 23, false);
	this->lblPWD = ui->NewLabel(*this, CSTR("Password"));
	this->lblPWD->SetRect(4, 76, 100, 23, false);
	this->txtPWD = ui->NewTextBox(*this, CSTR(""));
	this->txtPWD->SetRect(104, 76, 200, 23, false);
	this->txtPWD->SetPasswordChar('*');
	this->lblDatabase = ui->NewLabel(*this, CSTR("Database"));
	this->lblDatabase->SetRect(4, 100, 100, 23, false);
	this->txtDatabase = ui->NewTextBox(*this, CSTR(""));
	this->txtDatabase->SetRect(104, 100, 200, 23, false);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(104, 128, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(184, 128, 75, 23, false);
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
