#include "Stdafx.h"
#include "Crypto/Hash/CRC16.h"
#include "Data/ByteTool.h"
#include "IO/MODBUSTCPMaster.h"
#include "Math/Math.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include <stdio.h>
#define CMDDELAY 0

UInt32 __stdcall IO::MODBUSTCPMaster::ThreadProc(void *userObj)
{
	IO::MODBUSTCPMaster *me = (IO::MODBUSTCPMaster*)userObj;
	UInt8 buff[1024];
	OSInt buffSize = 0;
	OSInt readSize;
	OSInt i;
	Bool incomplete;
	AddrResultCb *cb;
//	Text::StringBuilderUTF8 *sb;
	me->threadRunning = true;
//	NEW_CLASS(sb, Text::StringBuilderUTF8());
	while (!me->threadToStop)
	{
		readSize = me->stm->Read(&buff[buffSize], 1024 - buffSize);
//		printf("ReadSize: %d\r\n", readSize);
		if (readSize > 0)
		{
//			sb->ClearStr();
//			sb->AppendHexBuff(&buff[buffSize], readSize, ' ', Text::LBT_CRLF);
//			printf("Received: %s\r\n", sb->ToString());
			buffSize += readSize;
			incomplete = false;
			i = 0;
			while (i < buffSize - 5)
			{
				if ( buff[i + 2] == 0 && buff[i + 3] == 0)
				{
					UInt16 packetSize = ReadMUInt16(&buff[i + 4]);
					if (i + 6 + packetSize > buffSize)
					{
						incomplete = true;
					}
					else
					{
						switch (buff[i + 7])
						{
						case 1:
						case 2:
						case 3:
						case 4:
							if (3 + buff[i + 8] == packetSize)
							{
								cb = me->cbMap->Get(buff[i + 6]);
								if (cb && cb->readFunc)
								{
									cb->readFunc(cb->userObj, buff[i + 7], &buff[i + 9], buff[i + 8]);
								}
								i += 6 + packetSize;
							}
							else
							{
								i++;
							}
							break;
						case 5:
						case 6:
						case 15:
						case 16:
							if (packetSize == 6)
							{
								cb = me->cbMap->Get(buff[i + 6]);
								if (cb && cb->setFunc)
								{
									cb->setFunc(cb->userObj, buff[i + 7], ReadMUInt16(&buff[i + 8]), ReadMUInt16(&buff[i + 10]));
								}
								i += 12;
							}
							else
							{
								i++;
							}
							break;
						default:
							i += packetSize + 6;
							break;
						}
					}
				}
				else
				{
					i++;
				}

				if (incomplete)
				{
					break;
				}
			}

			if (i >= buffSize)
			{
				buffSize = 0;
			}
			else if (i >= 0)
			{
				MemCopyO(buff, &buff[i], buffSize - i);
				buffSize -= i;
			}
		}
		else
		{
			Sync::Thread::Sleep(100);
		}
		
	}
//	DEL_CLASS(sb);
	me->threadRunning = false;
	return 0;
}

IO::MODBUSTCPMaster::MODBUSTCPMaster(IO::Stream *stm)
{
	this->stm = stm;
	this->threadRunning = false;
	this->threadToStop = false;
	this->tranId = 0;
	NEW_CLASS(this->cbMap, Data::Integer32Map<AddrResultCb*>());
	NEW_CLASS(this->clk, Manage::HiResClock());
	NEW_CLASS(this->stmMut, Sync::Mutex());
	if (this->stm)
	{
		Sync::Thread::Create(ThreadProc, this);
		while (!this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}
}

IO::MODBUSTCPMaster::~MODBUSTCPMaster()
{
	OSInt i;
	if (this->stm)
	{
		this->threadToStop = true;
		this->stm->Close();
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}
	Data::ArrayList<AddrResultCb*> *cbList = this->cbMap->GetValues();
	AddrResultCb *cb;
	i = cbList->GetCount();
	while (i-- > 0)
	{
		cb = cbList->GetItem(i);
		MemFree(cb);
	}
	DEL_CLASS(this->cbMap)

	DEL_CLASS(this->stmMut);
	DEL_CLASS(this->clk);
}

Bool IO::MODBUSTCPMaster::ReadCoils(UInt8 devAddr, UInt16 coilAddr, UInt16 coilCnt)
{
	UInt8 buff[12];
	WriteMInt16(buff, this->tranId);
	this->tranId++;
	buff[2] = 0;
	buff[3] = 0;
	buff[4] = 0;
	buff[5] = 6;
	buff[6] = devAddr;
	buff[7] = 1;
	WriteMInt16(&buff[8], coilAddr);
	WriteMInt16(&buff[10], coilCnt);
	if (this->stm)
	{
		this->stmMut->Lock();
		Double t = this->clk->GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::Thread::Sleep(CMDDELAY - Math::Double2Int32(t * 1000));
		}
		this->stm->Write(buff, 12);
		this->clk->Start();
		this->stmMut->Unlock();
	}
	return true;
}

Bool IO::MODBUSTCPMaster::ReadInputs(UInt8 devAddr, UInt16 inputAddr, UInt16 inputCnt)
{
	UInt8 buff[12];
	WriteMInt16(buff, this->tranId);
	this->tranId++;
	buff[2] = 0;
	buff[3] = 0;
	buff[4] = 0;
	buff[5] = 6;
	buff[6] = devAddr;
	buff[7] = 2;
	WriteMInt16(&buff[8], inputAddr);
	WriteMInt16(&buff[10], inputCnt);
	if (this->stm)
	{
		this->stmMut->Lock();
		Double t = this->clk->GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::Thread::Sleep(CMDDELAY - Math::Double2Int32(t * 1000));
		}
		this->stm->Write(buff, 12);
		this->clk->Start();
		this->stmMut->Unlock();
	}
	return true;
}

