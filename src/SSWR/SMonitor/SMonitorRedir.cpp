#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "SSWR/SMonitor/SMonitorRedir.h"
#include "Sync/MutexUsage.h"

void __stdcall SSWR::SMonitor::SMonitorRedir::OnDataUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	SSWR::SMonitor::SMonitorRedir *me = (SSWR::SMonitor::SMonitorRedir*)userData;
	if (dataSize >= 6 && buff[0] == 'S' && buff[1] == 'm')
	{
		UInt8 calcVal[2];
		me->CalcCRC(buff, dataSize - 2, calcVal);
		if (calcVal[0] == (buff[dataSize - 2] ^ 0x12) && calcVal[1] == (buff[dataSize - 1] ^ 0x34))
		{
			UInt16 cmdType = ReadUInt16(&buff[2]);
			switch (cmdType)
			{
			case 1:
				if (dataSize >= 14)
				{
					Int64 recTime = ReadInt64(&buff[4]);
					Int64 svrTime;
					if (dataSize >= 22)
					{
						svrTime = ReadInt64(&buff[12]);
					}
					else
					{
						svrTime = 0;
					}
					if (me->recReplyHdlr)
					{
						me->recReplyHdlr(me->recReplyObj, recTime, svrTime);
					}
				}
				break;
			case 9: //Photo Request
				break;
			case 13: //Photo Packet
				break;
			case 15: //Photo End
				break;
			case 21: //Output Change
				if (dataSize >= 8)
				{
//					UInt8 outputId = buff[4];
//					UInt8 outputState = buff[5];
				}
				break;
			}
		}
	}
}

void SSWR::SMonitor::SMonitorRedir::CalcCRC(const UInt8 *buff, UOSInt size, UInt8 *crcVal)
{
	Sync::MutexUsage mutUsage(this->dataCRCMut);
	this->dataCRC->Clear();
	this->dataCRC->Calc(buff, size);
	this->dataCRC->GetValue(crcVal);
}

SSWR::SMonitor::SMonitorRedir::SMonitorRedir(Net::SocketFactory *sockf)
{
	this->sockf = sockf;
	this->hostName = Text::StrCopyNew((const UTF8Char*)"sswroom.no-ip.org");
	this->port = 5100;
	this->recReplyHdlr = 0;
	this->recReplyObj = 0;
	NEW_CLASS(this->dataCRC, Crypto::Hash::CRC16(Crypto::Hash::CRC16::GetPolynomialCCITT()));
	NEW_CLASS(this->dataCRCMut, Sync::Mutex());
	NEW_CLASS(this->svr, Net::UDPServer(this->sockf, 0, 0, 0, OnDataUDPPacket, this, 0, 0, 2, false));
}

SSWR::SMonitor::SMonitorRedir::SMonitorRedir(Net::SocketFactory *sockf, const UTF8Char *hostName, UInt16 port)
{
	this->sockf = sockf;
	this->hostName = Text::StrCopyNew(hostName);
	this->port = port;
	this->recReplyHdlr = 0;
	this->recReplyObj = 0;
	NEW_CLASS(this->dataCRC, Crypto::Hash::CRC16(Crypto::Hash::CRC16::GetPolynomialCCITT()));
	NEW_CLASS(this->dataCRCMut, Sync::Mutex());
	NEW_CLASS(this->svr, Net::UDPServer(this->sockf, 0, 0, 0, OnDataUDPPacket, this, 0, 0, 2, false));
}

SSWR::SMonitor::SMonitorRedir::~SMonitorRedir()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->dataCRC);
	DEL_CLASS(this->dataCRCMut);
	Text::StrDelNew(this->hostName);
}

Bool SSWR::SMonitor::SMonitorRedir::IsError()
{
	return this->svr->IsError();
}

Bool SSWR::SMonitor::SMonitorRedir::SendDevReading(Int64 cliId, const SSWR::SMonitor::ISMonitorCore::DevRecord2 *rec, Int32 reportInterval, Int32 kaInterval)
{
	UInt8 buff[1024];
	buff[0] = 'S';
	buff[1] = 'm';
	WriteInt16(&buff[2], 0);
	WriteInt32(&buff[4], rec->profileId);
	WriteInt64(&buff[8], cliId);
	WriteInt64(&buff[16], rec->recTime);
	WriteInt32(&buff[24], rec->digitalVals);
	WriteInt32(&buff[28], reportInterval);
	WriteInt32(&buff[32], kaInterval);
	buff[36] = (UInt8)rec->nreading;
	buff[37] = (UInt8)rec->ndigital;
	buff[38] = (UInt8)rec->nOutput;
	buff[39] = 0;
	OSInt i = 0;
	while (i < rec->nreading)
	{
		WriteNInt64(&buff[40 + 16 * i], ReadNInt64(rec->readings[i].status));
		WriteDouble(&buff[40 + 16 * i + 8], rec->readings[i].reading);
		i++;
	}
	i = 40 + 16 * rec->nreading;
	UInt8 calcVal[2];
	this->CalcCRC(buff, i, calcVal);
	buff[i] = calcVal[0] ^ 0x12;
	buff[i + 1] = calcVal[1] ^ 0x34;

	Net::SocketUtil::AddressInfo addr;
	if (this->sockf->DNSResolveIP(this->hostName, &addr))
	{
		this->svr->SendTo(&addr, this->port, buff, i + 2);
	}
	return true;
}

