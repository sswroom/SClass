#include "Stdafx.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRNetworkBandwidthForm.h"

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::OnLogClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRNetworkBandwidthForm> me = userObj.GetNN<SSWR::AVIRead::AVIRNetworkBandwidthForm>();
	NN<IO::Stream> stm;
	if (me->stm.SetTo(stm))
	{
		me->stm.Delete();
		me->fs.Delete();
		me->txtLog->SetText(CSTR(""));
		return;
	}
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrInt64(sbuff, Data::DateTimeUtil::GetCurrTimeMillis());
	sptr = Text::StrConcatC(sptr, UTF8STRC(".csv"));
	NEW_CLASSNN(stm, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	me->fs = stm;
	NEW_CLASSOPT(me->stm, IO::BufferedOutputStream(stm, 16384));
	me->txtLog->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRNetworkBandwidthForm> me = userObj.GetNN<SSWR::AVIRead::AVIRNetworkBandwidthForm>();
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<DNSResult> res;
	Sync::MutexUsage mutUsage(me->dispMut);

	{
		Int64 dispTime = Data::DateTimeUtil::GetCurrTimeMillis() / 1000 - 1;
		Sync::MutexUsage mutUsage(me->dispMut);
		NN<Net::EthernetAnalyzer::BandwidthStat> stat;
		UInt32 ip;
		UOSInt i = 0;
		UOSInt j = me->lvDetail->GetCount();
		while (i < j)
		{
			stat = me->lvDetail->GetItem(i).GetNN<Net::EthernetAnalyzer::BandwidthStat>();
			if (stat == me->ipStats.GetItemNoCheck(i))
			{
/*				if ((stat->displayFlags & 1) == 0)
				{
#if IS_BYTEORDER_LE
					ip = BSWAPU32(stat->ip);
#else
					ip = stat->ip;
#endif
					if (me->analyzer->DNSTargetGetName(ip).SetTo(s))
					{
						stat->displayFlags |= 1;
						me->lvBandwidth->SetSubItem(i, 1, s);
					}
				}*/
				if (dispTime <= stat->displayTime)
				{
				}
				else if (stat->displayTime > stat->currStat.time)
				{
				}
				else if (dispTime > stat->currStat.time)
				{
					stat->displayTime = dispTime;
					me->lvDetail->SetSubItem(i, 2, CSTR("0"));
					me->lvDetail->SetSubItem(i, 3, CSTR("0"));
					me->lvDetail->SetSubItem(i, 4, CSTR("0"));
					me->lvDetail->SetSubItem(i, 5, CSTR("0"));
				}
				else if (dispTime == stat->lastStat.time)
				{
					stat->displayTime = dispTime;
					sptr = Text::StrUInt64(sbuff, stat->lastStat.recvBytes);
					me->lvDetail->SetSubItem(i, 2, CSTRP(sbuff, sptr));
					sptr = Text::StrUOSInt(sbuff, stat->lastStat.recvCnt);
					me->lvDetail->SetSubItem(i, 3, CSTRP(sbuff, sptr));
					sptr = Text::StrUInt64(sbuff, stat->lastStat.sendBytes);
					me->lvDetail->SetSubItem(i, 4, CSTRP(sbuff, sptr));
					sptr = Text::StrUOSInt(sbuff, stat->lastStat.sendCnt);
					me->lvDetail->SetSubItem(i, 5, CSTRP(sbuff, sptr));
				}
				else if (dispTime == stat->currStat.time)
				{
					stat->displayTime = dispTime;
					sptr = Text::StrUInt64(sbuff, stat->currStat.recvBytes);
					me->lvDetail->SetSubItem(i, 2, CSTRP(sbuff, sptr));
					sptr = Text::StrUOSInt(sbuff, stat->currStat.recvCnt);
					me->lvDetail->SetSubItem(i, 3, CSTRP(sbuff, sptr));
					sptr = Text::StrUInt64(sbuff, stat->currStat.sendBytes);
					me->lvDetail->SetSubItem(i, 4, CSTRP(sbuff, sptr));
					sptr = Text::StrUOSInt(sbuff, stat->currStat.sendCnt);
					me->lvDetail->SetSubItem(i, 5, CSTRP(sbuff, sptr));
				}
				else if (stat->displayTime == stat->lastStat.time)
				{
					stat->displayTime = dispTime;
					me->lvDetail->SetSubItem(i, 2, CSTR("0"));
					me->lvDetail->SetSubItem(i, 3, CSTR("0"));
					me->lvDetail->SetSubItem(i, 4, CSTR("0"));
					me->lvDetail->SetSubItem(i, 5, CSTR("0"));
				}
				else
				{
				}
			}
			else
			{
				stat = me->ipStats.GetItemNoCheck(i);
#if IS_BYTEORDER_LE
				ip = BSWAPU32(stat->ip);
#else
				ip = stat->ip;
#endif
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
				me->lvDetail->InsertItem(i, CSTRP(sbuff, sptr), stat);
				j++;
				stat->displayTime = stat->lastStat.time;
				if (me->dnsRes.Get(stat->ip).SetTo(res))
				{
					stat->displayFlags |= 1;
					me->lvDetail->SetSubItem(i, 1, res->name);
				}
				sptr = Text::StrUInt64(sbuff, stat->lastStat.recvBytes);
				me->lvDetail->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrUOSInt(sbuff, stat->lastStat.recvCnt);
				me->lvDetail->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, stat->lastStat.sendBytes);
				me->lvDetail->SetSubItem(i, 4, CSTRP(sbuff, sptr));
				sptr = Text::StrUOSInt(sbuff, stat->lastStat.sendCnt);
				me->lvDetail->SetSubItem(i, 5, CSTRP(sbuff, sptr));
			}
			i++;
		}
		i = j;
		j = me->ipStats.GetCount();
		while (i < j)
		{
			stat = me->ipStats.GetItemNoCheck(i);
#if IS_BYTEORDER_LE
			ip = BSWAPU32(stat->ip);
#else
			ip = stat->ip;
#endif
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
			me->lvDetail->InsertItem(i, CSTRP(sbuff, sptr), stat);
			stat->displayTime = stat->lastStat.time;
			if (me->dnsRes.Get(stat->ip).SetTo(res))
			{
				stat->displayFlags |= 1;
				me->lvDetail->SetSubItem(i, 1, res->name);
			}
			sptr = Text::StrUInt64(sbuff, stat->lastStat.recvBytes);
			me->lvDetail->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrUOSInt(sbuff, stat->lastStat.recvCnt);
			me->lvDetail->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, stat->lastStat.sendBytes);
			me->lvDetail->SetSubItem(i, 4, CSTRP(sbuff, sptr));
			sptr = Text::StrUOSInt(sbuff, stat->lastStat.sendCnt);
			me->lvDetail->SetSubItem(i, 5, CSTRP(sbuff, sptr));
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::OnDataPacket(AnyType userData, UnsafeArray<const UInt8> packetData, UOSInt packetSize)
{
	NN<SSWR::AVIRead::AVIRNetworkBandwidthForm> me = userData.GetNN<SSWR::AVIRead::AVIRNetworkBandwidthForm>();
	Int64 currTime = Data::DateTimeUtil::GetCurrTimeMillis() / 1000;
	NN<Net::EthernetAnalyzer::BandwidthStat> stat;
	Sync::MutexUsage mutUsage(me->dispMut);
	UInt16 etherType = ReadMUInt16(&packetData[12]);
	if (etherType == 0x0800)
	{
		if ((packetData[14 + 0] & 0xf0) == 0x40)
		{
			UInt32 srcIP = ReadMUInt32(&packetData[14 + 12]);
			UInt32 destIP = ReadMUInt32(&packetData[14 + 16]);
			if (!me->ipStats.Get(srcIP).SetTo(stat))
			{
				stat = Net::EthernetAnalyzer::BandwidthStatCreate(srcIP, currTime);
				me->ipStats.Put(srcIP, stat);
			}
			me->BandwidthStatTime(stat, currTime);
			stat->currStat.recvBytes += packetSize + 14;
			stat->currStat.recvCnt++;

			if (!me->ipStats.Get(destIP).SetTo(stat))
			{
				stat = Net::EthernetAnalyzer::BandwidthStatCreate(destIP, currTime);
				me->ipStats.Put(destIP, stat);
			}
			me->BandwidthStatTime(stat, currTime);
			stat->currStat.sendBytes += packetSize + 14;
			stat->currStat.sendCnt++;

			if (packetData[14 + 9] == 17)
			{
				UnsafeArray<const UInt8> ipData;
				UOSInt ipDataSize;
				if ((packetData[14] & 0xf) <= 5)
				{
					ipData = &packetData[14 + 20];
					ipDataSize = packetSize - 20 - 14;
				}
				else
				{
					ipData = &packetData[14 + ((packetData[14] & 0xf) << 2)];
					ipDataSize = packetSize - ((packetData[14] & 0xf) << 2) - 14;
				}
				if (ipDataSize >= 8)
				{
					UInt16 srcPort = ReadMUInt16(&ipData[0]);
					UInt16 udpLeng = ReadMUInt16(&ipData[4]);
					if (udpLeng <= ipDataSize && srcPort == 53)
					{
						Data::ArrayListNN<Net::DNSClient::RequestAnswer> answers;
						NN<Net::DNSClient::RequestAnswer> answer;
						Net::DNSClient::ParseAnswers(&ipData[8], ipDataSize - 8, answers);
						if (answers.GetCount() > 0)
						{
							NN<DNSResult> res;
							NN<Text::String> reqName;
							Data::Timestamp currTime = Data::Timestamp::UtcNow();
							reqName = answers.GetItemNoCheck(0)->name;
							answer = answers.GetItemNoCheck(answers.GetCount() - 1);
							if (answer->recType == 1)
							{
								UOSInt i;
								UInt32 sortIP;
								i = answers.GetCount();
								while (i-- > 0)
								{
									answer = answers.GetItemNoCheck(i);
									if (answer->recType == 1)
									{
										sortIP = ReadMUInt32(answer->addr.addr);
										if (!me->dnsRes.Get(sortIP).SetTo(res))
										{
											res = MemAllocNN(DNSResult);
											res->ip = sortIP;
											res->name = reqName->Clone();
											me->dnsRes.Put(sortIP, res);
										}
										else if (!res->name->Equals(reqName))
										{
											res->name->Release();
											res->name = reqName->Clone();
										}
									}
								}
							}
							Net::DNSClient::FreeAnswers(answers);
						}					
					}
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::DNSResultFree(NN<DNSResult> res)
{
	res->name->Release();
	MemFreeNN(res);
}

void SSWR::AVIRead::AVIRNetworkBandwidthForm::BandwidthStatTime(NN<Net::EthernetAnalyzer::BandwidthStat> stat, Int64 time)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt32 ip;
	NN<IO::Stream> stm;
	if (stat->currStat.time != time)
	{
		stat->lastStat = stat->currStat;
		stat->currStat.time = time;
		stat->currStat.recvBytes = 0;
		stat->currStat.recvCnt = 0;
		stat->currStat.sendBytes = 0;
		stat->currStat.sendCnt = 0;

		if (this->stm.SetTo(stm))
		{
#if IS_BYTEORDER_LE
			ip = BSWAPU32(stat->ip);
#else
			ip = stat->ip;
#endif
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
			*sptr++ = ',';
			sptr = Text::StrInt64(sptr, stat->lastStat.time);
			*sptr++ = ',';
			sptr = Text::StrUOSInt(sptr, stat->lastStat.recvCnt);
			*sptr++ = ',';
			sptr = Text::StrUInt64(sptr, stat->lastStat.recvBytes);
			*sptr++ = ',';
			sptr = Text::StrUOSInt(sptr, stat->lastStat.sendCnt);
			*sptr++ = ',';
			sptr = Text::StrUInt64(sptr, stat->lastStat.sendBytes);
			sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
			stm->WriteCont(sbuff, (UOSInt)(sptr - sbuff));
		}
	}
}

SSWR::AVIRead::AVIRNetworkBandwidthForm::AVIRNetworkBandwidthForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 300, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Network Bandwidth"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->socMon = 0;
	this->fs = 0;
	this->stm = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnLog = ui->NewButton(this->pnlControl, CSTR("Log"));
	this->btnLog->SetRect(4, 4, 75, 23, false);
	this->btnLog->HandleButtonClick(OnLogClicked, this);
	this->txtLog = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetRect(84, 4, 200, 23, false);
	this->lvDetail = ui->NewListView(*this, UI::ListViewStyle::Table, 6);
	this->lvDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDetail->SetFullRowSelect(true);
	this->lvDetail->SetShowGrid(true);
	this->lvDetail->AddColumn(CSTR("IP"), 120);
	this->lvDetail->AddColumn(CSTR("Name"), 200);
	this->lvDetail->AddColumn(CSTR("Recv Rate"), 120);
	this->lvDetail->AddColumn(CSTR("Recv Cnt"), 60);
	this->lvDetail->AddColumn(CSTR("Send Rate"), 120);
	this->lvDetail->AddColumn(CSTR("Send Cnt"), 60);

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
	this->stm.Delete();
	this->fs.Delete();
	this->ipStats.MemFreeAll();
	this->dnsRes.FreeAll(DNSResultFree);
}

void SSWR::AVIRead::AVIRNetworkBandwidthForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
