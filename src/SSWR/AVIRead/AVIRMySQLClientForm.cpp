#include "Stdafx.h"
#include "DB/ColDef.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRMySQLClientForm.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRMySQLClientForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMySQLClientForm *me = (SSWR::AVIRead::AVIRMySQLClientForm*)userObj;
	if (me->cli)
	{
		DEL_CLASS(me->cli);
		me->cli = 0;
		me->txtHost->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		me->txtUserName->SetReadOnly(false);
		me->txtPassword->SetReadOnly(false);
		me->txtDatabase->SetReadOnly(false);
		me->txtStatus->SetText((const UTF8Char*)"Disconnected");
		return;
	}
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	Text::StringBuilderUTF8 sbUser;
	Text::StringBuilderUTF8 sbPwd;
	Text::StringBuilderUTF8 sbDatabase;
	const UTF8Char *sDatabase;
	me->txtHost->GetText(&sbUser);
	me->txtPort->GetText(&sbPwd);
	if (sbUser.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter the host", (const UTF8Char*)"MySQL Client", me);
		me->txtHost->Focus();
		return;
	}
	if (sbPwd.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter the port", (const UTF8Char*)"MySQL Client", me);
		me->txtPort->Focus();
		return;
	}
	if (!me->core->GetSocketFactory()->DNSResolveIP(sbUser.ToString(), &addr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in resolving host", (const UTF8Char*)"MySQL Client", me);
		me->txtHost->Focus();
		return;
	}
	if (!Text::StrToUInt16(sbPwd.ToString(), &port))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Port invalid", (const UTF8Char*)"MySQL Client", me);
		me->txtHost->Focus();
		return;
	}
	sbUser.ClearStr();
	sbPwd.ClearStr();
	me->txtUserName->GetText(&sbUser);
	me->txtPassword->GetText(&sbPwd);
	me->txtDatabase->GetText(&sbDatabase);
	if (sbUser.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter the user name", (const UTF8Char*)"MySQL Client", me);
		me->txtUserName->Focus();
		return;
	}
	if (sbPwd.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter the password", (const UTF8Char*)"MySQL Client", me);
		me->txtPassword->Focus();
		return;
	}
	if (sbDatabase.GetLength() > 0)
	{
		sDatabase = sbDatabase.ToString();
	}
	else
	{
		sDatabase = 0;
	}
	
	me->cliConnected = false;
	NEW_CLASS(me->cli, Net::MySQLTCPClient(me->core->GetSocketFactory(), &addr, port, sbUser.ToString(), sbPwd.ToString(), sDatabase));
	if (me->cli->IsError())
	{
		sbUser.ClearStr();
		sbUser.AppendC(UTF8STRC("Error in connecting to server: "));
		me->cli->GetErrorMsg(&sbUser);
		DEL_CLASS(me->cli);
		me->cli = 0;
		UI::MessageDialog::ShowDialog(sbUser.ToString(), (const UTF8Char*)"MySQL Client", me);
	}
	else
	{
		me->txtHost->SetReadOnly(true);
		me->txtPort->SetReadOnly(true);
		me->txtUserName->SetReadOnly(true);
		me->txtPassword->SetReadOnly(true);
		me->txtDatabase->SetReadOnly(true);
	}
}

void __stdcall SSWR::AVIRead::AVIRMySQLClientForm::OnQueryClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMySQLClientForm *me = (SSWR::AVIRead::AVIRMySQLClientForm*)userObj;
	if (me->cli == 0)
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	me->txtQuery->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		return;
	}
	DB::DBReader *reader = me->cli->ExecuteReaderC(sb.ToString(), sb.GetLength());
	if (reader)
	{
		me->UpdateResult(reader);
		me->cli->CloseReader(reader);
	}
	else
	{
		sb.ClearStr();
		me->cli->GetErrorMsg(&sb);
		me->txtQueryStatus->SetText(sb.ToString());
	}
}

