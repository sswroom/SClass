#include "Stdafx.h"
#include "Crypto/Hash/CRC16.h"
#include "Data/ByteTool.h"
#include "IO/MODBUSRTUMaster.h"
#include "Math/Math.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"
#include <stdio.h>
#define CMDDELAY 200

UInt32 __stdcall IO::MODBUSRTUMaster::ThreadProc(AnyType userObj)
{
	NN<IO::MODBUSRTUMaster> me = userObj.GetNN<IO::MODBUSRTUMaster>();
	UInt8 buff[1024];
	UOSInt buffSize = 0;
	UOSInt readSize;
	UOSInt i;
	Bool incomplete;
	NN<AddrResultCb> cb;
	me->threadRunning = true;
	{
	//	Text::StringBuilderUTF8 sb;
		while (!me->threadToStop)
		{
			readSize = me->stm->Read(Data::ByteArray(&buff[buffSize], 1024 - buffSize));
	//		printf("ReadSize: %d\r\n", readSize);
			if (readSize > 0)
			{
	//			sb.ClearStr();
	//			sb.AppendHexBuff(&buff[buffSize], readSize, ' ', Text::LineBreakType::CRLF);
	//			printf("Received: %s\r\n", sb.ToString());
				buffSize += readSize;
				incomplete = false;
				i = 0;
				while (i < buffSize - 1)
				{
					switch (buff[i + 1])
					{
					case 1:
					case 2:
					case 3:
					case 4:
						if (i + 5 <= buffSize)
						{
							if (i + 5 + buff[i + 2] <= buffSize)
							{
								if (me->IsCRCValid(&buff[i], 5 + (UOSInt)buff[i + 2]))
								{
									if (me->cbMap.Get(buff[i]).SetTo(cb) && cb->readFunc)
									{
										cb->readFunc(cb->userObj, buff[i + 1], &buff[i + 3], buff[i + 2]);
									}
									i += 5 + (UOSInt)buff[i + 2];
								}
								else
								{
									printf("CRC Invalid\r\n");
									i++;
								}
							}
							else
							{
								incomplete = true;
							}
						}
						else
						{
							incomplete = true;
						}
						break;
					case 5:
					case 6:
					case 15:
					case 16:
						if (i + 8 <= buffSize)
						{
							if (me->IsCRCValid(&buff[i], 8))
							{
								if (me->cbMap.Get(buff[i]).SetTo(cb) && cb->setFunc)
								{
									cb->setFunc(cb->userObj, buff[i + 1], ReadMUInt16(&buff[i + 2]), ReadMUInt16(&buff[i + 4]));
								}
								i += 8;
							}
							else
							{
								printf("CRC Invalid\r\n");
								i++;
							}
						}
						else
						{
							incomplete = true;
						}
						break;
					default:
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
				Sync::SimpleThread::Sleep(100);
			}
			
		}
	//	DEL_CLASS(sb);
	}
	me->threadRunning = false;
	return 0;
}

void IO::MODBUSRTUMaster::CalcCRC(UInt8 *rtu, UOSInt rtuSize)
{
	UInt8 crcTmp[2];
	Sync::MutexUsage mutUsage(this->crcMut);
	this->crc->Clear();
	this->crc->Calc(rtu, rtuSize - 2);
	this->crc->GetValue(crcTmp);
	mutUsage.EndUse();
	rtu[rtuSize - 2] = (UInt8)~crcTmp[1];
	rtu[rtuSize - 1] = (UInt8)~crcTmp[0];
}

Bool IO::MODBUSRTUMaster::IsCRCValid(UInt8 *rtu, UOSInt rtuSize)
{
	UInt8 crcTmp[2];
	Sync::MutexUsage mutUsage(this->crcMut);
	this->crc->Clear();
	this->crc->Calc(rtu, rtuSize - 2);
	this->crc->GetValue(crcTmp);
	mutUsage.EndUse();
	return rtu[rtuSize - 2] == (UInt8)~crcTmp[1] && rtu[rtuSize - 1] == (UInt8)~crcTmp[0];
}

IO::MODBUSRTUMaster::MODBUSRTUMaster(IO::Stream *stm)
{
	this->stm = stm;
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->crc, Crypto::Hash::CRC16R(Crypto::Hash::CRC16::GetPolynomialANSI()));
	if (this->stm)
	{
		Sync::ThreadUtil::Create(ThreadProc, this);
		while (!this->threadRunning)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}
}

IO::MODBUSRTUMaster::~MODBUSRTUMaster()
{
	UOSInt i;
	if (this->stm)
	{
		this->threadToStop = true;
		this->stm->Close();
		while (this->threadRunning)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}
	NN<AddrResultCb> cb;
	i = this->cbMap.GetCount();
	while (i-- > 0)
	{
		cb = this->cbMap.GetItemNoCheck(i);
		MemFreeNN(cb);
	}
	DEL_CLASS(this->crc);
}

Bool IO::MODBUSRTUMaster::ReadCoils(UInt8 devAddr, UInt16 coilAddr, UInt16 coilCnt)
{
	UInt8 buff[8];
	buff[0] = devAddr;
	buff[1] = 1;
	WriteMInt16(&buff[2], coilAddr);
	WriteMInt16(&buff[4], coilCnt);
	this->CalcCRC(buff, 8);
	if (this->stm)
	{
		Sync::MutexUsage mutUsage(this->stmMut);
		Double t = this->clk.GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::SimpleThread::Sleep((UOSInt)(CMDDELAY - Double2Int32(t * 1000)));
		}
		this->stm->Write(buff, 8);
		this->clk.Start();
		mutUsage.EndUse();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(buff, 8, ' ', Text::LineBreakType::None);
		printf("Calculate:%s\r\n", sb.ToPtr());
	}
	return true;
}

