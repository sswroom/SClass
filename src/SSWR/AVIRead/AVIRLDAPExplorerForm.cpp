#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "SSWR/AVIRead/AVIRLDAPExplorerForm.h"

void __stdcall SSWR::AVIRead::AVIRLDAPExplorerForm::OnConnectClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLDAPExplorerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLDAPExplorerForm>();
	NN<Net::LDAPClient> cli;
	if (me->cli.SetTo(cli))
	{
		Net::LDAPClient::SearchResultsFree(me->dispResults);
		me->lbPath->ClearItems();
		me->lbObjects->ClearItems();
		me->lvValues->ClearItems();
		cli->Unbind();
		cli.Delete();
		me->cli = 0;
		me->btnConnect->SetText(CSTR("Connect"));
		return;
	}
	Text::StringBuilderUTF8 sb;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	NN<Net::SocketFactory> sockf = me->core->GetSocketFactory();
	me->txtHost->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter Host"), CSTR("LDAP Explorer"), me);
		return;
	}
	if (!sockf->DNSResolveIP(sb.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Error in resolving host"), CSTR("LDAP Explorer"), me);
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(sb);
	if (!sb.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid Port number"), CSTR("LDAP Explorer"), me);
		return;
	}
	NEW_CLASSNN(cli, Net::LDAPClient(sockf, addr, port, 30000));
	if (cli->IsError())
	{
		cli.Delete();
		me->ui->ShowMsgOK(CSTR("Error in connecting to LDAP Server"), CSTR("LDAP Explorer"), me);
		return;
	}
	me->cli = cli;
	Bool succ = false;
	if (me->cboAuthType->GetSelectedIndex() == 0)
	{
		succ = cli->Bind(CSTR_NULL, CSTR_NULL);
	}
	else if (me->cboAuthType->GetSelectedIndex() == 1)
	{
		Text::StringBuilderUTF8 sb2;
		sb.ClearStr();
		me->txtUserDN->GetText(sb);
		me->txtPassword->GetText(sb2);
		succ = cli->Bind(sb.ToCString(), sb2.ToCString());
	}
	if (!succ)
	{
		cli.Delete();
		me->cli = 0;
		me->ui->ShowMsgOK(CSTR("Error in binding to LDAP Server"), CSTR("LDAP Explorer"), me);
		return;
	}

	Data::ArrayListNN<Net::LDAPClient::SearchResObject> results;
	succ = cli->Search(CSTR(""), Net::LDAPClient::ST_BASE_OBJECT, Net::LDAPClient::DT_DEREF_IN_SEARCHING, 0, 0, false, (const UTF8Char*)"", results);
	if (!succ)
	{
		cli.Delete();
		me->cli = 0;
		me->ui->ShowMsgOK(CSTR("Error in searching for <ROOT> in LDAP Server"), CSTR("LDAP Explorer"), me);
		return;
	}
	NN<Net::LDAPClient::SearchResObject> obj;
	NN<Data::ArrayListNN<Net::LDAPClient::SearchResItem>> items;
	if (!results.GetItem(0).SetTo(obj) || obj->name->v[0] != 0 || !obj->items.SetTo(items))
	{
		Net::LDAPClient::SearchResultsFree(results);
		cli.Delete();
		me->cli = 0;
		me->ui->ShowMsgOK(CSTR("Unsupported <ROOT> information in LDAP Server"), CSTR("LDAP Explorer"), me);
		return;
	}
	succ = false;
	NN<Net::LDAPClient::SearchResItem> item;
	UOSInt i = items->GetCount();
	while (i-- > 0)
	{
		item = items->GetItemNoCheck(i);
		if (item->type->Equals(UTF8STRC("rootDomainNamingContext")))
		{
			Text::StringBuilderUTF8 sb;
			Text::PString sarr[32];
			UOSInt j;
			sb.Append(item->value);
			j = Text::StrSplitP(sarr, 32, sb, ',');
			me->rootLev = j;
			while (j-- > 0)
			{
				me->lbPath->AddItem(sarr[j].ToCString(), 0);
			}
			me->lbPath->SetSelectedIndex(me->rootLev - 1);
			succ = true;
			break;
		}
	}

	if (!succ)
	{
		Net::LDAPClient::SearchResultsFree(results);
		cli.Delete();
		me->cli = 0;
		me->ui->ShowMsgOK(CSTR("rootDomainNamingContext not found in LDAP Server"), CSTR("LDAP Explorer"), me);
		return;
	}
	me->btnConnect->SetText(CSTR("Disconnect"));
}

void __stdcall SSWR::AVIRead::AVIRLDAPExplorerForm::OnPathSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLDAPExplorerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLDAPExplorerForm>();
	NN<Net::LDAPClient> cli;
	UOSInt i = me->lbPath->GetSelectedIndex();
	if (i == INVALID_INDEX)
	{
		return;
	}
	Net::LDAPClient::SearchResultsFree(me->dispResults);
	me->lbObjects->ClearItems();
	me->lvValues->ClearItems();

	if (i < me->rootLev - 1)
	{
		while (me->lbPath->GetCount() > me->rootLev)
		{
			me->lbPath->RemoveItem(me->lbPath->GetCount() - 1);
		}
		return;
	}

	while (me->lbPath->GetCount() > i + 1)
	{
		me->lbPath->RemoveItem(me->lbPath->GetCount() - 1);
	}

	if (!me->cli.SetTo(cli))
	{
		return;
	}

	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	sptr = me->lbPath->GetItemText(sbuff, i).Or(sbuff);
	sb.AppendP(sbuff, sptr);
	while (i-- > 0)
	{
		sb.AppendUTF8Char(',');
		if (me->lbPath->GetItemText(sbuff, i).SetTo(sptr))
			sb.AppendP(sbuff, sptr);
	}

	Data::ArrayListNN<Net::LDAPClient::SearchResObject> results;
	NN<Net::LDAPClient::SearchResObject> obj;
	if (cli->Search(sb.ToCString(), Net::LDAPClient::ST_BASE_OBJECT, Net::LDAPClient::DT_DEREF_IN_SEARCHING, 0, 0, false, (const UTF8Char*)"", results))
	{
		if (results.RemoveAt(0).SetTo(obj))
		{
			me->dispResults.Add(obj);
		}
		Net::LDAPClient::SearchResultsFree(results);
		me->lbObjects->AddItem(CSTR("."), me->dispResults.GetItem(0).OrNull());
	}
	if (cli->Search(sb.ToCString(), Net::LDAPClient::ST_SINGLE_LEVEL, Net::LDAPClient::DT_DEREF_IN_SEARCHING, 0, 0, false, (const UTF8Char*)"", results))
	{
		UOSInt j;
		UOSInt k;
		i = 0;
		j = results.GetCount();
		while (i < j)
		{
			obj = results.GetItemNoCheck(i);
			if (obj->isRef)
			{
				Net::LDAPClient::SearchResObjectFree(obj);
			}
			else
			{
				me->dispResults.Add(obj);
				sb.ClearStr();
				k = 0;
				while (true)
				{
					if (obj->name->v[k] == '\\')
					{
						if (obj->name->v[k + 1] != 0)
						{
							sb.AppendChar(obj->name->v[k + 1], 1);
							k += 2;
						}
						else
						{
							sb.AppendChar(obj->name->v[k], 1);
							k++;
						}
					}
					else if (obj->name->v[k] == ',')
					{
						break;
					}
					else if (obj->name->v[k] == 0)
					{
						break;
					}
					else
					{
						sb.AppendChar(obj->name->v[k], 1);
						k++;
					}
				}
				me->lbObjects->AddItem(sb.ToCString(), obj);
			}
			i++;
		}
	}
	if (me->lbObjects->GetCount() > 0)
	{
		me->lbObjects->SetSelectedIndex(0);
	}
}

