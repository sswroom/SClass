#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/ARPInfo.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRIPScanForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRIPScanForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRIPScanForm *me = (SSWR::AVIRead::AVIRIPScanForm*)userObj;
	UInt8 buff[8];
	UTF8Char sbuff[32];
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
			UI::MessageDialog::ShowDialog((const UTF8Char*)"To many ip address", (const UTF8Char*)"Error", me);
			return;
		}

		me->scanner->Scan(ip);

		me->lvIP->ClearItems();
		Data::ArrayList<Net::ICMPScanner::ScanResult*> *resultList = me->scanner->GetResults();
		i = 0;
		j = resultList->GetCount();
		while (i < j)
		{
			result = resultList->GetItem(i);
			Net::SocketUtil::GetIPv4Name(sbuff, result->ip);
			me->lvIP->AddItem(sbuff, result);
			Text::StrHexBytes(sbuff, result->mac, 6, ':');
			me->lvIP->SetSubItem(i, 1, sbuff);
			me->lvIP->SetSubItem(i, 2, (const UTF8Char*)Net::MACInfo::GetMACInfoBuff(result->mac)->name);
			Text::StrDouble(sbuff, result->respTime);
			me->lvIP->SetSubItem(i, 3, sbuff);
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRIPScanForm::OnTimerTick(void *userObj)
{
//	SSWR::AVIRead::AVIRIPScanForm *me = (SSWR::AVIRead::AVIRIPScanForm*)userObj;
}

SSWR::AVIRead::AVIRIPScanForm::AVIRIPScanForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"IP Scan");

	this->core = core;
	this->sockf = core->GetSocketFactory();
	NEW_CLASS(this->scanner, Net::ICMPScanner(core->GetSocketFactory()));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblIP, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"IP"));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboIP, UI::GUIComboBox(ui, this->pnlControl, false));
	this->cboIP->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(254, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->lvIP, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvIP->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvIP->SetFullRowSelect(true);
	this->lvIP->SetShowGrid(true);
	this->lvIP->AddColumn((const UTF8Char*)"IP", 80);
	this->lvIP->AddColumn((const UTF8Char*)"MAC", 120);
	this->lvIP->AddColumn((const UTF8Char*)"Vendor", 250);
	this->lvIP->AddColumn((const UTF8Char*)"Resp", 80);
	
	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[32];
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
				Net::SocketUtil::IPType ipType = Net::SocketUtil::GetIPv4Type(ip);
				if (ipType == Net::SocketUtil::IT_PRIVATE)
				{
					Net::SocketUtil::GetIPv4Name(sbuff, ip);
					this->cboIP->AddItem(sbuff, (void*)(OSInt)ip);
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
