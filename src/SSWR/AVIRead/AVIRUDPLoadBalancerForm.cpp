#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRUDPLoadBalancerForm.h"

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
	/////////////////////////////
}

void __stdcall SSWR::AVIRead::AVIRUDPLoadBalancerForm::OnTimerTick(AnyType userObj)
{
	/////////////////////////////
}

void __stdcall SSWR::AVIRead::AVIRUDPLoadBalancerForm::FreeSession(NN<UDPSession> sess)
{
	sess->targetUDP.Delete();
	sess.Delete();
}

SSWR::AVIRead::AVIRUDPLoadBalancerForm::AVIRUDPLoadBalancerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("UDP Load Balancer"));

	this->stopping = false;
	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->sourceUDP = 0;
	this->nextTarget = 0;

	this->pnlListener = ui->NewPanel(*this);
	this->pnlListener->SetRect(0, 0, 100, 79, false);
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
	/////////////////////////////
//	this->pnlTarget = ui->New
//	NN<UI::GUIPanel> pnlTarget;
//	NN<UI::GUIListView> lvTarget;
//	NN<UI::GUILabel> lblTargetIP;
//	NN<UI::GUITextBox> txtTargetIP;
//	NN<UI::GUILabel> lblTargetPort;
//	NN<UI::GUITextBox> txtTargetPort;
//	NN<UI::GUIButton> btnTargetAdd;
//	NN<UI::GUIButton> btnTargetDel;
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
