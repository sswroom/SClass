#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/ASN1OIDDB.h"
#include "Net/ASN1Util.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRSNMPManagerForm.h"
#include "SSWR/AVIRead/AVIRSNMPWalkForm.h"
#include "Sync/SimpleThread.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRSNMPManagerForm::OnAgentAddClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNMPManagerForm *me = (SSWR::AVIRead::AVIRSNMPManagerForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Net::SocketUtil::AddressInfo addr;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	me->txtAgentAddr->GetText(sb);
	if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Error in parsing Agent Address"), CSTR("SNMP Manager"), me);
		return;
	}

	sb.ClearStr();
	me->txtCommunity->GetText(sb);
	if (sb.GetLength() <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter community"), CSTR("SNMP Manager"), me);
		return;
	}
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Data::ArrayList<Net::SNMPManager::AgentInfo *> agentList;
	NotNullPtr<Text::String> community = Text::String::New(sb.ToString(), sb.GetLength());
	j = me->mgr->AddAgents(addr, community, &agentList, me->chkAgentScan->IsChecked());
	community->Release();
	if (j <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Error in Adding Agent"), CSTR("SNMP Manager"), me);
	}
	else
	{
		Net::SNMPManager::AgentInfo *agent;
		if (me->chkSendToSvr->IsChecked())
		{
			Int64 cliId;
			UOSInt l;
			Net::SNMPManager::ReadingInfo *reading;
			Data::FastMap<UInt32, UInt16> readingMap;
			UInt16 currId;
			me->SendAgentValues(agentList);
			Sync::SimpleThread::Sleep(100);
			i = 0;
			while (i < j)
			{
				agent = agentList.GetItem(i);
				cliId = me->mgr->Agent2CliId(agent);
				if (agent->name)
				{
					me->redir->SendDevName(cliId, agent->name->v, agent->name->leng);
				}
				if (agent->model)
				{
					if (agent->vendor)
					{
						Text::StringBuilderUTF8 sbPlatform;
						sbPlatform.Append(agent->vendor);
						sbPlatform.AppendUTF8Char(' ');
						sbPlatform.Append(agent->model);
						me->redir->SendDevPlatform(cliId, sbPlatform.ToString(), sbPlatform.GetLength());
					}
					else
					{
						me->redir->SendDevPlatform(cliId, agent->model->v, agent->model->leng);
					}
				}
				if (agent->cpuName)
				{
					me->redir->SendDevPlatform(cliId, agent->cpuName->v, agent->cpuName->leng);
				}
				k = 0;
				l = agent->readingList->GetCount();
				while (k < l)
				{
					reading = agent->readingList->GetItem(k);
					currId = readingMap.Get((UInt32)reading->index);
					readingMap.Put((UInt32)reading->index, (UInt16)(currId + 1));
					me->redir->SendDevReadingName(cliId, k, (UInt16)reading->index, currId, reading->name->v, reading->name->leng);
					k++;
				}
				i++;
			}
		}
		i = 0;
		while (i < j)
		{
			agent = agentList.GetItem(i);
			sptr = Net::SocketUtil::GetAddrName(sbuff, agent->addr);
			k = me->lbAgent->AddItem(CSTRP(sbuff, sptr), agent);
			if (i == 0)
			{
				me->lbAgent->SetSelectedIndex(k);
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSNMPManagerForm::OnAgentSelChg(void *userObj)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	SSWR::AVIRead::AVIRSNMPManagerForm *me = (SSWR::AVIRead::AVIRSNMPManagerForm*)userObj;
	Net::SNMPManager::AgentInfo *agent = (Net::SNMPManager::AgentInfo*)me->lbAgent->GetSelectedItem();
	if (agent)
	{
		sptr = Net::SocketUtil::GetAddrName(sbuff, agent->addr);
		me->txtAgentDAddr->SetText(CSTRP(sbuff, sptr));
		me->txtAgentDescr->SetText(agent->descr->ToCString());
		if (agent->objIdLen > 0)
		{
			Text::StringBuilderUTF8 sb;
			Net::ASN1Util::OIDToString(agent->objId, agent->objIdLen, sb);
			me->txtAgentOID->SetText(sb.ToCString());
			sb.ClearStr();
			Net::ASN1OIDDB::OIDToNameString(agent->objId, agent->objIdLen, sb);
			me->txtAgentOIDName->SetText(sb.ToCString());
		}
		else
		{
			me->txtAgentOID->SetText(CSTR(""));
			me->txtAgentOIDName->SetText(CSTR(""));
		}
		if (agent->name)
		{
			me->txtAgentName->SetText(agent->name->ToCString());
		}
		else
		{
			me->txtAgentName->SetText(CSTR(""));
		}
		if (agent->contact)
		{
			me->txtAgentContact->SetText(agent->contact->ToCString());
		}
		else
		{
			me->txtAgentContact->SetText(CSTR(""));
		}
		if (agent->location)
		{
			me->txtAgentLocation->SetText(agent->location->ToCString());
		}
		else
		{
			me->txtAgentLocation->SetText(CSTR(""));
		}
		sptr = Text::StrHexBytes(sbuff, agent->mac, 6, ':');
		me->txtAgentPhyAddr->SetText(CSTRP(sbuff, sptr));
		const Net::MACInfo::MACEntry *ent = Net::MACInfo::GetMACInfoBuff(agent->mac);
		me->txtAgentVendor->SetText({ent->name, ent->nameLen});
		if (agent->model)
		{
			me->txtAgentModel->SetText(agent->model->ToCString());	
		}
		else
		{
			me->txtAgentModel->SetText(CSTR(""));	
		}
		me->lvAgentReading->ClearItems();
		UOSInt i = 0;
		UOSInt j = agent->readingList->GetCount();
		Net::SNMPManager::ReadingInfo *reading;
		while (i < j)
		{
			reading = agent->readingList->GetItem(i);
			me->lvAgentReading->AddItem(reading->name, reading);
			sptr = Text::StrUOSInt(sbuff, reading->index);
			me->lvAgentReading->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			me->lvAgentReading->SetSubItem(i, 2, SSWR::SMonitor::SAnalogSensor::GetReadingTypeName(reading->readingType));
			if (reading->valValid)
			{
				sptr = Text::StrDouble(sbuff, reading->currVal);
				me->lvAgentReading->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			}
			else
			{
				me->lvAgentReading->SetSubItem(i, 3, CSTR("-"));
			}
			
			i++;
		}
	}
	else
	{
		me->txtAgentDAddr->SetText(CSTR(""));
		me->txtAgentDescr->SetText(CSTR(""));
		me->txtAgentOID->SetText(CSTR(""));
		me->txtAgentOIDName->SetText(CSTR(""));
		me->txtAgentName->SetText(CSTR(""));
		me->txtAgentContact->SetText(CSTR(""));
		me->txtAgentLocation->SetText(CSTR(""));
		me->txtAgentAddr->SetText(CSTR(""));
		me->txtAgentVendor->SetText(CSTR(""));
		me->txtAgentModel->SetText(CSTR(""));
		me->lvAgentReading->ClearItems();
	}	
}

void __stdcall SSWR::AVIRead::AVIRSNMPManagerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRSNMPManagerForm *me = (SSWR::AVIRead::AVIRSNMPManagerForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	if (dt.ToTicks() - me->lastUpdateTime >= 30000)
	{
		Net::SNMPManager::ReadingInfo *reading;
		me->mgr->UpdateValues();
		UOSInt i = me->lvAgentReading->GetCount();
		while (i-- > 0)
		{
			reading = (Net::SNMPManager::ReadingInfo*)me->lvAgentReading->GetItem(i);
			if (reading->valValid)
			{
				sptr = Text::StrDouble(sbuff, reading->currVal);
				me->lvAgentReading->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			}
			else
			{
				me->lvAgentReading->SetSubItem(i, 3, CSTR("-"));
			}
		}
		if (me->chkSendToSvr->IsChecked())
		{
			Data::ArrayList<Net::SNMPManager::AgentInfo*> agentList;
			me->mgr->GetAgentList(agentList);
			if (agentList.GetCount() > 0)
			{
				me->SendAgentValues(agentList);
			}
		}
		dt.SetCurrTimeUTC();
		me->lastUpdateTime = dt.ToTicks();
	}
}

void __stdcall SSWR::AVIRead::AVIRSNMPManagerForm::OnAgentWalkClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNMPManagerForm *me = (SSWR::AVIRead::AVIRSNMPManagerForm*)userObj;
	Net::SNMPManager::AgentInfo *agent = (Net::SNMPManager::AgentInfo*)me->lbAgent->GetSelectedItem();
	if (agent)
	{
		SSWR::AVIRead::AVIRSNMPWalkForm frm(0, me->ui, me->core, agent->addr, agent->community);
		frm.ShowDialog(me);
	}
}

void SSWR::AVIRead::AVIRSNMPManagerForm::SendAgentValues(NotNullPtr<Data::ArrayList<Net::SNMPManager::AgentInfo *>> agentList)
{
	Net::SNMPManager::AgentInfo *agent;
	SSWR::SMonitor::ISMonitorCore::DevRecord2 devRec;
	Int64 cliId;
	UOSInt i = agentList->GetCount();
	while (i-- > 0)
	{
		agent = agentList->GetItem(i);
		Net::SNMPManager::Agent2Record(agent, &devRec, &cliId);
		this->redir->SendDevReading(cliId, &devRec, 30, 30);
	}
}

SSWR::AVIRead::AVIRSNMPManagerForm::AVIRSNMPManagerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("SNMP Manager"));

	this->core = core;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	this->lastUpdateTime = dt.ToTicks();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASSNN(this->pnlControl, UI::GUIPanel(ui, *this));
	this->pnlControl->SetRect(0, 0, 100, 104, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblAgentAddr, UI::GUILabel(ui, this->pnlControl, CSTR("Agent Addr")));
	this->lblAgentAddr->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtAgentAddr, UI::GUITextBox(ui, this->pnlControl, CSTR("")));
	this->txtAgentAddr->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->chkAgentScan, UI::GUICheckBox(ui, this->pnlControl, CSTR("Scan IP"), false));
	this->chkAgentScan->SetRect(304, 4, 100, 23, false);
	NEW_CLASS(this->lblCommunity, UI::GUILabel(ui, this->pnlControl, CSTR("Community")));
	this->lblCommunity->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtCommunity, UI::GUITextBox(ui, this->pnlControl, CSTR("public")));
	this->txtCommunity->SetRect(104, 28, 200, 23, false);
	this->btnAgentAdd = ui->NewButton(this->pnlControl, CSTR("Add"));
	this->btnAgentAdd->SetRect(104, 52, 75, 23, false);
	this->btnAgentAdd->HandleButtonClick(OnAgentAddClicked, this);
	NEW_CLASS(this->chkSendToSvr, UI::GUICheckBox(ui, this->pnlControl, CSTR("Send to Server"), false));
	this->chkSendToSvr->SetRect(104, 76, 150, 23, false);
	NEW_CLASS(this->lbAgent, UI::GUIListBox(ui, *this, false));
	this->lbAgent->SetRect(0, 0, 150, 23, false);
	this->lbAgent->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbAgent->HandleSelectionChange(OnAgentSelChg, this);
	this->hspAgent = ui->NewHSplitter(*this, 3, false);
	NEW_CLASSNN(this->pnlAgent, UI::GUIPanel(ui, *this));
	this->pnlAgent->SetRect(0, 0, 100, 248, false);
	this->pnlAgent->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblAgentDAddr, UI::GUILabel(ui, this->pnlAgent, CSTR("Agent Address")));
	this->lblAgentDAddr->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtAgentDAddr, UI::GUITextBox(ui, this->pnlAgent, CSTR("")));
	this->txtAgentDAddr->SetRect(104, 4, 150, 23, false);
	this->txtAgentDAddr->SetReadOnly(true);
	this->btnAgentWalk = ui->NewButton(this->pnlAgent, CSTR("Walk"));
	this->btnAgentWalk->SetRect(254, 4, 75, 23, false);
	this->btnAgentWalk->HandleButtonClick(OnAgentWalkClicked, this);
	NEW_CLASS(this->lblAgentDescr, UI::GUILabel(ui, this->pnlAgent, CSTR("Description")));
	this->lblAgentDescr->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtAgentDescr, UI::GUITextBox(ui, this->pnlAgent, CSTR("")));
	this->txtAgentDescr->SetRect(104, 28, 500, 23, false);
	this->txtAgentDescr->SetReadOnly(true);
	NEW_CLASS(this->lblAgentOID, UI::GUILabel(ui, this->pnlAgent, CSTR("OID")));
	this->lblAgentOID->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtAgentOID, UI::GUITextBox(ui, this->pnlAgent, CSTR("")));
	this->txtAgentOID->SetRect(104, 52, 150, 23, false);
	this->txtAgentOID->SetReadOnly(true);
	NEW_CLASS(this->lblAgentOIDName, UI::GUILabel(ui, this->pnlAgent, CSTR("OID Name")));
	this->lblAgentOIDName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtAgentOIDName, UI::GUITextBox(ui, this->pnlAgent, CSTR("")));
	this->txtAgentOIDName->SetRect(104, 76, 150, 23, false);
	this->txtAgentOIDName->SetReadOnly(true);
	NEW_CLASS(this->lblAgentName, UI::GUILabel(ui, this->pnlAgent, CSTR("Name")));
	this->lblAgentName->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtAgentName, UI::GUITextBox(ui, this->pnlAgent, CSTR("")));
	this->txtAgentName->SetRect(104, 100, 150, 23, false);
	this->txtAgentName->SetReadOnly(true);
	NEW_CLASS(this->lblAgentContact, UI::GUILabel(ui, this->pnlAgent, CSTR("Contact")));
	this->lblAgentContact->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtAgentContact, UI::GUITextBox(ui, this->pnlAgent, CSTR("")));
	this->txtAgentContact->SetRect(104, 124, 150, 23, false);
	this->txtAgentContact->SetReadOnly(true);
	NEW_CLASS(this->lblAgentLocation, UI::GUILabel(ui, this->pnlAgent, CSTR("Location")));
	this->lblAgentLocation->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtAgentLocation, UI::GUITextBox(ui, this->pnlAgent, CSTR("")));
	this->txtAgentLocation->SetRect(104, 148, 150, 23, false);
	this->txtAgentLocation->SetReadOnly(true);
	NEW_CLASS(this->lblAgentPhyAddr, UI::GUILabel(ui, this->pnlAgent, CSTR("Phy Addr")));
	this->lblAgentPhyAddr->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtAgentPhyAddr, UI::GUITextBox(ui, this->pnlAgent, CSTR("")));
	this->txtAgentPhyAddr->SetRect(104, 172, 150, 23, false);
	this->txtAgentPhyAddr->SetReadOnly(true);
	NEW_CLASS(this->lblAgentVendor, UI::GUILabel(ui, this->pnlAgent, CSTR("Vendor")));
	this->lblAgentVendor->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtAgentVendor, UI::GUITextBox(ui, this->pnlAgent, CSTR("")));
	this->txtAgentVendor->SetRect(104, 196, 150, 23, false);
	this->txtAgentVendor->SetReadOnly(true);
	NEW_CLASS(this->lblAgentModel, UI::GUILabel(ui, this->pnlAgent, CSTR("Model")));
	this->lblAgentModel->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtAgentModel, UI::GUITextBox(ui, this->pnlAgent, CSTR("")));
	this->txtAgentModel->SetRect(104, 220, 150, 23, false);
	this->txtAgentModel->SetReadOnly(true);
	NEW_CLASS(this->lvAgentReading, UI::GUIListView(ui, *this, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvAgentReading->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvAgentReading->SetShowGrid(true);
	this->lvAgentReading->SetFullRowSelect(true);
	this->lvAgentReading->AddColumn(CSTR("Name"), 150);
	this->lvAgentReading->AddColumn(CSTR("Index"), 60);
	this->lvAgentReading->AddColumn(CSTR("Type"), 100);
	this->lvAgentReading->AddColumn(CSTR("Value"), 150);

	this->AddTimer(1000, OnTimerTick, this);

	NotNullPtr<Net::SocketFactory> sockf = this->core->GetSocketFactory();
	NEW_CLASS(this->redir, SSWR::SMonitor::SMonitorRedir(sockf, this->core->GetLog()));
	NEW_CLASS(this->mgr, Net::SNMPManager(sockf, this->core->GetLog()));
	if (this->mgr->IsError())
	{
		this->ui->ShowMsgOK(CSTR("Error in starting SNMP Manager"), CSTR("Error"), this);
	}

	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UInt32 ip;
	UInt32 netmask;
	sockf->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		ip = connInfo->GetDefaultGW();
		if (ip)
		{
			netmask = Net::SocketUtil::GetDefNetMaskv4(ip);
			ip |= ~netmask;
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
			this->txtAgentAddr->SetText(CSTRP(sbuff, sptr));
		}
		DEL_CLASS(connInfo);
		i++;
	}
}

SSWR::AVIRead::AVIRSNMPManagerForm::~AVIRSNMPManagerForm()
{
	DEL_CLASS(this->mgr);
	DEL_CLASS(this->redir);
}

void SSWR::AVIRead::AVIRSNMPManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Bool SSWR::AVIRead::AVIRSNMPManagerForm::IsError()
{
	return this->mgr->IsError();
}