Bool SSWR::SMonitor::SMonitorRedir::SendDevName(Int64 cliId, const UTF8Char *name)
{
	UInt8 buff[1024];
	OSInt size;
	buff[0] = 'S';
	buff[1] = 'm';
	WriteInt16(&buff[2], 16);
	WriteInt64(&buff[4], cliId);
	size = Text::StrConcat(&buff[12], name) - buff;
	UInt8 calcVal[2];
	this->CalcCRC(buff, size, calcVal);
	buff[size] = calcVal[0] ^ 0x12;
	buff[size + 1] = calcVal[1] ^ 0x34;

	Net::SocketUtil::AddressInfo addr;
	if (this->sockf->DNSResolveIP(this->hostName, &addr))
	{
		this->svr->SendTo(&addr, this->port, buff, size + 2);
	}
	return true;
}

Bool SSWR::SMonitor::SMonitorRedir::SendDevPlatform(Int64 cliId, const UTF8Char *platform)
{
	UInt8 buff[1024];
	OSInt size;
	buff[0] = 'S';
	buff[1] = 'm';
	WriteInt16(&buff[2], 18);
	WriteInt64(&buff[4], cliId);
	size = Text::StrConcat(&buff[12], platform) - buff;
	UInt8 calcVal[2];
	Sync::MutexUsage mutUsage(this->dataCRCMut);
	this->dataCRC->Clear();
	this->dataCRC->Calc(buff, size);
	this->dataCRC->GetValue(calcVal);
	mutUsage.EndUse();
	buff[size] = calcVal[0] ^ 0x12;
	buff[size + 1] = calcVal[1] ^ 0x34;

	Net::SocketUtil::AddressInfo addr;
	if (this->sockf->DNSResolveIP(this->hostName, &addr))
	{
		this->svr->SendTo(&addr, this->port, buff, size + 2);
	}
	return true;
}

Bool SSWR::SMonitor::SMonitorRedir::SendDevCPUName(Int64 cliId, const UTF8Char *cpuName)
{
	UInt8 buff[1024];
	OSInt size;
	buff[0] = 'S';
	buff[1] = 'm';
	WriteInt16(&buff[2], 20);
	WriteInt64(&buff[4], cliId);
	size = Text::StrConcat(&buff[12], cpuName) - buff;
	UInt8 calcVal[2];
	this->CalcCRC(buff, size, calcVal);
	buff[size] = calcVal[0] ^ 0x12;
	buff[size + 1] = calcVal[1] ^ 0x34;

	Net::SocketUtil::AddressInfo addr;
	if (this->sockf->DNSResolveIP(this->hostName, &addr))
	{
		this->svr->SendTo(&addr, this->port, buff, size + 2);
	}
	return true;
}

Bool SSWR::SMonitor::SMonitorRedir::SendDevReadingName(Int64 cliId, OSInt index, UInt16 sensorId, UInt16 readingId, const UTF8Char *readingName)
{
	UInt8 buff[1024];
	OSInt size;
	buff[0] = 'S';
	buff[1] = 'm';
	WriteInt16(&buff[2], 22);
	WriteInt64(&buff[4], cliId);
	WriteInt32(&buff[12], (Int32)index);
	WriteInt16(&buff[16], sensorId);
	WriteInt16(&buff[18], readingId);
	size = Text::StrConcat(&buff[20], readingName) - buff;
	UInt8 calcVal[2];
	this->CalcCRC(buff, size, calcVal);
	buff[size] = calcVal[0] ^ 0x12;
	buff[size + 1] = calcVal[1] ^ 0x34;

	Net::SocketUtil::AddressInfo addr;
	if (this->sockf->DNSResolveIP(this->hostName, &addr))
	{
		this->svr->SendTo(&addr, this->port, buff, size + 2);
	}
	return true;
}

Bool SSWR::SMonitor::SMonitorRedir::SendDevVersion(Int64 cliId, Int64 progVersion)
{
	UInt8 buff[30];
	OSInt size;
	buff[0] = 'S';
	buff[1] = 'm';
	WriteInt16(&buff[2], 24);
	WriteInt64(&buff[4], cliId);
	WriteInt64(&buff[12], progVersion);
	size = 20;
	UInt8 calcVal[2];
	this->CalcCRC(buff, 20, calcVal);
	buff[size] = calcVal[0] ^ 0x12;
	buff[size + 1] = calcVal[1] ^ 0x34;

	Net::SocketUtil::AddressInfo addr;
	if (this->sockf->DNSResolveIP(this->hostName, &addr))
	{
		this->svr->SendTo(&addr, this->port, buff, size + 2);
	}
	return true;
}

void SSWR::SMonitor::SMonitorRedir::HandleRecReply(RecordReplyHandler hdlr, void *userObj)
{
	this->recReplyObj = userObj;
	this->recReplyHdlr = hdlr;
}
