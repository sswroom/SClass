#include "Stdafx.h"
#include "IO/HuaweiGSMModemController.h"

IO::HuaweiGSMModemController::HuaweiGSMModemController(NN<IO::ATCommandChannel> channel, Bool needRelease) : IO::GSMModemController(channel, needRelease)
{
}

IO::HuaweiGSMModemController::~HuaweiGSMModemController()
{

}

UTF8Char *IO::HuaweiGSMModemController::HuaweiGetICCID(UTF8Char *iccid)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, UTF8STRC("AT^ICCID?"), 3000);
	if (sptr == 0)
	{
		return 0;
	}
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("^ICCID: ")))
	{
		return Text::StrConcatC(iccid, &sbuff[8], (UOSInt)(sptr - &sbuff[8]));
	}
	return 0;
}

Bool IO::HuaweiGSMModemController::HuaweiGetCardMode(SIMCardType *simType)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, UTF8STRC("AT^CARDMODE"), 3000);
	if (sptr == 0)
	{
		*simType = SIMCardType::NoCard;
		return false;
	}
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("^CARDMODE:")))
	{
		sptr = &sbuff[10];
		while (*sptr == ' ')
			sptr++;
		*simType = (SIMCardType)Text::StrToInt32(sptr);
		return true;
	}
	*simType = SIMCardType::NoCard;
	return false;
}

Bool IO::HuaweiGSMModemController::HuaweiGetSysInfoEx(ServiceStatus *srvStatus, ServiceDomain *srvDomain, Bool *roamStatus, SIMState *simState, Bool *lockState, SysMode *sysMode, SubMode *subMode)
{
	Text::PString sarr[9];
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, UTF8STRC("AT^SYSINFOEX"), 3000);
	if (sptr == 0)
	{
		return false;
	}
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("^SYSINFOEX:")))
	{
		UTF8Char *sptr2 = &sbuff[11];
		while (*sptr2 == ' ')
			sptr2++;
		if (Text::StrSplitP(sarr, 9, Text::PString(sptr2, (UOSInt)(sptr - sptr2)), ',') == 9)
		{
			*srvStatus = (ServiceStatus)sarr[0].ToInt32();
			*srvDomain = (ServiceDomain)sarr[1].ToInt32();
			*roamStatus = sarr[2].ToInt32() != 0;
			*simState = (SIMState)sarr[3].ToInt32();
			*lockState = sarr[4].ToInt32() != 0;
			*sysMode = (SysMode)sarr[5].ToInt32();
			*subMode = (SubMode)sarr[7].ToInt32();
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

Bool IO::HuaweiGSMModemController::HuaweiGetSignalStrength(SignalStrengthInfo *csq)
{
	Text::PString sarr[9];
	UOSInt sarrCnt;
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, UTF8STRC("AT^HCSQ?"), 3000);
	if (sptr == 0)
	{
		return false;
	}
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("^HCSQ:")))
	{
		UTF8Char *sptr2 = &sbuff[6];
		while (*sptr2 == ' ')
			sptr2++;
		sarrCnt = Text::StrSplitP(sarr, 6, Text::PString(sptr2, (UOSInt)(sptr - sptr2)), ',');
		if (sarr[0].Equals(UTF8STRC("\"NOSERVICE\"")))
		{
			csq->sysmode = SysMode::NoService;
		}
		else if (sarr[0].Equals(UTF8STRC("\"GSM\"")))
		{
			csq->sysmode = SysMode::GSM;
		}
		else if (sarr[0].Equals(UTF8STRC("\"WCDMA\"")))
		{
			csq->sysmode = SysMode::WCDMA;
		}
		else if (sarr[0].Equals(UTF8STRC("\"TD-SCDMA\"")))
		{
			csq->sysmode = SysMode::TD_SCDMA;
		}
		else if (sarr[0].Equals(UTF8STRC("\"LTE\"")))
		{
			csq->sysmode = SysMode::LTE;
		}
		else if (sarr[0].Equals(UTF8STRC("\"CDMA\"")))
		{
			csq->sysmode = SysMode::CDMA;
		}
		else
		{
			csq->sysmode = SysMode::NoService;
		}
		if (sarrCnt >= 2)
		{
			csq->value1 = sarr[1].ToUInt32();
		}
		else
		{
			csq->value1 = 0;
		}
		if (sarrCnt >= 3)
		{
			csq->value2 = sarr[2].ToUInt32();
		}
		else
		{
			csq->value2 = 0;
		}
		if (sarrCnt >= 4)
		{
			csq->value3 = sarr[3].ToUInt32();
		}
		else
		{
			csq->value3 = 0;
		}
		if (sarrCnt >= 5)
		{
			csq->value4 = sarr[4].ToUInt32();
		}
		else
		{
			csq->value4 = 0;
		}
		return true;
	}
	return false;
}

