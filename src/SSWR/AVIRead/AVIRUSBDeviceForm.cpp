#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "IO/DeviceDB.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRUSBDeviceForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRUSBDeviceForm::OnDevicesSelChg(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRUSBDeviceForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUSBDeviceForm>();
	IO::USBInfo *usb;
	usb = (IO::USBInfo*)me->lbDevices->GetSelectedItem().p;
	if (usb == 0)
	{
		me->txtVendorId->SetText(CSTR(""));
		me->txtVendorName->SetText(CSTR(""));
		me->txtProductId->SetText(CSTR(""));
		me->txtDevice->SetText(CSTR(""));
		me->txtDispName->SetText(CSTR(""));
		me->txtDBName->SetText(CSTR(""));
	}
	else
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		sptr = Text::StrHexVal16(sbuff, usb->GetVendorId());
		me->txtVendorId->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrHexVal16(sbuff, usb->GetProductId());
		me->txtProductId->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrHexVal16(sbuff, usb->GetRevision());
		me->txtDevice->SetText(CSTRP(sbuff, sptr));
		me->txtDispName->SetText(usb->GetDispName().OrEmpty());

		const IO::DeviceDB::USBDeviceInfo *dev;
		dev = IO::DeviceDB::GetUSBInfo(usb->GetVendorId(), usb->GetProductId(), usb->GetRevision());
		if (dev)
		{
			me->txtDBName->SetText({(const UTF8Char*)dev->productName, Text::StrCharCnt(dev->productName)});
			Text::CString vendorName = IO::DeviceDB::GetUSBVendorName(dev->vendorId);
			if (vendorName.v)
			{
				me->txtVendorName->SetText(vendorName.OrEmpty());
			}
			else
			{
				me->txtVendorName->SetText(CSTR(""));
			}
		}
		else
		{
			me->txtDBName->SetText(CSTR(""));
			me->txtVendorName->SetText(CSTR(""));
		}
	}
}

OSInt __stdcall SSWR::AVIRead::AVIRUSBDeviceForm::ItemCompare(IO::USBInfo *item1, IO::USBInfo *item2)
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

	if (item1->GetRevision() > item2->GetRevision())
	{
		return 1;
	}
	else if (item1->GetRevision() < item2->GetRevision())
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

SSWR::AVIRead::AVIRUSBDeviceForm::AVIRUSBDeviceForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("USB Devices"));
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
	this->lblDevice = ui->NewLabel(this->pnlDevices, CSTR("Device"));
	this->lblDevice->SetRect(4, 52, 100, 23, false);
	this->txtDevice = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtDevice->SetRect(104, 52, 100, 23, false);
	this->txtDevice->SetReadOnly(true);
	this->lblVendorName = ui->NewLabel(this->pnlDevices, CSTR("Vendor Name"));
	this->lblVendorName->SetRect(4, 76, 100, 23, false);
	this->txtVendorName = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtVendorName->SetRect(104, 76, 500, 23, false);
	this->txtVendorName->SetReadOnly(true);
	this->lblDispName = ui->NewLabel(this->pnlDevices, CSTR("Disp Name"));
	this->lblDispName->SetRect(4, 100, 100, 23, false);
	this->txtDispName = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtDispName->SetRect(104, 100, 500, 23, false);
	this->txtDispName->SetReadOnly(true);
	this->lblDBName = ui->NewLabel(this->pnlDevices, CSTR("DB Name"));
	this->lblDBName->SetRect(4, 124, 100, 23, false);
	this->txtDBName = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtDBName->SetRect(104, 124, 500, 23, false);
	this->txtDBName->SetReadOnly(true);

	this->OnMonitorChanged();

	IO::USBInfo *usb;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[32];
	UTF8Char *sptr;

	IO::USBInfo::GetUSBList(this->usbList);
	Data::Sort::ArtificialQuickSortFunc<IO::USBInfo*>::Sort(this->usbList, ItemCompare);
	
	i = 0;
	j = this->usbList.GetCount();
	while (i < j)
	{
		usb = this->usbList.GetItem(i);
		sptr = Text::StrHexVal16(sbuff, usb->GetVendorId());
		*sptr++ = ':';
		sptr = Text::StrHexVal16(sptr, usb->GetProductId());
		this->lbDevices->AddItem(CSTRP(sbuff, sptr), usb);
		i++;
	}
}

SSWR::AVIRead::AVIRUSBDeviceForm::~AVIRUSBDeviceForm()
{
	IO::USBInfo *usb;
	UOSInt i = this->usbList.GetCount();
	while (i-- > 0)
	{
		usb = this->usbList.GetItem(i);
		DEL_CLASS(usb);
	}
}

void SSWR::AVIRead::AVIRUSBDeviceForm::OnMonitorChanged()
{
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
}
