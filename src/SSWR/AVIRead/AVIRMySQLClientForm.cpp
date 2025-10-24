#include "Stdafx.h"
#include "DB/ColDef.h"
#include "IO/Path.h"
#include "Math/Math_C.h"
#include "SSWR/AVIRead/AVIRMySQLClientForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRMySQLClientForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMySQLClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMySQLClientForm>();
	NN<Net::MySQLTCPClient> cli;
	if (me->cli.SetTo(cli))
	{
		cli.Delete();
		me->cli = 0;
		me->txtHost->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		me->txtUserName->SetReadOnly(false);
		me->txtPassword->SetReadOnly(false);
		me->txtDatabase->SetReadOnly(false);
		me->txtStatus->SetText(CSTR("Disconnected"));
		return;
	}
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	Text::StringBuilderUTF8 sbUser;
	Text::StringBuilderUTF8 sbPwd;
	Text::StringBuilderUTF8 sbDatabase;
	Text::CString sDatabase;
	me->txtHost->GetText(sbUser);
	me->txtPort->GetText(sbPwd);
	if (sbUser.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter the host"), CSTR("MySQL Client"), me);
		me->txtHost->Focus();
		return;
	}
	if (sbPwd.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter the port"), CSTR("MySQL Client"), me);
		me->txtPort->Focus();
		return;
	}
	if (!me->core->GetSocketFactory()->DNSResolveIP(sbUser.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Error in resolving host"), CSTR("MySQL Client"), me);
		me->txtHost->Focus();
		return;
	}
	if (!Text::StrToUInt16(sbPwd.ToString(), port))
	{
		me->ui->ShowMsgOK(CSTR("Port invalid"), CSTR("MySQL Client"), me);
		me->txtHost->Focus();
		return;
	}
	sbUser.ClearStr();
	sbPwd.ClearStr();
	me->txtUserName->GetText(sbUser);
	me->txtPassword->GetText(sbPwd);
	me->txtDatabase->GetText(sbDatabase);
	if (sbUser.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter the user name"), CSTR("MySQL Client"), me);
		me->txtUserName->Focus();
		return;
	}
	if (sbPwd.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter the password"), CSTR("MySQL Client"), me);
		me->txtPassword->Focus();
		return;
	}
	if (sbDatabase.GetLength() > 0)
	{
		sDatabase = sbDatabase.ToCString();
	}
	else
	{
		sDatabase = nullptr;
	}
	
	me->cliConnected = false;
	NEW_CLASSNN(cli, Net::MySQLTCPClient(me->core->GetTCPClientFactory(), addr, port, sbUser.ToCString(), sbPwd.ToCString(), sDatabase));
	if (cli->IsError())
	{
		sbUser.ClearStr();
		sbUser.AppendC(UTF8STRC("Error in connecting to server: "));
		cli->GetLastErrorMsg(sbUser);
		cli.Delete();
		me->ui->ShowMsgOK(sbUser.ToCString(), CSTR("MySQL Client"), me);
	}
	else
	{
		me->cli = cli;
		me->txtHost->SetReadOnly(true);
		me->txtPort->SetReadOnly(true);
		me->txtUserName->SetReadOnly(true);
		me->txtPassword->SetReadOnly(true);
		me->txtDatabase->SetReadOnly(true);
	}
}

