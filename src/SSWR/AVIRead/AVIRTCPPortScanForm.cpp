#include "Stdafx.h"
#include "Net/PacketAnalyzerEthernet.h"
#include "SSWR/AVIRead/AVIRTCPPortScanForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRTCPPortScanForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPPortScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPPortScanForm>();
	NN<Net::TCPPortScanner> scanner;
	if (me->scanner.NotNull())
	{
		me->scanner.Delete();
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
		me->txtThreadCnt->GetText(sb);
		if (!sb.ToUInt32(threadCnt))
		{
			return;
		}
		sb.ClearStr();
		me->txtMaxPort->GetText(sb);
		if (!sb.ToUInt16(maxPort))
		{
			return;
		}
		sb.ClearStr();
		me->txtIP->GetText(sb);
		if (me->sockf->DNSResolveIP(sb.ToCString(), addr))
		{
			NEW_CLASSNN(scanner, Net::TCPPortScanner(me->sockf, threadCnt, OnPortUpdated, me));
			me->scanner = scanner;
			scanner->Start(&addr, maxPort);
			me->txtIP->SetReadOnly(true);
			me->txtThreadCnt->SetReadOnly(true);
			me->txtMaxPort->SetReadOnly(true);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPPortScanForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTCPPortScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPPortScanForm>();
	NN<Net::TCPPortScanner> scanner;
	if (me->scanner.SetTo(scanner))
	{
		if (me->listUpdated)
		{
			UTF8Char sbuff[32];
			UnsafeArray<UTF8Char> sptr;
			Data::ArrayList<UInt16> portList;
			UOSInt i;
			UOSInt j;
			UInt16 port;
			Text::CStringNN cstr;
			me->listUpdated = false;
			scanner->GetAvailablePorts(&portList);
			me->lvPort->ClearItems();
			i = 0;
			j = portList.GetCount();
			while (i < j)
			{
				port = portList.GetItem(i);
				sptr = Text::StrUInt16(sbuff, port);
				me->lvPort->AddItem(CSTRP(sbuff, sptr), (void*)(UOSInt)port);
				if (Net::PacketAnalyzerEthernet::TCPPortGetName(port).SetTo(cstr))
				{
					me->lvPort->SetSubItem(i, 1, cstr);
				}
				i++;
			}
		}

		if (scanner->IsFinished())
		{
			me->scanner.Delete();
			me->txtIP->SetReadOnly(false);
			me->txtThreadCnt->SetReadOnly(false);
			me->txtMaxPort->SetReadOnly(false);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRTCPPortScanForm::OnPortUpdated(AnyType userObj, UInt16 port)
{
	NN<SSWR::AVIRead::AVIRTCPPortScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTCPPortScanForm>();
	me->listUpdated = true;
}

SSWR::AVIRead::AVIRTCPPortScanForm::AVIRTCPPortScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("TCP Port Scan"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->scanner = 0;
	this->listUpdated = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 79, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblIP = ui->NewLabel(this->pnlControl, CSTR("IP"));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	this->txtIP = ui->NewTextBox(this->pnlControl, CSTR("127.0.0.1"));
	this->txtIP->SetRect(104, 4, 150, 23, false);
	this->lblThreadCnt = ui->NewLabel(this->pnlControl, CSTR("Thread Cnt"));
	this->lblThreadCnt->SetRect(4, 28, 100, 23, false);
	this->txtThreadCnt = ui->NewTextBox(this->pnlControl, CSTR("20"));
	this->txtThreadCnt->SetRect(104, 28, 100, 23, false);
	this->lblMaxPort = ui->NewLabel(this->pnlControl, CSTR("Max Port"));
	this->lblMaxPort->SetRect(4, 52, 100, 23, false);
	this->txtMaxPort = ui->NewTextBox(this->pnlControl, CSTR("65535"));
	this->txtMaxPort->SetRect(104, 52, 100, 23, false);
	this->btnStart = ui->NewButton(this->pnlControl, CSTR("Start"));
	this->btnStart->SetRect(204, 52, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lvPort = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvPort->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPort->SetFullRowSelect(true);
	this->lvPort->SetShowGrid(true);
	this->lvPort->AddColumn(CSTR("Port"), 80);
	this->lvPort->AddColumn(CSTR("Name"), 200);
	
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRTCPPortScanForm::~AVIRTCPPortScanForm()
{
	this->scanner.Delete();
}

void SSWR::AVIRead::AVIRTCPPortScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
