#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "IO/DeviceDB.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRHIDDeviceForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRHIDDeviceForm::OnDevicesSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRHIDDeviceForm *me = (SSWR::AVIRead::AVIRHIDDeviceForm*)userObj;
	IO::HIDInfo *hid;
	hid = (IO::HIDInfo*)me->lbDevices->GetSelectedItem();
	if (hid == 0)
	{
		me->txtVendorId->SetText(CSTR(""));
		me->txtProductId->SetText(CSTR(""));
		me->txtDevPath->SetText(CSTR(""));
	}
	else
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		sptr = Text::StrHexVal16(sbuff, hid->GetVendorId());
		me->txtVendorId->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrHexVal16(sbuff, hid->GetProductId());
		me->txtProductId->SetText(CSTRP(sbuff, sptr));
		me->txtDevPath->SetText(hid->GetDevPath()->ToCString());
	}
}

OSInt __stdcall SSWR::AVIRead::AVIRHIDDeviceForm::ItemCompare(IO::HIDInfo *item1, IO::HIDInfo *item2)
{
	if (item1->GetVendorId() > item2->GetVendorId())
	{
		return 1;
	}
	else if (item1->GetVendorId() < item2->GetVendorId())
	{
		return -1;
	}
	if (item1->GetProductId() > item2->GetProductId())
	{
		return 1;
	}
	else if (item1->GetProductId() < item2->GetProductId())
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

SSWR::AVIRead::AVIRHIDDeviceForm::AVIRHIDDeviceForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("HID Devices"));
	this->SetFont(0, 0, 8.25, false);

	this->lbDevices = ui->NewListBox(*this, false);
	this->lbDevices->SetRect(0, 0, 100, 23, false);
	this->lbDevices->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevices->HandleSelectionChange(OnDevicesSelChg, this);
	this->hspDevices = ui->NewHSplitter(*this, 3, false);
	this->pnlDevices = ui->NewPanel(*this);
	this->pnlDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblVendorId = ui->NewLabel(this->pnlDevices, CSTR("VendorId"));
	this->lblVendorId->SetRect(4, 4, 100, 23, false);
	this->txtVendorId = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtVendorId->SetRect(104, 4, 100, 23, false);
	this->txtVendorId->SetReadOnly(true);
	this->lblProductId = ui->NewLabel(this->pnlDevices, CSTR("ProductId"));
	this->lblProductId->SetRect(4, 28, 100, 23, false);
	this->txtProductId = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtProductId->SetRect(104, 28, 100, 23, false);
	this->txtProductId->SetReadOnly(true);
	this->lblDevPath = ui->NewLabel(this->pnlDevices, CSTR("Dev Path"));
	this->lblDevPath->SetRect(4, 52, 100, 23, false);
	this->txtDevPath = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtDevPath->SetRect(104, 52, 600, 23, false);
	this->txtDevPath->SetReadOnly(true);

	this->OnMonitorChanged();

	IO::HIDInfo *hid;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[32];
	UTF8Char *sptr;

	IO::HIDInfo::GetHIDList(this->hidList);
	Data::Sort::ArtificialQuickSortFunc<IO::HIDInfo*>::Sort(this->hidList, ItemCompare);
	
	i = 0;
	j = this->hidList.GetCount();
	while (i < j)
	{
		hid = this->hidList.GetItem(i);
		sptr = Text::StrHexVal16(sbuff, hid->GetVendorId());
		*sptr++ = ':';
		sptr = Text::StrHexVal16(sptr, hid->GetProductId());
		this->lbDevices->AddItem(CSTRP(sbuff, sptr), hid);
		i++;
	}
}

SSWR::AVIRead::AVIRHIDDeviceForm::~AVIRHIDDeviceForm()
{
	IO::HIDInfo *hid;
	UOSInt i = this->hidList.GetCount();
	while (i-- > 0)
	{
		hid = this->hidList.GetItem(i);
		DEL_CLASS(hid);
	}
}

void SSWR::AVIRead::AVIRHIDDeviceForm::OnMonitorChanged()
{
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
}
