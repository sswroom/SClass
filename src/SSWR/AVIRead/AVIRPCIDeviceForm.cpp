#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "IO/DeviceDB.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRPCIDeviceForm.h"
#include "Text/MyString.h"
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
		me->txtDispName->SetText(pci->GetDispName().OrEmpty());

		const IO::DeviceDB::DeviceInfo *dev;
		dev = IO::DeviceDB::GetPCIInfo(pci->GetVendorId(), pci->GetProductId());
		if (dev)
		{
			me->txtDBName->SetText({(const UTF8Char*)dev->productName, Text::StrCharCnt(dev->productName)});
			me->txtVendorName->SetText(IO::DeviceDB::GetPCIVendorName(dev->vendorId).OrEmpty());
		}
		else
		{
			me->txtDBName->SetText(CSTR(""));
			me->txtVendorName->SetText(CSTR(""));
		}
	}
}

OSInt __stdcall SSWR::AVIRead::AVIRPCIDeviceForm::ItemCompare(IO::PCIInfo *item1, IO::PCIInfo *item2)
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

SSWR::AVIRead::AVIRPCIDeviceForm::AVIRPCIDeviceForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("PCI Devices"));
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
	this->lblVendorName = ui->NewLabel(this->pnlDevices, CSTR("Vendor Name"));
	this->lblVendorName->SetRect(4, 28, 100, 23, false);
	this->txtVendorName = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtVendorName->SetRect(104, 28, 500, 23, false);
	this->txtVendorName->SetReadOnly(true);
	this->lblProductId = ui->NewLabel(this->pnlDevices, CSTR("ProductId"));
	this->lblProductId->SetRect(4, 52, 100, 23, false);
	this->txtProductId = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtProductId->SetRect(104, 52, 100, 23, false);
	this->txtProductId->SetReadOnly(true);
	this->lblDispName = ui->NewLabel(this->pnlDevices, CSTR("Disp Name"));
	this->lblDispName->SetRect(4, 76, 100, 23, false);
	this->txtDispName = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtDispName->SetRect(104, 76, 500, 23, false);
	this->txtDispName->SetReadOnly(true);
	this->lblDBName = ui->NewLabel(this->pnlDevices, CSTR("DB Name"));
	this->lblDBName->SetRect(4, 100, 100, 23, false);
	this->txtDBName = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtDBName->SetRect(104, 100, 500, 23, false);
	this->txtDBName->SetReadOnly(true);

	this->OnMonitorChanged();

	IO::PCIInfo *pci;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[32];
	UTF8Char *sptr;

	IO::PCIInfo::GetPCIList(this->pciList);
	Data::Sort::ArtificialQuickSortFunc<IO::PCIInfo*>::Sort(this->pciList, ItemCompare);
	
	i = 0;
	j = this->pciList.GetCount();
	while (i < j)
	{
		pci = this->pciList.GetItem(i);
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
	UOSInt i = this->pciList.GetCount();
	while (i-- > 0)
	{
		pci = this->pciList.GetItem(i);
		DEL_CLASS(pci);
	}
}

void SSWR::AVIRead::AVIRPCIDeviceForm::OnMonitorChanged()
{
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
}
