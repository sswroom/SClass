#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "SSWR/AVIRead/AVIRLDAPExplorerForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRLDAPExplorerForm::OnConnectClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLDAPExplorerForm *me = (SSWR::AVIRead::AVIRLDAPExplorerForm*)userObj;
	if (me->cli)
	{
		Net::LDAPClient::SearchResultsFree(me->dispResults);
		me->lbPath->ClearItems();
		me->lbObjects->ClearItems();
		me->lvValues->ClearItems();
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
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter Host", (const UTF8Char*)"LDAP Explorer", me);
		return;
	}
	if (!sockf->DNSResolveIP(sb.ToString(), &addr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in resolving host", (const UTF8Char*)"LDAP Explorer", me);
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(&sb);
	if (!sb.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid Port number", (const UTF8Char*)"LDAP Explorer", me);
		return;
	}
	NEW_CLASS(me->cli, Net::LDAPClient(sockf, &addr, port));
	if (me->cli->IsError())
	{
		DEL_CLASS(me->cli);
		me->cli = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in connecting to LDAP Server", (const UTF8Char*)"LDAP Explorer", me);
		return;
	}
	Bool succ = false;
	if (me->cboAuthType->GetSelectedIndex() == 0)
	{
		succ = me->cli->Bind(0, 0);
	}
	else if (me->cboAuthType->GetSelectedIndex() == 1)
	{
		Text::StringBuilderUTF8 sb2;
		sb.ClearStr();
		me->txtUserDN->GetText(&sb);
		me->txtPassword->GetText(&sb2);
		succ = me->cli->Bind(sb.ToString(), sb2.ToString());
	}
	if (!succ)
	{
		DEL_CLASS(me->cli);
		me->cli = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in binding to LDAP Server", (const UTF8Char*)"LDAP Explorer", me);
		return;
	}

	Data::ArrayList<Net::LDAPClient::SearchResObject*> results;
	succ = me->cli->Search((const UTF8Char*)"", Net::LDAPClient::ST_BASE_OBJECT, Net::LDAPClient::DT_DEREF_IN_SEARCHING, 0, 0, false, (const UTF8Char*)"", &results);
	if (!succ)
	{
		DEL_CLASS(me->cli);
		me->cli = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in searching for <ROOT> in LDAP Server", (const UTF8Char*)"LDAP Explorer", me);
		return;
	}
	Net::LDAPClient::SearchResObject *obj;
	obj = results.GetItem(0);
	if (obj == 0 || obj->name[0] != 0 || obj->items == 0)
	{
		Net::LDAPClient::SearchResultsFree(&results);
		DEL_CLASS(me->cli);
		me->cli = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Unsupported <ROOT> information in LDAP Server", (const UTF8Char*)"LDAP Explorer", me);
		return;
	}
	succ = false;
	Net::LDAPClient::SearchResItem *item;
	UOSInt i = obj->items->GetCount();
	while (i-- > 0)
	{
		item = obj->items->GetItem(i);
		if (Text::StrEquals(item->type, (const UTF8Char*)"rootDomainNamingContext"))
		{
			Text::StringBuilderUTF8 sb;
			UTF8Char *sarr[32];
			UOSInt j;
			sb.Append(item->value);
			j = Text::StrSplit(sarr, 32, sb.ToString(), ',');
			me->rootLev = j;
			while (j-- > 0)
			{
				me->lbPath->AddItem(sarr[j], 0);
			}
			me->lbPath->SetSelectedIndex(me->rootLev - 1);
			succ = true;
			break;
		}
	}

	if (!succ)
	{
		Net::LDAPClient::SearchResultsFree(&results);
		DEL_CLASS(me->cli);
		me->cli = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char*)"rootDomainNamingContext not found in LDAP Server", (const UTF8Char*)"LDAP Explorer", me);
		return;
	}
	me->btnConnect->SetText((const UTF8Char*)"Disconnect");
}

