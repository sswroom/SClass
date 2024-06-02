#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "SSWR/SMonitor/SMonitorRedir.h"
#include "Sync/MutexUsage.h"

void __stdcall SSWR::SMonitor::SMonitorRedir::OnDataUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData)
{
	NN<SSWR::SMonitor::SMonitorRedir> me = userData.GetNN<SSWR::SMonitor::SMonitorRedir>();
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
	this->dataCRC.Clear();
	this->dataCRC.Calc(buff, size);
	this->dataCRC.GetValue(crcVal);
}

SSWR::SMonitor::SMonitorRedir::SMonitorRedir(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log) : dataCRC(Crypto::Hash::CRC16::GetPolynomialCCITT())
{
	this->sockf = sockf;
	this->hostName = Text::String::New(UTF8STRC("sswroom.no-ip.org"));
	this->port = 5100;
	this->recReplyHdlr = 0;
	this->recReplyObj = 0;
	NEW_CLASS(this->svr, Net::UDPServer(this->sockf, 0, 0, CSTR_NULL, OnDataUDPPacket, this, log, CSTR_NULL, 2, false));
}

SSWR::SMonitor::SMonitorRedir::SMonitorRedir(NN<Net::SocketFactory> sockf, NN<Text::String> hostName, UInt16 port, NN<IO::LogTool> log) : dataCRC(Crypto::Hash::CRC16::GetPolynomialCCITT())
{
	this->sockf = sockf;
	this->hostName = hostName->Clone();
	this->port = port;
	this->recReplyHdlr = 0;
	this->recReplyObj = 0;
	NEW_CLASS(this->svr, Net::UDPServer(this->sockf, 0, 0, CSTR_NULL, OnDataUDPPacket, this, log, CSTR_NULL, 2, false));
}

SSWR::SMonitor::SMonitorRedir::~SMonitorRedir()
{
	DEL_CLASS(this->svr);
	this->hostName->Release();
}

Bool SSWR::SMonitor::SMonitorRedir::IsError()
{
	return this->svr->IsError();
}

Bool SSWR::SMonitor::SMonitorRedir::SendDevReading(Int64 cliId, NN<const SSWR::SMonitor::ISMonitorCore::DevRecord2> rec, Int32 reportInterval, Int32 kaInterval)
{
	UInt8 buff[1024];
	buff[0] = 'S';
	buff[1] = 'm';
	WriteInt16(&buff[2], 0);
	WriteInt32(&buff[4], rec->profileId);
	WriteInt64(&buff[8], cliId);
	WriteInt64(&buff[16], rec->recTime);
	WriteUInt32(&buff[24], rec->digitalVals);
	WriteInt32(&buff[28], reportInterval);
	WriteInt32(&buff[32], kaInterval);
	buff[36] = (UInt8)rec->nreading;
	buff[37] = (UInt8)rec->ndigital;
	buff[38] = (UInt8)rec->nOutput;
	buff[39] = 0;
	UOSInt i = 0;
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
	if (this->sockf->DNSResolveIP(this->hostName->ToCString(), addr))
	{
		this->svr->SendTo(addr, this->port, buff, i + 2);
	}
	return true;
}

Bool SSWR::SMonitor::SMonitorRedir::SendDevName(Int64 cliId, UnsafeArray<const UTF8Char> name, UOSInt nameLen)
{
	UInt8 buff[1024];
	UOSInt size;
	buff[0] = 'S';
	buff[1] = 'm';
	WriteInt16(&buff[2], 16);
	WriteInt64(&buff[4], cliId);
	size = (UOSInt)(Text::StrConcatC(&buff[12], name, nameLen) - buff);
	UInt8 calcVal[2];
	this->CalcCRC(buff, size, calcVal);
	buff[size] = calcVal[0] ^ 0x12;
	buff[size + 1] = calcVal[1] ^ 0x34;

	Net::SocketUtil::AddressInfo addr;
	if (this->sockf->DNSResolveIP(this->hostName->ToCString(), addr))
	{
		this->svr->SendTo(addr, this->port, buff, size + 2);
	}
	return true;
}

