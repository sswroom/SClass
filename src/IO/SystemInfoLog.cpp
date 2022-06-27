#include "Stdafx.h"
#include "IO/SystemInfoLog.h"
#include "Text/MyString.h"

void IO::SystemInfoLog::FreeServerRole(ServerRole *serverRole)
{
	SDEL_TEXT(serverRole->name);
	SDEL_TEXT(serverRole->data);
	MemFree(serverRole);
}

void IO::SystemInfoLog::FreeDeviceInfo(DeviceInfo *deviceInfo)
{
	SDEL_STRING(deviceInfo->desc);
	SDEL_STRING(deviceInfo->hwId);
	SDEL_STRING(deviceInfo->service);
	SDEL_STRING(deviceInfo->driver);
	MemFree(deviceInfo);
}

void IO::SystemInfoLog::FreeDriverInfo(DriverInfo *driverInfo)
{
	SDEL_STRING(driverInfo->fileName);
	SDEL_STRING(driverInfo->creationDate);
	SDEL_STRING(driverInfo->version);
	SDEL_STRING(driverInfo->manufacturer);
	SDEL_STRING(driverInfo->productName);
	SDEL_STRING(driverInfo->group);
	MemFree(driverInfo);
}

IO::SystemInfoLog::SystemInfoLog(Text::CString sourceName) : IO::ParsedObject(sourceName)
{
	this->osName = 0;
	this->osVer = 0;
	this->osLocale = 0;
	this->architecture = 0;
	this->productType = 0;
	NEW_CLASS(this->serverRoles, Data::ArrayList<ServerRole*>());
	NEW_CLASS(this->deviceInfos, Data::ArrayList<DeviceInfo*>());
	NEW_CLASS(this->driverInfos, Data::ArrayList<DriverInfo*>());
}

IO::SystemInfoLog::~SystemInfoLog()
{
	LIST_FREE_FUNC(this->serverRoles, FreeServerRole);
	DEL_CLASS(this->serverRoles);
	LIST_FREE_FUNC(this->deviceInfos, FreeDeviceInfo);
	DEL_CLASS(this->deviceInfos);
	LIST_FREE_FUNC(this->driverInfos, FreeDriverInfo);
	DEL_CLASS(this->driverInfos);
	SDEL_STRING(this->osName);
	SDEL_STRING(this->osVer);
}

IO::ParserType IO::SystemInfoLog::GetParserType() const
{
	return ParserType::SystemInfoLog;
}

void IO::SystemInfoLog::SetOSName(Text::CString osName)
{
	SDEL_STRING(this->osName);
	this->osName = Text::String::NewOrNull(osName);
}

Text::String *IO::SystemInfoLog::GetOSName() const
{
	return this->osName;
}

void IO::SystemInfoLog::SetOSVer(Text::CString osVer)
{
	SDEL_STRING(this->osVer);
	this->osVer = Text::String::NewOrNull(osVer);
}

Text::String *IO::SystemInfoLog::GetOSVer() const
{
	return this->osVer;
}

void IO::SystemInfoLog::SetOSLocale(UInt32 osLocale)
{
	this->osLocale = osLocale;
}

UInt32 IO::SystemInfoLog::GetOSLocale() const
{
	return this->osLocale;
}

void IO::SystemInfoLog::SetArchitecture(UInt32 architecture)
{
	this->architecture = architecture;
}

UInt32 IO::SystemInfoLog::GetArchitecture() const
{
	return this->architecture;
}

void IO::SystemInfoLog::SetProductType(UInt32 productType)
{
	this->productType = productType;
}

UInt32 IO::SystemInfoLog::GetProductType() const
{
	return this->productType;
}

void IO::SystemInfoLog::AddServerRole(const UTF8Char *name, const UTF8Char *data)
{
	ServerRole *role = MemAlloc(ServerRole, 1);
	role->name = SCOPY_TEXT(name);
	role->data = SCOPY_TEXT(data);
	this->serverRoles->Add(role);
}

const Data::ArrayList<IO::SystemInfoLog::ServerRole*> *IO::SystemInfoLog::GetServerRoles() const
{
	return this->serverRoles;
}

void IO::SystemInfoLog::AddDeviceInfo(const UTF8Char *desc, const UTF8Char *hwId, const UTF8Char *service, const UTF8Char *driver)
{
	DeviceInfo *dev = MemAlloc(DeviceInfo, 1);
	dev->desc = Text::String::NewOrNullSlow(desc);
	dev->hwId = Text::String::NewOrNullSlow(hwId);
	dev->service = Text::String::NewOrNullSlow(service);
	dev->driver = Text::String::NewOrNullSlow(driver);
	this->deviceInfos->Add(dev);
}

const Data::ArrayList<IO::SystemInfoLog::DeviceInfo*> *IO::SystemInfoLog::GetDeviceInfos() const
{
	return this->deviceInfos;
}

void IO::SystemInfoLog::AddDriverInfo(const UTF8Char *fileName, UInt64 fileSize, const UTF8Char *creationDate, const UTF8Char *version, const UTF8Char *manufacturer, const UTF8Char *productName, const UTF8Char *group, UInt32 altitude)
{
	DriverInfo *driver = MemAlloc(DriverInfo, 1);
	driver->fileName = Text::String::NewOrNullSlow(fileName);
	driver->fileSize = fileSize;
	driver->creationDate = Text::String::NewOrNullSlow(creationDate);
	driver->version = Text::String::NewOrNullSlow(version);
	driver->manufacturer = Text::String::NewOrNullSlow(manufacturer);
	driver->productName = Text::String::NewOrNullSlow(productName);
	driver->group = Text::String::NewOrNullSlow(group);
	driver->altitude = altitude;
	this->driverInfos->Add(driver);
}

const Data::ArrayList<IO::SystemInfoLog::DriverInfo*> *IO::SystemInfoLog::GetDriverInfos() const
{
	return this->driverInfos;
}