void __stdcall SSWR::AVIRead::AVIRMySQLClientForm::OnQueryClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMySQLClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMySQLClientForm>();
	NN<Net::MySQLTCPClient> cli;
	if (!me->cli.SetTo(cli))
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	me->txtQuery->GetText(sb);
	if (sb.GetLength() == 0)
	{
		return;
	}
	NN<DB::DBReader> reader;
	if (cli->ExecuteReader(sb.ToCString()).SetTo(reader))
	{
		me->UpdateResult(reader);
		cli->CloseReader(reader);
	}
	else
	{
		sb.ClearStr();
		cli->GetLastErrorMsg(sb);
		me->txtQueryStatus->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRMySQLClientForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMySQLClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMySQLClientForm>();
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UInt8 buff[48];
	UOSInt i;
	NN<Text::String> s;
	NN<Net::MySQLTCPClient> cli;
	if (me->cli.SetTo(cli))
	{
		if (!me->cliConnected && cli->ServerInfoRecv())
		{
			me->cliConnected = true;
			s = Text::String::OrEmpty(cli->GetServerVer());
			me->txtServerVer->SetText(s->ToCString());
			sptr = Text::StrUInt32(sbuff, cli->GetConnId());
			me->txtConnId->SetText(CSTRP(sbuff, sptr));
			i = cli->GetAuthPluginData(buff);
			sptr = Text::StrHexBytes(sbuff, buff, i, ' ');
			me->txtAuthPluginData->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), cli->GetServerCap());
			me->txtServerCap->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, cli->GetServerCS());
			me->txtServerCS->SetText(CSTRP(sbuff, sptr));
			me->txtStatus->SetText(CSTR("Connected"));
		}
		if (cli->IsError())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Disconnected: "));
			cli->GetLastErrorMsg(sb);
			cli.Delete();
			me->cli = 0;
			me->txtHost->SetReadOnly(false);
			me->txtPort->SetReadOnly(false);
			me->txtUserName->SetReadOnly(false);
			me->txtPassword->SetReadOnly(false);
			me->txtDatabase->SetReadOnly(false);
			me->txtStatus->SetText(sb.ToCString());
		}
	}
}


void SSWR::AVIRead::AVIRMySQLClientForm::UpdateResult(NN<DB::DBReader> r)
{
	OSInt rowChg;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt *colSize;

	rowChg = r->GetRowChanged();
	if (rowChg == -1)
	{
		this->txtQueryStatus->SetText(CSTR(""));
		this->lvQueryResult->ClearAll();

		Text::StringBuilderUTF8 sb;
		DB::ColDef col(CSTR(""));
		j = r->ColCount();
		this->lvQueryResult->ChangeColumnCnt(j);
		i = 0;
		colSize = MemAlloc(UOSInt, j);
		while (i < j)
		{
			if (r->GetColDef(i, col))
			{
				this->lvQueryResult->AddColumn(col.GetColName(), 100);
			}
			else
			{
				this->lvQueryResult->AddColumn(CSTR("Unnamed"), 100);
			}
			colSize[i] = 0;
			i++;
		}

		OSInt rowCnt = 0;
		while (r->ReadNext())
		{
			i = 1;
			sb.ClearStr();
			r->GetStr(0, sb);
			if (sb.GetLength() > colSize[0])
				colSize[0] = sb.GetLength();
			k = this->lvQueryResult->AddItem(sb.ToCString(), 0);
			while (i < j)
			{
				sb.ClearStr();
				r->GetStr(i, sb);
				this->lvQueryResult->SetSubItem(k, i, sb.ToCString());

				if (sb.GetLength() > colSize[i])
					colSize[i] = sb.GetLength();
				i++;
			}
			rowCnt++;
			if (rowCnt > 1000)
				break;
		}

		k = 0;
		i = j;
		while (i-- > 0)
		{
			k += colSize[i];
		}
		if (k > 0)
		{
			Double w = this->lvQueryResult->GetSize().x;
			w -= UOSInt2Double(20 + j * 6);
			if (w < 0)
				w = 0;
			i = 0;
			while (i < j)
			{
				this->lvQueryResult->SetColumnWidth(i, (UOSInt2Double(colSize[i]) * w / UOSInt2Double(k) + 6));
				i++;
			}
		}
		MemFree(colSize);
	}
	else
	{
		this->lvQueryResult->ClearItems();
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Record changed = "));
		sb.AppendOSInt(rowChg);
		this->txtQueryStatus->SetText(sb.ToCString());
	}
}

