#include "Stdafx.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "Net/BandwidthLogger.h"

void __stdcall Net::BandwidthLogger::OnDataPacket(AnyType userData, UnsafeArray<const UInt8> packetData, UIntOS packetSize)
{
	NN<Net::BandwidthLogger> me = userData.GetNN<Net::BandwidthLogger>();
	Int64 currTime = Data::DateTimeUtil::GetCurrTimeMillis() / 1000;
	NN<Net::EthernetAnalyzer::BandwidthStat> stat;
	Sync::MutexUsage mutUsage(me->mut);
	UInt16 etherType = ReadMUInt16(&packetData[12]);
	if (etherType == 0x0800)
	{
		if ((packetData[14 + 0] & 0xf0) == 0x40)
		{
			UInt32 srcIP = ReadMUInt32(&packetData[14 + 12]);
			UInt32 destIP = ReadMUInt32(&packetData[14 + 16]);
			if (!me->ipStats.Get(srcIP).SetTo(stat))
			{
				stat = Net::EthernetAnalyzer::BandwidthStatCreate(srcIP, currTime);
				me->ipStats.Put(srcIP, stat);
			}
			me->BandwidthStatTime(stat, currTime);
			stat->currStat.recvBytes += packetSize + 14;
			stat->currStat.recvCnt++;

			if (!me->ipStats.Get(destIP).SetTo(stat))
			{
				stat = Net::EthernetAnalyzer::BandwidthStatCreate(destIP, currTime);
				me->ipStats.Put(destIP, stat);
			}
			me->BandwidthStatTime(stat, currTime);
			stat->currStat.sendBytes += packetSize + 14;
			stat->currStat.sendCnt++;

			if (packetData[14 + 9] == 17)
			{
				UnsafeArray<const UInt8> ipData;
				UIntOS ipDataSize;
				if ((packetData[14] & 0xf) <= 5)
				{
					ipData = &packetData[14 + 20];
					ipDataSize = packetSize - 20 - 14;
				}
				else
				{
					ipData = &packetData[14 + ((packetData[14] & 0xf) << 2)];
					ipDataSize = packetSize - ((packetData[14] & 0xf) << 2) - 14;
				}
				if (ipDataSize >= 8)
				{
					UInt16 srcPort = ReadMUInt16(&ipData[0]);
					UInt16 udpLeng = ReadMUInt16(&ipData[4]);
					if (udpLeng <= ipDataSize && srcPort == 53)
					{
						Data::ArrayListNN<Net::DNSClient::RequestAnswer> answers;
						NN<Net::DNSClient::RequestAnswer> answer;
						Net::DNSClient::ParseAnswers(&ipData[8], ipDataSize - 8, answers);
						if (answers.GetCount() > 0)
						{
							NN<DNSResult> res;
							NN<Text::String> reqName;
							Data::Timestamp currTime = Data::Timestamp::UtcNow();
							reqName = answers.GetItemNoCheck(0)->name;
							answer = answers.GetItemNoCheck(answers.GetCount() - 1);
							if (answer->recType == 1)
							{
								UIntOS i;
								UInt32 sortIP;
								i = answers.GetCount();
								while (i-- > 0)
								{
									answer = answers.GetItemNoCheck(i);
									if (answer->recType == 1)
									{
										sortIP = ReadMUInt32(answer->addr.addr);
										if (!me->dnsRes.Get(sortIP).SetTo(res))
										{
											res = MemAllocNN(DNSResult);
											res->ip = sortIP;
											res->name = reqName->Clone();
											me->dnsRes.Put(sortIP, res);
										}
										else if (!res->name->Equals(reqName))
										{
											res->name->Release();
											res->name = reqName->Clone();
										}
									}
								}
							}
							Net::DNSClient::FreeAnswers(answers);
						}					
					}
				}
			}
		}
	}	
}

void __stdcall Net::BandwidthLogger::DNSResultFree(NN<DNSResult> res)
{
	res->name->Release();
	MemFreeNN(res);
}


