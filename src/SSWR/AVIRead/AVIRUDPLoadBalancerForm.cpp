#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRUDPLoadBalancerForm.h"

#define TITLE CSTR("UDP Load Balancer")

void __stdcall SSWR::AVIRead::AVIRUDPLoadBalancerForm::OnSourceUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<SSWR::AVIRead::AVIRUDPLoadBalancerForm> me = userData.GetNN<SSWR::AVIRead::AVIRUDPLoadBalancerForm>();
	if (me->stopping)
		return;
	NN<UDPSession> sess;
	Optional<UDPSession> selSess = 0;
	Sync::MutexUsage mutUsage(me->sessMut);
	UOSInt i = me->sessList.GetCount();
	while (i-- > 0)
	{
		sess = me->sessList.GetItemNoCheck(i);
		if (sess->sourcePort == port && Net::SocketUtil::AddrEquals(sess->sourceAddr, addr))
		{
			selSess = sess;
			sess->lastDataTime = Data::Timestamp::UtcNow();
			break;
		}
	}
	if (!selSess.SetTo(sess))
	{
		NN<UDPTarget> target = me->targetList.GetItemNoCheck(me->nextTarget);
		me->nextTarget = (me->nextTarget + 1) % me->targetList.GetCount();
		NEW_CLASSNN(sess, UDPSession());
		sess->sourceAddr = *addr.Ptr();
		sess->sourcePort = port;
		sess->lastDataTime = Data::Timestamp::UtcNow();
		sess->targetAddr = target->targetAddr;
		sess->targetPort = target->targetPort;
		sess->me = me;
		sess->displayed = false;
		sess->sessCreatedTime = sess->lastDataTime;
		NEW_CLASSNN(sess->targetUDP, Net::UDPServer(me->sockf, 0, 0, 0, OnTargetUDPPacket, sess, me->log, 0, 2, true));
		me->sessList.Add(sess);
	}
	sess->targetUDP->SendTo(sess->targetAddr, sess->targetPort, data.Arr(), data.GetSize());
}

void __stdcall SSWR::AVIRead::AVIRUDPLoadBalancerForm::OnTargetUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<UDPSession> sess = userData.GetNN<UDPSession>();
	NN<Net::UDPServer> udp;
	Sync::MutexUsage mutUsage(sess->me->sessMut);
	if (sess->me->stopping || !sess->me->sourceUDP.SetTo(udp))
		return;
	udp->SendTo(sess->sourceAddr, sess->sourcePort, data.Arr(), data.GetSize());
}

void __stdcall SSWR::AVIRead::AVIRUDPLoadBalancerForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUDPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPLoadBalancerForm>();
	NN<Net::UDPServer> udp;
	if (me->sourceUDP.SetTo(udp))
	{
		me->stopping = true;
		{
			Sync::MutexUsage mutUsage(me->sessMut);
			me->sessList.FreeAll(FreeSession);
			me->lvSession->ClearItems();
		}
		me->sourceUDP.Delete();
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
	NEW_CLASSNN(udp, Net::UDPServer(me->sockf, 0, port, 0, OnSourceUDPPacket, me, me->log, 0, 4, true));
	if (udp->IsError())
	{
		udp.Delete();
		me->ui->ShowMsgOK(CSTR("Error in listening to UDP port"), TITLE, me);
		return;
	}
	me->sourceUDP = udp;
	me->txtListenerPort->SetReadOnly(true);
	me->txtTimeout->SetReadOnly(true);
	me->btnStart->SetText(CSTR("Stop"));
}

void __stdcall SSWR::AVIRead::AVIRUDPLoadBalancerForm::OnTargetAddClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUDPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPLoadBalancerForm>();
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
	NN<UDPTarget> target;
	NEW_CLASSNN(target, UDPTarget());
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

void __stdcall SSWR::AVIRead::AVIRUDPLoadBalancerForm::OnTargetDelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUDPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPLoadBalancerForm>();
	UOSInt i = me->lvTarget->GetSelectedIndex();
	if (i == INVALID_INDEX)
		return;
	Sync::MutexUsage mutUsage(me->sessMut);
	if (me->sourceUDP.NotNull() && me->targetList.GetCount() == 1)
	{
		me->ui->ShowMsgOK(CSTR("Cannot delete all target when it is started"), TITLE, me);
		return;
	}
	NN<UDPTarget> target;
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

void __stdcall SSWR::AVIRead::AVIRUDPLoadBalancerForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUDPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPLoadBalancerForm>();
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	if (me->sourceUDP.NotNull())
	{
		Sync::MutexUsage mutUsage(me->sessMut);
		NN<UDPSession> sess;
		Data::Timestamp currTime = Data::Timestamp::UtcNow();
		UOSInt i = 0;
		UOSInt j = me->sessList.GetCount();
		UOSInt k;
		while (i < j)
		{
			sess = me->sessList.GetItemNoCheck(i);
			if ((currTime - sess->lastDataTime).GetTotalSec() >= 900)
			{
				me->sessList.RemoveAt(i);
				if (sess->displayed)
				{
					me->lvSession->RemoveItem(i);
				}
				FreeSession(sess);
			}
			else if (!sess->displayed)
			{
				sess->displayed = true;
				sbuff[0] = 0;
				sptr = Net::SocketUtil::GetAddrName(sbuff, sess->sourceAddr, sess->sourcePort).Or(sbuff);
				k = me->lvSession->AddItem(CSTRP(sbuff, sptr), sess);
				sbuff[0] = 0;
				sptr = Net::SocketUtil::GetAddrName(sbuff, sess->targetAddr, sess->targetPort).Or(sbuff);
				me->lvSession->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				sptr = sess->sessCreatedTime.ToStringNoZone(sbuff);
				me->lvSession->SetSubItem(k, 2, CSTRP(sbuff, sptr));
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPLoadBalancerForm::FreeSession(NN<UDPSession> sess)
{
	sess->targetUDP.Delete();
	sess.Delete();
}

SSWR::AVIRead::AVIRUDPLoadBalancerForm::AVIRUDPLoadBalancerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(TITLE);

	this->stopping = false;
	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->sourceUDP = 0;
	this->nextTarget = 0;
	this->dispNextTarget = INVALID_INDEX;
	this->sessTimeout = 30000;

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
	this->lvSession = ui->NewListView(this->tpSession, UI::ListViewStyle::Table, 3);
	this->lvSession->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSession->SetFullRowSelect(true);
	this->lvSession->SetShowGrid(true);
	this->lvSession->AddColumn(CSTR("Source"), 150);
	this->lvSession->AddColumn(CSTR("Target"), 150);
	this->lvSession->AddColumn(CSTR("Session Start"), 150);

	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRUDPLoadBalancerForm::~AVIRUDPLoadBalancerForm()
{
	this->stopping = true;
	{
		Sync::MutexUsage mutUsage(this->sessMut);
		this->sessList.FreeAll(FreeSession);
	}
	this->sourceUDP.Delete();
	this->targetList.DeleteAll();
}

void SSWR::AVIRead::AVIRUDPLoadBalancerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
