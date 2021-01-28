#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/ATCommandChannel.h"
#include "IO/FileStream.h"
#include "Sync/Thread.h"
#include "Data/DateTime.h"

//#define DEBUG

UInt32 __stdcall IO::ATCommandChannel::CmdThread(void *userObj)
{
	IO::ATCommandChannel *me = (IO::ATCommandChannel*)userObj;
#if defined(DEBUG)
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"Received.dat", IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
#endif
	UInt8 readBuff[2048];
	OSInt readSize;
	OSInt buffSize = 0;
	OSInt i;
	OSInt cmdStart;
	Char *cmdResult;
	me->threadRunning = true;
	me->cmdEvt->Set();
	while (!me->threadToStop)
	{
		readSize = me->stm->Read(&readBuff[buffSize], 2048 - buffSize);
		if (readSize == 0)
		{
			Sync::Thread::Sleep(100);
		}
		else
		{
#if defined(DEBUG)
			fs->Write(&readBuff[buffSize], readSize);
#endif
			buffSize += readSize;
			i = 0;
			cmdStart = 0;
			while (i < buffSize)
			{
				if (readBuff[i] == 13 || readBuff[i] == 10)
				{
					if (cmdStart == i)
					{
						cmdStart++;
					}
					else
					{
						cmdResult = MemAlloc(Char, i - cmdStart + 1);
						MemCopyNO(cmdResult, &readBuff[cmdStart], i - cmdStart);
						cmdResult[i - cmdStart] = 0;
						if (me->log)
						{
							me->log->LogMessage((const UTF8Char*)cmdResult, IO::ILogHandler::LOG_LEVEL_RAW);
						}

						if (me->cmdHdlr && me->cmdHdlr(me->cmdHdlrObj, cmdResult))
						{
							MemFree(cmdResult);
						}
						else
						{
							me->cmdResultMut->Lock();
							me->cmdResults->Add(cmdResult);
							me->cmdResultMut->Unlock();
							me->cmdEvt->Set();
						}
						cmdStart = i + 1;
					}
				}
				else if (readBuff[i] == 16 && cmdStart == i)
				{
					i++;
					if (i >= buffSize)
						break;
					if (me->evtHdlr)
					{
						me->evtHdlr(me->evtHdlrObj, readBuff[i]);
					}
					cmdStart = i + 1;
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
#if defined(DEBUG)
	DEL_CLASS(fs);
#endif
	me->cmdEvt->Set();
	me->threadRunning = false;
	return 0;
}

void IO::ATCommandChannel::ClearResults()
{
	const Char *cmdRes;
	while (this->cmdResults->GetCount() > 0)
	{
		this->cmdResultMut->Lock();
		cmdRes = this->cmdResults->RemoveAt(0);
		this->cmdResultMut->Unlock();
		MemFree((void*)cmdRes);
	}
}

IO::ATCommandChannel::ATCommandChannel(IO::Stream *stm, Bool needRelease)
{
	this->stm = stm;
	this->stmRelease = needRelease;
	NEW_CLASS(this->cmdMut, Sync::Mutex());
	NEW_CLASS(this->cmdEvt, Sync::Event(true, (const UTF8Char*)"IO.ATCommandChannel.cmdEvt"));
	NEW_CLASS(this->cmdResultMut, Sync::Mutex());
	NEW_CLASS(this->cmdResults, Data::ArrayList<const Char *>());
	this->evtHdlr = 0;
	this->evtHdlrObj = 0;
	this->cmdHdlr = 0;
	this->cmdHdlrObj = 0;
	this->log = 0;

	this->threadRunning = false;
	this->threadToStop = false;
	Sync::Thread::Create(CmdThread, this);
	while (!this->threadRunning)
	{
		this->cmdEvt->Wait(100);
	}
}

IO::ATCommandChannel::~ATCommandChannel()
{
	OSInt i;
	this->Close();
	i = this->cmdResults->GetCount();
	while (i-- > 0)
	{
		MemFree((void*)this->cmdResults->RemoveAt(i));
	}
	DEL_CLASS(this->cmdResults);
	DEL_CLASS(this->cmdResultMut);
	DEL_CLASS(this->cmdEvt);
	DEL_CLASS(this->cmdMut);
	if (this->stmRelease)
	{
		DEL_CLASS(this->stm);
	}
}

IO::Stream *IO::ATCommandChannel::GetStream()
{
	return this->stm;
}

OSInt IO::ATCommandChannel::SendATCommand(Data::ArrayList<const Char *> *retArr, const Char *atCmd, Int32 timeoutMS)
{
	Data::DateTime dt;
	Data::DateTime dt2;
	OSInt i = Text::StrCharCnt(atCmd);
	OSInt retSize = 0;
//	Bool cmdBegin = false;
	Bool cmdEnd = false;
	const Char *cmdRes;
	if (!this->CmdBegin())
		return 0;
	this->CmdSend((UInt8*)atCmd, i);
	this->CmdSend((UInt8*)"\r", 1);
	
	dt.SetCurrTimeUTC();
	while (true)
	{
		while ((cmdRes = this->CmdGetNextResult(1000)) != 0)
		{
			dt.SetCurrTimeUTC();
			retArr->Add(cmdRes);
			retSize++;
			if (Text::StrCompare(cmdRes, "OK") == 0)
			{
				cmdEnd = true;
				break;
			}
			if (Text::StrCompare(cmdRes, "ERROR") == 0)
			{
				cmdEnd = true;
				break;
			}
			if (Text::StrStartsWith(cmdRes, "+CME ERROR"))
			{
				cmdEnd = true;
				break;
			}
		}
		if (cmdEnd || !this->threadRunning)
			break;
		dt2.SetCurrTimeUTC();
		if (dt2.DiffMS(&dt) >= timeoutMS)
			break;
	}

	this->CmdEnd();
	return retSize;
}

OSInt IO::ATCommandChannel::SendATCommands(Data::ArrayList<const Char *> *retArr, const Char *atCmd, const Char *atCmdSub, Int32 timeoutMS)
{
	Data::DateTime dt;
	Data::DateTime dt2;
	OSInt i = Text::StrCharCnt(atCmd);
	OSInt retSize = 0;
//	Bool cmdBegin = false;
	Bool cmdEnd = false;
	const Char *cmdRes;
	if (!this->CmdBegin())
		return 0;
	this->CmdSend((UInt8*)atCmd, i);
	this->CmdSend((UInt8*)"\r", 1);
	Sync::Thread::Sleep(1000);
	i = Text::StrCharCnt(atCmdSub);
	this->CmdSend((UInt8*)atCmdSub, i);
	this->CmdSend((UInt8*)"\x1a", 1);
	
	dt.SetCurrTimeUTC();
	while (true)
	{
		while ((cmdRes = this->CmdGetNextResult(1000)) != 0)
		{
			dt.SetCurrTimeUTC();
			retArr->Add(cmdRes);
			retSize++;
			if (Text::StrCompare(cmdRes, "OK") == 0)
			{
				cmdEnd = true;
				break;
			}
			if (Text::StrCompare(cmdRes, "ERROR") == 0)
			{
				cmdEnd = true;
				break;
			}
			if (Text::StrStartsWith(cmdRes, "+CME ERROR"))
			{
				cmdEnd = true;
				break;
			}
		}
		if (cmdEnd || !this->threadRunning)
			break;
		dt2.SetCurrTimeUTC();
		if (dt2.DiffMS(&dt) >= timeoutMS)
			break;
	}

	this->CmdEnd();
	return retSize;
}

OSInt IO::ATCommandChannel::SendDialCommand(Data::ArrayList<const Char *> *retArr, const Char *atCmd, Int32 timeoutMS)
{
	Data::DateTime dt;
	Data::DateTime dt2;
	OSInt i = Text::StrCharCnt(atCmd);
	OSInt retSize = 0;
//	Bool cmdBegin = false;
	Bool cmdEnd = false;
	const Char *cmdRes;
	if (!this->CmdBegin())
		return 0;
	this->CmdSend((UInt8*)atCmd, i);
	this->CmdSend((UInt8*)"\r", 1);
	
	dt.SetCurrTimeUTC();
	while (true)
	{
		while ((cmdRes = this->CmdGetNextResult(1000)) != 0)
		{
			dt.SetCurrTimeUTC();
			retArr->Add(cmdRes);
			retSize++;
			if (Text::StrEquals(cmdRes, "NO DIALTONE"))
			{
				cmdEnd = true;
				break;
			}
			if (Text::StrEquals(cmdRes, "VCON"))
			{
				cmdEnd = true;
				break;
			}
			if (Text::StrEquals(cmdRes, "BUSY"))
			{
				cmdEnd = true;
				break;
			}
			if (Text::StrEquals(cmdRes, "NO CARRIER"))
			{
				cmdEnd = true;
				break;
			}
			if (Text::StrEquals(cmdRes, "OK"))
			{
				cmdEnd = true;
				break;
			}
			if (Text::StrEquals(cmdRes, "ERROR"))
			{
				cmdEnd = true;
				break;
			}
			if (Text::StrStartsWith(cmdRes, "+CME ERROR"))
			{
				cmdEnd = true;
				break;
			}
		}
		if (cmdEnd || !this->threadRunning)
			break;
		dt2.SetCurrTimeUTC();
		if (dt2.DiffMS(&dt) >= timeoutMS)
			break;
	}

	this->CmdEnd();
	return retSize;
}

Bool IO::ATCommandChannel::CmdBegin()
{
	if (!this->threadRunning)
		return false;
	this->cmdMut->Lock();
	this->ClearResults();
	return true;
}

void IO::ATCommandChannel::CmdEnd()
{
	this->cmdMut->Unlock();
}

OSInt IO::ATCommandChannel::CmdSend(const UInt8 *data, OSInt dataSize)
{
	return this->stm->Write(data, dataSize);
}

const Char *IO::ATCommandChannel::CmdGetNextResult(Int32 timeoutMS)
{
	const Char *cmdRes = 0;
	this->cmdEvt->Clear();
	if (this->cmdResults->GetCount() > 0)
	{
		this->cmdResultMut->Lock();
		cmdRes = this->cmdResults->RemoveAt(0);
		this->cmdResultMut->Unlock();
		return cmdRes;
	}
	this->cmdEvt->Wait(timeoutMS);
	if (this->cmdResults->GetCount() > 0)
	{
		this->cmdResultMut->Lock();
		cmdRes = this->cmdResults->RemoveAt(0);
		this->cmdResultMut->Unlock();
		return cmdRes;
	}
	return 0;
}

void IO::ATCommandChannel::Close()
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

void IO::ATCommandChannel::SetEventHandler(EventHandler evtHdlr, void *userObj)
{
	this->evtHdlrObj = userObj;
	this->evtHdlr = evtHdlr;
}

void IO::ATCommandChannel::SetCommandHandler(CommandHandler cmdHdlr, void *userObj)
{
	this->cmdHdlrObj = userObj;
	this->cmdHdlr = cmdHdlr;
}

void IO::ATCommandChannel::SetLogger(IO::ILogger *log)
{
	this->log = log;
}
