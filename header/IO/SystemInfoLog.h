#ifndef _SM_IO_SYSTEMINFOLOG
#define _SM_IO_SYSTEMINFOLOG
#include "Data/ArrayList.h"
#include "IO/ParsedObject.h"

namespace IO
{
	class SystemInfoLog : public IO::ParsedObject
	{
	public:
		struct ServerRole
		{
			const UTF8Char *name;
			const UTF8Char *data;
		};

		struct DeviceInfo
		{
			Text::String *desc;
			Text::String *hwId;
			Text::String *service;
			Text::String *driver;
		};

		struct DriverInfo
		{
			Text::String *fileName;
			UInt64 fileSize;
			Text::String *creationDate;
			Text::String *version;
			Text::String *manufacturer;
			Text::String *productName;
			Text::String *group;
			UInt32 altitude;
		};
	private:
		Text::String *osName;
		Text::String *osVer;
		UInt32 osLocale;
		UInt32 architecture;
		UInt32 productType;
		Data::ArrayList<ServerRole*> *serverRoles;
		Data::ArrayList<DeviceInfo*> *deviceInfos;
		Data::ArrayList<DriverInfo*> *driverInfos;

		void FreeServerRole(ServerRole *serverRole);
		void FreeDeviceInfo(DeviceInfo *deviceInfo);
		void FreeDriverInfo(DriverInfo *driverInfo);
	public:
		SystemInfoLog(Text::CString sourceName);
		virtual ~SystemInfoLog();

		virtual IO::ParserType GetParserType() const;

		void SetOSName(Text::CString osName);
		Text::String *GetOSName() const;
		void SetOSVer(Text::CString osVer);
		Text::String *GetOSVer() const;
		void SetOSLocale(UInt32 osLocale);
		UInt32 GetOSLocale() const;
		void SetArchitecture(UInt32 architecture);
		UInt32 GetArchitecture() const;
		void SetProductType(UInt32 productType);
		UInt32 GetProductType() const;
		void AddServerRole(const UTF8Char *name, const UTF8Char *data);
		const Data::ArrayList<ServerRole*> *GetServerRoles() const;
		void AddDeviceInfo(const UTF8Char *desc, const UTF8Char *hwId, const UTF8Char *service, const UTF8Char *driver);
		const Data::ArrayList<DeviceInfo*> *GetDeviceInfos() const;
		void AddDriverInfo(const UTF8Char *fileName, UInt64 fileSize, const UTF8Char *creationDate, const UTF8Char *version, const UTF8Char *manufacturer, const UTF8Char *productName, const UTF8Char *group, UInt32 altitude);
		const Data::ArrayList<DriverInfo*> *GetDriverInfos() const;
	};
}

#endif
