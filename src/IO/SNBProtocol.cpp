#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SNBProtocol.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall IO::SNBProtocol::RecvThread(void *userObj)
{
	IO::SNBProtocol *me = (IO::SNBProtocol*)userObj;
	UInt8 *recvBuff;
	UOSInt buffSize;
	UOSInt recvSize;
	UOSInt i;
	UOSInt j;
	UInt8 chk;
	me->running = true;
	recvBuff = MemAlloc(UInt8, 4096);
	buffSize = 0;
	while (!me->toStop)
	{
		recvSize = me->stm->Read(Data::ByteArray(&recvBuff[buffSize], 4096 - buffSize));
		if (recvSize <= 0)
		{
			Sync::SimpleThread::Sleep(100);
		}
		else
		{
			buffSize += recvSize;
			i = 0;
			while (i < buffSize - 5)
			{
				if (recvBuff[i] == 1 && recvBuff[i + 1] == 2 && recvBuff[i + 2] == 1)
				{
					recvSize = recvBuff[i + 3];
					if (recvSize + i + 5 > buffSize)
						break;
					j = recvSize + i + 4;
					chk = 0;
					while (j-- > i)
					{
						chk = (UInt8)((chk + recvBuff[j]) & 0xff);
					}
					if (chk == recvBuff[recvSize + i + 4])
					{
						me->protoHdlr(me->protoObj, recvBuff[i + 4], recvSize - 1, &recvBuff[i + 5]);
						i += recvSize + 4;
					}
				}
				i++;
			}
			if (i >= buffSize)
			{
				buffSize = 0;
			}
			else if (i > 0)
			{
				MemCopyO(recvBuff, &recvBuff[i], buffSize - i);
				buffSize -= i;
			}
		}
	}
	MemFree(recvBuff);
	me->running = false;
	return 0;
}

IO::SNBProtocol::SNBProtocol(NotNullPtr<IO::Stream> stm, ProtocolHandler protoHdlr, void *userObj)
{
	this->stm = stm;
	this->protoHdlr = protoHdlr;
	this->protoObj = userObj;
	this->running = false;
	this->toStop = false;

	Sync::ThreadUtil::Create(RecvThread, this);
	while (!this->running)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

IO::SNBProtocol::~SNBProtocol()
{
	this->toStop = true;
	this->stm->Close();
	while (this->running)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->stm.Delete();
}

void IO::SNBProtocol::SendCommand(UInt8 cmdType, UOSInt cmdSize, UInt8 *cmd)
{
	UInt8 buff[262];
	UInt8 chk;
	UOSInt i;
	UOSInt j;
	buff[0] = 0;
	buff[1] = 2;
	buff[2] = 1;
	buff[3] = (UInt8)(cmdSize + 1);
	buff[4] = cmdType;
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[5], cmd, cmdSize);
	}
	chk = 0;
	i = 0;
	j = 5 + cmdSize;
	while (i < j)
	{
		chk = (UInt8)((chk + buff[i]) & 0xff);
		i++;
	}
	buff[j] = chk;
	this->stm->Write(buff, j + 1);
}
