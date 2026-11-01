#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Data/DateTime.h"
#include "Net/NetAttackDetector.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

#define TIMEDETECTRANGE 3000
#define DETECTCOUNT 5

static void UpdateTCPSourceStatus(NN<Net::NetAttackDetector::TCPSourceStatus> status, UInt16 destPort, Bool hasCVEOption, Int64 reqTime, Bool &portScanDetected, Bool &cveDetected)
{
	if (status->lastDetectTime + TIMEDETECTRANGE < reqTime)
	{
		status->detectCnt = 1;
	}
	else
	{
		Int64 lastTime = status->targetPortMap.Get(destPort);
		if (lastTime == 0 || lastTime + TIMEDETECTRANGE < reqTime)
		{
			status->detectCnt++;
			if (status->detectCnt == DETECTCOUNT)
			{
				portScanDetected = true;
			}
		}
	}
	status->targetPortMap.Put(destPort, reqTime);
	status->lastDetectTime = reqTime;
	if (hasCVEOption && (status->lastCVEAlertTime == 0 || status->lastCVEAlertTime + TIMEDETECTRANGE < reqTime))
	{
		status->lastCVEAlertTime = reqTime;
		cveDetected = true;
	}
}

void __stdcall Net::NetAttackDetector::OnRAWData(AnyType userData, UnsafeArray<const UInt8> packetData, UIntOS packetSize)
{
	NN<Net::NetAttackDetector> me = userData.GetNN<Net::NetAttackDetector>();
	UInt8 macBuff[8];
	UInt16 etherType;
	Net::SocketUtil::AddressInfo addr;
	if (packetSize < 14)
	{
		return;
	}
	etherType = ReadMUInt16(&packetData[12]);
	if (etherType == 0x0800) //IPv4
	{
		if (packetSize >= 34)
		{
			UnsafeArray<const UInt8> ipData = &packetData[14];
			UIntOS ipHeaderSize = (ipData[0] & 0x0f) << 2;
			UInt16 ipPacketSize = ReadMUInt16(&ipData[2]);
			if ((ipData[0] & 0xf0) == 0x40 && ipHeaderSize >= 20 && ipPacketSize >= ipHeaderSize && ipPacketSize <= packetSize - 14 && ipData[9] == 6 && (ReadMUInt16(&ipData[6]) & 0x1fff) == 0 && ipPacketSize - ipHeaderSize >= 20)
			{
				UnsafeArray<const UInt8> tcpData = &ipData[ipHeaderSize];
				UIntOS tcpHeaderSize = (tcpData[12] >> 4) << 2;
				if (tcpHeaderSize >= 20 && ipPacketSize - ipHeaderSize >= tcpHeaderSize && (tcpData[13] & 0x02) != 0 && (tcpData[13] & 0x10) == 0)
				{
					UInt32 sourceIP = ReadNUInt32(&ipData[12]);
					UInt16 destPort = ReadMUInt16(&tcpData[2]);
					Bool hasCVEOption = false;
					UIntOS optionIndex = 20;
					while (optionIndex < ipHeaderSize)
					{
						UInt8 optionType = ipData[optionIndex];
						if (optionType == 0)
							break;
						if (optionType == 1)
						{
							optionIndex++;
							continue;
						}
						if (optionIndex + 2 > ipHeaderSize)
							break;
						UInt8 optionSize = ipData[optionIndex + 1];
						if (optionSize < 2 || optionSize > ipHeaderSize - optionIndex)
							break;
						if (optionType == 7 || optionType == 68)
						{
							hasCVEOption = true;
							break;
						}
						optionIndex += optionSize;
					}

					Int64 reqTime;
					Data::DateTime dt;
					dt.SetCurrTimeUTC();
					reqTime = dt.ToTicks();
					Bool portScanDetected = false;
					Bool cveDetected = false;
					NN<TCPSourceStatus> status;
					{
						Sync::MutexUsage mutUsage(me->adapterMut);
						if (!me->tcpSourceMap.Get(sourceIP).SetTo(status))
						{
							NEW_CLASSNN(status, TCPSourceStatus());
							status->lastDetectTime = 0;
							status->detectCnt = 0;
							status->lastCVEAlertTime = 0;
							me->tcpSourceMap.Put(sourceIP, status);
						}
						UpdateTCPSourceStatus(status, destPort, hasCVEOption, reqTime, portScanDetected, cveDetected);
					}

					if (portScanDetected || cveDetected)
					{
						Net::SocketUtil::SetAddrInfoV4(addr, sourceIP);
						if (portScanDetected)
							me->hdlr(addr, AttackType::TCPPortScan, me->userData);
						if (cveDetected)
							me->hdlr(addr, AttackType::CVE_2026_98432, me->userData);
					}
				}
			}
		}
	}
	else if (etherType == 0x86dd) //IPv6
	{
		if (packetSize >= 54)
		{
			UnsafeArray<const UInt8> ipData = &packetData[14];
			UInt16 payloadSize = ReadMUInt16(&ipData[4]);
			if ((ipData[0] & 0xf0) == 0x60 && payloadSize > 0 && payloadSize <= packetSize - 54)
			{
				UInt8 nextHeader = ipData[6];
				UIntOS payloadOffset = 40;
				UIntOS payloadEnd = payloadOffset + payloadSize;
				Bool hasCVEOption = false;
				Bool validHeaderChain = true;
				UIntOS extHeaderCount = 0;
				while (nextHeader != 6 && validHeaderChain && extHeaderCount++ < 16)
				{
					if (nextHeader == 0 || nextHeader == 43 || nextHeader == 60 || nextHeader == 135 || nextHeader == 139 || nextHeader == 140)
					{
						if (payloadOffset + 2 > payloadEnd)
						{
							validHeaderChain = false;
							break;
						}
						UInt8 extensionType = nextHeader;
						UInt8 followingHeader = ipData[payloadOffset];
						UIntOS extensionSize = ((UIntOS)ipData[payloadOffset + 1] + 1) << 3;
						if (extensionSize > payloadEnd - payloadOffset)
						{
							validHeaderChain = false;
							break;
						}
						if ((extensionType == 0 || extensionType == 60) && !hasCVEOption)
						{
							UIntOS optionOffset = payloadOffset + 2;
							UIntOS optionEnd = payloadOffset + extensionSize;
							while (optionOffset < optionEnd)
							{
								UInt8 optionType = ipData[optionOffset];
								if (optionType == 0)
								{
									optionOffset++;
									continue;
								}
								if (optionOffset + 2 > optionEnd)
									break;
								UInt8 optionSize = ipData[optionOffset + 1];
								if (optionSize > optionEnd - optionOffset - 2)
									break;
								if (optionType == 7)
								{
									hasCVEOption = true;
									break;
								}
								optionOffset += (UIntOS)optionSize + 2;
							}
						}
						payloadOffset += extensionSize;
						nextHeader = followingHeader;
					}
					else if (nextHeader == 44)
					{
						if (payloadOffset + 8 > payloadEnd)
						{
							validHeaderChain = false;
							break;
						}
						UInt8 followingHeader = ipData[payloadOffset];
						UInt16 fragmentOffset = ReadMUInt16(&ipData[payloadOffset + 2]) & 0xfff8;
						payloadOffset += 8;
						nextHeader = followingHeader;
						if (fragmentOffset != 0)
							validHeaderChain = false;
					}
					else if (nextHeader == 51)
					{
						if (payloadOffset + 2 > payloadEnd)
						{
							validHeaderChain = false;
							break;
						}
						UInt8 followingHeader = ipData[payloadOffset];
						UIntOS extensionSize = ((UIntOS)ipData[payloadOffset + 1] + 2) << 2;
						if (extensionSize > payloadEnd - payloadOffset)
						{
							validHeaderChain = false;
							break;
						}
						payloadOffset += extensionSize;
						nextHeader = followingHeader;
					}
					else
					{
						validHeaderChain = false;
					}
				}

				if (validHeaderChain && nextHeader == 6 && payloadOffset + 20 <= payloadEnd)
				{
					UnsafeArray<const UInt8> tcpData = &ipData[payloadOffset];
					UIntOS tcpHeaderSize = (tcpData[12] >> 4) << 2;
					if (tcpHeaderSize >= 20 && tcpHeaderSize <= payloadEnd - payloadOffset && (tcpData[13] & 0x02) != 0 && (tcpData[13] & 0x10) == 0)
					{
						UInt16 destPort = ReadMUInt16(&tcpData[2]);
						TCPSourceIPv6Key sourceKey;
						sourceKey.high = ReadMUInt64(&ipData[8]);
						sourceKey.low = ReadMUInt64(&ipData[16]);
						Int64 reqTime;
						Data::DateTime dt;
						dt.SetCurrTimeUTC();
						reqTime = dt.ToTicks();
						Bool portScanDetected = false;
						Bool cveDetected = false;
						NN<TCPSourceStatus> status;
						{
							Sync::MutexUsage mutUsage(me->adapterMut);
							if (!me->tcpSourceIPv6Map.Get(sourceKey).SetTo(status))
							{
								NEW_CLASSNN(status, TCPSourceStatus());
								status->lastDetectTime = 0;
								status->detectCnt = 0;
								status->lastCVEAlertTime = 0;
								me->tcpSourceIPv6Map.Put(sourceKey, status);
							}
							UpdateTCPSourceStatus(status, destPort, hasCVEOption, reqTime, portScanDetected, cveDetected);
						}

						if (portScanDetected || cveDetected)
						{
							Net::SocketUtil::SetAddrInfoV6(addr, &ipData[8], 0);
							if (portScanDetected)
								me->hdlr(addr, AttackType::TCPPortScan, me->userData);
							if (cveDetected)
								me->hdlr(addr, AttackType::CVE_2026_98432, me->userData);
						}
					}
				}
			}
		}
	}
	else if (etherType == 0x0806) // ARP
	{
		if (packetSize >= 42)
		{
			UInt16 opcode = ReadMUInt16(&packetData[20]);
			if (opcode == 1) //Request
			{
				UInt32 ip;
				Int64 reqTime;
				Int64 lastTime;
				UInt64 iMAC;
				NN<AdapterStatus> adapter;
				Data::DateTime dt;
				dt.SetCurrTimeUTC();
				reqTime = dt.ToTicks();
				macBuff[0] = 0;
				macBuff[1] = 0;
				MemCopyNO(&macBuff[2], &packetData[6], 6);
				iMAC = ReadMUInt64(macBuff);
				ip = ReadMUInt32(&packetData[38]);
				Sync::MutexUsage mutUsage(me->adapterMut);
				if (me->adapterMap.Get(iMAC).SetTo(adapter))
				{
					if (adapter->lastDetectTime + TIMEDETECTRANGE < reqTime)
					{
						adapter->detectCnt = 1;
					}
					else
					{
						lastTime = adapter->targetIPMap.Get(ip);
						if (lastTime == 0 || lastTime + TIMEDETECTRANGE < reqTime)
						{
							adapter->detectCnt++;
							if (adapter->detectCnt == DETECTCOUNT)
							{
								Net::SocketUtil::SetAddrInfoMAC(addr, &macBuff[2]);
								me->hdlr(addr, AttackType::IPScan, me->userData);
							}
						}
					}
					adapter->targetIPMap.Put(ip, reqTime);
					adapter->lastDetectTime = reqTime;
				}
				else
				{
					NEW_CLASSNN(adapter, AdapterStatus());
					adapter->iMAC = iMAC;
					adapter->lastDetectTime = reqTime;
					adapter->detectCnt = 1;
					adapter->targetIPMap.Put(ip, reqTime);
					me->adapterMap.Put(iMAC, adapter);
				}
			}
			else if (opcode == 2) //Reply
			{
				UInt32 ip;
				Int64 reqTime;
				Int64 lastTime;
				UInt64 iMAC;
				NN<AdapterStatus> adapter;
				Data::DateTime dt;
				dt.SetCurrTimeUTC();
				reqTime = dt.ToTicks();
				macBuff[0] = 0;
				macBuff[1] = 0;
				MemCopyNO(&macBuff[2], &packetData[0], 6);
				iMAC = ReadMUInt64(macBuff);
				ip = ReadMUInt32(&packetData[28]);
				Sync::MutexUsage mutUsage(me->adapterMut);
				if (me->adapterMap.Get(iMAC).SetTo(adapter))
				{
					if (adapter->lastDetectTime + TIMEDETECTRANGE < reqTime)
					{
						adapter->detectCnt = 1;
					}
					else
					{
						lastTime = adapter->targetIPMap.Get(ip);
						if (lastTime == 0 || lastTime + TIMEDETECTRANGE < reqTime)
						{
							adapter->detectCnt++;
							if (adapter->detectCnt == DETECTCOUNT)
							{
								Net::SocketUtil::SetAddrInfoMAC(addr, &macBuff[2]);
								me->hdlr(addr, AttackType::IPScan, me->userData);
							}
						}
					}
					adapter->targetIPMap.Put(ip, reqTime);
					adapter->lastDetectTime = reqTime;
				}
				else
				{
					NEW_CLASSNN(adapter, AdapterStatus());
					adapter->iMAC = iMAC;
					adapter->lastDetectTime = reqTime;
					adapter->detectCnt = 1;
					adapter->targetIPMap.Put(ip, reqTime);
					me->adapterMap.Put(iMAC, adapter);
				}
			}
		}
	}
}

