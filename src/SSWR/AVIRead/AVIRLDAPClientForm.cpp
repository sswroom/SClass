#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "SSWR/AVIRead/AVIRLDAPClientForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRLDAPClientForm::OnConnectClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLDAPClientForm *me = (SSWR::AVIRead::AVIRLDAPClientForm*)userObj;
	if (me->cli)
	{
		me->cli->Unbind();
		DEL_CLASS(me->cli);
		me->cli = 0;
		me->btnConnect->SetText(CSTR("Connect"));
		return;
	}
	Text::StringBuilderUTF8 sb;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	NotNullPtr<Net::SocketFactory> sockf = me->core->GetSocketFactory();
	me->txtHost->GetText(sb);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter Host"), CSTR("LDAP Client"), me);
		return;
	}
	if (!sockf->DNSResolveIP(sb.ToCString(), &addr))
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in resolving host"), CSTR("LDAP Client"), me);
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(sb);
	if (!sb.ToUInt16(port))
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter valid Port number"), CSTR("LDAP Client"), me);
		return;
	}
	NEW_CLASS(me->cli, Net::LDAPClient(sockf, &addr, port, 15000));
	if (me->cli->IsError())
	{
		DEL_CLASS(me->cli);
		me->cli = 0;
		UI::MessageDialog::ShowDialog(CSTR("Error in connecting to LDAP Server"), CSTR("LDAP Client"), me);
		return;
	}
	Bool succ = false;
	if (me->cboAuthType->GetSelectedIndex() == 0)
	{
		succ = me->cli->Bind(CSTR_NULL, CSTR_NULL);
	}
	else if (me->cboAuthType->GetSelectedIndex() == 1)
	{
		Text::StringBuilderUTF8 sb2;
		sb.ClearStr();
		me->txtUserDN->GetText(sb);
		me->txtPassword->GetText(sb2);
		succ = me->cli->Bind(sb.ToCString(), sb2.ToCString());
	}
	if (succ)
	{
		me->btnConnect->SetText(CSTR("Disconnect"));
	}
	else
	{
		DEL_CLASS(me->cli);
		me->cli = 0;
		UI::MessageDialog::ShowDialog(CSTR("Error in binding to LDAP Server"), CSTR("LDAP Client"), me);
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRLDAPClientForm::OnSearchClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLDAPClientForm *me = (SSWR::AVIRead::AVIRLDAPClientForm*)userObj;
	if (me->cli == 0)
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Data::ArrayList<Net::LDAPClient::SearchResObject*> results;
	me->txtSearchBase->GetText(sb);
	me->txtSearchFilter->GetText(sb2);
	if (!me->cli->Search(sb.ToCString(), (Net::LDAPClient::ScopeType)(OSInt)me->cboSearchScope->GetSelectedItem(), (Net::LDAPClient::DerefType)(OSInt)me->cboSearchDerefAliases->GetSelectedItem(), 0, 0, false, sb2.ToString(), &results))
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in searching from server"), CSTR("LDAP Client"), me);
	}
	else
	{
		Net::LDAPClient::SearchResultsFree(me->dispResults);
		me->dispResults->AddAll(results);
		me->lvSearch->ClearItems();
		me->cboSearchResult->ClearItems();
		Net::LDAPClient::SearchResObject *obj;
		UOSInt i;
		UOSInt j;
		i = 0;
		j = me->dispResults->GetCount();
		while (i < j)
		{
			obj = me->dispResults->GetItem(i);
			me->cboSearchResult->AddItem(obj->name, obj);
			i++;
		}
		if (j > 0)
		{
			me->cboSearchResult->SetSelectedIndex(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRLDAPClientForm::OnSearchResultSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRLDAPClientForm *me = (SSWR::AVIRead::AVIRLDAPClientForm*)userObj;
	Net::LDAPClient::SearchResObject *obj = (Net::LDAPClient::SearchResObject*)me->cboSearchResult->GetSelectedItem();
	me->lvSearch->ClearItems();
	if (obj && obj->items)
	{
		UOSInt i;
		UOSInt j;
		UOSInt k;
		Net::LDAPClient::SearchResItem *item;
		Text::StringBuilderUTF8 sb;
		i = 0;
		j = obj->items->GetCount();
		while (i < j)
		{
			item = obj->items->GetItem(i);
			k = me->lvSearch->AddItem(item->type, 0);
			sb.ClearStr();
			Net::LDAPClient::SearchResDisplay(item->type->ToCString(), item->value->ToCString(), sb);
			me->lvSearch->SetSubItem(k, 1, sb.ToCString());
			i++;
		}
	}
}

SSWR::AVIRead::AVIRLDAPClientForm::AVIRLDAPClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("LDAP Client"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 151, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this->pnlRequest, CSTR("Host")));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtHost->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlRequest, CSTR("Port")));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlRequest, CSTR("389")));
	this->txtPort->SetRect(104, 28, 120, 23, false);
	NEW_CLASS(this->lblAuthType, UI::GUILabel(ui, this->pnlRequest, CSTR("Auth Type")));
	this->lblAuthType->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->cboAuthType, UI::GUIComboBox(ui, this->pnlRequest, false));
	this->cboAuthType->SetRect(104, 52, 120, 23, false);
	this->cboAuthType->AddItem(CSTR("Anonymous"), (void*)1);
	this->cboAuthType->AddItem(CSTR("Simple Password"), (void*)2);
	this->cboAuthType->SetSelectedIndex(0);
	NEW_CLASS(this->lblUserDN, UI::GUILabel(ui, this->pnlRequest, CSTR("User DN")));
	this->lblUserDN->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtUserDN, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtUserDN->SetRect(104, 76, 200, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this->pnlRequest, CSTR("Password")));
	this->lblPassword->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtPassword->SetRect(104, 100, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	NEW_CLASS(this->btnConnect, UI::GUIButton(ui, this->pnlRequest, CSTR("Connect")));
	this->btnConnect->SetRect(104, 124, 75, 23, false);
	this->btnConnect->HandleButtonClick(OnConnectClicked, this);

	NEW_CLASS(this->grpSearch, UI::GUIGroupBox(ui, this, CSTR("Search")));
	this->grpSearch->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlSearchRequest, UI::GUIPanel(ui, this->grpSearch));
	this->pnlSearchRequest->SetRect(0, 0, 100, 124, false);
	this->pnlSearchRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblSearchBase, UI::GUILabel(ui, this->pnlSearchRequest, CSTR("Base Object")));
	this->lblSearchBase->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSearchBase, UI::GUITextBox(ui, this->pnlSearchRequest, CSTR("dc=simon,dc=local")));
	this->txtSearchBase->SetRect(104, 4, 400, 23, false);
	NEW_CLASS(this->lblSearchScope, UI::GUILabel(ui, this->pnlSearchRequest, CSTR("Scope")));
	this->lblSearchScope->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboSearchScope, UI::GUIComboBox(ui, this->pnlSearchRequest, false));
	this->cboSearchScope->SetRect(104, 28, 150, 23, false);
	this->cboSearchScope->AddItem(CSTR("Base Object"), (void *)0);
	this->cboSearchScope->AddItem(CSTR("Single Level"), (void *)1);
	this->cboSearchScope->AddItem(CSTR("Whole Subtree"), (void *)2);
	this->cboSearchScope->SetSelectedIndex(0);
	NEW_CLASS(this->lblSearchDerefAliases, UI::GUILabel(ui, this->pnlSearchRequest, CSTR("Deref Aliases")));
	this->lblSearchDerefAliases->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->cboSearchDerefAliases, UI::GUIComboBox(ui, this->pnlSearchRequest, false));
	this->cboSearchDerefAliases->SetRect(104, 52, 150, 23, false);
	this->cboSearchDerefAliases->AddItem(CSTR("Never Deref Aliases"), (void *)0);
	this->cboSearchDerefAliases->AddItem(CSTR("Deref In Searching"), (void *)1);
	this->cboSearchDerefAliases->AddItem(CSTR("Deref Finding Base Object"), (void *)2);
	this->cboSearchDerefAliases->AddItem(CSTR("Deref Always"), (void *)3);
	this->cboSearchDerefAliases->SetSelectedIndex(1);
	NEW_CLASS(this->lblSearchFilter, UI::GUILabel(ui, this->pnlSearchRequest, CSTR("Filter")));
	this->lblSearchFilter->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtSearchFilter, UI::GUITextBox(ui, this->pnlSearchRequest, CSTR("(&(objectClass=user)(userPrincipalName=admin@simon.local))")));
	this->txtSearchFilter->SetRect(104, 76, 400, 23, false);
	NEW_CLASS(this->btnSearch, UI::GUIButton(ui, this->pnlSearchRequest, CSTR("Search")));
	this->btnSearch->SetRect(104, 100, 75, 23, false);
	this->btnSearch->HandleButtonClick(OnSearchClicked, this);
	NEW_CLASS(this->cboSearchResult, UI::GUIComboBox(ui, this->grpSearch, false));
	this->cboSearchResult->SetRect(0, 0, 100, 23, false);
	this->cboSearchResult->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboSearchResult->HandleSelectionChange(OnSearchResultSelChg, this);
	NEW_CLASS(this->lvSearch, UI::GUIListView(ui, this->grpSearch, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvSearch->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSearch->SetFullRowSelect(true);
	this->lvSearch->SetShowGrid(true);
	this->lvSearch->AddColumn(CSTR("Type"), 200);
	this->lvSearch->AddColumn(CSTR("Value"), 500);

	this->cli = 0;
	NEW_CLASS(this->dispResults, Data::ArrayList<Net::LDAPClient::SearchResObject*>());
}

SSWR::AVIRead::AVIRLDAPClientForm::~AVIRLDAPClientForm()
{
	SDEL_CLASS(this->cli);
	Net::LDAPClient::SearchResultsFree(this->dispResults);
	DEL_CLASS(this->dispResults);
}

void SSWR::AVIRead::AVIRLDAPClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
