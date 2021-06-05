#include "Stdafx.h"
#include "Net/ARPInfo.h"
#include "SSWR/AVIRead/AVIRNetInfoForm.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRNetInfoForm::OnAdaptorSelChg(void *userObj)
{
	UTF8Char sbuff[512];
//	UTF8Char *sptr;
	UInt8 buff[16];
	UInt32 ipAddr;
	UOSInt i;
	SSWR::AVIRead::AVIRNetInfoForm *me = (SSWR::AVIRead::AVIRNetInfoForm*)userObj;
	Net::ConnectionInfo *connInfo = (Net::ConnectionInfo*)me->lbAdaptors->GetSelectedItem();
	Net::ConnectionInfo::ConnectionType connType;
	if (connInfo)
	{
		connInfo->GetName(sbuff);
		me->txtAdaptorName->SetText(sbuff);
		connInfo->GetDescription(sbuff);
		me->txtAdaptorDesc->SetText(sbuff);
		connInfo->GetDNSSuffix(sbuff);
		me->txtAdaptorDNSSuffix->SetText(sbuff);
		connType = connInfo->GetConnectionType();
		switch (connType)
		{
		case Net::ConnectionInfo::CT_DIALUP:
			me->txtAdaptorConnType->SetText((const UTF8Char*)"Dial-up");
			break;
		case Net::ConnectionInfo::CT_ETHERNET:
			me->txtAdaptorConnType->SetText((const UTF8Char*)"Ethernet");
			break;
		case Net::ConnectionInfo::CT_LOOPBACK:
			me->txtAdaptorConnType->SetText((const UTF8Char*)"Loopback");
			break;
		case Net::ConnectionInfo::CT_WIFI:
			me->txtAdaptorConnType->SetText((const UTF8Char*)"WIFI");
			break;
		case Net::ConnectionInfo::CT_UNKNOWN:
		default:
			me->txtAdaptorConnType->SetText((const UTF8Char*)"Unknown");
			break;
		}
		Text::StrInt32(sbuff, (Int32)connInfo->GetMTU());
		me->txtAdaptorMTU->SetText(sbuff);
		i = connInfo->GetPhysicalAddress(buff, 16);
		if (i > 0)
		{
			Text::StrHexBytes(sbuff, buff, i, ':');
		}
		else
		{
			sbuff[0] = 0;
		}
		me->txtAdaptorPhysicalAddr->SetText(sbuff);
		switch (connInfo->GetConnectionStatus())
		{
		case Net::ConnectionInfo::CS_DORMANT:
			me->txtAdaptorMediaState->SetText((const UTF8Char*)"Dormant");
			break;
		case Net::ConnectionInfo::CS_DOWN:
			me->txtAdaptorMediaState->SetText((const UTF8Char*)"Down");
			break;
		case Net::ConnectionInfo::CS_LOWERLAYERDOWN:
			me->txtAdaptorMediaState->SetText((const UTF8Char*)"Lower Layer Down");
			break;
		case Net::ConnectionInfo::CS_NOTPRESENT:
			me->txtAdaptorMediaState->SetText((const UTF8Char*)"Not Present");
			break;
		case Net::ConnectionInfo::CS_TESTING:
			me->txtAdaptorMediaState->SetText((const UTF8Char*)"Testing");
			break;
		case Net::ConnectionInfo::CS_UP:
			me->txtAdaptorMediaState->SetText((const UTF8Char*)"Up");
			break;
		case Net::ConnectionInfo::CS_UNKNOWN:
		default:
			me->txtAdaptorMediaState->SetText((const UTF8Char*)"Unknown");
			break;
		}

		me->lbAdaptorIP->ClearItems();
		i = 0;
		while ((ipAddr = connInfo->GetIPAddress(i++)) != 0)
		{
			Net::SocketUtil::GetIPv4Name(sbuff, ipAddr);
			me->lbAdaptorIP->AddItem(sbuff, 0);
		}
		ipAddr = connInfo->GetDefaultGW();
		if (ipAddr)
		{
			Net::SocketUtil::GetIPv4Name(sbuff, ipAddr);
			me->txtAdaptorGW->SetText(sbuff);
		}
		else
		{
			me->txtAdaptorGW->SetText((const UTF8Char*)"");
		}
		me->lbAdaptorDNS->ClearItems();
		i = 0;
		while ((ipAddr = connInfo->GetDNSAddress(i++)) != 0)
		{
			Net::SocketUtil::GetIPv4Name(sbuff, ipAddr);
			me->lbAdaptorDNS->AddItem(sbuff, 0);
		}

		if (connInfo->IsDhcpEnabled())
		{
			Data::DateTime *dt;
			me->txtAdaptorDHCPEnable->SetText((const UTF8Char*)"Yes");
			Net::SocketUtil::GetIPv4Name(sbuff, connInfo->GetDhcpServer());
			me->txtAdaptorDHCPServer->SetText(sbuff);
			dt = connInfo->GetDhcpLeaseTime();
			if (dt == 0)
			{
				me->txtAdaptorDHCPLeaseTime->SetText((const UTF8Char*)"");
			}
			else 
			{
				dt->ToString(sbuff);
				me->txtAdaptorDHCPLeaseTime->SetText(sbuff);
			}
			dt = connInfo->GetDhcpLeaseExpire();
			if (dt == 0)
			{
				me->txtAdaptorDHCPLeaseExpire->SetText((const UTF8Char*)"");
			}
			else
			{
				dt->ToString(sbuff);
				me->txtAdaptorDHCPLeaseExpire->SetText(sbuff);
			}
		}
		else
		{
			me->txtAdaptorDHCPEnable->SetText((const UTF8Char*)"No");
			me->txtAdaptorDHCPServer->SetText((const UTF8Char*)"");
			me->txtAdaptorDHCPLeaseTime->SetText((const UTF8Char*)"");
			me->txtAdaptorDHCPLeaseExpire->SetText((const UTF8Char*)"");
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRNetInfoForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRNetInfoForm *me = (SSWR::AVIRead::AVIRNetInfoForm *)userObj;
	UOSInt i = me->tcMain->GetSelectedIndex();
	if (i == 1)
	{
		me->UpdateIPStats();
	}
	else if (i == 2)
	{
		me->UpdateTCPStats();
	}
	else if (i == 3)
	{
		me->UpdateUDPStats();
	}
	else if (i == 5)
	{
		if (me->chkPortAuto->IsChecked())
		{
			me->UpdatePortStats();
		}
	}
	else if (i == 6)
	{
		me->UpdateWIFINetworks();
	}
}

void __stdcall SSWR::AVIRead::AVIRNetInfoForm::OnPortClicked(void *userObj)
{
	SSWR::AVIRead::AVIRNetInfoForm *me = (SSWR::AVIRead::AVIRNetInfoForm *)userObj;
	me->UpdatePortStats();
}

void SSWR::AVIRead::AVIRNetInfoForm::UpdateIPStats()
{
	UTF8Char sbuff[12];
	Net::SocketFactory::IPInfo info;
	if (this->core->GetSocketFactory()->GetIPInfo(&info))
	{
		Text::StrUInt32(sbuff, info.dwForwarding);
		this->txtIPStatForwarding->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwDefaultTTL);
		this->txtIPStatDefTTL->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwInReceives);
		this->txtIPStatNRecv->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwInHdrErrors);
		this->txtIPStatNHdrError->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwInAddrErrors);
		this->txtIPStatNAddrError->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwForwDatagrams);
		this->txtIPStatNForwDatag->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwInUnknownProtos);
		this->txtIPStatNUnkProtos->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwInDiscards);
		this->txtIPStatNDiscard->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwInDelivers);
		this->txtIPStatNDeliver->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwOutRequests);
		this->txtIPStatNOutRequest->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwRoutingDiscards);
		this->txtIPStatNRoutingDiscard->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwOutDiscards);
		this->txtIPStatNOutDiscard->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwOutNoRoutes);
		this->txtIPStatNOutNoRoute->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwReasmTimeout);
		this->txtIPStatReasmTimeout->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwReasmReqds);
		this->txtIPStatNReasmReqds->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwReasmOks);
		this->txtIPStatNReasmOk->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwReasmFails);
		this->txtIPStatNReasmFail->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwFragOks);
		this->txtIPStatNFragOksl->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwFragFails);
		this->txtIPStatNFragFail->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwFragCreates);
		this->txtIPStatNFragCreate->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwNumIf);
		this->txtIPStatNIf->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwNumAddr);
		this->txtIPStatNAddr->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwNumRoutes);
		this->txtIPStatNRoute->SetText(sbuff);
	}
	else
	{
		this->txtIPStatForwarding->SetText((const UTF8Char*)"");
		this->txtIPStatDefTTL->SetText((const UTF8Char*)"");
		this->txtIPStatNRecv->SetText((const UTF8Char*)"");
		this->txtIPStatNHdrError->SetText((const UTF8Char*)"");
		this->txtIPStatNAddrError->SetText((const UTF8Char*)"");
		this->txtIPStatNForwDatag->SetText((const UTF8Char*)"");
		this->txtIPStatNUnkProtos->SetText((const UTF8Char*)"");
		this->txtIPStatNDiscard->SetText((const UTF8Char*)"");
		this->txtIPStatNDeliver->SetText((const UTF8Char*)"");
		this->txtIPStatNOutRequest->SetText((const UTF8Char*)"");
		this->txtIPStatNRoutingDiscard->SetText((const UTF8Char*)"");
		this->txtIPStatNOutDiscard->SetText((const UTF8Char*)"");
		this->txtIPStatNOutNoRoute->SetText((const UTF8Char*)"");
		this->txtIPStatReasmTimeout->SetText((const UTF8Char*)"");
		this->txtIPStatNReasmReqds->SetText((const UTF8Char*)"");
		this->txtIPStatNReasmOk->SetText((const UTF8Char*)"");
		this->txtIPStatNReasmFail->SetText((const UTF8Char*)"");
		this->txtIPStatNFragOksl->SetText((const UTF8Char*)"");
		this->txtIPStatNFragFail->SetText((const UTF8Char*)"");
		this->txtIPStatNFragCreate->SetText((const UTF8Char*)"");
		this->txtIPStatNIf->SetText((const UTF8Char*)"");
		this->txtIPStatNAddr->SetText((const UTF8Char*)"");
		this->txtIPStatNRoute->SetText((const UTF8Char*)"");
	}
}