Bool IO::MODBUSTCPMaster::ReadHoldingRegisters(UInt8 devAddr, UInt16 regAddr, UInt16 regCnt)
{
	UInt8 buff[12];
	WriteMInt16(buff, this->tranId);
	this->tranId++;
	buff[2] = 0;
	buff[3] = 0;
	buff[4] = 0;
	buff[5] = 6;
	buff[6] = devAddr;
	buff[7] = 3;
	WriteMInt16(&buff[8], regAddr);
	WriteMInt16(&buff[10], regCnt);
	if (this->stm)
	{
		this->stmMut->Lock();
		Double t = this->clk->GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::Thread::Sleep(CMDDELAY - Math::Double2Int32(t * 1000));
		}
		this->stm->Write(buff, 12);
		this->clk->Start();
		this->stmMut->Unlock();
	}
	return true;
}

Bool IO::MODBUSTCPMaster::ReadInputRegisters(UInt8 devAddr, UInt16 regAddr, UInt16 regCnt)
{
	UInt8 buff[12];
	WriteMInt16(buff, this->tranId);
	this->tranId++;
	buff[2] = 0;
	buff[3] = 0;
	buff[4] = 0;
	buff[5] = 6;
	buff[6] = devAddr;
	buff[7] = 4;
	WriteMInt16(&buff[8], regAddr);
	WriteMInt16(&buff[10], regCnt);
	if (this->stm)
	{
		this->stmMut->Lock();
		Double t = this->clk->GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::Thread::Sleep(CMDDELAY - Math::Double2Int32(t * 1000));
		}
		this->stm->Write(buff, 12);
		this->clk->Start();
		this->stmMut->Unlock();
	}
	return true;
}

Bool IO::MODBUSTCPMaster::WriteCoil(UInt8 devAddr, UInt16 coilAddr, Bool isHigh)
{
	UInt8 buff[12];
	WriteMInt16(buff, this->tranId);
	this->tranId++;
	buff[2] = 0;
	buff[3] = 0;
	buff[4] = 0;
	buff[5] = 6;
	buff[6] = devAddr;
	buff[7] = 5;
	WriteMInt16(&buff[8], coilAddr);
	if (isHigh)
	{
		WriteMInt16(&buff[10], 0xff00);
	}
	else
	{
		WriteMInt16(&buff[10], 0);
	}	
	if (this->stm)
	{
		this->stmMut->Lock();
		Double t = this->clk->GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::Thread::Sleep(CMDDELAY - Math::Double2Int32(t * 1000));
		}
		this->stm->Write(buff, 12);
		this->clk->Start();
		this->stmMut->Unlock();
	}
	return true;
}

Bool IO::MODBUSTCPMaster::WriteHoldingRegister(UInt8 devAddr, UInt16 regAddr, UInt16 val)
{
	UInt8 buff[12];
	WriteMInt16(buff, this->tranId);
	this->tranId++;
	buff[2] = 0;
	buff[3] = 0;
	buff[4] = 0;
	buff[5] = 6;
	buff[6] = devAddr;
	buff[7] = 6;
	WriteMInt16(&buff[8], regAddr);
	WriteMInt16(&buff[10], val);
	if (this->stm)
	{
		this->stmMut->Lock();
		Double t = this->clk->GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::Thread::Sleep(CMDDELAY - Math::Double2Int32(t * 1000));
		}
		this->stm->Write(buff, 12);
		this->clk->Start();
		this->stmMut->Unlock();
	}
	return true;
}

Bool IO::MODBUSTCPMaster::WriteHoldingRegisters(UInt8 devAddr, UInt16 regAddr, UInt16 cnt, UInt8 *val)
{
	UInt8 buff[256];
	WriteMInt16(buff, this->tranId);
	this->tranId++;
	buff[2] = 0;
	buff[3] = 0;
	WriteMInt16(&buff[4], 7 + cnt * 2);
	buff[4] = 0;
	buff[5] = 6;
	buff[6] = devAddr;
	buff[7] = 16;
	WriteMInt16(&buff[8], regAddr);
	WriteMInt16(&buff[10], cnt);
	buff[12] = (UInt8)(cnt << 1);
	MemCopyNO(&buff[13], val, cnt * 2);
	if (this->stm)
	{
		this->stmMut->Lock();
		Double t = this->clk->GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::Thread::Sleep(CMDDELAY - Math::Double2Int32(t * 1000));
		}
		this->stm->Write(buff, cnt * 2 + 13);
		this->clk->Start();
		this->stmMut->Unlock();
	}
	return true;
}

void IO::MODBUSTCPMaster::HandleReadResult(UInt8 addr, ReadResultFunc readFunc, SetResultFunc setFunc, void *userObj)
{
	AddrResultCb *cb = this->cbMap->Get(addr);
	if (cb)
	{
		cb->readFunc = readFunc;
		cb->setFunc = setFunc;
		cb->userObj = userObj;
	}
	else
	{
		cb = MemAlloc(AddrResultCb, 1);
		cb->readFunc = readFunc;
		cb->setFunc = setFunc;
		cb->userObj = userObj;
		this->cbMap->Put(addr, cb);
	}
}