Bool IO::MODBUSRTUMaster::ReadInputs(UInt8 devAddr, UInt16 inputAddr, UInt16 inputCnt)
{
	UInt8 buff[8];
	buff[0] = devAddr;
	buff[1] = 2;
	WriteMInt16(&buff[2], inputAddr);
	WriteMInt16(&buff[4], inputCnt);
	this->CalcCRC(buff, 8);
	if (this->stm)
	{
		Sync::MutexUsage mutUsage(this->stmMut);
		Double t = this->clk.GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::SimpleThread::Sleep((UOSInt)(CMDDELAY - Double2Int32(t * 1000)));
		}
		this->stm->Write(buff, 8);
		this->clk.Start();
		mutUsage.EndUse();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(buff, 8, ' ', Text::LineBreakType::None);
		printf("Calculate:%s\r\n", sb.ToPtr());
	}
	return true;
}

Bool IO::MODBUSRTUMaster::ReadHoldingRegisters(UInt8 devAddr, UInt16 regAddr, UInt16 regCnt)
{
	UInt8 buff[8];
	buff[0] = devAddr;
	buff[1] = 3;
	WriteMInt16(&buff[2], regAddr);
	WriteMInt16(&buff[4], regCnt);
	this->CalcCRC(buff, 8);
	if (this->stm)
	{
		Sync::MutexUsage mutUsage(this->stmMut);
		Double t = this->clk.GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::SimpleThread::Sleep((UOSInt)(CMDDELAY - Double2Int32(t * 1000)));
		}
		this->stm->Write(buff, 8);
		this->clk.Start();
		mutUsage.EndUse();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(buff, 8, ' ', Text::LineBreakType::None);
		printf("Calculate:%s\r\n", sb.ToPtr());
	}
	return true;
}

Bool IO::MODBUSRTUMaster::ReadInputRegisters(UInt8 devAddr, UInt16 regAddr, UInt16 regCnt)
{
	UInt8 buff[8];
	buff[0] = devAddr;
	buff[1] = 4;
	WriteMInt16(&buff[2], regAddr);
	WriteMInt16(&buff[4], regCnt);
	this->CalcCRC(buff, 8);
	if (this->stm)
	{
		Sync::MutexUsage mutUsage(this->stmMut);
		Double t = this->clk.GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::SimpleThread::Sleep((UOSInt)(CMDDELAY - Double2Int32(t * 1000)));
		}
		this->stm->Write(buff, 8);
		this->clk.Start();
		mutUsage.EndUse();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(buff, 8, ' ', Text::LineBreakType::None);
		printf("Calculate:%s\r\n", sb.ToPtr());
	}
	return true;
}

