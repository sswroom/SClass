#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/AddressRange.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "Net/NetBIOSUtil.h"
#include "SSWR/AVIRead/AVIRNetBIOSScannerForm.h"

void __stdcall SSWR::AVIRead::AVIRNetBIOSScannerForm::OnRequestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRNetBIOSScannerForm *me = (SSWR::AVIRead::AVIRNetBIOSScannerForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Net::SocketUtil::AddressInfo addr;
	me->txtTargetAddr->GetText(sb);
	Net::AddressRange range(sb.ToString(), sb.GetLength(), me->chkTargetScan->IsChecked());
	if (range.GetCount() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Error in parsing Target Address"), CSTR("NetBIOS Scanner"), me);
		return;
	}
	if (range.GetCount() > 1024)
	{
		me->ui->ShowMsgOK(CSTR("Too many address to send"), CSTR("NetBIOS Scanner"), me);
		return;
	}

	UOSInt i = 0;
	UOSInt j = range.GetCount();
	while (i < j)
	{
		if (range.GetItem(i, &addr))
		{
			me->netbios->SendRequest(ReadNUInt32(addr.addr));
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRNetBIOSScannerForm::OnAnswerSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRNetBIOSScannerForm *me = (SSWR::AVIRead::AVIRNetBIOSScannerForm*)userObj;
	Sync::MutexUsage mutUsage;
	me->netbios->GetAnswers(mutUsage);
	Net::NetBIOSScanner::NameAnswer *ans = (Net::NetBIOSScanner::NameAnswer*)me->lvAnswers->GetSelectedItem();
	me->lvEntries->ClearItems();
	if (ans)
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		UOSInt i = 0;
		UOSInt j = ans->namesCnt;
		while (i < j)
		{
			me->lvEntries->AddItem({ans->names[i].nameBuff, Text::StrCharCnt(ans->names[i].nameBuff)}, 0);
			sptr = Text::StrHexByte(Text::StrConcatC(sbuff, UTF8STRC("0x")), ans->names[i].nameType);
			me->lvEntries->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			me->lvEntries->SetSubItem(i, 2, Net::NetBIOSUtil::NameTypeGetName(ans->names[i].nameType));
			sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), ans->names[i].flags);
			me->lvEntries->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRNetBIOSScannerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRNetBIOSScannerForm *me = (SSWR::AVIRead::AVIRNetBIOSScannerForm*)userObj;
	if (me->tableUpdated)
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		me->tableUpdated = false;
		me->lvAnswers->ClearItems();
		Sync::MutexUsage mutUsage;
		NotNullPtr<const Data::ReadingList<Net::NetBIOSScanner::NameAnswer*>> ansList = me->netbios->GetAnswers(mutUsage);
		Net::NetBIOSScanner::NameAnswer *ans;
		UOSInt i = 0;
		UOSInt j = ansList->GetCount();
		UOSInt k;
		while (i < j)
		{
			ans = ansList->GetItem(i);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadMUInt32((UInt8*)&ans->sortableIP));
			me->lvAnswers->AddItem(CSTRP(sbuff, sptr), ans);
			sptr = Text::StrHexBytes(sbuff, ans->unitId, 6, ':');
			me->lvAnswers->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			const Net::MACInfo::MACEntry *mac = Net::MACInfo::GetMACInfoBuff(ans->unitId);
			me->lvAnswers->SetSubItem(i, 2, {mac->name, mac->nameLen});
			sptr = Text::StrUInt32(sbuff, ans->ttl);
			me->lvAnswers->SetSubItem(i, 4, CSTRP(sbuff, sptr));
			if (ans->names)
			{
				k = 0;
				while (k < ans->namesCnt)
				{
					if (ans->names[k].nameType == 0 && (ans->names[k].flags & 0x8000) == 0)
					{
						me->lvAnswers->SetSubItem(i, 3, Text::CStringNN::FromPtr(ans->names[k].nameBuff));
						break;
					}
					k++;
				}
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRNetBIOSScannerForm::OnAnswerUpdated(void *userObj, UInt32 sortableIP)
{
	SSWR::AVIRead::AVIRNetBIOSScannerForm *me = (SSWR::AVIRead::AVIRNetBIOSScannerForm*)userObj;
	me->tableUpdated = true;
}

SSWR::AVIRead::AVIRNetBIOSScannerForm::AVIRNetBIOSScannerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("NetBIOS Scanner"));

	this->core = core;
	this->tableUpdated = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 104, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblTargetAddr = ui->NewLabel(this->pnlControl, CSTR("Target Addr"));
	this->lblTargetAddr->SetRect(4, 4, 100, 23, false);
	this->txtTargetAddr = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtTargetAddr->SetRect(104, 4, 200, 23, false);
	this->chkTargetScan = ui->NewCheckBox(this->pnlControl, CSTR("Scan IP"), false);
	this->chkTargetScan->SetRect(304, 4, 100, 23, false);
	this->btnRequest = ui->NewButton(this->pnlControl, CSTR("Request"));
	this->btnRequest->SetRect(104, 28, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);

	this->lvAnswers = ui->NewListView(*this, UI::ListViewStyle::Table, 5);
	this->lvAnswers->SetRect(0, 0, 100, 300, false);
	this->lvAnswers->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvAnswers->SetShowGrid(true);
	this->lvAnswers->SetFullRowSelect(true);
	this->lvAnswers->AddColumn(CSTR("IP"), 100);
	this->lvAnswers->AddColumn(CSTR("Unit ID"), 120);
	this->lvAnswers->AddColumn(CSTR("Vendor"), 120);
	this->lvAnswers->AddColumn(CSTR("Name"), 140);
	this->lvAnswers->AddColumn(CSTR("TTL"), 80);
	this->lvAnswers->HandleSelChg(OnAnswerSelChg, this);
	this->vspAnswers = ui->NewVSplitter(*this, 3, false);
	this->lvEntries = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
	this->lvEntries->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvEntries->SetShowGrid(true);
	this->lvEntries->SetFullRowSelect(true);
	this->lvEntries->AddColumn(CSTR("Name"), 140);
	this->lvEntries->AddColumn(CSTR("TypeCode"), 80);
	this->lvEntries->AddColumn(CSTR("TypeName"), 150);
	this->lvEntries->AddColumn(CSTR("Flags"), 80);

	NotNullPtr<Net::SocketFactory> sockf = this->core->GetSocketFactory();
	NEW_CLASS(this->netbios, Net::NetBIOSScanner(sockf, this->core->GetLog()));
	if (this->netbios->IsError())
	{
		this->ui->ShowMsgOK(CSTR("Error in starting NetBIOS Scanner"), CSTR("NetBIOS Scanner"), this);
	}
	this->netbios->SetAnswerHandler(OnAnswerUpdated, this);

	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UInt32 ip;
	UInt32 netmask;
	sockf->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		ip = connInfo->GetDefaultGW();
		if (ip)
		{
			netmask = Net::SocketUtil::GetDefNetMaskv4(ip);
			ip |= ~netmask;
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
			this->txtTargetAddr->SetText(CSTRP(sbuff, sptr));
		}
		DEL_CLASS(connInfo);
		i++;
	}

	this->AddTimer(500, OnTimerTick, this);
}

SSWR::AVIRead::AVIRNetBIOSScannerForm::~AVIRNetBIOSScannerForm()
{
	DEL_CLASS(this->netbios);
}

void SSWR::AVIRead::AVIRNetBIOSScannerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Bool SSWR::AVIRead::AVIRNetBIOSScannerForm::IsError()
{
	return this->netbios->IsError();
}
