#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/ATCommandChannel.h"
#include "IO/FileStream.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

//#define DEBUG

UInt32 __stdcall IO::ATCommandChannel::CmdThread(AnyType userObj)
{
	NN<IO::ATCommandChannel> me = userObj.GetNN<IO::ATCommandChannel>();
#if defined(DEBUG)
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(CSTR("Received.dat"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
#endif
	UInt8 readBuff[2048];
	UOSInt readSize;
	UOSInt buffSize = 0;
	UOSInt i;
	UOSInt cmdStart;
	NN<Text::String> cmdResult;
	me->threadRunning = true;
	me->cmdEvt.Set();
	while (!me->threadToStop)
	{
		readSize = me->stm->Read(Data::ByteArray(&readBuff[buffSize], 2048 - buffSize));
		if (readSize == 0)
		{
			Sync::SimpleThread::Sleep(100);
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
						cmdResult = Text::String::New(i - cmdStart);
						MemCopyNO(cmdResult->v.Ptr(), &readBuff[cmdStart], i - cmdStart);
						cmdResult->v[i - cmdStart] = 0;
						if (me->log)
						{
							me->log->LogMessage({cmdResult->v, (i - cmdStart)}, IO::LogHandler::LogLevel::Raw);
						}

						if (me->cmdHdlr && me->cmdHdlr(me->cmdHdlrObj, cmdResult->v, i - cmdStart))
						{
							cmdResult->Release();
						}
						else
						{
							Sync::MutexUsage mutUsage(me->cmdResultMut);
							me->cmdResults.Add(cmdResult);
							mutUsage.EndUse();
							me->cmdEvt.Set();
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
	me->cmdEvt.Set();
	me->threadRunning = false;
	return 0;
}

void IO::ATCommandChannel::ClearResults()
{
	while (this->cmdResults.GetCount() > 0)
	{
		Sync::MutexUsage mutUsage(this->cmdResultMut);
		OPTSTR_DEL(this->cmdResults.RemoveAt(0));
	}
}

IO::ATCommandChannel::ATCommandChannel(NN<IO::Stream> stm, Bool needRelease)
{
	this->stm = stm;
	this->stmRelease = needRelease;
	this->evtHdlr = 0;
	this->evtHdlrObj = 0;
	this->cmdHdlr = 0;
	this->cmdHdlrObj = 0;
	this->log = 0;

	this->threadRunning = false;
	this->threadToStop = false;
	Sync::ThreadUtil::Create(CmdThread, this);
	while (!this->threadRunning)
	{
		this->cmdEvt.Wait(100);
	}
}

IO::ATCommandChannel::~ATCommandChannel()
{
	UOSInt i;
	this->Close();
	i = this->cmdResults.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->cmdResults.RemoveAt(i));
	}
	if (this->stmRelease)
	{
		this->stm.Delete();
	}
}

NN<IO::Stream> IO::ATCommandChannel::GetStream() const
{
	return this->stm;
}

UOSInt IO::ATCommandChannel::SendATCommand(NN<Data::ArrayListStringNN> retArr, UnsafeArray<const UTF8Char> atCmd, UOSInt atCmdLen, Data::Duration timeout)
{
	Data::DateTime dt;
	Data::DateTime dt2;
	UOSInt retSize = 0;
//	Bool cmdBegin = false;
	Bool cmdEnd = false;
	NN<Text::String> cmdRes;
	Sync::MutexUsage mutUsage;
	if (!this->UseCmd(mutUsage))
		return 0;
	this->CmdSend(atCmd, atCmdLen);
	this->CmdSend(U8STR("\r"), 1);
	
	dt.SetCurrTimeUTC();
	while (true)
	{
		while (this->CmdGetNextResult(1000).SetTo(cmdRes))
		{
			dt.SetCurrTimeUTC();
			retArr->Add(cmdRes);
			retSize++;
			if (cmdRes->Equals(UTF8STRC("OK")))
			{
				cmdEnd = true;
				break;
			}
			if (cmdRes->Equals(UTF8STRC("ERROR")))
			{
				cmdEnd = true;
				break;
			}
			if (cmdRes->StartsWith(UTF8STRC("+CME ERROR")))
			{
				cmdEnd = true;
				break;
			}
		}
		if (cmdEnd || !this->threadRunning)
			break;
		dt2.SetCurrTimeUTC();
		if (dt2.Diff(dt) >= timeout)
			break;
	}

	return retSize;
}

UOSInt IO::ATCommandChannel::SendATCommands(NN<Data::ArrayListStringNN> retArr, UnsafeArray<const UTF8Char> atCmd, UOSInt atCmdLen, UnsafeArray<const UTF8Char> atCmdSub, Data::Duration timeout)
{
	Data::DateTime dt;
	Data::DateTime dt2;
	UOSInt retSize = 0;
//	Bool cmdBegin = false;
	Bool cmdEnd = false;
	NN<Text::String> cmdRes;
	Sync::MutexUsage mutUsage;
	if (!this->UseCmd(mutUsage))
		return 0;
	this->CmdSend(atCmd, atCmdLen);
	this->CmdSend(U8STR("\r"), 1);
	Sync::SimpleThread::Sleep(1000);
	this->CmdSend(atCmdSub, Text::StrCharCnt(atCmdSub));
	this->CmdSend(U8STR("\x1a"), 1);
	
	dt.SetCurrTimeUTC();
	while (true)
	{
		while (this->CmdGetNextResult(1000).SetTo(cmdRes))
		{
			dt.SetCurrTimeUTC();
			retArr->Add(cmdRes);
			retSize++;
			if (cmdRes->Equals(UTF8STRC("OK")))
			{
				cmdEnd = true;
				break;
			}
			if (cmdRes->Equals(UTF8STRC("ERROR")))
			{
				cmdEnd = true;
				break;
			}
			if (cmdRes->StartsWith(UTF8STRC("+CME ERROR")))
			{
				cmdEnd = true;
				break;
			}
		}
		if (cmdEnd || !this->threadRunning)
			break;
		dt2.SetCurrTimeUTC();
		if (dt2.Diff(dt) >= timeout)
			break;
	}
	return retSize;
}

UOSInt IO::ATCommandChannel::SendDialCommand(NN<Data::ArrayListStringNN> retArr, UnsafeArray<const UTF8Char> atCmd, UOSInt atCmdLen, Data::Duration timeout)
{
	Data::DateTime dt;
	Data::DateTime dt2;
	UOSInt retSize = 0;
//	Bool cmdBegin = false;
	Bool cmdEnd = false;
	NN<Text::String> cmdRes;
	Sync::MutexUsage mutUsage;
	if (!this->UseCmd(mutUsage))
		return 0;
	this->CmdSend(atCmd, atCmdLen);
	this->CmdSend(U8STR("\r"), 1);
	
	dt.SetCurrTimeUTC();
	while (true)
	{
		while (this->CmdGetNextResult(1000).SetTo(cmdRes))
		{
			dt.SetCurrTimeUTC();
			retArr->Add(cmdRes);
			retSize++;
			if (cmdRes->Equals(UTF8STRC("NO DIALTONE")))
			{
				cmdEnd = true;
				break;
			}
			if (cmdRes->Equals(UTF8STRC("VCON")))
			{
				cmdEnd = true;
				break;
			}
			if (cmdRes->Equals(UTF8STRC("BUSY")))
			{
				cmdEnd = true;
				break;
			}
			if (cmdRes->Equals(UTF8STRC("NO CARRIER")))
			{
				cmdEnd = true;
				break;
			}
			if (cmdRes->Equals(UTF8STRC("OK")))
			{
				cmdEnd = true;
				break;
			}
			if (cmdRes->Equals(UTF8STRC("ERROR")))
			{
				cmdEnd = true;
				break;
			}
			if (cmdRes->StartsWith(UTF8STRC("+CME ERROR")))
			{
				cmdEnd = true;
				break;
			}
		}
		if (cmdEnd || !this->threadRunning)
			break;
		dt2.SetCurrTimeUTC();
		if (dt2.Diff(dt) >= timeout)
			break;
	}
	return retSize;
}

Bool IO::ATCommandChannel::UseCmd(NN<Sync::MutexUsage> mutUsage)
{
	if (!this->threadRunning)
		return false;
	mutUsage->ReplaceMutex(this->cmdMut);
	this->ClearResults();
	return true;
}

UOSInt IO::ATCommandChannel::CmdSend(UnsafeArray<const UInt8> data, UOSInt dataSize)
{
	return this->stm->Write(Data::ByteArrayR(data, dataSize));
}

Optional<Text::String> IO::ATCommandChannel::CmdGetNextResult(Data::Duration timeout)
{
	Optional<Text::String> cmdRes = 0;
	this->cmdEvt.Clear();
	if (this->cmdResults.GetCount() > 0)
	{
		Sync::MutexUsage mutUsage(this->cmdResultMut);
		cmdRes = this->cmdResults.RemoveAt(0);
		mutUsage.EndUse();
		return cmdRes;
	}
	this->cmdEvt.Wait(timeout);
	if (this->cmdResults.GetCount() > 0)
	{
		Sync::MutexUsage mutUsage(this->cmdResultMut);
		cmdRes = this->cmdResults.RemoveAt(0);
		mutUsage.EndUse();
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
			Sync::SimpleThread::Sleep(10);
		}
	}
}

void IO::ATCommandChannel::SetEventHandler(EventHandler evtHdlr, AnyType userObj)
{
	this->evtHdlrObj = userObj;
	this->evtHdlr = evtHdlr;
}

void IO::ATCommandChannel::SetCommandHandler(CommandHandler cmdHdlr, AnyType userObj)
{
	this->cmdHdlrObj = userObj;
	this->cmdHdlr = cmdHdlr;
}

void IO::ATCommandChannel::SetLogger(IO::ILogger *log)
{
	this->log = log;
}
