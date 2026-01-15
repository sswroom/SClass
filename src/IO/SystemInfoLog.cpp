#include "Stdafx.h"
#include "IO/SystemInfoLog.h"
#include "Text/MyString.h"

void __stdcall IO::SystemInfoLog::FreeServerRole(NN<ServerRole> serverRole)
{
	SDEL_TEXT(serverRole->name);
	SDEL_TEXT(serverRole->data);
	MemFreeNN(serverRole);
}

void __stdcall IO::SystemInfoLog::FreeDeviceInfo(NN<DeviceInfo> deviceInfo)
{
	deviceInfo->desc->Release();
	deviceInfo->hwId->Release();
	OPTSTR_DEL(deviceInfo->service);
	OPTSTR_DEL(deviceInfo->driver);
	MemFreeNN(deviceInfo);
}

void __stdcall IO::SystemInfoLog::FreeDriverInfo(NN<DriverInfo> driverInfo)
{
	driverInfo->fileName->Release();
	OPTSTR_DEL(driverInfo->creationDate);
	OPTSTR_DEL(driverInfo->version);
	OPTSTR_DEL(driverInfo->manufacturer);
	OPTSTR_DEL(driverInfo->productName);
	OPTSTR_DEL(driverInfo->group);
	MemFreeNN(driverInfo);
}

IO::SystemInfoLog::SystemInfoLog(Text::CStringNN sourceName) : IO::ParsedObject(sourceName)
{
	this->osName = nullptr;
	this->osVer = nullptr;
	this->osLocale = 0;
	this->architecture = 0;
	this->productType = 0;
}

IO::SystemInfoLog::~SystemInfoLog()
{
	this->serverRoles.FreeAll(FreeServerRole);
	this->deviceInfos.FreeAll(FreeDeviceInfo);
	this->driverInfos.FreeAll(FreeDriverInfo);
	OPTSTR_DEL(this->osName);
	OPTSTR_DEL(this->osVer);
}

IO::ParserType IO::SystemInfoLog::GetParserType() const
{
	return ParserType::SystemInfoLog;
}

void IO::SystemInfoLog::SetOSName(Text::CString osName)
{
	OPTSTR_DEL(this->osName);
	this->osName = Text::String::NewOrNull(osName);
}

Optional<Text::String> IO::SystemInfoLog::GetOSName() const
{
	return this->osName;
}

void IO::SystemInfoLog::SetOSVer(Text::CString osVer)
{
	OPTSTR_DEL(this->osVer);
	this->osVer = Text::String::NewOrNull(osVer);
}

Optional<Text::String> IO::SystemInfoLog::GetOSVer() const
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

void IO::SystemInfoLog::AddServerRole(UnsafeArray<const UTF8Char> name, UnsafeArray<const UTF8Char> data)
{
	NN<ServerRole> role = MemAllocNN(ServerRole);
	role->name = Text::StrCopyNew(name);
	role->data = Text::StrCopyNew(data);
	this->serverRoles.Add(role);
}

NN<const Data::ArrayListNN<IO::SystemInfoLog::ServerRole>> IO::SystemInfoLog::GetServerRoles() const
{
	return this->serverRoles;
}

void IO::SystemInfoLog::AddDeviceInfo(UnsafeArray<const UTF8Char> desc, UnsafeArray<const UTF8Char> hwId, UnsafeArrayOpt<const UTF8Char> service, UnsafeArrayOpt<const UTF8Char> driver)
{
	NN<DeviceInfo> dev = MemAllocNN(DeviceInfo);
	dev->desc = Text::String::NewNotNullSlow(desc);
	dev->hwId = Text::String::NewNotNullSlow(hwId);
	dev->service = Text::String::NewOrNullSlow(service);
	dev->driver = Text::String::NewOrNullSlow(driver);
	this->deviceInfos.Add(dev);
}

NN<const Data::ArrayListNN<IO::SystemInfoLog::DeviceInfo>> IO::SystemInfoLog::GetDeviceInfos() const
{
	return this->deviceInfos;
}

void IO::SystemInfoLog::AddDriverInfo(UnsafeArray<const UTF8Char> fileName, UInt64 fileSize, UnsafeArrayOpt<const UTF8Char> creationDate, UnsafeArrayOpt<const UTF8Char> version, UnsafeArrayOpt<const UTF8Char> manufacturer, UnsafeArrayOpt<const UTF8Char> productName, UnsafeArrayOpt<const UTF8Char> group, UInt32 altitude)
{
	NN<DriverInfo> driver = MemAllocNN(DriverInfo);
	driver->fileName = Text::String::NewNotNullSlow(fileName);
	driver->fileSize = fileSize;
	driver->creationDate = Text::String::NewOrNullSlow(creationDate);
	driver->version = Text::String::NewOrNullSlow(version);
	driver->manufacturer = Text::String::NewOrNullSlow(manufacturer);
	driver->productName = Text::String::NewOrNullSlow(productName);
	driver->group = Text::String::NewOrNullSlow(group);
	driver->altitude = altitude;
	this->driverInfos.Add(driver);
}

NN<const Data::ArrayListNN<IO::SystemInfoLog::DriverInfo>> IO::SystemInfoLog::GetDriverInfos() const
{
	return this->driverInfos;
}
