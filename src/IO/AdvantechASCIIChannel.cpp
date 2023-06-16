#include "Stdafx.h"
#include "IO/AdvantechASCIIChannel.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

#define CMDTIMEOUT 100

UInt32 __stdcall IO::AdvantechASCIIChannel::CmdThread(void *userObj)
{
	IO::AdvantechASCIIChannel *me = (IO::AdvantechASCIIChannel*)userObj;
	UInt8 readBuff[64];
	UOSInt readSize;
	UOSInt buffSize = 0;
	UOSInt i;
	UOSInt cmdStart;
	me->threadRunning = true;
	me->cmdEvt.Set();
	while (!me->threadToStop)
	{
		readSize = me->stm->Read(&readBuff[buffSize], 64 - buffSize);
		if (readSize == 0)
		{
			Sync::SimpleThread::Sleep(100);
		}
		else
		{
			buffSize += readSize;
			i = 0;
			cmdStart = 0;
			while (i < buffSize)
			{
				if (readBuff[i] == 13)
				{
					if (cmdStart == i)
					{
						cmdStart++;
					}
					else
					{
						{
							Sync::MutexUsage mutUsage(&me->cmdResMut);
							if (me->cmdResBuff)
							{
								me->cmdResEnd = Text::StrConcatC(me->cmdResBuff, &readBuff[cmdStart], i - cmdStart);
								me->cmdResBuff = 0;
								me->cmdEvt.Set();
							}
						}
						cmdStart = i + 1;
					}
				}
				i++;
			}
			if (cmdStart >= buffSize)
			{
				buffSize = 0;
			}
			else
			{
				MemCopyO(readBuff, &readBuff[cmdStart], buffSize - cmdStart);
				buffSize -= cmdStart;
			}
		}
	}
	me->cmdEvt.Set();
	me->threadRunning = false;
	return 0;
}

IO::AdvantechASCIIChannel::AdvantechASCIIChannel(IO::Stream *stm, Bool needRelease)
{
	this->stm = stm;
	this->stmRelease = needRelease;
	this->threadRunning = false;
	this->threadToStop = false;
	Sync::ThreadUtil::Create(CmdThread, this);
	while (!this->threadRunning)
	{
		this->cmdEvt.Wait(100);
	}
}

IO::AdvantechASCIIChannel::~AdvantechASCIIChannel()
{
	this->Close();
	if (this->stmRelease)
	{
		DEL_CLASS(this->stm);
	}
}

IO::Stream *IO::AdvantechASCIIChannel::GetStream()
{
	return this->stm;
}

UTF8Char *IO::AdvantechASCIIChannel::SendCommand(UTF8Char *replyBuff, const UTF8Char *cmd, UOSInt cmdLen)
{
	if (!this->threadRunning)
		return 0;
	Text::StrConcatC(replyBuff, cmd, cmdLen);
	replyBuff[cmdLen] = 13;
	Sync::MutexUsage mutUsage(&this->cmdResMut);
	this->stm->Write(replyBuff, cmdLen + 1);
	this->cmdResBuff = replyBuff;
	this->cmdResEnd = 0;
	mutUsage.EndUse();
	this->cmdEvt.Wait(CMDTIMEOUT);
	mutUsage.BeginUse();
	replyBuff = this->cmdResEnd;
	this->cmdResBuff = 0;
	this->cmdResEnd = 0;
	return replyBuff;
}

void IO::AdvantechASCIIChannel::Close()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		this->stm->Close();
		while (this->threadRunning)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}
}

UTF8Char *IO::AdvantechASCIIChannel::GetFirmwareVer(UTF8Char *firmwareBuff, UInt8 addr)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = 'F';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!')
	{
		return Text::StrConcatC(firmwareBuff, sbuff + 3, (UOSInt)(sptr - sbuff - 3));
	}
	return 0;
}

UTF8Char *IO::AdvantechASCIIChannel::GetModuleName(UTF8Char *moduleBuff, UInt8 addr)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = 'M';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!')
	{
		return Text::StrConcatC(moduleBuff, sbuff + 3, (UOSInt)(sptr - sbuff - 3));
	}
	return 0;
}

