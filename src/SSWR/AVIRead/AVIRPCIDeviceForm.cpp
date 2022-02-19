#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/DeviceDB.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRPCIDeviceForm.h"
#include "Text/MyString.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRPCIDeviceForm::OnDevicesSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRPCIDeviceForm *me = (SSWR::AVIRead::AVIRPCIDeviceForm*)userObj;
	IO::PCIInfo *pci;
	pci = (IO::PCIInfo*)me->lbDevices->GetSelectedItem();
	if (pci == 0)
	{
		me->txtVendorId->SetText(CSTR(""));
		me->txtVendorName->SetText(CSTR(""));
		me->txtProductId->SetText(CSTR(""));
		me->txtDispName->SetText(CSTR(""));
		me->txtDBName->SetText(CSTR(""));
	}
	else
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		sptr = Text::StrHexVal16(sbuff, pci->GetVendorId());
		me->txtVendorId->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrHexVal16(sbuff, pci->GetProductId());
		me->txtProductId->SetText(CSTRP(sbuff, sptr));
		me->txtDispName->SetText(pci->GetDispName());

		const IO::DeviceDB::PCIDeviceInfo *dev;
		dev = IO::DeviceDB::GetPCIInfo(pci->GetVendorId(), pci->GetProductId());
		if (dev)
		{
			me->txtDBName->SetText({(const UTF8Char*)dev->productName, Text::StrCharCnt(dev->productName)});
			Text::CString vendorName = IO::DeviceDB::GetPCIVendorName(dev->vendorId);
			if (vendorName.v)
			{
				me->txtVendorName->SetText(vendorName);
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

OSInt __stdcall SSWR::AVIRead::AVIRPCIDeviceForm::ItemCompare(void *item1, void *item2)
{
	IO::PCIInfo *pci1 = (IO::PCIInfo*)item1;
	IO::PCIInfo *pci2 = (IO::PCIInfo*)item2;
	if (pci1->GetVendorId() > pci2->GetVendorId())
	{
		return 1;
	}
	else if (pci1->GetVendorId() < pci2->GetVendorId())
	{
		return -1;
	}
	if (pci1->GetProductId() > pci2->GetProductId())
	{
		return 1;
	}
	else if (pci1->GetProductId() < pci2->GetProductId())
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

SSWR::AVIRead::AVIRPCIDeviceForm::AVIRPCIDeviceForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("PCI Devices"));
	this->SetFont(0, 0, 8.25, false);

	NEW_CLASS(this->lbDevices, UI::GUIListBox(ui, this, false));
	this->lbDevices->SetRect(0, 0, 100, 23, false);
	this->lbDevices->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevices->HandleSelectionChange(OnDevicesSelChg, this);
	NEW_CLASS(this->hspDevices, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlDevices, UI::GUIPanel(ui, this));
	this->pnlDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblVendorId, UI::GUILabel(ui, this->pnlDevices, CSTR("VendorId")));
	this->lblVendorId->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtVendorId, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtVendorId->SetRect(104, 4, 100, 23, false);
	this->txtVendorId->SetReadOnly(true);
	NEW_CLASS(this->lblVendorName, UI::GUILabel(ui, this->pnlDevices, CSTR("Vendor Name")));
	this->lblVendorName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtVendorName, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtVendorName->SetRect(104, 28, 500, 23, false);
	this->txtVendorName->SetReadOnly(true);
	NEW_CLASS(this->lblProductId, UI::GUILabel(ui, this->pnlDevices, CSTR("ProductId")));
	this->lblProductId->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtProductId, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtProductId->SetRect(104, 52, 100, 23, false);
	this->txtProductId->SetReadOnly(true);
	NEW_CLASS(this->lblDispName, UI::GUILabel(ui, this->pnlDevices, CSTR("Disp Name")));
	this->lblDispName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtDispName, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtDispName->SetRect(104, 76, 500, 23, false);
	this->txtDispName->SetReadOnly(true);
	NEW_CLASS(this->lblDBName, UI::GUILabel(ui, this->pnlDevices, CSTR("DB Name")));
	this->lblDBName->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtDBName, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtDBName->SetRect(104, 100, 500, 23, false);
	this->txtDBName->SetReadOnly(true);

	this->OnMonitorChanged();

	IO::PCIInfo *pci;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[32];
	UTF8Char *sptr;

	NEW_CLASS(this->pciList, Data::ArrayList<IO::PCIInfo*>());

	IO::PCIInfo::GetPCIList(this->pciList);
	void **arr = (void**)this->pciList->GetArray(&j);
	ArtificialQuickSort_SortCmp(arr, ItemCompare, 0, (OSInt)j - 1);
	
	i = 0;
	j = this->pciList->GetCount();
	while (i < j)
	{
		pci = this->pciList->GetItem(i);
		sptr = Text::StrHexVal16(sbuff, pci->GetVendorId());
		*sptr++ = ':';
		sptr = Text::StrHexVal16(sptr, pci->GetProductId());
		this->lbDevices->AddItem(CSTRP(sbuff, sptr), pci);
		i++;
	}
}

SSWR::AVIRead::AVIRPCIDeviceForm::~AVIRPCIDeviceForm()
{
	IO::PCIInfo *pci;
	UOSInt i = this->pciList->GetCount();
	while (i-- > 0)
	{
		pci = this->pciList->GetItem(i);
		DEL_CLASS(pci);
	}
	DEL_CLASS(this->pciList);
}

void SSWR::AVIRead::AVIRPCIDeviceForm::OnMonitorChanged()
{
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
}
