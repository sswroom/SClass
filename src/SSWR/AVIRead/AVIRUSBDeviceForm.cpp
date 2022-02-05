#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/DeviceDB.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRUSBDeviceForm.h"
#include "Text/MyString.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRUSBDeviceForm::OnDevicesSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRUSBDeviceForm *me = (SSWR::AVIRead::AVIRUSBDeviceForm*)userObj;
	IO::USBInfo *usb;
	usb = (IO::USBInfo*)me->lbDevices->GetSelectedItem();
	if (usb == 0)
	{
		me->txtVendorId->SetText((const UTF8Char*)"");
		me->txtVendorName->SetText((const UTF8Char*)"");
		me->txtProductId->SetText((const UTF8Char*)"");
		me->txtDevice->SetText((const UTF8Char*)"");
		me->txtDispName->SetText((const UTF8Char*)"");
		me->txtDBName->SetText((const UTF8Char*)"");
	}
	else
	{
		UTF8Char sbuff[32];
		Text::StrHexVal16(sbuff, usb->GetVendorId());
		me->txtVendorId->SetText(sbuff);
		Text::StrHexVal16(sbuff, usb->GetProductId());
		me->txtProductId->SetText(sbuff);
		Text::StrHexVal16(sbuff, usb->GetRevision());
		me->txtDevice->SetText(sbuff);
		me->txtDispName->SetText(usb->GetDispName().v);

		const IO::DeviceDB::USBDeviceInfo *dev;
		dev = IO::DeviceDB::GetUSBInfo(usb->GetVendorId(), usb->GetProductId(), usb->GetRevision());
		if (dev)
		{
			me->txtDBName->SetText((const UTF8Char*)dev->productName);
			Text::CString vendorName = IO::DeviceDB::GetUSBVendorName(dev->vendorId);
			if (vendorName.v)
			{
				me->txtVendorName->SetText(vendorName.v);
			}
			else
			{
				me->txtVendorName->SetText((const UTF8Char*)"");
			}
		}
		else
		{
			me->txtDBName->SetText((const UTF8Char*)"");
			me->txtVendorName->SetText((const UTF8Char*)"");
		}
	}
}

OSInt __stdcall SSWR::AVIRead::AVIRUSBDeviceForm::ItemCompare(void *item1, void *item2)
{
	IO::USBInfo *usb1 = (IO::USBInfo*)item1;
	IO::USBInfo *usb2 = (IO::USBInfo*)item2;
	if (usb1->GetVendorId() > usb2->GetVendorId())
	{
		return 1;
	}
	else if (usb1->GetVendorId() < usb2->GetVendorId())
	{
		return -1;
	}
	if (usb1->GetProductId() > usb2->GetProductId())
	{
		return 1;
	}
	else if (usb1->GetProductId() < usb2->GetProductId())
	{
		return -1;
	}

	if (usb1->GetRevision() > usb2->GetRevision())
	{
		return 1;
	}
	else if (usb1->GetRevision() < usb2->GetRevision())
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

SSWR::AVIRead::AVIRUSBDeviceForm::AVIRUSBDeviceForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText((const UTF8Char*)"USB Devices");
	this->SetFont(0, 0, 8.25, false);

	NEW_CLASS(this->lbDevices, UI::GUIListBox(ui, this, false));
	this->lbDevices->SetRect(0, 0, 100, 23, false);
	this->lbDevices->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevices->HandleSelectionChange(OnDevicesSelChg, this);
	NEW_CLASS(this->hspDevices, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlDevices, UI::GUIPanel(ui, this));
	this->pnlDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblVendorId, UI::GUILabel(ui, this->pnlDevices, (const UTF8Char*)"VendorId"));
	this->lblVendorId->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtVendorId, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtVendorId->SetRect(104, 4, 100, 23, false);
	this->txtVendorId->SetReadOnly(true);
	NEW_CLASS(this->lblProductId, UI::GUILabel(ui, this->pnlDevices, (const UTF8Char*)"ProductId"));
	this->lblProductId->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtProductId, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtProductId->SetRect(104, 28, 100, 23, false);
	this->txtProductId->SetReadOnly(true);
	NEW_CLASS(this->lblDevice, UI::GUILabel(ui, this->pnlDevices, (const UTF8Char*)"Device"));
	this->lblDevice->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtDevice, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtDevice->SetRect(104, 52, 100, 23, false);
	this->txtDevice->SetReadOnly(true);
	NEW_CLASS(this->lblVendorName, UI::GUILabel(ui, this->pnlDevices, (const UTF8Char*)"Vendor Name"));
	this->lblVendorName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtVendorName, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtVendorName->SetRect(104, 76, 500, 23, false);
	this->txtVendorName->SetReadOnly(true);
	NEW_CLASS(this->lblDispName, UI::GUILabel(ui, this->pnlDevices, (const UTF8Char*)"Disp Name"));
	this->lblDispName->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtDispName, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtDispName->SetRect(104, 100, 500, 23, false);
	this->txtDispName->SetReadOnly(true);
	NEW_CLASS(this->lblDBName, UI::GUILabel(ui, this->pnlDevices, (const UTF8Char*)"DB Name"));
	this->lblDBName->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtDBName, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtDBName->SetRect(104, 124, 500, 23, false);
	this->txtDBName->SetReadOnly(true);

	this->OnMonitorChanged();

	IO::USBInfo *usb;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[32];
	UTF8Char *sptr;

	NEW_CLASS(this->usbList, Data::ArrayList<IO::USBInfo*>());

	IO::USBInfo::GetUSBList(this->usbList);
	void **arr = (void**)this->usbList->GetArray(&j);
	ArtificialQuickSort_SortCmp(arr, ItemCompare, 0, (OSInt)j - 1);
	
	i = 0;
	j = this->usbList->GetCount();
	while (i < j)
	{
		usb = this->usbList->GetItem(i);
		sptr = Text::StrHexVal16(sbuff, usb->GetVendorId());
		*sptr++ = ':';
		sptr = Text::StrHexVal16(sptr, usb->GetProductId());
		this->lbDevices->AddItem({sbuff, (UOSInt)(sptr - sbuff)}, usb);
		i++;
	}
}

SSWR::AVIRead::AVIRUSBDeviceForm::~AVIRUSBDeviceForm()
{
	IO::USBInfo *usb;
	UOSInt i = this->usbList->GetCount();
	while (i-- > 0)
	{
		usb = this->usbList->GetItem(i);
		DEL_CLASS(usb);
	}
	DEL_CLASS(this->usbList);
}

void SSWR::AVIRead::AVIRUSBDeviceForm::OnMonitorChanged()
{
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
}
