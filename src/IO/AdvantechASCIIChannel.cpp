#include "Stdafx.h"
#include "IO/AdvantechASCIIChannel.h"
#include "Sync/Thread.h"

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
			Sync::Thread::Sleep(100);
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
	Sync::Thread::Create(CmdThread, this);
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
			Sync::Thread::Sleep(10);
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
