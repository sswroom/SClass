#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRTCPLoadBalancerForm.h"

#define TITLE CSTR("TCP Load Balancer")

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::OnClientConnect(NN<Socket> s, AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPLoadBalancerForm>();
	NN<Net::TCPClient> targetCli;
	NN<TCPTarget> target = me->targetList.GetItemNoCheck(me->nextTarget);
	me->nextTarget = (me->nextTarget + 1) % me->targetList.GetCount();
	NEW_CLASSNN(targetCli, Net::TCPClient(me->sockf, target->targetAddr, target->targetPort, 10000));
	if (targetCli->IsConnectError())
	{
		me->sockf->DestroySocket(s);
		targetCli.Delete();
		return;
	}
	NN<TCPSession> sess;
	NEW_CLASSNN(sess, TCPSession());
	sess->lastDataTime = Data::Timestamp::UtcNow();
	NEW_CLASSNN(sess->sourceCli, Net::TCPClient(me->sockf, s));
	sess->targetCli = targetCli;
	sess->me = me;
	sess->displayed = false;
	sess->sessCreatedTime = sess->lastDataTime;
	sess->sourceClosed = false;
	sess->targetClosed = false;
	sess->sourceDataSize = 0;
	sess->targetDataSize = 0;
	me->sessList.Add(sess);
	me->sourceCliMgr->AddClient(sess->sourceCli, sess);
	me->targetCliMgr->AddClient(sess->targetCli, sess);
}

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::OnSourceEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	NN<TCPSession> sess = cliData.GetNN<TCPSession>();
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		Sync::MutexUsage mutUsage(sess->mut);
		sess->sourceClosed = true;
		sess->targetCli->Close();
		if (sess->notUsed && sess->sourceClosed && sess->targetClosed)
		{
			mutUsage.EndUse();
			FreeSession(sess);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::OnSourceData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff)
{
	NN<TCPSession> sess = cliData.GetNN<TCPSession>();
	sess->lastDataTime = Data::Timestamp::Now();
	sess->sourceDataSize += buff.GetSize();
	sess->targetCli->WriteCont(buff.Arr(), buff.GetSize());
}

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::OnSourceTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{
//	NN<TCPSession> sess = cliData.GetNN<TCPSession>();
//	sess->targetCli->Close();
}

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::OnTargetEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	NN<TCPSession> sess = cliData.GetNN<TCPSession>();
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		Sync::MutexUsage mutUsage(sess->mut);
		sess->targetClosed = true;
		sess->sourceCli->Close();
		if (sess->notUsed && sess->sourceClosed && sess->targetClosed)
		{
			mutUsage.EndUse();
			FreeSession(sess);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::OnTargetData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff)
{
	NN<TCPSession> sess = cliData.GetNN<TCPSession>();
	sess->lastDataTime = Data::Timestamp::Now();
	sess->targetDataSize += buff.GetSize();
	sess->sourceCli->WriteCont(buff.Arr(), buff.GetSize());
}

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::OnTargetTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{
//	NN<TCPSession> sess = cliData.GetNN<TCPSession>();
//	sess->sourceCli->Close();
}

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPLoadBalancerForm>();
	NN<Net::TCPServer> svr;
	if (me->sourceSvr.SetTo(svr))
	{
		me->sourceSvr.Delete();
		me->sourceCliMgr->CloseAll();
		me->targetCliMgr->CloseAll();
		{
			Sync::MutexUsage mutUsage(me->sessMut);
			me->sessList.FreeAll(EndSession);
			me->lvSession->ClearItems();
		}
		me->txtListenerPort->SetReadOnly(false);
		me->txtTimeout->SetReadOnly(false);
		me->btnStart->SetText(CSTR("Start"));
		return;
	}
	if (me->targetList.GetCount() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please add target first"), TITLE, me);
		return;
	}
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	if (!me->txtListenerPort->GetText(sb) || !sb.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Port invalid"), TITLE, me);
		return;
	}
	sb.ClearStr();
	if (!me->txtTimeout->GetText(sb) || (me->sessTimeout = Data::Duration::FromStr(sb.ToCString())).IsZero())
	{
		me->ui->ShowMsgOK(CSTR("Timeout invalid"), TITLE, me);
		return;
	}
	NEW_CLASSNN(svr, Net::TCPServer(me->sockf, nullptr, port, me->log, OnClientConnect, me, CSTR("SVR: "), true));
	if (svr->IsV4Error())
	{
		svr.Delete();
		me->ui->ShowMsgOK(CSTR("Error in listening to TCP port"), TITLE, me);
		return;
	}
	me->sourceSvr = svr;
	me->txtListenerPort->SetReadOnly(true);
	me->txtTimeout->SetReadOnly(true);
	me->btnStart->SetText(CSTR("Stop"));
}

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::OnTargetAddClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPLoadBalancerForm>();
	UInt16 port;
	Net::SocketUtil::AddressInfo addr;
	addr.addrType = Net::AddrType::Unknown;
	Text::StringBuilderUTF8 sb;
	me->txtTargetIP->GetText(sb);
	if (!Net::SocketUtil::SetAddrInfo(addr, sb.ToCString()))
	{
		me->ui->ShowMsgOK(CSTR("IP is not valid"), TITLE, me);
		return;
	}
	sb.ClearStr();
	me->txtTargetPort->GetText(sb);
	if (!sb.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Port is not valid"), TITLE, me);
		return;
	}
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<TCPTarget> target;
	NEW_CLASSNN(target, TCPTarget());
	target->targetAddr = addr;
	target->targetPort = port;
	Sync::MutexUsage mutUsage(me->sessMut);
	me->targetList.Add(target);
	sbuff[0] = 0;
	sptr = Net::SocketUtil::GetAddrName(sbuff, target->targetAddr).Or(sbuff);
	UOSInt i = me->lvTarget->AddItem(CSTRP(sbuff, sptr), target);
	sptr = Text::StrUInt16(sbuff, target->targetPort);
	me->lvTarget->SetSubItem(i, 1, CSTRP(sbuff, sptr));
	if (me->nextTarget == i)
	{
		me->lvTarget->SetSubItem(i, 2, CSTR("*"));
		me->dispNextTarget = i;
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::OnTargetDelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPLoadBalancerForm>();
	UOSInt i = me->lvTarget->GetSelectedIndex();
	if (i == INVALID_INDEX)
		return;
	Sync::MutexUsage mutUsage(me->sessMut);
	if (me->sourceSvr.NotNull() && me->targetList.GetCount() == 1)
	{
		me->ui->ShowMsgOK(CSTR("Cannot delete all target when it is started"), TITLE, me);
		return;
	}
	NN<TCPTarget> target;
	me->lvTarget->RemoveItem(i);
	if (me->targetList.RemoveAt(i).SetTo(target))
	{
		target.Delete();
	}
	if (me->nextTarget == i)
	{
		me->nextTarget = 0;
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPLoadBalancerForm>();
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UOSInt nextTarget = me->nextTarget;
	if (nextTarget != me->dispNextTarget)
	{
		if (me->dispNextTarget != INVALID_INDEX)
		{
			me->lvTarget->SetSubItem(me->dispNextTarget, 2, CSTR(""));
		}
		me->dispNextTarget = nextTarget;
		me->lvTarget->SetSubItem(nextTarget, 2, CSTR("*"));
	}
	if (me->sourceSvr.NotNull())
	{
		Sync::MutexUsage mutUsage(me->sessMut);
		NN<TCPSession> sess;
		Data::Timestamp currTime = Data::Timestamp::UtcNow();
		UOSInt i = 0;
		UOSInt j = me->sessList.GetCount();
		UOSInt k;
		while (i < j)
		{
			sess = me->sessList.GetItemNoCheck(i);
			if ((currTime - sess->lastDataTime) >= me->sessTimeout || (sess->sourceClosed && sess->targetClosed))
			{
				me->sessList.RemoveAt(i);
				if (sess->displayed)
				{
					me->lvSession->RemoveItem(i);
				}
				EndSession(sess);
			}
			else if (!sess->displayed)
			{
				sess->displayed = true;
				sbuff[0] = 0;
				sptr = sess->sourceCli->GetRemoteName(sbuff).Or(sbuff);
				k = me->lvSession->AddItem(CSTRP(sbuff, sptr), sess);
				sbuff[0] = 0;
				sptr = sess->targetCli->GetRemoteName(sbuff).Or(sbuff);
				me->lvSession->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, sess->sourceDataSize);
				me->lvSession->SetSubItem(k, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, sess->targetDataSize);
				me->lvSession->SetSubItem(k, 3, CSTRP(sbuff, sptr));
				sptr = sess->sessCreatedTime.ToStringNoZone(sbuff);
				me->lvSession->SetSubItem(k, 4, CSTRP(sbuff, sptr));
			}
			else
			{
				sptr = Text::StrUInt64(sbuff, sess->sourceDataSize);
				me->lvSession->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, sess->targetDataSize);
				me->lvSession->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::EndSession(NN<TCPSession> sess)
{
	Sync::MutexUsage mutUsage(sess->mut);
	sess->notUsed = true;
	sess->sourceCli->Close();
	sess->targetCli->Close();
	if (sess->notUsed && sess->sourceClosed && sess->targetClosed)
	{
		mutUsage.EndUse();
		FreeSession(sess);
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPLoadBalancerForm::FreeSession(NN<TCPSession> sess)
{
	sess->targetCli.Delete();
	sess->sourceCli.Delete();
	sess.Delete();
}

SSWR::AVIRead::AVIRTCPLoadBalancerForm::AVIRTCPLoadBalancerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(TITLE);

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->sourceSvr = nullptr;
	this->nextTarget = 0;
	this->dispNextTarget = INVALID_INDEX;
	this->sessTimeout = 30000;
	NEW_CLASSNN(this->sourceCliMgr, Net::TCPClientMgr(60000, OnSourceEvent, OnSourceData, this, 16, OnSourceTimeout));
	NEW_CLASSNN(this->targetCliMgr, Net::TCPClientMgr(60000, OnTargetEvent, OnTargetData, this, 16, OnTargetTimeout));

	this->pnlListener = ui->NewPanel(*this);
	this->pnlListener->SetRect(0, 0, 100, 79, false);
	this->pnlListener->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblListenerPort = ui->NewLabel(this->pnlListener, CSTR("Port"));
	this->lblListenerPort->SetRect(4, 4, 100, 23, false);
	this->txtListenerPort = ui->NewTextBox(this->pnlListener, CSTR(""));
	this->txtListenerPort->SetRect(104, 4, 50, 23, false);
	this->lblTimeout = ui->NewLabel(this->pnlListener, CSTR("Timeout"));
	this->lblTimeout->SetRect(4, 28, 100, 23, false);
	this->txtTimeout = ui->NewTextBox(this->pnlListener, CSTR("30000ms"));
	this->txtTimeout->SetRect(104, 28, 100, 23, false);
	this->btnStart = ui->NewButton(this->pnlListener, CSTR("Start"));
	this->btnStart->SetRect(104, 52, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpTarget = this->tcMain->AddTabPage(CSTR("Target"));
	this->pnlTarget = ui->NewPanel(this->tpTarget);
	this->pnlTarget->SetRect(0, 0, 100, 31, false);
	this->pnlTarget->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvTarget = ui->NewListView(this->tpTarget, UI::ListViewStyle::Table, 4);
	this->lvTarget->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTarget->SetFullRowSelect(true);
	this->lvTarget->SetShowGrid(true);
	this->lvTarget->AddColumn(CSTR("IP"), 150);
	this->lvTarget->AddColumn(CSTR("Port"), 50);
	this->lvTarget->AddColumn(CSTR("Next?"), 50);
	this->lvTarget->AddColumn(CSTR("Session Cnt"), 50);
	this->lblTargetIP = ui->NewLabel(this->pnlTarget, CSTR("IP"));
	this->lblTargetIP->SetRect(4, 4, 50, 23, false);
	this->txtTargetIP = ui->NewTextBox(this->pnlTarget, CSTR(""));
	this->txtTargetIP->SetRect(54, 4, 150, 23, false);
	this->lblTargetPort = ui->NewLabel(this->pnlTarget, CSTR("Port"));
	this->lblTargetPort->SetRect(204, 4, 50, 23, false);
	this->txtTargetPort = ui->NewTextBox(this->pnlTarget, CSTR(""));
	this->txtTargetPort->SetRect(254, 4, 50, 23, false);
	this->btnTargetAdd = ui->NewButton(this->pnlTarget, CSTR("Add"));
	this->btnTargetAdd->SetRect(304, 4, 75, 23, false);
	this->btnTargetAdd->HandleButtonClick(OnTargetAddClicked, this);
	this->btnTargetDel = ui->NewButton(this->pnlTarget, CSTR("Delete"));
	this->btnTargetDel->SetRect(384, 4, 75, 23, false);
	this->btnTargetDel->HandleButtonClick(OnTargetDelClicked, this);

	this->tpSession = this->tcMain->AddTabPage(CSTR("Session"));
	this->lvSession = ui->NewListView(this->tpSession, UI::ListViewStyle::Table, 5);
	this->lvSession->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSession->SetFullRowSelect(true);
	this->lvSession->SetShowGrid(true);
	this->lvSession->AddColumn(CSTR("Source"), 150);
	this->lvSession->AddColumn(CSTR("Target"), 150);
	this->lvSession->AddColumn(CSTR("Source Data"), 100);
	this->lvSession->AddColumn(CSTR("Target Data"), 100);
	this->lvSession->AddColumn(CSTR("Session Start"), 150);

	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRTCPLoadBalancerForm::~AVIRTCPLoadBalancerForm()
{
	this->sourceSvr.Delete();
	this->sourceCliMgr->CloseAll();
	this->targetCliMgr->CloseAll();
	this->sourceCliMgr.Delete();
	this->targetCliMgr.Delete();
	{
		Sync::MutexUsage mutUsage(this->sessMut);
		this->sessList.FreeAll(FreeSession);
	}
	this->targetList.DeleteAll();
}

void SSWR::AVIRead::AVIRTCPLoadBalancerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