void __stdcall SSWR::AVIRead::AVIRMySQLClientForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRMySQLClientForm *me = (SSWR::AVIRead::AVIRMySQLClientForm*)userObj;
	UTF8Char sbuff[128];
	UInt8 buff[48];
	UOSInt i;
	Text::String *s;
	if (me->cli)
	{
		if (!me->cliConnected && me->cli->ServerInfoRecv())
		{
			me->cliConnected = true;
			s = Text::String::OrEmpty(me->cli->GetServerVer());
			me->txtServerVer->SetText(s->v);
			Text::StrUInt32(sbuff, me->cli->GetConnId());
			me->txtConnId->SetText(sbuff);
			i = me->cli->GetAuthPluginData(buff);
			Text::StrHexBytes(sbuff, buff, i, ' ');
			me->txtAuthPluginData->SetText(sbuff);
			Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), me->cli->GetServerCap());
			me->txtServerCap->SetText(sbuff);
			Text::StrUInt16(sbuff, me->cli->GetServerCS());
			me->txtServerCS->SetText(sbuff);
			me->txtStatus->SetText((const UTF8Char*)"Connected");
		}
		if (me->cli->IsError())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Disconnected: "));
			me->cli->GetErrorMsg(&sb);
			DEL_CLASS(me->cli);
			me->cli = 0;
			me->txtHost->SetReadOnly(false);
			me->txtPort->SetReadOnly(false);
			me->txtUserName->SetReadOnly(false);
			me->txtPassword->SetReadOnly(false);
			me->txtDatabase->SetReadOnly(false);
			me->txtStatus->SetText(sb.ToString());
		}
	}
}


void SSWR::AVIRead::AVIRMySQLClientForm::UpdateResult(DB::DBReader *r)
{
	OSInt rowChg;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	DB::ColDef *col;
	Text::StringBuilderUTF8 *sb;
	UOSInt *colSize;

	rowChg = r->GetRowChanged();
	if (rowChg == -1)
	{
		this->txtQueryStatus->SetText((const UTF8Char*)"");
		this->lvQueryResult->ClearAll();

		NEW_CLASS(sb, Text::StringBuilderUTF8());
		NEW_CLASS(col, DB::ColDef((const UTF8Char*)""));
		j = r->ColCount();
		this->lvQueryResult->ChangeColumnCnt(j);
		i = 0;
		colSize = MemAlloc(UOSInt, j);
		while (i < j)
		{
			if (r->GetColDef(i, col))
			{
				this->lvQueryResult->AddColumn(col->GetColName(), 100);
			}
			else
			{
				this->lvQueryResult->AddColumn((const UTF8Char*)"Unnamed", 100);
			}
			colSize[i] = 0;
			i++;
		}
		DEL_CLASS(col);

		OSInt rowCnt = 0;
		while (r->ReadNext())
		{
			i = 1;
			sb->ClearStr();
			r->GetStr(0, sb);
			if (sb->GetLength() > colSize[0])
				colSize[0] = sb->GetLength();
			k = this->lvQueryResult->AddItem(sb->ToString(), 0);
			while (i < j)
			{
				sb->ClearStr();
				r->GetStr(i, sb);
				this->lvQueryResult->SetSubItem(k, i, sb->ToString());

				if (sb->GetLength() > colSize[i])
					colSize[i] = sb->GetLength();
				i++;
			}
			rowCnt++;
			if (rowCnt > 1000)
				break;
		}
		DEL_CLASS(sb);

		k = 0;
		i = j;
		while (i-- > 0)
		{
			k += colSize[i];
		}
		Double w;
		Double h;
		if (k > 0)
		{
			this->lvQueryResult->GetSize(&w, &h);
			w -= Math::UOSInt2Double(20 + j * 6);
			if (w < 0)
				w = 0;
			i = 0;
			while (i < j)
			{
				this->lvQueryResult->SetColumnWidth(i, (Math::UOSInt2Double(colSize[i]) * w / Math::UOSInt2Double(k) + 6));
				i++;
			}
		}
		MemFree(colSize);
	}
	else
	{
		this->lvQueryResult->ClearItems();
		NEW_CLASS(sb, Text::StringBuilderUTF8());
		sb->AppendC(UTF8STRC("Record changed = "));
		sb->AppendOSInt(rowChg);
		this->txtQueryStatus->SetText(sb->ToString());
		DEL_CLASS(sb);
	}
}