Net::NetAttackDetector::NetAttackDetector(NN<Net::SocketFactory> sockf, AttackHandler hdlr, AnyType userData, UIntOS threadCnt)
{
	this->sockf = sockf;
	this->hdlr = hdlr;
	this->userData = userData;
	this->socMon = nullptr;

	NN<Socket> soc;
	if (this->sockf->CreateRAWSocket().SetTo(soc))
	{
		NEW_CLASSOPT(this->socMon, Net::SocketMonitor(sockf, soc, OnRAWData, this, threadCnt));
	}
}

Net::NetAttackDetector::~NetAttackDetector()
{
	this->socMon.Delete();

	NN<Net::NetAttackDetector::AdapterStatus> adapter;
	UIntOS i = this->adapterMap.GetCount();
	while (i-- > 0)
	{
		adapter = this->adapterMap.GetItemNoCheck(i);
		adapter.Delete();
	}
	NN<Net::NetAttackDetector::TCPSourceStatus> tcpStatus;
	i = this->tcpSourceMap.GetCount();
	while (i-- > 0)
	{
		tcpStatus = this->tcpSourceMap.GetItemNoCheck(i);
		tcpStatus.Delete();
	}
	i = this->tcpSourceIPv6Map.GetCount();
	while (i-- > 0)
	{
		tcpStatus = this->tcpSourceIPv6Map.GetItemNoCheck(i);
		tcpStatus.Delete();
	}
}

Bool Net::NetAttackDetector::IsError()
{
	return this->socMon.IsNull();
}