void __stdcall SSWR::AVIRead::AVIRLDAPExplorerForm::OnObjectsSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLDAPExplorerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLDAPExplorerForm>();
	NN<Net::LDAPClient::SearchResObject> obj;
	NN<Data::ArrayListNN<Net::LDAPClient::SearchResItem>> items;
	me->lvValues->ClearItems();
	if (me->lbObjects->GetSelectedItem().GetOpt<Net::LDAPClient::SearchResObject>().SetTo(obj) && obj->items.SetTo(items))
	{
		UOSInt i;
		UOSInt j;
		UOSInt k;
		NN<Net::LDAPClient::SearchResItem> item;
		Text::StringBuilderUTF8 sb;
		i = 0;
		j = items->GetCount();
		while (i < j)
		{
			item = items->GetItemNoCheck(i);
			k = me->lvValues->AddItem(item->type, 0);
			sb.ClearStr();
			Net::LDAPClient::SearchResDisplay(item->type->ToCString(), item->value->ToCString(), sb);
			me->lvValues->SetSubItem(k, 1, sb.ToCString());
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRLDAPExplorerForm::OnObjectsDblClk(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLDAPExplorerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLDAPExplorerForm>();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i = me->lbObjects->GetSelectedIndex();
	if (i == INVALID_INDEX)
		return;
	sptr = me->lbObjects->GetItemText(sbuff, i).Or(sbuff);
	if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".")))
	{
		i = me->lbPath->GetSelectedIndex();
		if (i >= me->rootLev)
		{
			me->lbPath->SetSelectedIndex(i - 1);
		}
	}
	else
	{
		UOSInt j = me->lbPath->AddItem(CSTRP(sbuff, sptr), 0);
		me->lbPath->SetSelectedIndex(j);
	}
}