void SSWR::AVIRead::AVIRNetInfoForm::UpdateTCPStats()
{
	UTF8Char sbuff[12];
	Net::SocketFactory::TCPInfo info;
	if (this->core->GetSocketFactory()->GetTCPInfo(&info))
	{
		switch (info.dwRtoAlgorithm)
		{
		case 2: //MIB_TCP_RTO_CONSTANT
			this->txtTCPStatRtoAlgorithm->SetText((const UTF8Char*)"Constant Time-out");
			break;
		case 3: //MIB_TCP_RTO_RSRE
			this->txtTCPStatRtoAlgorithm->SetText((const UTF8Char*)"MIL-STD-1778 Appendix B");
			break;
		case 4: //MIB_TCP_RTO_VANJ
			this->txtTCPStatRtoAlgorithm->SetText((const UTF8Char*)"Van Jacobson's Algorithm");
			break;
		case 1: //MIB_TCP_RTO_OTHER
		default:
			this->txtTCPStatRtoAlgorithm->SetText((const UTF8Char*)"Other");
			break;
		}
		Text::StrUInt32(sbuff, info.dwRtoMin);
		this->txtTCPStatRtoMin->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwRtoMax);
		this->txtTCPStatRtoMax->SetText(sbuff);
		Text::StrInt32(sbuff, info.dwMaxConn);
		this->txtTCPStatMaxConn->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwActiveOpens);
		this->txtTCPStatActiveOpens->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwPassiveOpens);
		this->txtTCPStatPassiveOpens->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwAttemptFails);
		this->txtTCPStatAttemptFails->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwEstabResets);
		this->txtTCPStatEstabResets->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwCurrEstab);
		this->txtTCPStatCurrEstab->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwInSegs);
		this->txtTCPStatInSegs->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwOutSegs);
		this->txtTCPStatOutSegs->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwRetransSegs);
		this->txtTCPStatRetransSeg->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwInErrs);
		this->txtTCPStatInErrs->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwOutRsts);
		this->txtTCPStatOutRsts->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwNumConns);
		this->txtTCPStatNumConns->SetText(sbuff);
	}
	else
	{
		this->txtTCPStatRtoAlgorithm->SetText((const UTF8Char*)"");
		this->txtTCPStatRtoMin->SetText((const UTF8Char*)"");
		this->txtTCPStatRtoMax->SetText((const UTF8Char*)"");
		this->txtTCPStatMaxConn->SetText((const UTF8Char*)"");
		this->txtTCPStatActiveOpens->SetText((const UTF8Char*)"");
		this->txtTCPStatPassiveOpens->SetText((const UTF8Char*)"");
		this->txtTCPStatAttemptFails->SetText((const UTF8Char*)"");
		this->txtTCPStatEstabResets->SetText((const UTF8Char*)"");
		this->txtTCPStatCurrEstab->SetText((const UTF8Char*)"");
		this->txtTCPStatInSegs->SetText((const UTF8Char*)"");
		this->txtTCPStatOutSegs->SetText((const UTF8Char*)"");
		this->txtTCPStatRetransSeg->SetText((const UTF8Char*)"");
		this->txtTCPStatInErrs->SetText((const UTF8Char*)"");
		this->txtTCPStatOutRsts->SetText((const UTF8Char*)"");
		this->txtTCPStatNumConns->SetText((const UTF8Char*)"");
	}
}

