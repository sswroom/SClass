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
	SDEL_TEXT(deviceInfo->hwId);
	SDEL_TEXT(deviceInfo->service);
	SDEL_TEXT(deviceInfo->driver);
	MemFree(deviceInfo);
}

void IO::SystemInfoLog::FreeDriverInfo(DriverInfo *driverInfo)
{
	SDEL_STRING(driverInfo->fileName);
	SDEL_TEXT(driverInfo->creationDate);
	SDEL_TEXT(driverInfo->version);
	SDEL_TEXT(driverInfo->manufacturer);
	SDEL_TEXT(driverInfo->productName);
	SDEL_TEXT(driverInfo->group);
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
	SDEL_TEXT(this->osName);
	SDEL_TEXT(this->osVer);
}

IO::ParserType IO::SystemInfoLog::GetParserType()
{
	return ParserType::SystemInfoLog;
}

void IO::SystemInfoLog::SetOSName(const UTF8Char *osName)
{
	SDEL_TEXT(this->osName);
	this->osName = SCOPY_TEXT(osName);
}

const UTF8Char *IO::SystemInfoLog::GetOSName()
{
	return this->osName;
}

void IO::SystemInfoLog::SetOSVer(const UTF8Char *osVer)
{
	SDEL_TEXT(this->osVer);
	this->osVer = SCOPY_TEXT(osVer);
}

const UTF8Char *IO::SystemInfoLog::GetOSVer()
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
	dev->hwId = SCOPY_TEXT(hwId);
	dev->service = SCOPY_TEXT(service);
	dev->driver = SCOPY_TEXT(driver);
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
	driver->creationDate = SCOPY_TEXT(creationDate);
	driver->version = SCOPY_TEXT(version);
	driver->manufacturer = SCOPY_TEXT(manufacturer);
	driver->productName = SCOPY_TEXT(productName);
	driver->group = SCOPY_TEXT(group);
	driver->altitude = altitude;
	this->driverInfos->Add(driver);
}

Data::ArrayList<IO::SystemInfoLog::DriverInfo*> *IO::SystemInfoLog::GetDriverInfos()
{
	return this->driverInfos;
}
