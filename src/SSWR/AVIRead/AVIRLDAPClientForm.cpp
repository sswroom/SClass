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
		me->btnConnect->SetText((const UTF8Char*)"Connect");
		return;
	}
	Text::StringBuilderUTF8 sb;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	Net::SocketFactory *sockf = me->core->GetSocketFactory();
	me->txtHost->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter Host", (const UTF8Char*)"LDAP Client", me);
		return;
	}
	if (!sockf->DNSResolveIP(sb.ToString(), sb.GetLength(), &addr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in resolving host", (const UTF8Char*)"LDAP Client", me);
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(&sb);
	if (!sb.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid Port number", (const UTF8Char*)"LDAP Client", me);
		return;
	}
	NEW_CLASS(me->cli, Net::LDAPClient(sockf, &addr, port));
	if (me->cli->IsError())
	{
		DEL_CLASS(me->cli);
		me->cli = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in connecting to LDAP Server", (const UTF8Char*)"LDAP Client", me);
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
		me->txtUserDN->GetText(&sb);
		me->txtPassword->GetText(&sb2);
		succ = me->cli->Bind(sb.ToCString(), sb2.ToCString());
	}
	if (succ)
	{
		me->btnConnect->SetText((const UTF8Char*)"Disconnect");
	}
	else
	{
		DEL_CLASS(me->cli);
		me->cli = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in binding to LDAP Server", (const UTF8Char*)"LDAP Client", me);
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
	me->txtSearchBase->GetText(&sb);
	me->txtSearchFilter->GetText(&sb2);
	if (!me->cli->Search(sb.ToCString(), (Net::LDAPClient::ScopeType)(OSInt)me->cboSearchScope->GetSelectedItem(), (Net::LDAPClient::DerefType)(OSInt)me->cboSearchDerefAliases->GetSelectedItem(), 0, 0, false, sb2.ToString(), &results))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in searching from server", (const UTF8Char*)"LDAP Client", me);
	}
	else
	{
		Net::LDAPClient::SearchResultsFree(me->dispResults);
		me->dispResults->AddAll(&results);
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
			Net::LDAPClient::SearchResDisplay(item->type, item->value, &sb);
			me->lvSearch->SetSubItem(k, 1, sb.ToString());
			i++;
		}
	}
}

SSWR::AVIRead::AVIRLDAPClientForm::AVIRLDAPClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"LDAP Client");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 151, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)""));
	this->txtHost->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"389"));
	this->txtPort->SetRect(104, 28, 120, 23, false);
	NEW_CLASS(this->lblAuthType, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Auth Type"));
	this->lblAuthType->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->cboAuthType, UI::GUIComboBox(ui, this->pnlRequest, false));
	this->cboAuthType->SetRect(104, 52, 120, 23, false);
	this->cboAuthType->AddItem((const UTF8Char*)"Anonymous", (void*)1);
	this->cboAuthType->AddItem((const UTF8Char*)"Simple Password", (void*)2);
	this->cboAuthType->SetSelectedIndex(0);
	NEW_CLASS(this->lblUserDN, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"User DN"));
	this->lblUserDN->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtUserDN, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)""));
	this->txtUserDN->SetRect(104, 76, 200, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Password"));
	this->lblPassword->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)""));
	this->txtPassword->SetRect(104, 100, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	NEW_CLASS(this->btnConnect, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"Connect"));
	this->btnConnect->SetRect(104, 124, 75, 23, false);
	this->btnConnect->HandleButtonClick(OnConnectClicked, this);

	NEW_CLASS(this->grpSearch, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Search"));
	this->grpSearch->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlSearchRequest, UI::GUIPanel(ui, this->grpSearch));
	this->pnlSearchRequest->SetRect(0, 0, 100, 124, false);
	this->pnlSearchRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblSearchBase, UI::GUILabel(ui, this->pnlSearchRequest, (const UTF8Char*)"Base Object"));
	this->lblSearchBase->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSearchBase, UI::GUITextBox(ui, this->pnlSearchRequest, (const UTF8Char*)"dc=corp,dc=stoneroad,dc=com,dc=hk"));
	this->txtSearchBase->SetRect(104, 4, 400, 23, false);
	NEW_CLASS(this->lblSearchScope, UI::GUILabel(ui, this->pnlSearchRequest, (const UTF8Char*)"Scope"));
	this->lblSearchScope->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboSearchScope, UI::GUIComboBox(ui, this->pnlSearchRequest, false));
	this->cboSearchScope->SetRect(104, 28, 150, 23, false);
	this->cboSearchScope->AddItem((const UTF8Char*)"Base Object", (void *)0);
	this->cboSearchScope->AddItem((const UTF8Char*)"Single Level", (void *)1);
	this->cboSearchScope->AddItem((const UTF8Char*)"Whole Subtree", (void *)2);
	this->cboSearchScope->SetSelectedIndex(0);
	NEW_CLASS(this->lblSearchDerefAliases, UI::GUILabel(ui, this->pnlSearchRequest, (const UTF8Char*)"Deref Aliases"));
	this->lblSearchDerefAliases->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->cboSearchDerefAliases, UI::GUIComboBox(ui, this->pnlSearchRequest, false));
	this->cboSearchDerefAliases->SetRect(104, 52, 150, 23, false);
	this->cboSearchDerefAliases->AddItem((const UTF8Char*)"Never Deref Aliases", (void *)0);
	this->cboSearchDerefAliases->AddItem((const UTF8Char*)"Deref In Searching", (void *)1);
	this->cboSearchDerefAliases->AddItem((const UTF8Char*)"Deref Finding Base Object", (void *)2);
	this->cboSearchDerefAliases->AddItem((const UTF8Char*)"Deref Always", (void *)3);
	this->cboSearchDerefAliases->SetSelectedIndex(1);
	NEW_CLASS(this->lblSearchFilter, UI::GUILabel(ui, this->pnlSearchRequest, (const UTF8Char*)"Filter"));
	this->lblSearchFilter->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtSearchFilter, UI::GUITextBox(ui, this->pnlSearchRequest, (const UTF8Char*)"(&(objectClass=user)(userPrincipalName=enma@corp.stoneroad.com.hk))"));
	this->txtSearchFilter->SetRect(104, 76, 400, 23, false);
	NEW_CLASS(this->btnSearch, UI::GUIButton(ui, this->pnlSearchRequest, (const UTF8Char*)"Search"));
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
	this->lvSearch->AddColumn((const UTF8Char*)"Type", 200);
	this->lvSearch->AddColumn((const UTF8Char*)"Value", 500);

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
