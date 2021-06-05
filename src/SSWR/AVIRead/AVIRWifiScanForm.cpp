#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRWifiScanForm.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRWifiScanForm::OnScanClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWifiScanForm *me = (SSWR::AVIRead::AVIRWifiScanForm*)userObj;
	me->WifiScan();
}

void __stdcall SSWR::AVIRead::AVIRWifiScanForm::OnWifiSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRWifiScanForm *me = (SSWR::AVIRead::AVIRWifiScanForm*)userObj;
	Net::WirelessLAN::BSSInfo *bss = (Net::WirelessLAN::BSSInfo*)me->lvWifi->GetSelectedItem();
	if (bss == 0)
	{
		me->txtWifi->SetText((const UTF8Char*)"");
	}
	else
	{
		UOSInt i;
		UOSInt j;
		Net::WirelessLANIE *ie;
		Text::StringBuilderUTF8 sb;
		i = 0;
		j = bss->GetIECount();
		while (i < j)
		{
			ie = bss->GetIE(i);
			if (i > 0)
			{
				sb.Append((const UTF8Char*)"\r\n");
			}
			Net::WirelessLANIE::ToString(ie->GetIEBuff(), &sb);
			i++;
		}
		me->txtWifi->SetText(sb.ToString());
	}
	
}

void SSWR::AVIRead::AVIRWifiScanForm::WifiScan()
{
	this->WifiClear();
	if (this->wlanInterf)
	{
		UTF8Char sbuff[64];
		UInt8 id[8];
		UInt64 imac;
		Manage::HiResClock clk;
		const UTF8Char *csptr;
		const UTF8Char *ssid;
		Data::ArrayList<Net::WirelessLAN::BSSInfo*> bssList;
		Net::WirelessLAN::BSSInfo *bss;
		Double t1;
		Double t2;
		UOSInt i;
		UOSInt j;
		this->wlanInterf->Scan();
		t1 = clk.GetTimeDiff();
		Sync::Thread::Sleep(5000);

		clk.Start();
		this->wlanInterf->GetBSSList(&bssList);
		t2 = clk.GetTimeDiff();

		Text::StrDouble(sbuff, t1);
		this->txtScanTime->SetText(sbuff);
		Text::StrDouble(sbuff, t2);
		this->txtResultTime->SetText(sbuff);

		i = 0;
		j = bssList.GetCount();
		while (i < j)
		{
			bss = bssList.GetItem(i);
			ssid = bss->GetSSID();
			MemCopyNO(&id[2], bss->GetMAC(), 6);
			id[0] = 0;
			id[1] = 0;
			imac = ReadMUInt64(id);

			this->lvWifi->AddItem(ssid, bss);
			Text::StrUInt32(sbuff, bss->GetPHYId());
			this->lvWifi->SetSubItem(i, 1, sbuff);
			Text::StrHexBytes(sbuff, &id[2], 6, ':');
			this->lvWifi->SetSubItem(i, 2, sbuff);
			Text::StrInt32(sbuff, bss->GetBSSType());
			this->lvWifi->SetSubItem(i, 3, (const UTF8Char*)Net::MACInfo::GetMACInfo(imac)->name);
			this->lvWifi->SetSubItem(i, 4, sbuff);
			Text::StrInt32(sbuff, bss->GetPHYType());
			this->lvWifi->SetSubItem(i, 5, sbuff);
			Text::StrDouble(sbuff, bss->GetRSSI());
			this->lvWifi->SetSubItem(i, 6, sbuff);
			Text::StrInt32(sbuff, bss->GetLinkQuality());
			this->lvWifi->SetSubItem(i, 7, sbuff);
			Text::StrDouble(sbuff, bss->GetFreq());
			this->lvWifi->SetSubItem(i, 8, sbuff);
			const UInt8 *oui;
			if ((csptr = bss->GetCountry()) != 0)
				this->lvWifi->SetSubItem(i, 9, csptr);
			oui = bss->GetChipsetOUI(0);
			if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
				this->lvWifi->SetSubItem(i, 10, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(oui)->name);
			oui = bss->GetChipsetOUI(1);
			if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
				this->lvWifi->SetSubItem(i, 11, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(oui)->name);
			oui = bss->GetChipsetOUI(2);
			if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
				this->lvWifi->SetSubItem(i, 12, (const UTF8Char*)Net::MACInfo::GetMACInfoOUI(oui)->name);
			if ((csptr = bss->GetManuf()) != 0)
				this->lvWifi->SetSubItem(i, 13, csptr);
			if ((csptr = bss->GetModel()) != 0)
				this->lvWifi->SetSubItem(i, 14, csptr);
			if ((csptr = bss->GetSN()) != 0)
				this->lvWifi->SetSubItem(i, 15, csptr);

			i++;
		}
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"No WiFi interface found", (const UTF8Char*)"WiFi Scan", this);
	}	
}