void SSWR::AVIRead::AVIRNetInfoForm::UpdateUDPStats()
{
	UTF8Char sbuff[12];
	Net::SocketFactory::UDPInfo info;
	if (this->core->GetSocketFactory()->GetUDPInfo(&info))
	{
		Text::StrUInt32(sbuff, info.dwInDatagrams);
		this->txtUDPStatInDatagrams->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwNoPorts);
		this->txtUDPStatNoPorts->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwInErrors);
		this->txtUDPStatInErrors->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwOutDatagrams);
		this->txtUDPStatOutDatagrams->SetText(sbuff);
		Text::StrUInt32(sbuff, info.dwNumAddrs);
		this->txtUDPStatNumAddrs->SetText(sbuff);
	}
	else
	{
		this->txtUDPStatInDatagrams->SetText((const UTF8Char*)"");
		this->txtUDPStatNoPorts->SetText((const UTF8Char*)"");
		this->txtUDPStatInErrors->SetText((const UTF8Char*)"");
		this->txtUDPStatOutDatagrams->SetText((const UTF8Char*)"");
		this->txtUDPStatNumAddrs->SetText((const UTF8Char*)"");
	}
}

void SSWR::AVIRead::AVIRNetInfoForm::UpdateARPStats()
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char sbuff[64];
//	UTF8Char *sptr;
	UInt32 ipAddr;
	UInt8 buff[32];
	UOSInt v;


	Data::ArrayList<Net::ARPInfo *> arpList;
	Net::ARPInfo *arp;
	this->lvARPInfo->ClearItems();
	Net::ARPInfo::GetARPInfoList(&arpList);
	i = 0;
	j = arpList.GetCount();
	while (i < j)
	{
		arp = arpList.GetItem(i);
		ipAddr = arp->GetIPAddress();
		Net::SocketUtil::GetIPv4Name(sbuff, ipAddr);
		k = this->lvARPInfo->AddItem(sbuff, 0);
		v = arp->GetPhysicalAddr(buff);
		if (v > 0)
		{
			Text::StrHexBytes(sbuff, buff, v, ':');
		}
		else
		{
			sbuff[0] = 0;
		}
		this->lvARPInfo->SetSubItem(k, 1, sbuff);
		Text::StrUInt32(sbuff, arp->GetAdaptorIndex());
		this->lvARPInfo->SetSubItem(k, 2, sbuff);
		switch (arp->GetARPType())
		{
		case Net::ARPInfo::ARPT_STATIC:
			this->lvARPInfo->SetSubItem(k, 3, (const UTF8Char*)"Static");
			break;
		case Net::ARPInfo::ARPT_DYNAMIC:
			this->lvARPInfo->SetSubItem(k, 3, (const UTF8Char*)"Dynamic");
			break;
		case Net::ARPInfo::ARPT_INVALID:
			this->lvARPInfo->SetSubItem(k, 3, (const UTF8Char*)"Invalid");
			break;
		case Net::ARPInfo::ARPT_OTHER:
		default:
			this->lvARPInfo->SetSubItem(k, 3, (const UTF8Char*)"Other");
			break;
		}
		
		DEL_CLASS(arp);
		i++;
	}
}

void SSWR::AVIRead::AVIRNetInfoForm::ReleaseConns()
{
	Net::ConnectionInfo *connInfo;
	UOSInt i;
	i = this->conns->GetCount();
	while (i-- > 0)
	{
		connInfo = this->conns->RemoveAt(i);
		DEL_CLASS(connInfo);
	}
}

void SSWR::AVIRead::AVIRNetInfoForm::UpdateConns()
{
	UTF8Char sbuff[512];
	this->ReleaseConns();
	this->core->GetSocketFactory()->GetConnInfoList(this->conns);
	UOSInt i;
	UOSInt j;
	Net::ConnectionInfo *connInfo;
	this->lbAdaptors->ClearItems();
	i = 0;
	j = this->conns->GetCount();
	while (i < j)
	{
		connInfo = this->conns->GetItem(i);
		connInfo->GetName(sbuff);
		this->lbAdaptors->AddItem(sbuff, connInfo);
		i++;
	}
}

void SSWR::AVIRead::AVIRNetInfoForm::ReleaseWIFIIFs()
{
	Net::WirelessLAN::Interface *interf;
	UOSInt i = this->wlanIfs->GetCount();
	while (i-- > 0)
	{
		interf = this->wlanIfs->RemoveAt(i);
		DEL_CLASS(interf);
	}
}

void SSWR::AVIRead::AVIRNetInfoForm::UpdateWIFIIFs()
{
	UOSInt i;
	UOSInt j;
	Net::WirelessLAN::Interface *interf;
	if (!this->wlan->IsError())
	{
		this->ReleaseWIFIIFs();
		this->wlan->GetInterfaces(this->wlanIfs);
		this->lbWIFIIFs->ClearItems();
		i = 0;
		j = this->wlanIfs->GetCount();
		while (i < j)
		{
			interf = this->wlanIfs->GetItem(i);
			this->lbWIFIIFs->AddItem(interf->GetName(), interf);
			i++;
		}
	}
}