void Net::BandwidthLogger::BandwidthStatTime(NN<Net::EthernetAnalyzer::BandwidthStat> stat, Int64 time)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt32 ip;
	NN<IO::Stream> stm;
	if (stat->currStat.time != time)
	{
		stat->lastStat = stat->currStat;
		stat->currStat.time = time;
		stat->currStat.recvBytes = 0;
		stat->currStat.recvCnt = 0;
		stat->currStat.sendBytes = 0;
		stat->currStat.sendCnt = 0;

		if (this->stm.SetTo(stm))
		{
#if IS_BYTEORDER_LE
			ip = BSWAPU32(stat->ip);
#else
			ip = stat->ip;
#endif
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
			*sptr++ = ',';
			sptr = Text::StrInt64(sptr, stat->lastStat.time);
			*sptr++ = ',';
			sptr = Text::StrUIntOS(sptr, stat->lastStat.recvCnt);
			*sptr++ = ',';
			sptr = Text::StrUInt64(sptr, stat->lastStat.recvBytes);
			*sptr++ = ',';
			sptr = Text::StrUIntOS(sptr, stat->lastStat.sendCnt);
			*sptr++ = ',';
			sptr = Text::StrUInt64(sptr, stat->lastStat.sendBytes);
			sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
			stm->WriteCont(sbuff, (UIntOS)(sptr - sbuff));
		}
	}
}

Net::BandwidthLogger::BandwidthLogger(NN<Net::SocketFactory> sockf)
{
	this->sockf = sockf;
	this->socMon = nullptr;
	this->fs = nullptr;
	this->stm = nullptr;
	this->logBeginTime = 0;
	NN<Socket> s;
	if (sockf->CreateRAWSocket().SetTo(s))
	{
		NEW_CLASSOPT(this->socMon, Net::SocketMonitor(this->sockf, s, OnDataPacket, this, 4));
	}
}

Net::BandwidthLogger::~BandwidthLogger()
{
	this->socMon.Delete();
	this->EndLogFile();
	this->ipStats.MemFreeAll();
	this->dnsRes.FreeAll(DNSResultFree);
}

Bool Net::BandwidthLogger::BeginLogFile(Text::CStringNN fileName)
{
	NN<IO::Stream> stm;
	NN<IO::Stream> stm2;
	if (this->stm.NotNull())
		return false;
	NEW_CLASSNN(stm, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (stm->IsDown())
	{
		stm.Delete();
		return false;
	}
	NEW_CLASSNN(stm2, IO::BufferedOutputStream(stm, 16384));
	stm2->Write(CSTR("IP,Time,RecvCnt,RecvBytes,SendCnt,SendBytes\r\n").ToByteArray());
	Sync::MutexUsage mutUsage(this->mut);
	this->logBeginTime = Data::DateTimeUtil::GetCurrTimeMillis() / 1000;
	this->fs = stm;
	this->stm = stm2;
	return true;
}

void Net::BandwidthLogger::EndLogFile()
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt32 ip;
	NN<IO::Stream> stm;
	if (this->stm.SetTo(stm))
	{
		Int64 logTime = this->logBeginTime;
		NN<Net::EthernetAnalyzer::BandwidthStat> stat;
		Sync::MutexUsage mutUsage(this->mut);
		UIntOS i = 0;
		UIntOS j = this->ipStats.GetCount();
		while (i < j)
		{
			stat = this->ipStats.GetItemNoCheck(i);
			if (stat->currStat.time >= logTime)
			{
#if IS_BYTEORDER_LE
				ip = BSWAPU32(stat->ip);
#else
				ip = stat->ip;
#endif
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
				*sptr++ = ',';
				sptr = Text::StrInt64(sptr, stat->currStat.time);
				*sptr++ = ',';
				sptr = Text::StrUIntOS(sptr, stat->currStat.recvCnt);
				*sptr++ = ',';
				sptr = Text::StrUInt64(sptr, stat->currStat.recvBytes);
				*sptr++ = ',';
				sptr = Text::StrUIntOS(sptr, stat->currStat.sendCnt);
				*sptr++ = ',';
				sptr = Text::StrUInt64(sptr, stat->currStat.sendBytes);
				sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
				stm->WriteCont(sbuff, (UIntOS)(sptr - sbuff));
			}
			i++;
		}
		this->stm.Delete();
		this->fs.Delete();
	}
}

NN<const Data::UInt32FastMapNN<Net::EthernetAnalyzer::BandwidthStat>> Net::BandwidthLogger::GetIPStats(NN<Sync::MutexUsage> mutUsage) const
{
	mutUsage->ReplaceMutex(this->mut);
	return this->ipStats;
}

Optional<Text::String> Net::BandwidthLogger::GetIPName(UInt32 ip, NN<Sync::MutexUsage> mutUsage) const
{
	mutUsage->ReplaceMutex(this->mut);
	NN<DNSResult> res;
	if (this->dnsRes.Get(ip).SetTo(res))
	{
		return res->name;
	}
	return nullptr;
}