void SSWR::AVIRead::AVIRWifiScanForm::WifiClear()
{
	Net::WirelessLAN::BSSInfo *bss;
	UOSInt i = this->lvWifi->GetCount();
	while (i-- > 0)
	{
		bss = (Net::WirelessLAN::BSSInfo*)this->lvWifi->GetItem(i);
		DEL_CLASS(bss);
	}
	this->lvWifi->ClearItems();
}

SSWR::AVIRead::AVIRWifiScanForm::AVIRWifiScanForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Wifi Scan");

	this->core = core;
	NEW_CLASS(this->wlan, Net::WirelessLAN());
	Data::ArrayList<Net::WirelessLAN::Interface*> interfList;
	this->wlan->GetInterfaces(&interfList);
	this->wlanInterf = interfList.GetItem(0);
	UOSInt i = interfList.GetCount();
	while (i-- > 1)
	{
		Net::WirelessLAN::Interface *interf = interfList.GetItem(i);
		DEL_CLASS(interf);
	}


	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnScan, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Scan"));
	this->btnScan->SetRect(4, 4, 75, 23, false);
	this->btnScan->HandleButtonClick(OnScanClicked, this);
	NEW_CLASS(this->lblScanTime, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Scan Time"));
	this->lblScanTime->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->txtScanTime, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)""));
	this->txtScanTime->SetReadOnly(true);
	this->txtScanTime->SetRect(204, 4, 100, 23, false);
	NEW_CLASS(this->lblResultTime, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Result Time"));
	this->lblResultTime->SetRect(304, 4, 100, 23, false);
	NEW_CLASS(this->txtResultTime, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)""));
	this->txtResultTime->SetReadOnly(true);
	this->txtResultTime->SetRect(404, 4, 100, 23, false);
	NEW_CLASS(this->txtWifi, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtWifi->SetRect(0, 0, 100, 128, false);
	this->txtWifi->SetReadOnly(true);
	this->txtWifi->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->vspWifi, UI::GUIVSplitter(ui, this, 3, true));
	NEW_CLASS(this->lvWifi, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 16));
	this->lvWifi->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvWifi->SetShowGrid(true);
	this->lvWifi->SetFullRowSelect(true);
	this->lvWifi->AddColumn((const UTF8Char*)"SSID", 200);
	this->lvWifi->AddColumn((const UTF8Char*)"PhyId", 40);
	this->lvWifi->AddColumn((const UTF8Char*)"MAC", 120);
	this->lvWifi->AddColumn((const UTF8Char*)"Vendor", 120);
	this->lvWifi->AddColumn((const UTF8Char*)"BSSType", 60);
	this->lvWifi->AddColumn((const UTF8Char*)"PHYType", 60);
	this->lvWifi->AddColumn((const UTF8Char*)"RSSI", 60);
	this->lvWifi->AddColumn((const UTF8Char*)"Link Quality", 60);
	this->lvWifi->AddColumn((const UTF8Char*)"Frequency", 80);
	this->lvWifi->AddColumn((const UTF8Char*)"Country", 60);
	this->lvWifi->AddColumn((const UTF8Char*)"Vendor1", 120);
	this->lvWifi->AddColumn((const UTF8Char*)"Vendor2", 120);
	this->lvWifi->AddColumn((const UTF8Char*)"Vendor3", 120);
	this->lvWifi->AddColumn((const UTF8Char*)"Manufacturer", 100);
	this->lvWifi->AddColumn((const UTF8Char*)"Model", 100);
	this->lvWifi->AddColumn((const UTF8Char*)"S/N", 100);
	this->lvWifi->HandleSelChg(OnWifiSelChg, this);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::AVIRead::AVIRWifiScanForm::~AVIRWifiScanForm()
{
	this->WifiClear();
	SDEL_CLASS(this->wlanInterf);
	DEL_CLASS(this->wlan);
}

void SSWR::AVIRead::AVIRWifiScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