void SSWR::AVIRead::AVIRNetInfoForm::UpdateWIFINetworks()
{
	Net::WirelessLAN::Interface *interf = (Net::WirelessLAN::Interface*)this->lbWIFIIFs->GetSelectedItem();
	if (interf == 0)
	{
		this->lvWIFINetwork->ClearItems();
	}
	else
	{
		UTF8Char sbuff[32];
		UOSInt i;
		UOSInt j;
		UOSInt k;
		Data::ArrayList<Net::WirelessLAN::Network*> networks;
		Net::WirelessLAN::Network *network;
		interf->GetNetworks(&networks);
		this->lvWIFINetwork->ClearItems();
		i = 0;
		j = networks.GetCount();
		while (i < j)
		{
			network = networks.GetItem(i);
			k = this->lvWIFINetwork->AddItem(network->GetSSID(), 0);
			Text::StrDouble(sbuff, network->GetRSSI());
			this->lvWIFINetwork->SetSubItem(k, 1, sbuff);
			DEL_CLASS(network);
			i++;
		}

		Data::ArrayList<Net::WirelessLAN::BSSInfo*> bssList;
		Net::WirelessLAN::BSSInfo *bss;
		interf->GetBSSList(&bssList);
		const UTF8Char *csptr;
		this->lvWIFIBSS->ClearItems();
		i = 0;
		j = bssList.GetCount();
		while (i < j)
		{
			bss = bssList.GetItem(i);
			k = this->lvWIFIBSS->AddItem(bss->GetSSID(), 0);
			Text::StrUInt32(sbuff, bss->GetPHYId());
			this->lvWIFIBSS->SetSubItem(k, 1, sbuff);
			Text::StrHexBytes(sbuff, bss->GetMAC(), 6, 0);
			this->lvWIFIBSS->SetSubItem(k, 2, sbuff);
			Text::StrInt32(sbuff, bss->GetBSSType());
			this->lvWIFIBSS->SetSubItem(k, 3, sbuff);
			Text::StrInt32(sbuff, bss->GetPHYType());
			this->lvWIFIBSS->SetSubItem(k, 4, sbuff);
			Text::StrDouble(sbuff, bss->GetRSSI());
			this->lvWIFIBSS->SetSubItem(k, 5, sbuff);
			Text::StrInt32(sbuff, bss->GetLinkQuality());
			this->lvWIFIBSS->SetSubItem(k, 6, sbuff);
			if ((csptr = bss->GetManuf()) != 0)
			{
				this->lvWIFIBSS->SetSubItem(k, 7, csptr);
			}
			if ((csptr = bss->GetModel()) != 0)
			{
				this->lvWIFIBSS->SetSubItem(k, 8, csptr);
			}
			if ((csptr = bss->GetSN()) != 0)
			{
				this->lvWIFIBSS->SetSubItem(k, 9, csptr);
			}
			DEL_CLASS(bss);
			i++;
		}
		if (wlanScanCnt-- <= 0)
		{
			interf->Scan();
			wlanScanCnt = 3;
		}
	}
}

void SSWR::AVIRead::AVIRNetInfoForm::UpdatePortStats()
{
	Data::ArrayList<Net::SocketFactory::PortInfo2 *> portInfoList;
	UTF8Char sbuff[64];
	Net::SocketFactory::PortInfo2 *portInfo;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	this->lvPortInfo->ClearItems();
	this->core->GetSocketFactory()->QueryPortInfos2(&portInfoList, Net::SocketFactory::PT_ALL, 0);
	i = 0;
	j = portInfoList.GetCount();
	while (i < j)
	{
		portInfo = portInfoList.GetItem(i);
		if (portInfo->protoType == Net::SocketFactory::PT_TCP || portInfo->protoType == Net::SocketFactory::PT_TCP6)
		{
			if (portInfo->protoType == Net::SocketFactory::PT_TCP)
			{
				k = this->lvPortInfo->AddItem((const UTF8Char*)"TCP", 0);
			}
			else
			{
				k = this->lvPortInfo->AddItem((const UTF8Char*)"TCP6", 0);
			}			
			Net::SocketUtil::GetAddrName(sbuff, &portInfo->localAddr, (UInt16)portInfo->localPort);
			this->lvPortInfo->SetSubItem(k, 1, sbuff);
			Net::SocketUtil::GetAddrName(sbuff, &portInfo->foreignAddr, (UInt16)portInfo->foreignPort);
			this->lvPortInfo->SetSubItem(k, 2, sbuff);
			switch (portInfo->portState)
			{
			case Net::SocketFactory::PS_CLOSED:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"CLOSED");
				break;
			case Net::SocketFactory::PS_LISTEN:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"LISTEN");
				break;
			case Net::SocketFactory::PS_SYN_SENT:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"SYN-SENT");
				break;
			case Net::SocketFactory::PS_SYN_RCVD:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"SYN-RECEIVED");
				break;
			case Net::SocketFactory::PS_ESTAB:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"ESTABLISHED");
				break;
			case Net::SocketFactory::PS_FIN_WAIT1:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"FIN-WAIT-1");
				break;
			case Net::SocketFactory::PS_FIN_WAIT2:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"FIN-WAIT-2");
				break;
			case Net::SocketFactory::PS_CLOSE_WAIT:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"CLOSE-WAIT");
				break;
			case Net::SocketFactory::PS_CLOSING:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"CLOSING");
				break;
			case Net::SocketFactory::PS_LAST_ACK:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"LAST-ACK");
				break;
			case Net::SocketFactory::PS_TIME_WAIT:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"TIME-WAIT");
				break;
			case Net::SocketFactory::PS_DELETE_TCB:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"DELETE-TCB");
				break;
			case Net::SocketFactory::PS_UNKNOWN:
			default:
				this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"Unknown");
				break;
			}
			Text::StrInt32(sbuff, portInfo->processId);
			this->lvPortInfo->SetSubItem(k, 4, sbuff);
		}
		else
		{
			if (portInfo->protoType == Net::SocketFactory::PT_UDP)
			{
				k = this->lvPortInfo->AddItem((const UTF8Char*)"UDP", 0);
			}
			else if (portInfo->protoType == Net::SocketFactory::PT_UDP6)
			{
				k = this->lvPortInfo->AddItem((const UTF8Char*)"UDP6", 0);
			}
			else if (portInfo->protoType == Net::SocketFactory::PT_RAW)
			{
				k = this->lvPortInfo->AddItem((const UTF8Char*)"RAW", 0);
			}
			else if (portInfo->protoType == Net::SocketFactory::PT_RAW6)
			{
				k = this->lvPortInfo->AddItem((const UTF8Char*)"RAW6", 0);
			}
			else
			{
				k = this->lvPortInfo->AddItem((const UTF8Char*)"?", 0);
			}
			Net::SocketUtil::GetAddrName(sbuff, &portInfo->localAddr, (UInt16)portInfo->localPort);
			this->lvPortInfo->SetSubItem(k, 1, sbuff);
			Net::SocketUtil::GetAddrName(sbuff, &portInfo->foreignAddr, (UInt16)portInfo->foreignPort);
			this->lvPortInfo->SetSubItem(k, 2, sbuff);
			this->lvPortInfo->SetSubItem(k, 3, (const UTF8Char*)"");
			Text::StrInt32(sbuff, portInfo->processId);
			this->lvPortInfo->SetSubItem(k, 4, sbuff);
		}
		i++;
	}
	this->core->GetSocketFactory()->FreePortInfos2(&portInfoList);
}

