#ifndef _SM_IO_SYSTEMINFOLOG
#define _SM_IO_SYSTEMINFOLOG
#include "Data/ArrayListNN.h"
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
			NN<Text::String> desc;
			NN<Text::String> hwId;
			Optional<Text::String> service;
			Optional<Text::String> driver;
		};

		struct DriverInfo
		{
			NN<Text::String> fileName;
			UInt64 fileSize;
			Optional<Text::String> creationDate;
			Optional<Text::String> version;
			Optional<Text::String> manufacturer;
			Optional<Text::String> productName;
			Optional<Text::String> group;
			UInt32 altitude;
		};
	private:
		Optional<Text::String> osName;
		Optional<Text::String> osVer;
		UInt32 osLocale;
		UInt32 architecture;
		UInt32 productType;
		Data::ArrayListNN<ServerRole> serverRoles;
		Data::ArrayListNN<DeviceInfo> deviceInfos;
		Data::ArrayListNN<DriverInfo> driverInfos;

		static void FreeServerRole(NN<ServerRole> serverRole);
		static void FreeDeviceInfo(NN<DeviceInfo> deviceInfo);
		static void FreeDriverInfo(NN<DriverInfo> driverInfo);
	public:
		SystemInfoLog(Text::CStringNN sourceName);
		virtual ~SystemInfoLog();

		virtual IO::ParserType GetParserType() const;

		void SetOSName(Text::CString osName);
		Optional<Text::String> GetOSName() const;
		void SetOSVer(Text::CString osVer);
		Optional<Text::String> GetOSVer() const;
		void SetOSLocale(UInt32 osLocale);
		UInt32 GetOSLocale() const;
		void SetArchitecture(UInt32 architecture);
		UInt32 GetArchitecture() const;
		void SetProductType(UInt32 productType);
		UInt32 GetProductType() const;
		void AddServerRole(const UTF8Char *name, const UTF8Char *data);
		NN<const Data::ArrayListNN<ServerRole>> GetServerRoles() const;
		void AddDeviceInfo(const UTF8Char *desc, const UTF8Char *hwId, const UTF8Char *service, const UTF8Char *driver);
		NN<const Data::ArrayListNN<DeviceInfo>> GetDeviceInfos() const;
		void AddDriverInfo(const UTF8Char *fileName, UInt64 fileSize, const UTF8Char *creationDate, const UTF8Char *version, const UTF8Char *manufacturer, const UTF8Char *productName, const UTF8Char *group, UInt32 altitude);
		NN<const Data::ArrayListNN<DriverInfo>> GetDriverInfos() const;
	};
}

#endif
