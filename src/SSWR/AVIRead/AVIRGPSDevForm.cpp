#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/TCPClient.h"
#include "SSWR/AVIRead/AVIRGPSDevForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyStringW.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall SSWR::AVIRead::AVIRGPSDevForm::ClientThread(void *userObj)
{
	SSWR::AVIRead::AVIRGPSDevForm *me = (SSWR::AVIRead::AVIRGPSDevForm*)userObj;
	UInt8 *recvBuff;
	UOSInt recvBuffSize;
	UOSInt readSize;
	recvBuff = MemAlloc(UInt8, 16384);
	recvBuffSize = 0;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		if (me->cli)
		{
			readSize = me->cli->Read(&recvBuff[recvBuffSize], 16384 - recvBuffSize);

			if (readSize == 0)
			{
				Sync::MutexUsage mutUsage(me->cliMut);
				SDEL_CLASS(me->cli);
				mutUsage.EndUse();
				recvBuffSize = 0;
			}
			else
			{
				recvBuffSize += readSize;
				readSize = me->protoHdlr->ParseProtocol(me->cli, 0, me, recvBuff, recvBuffSize);
				if (readSize == 0)
				{
					recvBuffSize = 0;
				}
				else if (readSize >= recvBuffSize)
				{
				}
				else
				{
					MemCopyO(recvBuff, &recvBuff[recvBuffSize - readSize], readSize);
					recvBuffSize = readSize;
				}
			}
		}
		else
		{
			me->threadEvt->Wait(1000);
		}
	}
	MemFree(recvBuff);
	me->threadRunning = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnConnClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSDevForm *me = (SSWR::AVIRead::AVIRGPSDevForm*)userObj;
	if (me->cli)
	{
		me->ToStop();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port = 0;
		me->txtPort->GetText(&sb);
		sb.ToUInt16(&port);
		sb.ClearStr();
		me->txtHost->GetText(&sb);
		if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToString(), &addr) || port <= 0 || port >= 65536)
		{

		}
		else
		{
			Net::TCPClient *cli;
			NEW_CLASS(cli, Net::TCPClient(me->core->GetSocketFactory(), &addr, port));
			if (cli->IsClosed())
			{
				DEL_CLASS(cli);
			}
			else
			{
				me->dispConn = true;
				me->txtStatusConn->SetText((const UTF8Char*)"Connected");
				me->txtHost->SetReadOnly(true);
				me->txtPort->SetReadOnly(true);
				me->cli = cli;
				me->threadEvt->Set();
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnDeviceRClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSDevForm *me = (SSWR::AVIRead::AVIRGPSDevForm*)userObj;
	me->SendGetDevices();
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnUserRClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSDevForm *me = (SSWR::AVIRead::AVIRGPSDevForm*)userObj;
	me->SendGetUsers();
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnAlertRClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSDevForm *me = (SSWR::AVIRead::AVIRGPSDevForm*)userObj;
	me->SendGetAlerts();
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnDeviceSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRGPSDevForm *me = (SSWR::AVIRead::AVIRGPSDevForm*)userObj;
	Text::String *s = me->lbDevice->GetSelectedItemTextNew();
	if (s)
	{
		me->SendGetDevice(s->ToInt64());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnAlertSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRGPSDevForm *me = (SSWR::AVIRead::AVIRGPSDevForm*)userObj;
	Text::String *s = me->lbAlert->GetSelectedItemTextNew();
	if (s)
	{
		me->SendGetAlert(s->ToInt32());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnUserSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRGPSDevForm *me = (SSWR::AVIRead::AVIRGPSDevForm*)userObj;
	Text::String *s = me->lbUser->GetSelectedItemTextNew();
	if (s)
	{
		me->SendGetUser(s->ToInt32());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRGPSDevForm *me = (SSWR::AVIRead::AVIRGPSDevForm*)userObj;
	UTF8Char sbuff[64];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	if (me->cli == 0 && me->dispConn)
	{
		me->dispConn = false;
		me->txtHost->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		me->txtStatusConn->SetText((const UTF8Char*)"Not Connected");
	}
	if (me->alertUpd)
	{
		Sync::MutexUsage mutUsage(me->alertMut);
		me->alertUpd = false;
		me->lbAlert->ClearItems();
		i = 0;
		j = me->alertList->GetCount();
		while (i < j)
		{
			Text::StrInt32(sbuff, me->alertList->GetItem(i));
			me->lbAlert->AddItem(sbuff, 0);
			i++;
		}
		mutUsage.EndUse();
	}
	if (me->deviceUpd)
	{
		Sync::MutexUsage mutUsage(me->deviceMut);
		me->deviceUpd = false;
		me->lbDevice->ClearItems();
		i = 0;
		j = me->deviceList->GetCount();
		while (i < j)
		{
			Text::StrInt64(sbuff, me->deviceList->GetItem(i));
			me->lbDevice->AddItem(sbuff, 0);
			i++;
		}
		mutUsage.EndUse();
	}
	if (me->userUpd)
	{
		Sync::MutexUsage mutUsage(me->userMut);
		me->userUpd = false;
		me->lbUser->ClearItems();
		i = 0;
		j = me->userList->GetCount();
		while (i < j)
		{
			Text::StrInt32(sbuff, me->userList->GetItem(i));
			me->lbUser->AddItem(sbuff, 0);
			i++;
		}
		mutUsage.EndUse();
	}
	if (me->devContUpd)
	{
		Data::DateTime dt;
		me->devContUpd = false;
		Sync::MutexUsage mutUsage(me->devContMut);
		me->lvDevInfo->ClearItems();
		i = 0;
		j = me->devConts->GetCount();
		while (i < j)
		{
			k = me->lvDevInfo->AddItem(me->devConts->GetItem(i), 0);
			me->lvDevInfo->SetSubItem(k, 1, me->devConts->GetItem(i + 1));
			i += 2;
		}
		me->lvDevAlert->ClearItems();
		dt.ToUTCTime();
		i = 0;
		j = me->devContACnt;
		while (i < j)
		{
			Text::StrInt32(sbuff, me->devContAlerts[i].alertId);
			k = me->lvDevAlert->AddItem(sbuff, 0);
			me->lvDevAlert->SetSubItem(k, 1, me->devContAlerts[i].isAlerting?(const UTF8Char*)"1":(const UTF8Char*)"0");
			me->lvDevAlert->SetSubItem(k, 2, me->devContAlerts[i].isAlerted?(const UTF8Char*)"1":(const UTF8Char*)"0");
			me->lvDevAlert->SetSubItem(k, 3, me->devContAlerts[i].isFirst?(const UTF8Char*)"1":(const UTF8Char*)"0");
			dt.SetTicks(me->devContAlerts[i].beginTimeTick);
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->lvDevAlert->SetSubItem(k, 4, sbuff);
			dt.SetTicks(me->devContAlerts[i].lastTimeTick);
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->lvDevAlert->SetSubItem(k, 5, sbuff);
			i++;
		}
		me->lvDevGuard->ClearItems();
		i = 0;
		j = me->devContGCnt;
		while (i < j)
		{
			Text::StrInt32(sbuff, me->devContGuards[i].userId);
			k = me->lvDevGuard->AddItem(sbuff, 0);
			me->lvDevGuard->SetSubItem(k, 1, me->devContGuards[i].found?L"1":L"0");
			Text::StrDouble(sbuff, me->devContGuards[i].alarmLat);
			me->lvDevGuard->SetSubItem(k, 2, sbuff);
			Text::StrDouble(sbuff, me->devContGuards[i].alarmLon);
			me->lvDevGuard->SetSubItem(k, 3, sbuff);
			Text::StrInt32(sbuff, me->devContGuards[i].alarmType);
			me->lvDevGuard->SetSubItem(k, 4, sbuff);
			dt.SetTicks(me->devContGuards[i].alarmStartTicks);
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->lvDevGuard->SetSubItem(k, 5, sbuff);
			Text::StrInt32(sbuff, me->devContGuards[i].alarmStatus);
			me->lvDevGuard->SetSubItem(k, 6, sbuff);
			Text::StrInt32(sbuff, me->devContGuards[i].alarmStatus2);
			me->lvDevGuard->SetSubItem(k, 7, sbuff);
			Text::StrInt32(sbuff, me->devContGuards[i].alarmStatus3);
			me->lvDevGuard->SetSubItem(k, 8, sbuff);
			Text::StrInt32(sbuff, me->devContGuards[i].alarmStatus4);
			me->lvDevGuard->SetSubItem(k, 9, sbuff);
			Text::StrInt32(sbuff, me->devContGuards[i].alarmStatus5);
			me->lvDevGuard->SetSubItem(k, 10, sbuff);
			i++;
		}
		mutUsage.EndUse();
	}
	if (me->alertContUpd)
	{
		me->alertContUpd = false;
		Sync::MutexUsage mutUsage(me->alertContMut);
		me->lvAlert->ClearItems();
		i = 0;
		j = me->alertConts->GetCount();
		while (i < j)
		{
			k = me->lvAlert->AddItem(me->alertConts->GetItem(i), 0);
			me->lvAlert->SetSubItem(k, 1, me->alertConts->GetItem(i + 1));
			i += 2;
		}
		me->lbAlertDev->ClearItems();
		i = 0;
		j = me->alertContDevs->GetCount();
		while (i < j)
		{
			Text::StrInt64(sbuff, me->alertContDevs->GetItem(i));
			me->lbAlertDev->AddItem(sbuff, 0);;
			i++;
		}
		mutUsage.EndUse();
	}
	if (me->userContUpd)
	{
		me->userContUpd = false;
		Sync::MutexUsage mutUsage(me->userContMut);
		me->lvUser->ClearItems();
		i = 0;
		j = me->userConts->GetCount();
		while (i < j)
		{
			k = me->lvUser->AddItem(me->userConts->GetItem(i), 0);
			me->lvUser->SetSubItem(k, 1, me->userConts->GetItem(i + 1));
			i += 2;
		}
		me->lbUserDev->ClearItems();
		i = 0;
		j = me->userContDevs->GetCount();
		while (i < j)
		{
			Text::StrInt64(sbuff, me->userContDevs->GetItem(i));
			me->lbUserDev->AddItem(sbuff, 0);;
			i++;
		}
		mutUsage.EndUse();
	}
}

void SSWR::AVIRead::AVIRGPSDevForm::ToStop()
{
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Close();
		mutUsage.EndUse();
		while (this->cli)
		{
			Sync::Thread::Sleep(10);
		}
	}
	else
	{
		mutUsage.EndUse();
	}
}

void SSWR::AVIRead::AVIRGPSDevForm::ClearDevConts()
{
	UOSInt i;
	Sync::MutexUsage mutUsage(this->devContMut);
	i = this->devConts->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->devConts->GetItem(i));
	}
	this->devConts->Clear();
	if (this->devContAlerts)
	{
		MemFree(this->devContAlerts);
		this->devContAlerts = 0;
	}
	this->devContACnt = 0;
	if (this->devContGuards)
	{
		MemFree(this->devContGuards);
		this->devContGuards = 0;
	}
	this->devContGCnt = 0;
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRGPSDevForm::ClearAlertConts()
{
	UOSInt i;
	Sync::MutexUsage mutUsage(this->alertContMut);
	i = this->alertConts->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->alertConts->GetItem(i));
	}
	this->alertConts->Clear();
	this->alertContDevs->Clear();
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRGPSDevForm::ClearUserConts()
{
	UOSInt i;
	Sync::MutexUsage mutUsage(this->userContMut);
	i = this->userConts->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->userConts->GetItem(i));
	}
	this->userConts->Clear();
	this->userContDevs->Clear();
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRGPSDevForm::SendGetAlerts()
{
	UInt8 buff[12];
	UOSInt cmdSize = this->protoHdlr->BuildPacket(buff, 0, 0, 0, 0, 0);
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Write(buff, cmdSize);
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRGPSDevForm::SendGetDevices()
{
	UInt8 buff[12];
	UOSInt cmdSize = this->protoHdlr->BuildPacket(buff, 2, 0, 0, 0, 0);
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Write(buff, cmdSize);
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRGPSDevForm::SendGetUsers()
{
	UInt8 buff[12];
	UOSInt cmdSize = this->protoHdlr->BuildPacket(buff, 4, 0, 0, 0, 0);
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Write(buff, cmdSize);
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRGPSDevForm::SendGetDevice(Int64 devId)
{
	UInt8 buff[20];
	UOSInt cmdSize = this->protoHdlr->BuildPacket(buff, 6, 0, (UInt8*)&devId, 8, 0);
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Write(buff, cmdSize);
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRGPSDevForm::SendGetAlert(Int32 alertId)
{
	UInt8 buff[20];
	UOSInt cmdSize = this->protoHdlr->BuildPacket(buff, 8, 0, (UInt8*)&alertId, 4, 0);
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Write(buff, cmdSize);
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRGPSDevForm::SendGetUser(Int32 userId)
{
	UInt8 buff[20];
	UOSInt cmdSize = this->protoHdlr->BuildPacket(buff, 10, 0, (UInt8*)&userId, 4, 0);
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Write(buff, cmdSize);
	}
	mutUsage.EndUse();
}

SSWR::AVIRead::AVIRGPSDevForm::AVIRGPSDevForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"GPSDev Viewer");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->cli = 0;
	NEW_CLASS(this->cliMut, Sync::Mutex());
	NEW_CLASS(this->protoHdlr, IO::ProtoHdlr::ProtoGPSDevInfoHandler(this));

	NEW_CLASS(this->alertMut, Sync::Mutex());
	NEW_CLASS(this->alertList, Data::ArrayList<Int32>());
	this->alertUpd = false;
	NEW_CLASS(this->deviceMut, Sync::Mutex());
	NEW_CLASS(this->deviceList, Data::ArrayList<Int64>());
	this->deviceUpd = false;
	NEW_CLASS(this->userMut, Sync::Mutex());
	NEW_CLASS(this->userList, Data::ArrayList<Int32>());
	this->userUpd = false;
	this->devContUpd = false;
	NEW_CLASS(this->devContMut, Sync::Mutex());
	NEW_CLASS(this->devConts, Data::ArrayList<const UTF8Char *>());
	this->devContACnt = 0;
	this->devContAlerts = 0;
	this->devContGCnt = 0;
	this->devContGuards = 0;
	this->alertContUpd = false;
	NEW_CLASS(this->alertContMut, Sync::Mutex());
	NEW_CLASS(this->alertContDevs, Data::ArrayList<Int64>());
	NEW_CLASS(this->alertConts, Data::ArrayList<const UTF8Char*>());
	this->userContUpd = false;
	NEW_CLASS(this->userContMut, Sync::Mutex());
	NEW_CLASS(this->userContDevs, Data::ArrayList<Int64>());
	NEW_CLASS(this->userConts, Data::ArrayList<const UTF8Char*>());

	NEW_CLASS(this->pnlConn, UI::GUIPanel(ui, this));
	this->pnlConn->SetRect(0, 0, 100, 56, false);
	this->pnlConn->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this->pnlConn, (const UTF8Char*)"Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this->pnlConn, (const UTF8Char*)"127.0.0.1"));
	this->txtHost->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlConn, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlConn, (const UTF8Char*)"7500"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->btnConn, UI::GUIButton(ui, this->pnlConn, (const UTF8Char*)"Start"));
	this->btnConn->SetRect(204, 28, 75, 23, false);
	this->btnConn->HandleButtonClick(OnConnClicked, this);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpStatus = this->tcMain->AddTabPage((const UTF8Char*)"Status");
	NEW_CLASS(this->lblStatusConn, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Conn Status"));
	this->lblStatusConn->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStatusConn, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)"Not Connected"));
	this->txtStatusConn->SetRect(104, 4, 100, 23, false);
	this->txtStatusConn->SetReadOnly(true);

	this->tpDevice = this->tcMain->AddTabPage((const UTF8Char*)"Device");
	NEW_CLASS(this->pnlDeviceC, UI::GUIPanel(ui, this->tpDevice));
	this->pnlDeviceC->SetRect(0, 0, 200, 23, false);
	this->pnlDeviceC->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->btnDeviceR, UI::GUIButton(ui, this->pnlDeviceC, (const UTF8Char*)"Refresh"));
	this->btnDeviceR->SetRect(0, 0, 100, 23, false);
	this->btnDeviceR->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDeviceR->HandleButtonClick(OnDeviceRClicked, this);
	NEW_CLASS(this->lbDevice, UI::GUIListBox(ui, this->pnlDeviceC, false));
	this->lbDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbDevice->HandleSelectionChange(OnDeviceSelChg, this);
	NEW_CLASS(this->hspDevice, UI::GUIHSplitter(ui, this->tpDevice, 3, false));
	NEW_CLASS(this->tcDevice, UI::GUITabControl(ui, this->tpDevice));
	this->tcDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpDevInfo = this->tcDevice->AddTabPage((const UTF8Char*)"Info");
	NEW_CLASS(this->lvDevInfo, UI::GUIListView(ui, this->tpDevInfo, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvDevInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevInfo->SetFullRowSelect(true);
	this->lvDevInfo->SetShowGrid(true);
	this->lvDevInfo->AddColumn((const UTF8Char*)"Name", 200);
	this->lvDevInfo->AddColumn((const UTF8Char*)"Value", 400);
	this->tpDevAlert = this->tcDevice->AddTabPage((const UTF8Char*)"Alert");
	NEW_CLASS(this->lvDevAlert, UI::GUIListView(ui, this->tpDevAlert, UI::GUIListView::LVSTYLE_TABLE, 6));
	this->lvDevAlert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevAlert->SetFullRowSelect(true);
	this->lvDevAlert->SetShowGrid(true);
	this->lvDevAlert->AddColumn((const UTF8Char*)"AlertId", 60);
	this->lvDevAlert->AddColumn((const UTF8Char*)"isAlerting", 60);
	this->lvDevAlert->AddColumn((const UTF8Char*)"isAlerted", 60);
	this->lvDevAlert->AddColumn((const UTF8Char*)"isFirst", 60);
	this->lvDevAlert->AddColumn((const UTF8Char*)"Begin Time (UTC)", 200);
	this->lvDevAlert->AddColumn((const UTF8Char*)"Last Time (UTC)", 200);
	this->tpDevGuard = this->tcDevice->AddTabPage((const UTF8Char*)"Guard");
	NEW_CLASS(this->lvDevGuard, UI::GUIListView(ui, this->tpDevGuard, UI::GUIListView::LVSTYLE_TABLE, 11));
	this->lvDevGuard->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevGuard->SetFullRowSelect(true);
	this->lvDevGuard->SetShowGrid(true);
	this->lvDevGuard->AddColumn((const UTF8Char*)"User Id", 60);
	this->lvDevGuard->AddColumn((const UTF8Char*)"Found", 60);
	this->lvDevGuard->AddColumn((const UTF8Char*)"AlarmLat", 100);
	this->lvDevGuard->AddColumn((const UTF8Char*)"AlarmLon", 100);
	this->lvDevGuard->AddColumn((const UTF8Char*)"AlarmType", 60);
	this->lvDevGuard->AddColumn((const UTF8Char*)"AlarmStart",200);
	this->lvDevGuard->AddColumn((const UTF8Char*)"AlarmStatus", 60);
	this->lvDevGuard->AddColumn((const UTF8Char*)"AlarmStatus2", 60);
	this->lvDevGuard->AddColumn((const UTF8Char*)"AlarmStatus3", 60);
	this->lvDevGuard->AddColumn((const UTF8Char*)"AlarmStatus4", 60);
	this->lvDevGuard->AddColumn((const UTF8Char*)"AlarmStatus5", 60);

	this->tpUser = this->tcMain->AddTabPage((const UTF8Char*)"User");
	NEW_CLASS(this->pnlUserC, UI::GUIPanel(ui, this->tpUser));
	this->pnlUserC->SetRect(0, 0, 200, 23, false);
	this->pnlUserC->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->btnUserR, UI::GUIButton(ui, this->pnlUserC, (const UTF8Char*)"Refresh"));
	this->btnUserR->SetRect(0, 0, 100, 23, false);
	this->btnUserR->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnUserR->HandleButtonClick(OnUserRClicked, this);
	NEW_CLASS(this->lbUser, UI::GUIListBox(ui, this->pnlUserC, false));
	this->lbUser->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbUser->HandleSelectionChange(OnUserSelChg, this);
	NEW_CLASS(this->hspUser, UI::GUIHSplitter(ui, this->tpUser, 3, false));
	NEW_CLASS(this->pnlUser, UI::GUIPanel(ui, this->tpUser));
	this->pnlUser->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lbUserDev, UI::GUIListBox(ui, this->pnlUser, false));
	this->lbUserDev->SetRect(0, 0, 150, 23, false);
	this->lbUserDev->SetDockType(UI::GUIControl::DOCK_RIGHT);
	NEW_CLASS(this->lvUser, UI::GUIListView(ui, this->pnlUser, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvUser->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvUser->SetShowGrid(true);
	this->lvUser->SetFullRowSelect(true);
	this->lvUser->AddColumn((const UTF8Char*)"Name", 200);
	this->lvUser->AddColumn((const UTF8Char*)"Value", 400);

	this->tpAlert = this->tcMain->AddTabPage((const UTF8Char*)"Alert");
	NEW_CLASS(this->pnlAlertC, UI::GUIPanel(ui, this->tpAlert));
	this->pnlAlertC->SetRect(0, 0, 200, 23, false);
	this->pnlAlertC->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->btnAlertR, UI::GUIButton(ui, this->pnlAlertC, (const UTF8Char*)"Refresh"));
	this->btnAlertR->SetRect(0, 0, 100, 23, false);
	this->btnAlertR->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnAlertR->HandleButtonClick(OnAlertRClicked, this);
	NEW_CLASS(this->lbAlert, UI::GUIListBox(ui, this->pnlAlertC, false));
	this->lbAlert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbAlert->HandleSelectionChange(OnAlertSelChg, this);
	NEW_CLASS(this->hspAlert, UI::GUIHSplitter(ui, this->tpAlert, 3, false));
	NEW_CLASS(this->pnlAlert, UI::GUIPanel(ui, this->tpAlert));
	this->pnlAlert->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lbAlertDev, UI::GUIListBox(ui, this->pnlAlert, false));
	this->lbAlertDev->SetRect(0, 0, 150, 23, false);
	this->lbAlertDev->SetDockType(UI::GUIControl::DOCK_RIGHT);
	NEW_CLASS(this->lvAlert, UI::GUIListView(ui, this->pnlAlert, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvAlert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvAlert->SetShowGrid(true);
	this->lvAlert->SetFullRowSelect(true);
	this->lvAlert->AddColumn((const UTF8Char*)"Name", 200);
	this->lvAlert->AddColumn((const UTF8Char*)"Value", 400);

	this->dispConn = false;
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRGPSDevForm.threadEvt"));
	Sync::Thread::Create(ClientThread, this);
	while (!this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRGPSDevForm::~AVIRGPSDevForm()
{
	this->threadToStop = true;
	ToStop();
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->threadEvt);
	DEL_CLASS(this->protoHdlr);
	DEL_CLASS(this->cliMut);

	this->ClearDevConts();
	this->ClearAlertConts();
	this->ClearUserConts();
	DEL_CLASS(this->devConts);
	DEL_CLASS(this->devContMut);
	DEL_CLASS(this->alertList);
	DEL_CLASS(this->alertMut);
	DEL_CLASS(this->deviceList);
	DEL_CLASS(this->deviceMut);
	DEL_CLASS(this->userList);
	DEL_CLASS(this->userMut);
	DEL_CLASS(this->alertContMut);
	DEL_CLASS(this->alertConts);
	DEL_CLASS(this->alertContDevs);
	DEL_CLASS(this->userContMut);
	DEL_CLASS(this->userConts);
	DEL_CLASS(this->userContDevs);
}

void SSWR::AVIRead::AVIRGPSDevForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGPSDevForm::DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
	switch (cmdType)
	{
	case 1:
		if (cmdSize >= 4)
		{
			UOSInt cnt = ReadUInt32(&cmd[0]);
			OSInt i;
			if (cmdSize >= cnt * 4 + 4)
			{
				Sync::MutexUsage mutUsage(this->alertMut);
				this->alertList->Clear();
				i = 4;
				while (cnt-- > 0)
				{
					this->alertList->Add(ReadInt32(&cmd[i]));
					i += 4;
				}
				this->alertUpd = true;
				mutUsage.EndUse();
			}
		}
		break;
	case 3:
		if (cmdSize >= 4)
		{
			UOSInt cnt = ReadUInt32(&cmd[0]);
			OSInt i;
			if (cmdSize >= cnt * 8 + 4)
			{
				Sync::MutexUsage mutUsage(this->deviceMut);
				this->deviceList->Clear();
				i = 4;
				while (cnt-- > 0)
				{
					this->deviceList->Add(ReadInt64(&cmd[i]));
					i += 8;
				}
				this->deviceUpd = true;
				mutUsage.EndUse();
			}
		}
		break;
	case 5:
		if (cmdSize >= 4)
		{
			UOSInt cnt = ReadUInt32(&cmd[0]);
			OSInt i;
			if (cmdSize >= cnt * 4 + 4)
			{
				Sync::MutexUsage mutUsage(this->userMut);
				this->userList->Clear();
				i = 4;
				while (cnt-- > 0)
				{
					this->userList->Add(ReadInt32(&cmd[i]));
					i += 4;
				}
				this->userUpd = true;
				mutUsage.EndUse();
			}
		}
		break;
	case 7:
		if (cmdSize > 100)
		{
			UTF8Char sbuff[32];
			UInt32 startSize = ReadUInt32(&cmd[0]);
			UOSInt i;
			UOSInt j;
			UOSInt k;
			if (startSize >= 98 && startSize <= cmdSize)
			{
				Data::DateTime dt;
				this->ClearDevConts();
				Sync::MutexUsage mutUsage(this->devContMut);
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"CompId"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[4]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"DevId"));
				Text::StrInt64(sbuff, ReadInt64(&cmd[8]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"IP Time"));
				dt.SetTicks(ReadInt64(&cmd[16]));
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"Last Signal Time"));
				dt.SetTicks(ReadInt64(&cmd[24]));
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"Last Loc Time"));
				dt.SetTicks(ReadInt64(&cmd[32]));
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"LastLat"));
				Text::StrDouble(sbuff, ReadDouble(&cmd[40]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"LastLon"));
				Text::StrDouble(sbuff, ReadDouble(&cmd[48]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"GuardDist"));
				Text::StrDouble(sbuff, ReadDouble(&cmd[56]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"GuardFlags"));
				Text::StrHexVal32(sbuff, ReadUInt32(&cmd[64]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"GuardFlags2"));
				Text::StrHexVal32(sbuff, ReadUInt32(&cmd[68]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"GuardFlags3"));
				Text::StrHexVal32(sbuff, ReadUInt32(&cmd[72]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"GuardFlags4"));
				Text::StrHexVal32(sbuff, ReadUInt32(&cmd[76]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"GuardFlags5"));
				Text::StrHexVal32(sbuff, ReadUInt32(&cmd[80]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"DevPeriod"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[84]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"Status3"));
				Text::StrHexVal32(sbuff, ReadUInt32(&cmd[88]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"LastIP"));
				Net::SocketUtil::GetIPv4Name(sbuff, ReadUInt32(&cmd[92]));
				this->devConts->Add(Text::StrCopyNew(sbuff));
				this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"LastPort"));
				Text::StrInt32(sbuff, ReadUInt16(&cmd[96]));
				this->devConts->Add(Text::StrCopyNew(sbuff));

				i = startSize;
				j = ReadUInt16(&cmd[i]);
				if (i + 2 * j + 4 <= cmdSize)
				{
					k = ReadUInt16(&cmd[i + 2 * j + 2]);
					*(WChar*)&cmd[i + 2 * j + 2] = 0;
					this->devConts->Add(Text::StrCopyNew((const UTF8Char*)"IMEI"));
					this->devConts->Add(Text::StrToUTF8New((WChar*)&cmd[i + 2]));
					WriteInt16(&cmd[i + 2 * j + 2], (Int16)k);
					i += j * 2 + 2;
				}
				else
				{
					i = cmdSize;
				}
				if (i + 2 <= cmdSize)
				{
					j = ReadUInt16(&cmd[i]);
					if (i + j * 24 + 2 <= cmdSize)
					{
						this->devContACnt = j;
						this->devContAlerts = MemAlloc(DevAlert, j);
						i += 2;
						k = 0;
						while (k < j)
						{
							this->devContAlerts[k].alertId = ReadInt32(&cmd[i]);
							this->devContAlerts[k].isAlerting = (cmd[i + 4] != 0);
							this->devContAlerts[k].isAlerted = (cmd[i + 5] != 0);
							this->devContAlerts[k].isFirst = (cmd[i + 6] != 0);
							this->devContAlerts[k].beginTimeTick = ReadInt64(&cmd[i + 8]);
							this->devContAlerts[k].lastTimeTick = ReadInt64(&cmd[i + 16]);
							i += 24;
							k++;
						}
					}
					else
					{
						i = cmdSize;
					}
				}

				if (i + 2 <= cmdSize)
				{
					j = ReadUInt16(&cmd[i]);
					if (i + j * 56 + 2 <= cmdSize)
					{
						this->devContGCnt = j;
						this->devContGuards = MemAlloc(DevGuard, j);
						i += 2;
						k = 0;
						while (k < j)
						{
							this->devContGuards[k].userId = ReadInt32(&cmd[i]);
							this->devContGuards[k].alarmType = ReadInt32(&cmd[i + 4]);
							this->devContGuards[k].alarmLat = ReadDouble(&cmd[i + 8]);
							this->devContGuards[k].alarmLon = ReadDouble(&cmd[i + 16]);
							this->devContGuards[k].alarmStartTicks = ReadInt64(&cmd[i + 24]);
							this->devContGuards[k].found = (cmd[i + 32] != 0);
							this->devContGuards[k].alarmStatus = ReadInt32(&cmd[i + 36]);
							this->devContGuards[k].alarmStatus2 = ReadInt32(&cmd[i + 40]);
							this->devContGuards[k].alarmStatus3 = ReadInt32(&cmd[i + 44]);
							this->devContGuards[k].alarmStatus4 = ReadInt32(&cmd[i + 48]);
							this->devContGuards[k].alarmStatus5 = ReadInt32(&cmd[i + 52]);
							i += 56;
							k++;
						}
					}
					else
					{
						i = cmdSize;
					}
				}

				this->devContUpd = true;
				mutUsage.EndUse();
			}
		}
		break;
	case 9:
		if (cmdSize >= 96)
		{
			UTF8Char sbuff[32];
			UInt32 startSize = ReadUInt32(&cmd[0]);
			UOSInt i;
			UOSInt j;
			UOSInt k;
			if (startSize >= 92 && startSize <= cmdSize)
			{
				Data::DateTime dt;
				this->ClearAlertConts();
				Sync::MutexUsage mutUsage(this->alertContMut);
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"AlertId"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[4]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"AlertType"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[8]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"CompId"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[12]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"WebUserId"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[16]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"AlertTarget"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[20]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"AlertTargetId"));
				Text::StrInt64(sbuff, ReadInt64(&cmd[24]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"ZoneId"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[32]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"Period"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[36]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"AlertTime0"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[40]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"AlertTime1"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[44]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"AlertTime2"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[48]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"AlertTime3"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[52]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"AlertTime4"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[56]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"AlertTime5"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[60]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"AlertTime6"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[64]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"CreateUserId"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[68]));
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"ModifyTime"));
				dt.SetTicks(ReadInt64(&cmd[72]));
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"CreateTime"));
				dt.SetTicks(ReadInt64(&cmd[80]));
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"Found"));
				Text::StrInt32(sbuff, cmd[88]);
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"Enabled"));
				Text::StrInt32(sbuff, cmd[89]);
				this->alertConts->Add(Text::StrCopyNew(sbuff));
				this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"ActionPopup"));
				Text::StrInt32(sbuff, cmd[90]);
				this->alertConts->Add(Text::StrCopyNew(sbuff));

				i = startSize;
				j = ReadUInt16(&cmd[i]);
				if (i + 2 * j + 4 <= cmdSize)
				{
					k = ReadUInt16(&cmd[i + 2 * j + 2]);
					*(WChar*)&cmd[i + 2 * j + 2] = 0;
					this->alertConts->Add(Text::StrCopyNew((const UTF8Char*)"Name"));
					this->alertConts->Add(Text::StrToUTF8New((WChar*)&cmd[i + 2]));
					WriteInt16(&cmd[i + 2 * j + 2], (Int16)k);
					i += j * 2 + 2;
				}
				else
				{
					i = cmdSize;
				}
				if (i + 2 <= cmdSize)
				{
					j = ReadUInt16(&cmd[i]);
					if (i + j * 8 + 2 <= cmdSize)
					{
						i += 2;
						k = 0;
						while (k < j)
						{
							this->alertContDevs->Add(ReadInt64(&cmd[i]));
							i += 8;
							k++;
						}
					}
					else
					{
						i = cmdSize;
					}
				}

				this->alertContUpd = true;
				mutUsage.EndUse();
			}
		}
		break;
	case 11:
		if (cmdSize >= 24)
		{
			UTF8Char sbuff[32];
			UInt32 startSize = ReadUInt32(&cmd[0]);
			UOSInt i;
			UOSInt j;
			UOSInt k;
			if (startSize >= 22 && startSize <= cmdSize)
			{
				Data::DateTime dt;
				this->ClearUserConts();
				Sync::MutexUsage mutUsage(this->userContMut);
				this->userConts->Add(Text::StrCopyNew((const UTF8Char*)"UserId"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[4]));
				this->userConts->Add(Text::StrCopyNew(sbuff));
				this->userConts->Add(Text::StrCopyNew((const UTF8Char*)"UserType"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[8]));
				this->userConts->Add(Text::StrCopyNew(sbuff));
				this->userConts->Add(Text::StrCopyNew((const UTF8Char*)"TimeZone(M)"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[12]));
				this->userConts->Add(Text::StrCopyNew(sbuff));
				this->userConts->Add(Text::StrCopyNew((const UTF8Char*)"LicDisplay"));
				Text::StrInt32(sbuff, ReadInt32(&cmd[16]));
				this->userConts->Add(Text::StrCopyNew(sbuff));
				this->userConts->Add(Text::StrCopyNew((const UTF8Char*)"Updated"));
				Text::StrInt32(sbuff, cmd[20]);
				this->userConts->Add(Text::StrCopyNew(sbuff));

				i = startSize;
				if (i + 2 <= cmdSize)
				{
					j = ReadUInt16(&cmd[i]);
					if (i + j * 8 + 2 <= cmdSize)
					{
						i += 2;
						k = 0;
						while (k < j)
						{
							this->userContDevs->Add(ReadInt64(&cmd[i]));
							i += 8;
							k++;
						}
					}
					else
					{
						i = cmdSize;
					}
				}

				this->userContUpd = true;
				mutUsage.EndUse();
			}
		}
		break;
	}
}

void SSWR::AVIRead::AVIRGPSDevForm::DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}
