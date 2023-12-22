#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRSystemInfoLogForm.h"

SSWR::AVIRead::AVIRSystemInfoLogForm::AVIRSystemInfoLogForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::SystemInfoLog *sysInfo) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("System Info Log"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->sysInfo = sysInfo;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpSystem = this->tcMain->AddTabPage(CSTR("System"));
	this->lblOSName = ui->NewLabel(this->tpSystem, CSTR("OS Name"));
	this->lblOSName->SetRect(4, 4, 100, 23, false);
	this->txtOSName = ui->NewTextBox(this->tpSystem, CSTR(""));
	this->txtOSName->SetRect(104, 4, 400, 23, false);
	this->txtOSName->SetReadOnly(true);
	this->lblOSVer = ui->NewLabel(this->tpSystem, CSTR("OS Version"));
	this->lblOSVer->SetRect(4, 28, 100, 23, false);
	this->txtOSVer = ui->NewTextBox(this->tpSystem, CSTR(""));
	this->txtOSVer->SetRect(104, 28, 400, 23, false);
	this->txtOSVer->SetReadOnly(true);
	this->lblOSLocale = ui->NewLabel(this->tpSystem, CSTR("OS Locale"));
	this->lblOSLocale->SetRect(4, 52, 100, 23, false);
	this->txtOSLocale = ui->NewTextBox(this->tpSystem, CSTR(""));
	this->txtOSLocale->SetRect(104, 52, 400, 23, false);
	this->txtOSLocale->SetReadOnly(true);
	this->lblArchitecture = ui->NewLabel(this->tpSystem, CSTR("Architecture"));
	this->lblArchitecture->SetRect(4, 76, 100, 23, false);
	this->txtArchitecture = ui->NewTextBox(this->tpSystem, CSTR(""));
	this->txtArchitecture->SetRect(104, 76, 400, 23, false);
	this->txtArchitecture->SetReadOnly(true);
	this->lblProductType = ui->NewLabel(this->tpSystem, CSTR("Product Type"));
	this->lblProductType->SetRect(4, 100, 100, 23, false);
	this->txtProductType = ui->NewTextBox(this->tpSystem, CSTR(""));
	this->txtProductType->SetRect(104, 100, 400, 23, false);
	this->txtProductType->SetReadOnly(true);

	this->tpDevices = this->tcMain->AddTabPage(CSTR("Devices"));
	this->lvDevices = ui->NewListView(this->tpDevices, UI::ListViewStyle::Table, 4);
	this->lvDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevices->SetShowGrid(true);
	this->lvDevices->SetFullRowSelect(true);
	this->lvDevices->AddColumn(CSTR("Description"), 300);
	this->lvDevices->AddColumn(CSTR("Hardware ID"), 200);
	this->lvDevices->AddColumn(CSTR("Service"), 100);
	this->lvDevices->AddColumn(CSTR("Driver"), 100);

	this->tpDrivers = this->tcMain->AddTabPage(CSTR("Devices"));
	this->lvDrivers = ui->NewListView(this->tpDrivers, UI::ListViewStyle::Table, 8);
	this->lvDrivers->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDrivers->SetShowGrid(true);
	this->lvDrivers->SetFullRowSelect(true);
	this->lvDrivers->AddColumn(CSTR("File Name"), 120);
	this->lvDrivers->AddColumn(CSTR("File Size"), 80);
	this->lvDrivers->AddColumn(CSTR("Creation Date"), 140);
	this->lvDrivers->AddColumn(CSTR("Version"), 100);
	this->lvDrivers->AddColumn(CSTR("Manufacturer"), 200);
	this->lvDrivers->AddColumn(CSTR("ProductName"), 300);
	this->lvDrivers->AddColumn(CSTR("Group"), 130);
	this->lvDrivers->AddColumn(CSTR("Altitude"), 100);

	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<Text::String> s;
	if (this->sysInfo->GetOSName().SetTo(s))
	{
		this->txtOSName->SetText(s->ToCString());
	}
	if (this->sysInfo->GetOSVer().SetTo(s))
	{
		this->txtOSVer->SetText(s->ToCString());
	}
	if (this->sysInfo->GetOSLocale())
	{
		sptr = Text::StrUInt32(sbuff, this->sysInfo->GetOSLocale());
		this->txtOSLocale->SetText(CSTRP(sbuff, sptr));
	}
	if (this->sysInfo->GetArchitecture())
	{
		sptr = Text::StrUInt32(sbuff, this->sysInfo->GetArchitecture());
		this->txtArchitecture->SetText(CSTRP(sbuff, sptr));
	}
	if (this->sysInfo->GetProductType())
	{
		sptr = Text::StrUInt32(sbuff, this->sysInfo->GetProductType());
		this->txtProductType->SetText(CSTRP(sbuff, sptr));
	}

	NotNullPtr<const Data::ArrayList<IO::SystemInfoLog::DeviceInfo*>> devList = this->sysInfo->GetDeviceInfos();
	IO::SystemInfoLog::DeviceInfo *dev;
	UOSInt i = 0;
	UOSInt j = devList->GetCount();
	while (i < j)
	{
		dev = devList->GetItem(i);
		this->lvDevices->AddItem(dev->desc, dev);
		this->lvDevices->SetSubItem(i, 1, dev->hwId);
		if (dev->service.SetTo(s))
		{
			this->lvDevices->SetSubItem(i, 2, s);
		}
		if (dev->driver.SetTo(s))
		{
			this->lvDevices->SetSubItem(i, 3, s);
		}
		i++;
	}

	NotNullPtr<const Data::ArrayList<IO::SystemInfoLog::DriverInfo*>> driverList = this->sysInfo->GetDriverInfos();
	IO::SystemInfoLog::DriverInfo *driver;
	i = 0;
	j = driverList->GetCount();
	while (i < j)
	{
		driver = driverList->GetItem(i);
		this->lvDrivers->AddItem(driver->fileName, driver);
		sptr = Text::StrUInt64(sbuff, driver->fileSize);
		this->lvDrivers->SetSubItem(i, 1, CSTRP(sbuff, sptr));

		if (driver->creationDate.SetTo(s))
		{
			this->lvDrivers->SetSubItem(i, 2, s);
		}
		if (driver->version.SetTo(s))
		{
			this->lvDrivers->SetSubItem(i, 3, s);
		}
		if (driver->manufacturer.SetTo(s))
		{
			this->lvDrivers->SetSubItem(i, 4, s);
		}
		if (driver->productName.SetTo(s))
		{
			this->lvDrivers->SetSubItem(i, 5, s);
		}
		if (driver->group.SetTo(s))
		{
			this->lvDrivers->SetSubItem(i, 6, s);
		}
		if (driver->altitude)
		{
			sptr = Text::StrUInt32(sbuff, driver->altitude);
			this->lvDrivers->SetSubItem(i, 7, CSTRP(sbuff, sptr));
		}
		i++;
	}
}

SSWR::AVIRead::AVIRSystemInfoLogForm::~AVIRSystemInfoLogForm()
{
	DEL_CLASS(this->sysInfo);
}

void SSWR::AVIRead::AVIRSystemInfoLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
