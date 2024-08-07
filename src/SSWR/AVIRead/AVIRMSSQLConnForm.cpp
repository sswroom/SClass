#include "Stdafx.h"
#include "DB/MSSQLConn.h"
#include "SSWR/AVIRead/AVIRMSSQLConnForm.h"
#include "SSWR/AVIRead/AVIRTableMsgForm.h"
#include "Text/MyString.h"
#include "UI/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRMSSQLConnForm::OnPasteJDBCClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMSSQLConnForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMSSQLConnForm>();
	Text::StringBuilderUTF8 sb;
	if (UI::Clipboard::GetString(me->GetHandle(), sb))
	{
		if (sb.StartsWith(UTF8STRC("jdbc:sqlserver://")))
		{
			Text::PString sarr[2];
			Text::PString sarr2[2];
			UOSInt i = Text::StrSplitP(sarr, 2, sb.Substring(17), ';');
			if (Text::StrSplitP(sarr2, 2, sarr[0], ':') == 2)
			{
				me->txtServer->SetText(sarr2[0].ToCString());
				me->txtPort->SetText(sarr2[1].ToCString());
			}
			else
			{
				me->txtServer->SetText(sarr2[0].ToCString());
				me->txtPort->SetText(CSTR("1433"));
			}
			while (i == 2)
			{
				i = Text::StrSplitP(sarr, 2, sarr[1], ';');
				if (sarr[0].StartsWithICase(UTF8STRC("DATABASENAME=")))
				{
					me->txtDatabase->SetText(sarr[0].ToCString().Substring(13));
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMSSQLConnForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMSSQLConnForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMSSQLConnForm>();
	Text::StringBuilderUTF8 sbServer;
	Text::StringBuilderUTF8 sbPort;
	Text::StringBuilderUTF8 sbDatabase;
	Text::StringBuilderUTF8 sbUser;
	Text::StringBuilderUTF8 sbPassword;
	UInt16 port;
	me->txtServer->GetText(sbServer);
	me->txtPort->GetText(sbPort);
	me->txtDatabase->GetText(sbDatabase);
	me->txtUser->GetText(sbUser);
	me->txtPassword->GetText(sbPassword);

	if (sbServer.GetCharCnt() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter server"), CSTR("MSSQL Conn"), me);
		return;
	}	
	if (!sbPort.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid port number"), CSTR("MSSQL Conn"), me);
		return;
	}	
	if (sbDatabase.GetCharCnt() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter database"), CSTR("MSSQL Conn"), me);
		return;
	}	
	if (sbUser.GetCharCnt() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter user"), CSTR("MSSQL Conn"), me);
		return;
	}	
	if (sbPassword.GetCharCnt() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter password"), CSTR("MSSQL Conn"), me);
		return;
	}
	sbPort.ClearStr();
	me->conn = DB::MSSQLConn::OpenConnTCP(sbServer.ToCString(), port, me->chkEncrypt->IsChecked(), sbDatabase.ToCString(), sbUser.ToCString(), sbPassword.ToCString(), me->core->GetLog(), &sbPort);
	if (!me->conn.IsNull())
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		sbUser.ClearStr();
		sbUser.AppendC(UTF8STRC("Error in opening ODBC connection\r\n"));
		sbUser.AppendC(sbPort.ToString(), sbPort.GetLength());
		me->ui->ShowMsgOK(sbUser.ToCString(), CSTR("MSSQL Conn"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRMSSQLConnForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMSSQLConnForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMSSQLConnForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRMSSQLConnForm::AVIRMSSQLConnForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 220, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MSSQL Connection"));
	this->SetNoResize(true);

	this->core = core;
	this->conn = 0;
	this->isError = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblDriver = ui->NewLabel(*this, CSTR("Driver"));
	this->lblDriver->SetRect(4, 4, 100, 23, false);
	this->txtDriver = ui->NewTextBox(*this, CSTR(""));
	this->txtDriver->SetRect(104, 4, 300, 23, false);
	this->txtDriver->SetReadOnly(true);
	this->lblServer = ui->NewLabel(*this, CSTR("Server"));
	this->lblServer->SetRect(4, 28, 100, 23, false);
	this->txtServer = ui->NewTextBox(*this, CSTR("localhost"));
	this->txtServer->SetRect(104, 28, 200, 23, false);
	this->btnPasteJDBC = ui->NewButton(*this, CSTR("Paste from JDBC Str"));
	this->btnPasteJDBC->SetRect(304, 28, 150, 23, false);
	this->btnPasteJDBC->HandleButtonClick(OnPasteJDBCClicked, this);
	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 52, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("1433"));
	this->txtPort->SetRect(104, 52, 100, 23, false);
	this->chkEncrypt = ui->NewCheckBox(*this, CSTR("Encrypt"), false);
	this->chkEncrypt->SetRect(204, 52, 100, 23, false);
	this->lblDatabase = ui->NewLabel(*this, CSTR("Database"));
	this->lblDatabase->SetRect(4, 76, 100, 23, false);
	this->txtDatabase = ui->NewTextBox(*this, CSTR(""));
	this->txtDatabase->SetRect(104, 76, 200, 23, false);
	this->lblUser = ui->NewLabel(*this, CSTR("User"));
	this->lblUser->SetRect(4, 100, 100, 23, false);
	this->txtUser = ui->NewTextBox(*this, CSTR(""));
	this->txtUser->SetRect(104, 100, 200, 23, false);
	this->lblPassword = ui->NewLabel(*this, CSTR("Password"));
	this->lblPassword->SetRect(4, 124, 100, 23, false);
	this->txtPassword = ui->NewTextBox(*this, CSTR(""));
	this->txtPassword->SetRect(104, 124, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(104, 148, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(184, 148, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	NN<Text::String> driverName;
	if (DB::MSSQLConn::GetDriverNameNew().SetTo(driverName))
	{
		this->txtDriver->SetText(driverName->ToCString());
		driverName->Release();
	}
	else
	{
		this->isError = true;
	}
}

SSWR::AVIRead::AVIRMSSQLConnForm::~AVIRMSSQLConnForm()
{
}

void SSWR::AVIRead::AVIRMSSQLConnForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Bool SSWR::AVIRead::AVIRMSSQLConnForm::IsDriverNotFound()
{
	return this->isError;
}

Optional<DB::DBConn> SSWR::AVIRead::AVIRMSSQLConnForm::GetDBConn()
{
	return this->conn;
}