void __stdcall SSWR::AVIRead::AVIRLDAPExplorerForm::OnPathSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRLDAPExplorerForm *me = (SSWR::AVIRead::AVIRLDAPExplorerForm*)userObj;
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

	if (me->cli == 0)
	{
		return;
	}

	UTF8Char sbuff[256];
	Text::StringBuilderUTF8 sb;
	me->lbPath->GetItemText(sbuff, i);
	sb.Append(sbuff);
	while (i-- > 0)
	{
		me->lbPath->GetItemText(sbuff, i);
		sb.AppendChar(',', 1);
		sb.Append(sbuff);
	}

	Data::ArrayList<Net::LDAPClient::SearchResObject*> results;
	if (me->cli->Search(sb.ToString(), Net::LDAPClient::ST_BASE_OBJECT, Net::LDAPClient::DT_DEREF_IN_SEARCHING, 0, 0, false, (const UTF8Char*)"", &results))
	{
		me->dispResults->Add(results.RemoveAt(0));
		Net::LDAPClient::SearchResultsFree(&results);
		me->lbObjects->AddItem((const UTF8Char*)".", me->dispResults->GetItem(0));
	}
	if (me->cli->Search(sb.ToString(), Net::LDAPClient::ST_SINGLE_LEVEL, Net::LDAPClient::DT_DEREF_IN_SEARCHING, 0, 0, false, (const UTF8Char*)"", &results))
	{
		UOSInt j;
		UOSInt k;
		Net::LDAPClient::SearchResObject *obj;
		i = 0;
		j = results.GetCount();
		while (i < j)
		{
			obj = results.GetItem(i);
			if (obj->isRef)
			{
				Net::LDAPClient::SearchResObjectFree(obj);
			}
			else
			{
				me->dispResults->Add(obj);
				sb.ClearStr();
				k = 0;
				while (true)
				{
					if (obj->name[k] == '\\')
					{
						if (obj->name[k + 1] != 0)
						{
							sb.AppendChar(obj->name[k + 1], 1);
							k += 2;
						}
						else
						{
							sb.AppendChar(obj->name[k], 1);
							k++;
						}
					}
					else if (obj->name[k] == ',')
					{
						break;
					}
					else if (obj->name[k] == 0)
					{
						break;
					}
					else
					{
						sb.AppendChar(obj->name[k], 1);
						k++;
					}
				}
				me->lbObjects->AddItem(sb.ToString(), obj);
			}
			i++;
		}
	}
	if (me->lbObjects->GetCount() > 0)
	{
		me->lbObjects->SetSelectedIndex(0);
	}
}

void __stdcall SSWR::AVIRead::AVIRLDAPExplorerForm::OnObjectsSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRLDAPExplorerForm *me = (SSWR::AVIRead::AVIRLDAPExplorerForm*)userObj;
	Net::LDAPClient::SearchResObject *obj = (Net::LDAPClient::SearchResObject*)me->lbObjects->GetSelectedItem();
	me->lvValues->ClearItems();
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
			k = me->lvValues->AddItem(item->type, 0);
			sb.ClearStr();
			Net::LDAPClient::SearchResDisplay(item->type, item->value, &sb);
			me->lvValues->SetSubItem(k, 1, sb.ToString());
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRLDAPExplorerForm::OnObjectsDblClk(void *userObj)
{
	SSWR::AVIRead::AVIRLDAPExplorerForm *me = (SSWR::AVIRead::AVIRLDAPExplorerForm*)userObj;
	UTF8Char sbuff[256];
	UOSInt i = me->lbObjects->GetSelectedIndex();
	if (i == INVALID_INDEX)
		return;
	me->lbObjects->GetItemText(sbuff, i);
	if (Text::StrEquals(sbuff, (const UTF8Char*)"."))
	{
		i = me->lbPath->GetSelectedIndex();
		if (i >= me->rootLev)
		{
			me->lbPath->SetSelectedIndex(i - 1);
		}
	}
	else
	{
		UOSInt j = me->lbPath->AddItem(sbuff, 0);
		me->lbPath->SetSelectedIndex(j);
	}
}

SSWR::AVIRead::AVIRLDAPExplorerForm::AVIRLDAPExplorerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"LDAP Explorer");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->rootLev = 0;

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

	NEW_CLASS(this->lbPath, UI::GUIListBox(ui, this, false));
	this->lbPath->SetRect(0, 0, 150, 23, false);
	this->lbPath->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbPath->HandleSelectionChange(OnPathSelChg, this);
	NEW_CLASS(this->hspPath, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->lbObjects, UI::GUIListBox(ui, this, false));
	this->lbObjects->SetRect(0, 0, 150, 23, false);
	this->lbObjects->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbObjects->HandleSelectionChange(OnObjectsSelChg, this);
	this->lbObjects->HandleDoubleClicked(OnObjectsDblClk, this);
	NEW_CLASS(this->hspObjects, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->lvValues, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvValues->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvValues->SetFullRowSelect(true);
	this->lvValues->SetShowGrid(true);
	this->lvValues->AddColumn((const UTF8Char*)"Type", 200);
	this->lvValues->AddColumn((const UTF8Char*)"Value", 500);

	this->cli = 0;
	NEW_CLASS(this->dispResults, Data::ArrayList<Net::LDAPClient::SearchResObject*>());
}

SSWR::AVIRead::AVIRLDAPExplorerForm::~AVIRLDAPExplorerForm()
{
	SDEL_CLASS(this->cli);
	Net::LDAPClient::SearchResultsFree(this->dispResults);
	DEL_CLASS(this->dispResults);
}

void SSWR::AVIRead::AVIRLDAPExplorerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
