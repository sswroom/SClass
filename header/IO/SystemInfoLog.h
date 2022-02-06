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
			const UTF8Char *hwId;
			const UTF8Char *service;
			const UTF8Char *driver;
		};

		struct DriverInfo
		{
			Text::String *fileName;
			UInt64 fileSize;
			const UTF8Char *creationDate;
			const UTF8Char *version;
			const UTF8Char *manufacturer;
			const UTF8Char *productName;
			const UTF8Char *group;
			UInt32 altitude;
		};
	private:
		const UTF8Char *osName;
		const UTF8Char *osVer;
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
		SystemInfoLog(const UTF8Char *sourceName);
		virtual ~SystemInfoLog();

		virtual IO::ParserType GetParserType();

		void SetOSName(const UTF8Char *osName);
		const UTF8Char *GetOSName();
		void SetOSVer(const UTF8Char *osVer);
		const UTF8Char *GetOSVer();
		void SetOSLocale(UInt32 osLocale);
		UInt32 GetOSLocale();
		void SetArchitecture(UInt32 architecture);
		UInt32 GetArchitecture();
		void SetProductType(UInt32 productType);
		UInt32 GetProductType();
		void AddServerRole(const UTF8Char *name, const UTF8Char *data);
		Data::ArrayList<ServerRole*> *GetServerRoles();
		void AddDeviceInfo(const UTF8Char *desc, const UTF8Char *hwId, const UTF8Char *service, const UTF8Char *driver);
		Data::ArrayList<DeviceInfo*> *GetDeviceInfos();
		void AddDriverInfo(const UTF8Char *fileName, UInt64 fileSize, const UTF8Char *creationDate, const UTF8Char *version, const UTF8Char *manufacturer, const UTF8Char *productName, const UTF8Char *group, UInt32 altitude);
		Data::ArrayList<DriverInfo*> *GetDriverInfos();
	};
}

#endif