Bool IO::HuaweiGSMModemController::HuaweiGetDHCP(OutParam<UInt32> clientIP, OutParam<UInt32> netmask, OutParam<UInt32> gateway, OutParam<UInt32> dhcp, OutParam<UInt32> priDNS, OutParam<UInt32> secDNS, OutParam<UInt64> maxRXbps, OutParam<UInt64> maxTXbps)
{
	Text::PString sarr[9];
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, UTF8STRC("AT^DHCP?"), 3000);
	if (sptr == 0)
	{
		return false;
	}
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("^DHCP:")))
	{
		if (Text::StrSplitTrimP(sarr, 9, Text::PString(&sbuff[6], (UOSInt)(sptr - &sbuff[6])), ',') == 8)
		{
			UInt32 lclientIP;
			UInt32 lnetmask;
			UInt32 lgateway;
			UInt32 ldhcp;
			UInt32 lpriDNS;
			UInt32 lsecDNS;
			if (!sarr[0].Hex2UInt32(lclientIP)) return false;
			if (!sarr[1].Hex2UInt32(lnetmask)) return false;
			if (!sarr[2].Hex2UInt32(lgateway)) return false;
			if (!sarr[3].Hex2UInt32(ldhcp)) return false;
			if (!sarr[4].Hex2UInt32(lpriDNS)) return false;
			if (!sarr[5].Hex2UInt32(lsecDNS)) return false;
			if (!sarr[6].ToUInt64(maxRXbps)) return false;
			if (!sarr[7].ToUInt64(maxTXbps)) return false;
#if IS_BYTEORDER_LE == 0
			lclientIP = BSWAPU32(lclientIP);
			lnetmask = BSWAPU32(lnetmask);
			lgateway = BSWAPU32(lgateway);
			ldhcp = BSWAPU32(ldhcp);
			lpriDNS = BSWAPU32(lpriDNS);
			lsecDNS = BSWAPU32(lsecDNS);
#endif
			clientIP.Set(lclientIP);
			netmask.Set(lnetmask);
			gateway.Set(lgateway);
			dhcp.Set(ldhcp);
			priDNS.Set(lpriDNS);
			secDNS.Set(lsecDNS);
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

UTF8Char *IO::HuaweiGSMModemController::RSSIGetName(UTF8Char *sbuff, UInt32 rssi)
{
	if (rssi == 0)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("<-120 dBm"));
	}
	if (rssi <= 96)
	{
		return Text::StrConcatC(Text::StrInt32(sbuff, -121 + (Int32)rssi), UTF8STRC(" dBm"));
	}
	return Text::StrConcatC(sbuff, UTF8STRC("Unknown"));
}

Double IO::HuaweiGSMModemController::RSSIGetdBm(UInt32 rssi)
{
	if (rssi <= 96)
	{
		return -121 + (Double)rssi;
	}
	return 0;
}

UTF8Char *IO::HuaweiGSMModemController::RSCPGetName(UTF8Char *sbuff, UInt32 rscp)
{
	if (rscp == 0)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("<-120 dBm"));
	}
	if (rscp <= 96)
	{
		return Text::StrConcatC(Text::StrInt32(sbuff, -121 + (Int32)rscp), UTF8STRC(" dBm"));
	}
	return Text::StrConcatC(sbuff, UTF8STRC("Unknown"));
}

Double IO::HuaweiGSMModemController::RSCPGetdBm(UInt32 rscp)
{
	if (rscp <= 96)
	{
		return -121 + (Double)rscp;
	}
	return 0;
}

UTF8Char *IO::HuaweiGSMModemController::ECIOGetName(UTF8Char *sbuff, UInt32 ecio)
{
	if (ecio == 0)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("<-32 dBm"));
	}
	if (ecio <= 65)
	{
		if (ecio & 1)
		{
			return Text::StrConcatC(Text::StrInt32(sbuff, -32 + (Int32)(ecio >> 1)), UTF8STRC(" dBm"));
		}
		else
		{
			return Text::StrConcatC(Text::StrInt32(sbuff, -32 + (Int32)(ecio >> 1)), UTF8STRC(".5 dBm"));
		}
	}
	return Text::StrConcatC(sbuff, UTF8STRC("Unknown"));
}