SSWR::AVIRead::AVIRMySQLClientForm::AVIRMySQLClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText((const UTF8Char*)"MySQL Client");
	this->SetFont(0, 8.25, false);
	this->cli = 0;

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage((const UTF8Char*)"Control");
	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this->tpControl, (const UTF8Char*)"Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this->tpControl, (const UTF8Char*)"127.0.0.1"));
	this->txtHost->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->tpControl, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->tpControl, (const UTF8Char*)"3306"));
	this->txtPort->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblUserName, UI::GUILabel(ui, this->tpControl, (const UTF8Char*)"User Name"));
	this->lblUserName->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtUserName, UI::GUITextBox(ui, this->tpControl, (const UTF8Char*)"root"));
	this->txtUserName->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this->tpControl, (const UTF8Char*)"Password"));
	this->lblPassword->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this->tpControl, (const UTF8Char*)""));
	this->txtPassword->SetRect(104, 76, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	NEW_CLASS(this->lblDatabase, UI::GUILabel(ui, this->tpControl, (const UTF8Char*)"Database"));
	this->lblDatabase->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtDatabase, UI::GUITextBox(ui, this->tpControl, (const UTF8Char*)""));
	this->txtDatabase->SetRect(104, 100, 200, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->tpControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(104, 124, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this->tpControl, (const UTF8Char*)"Status"));
	this->lblStatus->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this->tpControl, (const UTF8Char*)"Not connected"));
	this->txtStatus->SetRect(104, 148, 150, 23, false);
	this->txtStatus->SetReadOnly(true);

	this->tpInfo = this->tcMain->AddTabPage((const UTF8Char*)"Info");
	NEW_CLASS(this->lblServerVer, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Server Ver"));
	this->lblServerVer->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtServerVer, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)""));
	this->txtServerVer->SetRect(104, 4, 250, 23, false);
	this->txtServerVer->SetReadOnly(true);
	NEW_CLASS(this->lblConnId, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Conn Id"));
	this->lblConnId->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtConnId, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)""));
	this->txtConnId->SetRect(104, 28, 150, 23, false);
	this->txtConnId->SetReadOnly(true);
	NEW_CLASS(this->lblAuthPluginData, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Auth Data"));
	this->lblAuthPluginData->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtAuthPluginData, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)""));
	this->txtAuthPluginData->SetRect(104, 52, 400, 23, false);
	this->txtAuthPluginData->SetReadOnly(true);
	NEW_CLASS(this->lblServerCap, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Capability"));
	this->lblServerCap->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtServerCap, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)""));
	this->txtServerCap->SetRect(104, 76, 120, 23, false);
	this->txtServerCap->SetReadOnly(true);
	NEW_CLASS(this->lblServerCS, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"CharacterSet"));
	this->lblServerCS->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtServerCS, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)""));
	this->txtServerCS->SetRect(104, 100, 100, 23, false);
	this->txtServerCS->SetReadOnly(true);

	tpQuery = this->tcMain->AddTabPage((const UTF8Char*)"Query");
	NEW_CLASS(this->pnlQuery, UI::GUIPanel(ui, this->tpQuery));
	this->pnlQuery->SetRect(0, 0, 100, 31, false);
	this->pnlQuery->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnQuery, UI::GUIButton(ui, this->pnlQuery, (const UTF8Char*)"Query"));
	this->btnQuery->SetRect(4, 4, 75, 23, false);
	this->btnQuery->HandleButtonClick(OnQueryClicked, this);
	NEW_CLASS(this->txtQuery, UI::GUITextBox(ui, this->tpQuery, (const UTF8Char*)"", true));
	this->txtQuery->SetRect(0, 0, 100, 160, false);
	this->txtQuery->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->txtQueryStatus, UI::GUITextBox(ui, this->tpQuery, (const UTF8Char*)""));
	this->txtQueryStatus->SetRect(0, 0, 100, 23, false);
	this->txtQueryStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtQueryStatus->SetReadOnly(true);
	NEW_CLASS(this->vspQuery, UI::GUIVSplitter(ui, this->tpQuery, 3, false));
	NEW_CLASS(this->lvQueryResult, UI::GUIListView(ui, this->tpQuery, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvQueryResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvQueryResult->SetFullRowSelect(true);
	this->lvQueryResult->SetShowGrid(true);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRMySQLClientForm::~AVIRMySQLClientForm()
{
	if (this->cli)
	{
		DEL_CLASS(this->cli);
		this->cli = 0;
	}
}

void SSWR::AVIRead::AVIRMySQLClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
