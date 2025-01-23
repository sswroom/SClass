#include "Stdafx.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRNetworkBandwidthForm.h"

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::OnLogClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRNetworkBandwidthForm> me = userObj.GetNN<SSWR::AVIRead::AVIRNetworkBandwidthForm>();
	if (me->logger.IsLogging())
	{
		me->logger.EndLogFile();
		me->txtLog->SetText(CSTR(""));
		return;
	}
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrInt64(sbuff, Data::DateTimeUtil::GetCurrTimeMillis());
	sptr = Text::StrConcatC(sptr, UTF8STRC(".csv"));
	if (me->logger.BeginLogFile(CSTRP(sbuff, sptr)))
	{
		me->txtLog->SetText(CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIRNetworkBandwidthForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRNetworkBandwidthForm> me = userObj.GetNN<SSWR::AVIRead::AVIRNetworkBandwidthForm>();
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> res;
	Sync::MutexUsage mutUsage;
	NN<const Data::UInt32FastMapNN<Net::EthernetAnalyzer::BandwidthStat>> ipStats = me->logger.GetIPStats(mutUsage);

	Int64 dispTime = Data::DateTimeUtil::GetCurrTimeMillis() / 1000 - 1;
	NN<Net::EthernetAnalyzer::BandwidthStat> stat;
	UInt32 ip;
	UOSInt i = 0;
	UOSInt j = me->lvDetail->GetCount();
	while (i < j)
	{
		stat = me->lvDetail->GetItem(i).GetNN<Net::EthernetAnalyzer::BandwidthStat>();
		if (stat == ipStats->GetItemNoCheck(i))
		{
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
			stat = ipStats->GetItemNoCheck(i);
#if IS_BYTEORDER_LE
			ip = BSWAPU32(stat->ip);
#else
			ip = stat->ip;
#endif
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
			me->lvDetail->InsertItem(i, CSTRP(sbuff, sptr), stat);
			j++;
			stat->displayTime = stat->lastStat.time;
			if (me->logger.GetIPName(stat->ip, mutUsage).SetTo(res))
			{
				stat->displayFlags |= 1;
				me->lvDetail->SetSubItem(i, 1, res);
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
	j = ipStats->GetCount();
	while (i < j)
	{
		stat = ipStats->GetItemNoCheck(i);
#if IS_BYTEORDER_LE
		ip = BSWAPU32(stat->ip);
#else
		ip = stat->ip;
#endif
		sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
		me->lvDetail->InsertItem(i, CSTRP(sbuff, sptr), stat);
		stat->displayTime = stat->lastStat.time;
		if (me->logger.GetIPName(stat->ip, mutUsage).SetTo(res))
		{
			stat->displayFlags |= 1;
			me->lvDetail->SetSubItem(i, 1, res);
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

SSWR::AVIRead::AVIRNetworkBandwidthForm::AVIRNetworkBandwidthForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 300, ui), logger(core->GetSocketFactory())
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Network Bandwidth"));

	this->core = core;
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

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRNetworkBandwidthForm::~AVIRNetworkBandwidthForm()
{
}

void SSWR::AVIRead::AVIRNetworkBandwidthForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