Double IO::HuaweiGSMModemController::ECIOGetdBm(UInt32 ecio)
{
	if (ecio <= 65)
	{
		return -32.5 + 0.5 * (Double)ecio;
	}
	return 0;
}

UTF8Char *IO::HuaweiGSMModemController::RSRPGetName(UTF8Char *sbuff, UInt32 rsrp)
{
	if (rsrp == 0)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("<-140 dBm"));
	}
	if (rsrp <= 97)
	{
		return Text::StrConcatC(Text::StrInt32(sbuff, -141 + (Int32)rsrp), UTF8STRC(" dBm"));
	}
	return Text::StrConcatC(sbuff, UTF8STRC("Unknown"));
}

Double IO::HuaweiGSMModemController::RSRPGetdBm(UInt32 rsrp)
{
	if (rsrp <= 97)
	{
		return -141 + (Double)rsrp;
	}
	return 0;
}

UTF8Char *IO::HuaweiGSMModemController::SINRGetName(UTF8Char *sbuff, UInt32 sinr)
{
	if (sinr == 0)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("<-20 dBm"));
	}
	if (sinr <= 251)
	{
		return Text::StrConcatC(Text::StrDouble(sbuff, -20.2 + (Double)sinr * 0.2), UTF8STRC(" dBm"));
	}
	return Text::StrConcatC(sbuff, UTF8STRC("Unknown"));
}

Double IO::HuaweiGSMModemController::SINRGetdBm(UInt32 sinr)
{
	if (sinr <= 251)
	{
		return -20.2 + (Double)sinr * 0.2;
	}
	return 0;
}

UTF8Char *IO::HuaweiGSMModemController::RSRQGetName(UTF8Char *sbuff, UInt32 rsrq)
{
	if (rsrq == 0)
	{
		return Text::StrConcatC(sbuff, UTF8STRC("<-19.5 dBm"));
	}
	if (rsrq <= 34)
	{
		if (rsrq & 1)
		{
			return Text::StrConcatC(Text::StrInt32(sbuff, -19 + (Int32)(rsrq >> 1)), UTF8STRC(".5 dBm"));
		}
		else
		{
			return Text::StrConcatC(Text::StrInt32(sbuff, -20 + (Int32)(rsrq >> 1)), UTF8STRC(" dBm"));
		}
	}
	return Text::StrConcatC(sbuff, UTF8STRC("Unknown"));
}

Double IO::HuaweiGSMModemController::RSRQGetdBm(UInt32 rsrq)
{
	if (rsrq <= 34)
	{
		return -20.0 + (Double)rsrq * 0.5;
	}
	return 0;
}

