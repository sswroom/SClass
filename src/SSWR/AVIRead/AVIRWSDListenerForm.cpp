#include "Stdafx.h"
#include "IO/MemoryReadingStream.h"
#include "SSWR/AVIRead/AVIRWSDListenerForm.h"
#include "Text/XMLReader.h"

void __stdcall SSWR::AVIRead::AVIRWSDListenerForm::OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<SSWR::AVIRead::AVIRWSDListenerForm> me = userData.GetNN<SSWR::AVIRead::AVIRWSDListenerForm>();
	NN<PacketInfo> packet;
	NEW_CLASSNN(packet, PacketInfo());
	packet->addr = addr.Ptr()[0];
	packet->port = port;
	packet->ts = Data::Timestamp::Now();
	NEW_CLASSNN(packet->buff, Data::ByteBuffer(data));
	Sync::MutexUsage mutUsage(me->packetMut);
	me->packetList.Add(packet);
	me->packetUpdated = true;
}

void __stdcall SSWR::AVIRead::AVIRWSDListenerForm::OnTimerTick(AnyType userData)
{
	NN<SSWR::AVIRead::AVIRWSDListenerForm> me = userData.GetNN<SSWR::AVIRead::AVIRWSDListenerForm>();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<PacketInfo> packet;
	if (me->packetUpdated)
	{
		Sync::MutexUsage mutUsage(me->packetMut);
		while (me->packetList.GetCount() > 100)
		{
			me->lvPackets->RemoveItem(0);
			if (me->packetList.RemoveAt(0).SetTo(packet))
			{
				PacketInfoFree(packet);
			}
		}
		UIntOS i = me->lvPackets->GetCount();
		UIntOS j = me->packetList.GetCount();
		while (i < j)
		{
			packet = me->packetList.GetItemNoCheck(i);
			sptr = packet->ts.ToString(sbuff, "HH:mm:ss.ffffff");
			me->lvPackets->AddItem(CSTRP(sbuff, sptr), packet);
			sptr = Net::SocketUtil::GetAddrName(sbuff, packet->addr).Or(sbuff);
			*sptr++ = ':';
			sptr = Text::StrUInt16(sptr, packet->port);
			me->lvPackets->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			i++;
		}
		me->packetUpdated = false;
	}
}

void __stdcall SSWR::AVIRead::AVIRWSDListenerForm::OnPacketsSelChg(AnyType userData)
{
	NN<SSWR::AVIRead::AVIRWSDListenerForm> me = userData.GetNN<SSWR::AVIRead::AVIRWSDListenerForm>();
	NN<PacketInfo> packet;
	if (me->lvPackets->GetSelectedItem().GetOpt<PacketInfo>().SetTo(packet))
	{
		Text::StringBuilderUTF8 sb;
		IO::MemoryReadingStream mstm(*packet->buff.Ptr());
		if (Text::XMLReader::XMLWellFormat(me->core->GetEncFactory(), mstm, 0, sb))
		{
			me->txtRAW->SetText(sb.ToCString());
		}
		else
		{
			me->txtRAW->SetText(CSTR("Error in parsing as XML"));
		}
	}
	else
	{
		me->txtRAW->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRWSDListenerForm::PacketInfoFree(NN<PacketInfo> packet)
{
	packet->buff.Delete();
	packet.Delete();
}

SSWR::AVIRead::AVIRWSDListenerForm::AVIRWSDListenerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("WSD Listener"));
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lvPackets = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvPackets->SetRect(0, 0, 200, 23, false);
	this->lvPackets->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lvPackets->SetFullRowSelect(true);
	this->lvPackets->SetShowGrid(true);
	this->lvPackets->AddColumn(CSTR("Time"), 80);
	this->lvPackets->AddColumn(CSTR("Source"), 120);
	this->lvPackets->HandleSelChg(OnPacketsSelChg, this);
	this->hspPackets = ui->NewHSplitter(*this, 3, false);
	this->txtRAW = ui->NewTextBox(*this, CSTR(""), true);
	this->txtRAW->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtRAW->SetReadOnly(true);

	NEW_CLASSNN(this->listener, Net::UDPServer(core->GetSocketFactory(), nullptr, 3702, nullptr, OnUDPPacket, this, this->log, CSTR("UDP: "), 3, true));
	this->packetUpdated = false;
	if (this->listener->IsError())
	{
		ui->ShowMsgOK(CSTR("Error in initializing WSD Listener"), CSTR("WSD Listener"), this);
	}
	else
	{
		this->listener->AddMulticastIP(Net::SocketUtil::GetIPAddr(CSTR("239.255.255.250")));
	}
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRWSDListenerForm::~AVIRWSDListenerForm()
{
	this->listener.Delete();
	this->packetList.FreeAll(PacketInfoFree);
}

void SSWR::AVIRead::AVIRWSDListenerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
