#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRWifiScanForm.h"
#include "Sync/SimpleThread.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRWifiScanForm::OnScanClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRWifiScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiScanForm>();
	me->WifiScan();
}

void __stdcall SSWR::AVIRead::AVIRWifiScanForm::OnWifiSelChg(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRWifiScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiScanForm>();
	Net::WirelessLAN::BSSInfo *bss = (Net::WirelessLAN::BSSInfo*)me->lvWifi->GetSelectedItem();
	if (bss == 0)
	{
		me->txtWifi->SetText(CSTR(""));
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
				sb.AppendC(UTF8STRC("\r\n"));
			}
			Net::WirelessLANIE::ToString(ie->GetIEBuff(), sb);
			i++;
		}
		me->txtWifi->SetText(sb.ToCString());
	}
	
}

void SSWR::AVIRead::AVIRWifiScanForm::WifiScan()
{
	this->WifiClear();
	if (this->wlanInterf)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		UInt8 id[8];
		UInt64 imac;
		Manage::HiResClock clk;
		NotNullPtr<Text::String> s;
		const UTF8Char *csptr;
		NotNullPtr<Text::String> ssid;
		Data::ArrayList<Net::WirelessLAN::BSSInfo*> bssList;
		Net::WirelessLAN::BSSInfo *bss;
		Double t1;
		Double t2;
		UOSInt i;
		UOSInt j;
		this->wlanInterf->Scan();
		t1 = clk.GetTimeDiff();
		Sync::SimpleThread::Sleep(5000);

		clk.Start();
		this->wlanInterf->GetBSSList(&bssList);
		t2 = clk.GetTimeDiff();

		sptr = Text::StrDouble(sbuff, t1);
		this->txtScanTime->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, t2);
		this->txtResultTime->SetText(CSTRP(sbuff, sptr));

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
			sptr = Text::StrUInt32(sbuff, bss->GetPHYId());
			this->lvWifi->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			sptr = Text::StrHexBytes(sbuff, &id[2], 6, ':');
			this->lvWifi->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, bss->GetBSSType());
			const Net::MACInfo::MACEntry * entry = Net::MACInfo::GetMACInfo(imac);
			this->lvWifi->SetSubItem(i, 3, {entry->name, entry->nameLen});
			this->lvWifi->SetSubItem(i, 4, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, bss->GetPHYType());
			this->lvWifi->SetSubItem(i, 5, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, bss->GetRSSI());
			this->lvWifi->SetSubItem(i, 6, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, bss->GetLinkQuality());
			this->lvWifi->SetSubItem(i, 7, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, bss->GetFreq());
			this->lvWifi->SetSubItem(i, 8, CSTRP(sbuff, sptr));
			const UInt8 *oui;
			if ((csptr = bss->GetCountry()) != 0)
				this->lvWifi->SetSubItem(i, 9, Text::CStringNN::FromPtr(csptr));
			oui = bss->GetChipsetOUI(0);
			if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
			{
				entry = Net::MACInfo::GetMACInfoOUI(oui);
				this->lvWifi->SetSubItem(i, 10, {entry->name, entry->nameLen});
			}
			oui = bss->GetChipsetOUI(1);
			if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
			{
				entry = Net::MACInfo::GetMACInfoOUI(oui);
				this->lvWifi->SetSubItem(i, 11, {entry->name, entry->nameLen});
			}
			oui = bss->GetChipsetOUI(2);
			if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
			{
				entry = Net::MACInfo::GetMACInfoOUI(oui);
				this->lvWifi->SetSubItem(i, 12, {entry->name, entry->nameLen});
			}
			if (s.Set(bss->GetManuf()))
				this->lvWifi->SetSubItem(i, 13, s);
			if (s.Set(bss->GetModel()))
				this->lvWifi->SetSubItem(i, 14, s);
			if (s.Set(bss->GetSN()))
				this->lvWifi->SetSubItem(i, 15, s);

			i++;
		}
	}
	else
	{
		this->ui->ShowMsgOK(CSTR("No WiFi interface found"), CSTR("WiFi Scan"), this);
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

SSWR::AVIRead::AVIRWifiScanForm::AVIRWifiScanForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Wifi Scan"));

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

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnScan = ui->NewButton(this->pnlControl, CSTR("Scan"));
	this->btnScan->SetRect(4, 4, 75, 23, false);
	this->btnScan->HandleButtonClick(OnScanClicked, this);
	this->lblScanTime = ui->NewLabel(this->pnlControl, CSTR("Scan Time"));
	this->lblScanTime->SetRect(104, 4, 100, 23, false);
	this->txtScanTime = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtScanTime->SetReadOnly(true);
	this->txtScanTime->SetRect(204, 4, 100, 23, false);
	this->lblResultTime = ui->NewLabel(this->pnlControl, CSTR("Result Time"));
	this->lblResultTime->SetRect(304, 4, 100, 23, false);
	this->txtResultTime = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtResultTime->SetReadOnly(true);
	this->txtResultTime->SetRect(404, 4, 100, 23, false);
	this->txtWifi = ui->NewTextBox(*this, CSTR(""), true);
	this->txtWifi->SetRect(0, 0, 100, 128, false);
	this->txtWifi->SetReadOnly(true);
	this->txtWifi->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->vspWifi = ui->NewVSplitter(*this, 3, true);
	this->lvWifi = ui->NewListView(*this, UI::ListViewStyle::Table, 16);
	this->lvWifi->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvWifi->SetShowGrid(true);
	this->lvWifi->SetFullRowSelect(true);
	this->lvWifi->AddColumn(CSTR("SSID"), 200);
	this->lvWifi->AddColumn(CSTR("PhyId"), 40);
	this->lvWifi->AddColumn(CSTR("MAC"), 120);
	this->lvWifi->AddColumn(CSTR("Vendor"), 120);
	this->lvWifi->AddColumn(CSTR("BSSType"), 60);
	this->lvWifi->AddColumn(CSTR("PHYType"), 60);
	this->lvWifi->AddColumn(CSTR("RSSI"), 60);
	this->lvWifi->AddColumn(CSTR("Link Quality"), 60);
	this->lvWifi->AddColumn(CSTR("Frequency"), 80);
	this->lvWifi->AddColumn(CSTR("Country"), 60);
	this->lvWifi->AddColumn(CSTR("Vendor1"), 120);
	this->lvWifi->AddColumn(CSTR("Vendor2"), 120);
	this->lvWifi->AddColumn(CSTR("Vendor3"), 120);
	this->lvWifi->AddColumn(CSTR("Manufacturer"), 100);
	this->lvWifi->AddColumn(CSTR("Model"), 100);
	this->lvWifi->AddColumn(CSTR("S/N"), 100);
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
