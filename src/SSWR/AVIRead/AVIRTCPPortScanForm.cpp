#include "Stdafx.h"
#include "Net/PacketAnalyzerEthernet.h"
#include "SSWR/AVIRead/AVIRTCPPortScanForm.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRTCPPortScanForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTCPPortScanForm *me = (SSWR::AVIRead::AVIRTCPPortScanForm*)userObj;
	if (me->scanner)
	{
		SDEL_CLASS(me->scanner);
		me->txtIP->SetReadOnly(false);
		me->txtThreadCnt->SetReadOnly(false);
		me->txtMaxPort->SetReadOnly(false);
	}
	else
	{
		UInt32 threadCnt = 0;
		UInt16 maxPort = 0;
		Net::SocketUtil::AddressInfo addr;
		Text::StringBuilderUTF8 sb;
		me->txtThreadCnt->GetText(&sb);
		if (!sb.ToUInt32(&threadCnt))
		{
			return;
		}
		sb.ClearStr();
		me->txtMaxPort->GetText(&sb);
		if (!sb.ToUInt16(&maxPort))
		{
			return;
		}
		sb.ClearStr();
		me->txtIP->GetText(&sb);
		if (me->sockf->DNSResolveIP(sb.ToString(), sb.GetLength(), &addr))
		{
			NEW_CLASS(me->scanner, Net::TCPPortScanner(me->sockf, threadCnt, OnPortUpdated, me));
			me->scanner->Start(&addr, maxPort);
			me->txtIP->SetReadOnly(true);
			me->txtThreadCnt->SetReadOnly(true);
			me->txtMaxPort->SetReadOnly(true);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPPortScanForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRTCPPortScanForm *me = (SSWR::AVIRead::AVIRTCPPortScanForm*)userObj;
	if (me->scanner)
	{
		if (me->listUpdated)
		{
			UTF8Char sbuff[32];
			UTF8Char *sptr;
			Data::ArrayList<UInt16> portList;
			UOSInt i;
			UOSInt j;
			UInt16 port;
			Text::CString cstr;
			me->listUpdated = false;
			me->scanner->GetAvailablePorts(&portList);
			me->lvPort->ClearItems();
			i = 0;
			j = portList.GetCount();
			while (i < j)
			{
				port = portList.GetItem(i);
				sptr = Text::StrUInt16(sbuff, port);
				me->lvPort->AddItem(CSTRP(sbuff, sptr), (void*)(UOSInt)port);
				cstr = Net::PacketAnalyzerEthernet::TCPPortGetName(port);
				if (cstr.v)
				{
					me->lvPort->SetSubItem(i, 1, cstr);
				}
				i++;
			}
		}

		if (me->scanner->IsFinished())
		{
			SDEL_CLASS(me->scanner);
			me->txtIP->SetReadOnly(false);
			me->txtThreadCnt->SetReadOnly(false);
			me->txtMaxPort->SetReadOnly(false);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPPortScanForm::OnPortUpdated(void *userObj, UInt16 port)
{
	SSWR::AVIRead::AVIRTCPPortScanForm *me = (SSWR::AVIRead::AVIRTCPPortScanForm*)userObj;
	me->listUpdated = true;
}

SSWR::AVIRead::AVIRTCPPortScanForm::AVIRTCPPortScanForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("TCP Port Scan"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->scanner = 0;
	this->listUpdated = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 79, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblIP, UI::GUILabel(ui, this->pnlControl, CSTR("IP")));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtIP, UI::GUITextBox(ui, this->pnlControl, CSTR("127.0.0.1")));
	this->txtIP->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblThreadCnt, UI::GUILabel(ui, this->pnlControl, CSTR("Thread Cnt")));
	this->lblThreadCnt->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtThreadCnt, UI::GUITextBox(ui, this->pnlControl, CSTR("20")));
	this->txtThreadCnt->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblMaxPort, UI::GUILabel(ui, this->pnlControl, CSTR("Max Port")));
	this->lblMaxPort->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtMaxPort, UI::GUITextBox(ui, this->pnlControl, CSTR("65535")));
	this->txtMaxPort->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, CSTR("Start")));
	this->btnStart->SetRect(204, 52, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->lvPort, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvPort->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPort->SetFullRowSelect(true);
	this->lvPort->SetShowGrid(true);
	this->lvPort->AddColumn(CSTR("Port"), 80);
	this->lvPort->AddColumn(CSTR("Name"), 200);
	
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRTCPPortScanForm::~AVIRTCPPortScanForm()
{
	SDEL_CLASS(this->scanner);
}

void SSWR::AVIRead::AVIRTCPPortScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