SSWR::AVIRead::AVIRMySQLClientForm::AVIRMySQLClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("MySQL Client"));
	this->SetFont(0, 0, 8.25, false);
	this->cli = 0;

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->lblHost = ui->NewLabel(this->tpControl, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(this->tpControl, CSTR("127.0.0.1"));
	this->txtHost->SetRect(104, 4, 200, 23, false);
	this->lblPort = ui->NewLabel(this->tpControl, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->tpControl, CSTR("3306"));
	this->txtPort->SetRect(104, 28, 200, 23, false);
	this->lblUserName = ui->NewLabel(this->tpControl, CSTR("User Name"));
	this->lblUserName->SetRect(4, 52, 100, 23, false);
	this->txtUserName = ui->NewTextBox(this->tpControl, CSTR("root"));
	this->txtUserName->SetRect(104, 52, 200, 23, false);
	this->lblPassword = ui->NewLabel(this->tpControl, CSTR("Password"));
	this->lblPassword->SetRect(4, 76, 100, 23, false);
	this->txtPassword = ui->NewTextBox(this->tpControl, CSTR(""));
	this->txtPassword->SetRect(104, 76, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	this->lblDatabase = ui->NewLabel(this->tpControl, CSTR("Database"));
	this->lblDatabase->SetRect(4, 100, 100, 23, false);
	this->txtDatabase = ui->NewTextBox(this->tpControl, CSTR(""));
	this->txtDatabase->SetRect(104, 100, 200, 23, false);
	this->btnStart = ui->NewButton(this->tpControl, CSTR("Start"));
	this->btnStart->SetRect(104, 124, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lblStatus = ui->NewLabel(this->tpControl, CSTR("Status"));
	this->lblStatus->SetRect(4, 148, 100, 23, false);
	this->txtStatus = ui->NewTextBox(this->tpControl, CSTR("Not connected"));
	this->txtStatus->SetRect(104, 148, 150, 23, false);
	this->txtStatus->SetReadOnly(true);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->lblServerVer = ui->NewLabel(this->tpInfo, CSTR("Server Ver"));
	this->lblServerVer->SetRect(4, 4, 100, 23, false);
	this->txtServerVer = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtServerVer->SetRect(104, 4, 250, 23, false);
	this->txtServerVer->SetReadOnly(true);
	this->lblConnId = ui->NewLabel(this->tpInfo, CSTR("Conn Id"));
	this->lblConnId->SetRect(4, 28, 100, 23, false);
	this->txtConnId = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtConnId->SetRect(104, 28, 150, 23, false);
	this->txtConnId->SetReadOnly(true);
	this->lblAuthPluginData = ui->NewLabel(this->tpInfo, CSTR("Auth Data"));
	this->lblAuthPluginData->SetRect(4, 52, 100, 23, false);
	this->txtAuthPluginData = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtAuthPluginData->SetRect(104, 52, 400, 23, false);
	this->txtAuthPluginData->SetReadOnly(true);
	this->lblServerCap = ui->NewLabel(this->tpInfo, CSTR("Capability"));
	this->lblServerCap->SetRect(4, 76, 100, 23, false);
	this->txtServerCap = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtServerCap->SetRect(104, 76, 120, 23, false);
	this->txtServerCap->SetReadOnly(true);
	this->lblServerCS = ui->NewLabel(this->tpInfo, CSTR("CharacterSet"));
	this->lblServerCS->SetRect(4, 100, 100, 23, false);
	this->txtServerCS = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtServerCS->SetRect(104, 100, 100, 23, false);
	this->txtServerCS->SetReadOnly(true);

	tpQuery = this->tcMain->AddTabPage(CSTR("Query"));
	this->pnlQuery = ui->NewPanel(this->tpQuery);
	this->pnlQuery->SetRect(0, 0, 100, 31, false);
	this->pnlQuery->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnQuery = ui->NewButton(this->pnlQuery, CSTR("Query"));
	this->btnQuery->SetRect(4, 4, 75, 23, false);
	this->btnQuery->HandleButtonClick(OnQueryClicked, this);
	this->txtQuery = ui->NewTextBox(this->tpQuery, CSTR(""), true);
	this->txtQuery->SetRect(0, 0, 100, 160, false);
	this->txtQuery->SetDockType(UI::GUIControl::DOCK_TOP);
	this->txtQueryStatus = ui->NewTextBox(this->tpQuery, CSTR(""));
	this->txtQueryStatus->SetRect(0, 0, 100, 23, false);
	this->txtQueryStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtQueryStatus->SetReadOnly(true);
	this->vspQuery = ui->NewVSplitter(this->tpQuery, 3, false);
	this->lvQueryResult = ui->NewListView(this->tpQuery, UI::ListViewStyle::Table, 2);
	this->lvQueryResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvQueryResult->SetFullRowSelect(true);
	this->lvQueryResult->SetShowGrid(true);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRMySQLClientForm::~AVIRMySQLClientForm()
{
	this->cli.Delete();
}

void SSWR::AVIRead::AVIRMySQLClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