SSWR::AVIRead::AVIRNetInfoForm::AVIRNetInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Network Info");

	this->core = core;
	NEW_CLASS(this->wlan, Net::WirelessLAN());
	this->wlanScanCnt = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpAdaptor = this->tcMain->AddTabPage((const UTF8Char*)"Adaptor");
	NEW_CLASS(this->lbAdaptors, UI::GUIListBox(ui, this->tpAdaptor, false));
	this->lbAdaptors->SetRect(0, 0, 200, 100, false);
	this->lbAdaptors->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbAdaptors->HandleSelectionChange(OnAdaptorSelChg, this);
	NEW_CLASS(this->hSplitter, UI::GUIHSplitter(ui, this->tpAdaptor, 3, false));
	NEW_CLASS(this->pnlAdaptor, UI::GUIPanel(ui, this->tpAdaptor));
	this->pnlAdaptor->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASS(this->lblAdaptorName, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"Name"));
	this->lblAdaptorName->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtAdaptorName, UI::GUITextBox(ui, this->pnlAdaptor, (const UTF8Char*)""));
	this->txtAdaptorName->SetRect(104, 4, 300, 23, false);
	this->txtAdaptorName->SetReadOnly(true);
	NEW_CLASS(this->lblAdaptorDesc, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"Description"));
	this->lblAdaptorDesc->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtAdaptorDesc, UI::GUITextBox(ui, this->pnlAdaptor, (const UTF8Char*)""));
	this->txtAdaptorDesc->SetRect(104, 28, 500, 23, false);
	this->txtAdaptorDesc->SetReadOnly(true);
	NEW_CLASS(this->lblAdaptorDNSSuffix, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"DNS Suffix"));
	this->lblAdaptorDNSSuffix->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtAdaptorDNSSuffix, UI::GUITextBox(ui, this->pnlAdaptor, (const UTF8Char*)""));
	this->txtAdaptorDNSSuffix->SetRect(104, 52, 300, 23, false);
	this->txtAdaptorDNSSuffix->SetReadOnly(true);
	NEW_CLASS(this->lblAdaptorConnType, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"Connection Type"));
	this->lblAdaptorConnType->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtAdaptorConnType, UI::GUITextBox(ui, this->pnlAdaptor, (const UTF8Char*)""));
	this->txtAdaptorConnType->SetRect(104, 76, 150, 23, false);
	this->txtAdaptorConnType->SetReadOnly(true);
	NEW_CLASS(this->lblAdaptorMTU, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"MTU"));
	this->lblAdaptorMTU->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtAdaptorMTU, UI::GUITextBox(ui, this->pnlAdaptor, (const UTF8Char*)""));
	this->txtAdaptorMTU->SetRect(104, 100, 100, 23, false);
	this->txtAdaptorMTU->SetReadOnly(true);
	NEW_CLASS(this->lblAdaptorPhysicalAddr, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"Physical Address"));
	this->lblAdaptorPhysicalAddr->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtAdaptorPhysicalAddr, UI::GUITextBox(ui, this->pnlAdaptor, (const UTF8Char*)""));
	this->txtAdaptorPhysicalAddr->SetRect(104, 124, 200, 23, false);
	this->txtAdaptorPhysicalAddr->SetReadOnly(true);
	NEW_CLASS(this->lblAdaptorMediaState, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"Media State"));
	this->lblAdaptorMediaState->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtAdaptorMediaState, UI::GUITextBox(ui, this->pnlAdaptor, (const UTF8Char*)""));
	this->txtAdaptorMediaState->SetRect(104, 148, 150, 23, false);
	this->txtAdaptorMediaState->SetReadOnly(true);
	NEW_CLASS(this->lblAdaptorIP, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"IP Addresses"));
	this->lblAdaptorIP->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->lbAdaptorIP, UI::GUIListBox(ui, this->pnlAdaptor, false));
	this->lbAdaptorIP->SetRect(104, 172, 150, 95, false);
	NEW_CLASS(this->lblAdaptorGW, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"Default Gateway"));
	this->lblAdaptorGW->SetRect(4, 268, 100, 23, false);
	NEW_CLASS(this->txtAdaptorGW, UI::GUITextBox(ui, this->pnlAdaptor, (const UTF8Char*)""));
	this->txtAdaptorGW->SetRect(104, 268, 150, 23, false);
	this->txtAdaptorGW->SetReadOnly(true);
	NEW_CLASS(this->lblAdaptorDNS, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"DNS Addresses"));
	this->lblAdaptorDNS->SetRect(4, 292, 100, 23, false);
	NEW_CLASS(this->lbAdaptorDNS, UI::GUIListBox(ui, this->pnlAdaptor, false));
	this->lbAdaptorDNS->SetRect(104, 292, 150, 95, false);
	NEW_CLASS(this->lblAdaptorDHCPEnable, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"DHCP Enabled"));
	this->lblAdaptorDHCPEnable->SetRect(4, 388, 100, 23, false);
	NEW_CLASS(this->txtAdaptorDHCPEnable, UI::GUITextBox(ui, this->pnlAdaptor, (const UTF8Char*)""));
	this->txtAdaptorDHCPEnable->SetRect(104, 388, 100, 23, false);
	this->txtAdaptorDHCPEnable->SetReadOnly(true);
	NEW_CLASS(this->lblAdaptorDHCPServer, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"DHCP Server"));
	this->lblAdaptorDHCPServer->SetRect(4, 412, 100, 23, false);
	NEW_CLASS(this->txtAdaptorDHCPServer, UI::GUITextBox(ui, this->pnlAdaptor, (const UTF8Char*)""));
	this->txtAdaptorDHCPServer->SetRect(104, 412, 150, 23, false);
	this->txtAdaptorDHCPServer->SetReadOnly(true);
	NEW_CLASS(this->lblAdaptorDHCPLeaseTime, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"Lease Time"));
	this->lblAdaptorDHCPLeaseTime->SetRect(4, 436, 100, 23, false);
	NEW_CLASS(this->txtAdaptorDHCPLeaseTime, UI::GUITextBox(ui, this->pnlAdaptor, (const UTF8Char*)""));
	this->txtAdaptorDHCPLeaseTime->SetRect(104, 436, 200, 23, false);
	this->txtAdaptorDHCPLeaseTime->SetReadOnly(true);
	NEW_CLASS(this->lblAdaptorDHCPLeaseExpire, UI::GUILabel(ui, this->pnlAdaptor, (const UTF8Char*)"Lease Expire"));
	this->lblAdaptorDHCPLeaseExpire->SetRect(4, 460, 100, 23, false);
	NEW_CLASS(this->txtAdaptorDHCPLeaseExpire, UI::GUITextBox(ui, this->pnlAdaptor, (const UTF8Char*)""));
	this->txtAdaptorDHCPLeaseExpire->SetRect(104, 460, 200, 23, false);
	this->txtAdaptorDHCPLeaseExpire->SetReadOnly(true);

	this->tpIPInfo = this->tcMain->AddTabPage((const UTF8Char*)"IP Info");
	NEW_CLASS(this->lblIPStatForwarding, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"IP Forwarding"));
	this->lblIPStatForwarding->SetRect(4, 4, 150, 23, false);
	NEW_CLASS(this->txtIPStatForwarding, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatForwarding->SetRect(154, 4, 100, 23, false);
	this->txtIPStatForwarding->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatDefTTL, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Default TTL"));
	this->lblIPStatDefTTL->SetRect(4, 28, 150, 23, false);
	NEW_CLASS(this->txtIPStatDefTTL, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatDefTTL->SetRect(154, 28, 100, 23, false);
	this->txtIPStatDefTTL->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNRecv, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Datagram Received"));
	this->lblIPStatNRecv->SetRect(4, 52, 150, 23, false);
	NEW_CLASS(this->txtIPStatNRecv, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNRecv->SetRect(154, 52, 100, 23, false);
	this->txtIPStatNRecv->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNHdrError, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Header Errors"));
	this->lblIPStatNHdrError->SetRect(4, 76, 150, 23, false);
	NEW_CLASS(this->txtIPStatNHdrError, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNHdrError->SetRect(154, 76, 100, 23, false);
	this->txtIPStatNHdrError->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNAddrError, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Address Errors"));
	this->lblIPStatNAddrError->SetRect(4, 100, 150, 23, false);
	NEW_CLASS(this->txtIPStatNAddrError, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNAddrError->SetRect(154, 100, 100, 23, false);
	this->txtIPStatNAddrError->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNForwDatag, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Datagram Forwarded"));
	this->lblIPStatNForwDatag->SetRect(4, 124, 150, 23, false);
	NEW_CLASS(this->txtIPStatNForwDatag, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNForwDatag->SetRect(154, 124, 100, 23, false);
	this->txtIPStatNForwDatag->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNUnkProtos, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Unknown Protocol"));
	this->lblIPStatNUnkProtos->SetRect(4, 148, 150, 23, false);
	NEW_CLASS(this->txtIPStatNUnkProtos, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNUnkProtos->SetRect(154, 148, 100, 23, false);
	this->txtIPStatNUnkProtos->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNDiscard, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Receive Discarded"));
	this->lblIPStatNDiscard->SetRect(4, 172, 150, 23, false);
	NEW_CLASS(this->txtIPStatNDiscard, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNDiscard->SetRect(154, 172, 100, 23, false);
	this->txtIPStatNDiscard->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNDeliver, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Datagram Delivered"));
	this->lblIPStatNDeliver->SetRect(4, 196, 150, 23, false);
	NEW_CLASS(this->txtIPStatNDeliver, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNDeliver->SetRect(154, 196, 100, 23, false);
	this->txtIPStatNDeliver->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNOutRequest, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Outgoing Requested"));
	this->lblIPStatNOutRequest->SetRect(4, 220, 150, 23, false);
	NEW_CLASS(this->txtIPStatNOutRequest, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNOutRequest->SetRect(154, 220, 100, 23, false);
	this->txtIPStatNOutRequest->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNRoutingDiscard, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Outgoing Discarded"));
	this->lblIPStatNRoutingDiscard->SetRect(4, 244, 150, 23, false);
	NEW_CLASS(this->txtIPStatNRoutingDiscard, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNRoutingDiscard->SetRect(154, 244, 100, 23, false);
	this->txtIPStatNRoutingDiscard->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNOutDiscard, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Transmitted Discarded"));
	this->lblIPStatNOutDiscard->SetRect(4, 268, 150, 23, false);
	NEW_CLASS(this->txtIPStatNOutDiscard, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNOutDiscard->SetRect(154, 268, 100, 23, false);
	this->txtIPStatNOutDiscard->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNOutNoRoute, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Datagram No Route"));
	this->lblIPStatNOutNoRoute->SetRect(4, 292, 150, 23, false);
	NEW_CLASS(this->txtIPStatNOutNoRoute, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNOutNoRoute->SetRect(154, 292, 100, 23, false);
	this->txtIPStatNOutNoRoute->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatReasmTimeout, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Reassembly Timeout"));
	this->lblIPStatReasmTimeout->SetRect(4, 316, 150, 23, false);
	NEW_CLASS(this->txtIPStatReasmTimeout, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatReasmTimeout->SetRect(154, 316, 100, 23, false);
	this->txtIPStatReasmTimeout->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNReasmReqds, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Reassembly Count"));
	this->lblIPStatNReasmReqds->SetRect(4, 340, 150, 23, false);
	NEW_CLASS(this->txtIPStatNReasmReqds, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNReasmReqds->SetRect(154, 340, 100, 23, false);
	this->txtIPStatNReasmReqds->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNReasmOk, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Reassembly Ok"));
	this->lblIPStatNReasmOk->SetRect(4, 364, 150, 23, false);
	NEW_CLASS(this->txtIPStatNReasmOk, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNReasmOk->SetRect(154, 364, 100, 23, false);
	this->txtIPStatNReasmOk->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNReasmFail, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Reassembly Failed"));
	this->lblIPStatNReasmFail->SetRect(4, 388, 150, 23, false);
	NEW_CLASS(this->txtIPStatNReasmFail, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNReasmFail->SetRect(154, 388, 100, 23, false);
	this->txtIPStatNReasmFail->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNFragOk, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Fragment Ok"));
	this->lblIPStatNFragOk->SetRect(4, 412, 150, 23, false);
	NEW_CLASS(this->txtIPStatNFragOksl, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNFragOksl->SetRect(154, 412, 100, 23, false);
	this->txtIPStatNFragOksl->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNFragFail, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Fragment Failed"));
	this->lblIPStatNFragFail->SetRect(4, 436, 150, 23, false);
	NEW_CLASS(this->txtIPStatNFragFail, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNFragFail->SetRect(154, 436, 100, 23, false);
	this->txtIPStatNFragFail->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNFragCreate, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Fragment Created"));
	this->lblIPStatNFragCreate->SetRect(4, 460, 150, 23, false);
	NEW_CLASS(this->txtIPStatNFragCreate, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNFragCreate->SetRect(154, 460, 100, 23, false);
	this->txtIPStatNFragCreate->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNIf, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Number of Interfaces"));
	this->lblIPStatNIf->SetRect(4, 484, 150, 23, false);
	NEW_CLASS(this->txtIPStatNIf, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNIf->SetRect(154, 484, 100, 23, false);
	this->txtIPStatNIf->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNAddr, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Number of IP Addresses"));
	this->lblIPStatNAddr->SetRect(4, 508, 150, 23, false);
	NEW_CLASS(this->txtIPStatNAddr, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNAddr->SetRect(154, 508, 100, 23, false);
	this->txtIPStatNAddr->SetReadOnly(true);
	NEW_CLASS(this->lblIPStatNRoute, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Number of IP Routes"));
	this->lblIPStatNRoute->SetRect(4, 532, 150, 23, false);
	NEW_CLASS(this->txtIPStatNRoute, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPStatNRoute->SetRect(154, 532, 100, 23, false);
	this->txtIPStatNRoute->SetReadOnly(true);

	this->tpTCPInfo = this->tcMain->AddTabPage((const UTF8Char*)"TCP Info");
	NEW_CLASS(this->lblTCPStatRtoAlgorithm, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"RTO Algorithm"));
	this->lblTCPStatRtoAlgorithm->SetRect(4, 4, 150, 23, false);
	NEW_CLASS(this->txtTCPStatRtoAlgorithm, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatRtoAlgorithm->SetRect(154, 4, 300, 23, false);
	this->txtTCPStatRtoAlgorithm->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatRtoMin, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"RTO Minimum"));
	this->lblTCPStatRtoMin->SetRect(4, 28, 150, 23, false);
	NEW_CLASS(this->txtTCPStatRtoMin, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatRtoMin->SetRect(154, 28, 100, 23, false);
	this->txtTCPStatRtoMin->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatRtoMax, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"RTO Maximum"));
	this->lblTCPStatRtoMax->SetRect(4, 52, 150, 23, false);
	NEW_CLASS(this->txtTCPStatRtoMax, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatRtoMax->SetRect(154, 52, 100, 23, false);
	this->txtTCPStatRtoMax->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatMaxConn, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"Max Connections"));
	this->lblTCPStatMaxConn->SetRect(4, 76, 150, 23, false);
	NEW_CLASS(this->txtTCPStatMaxConn, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatMaxConn->SetRect(154, 76, 100, 23, false);
	this->txtTCPStatMaxConn->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatActiveOpens, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"Active Opens"));
	this->lblTCPStatActiveOpens->SetRect(4, 100, 150, 23, false);
	NEW_CLASS(this->txtTCPStatActiveOpens, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatActiveOpens->SetRect(154, 100, 100, 23, false);
	this->txtTCPStatActiveOpens->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatPassiveOpens, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"Passive Opens"));
	this->lblTCPStatPassiveOpens->SetRect(4, 124, 150, 23, false);
	NEW_CLASS(this->txtTCPStatPassiveOpens, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatPassiveOpens->SetRect(154, 124, 100, 23, false);
	this->txtTCPStatPassiveOpens->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatAttemptFails, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"Attempt Fails"));
	this->lblTCPStatAttemptFails->SetRect(4, 148, 150, 23, false);
	NEW_CLASS(this->txtTCPStatAttemptFails, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatAttemptFails->SetRect(154, 148, 100, 23, false);
	this->txtTCPStatAttemptFails->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatEstabResets, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"Connection Resets"));
	this->lblTCPStatEstabResets->SetRect(4, 172, 150, 23, false);
	NEW_CLASS(this->txtTCPStatEstabResets, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatEstabResets->SetRect(154, 172, 100, 23, false);
	this->txtTCPStatEstabResets->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatCurrEstab, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"Currently Establish Conn"));
	this->lblTCPStatCurrEstab->SetRect(4, 196, 150, 23, false);
	NEW_CLASS(this->txtTCPStatCurrEstab, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatCurrEstab->SetRect(154, 196, 100, 23, false);
	this->txtTCPStatCurrEstab->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatInSegs, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"Recv Segments"));
	this->lblTCPStatInSegs->SetRect(4, 220, 150, 23, false);
	NEW_CLASS(this->txtTCPStatInSegs, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatInSegs->SetRect(154, 220, 100, 23, false);
	this->txtTCPStatInSegs->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatOutSegs, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"Transmit Segments"));
	this->lblTCPStatOutSegs->SetRect(4, 244, 150, 23, false);
	NEW_CLASS(this->txtTCPStatOutSegs, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatOutSegs->SetRect(154, 244, 100, 23, false);
	this->txtTCPStatOutSegs->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatRetransSegs, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"Retransmit Segment"));
	this->lblTCPStatRetransSegs->SetRect(4, 268, 150, 23, false);
	NEW_CLASS(this->txtTCPStatRetransSeg, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatRetransSeg->SetRect(154, 268, 100, 23, false);
	this->txtTCPStatRetransSeg->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatInErrs, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"Recv Errors"));
	this->lblTCPStatInErrs->SetRect(4, 292, 150, 23, false);
	NEW_CLASS(this->txtTCPStatInErrs, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatInErrs->SetRect(154, 292, 100, 23, false);
	this->txtTCPStatInErrs->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatOutRsts, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"Transmit with Errors"));
	this->lblTCPStatOutRsts->SetRect(4, 316, 150, 23, false);
	NEW_CLASS(this->txtTCPStatOutRsts, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatOutRsts->SetRect(154, 316, 100, 23, false);
	this->txtTCPStatOutRsts->SetReadOnly(true);
	NEW_CLASS(this->lblTCPStatNumConns, UI::GUILabel(ui, this->tpTCPInfo, (const UTF8Char*)"Number of Conns"));
	this->lblTCPStatNumConns->SetRect(4, 340, 150, 23, false);
	NEW_CLASS(this->txtTCPStatNumConns, UI::GUITextBox(ui, this->tpTCPInfo, (const UTF8Char*)""));
	this->txtTCPStatNumConns->SetRect(154, 340, 100, 23, false);
	this->txtTCPStatNumConns->SetReadOnly(true);

	this->tpUDPInfo = this->tcMain->AddTabPage((const UTF8Char*)"UDP Info");
	NEW_CLASS(this->lblUDPStatInDatagrams, UI::GUILabel(ui, this->tpUDPInfo, (const UTF8Char*)"Recv Datagrams"));
	this->lblUDPStatInDatagrams->SetRect(4, 4, 150, 23, false);
	NEW_CLASS(this->txtUDPStatInDatagrams, UI::GUITextBox(ui, this->tpUDPInfo, (const UTF8Char*)""));
	this->txtUDPStatInDatagrams->SetRect(154, 4, 100, 23, false);
	this->txtUDPStatInDatagrams->SetReadOnly(true);
	NEW_CLASS(this->lblUDPStatNoPorts, UI::GUILabel(ui, this->tpUDPInfo, (const UTF8Char*)"Recv Error (No Port)"));
	this->lblUDPStatNoPorts->SetRect(4, 28, 150, 23, false);
	NEW_CLASS(this->txtUDPStatNoPorts, UI::GUITextBox(ui, this->tpUDPInfo, (const UTF8Char*)""));
	this->txtUDPStatNoPorts->SetRect(154, 28, 100, 23, false);
	this->txtUDPStatNoPorts->SetReadOnly(true);
	NEW_CLASS(this->lblUDPStatInErrors, UI::GUILabel(ui, this->tpUDPInfo, (const UTF8Char*)"Recv Error (Other)"));
	this->lblUDPStatInErrors->SetRect(4, 52, 150, 23, false);
	NEW_CLASS(this->txtUDPStatInErrors, UI::GUITextBox(ui, this->tpUDPInfo, (const UTF8Char*)""));
	this->txtUDPStatInErrors->SetRect(154, 52, 100, 23, false);
	this->txtUDPStatInErrors->SetReadOnly(true);
	NEW_CLASS(this->lblUDPStatOutDatagrams, UI::GUILabel(ui, this->tpUDPInfo, (const UTF8Char*)"Send Datagrams"));
	this->lblUDPStatOutDatagrams->SetRect(4, 76, 150, 23, false);
	NEW_CLASS(this->txtUDPStatOutDatagrams, UI::GUITextBox(ui, this->tpUDPInfo, (const UTF8Char*)""));
	this->txtUDPStatOutDatagrams->SetRect(154, 76, 100, 23, false);
	this->txtUDPStatOutDatagrams->SetReadOnly(true);
	NEW_CLASS(this->lblUDPStatNumAddrs, UI::GUILabel(ui, this->tpUDPInfo, (const UTF8Char*)"UDP Listens"));
	this->lblUDPStatNumAddrs->SetRect(4, 100, 150, 23, false);
	NEW_CLASS(this->txtUDPStatNumAddrs, UI::GUITextBox(ui, this->tpUDPInfo, (const UTF8Char*)""));
	this->txtUDPStatNumAddrs->SetRect(154, 100, 100, 23, false);
	this->txtUDPStatNumAddrs->SetReadOnly(true);

	this->tpARPInfo = this->tcMain->AddTabPage((const UTF8Char*)"ARP Info");
	NEW_CLASS(this->lvARPInfo, UI::GUIListView(ui, this->tpARPInfo, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvARPInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvARPInfo->AddColumn((const UTF8Char*)"IP Address", 150);
	this->lvARPInfo->AddColumn((const UTF8Char*)"Physical Address", 250);
	this->lvARPInfo->AddColumn((const UTF8Char*)"Adaptor", 100);
	this->lvARPInfo->AddColumn((const UTF8Char*)"Type", 150);

	this->tpPortInfo = this->tcMain->AddTabPage((const UTF8Char*)"Port Info");
	NEW_CLASS(this->pnlPortInfo, UI::GUIPanel(ui, this->tpPortInfo));
	this->pnlPortInfo->SetRect(0, 0, 100, 40, false);
	this->pnlPortInfo->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnPortRefresh, UI::GUIButton(ui, this->pnlPortInfo, (const UTF8Char*)"&Refresh"));
	this->btnPortRefresh->SetRect(8, 8, 75, 23, false);
	this->btnPortRefresh->HandleButtonClick(OnPortClicked, this);
	NEW_CLASS(this->chkPortAuto, UI::GUICheckBox(ui, this->pnlPortInfo, (const UTF8Char*)"Auto Refresh", false));
	this->chkPortAuto->SetRect(100, 8, 100, 23, false);
	NEW_CLASS(this->lvPortInfo, UI::GUIListView(ui, this->tpPortInfo, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvPortInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPortInfo->AddColumn((const UTF8Char*)"Proto", 50);
	this->lvPortInfo->AddColumn((const UTF8Char*)"Local Addr", 150);
	this->lvPortInfo->AddColumn((const UTF8Char*)"Foreign Addr", 150);
	this->lvPortInfo->AddColumn((const UTF8Char*)"State", 200);
	this->lvPortInfo->AddColumn((const UTF8Char*)"PID", 100);
	this->lvPortInfo->SetFullRowSelect(true);

	if (!this->wlan->IsError())
	{
		this->tpWIFIInfo = this->tcMain->AddTabPage((const UTF8Char*)"Wireless");
		NEW_CLASS(this->lbWIFIIFs, UI::GUIListBox(ui, this->tpWIFIInfo, false));
		this->lbWIFIIFs->SetRect(0, 0, 200, 100, false);
		this->lbWIFIIFs->SetDockType(UI::GUIControl::DOCK_LEFT);
		NEW_CLASS(this->hspWIFI, UI::GUIHSplitter(ui, this->tpWIFIInfo, 3, false));
		NEW_CLASS(this->tcWIFI, UI::GUITabControl(ui, this->tpWIFIInfo));
		this->tcWIFI->SetDockType(UI::GUIControl::DOCK_FILL);
		this->tpWIFIAP = this->tcWIFI->AddTabPage((const UTF8Char*)"AP");
		NEW_CLASS(this->lvWIFINetwork, UI::GUIListView(ui, this->tpWIFIAP, UI::GUIListView::LVSTYLE_TABLE, 2));
		this->lvWIFINetwork->SetDockType(UI::GUIControl::DOCK_FILL);
		this->lvWIFINetwork->SetShowGrid(true);
		this->lvWIFINetwork->SetFullRowSelect(true);
		this->lvWIFINetwork->AddColumn((const UTF8Char*)"SSID", 200);
		this->lvWIFINetwork->AddColumn((const UTF8Char*)"RSSI", 100);
		this->tpWIFIBSS = this->tcWIFI->AddTabPage((const UTF8Char*)"BSS");
		NEW_CLASS(this->lvWIFIBSS, UI::GUIListView(ui, this->tpWIFIBSS, UI::GUIListView::LVSTYLE_TABLE, 10));
		this->lvWIFIBSS->SetDockType(UI::GUIControl::DOCK_FILL);
		this->lvWIFIBSS->SetShowGrid(true);
		this->lvWIFIBSS->SetFullRowSelect(true);
		this->lvWIFIBSS->AddColumn((const UTF8Char*)"SSID", 200);
		this->lvWIFIBSS->AddColumn((const UTF8Char*)"PhyId", 100);
		this->lvWIFIBSS->AddColumn((const UTF8Char*)"MAC", 100);
		this->lvWIFIBSS->AddColumn((const UTF8Char*)"BSSType", 100);
		this->lvWIFIBSS->AddColumn((const UTF8Char*)"PHYType", 100);
		this->lvWIFIBSS->AddColumn((const UTF8Char*)"RSSI", 100);
		this->lvWIFIBSS->AddColumn((const UTF8Char*)"Link Quality", 100);
		this->lvWIFIBSS->AddColumn((const UTF8Char*)"Manufacturer", 100);
		this->lvWIFIBSS->AddColumn((const UTF8Char*)"Model", 100);
		this->lvWIFIBSS->AddColumn((const UTF8Char*)"S/N", 100);
	}

	NEW_CLASS(this->conns, Data::ArrayList<Net::ConnectionInfo*>());
	NEW_CLASS(this->wlanIfs, Data::ArrayList<Net::WirelessLAN::Interface*>());
	this->UpdateConns();
	this->UpdateIPStats();
	this->UpdateTCPStats();
	this->UpdateUDPStats();
	this->UpdateARPStats();
	this->UpdateWIFIIFs();
	this->UpdatePortStats();

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRNetInfoForm::~AVIRNetInfoForm()
{
	this->ReleaseConns();
	this->ReleaseWIFIIFs();
	DEL_CLASS(this->conns);
	DEL_CLASS(this->wlanIfs);
	DEL_CLASS(this->wlan);
}

void SSWR::AVIRead::AVIRNetInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
