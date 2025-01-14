#include "Stdafx.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRNetworkBandwidthForm.h"

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::OnIPClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRNetworkBandwidthForm> me = userObj.GetNN<SSWR::AVIRead::AVIRNetworkBandwidthForm>();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char buff[16];
	UOSInt i = me->cboIP->GetSelectedIndex();
	if (i != INVALID_INDEX && me->cboIP->GetItemText(sbuff, i).SetTo(sptr))
	{
		UInt32 ip = Net::SocketUtil::GetIPAddr(CSTRP(sbuff, sptr));
		if (ip != 0)
		{
			WriteNUInt32(buff, ip);
			ip = ReadMUInt32(buff);
			NN<TargetStat> stat = MemAllocNN(TargetStat);
			stat->addr = ip;
			stat->updated = true;
			TimeStatClear(stat->currStat);
			TimeStatClear(stat->lastStat);
			stat->currStat.time = Data::DateTimeUtil::GetCurrTimeMillis() / 1000;
			Sync::MutexUsage mutUsage(me->dispMut);
			me->lvDetail->AddItem(CSTRP(sbuff, sptr), stat);
			me->cboIP->RemoveItem(i);
			*sptr++ = '_';
			sptr = Text::StrInt64(sptr, stat->currStat.time);
			sptr = Text::StrConcatC(sptr, UTF8STRC(".csv"));
			NEW_CLASSNN(stat->fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			NEW_CLASSNN(stat->stm, IO::BufferedOutputStream(stat->fs, 16384));
			me->ipStats.Put(ip, stat);
		}
	}

}

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::OnMACClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRNetworkBandwidthForm> me = userObj.GetNN<SSWR::AVIRead::AVIRNetworkBandwidthForm>();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char buff[16];
	UOSInt i = me->cboMAC->GetSelectedIndex();
	if (i != INVALID_INDEX && me->cboMAC->GetItemText(sbuff, i).SetTo(sptr))
	{
		buff[0] = 0;
		buff[1] = 0;
		if (Text::StrHex2BytesS(sbuff, &buff[2], ':') == 6)
		{
			Int64 addr = ReadMInt64(buff);
			NN<TargetStat> stat = MemAllocNN(TargetStat);
			stat->addr = addr;
			stat->updated = true;
			TimeStatClear(stat->currStat);
			TimeStatClear(stat->lastStat);
			stat->currStat.time = Data::DateTimeUtil::GetCurrTimeMillis() / 1000;
			Sync::MutexUsage mutUsage(me->dispMut);
			me->lvDetail->AddItem(CSTRP(sbuff, sptr), stat);
			me->cboMAC->RemoveItem(i);
			sptr = Text::StrHexBytes(sbuff, &buff[2], 6, 0);
			*sptr++ = '_';
			sptr = Text::StrInt64(sptr, stat->currStat.time);
			sptr = Text::StrConcatC(sptr, UTF8STRC(".csv"));
			NEW_CLASSNN(stat->fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			NEW_CLASSNN(stat->stm, IO::BufferedOutputStream(stat->fs, 16384));
			me->macStats.Put(addr, stat);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRNetworkBandwidthForm> me = userObj.GetNN<SSWR::AVIRead::AVIRNetworkBandwidthForm>();
	UInt8 tmpBuff[8];
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	Bool found;
	Int64 addr;
	Int64 lastAddr;
	UOSInt lastIndex;
	UInt32 ip;
	UInt32 lastIP;
	Sync::MutexUsage mutUsage(me->dispMut);
	if (me->macUpdated)
	{
		me->macUpdated = false;
		lastIndex = 0;
		lastAddr = 0;
		i = me->cboMAC->GetSelectedIndex();
		if (i != INVALID_INDEX && me->cboMAC->GetItemText(sbuff, i).SetTo(sptr))
		{
			tmpBuff[0] = 0;
			tmpBuff[1] = 0;
			if (Text::StrHex2BytesS(sbuff, &tmpBuff[2], ':') == 6)
			{
				lastAddr = ReadMInt64(tmpBuff);
			}
		}
		me->cboMAC->ClearItems();
		found = false;
		i = 0;
		j = me->macList.GetCount();
		while (i < j)
		{
			addr = me->macList.GetItem(i);
			if (!me->macStats.ContainsKey(addr))
			{
				WriteMInt64(tmpBuff, addr);
				sptr = Text::StrHexBytes(sbuff, &tmpBuff[2], 6, ':');
				if (lastAddr == addr)
				{
					lastIndex = me->cboMAC->AddItem(CSTRP(sbuff, sptr), 0);
				}
				else
				{
					me->cboMAC->AddItem(CSTRP(sbuff, sptr), 0);
				}
				found = true;
			}
			i++;
		}
		if (found)
		{
			me->cboMAC->SetSelectedIndex(lastIndex);
		}
	}
	if (me->ipUpdated)
	{
		me->ipUpdated = false;
		lastIP = 0;
		lastIndex = 0;
		i = me->cboIP->GetSelectedIndex();
		if (i != INVALID_INDEX && me->cboIP->GetItemText(sbuff, i).SetTo(sptr))
		{
			lastIP = Net::SocketUtil::GetIPAddr(CSTRP(sbuff, sptr));
			#if IS_BYTEORDER_LE
			lastIP = BSWAPU32(lastIP);
			#endif
		}
		me->cboIP->ClearItems();
		found = false;
		i = 0;
		j = me->ipList.GetCount();
		while (i < j)
		{
			ip = me->ipList.GetItem(i);
			if (!me->ipStats.ContainsKey(ip))
			{
				WriteMUInt32(tmpBuff, ip);
				sptr = Text::StrUInt16(sbuff, tmpBuff[0]);
				*sptr++ = '.';
				sptr = Text::StrUInt16(sptr, tmpBuff[1]);
				*sptr++ = '.';
				sptr = Text::StrUInt16(sptr, tmpBuff[2]);
				*sptr++ = '.';
				sptr = Text::StrUInt16(sptr, tmpBuff[3]);
				if (ip == lastIP)
				{
					lastIndex = me->cboIP->AddItem(CSTRP(sbuff, sptr), 0);
				}
				else
				{
					me->cboIP->AddItem(CSTRP(sbuff, sptr), 0);
				}
				found = true;
			}
			i++;
		}
		if (found)
		{
			me->cboIP->SetSelectedIndex(lastIndex);
		}
	}
	NN<TargetStat> stat;
	i = 0;
	j = me->lvDetail->GetCount();
	while (i < j)
	{
		stat = me->lvDetail->GetItem(i).GetNN<TargetStat>();
		if (stat->updated)
		{
			stat->updated = false;
			sptr = Text::StrUOSInt(sbuff, stat->lastStat.recvCnt);
			me->lvDetail->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, stat->lastStat.recvBytes);
			me->lvDetail->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrUOSInt(sbuff, stat->lastStat.sendCnt);
			me->lvDetail->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, stat->lastStat.sendBytes);
			me->lvDetail->SetSubItem(i, 4, CSTRP(sbuff, sptr));
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::OnDataPacket(AnyType userData, UnsafeArray<const UInt8> packetData, UOSInt packetSize)
{
	NN<SSWR::AVIRead::AVIRNetworkBandwidthForm> me = userData.GetNN<SSWR::AVIRead::AVIRNetworkBandwidthForm>();
	Int64 currTime = Data::DateTimeUtil::GetCurrTimeMillis() / 1000;
	UInt8 tmpBuff[8];
	Int64 addr;
	OSInt si;
	NN<TargetStat> stat;
	tmpBuff[0] = 0;
	tmpBuff[1] = 0;
	Sync::MutexUsage mutUsage(me->dispMut);
	MemCopyNO(&tmpBuff[2], &packetData[0], 6);
	addr = ReadMInt64(tmpBuff);
	si = me->macList.SortedIndexOf(addr);
	if (si < 0)
	{
		me->macList.Insert((UOSInt)~si, addr);
		me->macUpdated = true;
	}
	if (me->macStats.Get(addr).SetTo(stat))
	{
		TargetStatTime(stat, currTime);
		stat->currStat.sendCnt++;
		stat->currStat.sendBytes += packetSize;
	}
	MemCopyNO(&tmpBuff[2], &packetData[6], 6);
	addr = ReadMInt64(tmpBuff);
	si = me->macList.SortedIndexOf(addr);
	if (si < 0)
	{
		me->macList.Insert((UOSInt)~si, addr);
		me->macUpdated = true;
	}
	if (me->macStats.Get(addr).SetTo(stat))
	{
		TargetStatTime(stat, currTime);
		stat->currStat.recvCnt++;
		stat->currStat.recvBytes += packetSize;
	}
	UInt16 etherType = ReadMUInt16(&packetData[12]);
	if (etherType == 0x0800)
	{
		if ((packetData[14 + 0] & 0xf0) == 0x40)
		{
			UInt32 srcIP = ReadMUInt32(&packetData[14 + 12]);
			UInt32 destIP = ReadMUInt32(&packetData[14 + 16]);
			si = me->ipList.SortedIndexOf(srcIP);
			if (si < 0)
			{
				me->ipList.Insert((UOSInt)~si, srcIP);
				me->ipUpdated = true;
			}
			if (me->ipStats.Get(srcIP).SetTo(stat))
			{
				TargetStatTime(stat, currTime);
				stat->currStat.recvCnt++;
				stat->currStat.recvBytes += packetSize;
			}
			si = me->ipList.SortedIndexOf(destIP);
			if (si < 0)
			{
				me->ipList.Insert((UOSInt)~si, destIP);
				me->ipUpdated = true;
			}
			if (me->ipStats.Get(destIP).SetTo(stat))
			{
				TargetStatTime(stat, currTime);
				stat->currStat.sendCnt++;
				stat->currStat.sendBytes += packetSize;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::TargetStatTime(NN<TargetStat> stat, Int64 time)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (stat->currStat.time != time)
	{
		stat->lastStat = stat->currStat;
		TimeStatClear(stat->currStat);
		stat->currStat.time = time;
		sptr = Text::StrInt64(sbuff, stat->lastStat.time);
		*sptr++ = ',';
		sptr = Text::StrUOSInt(sptr, stat->lastStat.recvCnt);
		*sptr++ = ',';
		sptr = Text::StrUInt64(sptr, stat->lastStat.recvBytes);
		*sptr++ = ',';
		sptr = Text::StrUOSInt(sptr, stat->lastStat.sendCnt);
		*sptr++ = ',';
		sptr = Text::StrUInt64(sptr, stat->lastStat.sendBytes);
		sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
		stat->stm->Write(Data::ByteArrayR(sbuff, (UOSInt)(sptr - sbuff)));
		stat->updated = true;
	}
}

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::TargetStatFree(NN<TargetStat> stat)
{
	stat->stm.Delete();
	stat->fs.Delete();
	MemFreeNN(stat);
}

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::TimeStatClear(NN<TimeStat> stat)
{
	stat->time = 0;
	stat->recvCnt = 0;
	stat->recvBytes = 0;
	stat->sendCnt = 0;
	stat->sendBytes = 0;
}

SSWR::AVIRead::AVIRNetworkBandwidthForm::AVIRNetworkBandwidthForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 300, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Network Bandwidth"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->socMon = 0;
	this->ipUpdated = false;
	this->macUpdated = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 55, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblIP = ui->NewLabel(this->pnlControl, CSTR("IPv4"));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	this->cboIP = ui->NewComboBox(this->pnlControl, false);
	this->cboIP->SetRect(104, 4, 150, 23, false);
	this->btnIP = ui->NewButton(this->pnlControl, CSTR("Monitor"));
	this->btnIP->SetRect(254, 4, 75, 23, false);
	this->btnIP->HandleButtonClick(OnIPClicked, this);
	this->lblMAC = ui->NewLabel(this->pnlControl, CSTR("MAC"));
	this->lblMAC->SetRect(4, 28, 100, 23, false);
	this->cboMAC = ui->NewComboBox(this->pnlControl, false);
	this->cboMAC->SetRect(104, 28, 150, 23, false);
	this->btnMAC = ui->NewButton(this->pnlControl, CSTR("Monitor"));
	this->btnMAC->SetRect(254, 28, 75, 23, false);
	this->btnMAC->HandleButtonClick(OnMACClicked, this);
	this->lvDetail = ui->NewListView(*this, UI::ListViewStyle::Table, 5);
	this->lvDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetail->SetFullRowSelect(true);
	this->lvDetail->SetShowGrid(true);
	this->lvDetail->AddColumn(CSTR("Address"), 120);
	this->lvDetail->AddColumn(CSTR("Recv Cnt"), 60);
	this->lvDetail->AddColumn(CSTR("Recv Rate"), 120);
	this->lvDetail->AddColumn(CSTR("Send Cnt"), 60);
	this->lvDetail->AddColumn(CSTR("Send Rate"), 120);

	NN<Socket> s;
	if (sockf->CreateRAWSocket().SetTo(s))
	{
		NEW_CLASSOPT(this->socMon, Net::SocketMonitor(this->sockf, s, OnDataPacket, this, 4));
	}
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRNetworkBandwidthForm::~AVIRNetworkBandwidthForm()
{
	this->socMon.Delete();
	this->macStats.FreeAll(TargetStatFree);
	this->ipStats.FreeAll(TargetStatFree);
}

void SSWR::AVIRead::AVIRNetworkBandwidthForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
