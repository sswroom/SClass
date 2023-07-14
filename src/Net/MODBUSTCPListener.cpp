
#include "Stdafx.h"
#include "Net/MODBUSTCPListener.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Text/StringBuilderUTF8.h"
#include <stdio.h>

void __stdcall Net::MODBUSTCPListener::OnClientConn(Socket *s, void *userObj)
{
	Net::MODBUSTCPListener *me = (Net::MODBUSTCPListener*)userObj;
	NotNullPtr<Net::TCPClient> cli;
	NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
	me->cliMgr->AddClient(cli, 0);
}

void __stdcall Net::MODBUSTCPListener::OnClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		cli.Delete();
	}
}

void __stdcall Net::MODBUSTCPListener::OnClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
	if (size <= 6)
	{
		return;
	}
	UInt8 retBuff[256];
	Net::MODBUSTCPListener *me = (Net::MODBUSTCPListener*)userObj;
	UInt16 tranId = ReadMUInt16(&buff[0]);
	UInt16 protoId = ReadMUInt16(&buff[2]);
	UInt16 packetSize = ReadMUInt16(&buff[4]);
	Bool succ;
	Bool bVal;
	UInt16 iVal;
	UOSInt i;
	UOSInt j;
	if (packetSize >= 4 && size == (UOSInt)packetSize + 6)
	{
		UInt8 addr = buff[6];
		Sync::MutexUsage mutUsage(&me->devMut);
		IO::MODBUSDevSim *dev = me->devMap.Get(addr);
		if (dev)
		{
			switch (buff[7])
			{
			case 1: //Read coil
				if (packetSize == 6)
				{
					UInt16 devAddr = ReadMUInt16(&buff[8]);
					UInt16 nCoil = ReadMUInt16(&buff[10]);
					if (nCoil > 0)
					{
						UInt8 byteSize = (UInt8)((nCoil + 7) >> 3);
						WriteMUInt16(&retBuff[0], tranId);
						WriteMUInt16(&retBuff[2], 0);
						WriteMUInt16(&retBuff[4], (UInt16)(byteSize + 3));
						retBuff[6] = addr;
						retBuff[7] = 1;
						retBuff[8] = byteSize;
						succ = true;
						i = 0;
						j = 9;
						while (i < nCoil)
						{
							if (!dev->ReadCoil((UInt16)(devAddr + i), &bVal))
							{
								succ = false;
								break;
							}
							if (i & 7)
							{
								retBuff[j] = (UInt8)(retBuff[j] | (bVal << (i & 7)));
							}
							else
							{
								retBuff[j] = bVal;
							}
							i++;
							if ((i & 7) == 0)
							{
								j++;
							}
						}
						if (succ)
						{
							if (me->delay)
							{
								Sync::SimpleThread::Sleep(me->delay);
							}
							cli->Write(retBuff, (UOSInt)byteSize + 9);
						}
					}
				}
				break;
			case 2: //Read Discrete Inputs
				if (packetSize == 6)
				{
					UInt16 devAddr = ReadMUInt16(&buff[8]);
					UInt16 nInputs = ReadMUInt16(&buff[10]);
					if (nInputs > 0)
					{
						UInt8 byteSize = (UInt8)((nInputs + 7) >> 3);
						WriteMUInt16(&retBuff[0], tranId);
						WriteMUInt16(&retBuff[2], 0);
						WriteMUInt16(&retBuff[4], (UInt16)(byteSize + 3));
						retBuff[6] = addr;
						retBuff[7] = 2;
						retBuff[8] = byteSize;
						succ = true;
						i = 0;
						j = 9;
						while (i < nInputs)
						{
							if (!dev->ReadDescreteInput((UInt16)(devAddr + i), &bVal))
							{
								succ = false;
								break;
							}
							if (i & 7)
							{
								retBuff[j] = (UInt8)(retBuff[j] | (bVal << (i & 7)));
							}
							else
							{
								retBuff[j] = bVal;
							}
							i++;
							if ((i & 7) == 0)
							{
								j++;
							}
						}
						if (succ)
						{
							if (me->delay)
							{
								Sync::SimpleThread::Sleep(me->delay);
							}
							cli->Write(retBuff, (UOSInt)byteSize + 9);
						}
					}
				}
				break;
			case 3: //Read Holding Registers
				if (packetSize == 6)
				{
					UInt16 devAddr = ReadMUInt16(&buff[8]);
					UInt16 nReg = ReadMUInt16(&buff[10]);
					if (nReg > 0)
					{
						UInt8 byteSize = (UInt8)(nReg << 1);
						WriteMUInt16(&retBuff[0], tranId);
						WriteMUInt16(&retBuff[2], 0);
						WriteMUInt16(&retBuff[4], (UInt16)(byteSize + 3));
						retBuff[6] = addr;
						retBuff[7] = 3;
						retBuff[8] = byteSize;
						succ = true;
						i = 0;
						while (i < nReg)
						{
							if (!dev->ReadHoldingRegister((UInt16)(devAddr + i), &iVal))
							{
								succ = false;
								break;
							}
							WriteMUInt16(&retBuff[9 + i * 2], iVal);
							i++;
						}
						if (succ)
						{
							if (me->delay)
							{
								Sync::SimpleThread::Sleep(me->delay);
							}
							cli->Write(retBuff, (UOSInt)byteSize + 9);
						}
					}
				}
				break;
			case 4: //Read Input Registers
				if (packetSize == 6)
				{
					UInt16 devAddr = ReadMUInt16(&buff[8]);
					UInt16 nReg = ReadMUInt16(&buff[10]);
					if (nReg > 0)
					{
						UInt8 byteSize = (UInt8)(nReg << 1);
						WriteMUInt16(&retBuff[0], tranId);
						WriteMUInt16(&retBuff[2], 0);
						WriteMUInt16(&retBuff[4], (UInt16)(byteSize + 3));
						retBuff[6] = addr;
						retBuff[7] = 4;
						retBuff[8] = byteSize;
						succ = true;
						i = 0;
						while (i < nReg)
						{
							if (!dev->ReadInputRegister((UInt16)(devAddr + i), &iVal))
							{
								succ = false;
								break;
							}
							WriteMUInt16(&retBuff[9 + i * 2], iVal);
							i++;
						}
						if (succ)
						{
							if (me->delay)
							{
								Sync::SimpleThread::Sleep(me->delay);
							}
							cli->Write(retBuff, (UOSInt)byteSize + 9);
						}
					}
				}
				break;
			case 5: //Write single coil
				if (packetSize == 6)
				{
					UInt16 devAddr = ReadMUInt16(&buff[8]);
					UInt16 val = ReadMUInt16(&buff[10]);
					if (val == 0 || val == 0xff00)
					{
						if (dev->WriteCoil(devAddr, val == 0xff00))
						{
							WriteMUInt16(&retBuff[0], tranId);
							WriteMUInt16(&retBuff[2], 0);
							WriteMUInt16(&retBuff[4], 6);
							retBuff[6] = addr;
							retBuff[7] = 5;
							WriteMUInt16(&retBuff[8], devAddr);
							WriteMUInt16(&retBuff[10], val);
							if (me->delay)
							{
								Sync::SimpleThread::Sleep(me->delay);
							}
							cli->Write(retBuff, 12);
						}
					}
				}
				break;
			case 6: //Write single holding register
				if (packetSize == 6)
				{
					UInt16 devAddr = ReadMUInt16(&buff[8]);
					UInt16 val = ReadMUInt16(&buff[10]);
					if (dev->WriteHoldingRegister(devAddr, val))
					{
						WriteMUInt16(&retBuff[0], tranId);
						WriteMUInt16(&retBuff[2], 0);
						WriteMUInt16(&retBuff[4], 6);
						retBuff[6] = addr;
						retBuff[7] = 6;
						WriteMUInt16(&retBuff[8], devAddr);
						WriteMUInt16(&retBuff[10], val);
						if (me->delay)
						{
							Sync::SimpleThread::Sleep(me->delay);
						}
						cli->Write(retBuff, 12);
					}
				}
				break;
			default:
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("Received: tranId = "));
					sb.AppendU16(tranId);
					sb.AppendC(UTF8STRC(", protoId = "));
					sb.AppendU16(protoId);
					sb.AppendC(UTF8STRC(", packetSize = "));
					sb.AppendU16(packetSize);
					sb.AppendC(UTF8STRC(", "));
					sb.AppendHexBuff(buff + 6, packetSize, ' ', Text::LineBreakType::CRLF);
					printf("%s\r\n", sb.ToString());
				}
				break;
			}
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Received: "));
		sb.AppendHexBuff(buff, size, ' ', Text::LineBreakType::CRLF);
		printf("%s\r\n", sb.ToString());
	}
}

