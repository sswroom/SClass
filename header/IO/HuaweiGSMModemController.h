#ifndef _SM_IO_HUAWEIGSMMODEMCONTROLLER
#define _SM_IO_HUAWEIGSMMODEMCONTROLLER
#include "IO/GSMModemController.h"

namespace IO
{
	class HuaweiGSMModemController : public GSMModemController
	{
	public:
		enum class SIMCardType
		{
			NoCard,
			SIMCard,
			USIMCard,
			CSIMCard_Dual,
			SIMCard_Dual
		};

		enum class ServiceStatus
		{
			NoServices,
			RestrictedServices,
			ValidServices,
			RestrictedRegionService,
			PowerSavingState
		};

		enum class ServiceDomain
		{
			NoServices,
			CSServiceOnly,
			PSServiceOnly,
			PS_CSServices,
			NotRegistered,
			CDMA = 255
		};

		enum class SIMState
		{
			InvalidSIM,
			ValidSIM,
			InvalidSIM_CS,
			InvalidSIM_PS,
			InvalidSIM_PS_CS,
			ROMSIM = 240,
			NoSIM = 255
		};

		enum class SysMode
		{
			NoService,
			GSM,
			CDMA,
			WCDMA,
			TD_SCDMA,
			WIMAX,
			LTE
		};

		enum class SubMode
		{
			NoService,
			GSM,
			GPRS,
			EDGE,
			IS95A = 21,
			IS95B,
			CDMA2000,
			EVDO_Rel0,
			EVDO_RelA,
			EVDO_RelB,
			HybridCDMA2000,
			HybridEVDO_Rel0,
			HybridEVDO_RelA,
			HybridEVDO_RelB,
			EHRPD_Rel0,
			EHRPD_RelA,
			EHRPD_RelB,
			HybridEHRPD_Rel0,
			HybridEHRPD_RelA,
			HybridEHRPD_RelB,
			WCDMA = 41,
			HSDPA,
			HSUPA,
			HSPA,
			HSPA_,
			DC_HSPA_,
			TD_SCDMA = 61,
			_HSDPA,
			_HSUPA,
			_HSPA,
			_HSPA_,
			_802_16e = 81,
			LTE = 101
		};

		struct SignalStrengthInfo
		{
			SysMode sysmode;
			union
			{
				UInt32 value1;
				UInt32 gsmRSSI;
				UInt32 wcdmaRSSI;
				UInt32 tdscdmaRSSI;
				UInt32 lteRSSI;
			};
			union
			{
				UInt32 value2;
				UInt32 wcdmaRSCP;
				UInt32 tdscdmaRSCP;
				UInt32 lteRSRP;
			};
			union
			{
				UInt32 value3;
				UInt32 wcdmaECIO;
				UInt32 tdscdmaECIO;
				UInt32 lteSINR;
			};
			union
			{
				UInt32 value4;
				UInt32 lteRSRQ;
			};
		};

		struct VersionInfo
		{
			Optional<Text::String> biosDateTime;
			Optional<Text::String> exts;
			Optional<Text::String> extd;
			Optional<Text::String> exth;
			Optional<Text::String> extu;
			Optional<Text::String> cfg;
			Optional<Text::String> ini;
		};
	public:
		HuaweiGSMModemController(NN<IO::ATCommandChannel> channel, Bool needRelease);
		virtual ~HuaweiGSMModemController();

		//Huawei Proprietary: Mobile Termination Control and Status Interface
		UnsafeArrayOpt<UTF8Char> HuaweiGetICCID(UnsafeArray<UTF8Char> iccid); //AT^ICCID
		Bool HuaweiGetCardMode(OutParam<SIMCardType> simType); //AT^CARDMODE
		Bool HuaweiGetSysInfoEx(OutParam<ServiceStatus> srvStatus, OutParam<ServiceDomain> srvDomain, OutParam<Bool> roamStatus, OutParam<SIMState> simState, OutParam<Bool> lockState, OutParam<SysMode> sysMode, OutParam<SubMode> subMode); //AT^SYSINFOEX
		Bool HuaweiGetSignalStrength(NN<SignalStrengthInfo> csq); //AT^HCSQ
		Bool HuaweiGetDHCP(OutParam<UInt32> clientIP, OutParam<UInt32> netmask, OutParam<UInt32> gateway, OutParam<UInt32> dhcp, OutParam<UInt32> priDNS, OutParam<UInt32> secDNS, OutParam<UInt64> maxRXbps, OutParam<UInt64> maxTXbps); //AT^DHCP
		Bool HuaweiGetVersion(NN<VersionInfo> version);

		static UnsafeArray<UTF8Char> RSSIGetName(UnsafeArray<UTF8Char> sbuff, UInt32 rssi);
		static Double RSSIGetdBm(UInt32 rssi);
		static UnsafeArray<UTF8Char> RSCPGetName(UnsafeArray<UTF8Char> sbuff, UInt32 rscp);
		static Double RSCPGetdBm(UInt32 rscp);
		static UnsafeArray<UTF8Char> ECIOGetName(UnsafeArray<UTF8Char> sbuff, UInt32 ecio);
		static Double ECIOGetdBm(UInt32 ecio);
		static UnsafeArray<UTF8Char> RSRPGetName(UnsafeArray<UTF8Char> sbuff, UInt32 rsrp);
		static Double RSRPGetdBm(UInt32 rsrp);
		static UnsafeArray<UTF8Char> SINRGetName(UnsafeArray<UTF8Char> sbuff, UInt32 sinr);
		static Double SINRGetdBm(UInt32 sinr);
		static UnsafeArray<UTF8Char> RSRQGetName(UnsafeArray<UTF8Char> sbuff, UInt32 rscq);
		static Double RSRQGetdBm(UInt32 rscq);
		static Text::CStringNN SIMCardTypeGetName(SIMCardType simType);
		static Text::CStringNN ServiceStatusGetName(ServiceStatus srvStatus);
		static Text::CStringNN ServiceDomainGetName(ServiceDomain srvDomain);
		static Text::CStringNN SIMStateGetName(SIMState simState);
		static Text::CStringNN SysModeGetName(SysMode sysMode);
		static Text::CStringNN SubModeGetName(SubMode submode);
		static void FreeVersionInfo(NN<VersionInfo> version);
	};
}
#endif
