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
	public:
		HuaweiGSMModemController(IO::ATCommandChannel *channel, Bool needRelease);
		virtual ~HuaweiGSMModemController();

		//Huawei Proprietary: Mobile Termination Control and Status Interface
		UTF8Char *HuaweiGetICCID(UTF8Char *iccid); //AT^ICCID
		Bool HuaweiGetCardMode(SIMCardType *simType); //AT^CARDMODE
		Bool HuaweiGetSysInfoEx(ServiceStatus *srvStatus, ServiceDomain *srvDomain, Bool *roamStatus, SIMState *simState, Bool *lockState, SysMode *sysMode, SubMode *subMode); //AT^SYSINFOEX
		Bool HuaweiGetSignalStrength(SignalStrengthInfo *csq); //AT^HCSQ
		Bool HuaweiGetDHCP(UInt32 *clientIP, UInt32 *netmask, UInt32 *gateway, UInt32 *dhcp, UInt32 *priDNS, UInt32 *secDNS, UInt64 *maxRXbps, UInt64 *maxTXbps); //AT^DHCP

		static UTF8Char *RSSIGetName(UTF8Char *sbuff, UInt32 rssi);
		static Double RSSIGetdBm(UInt32 rssi);
		static UTF8Char *RSCPGetName(UTF8Char *sbuff, UInt32 rscp);
		static Double RSCPGetdBm(UInt32 rscp);
		static UTF8Char *ECIOGetName(UTF8Char *sbuff, UInt32 ecio);
		static Double ECIOGetdBm(UInt32 ecio);
		static UTF8Char *RSRPGetName(UTF8Char *sbuff, UInt32 rsrp);
		static Double RSRPGetdBm(UInt32 rsrp);
		static UTF8Char *SINRGetName(UTF8Char *sbuff, UInt32 sinr);
		static Double SINRGetdBm(UInt32 sinr);
		static UTF8Char *RSRQGetName(UTF8Char *sbuff, UInt32 rscq);
		static Double RSRQGetdBm(UInt32 rscq);
		static Text::CString SIMCardTypeGetName(SIMCardType simType);
		static Text::CString ServiceStatusGetName(ServiceStatus srvStatus);
		static Text::CString ServiceDomainGetName(ServiceDomain srvDomain);
		static Text::CString SIMStateGetName(SIMState simState);
		static Text::CString SysModeGetName(SysMode sysMode);
		static Text::CString SubModeGetName(SubMode submode);
	};
}
#endif