SSWR::AVIRead::AVIRLDAPExplorerForm::AVIRLDAPExplorerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("LDAP Explorer"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->rootLev = 0;

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 151, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblHost = ui->NewLabel(this->pnlRequest, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtHost->SetRect(104, 4, 150, 23, false);
	this->lblPort = ui->NewLabel(this->pnlRequest, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlRequest, CSTR("389"));
	this->txtPort->SetRect(104, 28, 120, 23, false);
	this->lblAuthType = ui->NewLabel(this->pnlRequest, CSTR("Auth Type"));
	this->lblAuthType->SetRect(4, 52, 100, 23, false);
	this->cboAuthType = ui->NewComboBox(this->pnlRequest, false);
	this->cboAuthType->SetRect(104, 52, 120, 23, false);
	this->cboAuthType->AddItem(CSTR("Anonymous"), (void*)1);
	this->cboAuthType->AddItem(CSTR("Simple Password"), (void*)2);
	this->cboAuthType->SetSelectedIndex(0);
	this->lblUserDN = ui->NewLabel(this->pnlRequest, CSTR("User DN"));
	this->lblUserDN->SetRect(4, 76, 100, 23, false);
	this->txtUserDN = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtUserDN->SetRect(104, 76, 200, 23, false);
	this->lblPassword = ui->NewLabel(this->pnlRequest, CSTR("Password"));
	this->lblPassword->SetRect(4, 100, 100, 23, false);
	this->txtPassword = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtPassword->SetRect(104, 100, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	this->btnConnect = ui->NewButton(this->pnlRequest, CSTR("Connect"));
	this->btnConnect->SetRect(104, 124, 75, 23, false);
	this->btnConnect->HandleButtonClick(OnConnectClicked, this);

	this->lbPath = ui->NewListBox(*this, false);
	this->lbPath->SetRect(0, 0, 150, 23, false);
	this->lbPath->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbPath->HandleSelectionChange(OnPathSelChg, this);
	this->hspPath = ui->NewHSplitter(*this, 3, false);
	this->lbObjects = ui->NewListBox(*this, false);
	this->lbObjects->SetRect(0, 0, 150, 23, false);
	this->lbObjects->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbObjects->HandleSelectionChange(OnObjectsSelChg, this);
	this->lbObjects->HandleDoubleClicked(OnObjectsDblClk, this);
	this->hspObjects = ui->NewHSplitter(*this, 3, false);
	this->lvValues = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvValues->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvValues->SetFullRowSelect(true);
	this->lvValues->SetShowGrid(true);
	this->lvValues->AddColumn(CSTR("Type"), 200);
	this->lvValues->AddColumn(CSTR("Value"), 500);

	this->cli = 0;
}

SSWR::AVIRead::AVIRLDAPExplorerForm::~AVIRLDAPExplorerForm()
{
	this->cli.Delete();
	Net::LDAPClient::SearchResultsFree(this->dispResults);
}

void SSWR::AVIRead::AVIRLDAPExplorerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