Bool IO::AdvantechASCIIChannel::GetConfigStatus(UInt8 addr, ADAMConfig *config)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '2';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 9)
	{
		UInt8 flags = Text::StrHex2UInt8C(&sbuff[7]);
		config->addr = Text::StrHex2UInt8C(&sbuff[1]);
		config->typeCode = (TypeCode)Text::StrHex2UInt8C(&sbuff[3]);
		config->baudRateCode = (BaudRate)Text::StrHex2UInt8C(&sbuff[5]);
		config->checksumEnabled = ((flags & 0x40) != 0);
		if (config->typeCode == TC_DIGITAL)
		{
			config->modbusProtocol = ((flags & 4) != 0);
			config->intTime = IT_UNKNOWN;
			config->dataFormat = DF_UNKNOWN;
			config->slewRate = SR_UNKNWON;
		}
		else if (config->typeCode == TC_FREQUENCY || config->typeCode == TC_COUNTER)
		{
			config->modbusProtocol = false;
			config->intTime = IT_UNKNOWN;
			config->dataFormat = DF_UNKNOWN;
			config->slewRate = SR_UNKNWON;
		}
		else if (config->typeCode >= 0x30 && config->typeCode <= 0x32) //Analog output (ADAM-4021)
		{
			config->modbusProtocol = false;
			config->intTime = IT_UNKNOWN;
			config->dataFormat = (DataFormat)(flags & 0x3);
			config->slewRate = (SlewRate)((flags & 0x3C) >> 2);
		}
		else
		{
			config->modbusProtocol = false;
			config->intTime = (IntegrationTime)((flags & 0x80) >> 7);
			config->dataFormat = (DataFormat)(flags & 0x3);
			config->slewRate = SR_UNKNWON;
		}
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::StoreCurrInputs()
{
	if (!this->threadRunning)
		return false;
	return this->stm->Write((const UInt8*)"#**", 3) == 3;
}

Bool IO::AdvantechASCIIChannel::AnalogOGetResetStatus(UInt8 addr, Bool *hasReset)
{
	return DigitalGetResetStatus(addr, hasReset);
}

Bool IO::AdvantechASCIIChannel::DigitalSetConfig(UInt8 addr, UInt8 newAddr, BaudRate baudRate, Bool checksum, Bool modbus)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	UInt8 flags;
	*sptr++ = '%';
	sptr = Text::StrHexByte(sptr, addr);
	sptr = Text::StrHexByte(sptr, newAddr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("40"));
	sptr = Text::StrHexByte(sptr, (UInt8)baudRate);
	flags = (UInt8)((checksum?0x40:0) | (modbus?4:0));
	sptr = Text::StrHexByte(sptr, flags);
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 3)
	{
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::DigitalSetAllOutput8Ch(UInt8 addr, UInt8 outputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '#';
	sptr = Text::StrHexByte(sptr, addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("00"));
	sptr = Text::StrHexByte(sptr, outputs);
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '>' && (sptr - sbuff) == 1)
	{
		return true;
	}
	return false;	
}