void __stdcall Net::MODBUSTCPListener::OnClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData)
{

}

Net::MODBUSTCPListener::MODBUSTCPListener(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log, Bool autoStart)
{
	this->sockf = sockf;
	this->delay = 0;
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(120, OnClientEvent, OnClientData, this, 2, OnClientTimeout));
	NEW_CLASS(this->svr, Net::TCPServer(this->sockf, port, log, OnClientConn, this, CSTR("MODBUSTCP: "), autoStart));
}

Net::MODBUSTCPListener::~MODBUSTCPListener()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->cliMgr);

	UOSInt i = this->devMap.GetCount();
	IO::MODBUSDevSim *dev;
	while (i-- > 0)
	{
		dev = this->devMap.GetItem(i);
		DEL_CLASS(dev);
	}
}

Bool Net::MODBUSTCPListener::Start()
{
	return this->svr->Start();
}

Bool Net::MODBUSTCPListener::IsError()
{
	return this->svr->IsV4Error();
}

void Net::MODBUSTCPListener::AddDevice(UInt8 addr, IO::MODBUSDevSim *dev)
{
	Sync::MutexUsage mutUsage(&this->devMut);
	dev = this->devMap.Put(addr, dev);
	SDEL_CLASS(dev);
}

UOSInt Net::MODBUSTCPListener::GetDeviceCount()
{
	return this->devMap.GetCount();
}

IO::MODBUSDevSim *Net::MODBUSTCPListener::GetDevice(UOSInt index)
{
	return this->devMap.GetItem(index);
}

UInt32 Net::MODBUSTCPListener::GetDeviceAddr(UOSInt index)
{
	return this->devMap.GetKey(index);
}

UInt32 Net::MODBUSTCPListener::GetDelay()
{
	return this->delay;
}

void Net::MODBUSTCPListener::SetDelay(UInt32 delay)
{
	this->delay = delay;
}
