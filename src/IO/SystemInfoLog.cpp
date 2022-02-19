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

IO::SystemInfoLog::SystemInfoLog(const UTF8Char *sourceName) : IO::ParsedObject(sourceName)
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

IO::ParserType IO::SystemInfoLog::GetParserType()
{
	return ParserType::SystemInfoLog;
}

void IO::SystemInfoLog::SetOSName(const UTF8Char *osName)
{
	SDEL_STRING(this->osName);
	this->osName = Text::String::NewOrNull(osName);
}

Text::String *IO::SystemInfoLog::GetOSName()
{
	return this->osName;
}

void IO::SystemInfoLog::SetOSVer(const UTF8Char *osVer)
{
	SDEL_STRING(this->osVer);
	this->osVer = Text::String::NewOrNull(osVer);
}

Text::String *IO::SystemInfoLog::GetOSVer()
{
	return this->osVer;
}

void IO::SystemInfoLog::SetOSLocale(UInt32 osLocale)
{
	this->osLocale = osLocale;
}

UInt32 IO::SystemInfoLog::GetOSLocale()
{
	return this->osLocale;
}

void IO::SystemInfoLog::SetArchitecture(UInt32 architecture)
{
	this->architecture = architecture;
}

UInt32 IO::SystemInfoLog::GetArchitecture()
{
	return this->architecture;
}

void IO::SystemInfoLog::SetProductType(UInt32 productType)
{
	this->productType = productType;
}

UInt32 IO::SystemInfoLog::GetProductType()
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

Data::ArrayList<IO::SystemInfoLog::ServerRole*> *IO::SystemInfoLog::GetServerRoles()
{
	return this->serverRoles;
}

void IO::SystemInfoLog::AddDeviceInfo(const UTF8Char *desc, const UTF8Char *hwId, const UTF8Char *service, const UTF8Char *driver)
{
	DeviceInfo *dev = MemAlloc(DeviceInfo, 1);
	dev->desc = Text::String::NewOrNull(desc);
	dev->hwId = Text::String::NewOrNull(hwId);
	dev->service = Text::String::NewOrNull(service);
	dev->driver = Text::String::NewOrNull(driver);
	this->deviceInfos->Add(dev);
}

Data::ArrayList<IO::SystemInfoLog::DeviceInfo*> *IO::SystemInfoLog::GetDeviceInfos()
{
	return this->deviceInfos;
}

void IO::SystemInfoLog::AddDriverInfo(const UTF8Char *fileName, UInt64 fileSize, const UTF8Char *creationDate, const UTF8Char *version, const UTF8Char *manufacturer, const UTF8Char *productName, const UTF8Char *group, UInt32 altitude)
{
	DriverInfo *driver = MemAlloc(DriverInfo, 1);
	driver->fileName = Text::String::NewOrNull(fileName);
	driver->fileSize = fileSize;
	driver->creationDate = Text::String::NewOrNull(creationDate);
	driver->version = Text::String::NewOrNull(version);
	driver->manufacturer = Text::String::NewOrNull(manufacturer);
	driver->productName = Text::String::NewOrNull(productName);
	driver->group = Text::String::NewOrNull(group);
	driver->altitude = altitude;
	this->driverInfos->Add(driver);
}

Data::ArrayList<IO::SystemInfoLog::DriverInfo*> *IO::SystemInfoLog::GetDriverInfos()
{
	return this->driverInfos;
}