Bool IO::AdvantechASCIIChannel::DigitalSetAllOutput16Ch(UInt8 addr, UInt16 outputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '#';
	sptr = Text::StrHexByte(sptr, addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("00"));
	sptr = Text::StrHexVal16(sptr, outputs);
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '>' && (sptr - sbuff) == 1)
	{
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::DigitalGetResetStatus(UInt8 addr, Bool *hasReset)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '5';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 4)
	{
		*hasReset = (sbuff[3] == '1');
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4050GetIOStatus(UInt8 addr, UInt16 *outputs, UInt16 *inputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '6';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 7)
	{
		*outputs = Text::StrHex2UInt8C(&sbuff[1]);
		*inputs = Text::StrHex2UInt8C(&sbuff[3]);
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4051GetIOStatus(UInt8 addr, UInt16 *outputs, UInt16 *inputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '6';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 7)
	{
		*outputs = 0;
		*inputs = Text::StrHex2UInt16C(&sbuff[1]);
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4052GetIOStatus(UInt8 addr, UInt16 *outputs, UInt16 *inputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '6';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 7)
	{
		*outputs = 0;
		*inputs = Text::StrHex2UInt8C(&sbuff[1]);
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4053GetIOStatus(UInt8 addr, UInt16 *outputs, UInt16 *inputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '6';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 7)
	{
		*outputs = 0;
		*inputs = Text::StrHex2UInt16C(&sbuff[1]);
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4055GetIOStatus(UInt8 addr, UInt16 *outputs, UInt16 *inputs)
{
	return this->ADAM4050GetIOStatus(addr, outputs, inputs);
}

Bool IO::AdvantechASCIIChannel::ADAM4056GetIOStatus(UInt8 addr, UInt16 *outputs, UInt16 *inputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '6';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 5)
	{
		*outputs = Text::StrHex2UInt8C(&sbuff[1]);
		*inputs = 0;
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4060GetIOStatus(UInt8 addr, UInt16 *outputs, UInt16 *inputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '6';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 7)
	{
		*outputs = Text::StrHex2UInt8C(&sbuff[1]);
		*inputs = 0;
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4068GetIOStatus(UInt8 addr, UInt16 *outputs, UInt16 *inputs)
{
	return this->ADAM4060GetIOStatus(addr, outputs, inputs);
}

Bool IO::AdvantechASCIIChannel::ADAM4050GetStoredIO(UInt8 addr, Bool *firstRead, UInt16 *outputs, UInt16 *inputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '4';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 9)
	{
		*firstRead = Text::StrHex2UInt8C(&sbuff[1]) != 0;
		*outputs = Text::StrHex2UInt8C(&sbuff[3]);
		*inputs = Text::StrHex2UInt8C(&sbuff[5]);
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4051GetStoredIO(UInt8 addr, Bool *firstRead, UInt16 *outputs, UInt16 *inputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '4';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 7)
	{
		*firstRead = Text::StrHex2UInt8C(&sbuff[1]) != 0;
		*outputs = 0;
		*inputs = Text::StrHex2UInt8C(&sbuff[3]);
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4052GetStoredIO(UInt8 addr, Bool *firstRead, UInt16 *outputs, UInt16 *inputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '4';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 9)
	{
		*firstRead = Text::StrHex2UInt8C(&sbuff[1]) != 0;
		*outputs = 0;
		*inputs = Text::StrHex2UInt8C(&sbuff[3]);
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4053GetStoredIO(UInt8 addr, Bool *firstRead, UInt16 *outputs, UInt16 *inputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '4';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 9)
	{
		*firstRead = Text::StrHex2UInt8C(&sbuff[1]) != 0;
		*outputs = 0;
		*inputs = Text::StrHex2UInt16C(&sbuff[3]);
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4060GetStoredIO(UInt8 addr, Bool *firstRead, UInt16 *outputs, UInt16 *inputs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '4';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 9)
	{
		*firstRead = Text::StrHex2UInt8C(&sbuff[1]) != 0;
		*outputs = Text::StrHex2UInt8C(&sbuff[3]);
		*inputs = 0;
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4068GetStoredIO(UInt8 addr, Bool *firstRead, UInt16 *outputs, UInt16 *inputs)
{
	return this->ADAM4060GetStoredIO(addr, firstRead, outputs, inputs);
}

Bool IO::AdvantechASCIIChannel::ADAM4080SetConfig(UInt8 addr, UInt8 newAddr, Bool frequency, BaudRate baudRate, Bool checksum, FreqGateTime freqGateTime)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	UInt8 flags;
	*sptr++ = '%';
	sptr = Text::StrHexByte(sptr, addr);
	sptr = Text::StrHexByte(sptr, newAddr);
	if (frequency)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("51"));
	}
	else
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("50"));
	}
	sptr = Text::StrHexByte(sptr, (UInt8)baudRate);
	flags = (UInt8)((checksum?0x40:0) | ((freqGateTime == FGT_1S)?4:0));
	sptr = Text::StrHexByte(sptr, flags);
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 3)
	{
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4080GetInputMode(UInt8 addr, UInt8 *inputMode)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = 'B';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 4)
	{
		*inputMode = (UInt8)(sbuff[3] - '0');
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4080SetInputMode(UInt8 addr, UInt8 inputMode)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = 'B';
	*sptr++ = (UInt8)('0' + inputMode);
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 3)
	{
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4080GetValue(UInt8 addr, UInt8 channel, UInt32 *value)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '#';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = (UInt8)('0' + channel);
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 9)
	{
		*value = Text::StrHex2UInt32C(&sbuff[1]);
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4080SetGateMode(UInt8 addr, GateMode gateMode)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '#';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = 'A';
	*sptr++ = (UInt8)('0' + (UInt8)gateMode);
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 3)
	{
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4080GetGateMode(UInt8 addr, GateMode *gateMode)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '#';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = 'A';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 4)
	{
		*gateMode = (GateMode)(sbuff[3] - '0');
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4080SetMaxCounter(UInt8 addr, UInt8 channel, UInt32 maxCounter)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '3';
	*sptr++ = (UInt8)('0' + channel);
	sptr = Text::StrHexVal32(sptr, maxCounter);
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 3)
	{
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4080GetMaxCounter(UInt8 addr, UInt8 channel, UInt32 *maxCounter)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '3';
	*sptr++ = (UInt8)('0' + channel);
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 11)
	{
		*maxCounter = Text::StrHex2UInt32C(&sbuff[3]);
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4080StartCounter(UInt8 addr, UInt8 channel)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '5';
	*sptr++ = (UInt8)('0' + channel);
	*sptr++ = '1';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 3)
	{
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4080StopCounter(UInt8 addr, UInt8 channel)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '5';
	*sptr++ = (UInt8)('0' + channel);
	*sptr++ = '0';
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 3)
	{
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4080CounterIsStarted(UInt8 addr, UInt8 channel, Bool *started)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '5';
	*sptr++ = (UInt8)('0' + channel);
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 4)
	{
		*started = (sbuff[3] == '1');
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4080ClearCounter(UInt8 addr, UInt8 channel)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '6';
	*sptr++ = (UInt8)('0' + channel);
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 3)
	{
		return true;
	}
	return false;
}

Bool IO::AdvantechASCIIChannel::ADAM4080CounterHasOverflow(UInt8 addr, UInt8 channel, Bool *overflow)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = sbuff;
	*sptr++ = '$';
	sptr = Text::StrHexByte(sptr, addr);
	*sptr++ = '7';
	*sptr++ = (UInt8)('0' + channel);
	sptr = this->SendCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff));
	if (sptr && sbuff[0] == '!' && (sptr - sbuff) == 4)
	{
		*overflow = (sbuff[3] == '1');
		return true;
	}
	return false;
}

UInt32 IO::AdvantechASCIIChannel::BaudRateGetBps(BaudRate baudRate)
{
	switch (baudRate)
	{
	case BR_1200:
		return 1200;
	case BR_2400:
		return 2400;
	case BR_4800:
		return 4800;
	case BR_9600:
		return 9600;
	case BR_19200:
		return 19200;
	case BR_38400:
		return 38400;
	case BR_57600:
		return 57600;
	case BR_115200:
		return 115200;
	default:
		return 0;
	}
}

Text::CString IO::AdvantechASCIIChannel::DataFormatGetName(DataFormat dataFormat)
{
	switch (dataFormat)
	{
	case DF_ENGINEERING_UNIT:
		return CSTR("Engineering units");
	case DF_PERCENT_FSR:
		return CSTR("% of FSR");
	case DF_TWOS_COMPLEMENT:
		return CSTR("two's complement of hexadecimal");
	case DF_OHMS:
		return CSTR("Ohms");
	case DF_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::AdvantechASCIIChannel::IntegrationTimeGetName(IntegrationTime intTime)
{
	switch (intTime)
	{
	case IT_50MS:
		return CSTR("50 ms (Operation under 60 Hz power)");
	case IT_60MS:
		return CSTR("60 ms (Operation under 50 Hz power)");
	case IT_UNKNOWN:
	default:
		return CSTR("Unknwon");
	}
}

Text::CString IO::AdvantechASCIIChannel::TypeCodeGetName(TypeCode typeCode)
{
	switch (typeCode)
	{
	case TC_15mV:
		return CSTR("+/-15mV");
	case TC_50mV:
		return CSTR("+/-50mV");
	case TC_100mV:
		return CSTR("+/-100mV");
	case TC_300mV:
		return CSTR("+/-300mV");
	case TC_1000mV:
		return CSTR("+/-1V");
	case TC_2500mV:
		return CSTR("+/-2.5V");
	case TC_20mA:
		return CSTR("+/-20mA");
	case TC_4_20mA:
		return CSTR("4-20mA");
	case TC_10V:
		return CSTR("+/-10V");
	case TC_5V:
		return CSTR("+/-5V");
	case TC_1V:
		return CSTR("+/-1V");
	case TC_500mV_4017:
		return CSTR("+/-500mV");
	case TC_150mV:
		return CSTR("+/-150mV");
	case TC_20mA_4017:
		return CSTR("+/-20mA");
	case TC_TYPE_J_THERMOCOUPLE:
		return CSTR("Type J Thermocouple 0 ~ 760 C");
	case TC_TYPE_K_THERMOCOUPLE:
		return CSTR("Type K Thermocouple 0 ~ 13700 C");
	case TC_TYPE_T_THERMOCOUPLE:
		return CSTR("Type T Thermocouple -100 ~ 400 C");
	case TC_TYPE_E_THERMOCOUPLE:
		return CSTR("Type E Thermocouple 0 ~ 1000 C");
	case TC_TYPE_R_THERMOCOUPLE:
		return CSTR("Type R Thermocouple 500 ~ 1750 C");
	case TC_TYPE_S_THERMOCOUPLE:
		return CSTR("Type S Thermocouple 500 ~ 1750 C");
	case TC_TYPE_B_THERMOCOUPLE:
		return CSTR("Type B Thermocouple 500 ~ 1800 C");
	case TC_TYPE_N_THERMOCOUPLE:
		return CSTR("Type N Thermocouple -200 ~ 1300 C");
	case TC_PLATINUM_100_IEC__50_150:
		return CSTR("Platinum 100 (IEC) -50 - 150 C");
	case TC_PLATINUM_100_IEC_0_100:
		return CSTR("Platinum 100 (IEC) 0 - 100 C");
	case TC_PLATINUM_100_IEC_0_200:
		return CSTR("Platinum 100 (IEC) 0 - 200 C");
	case TC_PLATINUM_100_IEC_0_400:
		return CSTR("Platinum 100 (IEC) 0 - 400 C");
	case TC_PLATINUM_100_IEC__200_200:
		return CSTR("Platinum 100 (IEC) -200 - 200 C");
	case TC_PLATINUM_100_JIS__50_150:
		return CSTR("Platinum 100 (JIS) -50 - 150 C");
	case TC_PLATINUM_100_JIS_0_100:
		return CSTR("Platinum 100 (JIS) 0 - 100 C");
	case TC_PLATINUM_100_JIS_0_200:
		return CSTR("Platinum 100 (JIS) 0 - 200 C");
	case TC_PLATINUM_100_JIS_0_400:
		return CSTR("Platinum 100 (JIS) 0 - 400 C");
	case TC_PLATINUM_100_JIS__200_200:
		return CSTR("Platinum 100 (JIS) -200 - 200 C");
	case TC_PLATINUM_1000:
		return CSTR("Platinum 1000 -40 - 160 C");
	case TC_BALCO_500:
		return CSTR("BALCO 500 -30 - 120 C");
	case TC_NI_604__80_100:
		return CSTR("Ni 604 -80 - 100 C");
	case TC_NI_604_0_100:
		return CSTR("Ni 604 0 - 100 C");
	case TC_0_20mA_4021:
		return CSTR("0 to 20mA");
	case TC_4_20mA_4021:
		return CSTR("4 to 20mA");
	case TC_0_10V_4021:
		return CSTR("0 to 10V");
	case TC_NI_508:
		return CSTR("Ni 508 -50 - 200 C");
	case TC_DIGITAL:
		return CSTR("Digital Input");
	case TC_0_100mV:
		return CSTR("0 - 100mV");
	case TC_0_500mV:
		return CSTR("0 - 500mV");
	case TC_0_1000mV:
		return CSTR("0 - 1V");
	case TC_0_2500mV:
		return CSTR("0 - 2.5V");
	case TC_0_10V:
		return CSTR("0 - 10V");
	case TC_0_5V:
		return CSTR("0 - 5V");
	case TC_0_1000mV_4017:
		return CSTR("0 - 1V");
	case TC_0_500mV_4017:
		return CSTR("0 - 500mV");
	case TC_0_150mV:
		return CSTR("0 - 150mV");
	case TC_0_20mA:
		return CSTR("0 - 20mA");
	case TC_COUNTER:
		return CSTR("Counter input mode");
	case TC_FREQUENCY:
		return CSTR("Frequency measurement mode");
	default:
		return CSTR("Unknown");
	}
}

Text::CString IO::AdvantechASCIIChannel::SlewRateGetName(SlewRate slewRate)
{
	switch (slewRate)
	{
	case SR_IMMEDIATE:
		return CSTR("Immediate Change");
	case SR_0_0625V_sec:
		return CSTR("0.0625V/sec 0.125mA/sec");
	case SR_0_125V_sec:
		return CSTR("0.125V/sec 0.250mA/sec");
	case SR_0_25V_sec:
		return CSTR("0.250V/sec 0.500mA/sec");
	case SR_0_5V_sec:
		return CSTR("0.500V/sec 1.000mA/sec");
	case SR_1V_sec:
		return CSTR("1.000V/sec 2.000mA/sec");
	case SR_2V_sec:
		return CSTR("2.000V/sec 4.000mA/sec");
	case SR_4V_sec:
		return CSTR("4.000V/sec 8.000mA/sec");
	case SR_8V_sec:
		return CSTR("8.000V/sec 16.000mA/sec");
	case SR_16V_sec:
		return CSTR("16.000V/sec 32.000mA/sec");
	case SR_32V_sec:
		return CSTR("32.000V/sec 64.000mA/sec");
	case SR_64V_sec:
		return CSTR("64.000V/sec 128.000mA/sec");
	case SR_UNKNWON:
	default:
		return CSTR("Unknown");
	}
}
