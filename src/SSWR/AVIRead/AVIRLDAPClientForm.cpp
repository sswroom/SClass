#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "SSWR/AVIRead/AVIRLDAPClientForm.h"

void __stdcall SSWR::AVIRead::AVIRLDAPClientForm::OnConnectClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLDAPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLDAPClientForm>();
	NN<Net::LDAPClient> cli;
	if (me->cli.SetTo(cli))
	{
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
		me->ui->ShowMsgOK(CSTR("Please enter Host"), CSTR("LDAP Client"), me);
		return;
	}
	if (!sockf->DNSResolveIP(sb.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Error in resolving host"), CSTR("LDAP Client"), me);
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(sb);
	if (!sb.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid Port number"), CSTR("LDAP Client"), me);
		return;
	}
	NEW_CLASSNN(cli, Net::LDAPClient(sockf, addr, port, 15000));
	if (cli->IsError())
	{
		cli.Delete();
		me->ui->ShowMsgOK(CSTR("Error in connecting to LDAP Server"), CSTR("LDAP Client"), me);
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
	if (succ)
	{
		me->btnConnect->SetText(CSTR("Disconnect"));
	}
	else
	{
		cli.Delete();
		me->cli = 0;
		me->ui->ShowMsgOK(CSTR("Error in binding to LDAP Server"), CSTR("LDAP Client"), me);
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRLDAPClientForm::OnSearchClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLDAPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLDAPClientForm>();
	NN<Net::LDAPClient> cli;
	if (!me->cli.SetTo(cli))
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Data::ArrayListNN<Net::LDAPClient::SearchResObject> results;
	me->txtSearchBase->GetText(sb);
	me->txtSearchFilter->GetText(sb2);
	if (!cli->Search(sb.ToCString(), (Net::LDAPClient::ScopeType)me->cboSearchScope->GetSelectedItem().GetOSInt(), (Net::LDAPClient::DerefType)me->cboSearchDerefAliases->GetSelectedItem().GetOSInt(), 0, 0, false, sb2.ToString(), results))
	{
		me->ui->ShowMsgOK(CSTR("Error in searching from server"), CSTR("LDAP Client"), me);
	}
	else
	{
		Net::LDAPClient::SearchResultsFree(me->dispResults);
		me->dispResults.AddAll(results);
		me->lvSearch->ClearItems();
		me->cboSearchResult->ClearItems();
		NN<Net::LDAPClient::SearchResObject> obj;
		UOSInt i;
		UOSInt j;
		i = 0;
		j = me->dispResults.GetCount();
		while (i < j)
		{
			obj = me->dispResults.GetItemNoCheck(i);
			me->cboSearchResult->AddItem(obj->name, obj);
			i++;
		}
		if (j > 0)
		{
			me->cboSearchResult->SetSelectedIndex(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRLDAPClientForm::OnSearchResultSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLDAPClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLDAPClientForm>();
	NN<Net::LDAPClient::SearchResObject> obj;
	NN<Data::ArrayListNN<Net::LDAPClient::SearchResItem>> items;
	me->lvSearch->ClearItems();
	if (me->cboSearchResult->GetSelectedItem().GetOpt<Net::LDAPClient::SearchResObject>().SetTo(obj) && obj->items.SetTo(items))
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
			k = me->lvSearch->AddItem(item->type, 0);
			sb.ClearStr();
			Net::LDAPClient::SearchResDisplay(item->type->ToCString(), item->value->ToCString(), sb);
			me->lvSearch->SetSubItem(k, 1, sb.ToCString());
			i++;
		}
	}
}

SSWR::AVIRead::AVIRLDAPClientForm::AVIRLDAPClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("LDAP Client"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

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

	this->grpSearch = ui->NewGroupBox(*this, CSTR("Search"));
	this->grpSearch->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlSearchRequest = ui->NewPanel(this->grpSearch);
	this->pnlSearchRequest->SetRect(0, 0, 100, 124, false);
	this->pnlSearchRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblSearchBase = ui->NewLabel(this->pnlSearchRequest, CSTR("Base Object"));
	this->lblSearchBase->SetRect(4, 4, 100, 23, false);
	this->txtSearchBase = ui->NewTextBox(this->pnlSearchRequest, CSTR("dc=simon,dc=local"));
	this->txtSearchBase->SetRect(104, 4, 400, 23, false);
	this->lblSearchScope = ui->NewLabel(this->pnlSearchRequest, CSTR("Scope"));
	this->lblSearchScope->SetRect(4, 28, 100, 23, false);
	this->cboSearchScope = ui->NewComboBox(this->pnlSearchRequest, false);
	this->cboSearchScope->SetRect(104, 28, 150, 23, false);
	this->cboSearchScope->AddItem(CSTR("Base Object"), (void *)0);
	this->cboSearchScope->AddItem(CSTR("Single Level"), (void *)1);
	this->cboSearchScope->AddItem(CSTR("Whole Subtree"), (void *)2);
	this->cboSearchScope->SetSelectedIndex(0);
	this->lblSearchDerefAliases = ui->NewLabel(this->pnlSearchRequest, CSTR("Deref Aliases"));
	this->lblSearchDerefAliases->SetRect(4, 52, 100, 23, false);
	this->cboSearchDerefAliases = ui->NewComboBox(this->pnlSearchRequest, false);
	this->cboSearchDerefAliases->SetRect(104, 52, 150, 23, false);
	this->cboSearchDerefAliases->AddItem(CSTR("Never Deref Aliases"), (void *)0);
	this->cboSearchDerefAliases->AddItem(CSTR("Deref In Searching"), (void *)1);
	this->cboSearchDerefAliases->AddItem(CSTR("Deref Finding Base Object"), (void *)2);
	this->cboSearchDerefAliases->AddItem(CSTR("Deref Always"), (void *)3);
	this->cboSearchDerefAliases->SetSelectedIndex(1);
	this->lblSearchFilter = ui->NewLabel(this->pnlSearchRequest, CSTR("Filter"));
	this->lblSearchFilter->SetRect(4, 76, 100, 23, false);
	this->txtSearchFilter = ui->NewTextBox(this->pnlSearchRequest, CSTR("(&(objectClass=user)(userPrincipalName=admin@simon.local))"));
	this->txtSearchFilter->SetRect(104, 76, 400, 23, false);
	this->btnSearch = ui->NewButton(this->pnlSearchRequest, CSTR("Search"));
	this->btnSearch->SetRect(104, 100, 75, 23, false);
	this->btnSearch->HandleButtonClick(OnSearchClicked, this);
	this->cboSearchResult = ui->NewComboBox(this->grpSearch, false);
	this->cboSearchResult->SetRect(0, 0, 100, 23, false);
	this->cboSearchResult->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboSearchResult->HandleSelectionChange(OnSearchResultSelChg, this);
	this->lvSearch = ui->NewListView(this->grpSearch, UI::ListViewStyle::Table, 2);
	this->lvSearch->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSearch->SetFullRowSelect(true);
	this->lvSearch->SetShowGrid(true);
	this->lvSearch->AddColumn(CSTR("Type"), 200);
	this->lvSearch->AddColumn(CSTR("Value"), 500);

	this->cli = 0;
}

SSWR::AVIRead::AVIRLDAPClientForm::~AVIRLDAPClientForm()
{
	this->cli.Delete();
	Net::LDAPClient::SearchResultsFree(this->dispResults);
}

void SSWR::AVIRead::AVIRLDAPClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