Text::CStringNN IO::HuaweiGSMModemController::SIMCardTypeGetName(SIMCardType simType)
{
	switch (simType)
	{
	case SIMCardType::NoCard:
		return CSTR("No card");
	case SIMCardType::SIMCard:
		return CSTR("SIM card");
	case SIMCardType::USIMCard:
		return CSTR("USIM card");
	case SIMCardType::CSIMCard_Dual:
		return CSTR("CSIM card (dual-mode card)");
	case SIMCardType::SIMCard_Dual:
		return CSTR("SIM card (dual-mode card)");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::HuaweiGSMModemController::ServiceStatusGetName(ServiceStatus srvStatus)
{
	switch (srvStatus)
	{
	case ServiceStatus::NoServices:
		return CSTR("No services");
	case ServiceStatus::RestrictedServices:
		return CSTR("Restricted services");
	case ServiceStatus::ValidServices:
		return CSTR("Valid services");
	case ServiceStatus::RestrictedRegionService:
		return CSTR("Restricted regional services");
	case ServiceStatus::PowerSavingState:
		return CSTR("Power saving or hibernate state");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::HuaweiGSMModemController::ServiceDomainGetName(ServiceDomain srvDomain)
{
	switch (srvDomain)
	{
	case ServiceDomain::NoServices:
		return CSTR("No services");
	case ServiceDomain::CSServiceOnly:
		return CSTR("CS service only");
	case ServiceDomain::PSServiceOnly:
		return CSTR("PS service only");
	case ServiceDomain::PS_CSServices:
		return CSTR("PS+CS services");
	case ServiceDomain::NotRegistered:
		return CSTR("Not registered to CS or PS; searching now");
	case ServiceDomain::CDMA:
		return CSTR("CDMA");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::HuaweiGSMModemController::SIMStateGetName(SIMState simState)
{
	switch (simState)
	{
	case SIMState::InvalidSIM:
		return CSTR("Invalid SIM card");
	case SIMState::ValidSIM:
		return CSTR("Valid SIM card");
	case SIMState::InvalidSIM_CS:
		return CSTR("Invalid SIM card in CS");
	case SIMState::InvalidSIM_PS:
		return CSTR("Invalid SIM card in PS");
	case SIMState::InvalidSIM_PS_CS:
		return CSTR("Invalid SIM card in PS and CS");
	case SIMState::ROMSIM:
		return CSTR("ROMSIM version");
	case SIMState::NoSIM:
		return CSTR("No SIM card is found");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::HuaweiGSMModemController::SysModeGetName(SysMode sysMode)
{
	switch (sysMode)
	{
	case SysMode::NoService:
		return CSTR("NO SERVICE");
	case SysMode::GSM:
		return CSTR("GSM");
	case SysMode::CDMA:
		return CSTR("CDMA");
	case SysMode::WCDMA:
		return CSTR("WCDMA");
	case SysMode::TD_SCDMA:
		return CSTR("TD-SCDMA");
	case SysMode::WIMAX:
		return CSTR("WiMAX");
	case SysMode::LTE:
		return CSTR("LTE");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::HuaweiGSMModemController::SubModeGetName(SubMode submode)
{
	switch (submode)
	{
	case SubMode::NoService:
		return CSTR("NO SERVICE");
	case SubMode::GSM:
		return CSTR("GSM");
	case SubMode::GPRS:
		return CSTR("GPRS");
	case SubMode::EDGE:
		return CSTR("EDGE");
	case SubMode::IS95A:
		return CSTR("IS95A");
	case SubMode::IS95B:
		return CSTR("IS95B");
	case SubMode::CDMA2000:
		return CSTR("CDMA2000 1X");
	case SubMode::EVDO_Rel0:
		return CSTR("EVDO Rel0");
	case SubMode::EVDO_RelA:
		return CSTR("EVDO RelA");
	case SubMode::EVDO_RelB:
		return CSTR("EVDO RelB");
	case SubMode::HybridCDMA2000:
		return CSTR("Hybrid (CDMA2000 1X)");
	case SubMode::HybridEVDO_Rel0:
		return CSTR("Hybrid (EVDO Rel0)");
	case SubMode::HybridEVDO_RelA:
		return CSTR("Hybrid (EVDO RelA)");
	case SubMode::HybridEVDO_RelB:
		return CSTR("Hybrid (EVDO RelB)");
	case SubMode::EHRPD_Rel0:
		return CSTR("eHRPD Rel0");
	case SubMode::EHRPD_RelA:
		return CSTR("eHRPD RelA");
	case SubMode::EHRPD_RelB:
		return CSTR("eHRPD RelB");
	case SubMode::HybridEHRPD_Rel0:
		return CSTR("Hybrid (eHRPD Rel0)");
	case SubMode::HybridEHRPD_RelA:
		return CSTR("Hybrid (eHRPD RelA)");
	case SubMode::HybridEHRPD_RelB:
		return CSTR("Hybrid (eHRPD RelB)");
	case SubMode::WCDMA:
		return CSTR("WCDMA");
	case SubMode::HSDPA:
		return CSTR("HSDPA");
	case SubMode::HSUPA:
		return CSTR("HSUPA");
	case SubMode::HSPA:
		return CSTR("HSPA");
	case SubMode::HSPA_:
		return CSTR("HSPA+");
	case SubMode::DC_HSPA_:
		return CSTR("DC-HSPA+");
	case SubMode::TD_SCDMA:
		return CSTR("TD-SCDMA");
	case SubMode::_HSDPA:
		return CSTR("HSDPA");
	case SubMode::_HSUPA:
		return CSTR("HSUPA");
	case SubMode::_HSPA:
		return CSTR("HSPA");
	case SubMode::_HSPA_:
		return CSTR("HSPA+");
	case SubMode::_802_16e:
		return CSTR("802.16e");
	case SubMode::LTE:
		return CSTR("LTE");
	default:
		return CSTR("Unknown");
	}
}
