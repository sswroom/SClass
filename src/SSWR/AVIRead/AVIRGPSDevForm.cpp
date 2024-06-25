#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "Net/TCPClient.h"
#include "SSWR/AVIRead/AVIRGPSDevForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringW.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall SSWR::AVIRead::AVIRGPSDevForm::ClientThread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSDevForm>();
	UOSInt recvBuffSize;
	UOSInt readSize;
	NN<Net::TCPClient> cli;
	{
		Data::ByteBuffer recvBuff(16384);
		recvBuffSize = 0;
		me->threadRunning = true;
		while (!me->threadToStop)
		{
			if (cli.Set(me->cli))
			{
				readSize = cli->Read(recvBuff.SubArray(recvBuffSize));

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
					readSize = me->protoHdlr.ParseProtocol(cli, 0, me, recvBuff.WithSize(recvBuffSize));
					if (readSize == 0)
					{
						recvBuffSize = 0;
					}
					else if (readSize >= recvBuffSize)
					{
					}
					else
					{
						recvBuff.CopyInner(0, recvBuffSize - readSize, readSize);
						recvBuffSize = readSize;
					}
				}
			}
			else
			{
				me->threadEvt->Wait(1000);
			}
		}
	}
	me->threadRunning = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnConnClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSDevForm>();
	if (me->cli)
	{
		me->ToStop();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port = 0;
		me->txtPort->GetText(sb);
		sb.ToUInt16(port);
		sb.ClearStr();
		me->txtHost->GetText(sb);
		if (!me->core->GetSocketFactory()->DNSResolveIP(sb.ToCString(), addr) || port <= 0)
		{

		}
		else
		{
			Net::TCPClient *cli;
			NEW_CLASS(cli, Net::TCPClient(me->core->GetSocketFactory(), addr, port, 30000));
			if (cli->IsClosed())
			{
				DEL_CLASS(cli);
			}
			else
			{
				me->dispConn = true;
				me->txtStatusConn->SetText(CSTR("Connected"));
				me->txtHost->SetReadOnly(true);
				me->txtPort->SetReadOnly(true);
				me->cli = cli;
				me->threadEvt->Set();
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnDeviceRClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSDevForm>();
	me->SendGetDevices();
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnUserRClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSDevForm>();
	me->SendGetUsers();
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnAlertRClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSDevForm>();
	me->SendGetAlerts();
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnDeviceSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSDevForm>();
	NN<Text::String> s;
	if (me->lbDevice->GetSelectedItemTextNew().SetTo(s))
	{
		me->SendGetDevice(s->ToInt64());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnAlertSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSDevForm>();
	NN<Text::String> s;
	if (me->lbAlert->GetSelectedItemTextNew().SetTo(s))
	{
		me->SendGetAlert(s->ToInt32());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnUserSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSDevForm>();
	NN<Text::String> s;
	if (me->lbUser->GetSelectedItemTextNew().SetTo(s))
	{
		me->SendGetUser(s->ToInt32());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSDevForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSDevForm>();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	if (me->cli == 0 && me->dispConn)
	{
		me->dispConn = false;
		me->txtHost->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		me->txtStatusConn->SetText(CSTR("Not Connected"));
	}
	if (me->alertUpd)
	{
		Sync::MutexUsage mutUsage(me->alertMut);
		me->alertUpd = false;
		me->lbAlert->ClearItems();
		i = 0;
		j = me->alertList.GetCount();
		while (i < j)
		{
			sptr = Text::StrInt32(sbuff, me->alertList.GetItem(i));
			me->lbAlert->AddItem(CSTRP(sbuff, sptr), 0);
			i++;
		}
	}
	if (me->deviceUpd)
	{
		Sync::MutexUsage mutUsage(me->deviceMut);
		me->deviceUpd = false;
		me->lbDevice->ClearItems();
		i = 0;
		j = me->deviceList.GetCount();
		while (i < j)
		{
			sptr = Text::StrInt64(sbuff, me->deviceList.GetItem(i));
			me->lbDevice->AddItem(CSTRP(sbuff, sptr), 0);
			i++;
		}
	}
	if (me->userUpd)
	{
		Sync::MutexUsage mutUsage(me->userMut);
		me->userUpd = false;
		me->lbUser->ClearItems();
		i = 0;
		j = me->userList.GetCount();
		while (i < j)
		{
			sptr = Text::StrInt32(sbuff, me->userList.GetItem(i));
			me->lbUser->AddItem(CSTRP(sbuff, sptr), 0);
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
		j = me->devConts.GetCount();
		while (i < j)
		{
			k = me->lvDevInfo->AddItem(Text::String::OrEmpty(me->devConts.GetItem(i)), 0);
			me->lvDevInfo->SetSubItem(k, 1, Text::String::OrEmpty(me->devConts.GetItem(i + 1)));
			i += 2;
		}
		me->lvDevAlert->ClearItems();
		dt.ToUTCTime();
		i = 0;
		j = me->devContACnt;
		while (i < j)
		{
			sptr = Text::StrInt32(sbuff, me->devContAlerts[i].alertId);
			k = me->lvDevAlert->AddItem(CSTRP(sbuff, sptr), 0);
			me->lvDevAlert->SetSubItem(k, 1, me->devContAlerts[i].isAlerting?CSTR("1"):CSTR("0"));
			me->lvDevAlert->SetSubItem(k, 2, me->devContAlerts[i].isAlerted?CSTR("1"):CSTR("0"));
			me->lvDevAlert->SetSubItem(k, 3, me->devContAlerts[i].isFirst?CSTR("1"):CSTR("0"));
			dt.SetTicks(me->devContAlerts[i].beginTimeTick);
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->lvDevAlert->SetSubItem(k, 4, CSTRP(sbuff, sptr));
			dt.SetTicks(me->devContAlerts[i].lastTimeTick);
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->lvDevAlert->SetSubItem(k, 5, CSTRP(sbuff, sptr));
			i++;
		}
		me->lvDevGuard->ClearItems();
		i = 0;
		j = me->devContGCnt;
		while (i < j)
		{
			sptr = Text::StrInt32(sbuff, me->devContGuards[i].userId);
			k = me->lvDevGuard->AddItem(CSTRP(sbuff, sptr), 0);
			me->lvDevGuard->SetSubItem(k, 1, me->devContGuards[i].found?CSTR("1"):CSTR("0"));
			sptr = Text::StrDouble(sbuff, me->devContGuards[i].alarmLat);
			me->lvDevGuard->SetSubItem(k, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, me->devContGuards[i].alarmLon);
			me->lvDevGuard->SetSubItem(k, 3, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, me->devContGuards[i].alarmType);
			me->lvDevGuard->SetSubItem(k, 4, CSTRP(sbuff, sptr));
			dt.SetTicks(me->devContGuards[i].alarmStartTicks);
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->lvDevGuard->SetSubItem(k, 5, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, me->devContGuards[i].alarmStatus);
			me->lvDevGuard->SetSubItem(k, 6, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, me->devContGuards[i].alarmStatus2);
			me->lvDevGuard->SetSubItem(k, 7, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, me->devContGuards[i].alarmStatus3);
			me->lvDevGuard->SetSubItem(k, 8, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, me->devContGuards[i].alarmStatus4);
			me->lvDevGuard->SetSubItem(k, 9, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, me->devContGuards[i].alarmStatus5);
			me->lvDevGuard->SetSubItem(k, 10, CSTRP(sbuff, sptr));
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
		j = me->alertConts.GetCount();
		while (i < j)
		{
			k = me->lvAlert->AddItem(Text::String::OrEmpty(me->alertConts.GetItem(i)), 0);
			me->lvAlert->SetSubItem(k, 1, Text::String::OrEmpty(me->alertConts.GetItem(i + 1)));
			i += 2;
		}
		me->lbAlertDev->ClearItems();
		i = 0;
		j = me->alertContDevs.GetCount();
		while (i < j)
		{
			sptr = Text::StrInt64(sbuff, me->alertContDevs.GetItem(i));
			me->lbAlertDev->AddItem(CSTRP(sbuff, sptr), 0);;
			i++;
		}
	}
	if (me->userContUpd)
	{
		me->userContUpd = false;
		Sync::MutexUsage mutUsage(me->userContMut);
		me->lvUser->ClearItems();
		i = 0;
		j = me->userConts.GetCount();
		while (i < j)
		{
			k = me->lvUser->AddItem(Text::String::OrEmpty(me->userConts.GetItem(i)), 0);
			me->lvUser->SetSubItem(k, 1, Text::String::OrEmpty(me->userConts.GetItem(i + 1)));
			i += 2;
		}
		me->lbUserDev->ClearItems();
		i = 0;
		j = me->userContDevs.GetCount();
		while (i < j)
		{
			sptr = Text::StrInt64(sbuff, me->userContDevs.GetItem(i));
			me->lbUserDev->AddItem(CSTRP(sbuff, sptr), 0);;
			i++;
		}
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
			Sync::SimpleThread::Sleep(10);
		}
	}
	else
	{
		mutUsage.EndUse();
	}
}

void SSWR::AVIRead::AVIRGPSDevForm::ClearDevConts()
{
	Sync::MutexUsage mutUsage(this->devContMut);
	this->devConts.FreeAll();
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
}

void SSWR::AVIRead::AVIRGPSDevForm::ClearAlertConts()
{
	Sync::MutexUsage mutUsage(this->alertContMut);
	this->alertConts.FreeAll();
	this->alertContDevs.Clear();
}

void SSWR::AVIRead::AVIRGPSDevForm::ClearUserConts()
{
	Sync::MutexUsage mutUsage(this->userContMut);
	this->userConts.FreeAll();
	this->userContDevs.Clear();
}

void SSWR::AVIRead::AVIRGPSDevForm::SendGetAlerts()
{
	UInt8 buff[12];
	UOSInt cmdSize = this->protoHdlr.BuildPacket(buff, 0, 0, buff, 0, 0);
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Write(Data::ByteArrayR(buff, cmdSize));
	}
}

void SSWR::AVIRead::AVIRGPSDevForm::SendGetDevices()
{
	UInt8 buff[12];
	UOSInt cmdSize = this->protoHdlr.BuildPacket(buff, 2, 0, buff, 0, 0);
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Write(Data::ByteArrayR(buff, cmdSize));
	}
}

void SSWR::AVIRead::AVIRGPSDevForm::SendGetUsers()
{
	UInt8 buff[12];
	UOSInt cmdSize = this->protoHdlr.BuildPacket(buff, 4, 0, buff, 0, 0);
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Write(Data::ByteArrayR(buff, cmdSize));
	}
}

void SSWR::AVIRead::AVIRGPSDevForm::SendGetDevice(Int64 devId)
{
	UInt8 buff[20];
	UOSInt cmdSize = this->protoHdlr.BuildPacket(buff, 6, 0, (UInt8*)&devId, 8, 0);
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Write(Data::ByteArrayR(buff, cmdSize));
	}
}

void SSWR::AVIRead::AVIRGPSDevForm::SendGetAlert(Int32 alertId)
{
	UInt8 buff[20];
	UOSInt cmdSize = this->protoHdlr.BuildPacket(buff, 8, 0, (UInt8*)&alertId, 4, 0);
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Write(Data::ByteArrayR(buff, cmdSize));
	}
}

void SSWR::AVIRead::AVIRGPSDevForm::SendGetUser(Int32 userId)
{
	UInt8 buff[20];
	UOSInt cmdSize = this->protoHdlr.BuildPacket(buff, 10, 0, (UInt8*)&userId, 4, 0);
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Write(Data::ByteArrayR(buff, cmdSize));
	}
}

