#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/ARPInfo.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRIPScanForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRIPScanForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRIPScanForm *me = (SSWR::AVIRead::AVIRIPScanForm*)userObj;
	UInt8 buff[8];
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Net::ICMPScanner::ScanResult *result;
	UOSInt i;
	UOSInt j;
	UInt32 ip = (UInt32)(OSInt)me->cboIP->GetSelectedItem();
	if (ip != 0)
	{
		WriteNUInt32(buff, ip);
		if (buff[0] == 192 && buff[1] == 168)
		{
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("To many ip address"), CSTR("Error"), me);
			return;
		}

		me->scanner->Scan(ip);

		me->lvIP->ClearItems();
		const Data::ReadingList<Net::ICMPScanner::ScanResult*> *resultList = me->scanner->GetResults();
		i = 0;
		j = resultList->GetCount();
		while (i < j)
		{
			result = resultList->GetItem(i);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, result->ip);
			me->lvIP->AddItem(CSTRP(sbuff, sptr), result);
			sptr = Text::StrHexBytes(sbuff, result->mac, 6, ':');
			me->lvIP->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			const Net::MACInfo::MACEntry *entry = Net::MACInfo::GetMACInfoBuff(result->mac);
			me->lvIP->SetSubItem(i, 2, {entry->name, entry->nameLen});
			sptr = Text::StrDouble(sbuff, result->respTime);
			me->lvIP->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRIPScanForm::OnTimerTick(void *userObj)
{
//	SSWR::AVIRead::AVIRIPScanForm *me = (SSWR::AVIRead::AVIRIPScanForm*)userObj;
}

SSWR::AVIRead::AVIRIPScanForm::AVIRIPScanForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("IP Scan"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	NEW_CLASS(this->scanner, Net::ICMPScanner(core->GetSocketFactory()));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblIP = ui->NewLabel(this->pnlControl, CSTR("IP"));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	this->cboIP = ui->NewComboBox(this->pnlControl, false);
	this->cboIP->SetRect(104, 4, 150, 23, false);
	this->btnStart = ui->NewButton(this->pnlControl, CSTR("Start"));
	this->btnStart->SetRect(254, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lvIP = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
	this->lvIP->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvIP->SetFullRowSelect(true);
	this->lvIP->SetShowGrid(true);
	this->lvIP->AddColumn(CSTR("IP"), 80);
	this->lvIP->AddColumn(CSTR("MAC"), 120);
	this->lvIP->AddColumn(CSTR("Vendor"), 250);
	this->lvIP->AddColumn(CSTR("Resp"), 80);
	
	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 ip;
	this->sockf->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		if (connInfo->GetConnectionStatus() == Net::ConnectionInfo::CS_UP)
		{
			k = 0;
			while (true)
			{
				ip = connInfo->GetIPAddress(k);
				if (ip == 0)
					break;
				Net::IPType ipType = Net::SocketUtil::GetIPv4Type(ip);
				if (ipType == Net::IPType::Private)
				{
					sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
					this->cboIP->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)ip);
				}
				k++;
			}
		}
		DEL_CLASS(connInfo);
		i++;
	}
	if (this->cboIP->GetCount() > 0)
	{
		this->cboIP->SetSelectedIndex(0);
	}
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRIPScanForm::~AVIRIPScanForm()
{
	DEL_CLASS(this->scanner);
}

void SSWR::AVIRead::AVIRIPScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