Bool SSWR::SMonitor::SMonitorRedir::SendDevPlatform(Int64 cliId, UnsafeArray<const UTF8Char> platform, UOSInt nameLen)
{
	UInt8 buff[1024];
	UOSInt size;
	buff[0] = 'S';
	buff[1] = 'm';
	WriteInt16(&buff[2], 18);
	WriteInt64(&buff[4], cliId);
	size = (UOSInt)(Text::StrConcatC(&buff[12], platform, nameLen) - buff);
	UInt8 calcVal[2];
	Sync::MutexUsage mutUsage(this->dataCRCMut);
	this->dataCRC.Clear();
	this->dataCRC.Calc(buff, size);
	this->dataCRC.GetValue(calcVal);
	mutUsage.EndUse();
	buff[size] = calcVal[0] ^ 0x12;
	buff[size + 1] = calcVal[1] ^ 0x34;

	Net::SocketUtil::AddressInfo addr;
	if (this->sockf->DNSResolveIP(this->hostName->ToCString(), addr))
	{
		this->svr->SendTo(addr, this->port, buff, size + 2);
	}
	return true;
}

Bool SSWR::SMonitor::SMonitorRedir::SendDevCPUName(Int64 cliId, UnsafeArray<const UTF8Char> cpuName, UOSInt nameLen)
{
	UInt8 buff[1024];
	UOSInt size;
	buff[0] = 'S';
	buff[1] = 'm';
	WriteInt16(&buff[2], 20);
	WriteInt64(&buff[4], cliId);
	size = (UOSInt)(Text::StrConcatC(&buff[12], cpuName, nameLen) - buff);
	UInt8 calcVal[2];
	this->CalcCRC(buff, size, calcVal);
	buff[size] = calcVal[0] ^ 0x12;
	buff[size + 1] = calcVal[1] ^ 0x34;

	Net::SocketUtil::AddressInfo addr;
	if (this->sockf->DNSResolveIP(this->hostName->ToCString(), addr))
	{
		this->svr->SendTo(addr, this->port, buff, size + 2);
	}
	return true;
}

Bool SSWR::SMonitor::SMonitorRedir::SendDevReadingName(Int64 cliId, UOSInt index, UInt16 sensorId, UInt16 readingId, UnsafeArray<const UTF8Char> readingName, UOSInt nameLen)
{
	UInt8 buff[1024];
	UOSInt size;
	buff[0] = 'S';
	buff[1] = 'm';
	WriteInt16(&buff[2], 22);
	WriteInt64(&buff[4], cliId);
	WriteUInt32(&buff[12], (UInt32)index);
	WriteInt16(&buff[16], sensorId);
	WriteInt16(&buff[18], readingId);
	size = (UOSInt)(Text::StrConcatC(&buff[20], readingName, nameLen) - buff);
	UInt8 calcVal[2];
	this->CalcCRC(buff, size, calcVal);
	buff[size] = calcVal[0] ^ 0x12;
	buff[size + 1] = calcVal[1] ^ 0x34;

	Net::SocketUtil::AddressInfo addr;
	if (this->sockf->DNSResolveIP(this->hostName->ToCString(), addr))
	{
		this->svr->SendTo(addr, this->port, buff, size + 2);
	}
	return true;
}

Bool SSWR::SMonitor::SMonitorRedir::SendDevVersion(Int64 cliId, Int64 progVersion)
{
	UInt8 buff[30];
	UOSInt size;
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
	if (this->sockf->DNSResolveIP(this->hostName->ToCString(), addr))
	{
		this->svr->SendTo(addr, this->port, buff, size + 2);
	}
	return true;
}

void SSWR::SMonitor::SMonitorRedir::HandleRecReply(RecordReplyHandler hdlr, AnyType userObj)
{
	this->recReplyObj = userObj;
	this->recReplyHdlr = hdlr;
}