SSWR::AVIRead::AVIRGPSDevForm::AVIRGPSDevForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui), protoHdlr(*this)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("GPSDev Viewer"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->cli = 0;

	this->alertUpd = false;
	this->deviceUpd = false;
	this->userUpd = false;
	this->devContUpd = false;
	this->devContACnt = 0;
	this->devContAlerts = 0;
	this->devContGCnt = 0;
	this->devContGuards = 0;
	this->alertContUpd = false;
	this->userContUpd = false;

	this->pnlConn = ui->NewPanel(*this);
	this->pnlConn->SetRect(0, 0, 100, 56, false);
	this->pnlConn->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblHost = ui->NewLabel(this->pnlConn, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(this->pnlConn, CSTR("127.0.0.1"));
	this->txtHost->SetRect(104, 4, 150, 23, false);
	this->lblPort = ui->NewLabel(this->pnlConn, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlConn, CSTR("7500"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	this->btnConn = ui->NewButton(this->pnlConn, CSTR("Start"));
	this->btnConn->SetRect(204, 28, 75, 23, false);
	this->btnConn->HandleButtonClick(OnConnClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->lblStatusConn = ui->NewLabel(this->tpStatus, CSTR("Conn Status"));
	this->lblStatusConn->SetRect(4, 4, 100, 23, false);
	this->txtStatusConn = ui->NewTextBox(this->tpStatus, CSTR("Not Connected"));
	this->txtStatusConn->SetRect(104, 4, 100, 23, false);
	this->txtStatusConn->SetReadOnly(true);

	this->tpDevice = this->tcMain->AddTabPage(CSTR("Device"));
	this->pnlDeviceC = ui->NewPanel(this->tpDevice);
	this->pnlDeviceC->SetRect(0, 0, 200, 23, false);
	this->pnlDeviceC->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnDeviceR = ui->NewButton(this->pnlDeviceC, CSTR("Refresh"));
	this->btnDeviceR->SetRect(0, 0, 100, 23, false);
	this->btnDeviceR->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDeviceR->HandleButtonClick(OnDeviceRClicked, this);
	this->lbDevice = ui->NewListBox(this->pnlDeviceC, false);
	this->lbDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbDevice->HandleSelectionChange(OnDeviceSelChg, this);
	this->hspDevice = ui->NewHSplitter(this->tpDevice, 3, false);
	this->tcDevice = ui->NewTabControl(this->tpDevice);
	this->tcDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpDevInfo = this->tcDevice->AddTabPage(CSTR("Info"));
	this->lvDevInfo = ui->NewListView(this->tpDevInfo, UI::ListViewStyle::Table, 2);
	this->lvDevInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevInfo->SetFullRowSelect(true);
	this->lvDevInfo->SetShowGrid(true);
	this->lvDevInfo->AddColumn(CSTR("Name"), 200);
	this->lvDevInfo->AddColumn(CSTR("Value"), 400);
	this->tpDevAlert = this->tcDevice->AddTabPage(CSTR("Alert"));
	this->lvDevAlert = ui->NewListView(this->tpDevAlert, UI::ListViewStyle::Table, 6);
	this->lvDevAlert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevAlert->SetFullRowSelect(true);
	this->lvDevAlert->SetShowGrid(true);
	this->lvDevAlert->AddColumn(CSTR("AlertId"), 60);
	this->lvDevAlert->AddColumn(CSTR("isAlerting"), 60);
	this->lvDevAlert->AddColumn(CSTR("isAlerted"), 60);
	this->lvDevAlert->AddColumn(CSTR("isFirst"), 60);
	this->lvDevAlert->AddColumn(CSTR("Begin Time (UTC)"), 200);
	this->lvDevAlert->AddColumn(CSTR("Last Time (UTC)"), 200);
	this->tpDevGuard = this->tcDevice->AddTabPage(CSTR("Guard"));
	this->lvDevGuard = ui->NewListView(this->tpDevGuard, UI::ListViewStyle::Table, 11);
	this->lvDevGuard->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevGuard->SetFullRowSelect(true);
	this->lvDevGuard->SetShowGrid(true);
	this->lvDevGuard->AddColumn(CSTR("User Id"), 60);
	this->lvDevGuard->AddColumn(CSTR("Found"), 60);
	this->lvDevGuard->AddColumn(CSTR("AlarmLat"), 100);
	this->lvDevGuard->AddColumn(CSTR("AlarmLon"), 100);
	this->lvDevGuard->AddColumn(CSTR("AlarmType"), 60);
	this->lvDevGuard->AddColumn(CSTR("AlarmStart"),200);
	this->lvDevGuard->AddColumn(CSTR("AlarmStatus"), 60);
	this->lvDevGuard->AddColumn(CSTR("AlarmStatus2"), 60);
	this->lvDevGuard->AddColumn(CSTR("AlarmStatus3"), 60);
	this->lvDevGuard->AddColumn(CSTR("AlarmStatus4"), 60);
	this->lvDevGuard->AddColumn(CSTR("AlarmStatus5"), 60);

	this->tpUser = this->tcMain->AddTabPage(CSTR("User"));
	this->pnlUserC = ui->NewPanel(this->tpUser);
	this->pnlUserC->SetRect(0, 0, 200, 23, false);
	this->pnlUserC->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnUserR = ui->NewButton(this->pnlUserC, CSTR("Refresh"));
	this->btnUserR->SetRect(0, 0, 100, 23, false);
	this->btnUserR->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnUserR->HandleButtonClick(OnUserRClicked, this);
	this->lbUser = ui->NewListBox(this->pnlUserC, false);
	this->lbUser->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbUser->HandleSelectionChange(OnUserSelChg, this);
	this->hspUser = ui->NewHSplitter(this->tpUser, 3, false);
	this->pnlUser = ui->NewPanel(this->tpUser);
	this->pnlUser->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbUserDev = ui->NewListBox(this->pnlUser, false);
	this->lbUserDev->SetRect(0, 0, 150, 23, false);
	this->lbUserDev->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->lvUser = ui->NewListView(this->pnlUser, UI::ListViewStyle::Table, 2);
	this->lvUser->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvUser->SetShowGrid(true);
	this->lvUser->SetFullRowSelect(true);
	this->lvUser->AddColumn(CSTR("Name"), 200);
	this->lvUser->AddColumn(CSTR("Value"), 400);

	this->tpAlert = this->tcMain->AddTabPage(CSTR("Alert"));
	this->pnlAlertC = ui->NewPanel(this->tpAlert);
	this->pnlAlertC->SetRect(0, 0, 200, 23, false);
	this->pnlAlertC->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnAlertR = ui->NewButton(this->pnlAlertC, CSTR("Refresh"));
	this->btnAlertR->SetRect(0, 0, 100, 23, false);
	this->btnAlertR->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnAlertR->HandleButtonClick(OnAlertRClicked, this);
	this->lbAlert = ui->NewListBox(this->pnlAlertC, false);
	this->lbAlert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbAlert->HandleSelectionChange(OnAlertSelChg, this);
	this->hspAlert = ui->NewHSplitter(this->tpAlert, 3, false);
	this->pnlAlert = ui->NewPanel(this->tpAlert);
	this->pnlAlert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbAlertDev = ui->NewListBox(this->pnlAlert, false);
	this->lbAlertDev->SetRect(0, 0, 150, 23, false);
	this->lbAlertDev->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->lvAlert = ui->NewListView(this->pnlAlert, UI::ListViewStyle::Table, 2);
	this->lvAlert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvAlert->SetShowGrid(true);
	this->lvAlert->SetFullRowSelect(true);
	this->lvAlert->AddColumn(CSTR("Name"), 200);
	this->lvAlert->AddColumn(CSTR("Value"), 400);

	this->dispConn = false;
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->threadEvt, Sync::Event(true));
	Sync::ThreadUtil::Create(ClientThread, this);
	while (!this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
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
		Sync::SimpleThread::Sleep(10);
	}
	DEL_CLASS(this->threadEvt);

	this->ClearDevConts();
	this->ClearAlertConts();
	this->ClearUserConts();
}

void SSWR::AVIRead::AVIRGPSDevForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGPSDevForm::DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize)
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
				this->alertList.Clear();
				i = 4;
				while (cnt-- > 0)
				{
					this->alertList.Add(ReadInt32(&cmd[i]));
					i += 4;
				}
				this->alertUpd = true;
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
				this->deviceList.Clear();
				i = 4;
				while (cnt-- > 0)
				{
					this->deviceList.Add(ReadInt64(&cmd[i]));
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
				this->userList.Clear();
				i = 4;
				while (cnt-- > 0)
				{
					this->userList.Add(ReadInt32(&cmd[i]));
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
			UnsafeArray<UTF8Char> sptr;
			UInt32 startSize = ReadUInt32(&cmd[0]);
			UOSInt i;
			UOSInt j;
			UOSInt k;
			if (startSize >= 98 && startSize <= cmdSize)
			{
				Data::DateTime dt;
				this->ClearDevConts();
				Sync::MutexUsage mutUsage(this->devContMut);
				this->devConts.Add(Text::String::New(UTF8STRC("CompId")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[4]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("DevId")));
				sptr = Text::StrInt64(sbuff, ReadInt64(&cmd[8]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("IP Time")));
				dt.SetTicks(ReadInt64(&cmd[16]));
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("Last Signal Time")));
				dt.SetTicks(ReadInt64(&cmd[24]));
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("Last Loc Time")));
				dt.SetTicks(ReadInt64(&cmd[32]));
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("LastLat")));
				sptr = Text::StrDouble(sbuff, ReadDouble(&cmd[40]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("LastLon")));
				sptr = Text::StrDouble(sbuff, ReadDouble(&cmd[48]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("GuardDist")));
				sptr = Text::StrDouble(sbuff, ReadDouble(&cmd[56]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("GuardFlags")));
				sptr = Text::StrHexVal32(sbuff, ReadUInt32(&cmd[64]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("GuardFlags2")));
				sptr = Text::StrHexVal32(sbuff, ReadUInt32(&cmd[68]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("GuardFlags3")));
				sptr = Text::StrHexVal32(sbuff, ReadUInt32(&cmd[72]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("GuardFlags4")));
				sptr = Text::StrHexVal32(sbuff, ReadUInt32(&cmd[76]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("GuardFlags5")));
				sptr = Text::StrHexVal32(sbuff, ReadUInt32(&cmd[80]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("DevPeriod")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[84]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("Status3")));
				sptr = Text::StrHexVal32(sbuff, ReadUInt32(&cmd[88]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("LastIP")));
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadUInt32(&cmd[92]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->devConts.Add(Text::String::New(UTF8STRC("LastPort")));
				sptr = Text::StrInt32(sbuff, ReadUInt16(&cmd[96]));
				this->devConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));

				i = startSize;
				j = ReadUInt16(&cmd[i]);
				if (i + 2 * j + 4 <= cmdSize)
				{
					k = ReadUInt16(&cmd[i + 2 * j + 2]);
					*(WChar*)&cmd[i + 2 * j + 2] = 0;
					this->devConts.Add(Text::String::New(UTF8STRC("IMEI")));
					this->devConts.Add(Text::String::NewNotNull((WChar*)&cmd[i + 2]));
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
			UnsafeArray<UTF8Char> sptr;
			UInt32 startSize = ReadUInt32(&cmd[0]);
			UOSInt i;
			UOSInt j;
			UOSInt k;
			if (startSize >= 92 && startSize <= cmdSize)
			{
				Data::DateTime dt;
				this->ClearAlertConts();
				Sync::MutexUsage mutUsage(this->alertContMut);
				this->alertConts.Add(Text::String::New(UTF8STRC("AlertId")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[4]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("AlertType")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[8]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("CompId")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[12]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("WebUserId")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[16]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("AlertTarget")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[20]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("AlertTargetId")));
				sptr = Text::StrInt64(sbuff, ReadInt64(&cmd[24]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("ZoneId")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[32]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("Period")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[36]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("AlertTime0")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[40]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("AlertTime1")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[44]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("AlertTime2")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[48]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("AlertTime3")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[52]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("AlertTime4")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[56]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("AlertTime5")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[60]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("AlertTime6")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[64]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("CreateUserId")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[68]));
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("ModifyTime")));
				dt.SetTicks(ReadInt64(&cmd[72]));
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("CreateTime")));
				dt.SetTicks(ReadInt64(&cmd[80]));
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("Found")));
				sptr = Text::StrInt32(sbuff, cmd[88]);
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("Enabled")));
				sptr = Text::StrInt32(sbuff, cmd[89]);
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->alertConts.Add(Text::String::New(UTF8STRC("ActionPopup")));
				sptr = Text::StrInt32(sbuff, cmd[90]);
				this->alertConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));

				i = startSize;
				j = ReadUInt16(&cmd[i]);
				if (i + 2 * j + 4 <= cmdSize)
				{
					k = ReadUInt16(&cmd[i + 2 * j + 2]);
					*(WChar*)&cmd[i + 2 * j + 2] = 0;
					this->alertConts.Add(Text::String::New(UTF8STRC("Name")));
					this->alertConts.Add(Text::String::NewNotNull((WChar*)&cmd[i + 2]));
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
							this->alertContDevs.Add(ReadInt64(&cmd[i]));
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
			UnsafeArray<UTF8Char> sptr;
			UInt32 startSize = ReadUInt32(&cmd[0]);
			UOSInt i;
			UOSInt j;
			UOSInt k;
			if (startSize >= 22 && startSize <= cmdSize)
			{
				Data::DateTime dt;
				this->ClearUserConts();
				Sync::MutexUsage mutUsage(this->userContMut);
				this->userConts.Add(Text::String::New(UTF8STRC("UserId")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[4]));
				this->userConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->userConts.Add(Text::String::New(UTF8STRC("UserType")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[8]));
				this->userConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->userConts.Add(Text::String::New(UTF8STRC("TimeZone(M)")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[12]));
				this->userConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->userConts.Add(Text::String::New(UTF8STRC("LicDisplay")));
				sptr = Text::StrInt32(sbuff, ReadInt32(&cmd[16]));
				this->userConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				this->userConts.Add(Text::String::New(UTF8STRC("Updated")));
				sptr = Text::StrInt32(sbuff, cmd[20]);
				this->userConts.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));

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
							this->userContDevs.Add(ReadInt64(&cmd[i]));
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

void SSWR::AVIRead::AVIRGPSDevForm::DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
}
