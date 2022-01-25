#include "Stdafx.h"
#include "DB/MSSQLConn.h"
#include "SSWR/AVIRead/AVIRMSSQLConnForm.h"
#include "SSWR/AVIRead/AVIRTableMsgForm.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRMSSQLConnForm::OnPasteJDBCClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMSSQLConnForm *me = (SSWR::AVIRead::AVIRMSSQLConnForm*)userObj;
	Text::StringBuilderUTF8 sb;
	if (Win32::Clipboard::GetString(me->GetHandle(), &sb))
	{
		if (sb.StartsWith(UTF8STRC("jdbc:sqlserver://")))
		{
			Text::PString sarr[2];
			Text::PString sarr2[2];
			UOSInt i = Text::StrSplitP(sarr, 2, sb.ToString() + 17, sb.GetLength() - 17, ';');
			if (Text::StrSplitP(sarr2, 2, sarr[0].v, sarr[0].leng, ':') == 2)
			{
				me->txtServer->SetText(sarr2[0].v);
				me->txtPort->SetText(sarr2[1].v);
			}
			else
			{
				me->txtServer->SetText(sarr2[0].v);
				me->txtPort->SetText((const UTF8Char*)"1433");
			}
			while (i == 2)
			{
				i = Text::StrSplitP(sarr, 2, sarr[1].v, sarr[1].leng, ';');
				if (Text::StrStartsWithICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("DATABASENAME=")))
				{
					me->txtDatabase->SetText(sarr[0].v + 13);
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMSSQLConnForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMSSQLConnForm *me = (SSWR::AVIRead::AVIRMSSQLConnForm*)userObj;
	Text::StringBuilderUTF8 sbServer;
	Text::StringBuilderUTF8 sbPort;
	Text::StringBuilderUTF8 sbDatabase;
	Text::StringBuilderUTF8 sbUser;
	Text::StringBuilderUTF8 sbPassword;
	UInt16 port;
	me->txtServer->GetText(&sbServer);
	me->txtPort->GetText(&sbPort);
	me->txtDatabase->GetText(&sbDatabase);
	me->txtUser->GetText(&sbUser);
	me->txtPassword->GetText(&sbPassword);

	if (sbServer.GetCharCnt() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter server", (const UTF8Char*)"MSSQL Conn", me);
		return;
	}	
	if (!sbPort.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid port number", (const UTF8Char*)"MSSQL Conn", me);
		return;
	}	
	if (sbDatabase.GetCharCnt() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter database", (const UTF8Char*)"MSSQL Conn", me);
		return;
	}	
	if (sbUser.GetCharCnt() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter user", (const UTF8Char*)"MSSQL Conn", me);
		return;
	}	
	if (sbPassword.GetCharCnt() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter password", (const UTF8Char*)"MSSQL Conn", me);
		return;
	}
	sbPort.ClearStr();
	me->conn = DB::MSSQLConn::OpenConnTCP(sbServer.ToString(), port, sbDatabase.ToString(), sbUser.ToString(), sbPassword.ToString(), me->core->GetLog(), &sbPort);
	if (me->conn)
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		sbUser.ClearStr();
		sbUser.AppendC(UTF8STRC("Error in opening ODBC connection\r\n"));
		sbUser.AppendC(sbPort.ToString(), sbPort.GetLength());
		UI::MessageDialog::ShowDialog(sbUser.ToString(), (const UTF8Char*)"MSSQL Conn", me);
	}
}

void __stdcall SSWR::AVIRead::AVIRMSSQLConnForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMSSQLConnForm *me = (SSWR::AVIRead::AVIRMSSQLConnForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRMSSQLConnForm::AVIRMSSQLConnForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 200, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"MSSQL Connection");
	this->SetNoResize(true);

	this->core = core;
	this->conn = 0;
	this->isError = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblDriver, UI::GUILabel(ui, this, (const UTF8Char*)"Driver"));
	this->lblDriver->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtDriver, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtDriver->SetRect(104, 4, 300, 23, false);
	this->txtDriver->SetReadOnly(true);
	NEW_CLASS(this->lblServer, UI::GUILabel(ui, this, (const UTF8Char*)"Server"));
	this->lblServer->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtServer, UI::GUITextBox(ui, this, (const UTF8Char*)"localhost"));
	this->txtServer->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->btnPasteJDBC, UI::GUIButton(ui, this, (const UTF8Char*)"Paste from JDBC Str"));
	this->btnPasteJDBC->SetRect(304, 28, 150, 23, false);
	this->btnPasteJDBC->HandleButtonClick(OnPasteJDBCClicked, this);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, (const UTF8Char*)"1433"));
	this->txtPort->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->lblDatabase, UI::GUILabel(ui, this, (const UTF8Char*)"Database"));
	this->lblDatabase->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtDatabase, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtDatabase->SetRect(104, 76, 200, 23, false);
	NEW_CLASS(this->lblUser, UI::GUILabel(ui, this, (const UTF8Char*)"User"));
	this->lblUser->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtUser, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtUser->SetRect(104, 100, 200, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this, (const UTF8Char*)"Password"));
	this->lblPassword->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtPassword->SetRect(104, 124, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, (const UTF8Char*)"OK"));
	this->btnOK->SetRect(104, 148, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"Cancel"));
	this->btnCancel->SetRect(184, 148, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	Text::String *driverName = DB::MSSQLConn::GetDriverNameNew();
	if (driverName)
	{
		this->txtDriver->SetText(driverName->v);
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

DB::DBConn *SSWR::AVIRead::AVIRMSSQLConnForm::GetDBConn()
{
	return this->conn;
}