Bool IO::MODBUSRTUMaster::WriteCoil(UInt8 devAddr, UInt16 coilAddr, Bool isHigh)
{
	UInt8 buff[8];
	buff[0] = devAddr;
	buff[1] = 5;
	WriteMInt16(&buff[2], coilAddr);
	if (isHigh)
	{
		WriteMInt16(&buff[4], 0xff00);
	}
	else
	{
		WriteMInt16(&buff[4], 0);
	}	
	this->CalcCRC(buff, 8);
	if (this->stm)
	{
		Sync::MutexUsage mutUsage(this->stmMut);
		Double t = this->clk.GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::SimpleThread::Sleep((UOSInt)(CMDDELAY - Double2Int32(t * 1000)));
		}
		this->stm->Write(buff, 8);
		this->clk.Start();
		mutUsage.EndUse();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(buff, 8, ' ', Text::LineBreakType::None);
		printf("Calculate:%s\r\n", sb.ToPtr());
	}
	return true;
}

Bool IO::MODBUSRTUMaster::WriteHoldingRegister(UInt8 devAddr, UInt16 regAddr, UInt16 val)
{
	UInt8 buff[8];
	buff[0] = devAddr;
	buff[1] = 6;
	WriteMInt16(&buff[2], regAddr);
	WriteMInt16(&buff[4], val);
	this->CalcCRC(buff, 8);
	if (this->stm)
	{
		Sync::MutexUsage mutUsage(this->stmMut);
		Double t = this->clk.GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::SimpleThread::Sleep((UOSInt)(CMDDELAY - Double2Int32(t * 1000)));
		}
//		Text::StringBuilderUTF8 sb;
//		sb.AppendHexBuff(buff, 8, ' ', Text::LineBreakType::None);
//		printf("Send: %s\r\n", sb.ToString());

		this->stm->Write(buff, 8);
		this->clk.Start();
		mutUsage.EndUse();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(buff, 8, ' ', Text::LineBreakType::None);
		printf("Calculate:%s\r\n", sb.ToPtr());
	}
	return true;
}

Bool IO::MODBUSRTUMaster::WriteHoldingRegisters(UInt8 devAddr, UInt16 regAddr, UInt16 cnt, UInt8 *val)
{
	UInt8 buff[256];
	buff[0] = devAddr;
	buff[1] = 16;
	WriteMInt16(&buff[2], regAddr);
	WriteMUInt16(&buff[4], cnt);
	buff[6] = (UInt8)(cnt << 1);
	MemCopyNO(&buff[7], val, (UOSInt)cnt * 2);
	this->CalcCRC(buff, (UOSInt)cnt * 2 + 9);
	if (this->stm)
	{
		Sync::MutexUsage mutUsage(this->stmMut);
		Double t = this->clk.GetTimeDiff();
		if (t < CMDDELAY * 0.001)
		{
			Sync::SimpleThread::Sleep((UOSInt)(CMDDELAY - Double2Int32(t * 1000)));
		}
//		Text::StringBuilderUTF8 sb;
//		sb.AppendHexBuff(buff, cnt * 2 + 9, ' ', Text::LineBreakType::None);
//		printf("Send: %s\r\n", sb.ToString());

		this->stm->Write(buff, (UOSInt)cnt * 2 + 9);
		this->clk.Start();
		mutUsage.EndUse();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(buff, (UOSInt)cnt * 2 + 9, ' ', Text::LineBreakType::None);
		printf("Calculate:%s\r\n", sb.ToPtr());
	}
	return true;
}

void IO::MODBUSRTUMaster::HandleReadResult(UInt8 addr, ReadResultFunc readFunc, SetResultFunc setFunc, AnyType userObj)
{
	NN<AddrResultCb> cb;
	if (this->cbMap.Get(addr).SetTo(cb))
	{
		cb->readFunc = readFunc;
		cb->setFunc = setFunc;
		cb->userObj = userObj;
	}
	else
	{
		cb = MemAllocNN(AddrResultCb);
		cb->readFunc = readFunc;
		cb->setFunc = setFunc;
		cb->userObj = userObj;
		this->cbMap.Put(addr, cb);
	}
}
