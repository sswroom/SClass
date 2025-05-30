#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ModemController.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

void IO::ModemController::ClearCmdResult()
{
	UOSInt i;
	i = this->cmdResults.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->cmdResults.RemoveAt(i));
	}
}

Bool IO::ModemController::IsCmdSucceed()
{
	NN<Text::String> s;
	UOSInt i = this->cmdResults.GetCount();
	if (i <= 0 || !this->cmdResults.GetItem(i - 1).SetTo(s))
		return false;
	return s->Equals(UTF8STRC("OK"));
}

UnsafeArrayOpt<UTF8Char> IO::ModemController::SendStringCommand(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen, Data::Duration timeout)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, cmdLen, timeout);
	UOSInt i = this->cmdResults.GetCount();
	UOSInt j;
	NN<Text::String> val;
//	printf("SendStringCommand, count = %d\r\n", i);
	if (i > 1)
	{
		if (this->cmdResults.GetItem(i - 1).SetTo(val) && val->Equals(UTF8STRC("OK")))
		{
			j = i - 2;
			UnsafeArray<UTF8Char> sptr = buff;
			buff[0] = 0;
			while (j >= 0)
			{
				if (this->cmdResults.GetItem(j).SetTo(val) && val->v[0] != 0)
				{
//					printf("SendStringCommand, Str = %s\r\n", val);
					sptr = val->ConcatTo(buff);
					break;
				}
				else
				{
					j--;
				}
				
			}
			
			ClearCmdResult();
			return sptr;
		}
		else
		{
			ClearCmdResult();
			return 0;
		}
	}
	else
	{
		ClearCmdResult();
		return 0;
	}
}

Bool IO::ModemController::SendStringCommand(NN<Data::ArrayListStringNN> resList, UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen, Data::Duration timeout)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, cmdLen, timeout);
	UOSInt i = this->cmdResults.GetCount();
	UOSInt j;
	NN<Text::String> val;
	if (i > 1)
	{
		if (this->cmdResults.GetItem(i - 1).SetTo(val) && val->Equals(UTF8STRC("OK")))
		{
			j = 0;
			while (j < i - 1)
			{
				if (this->cmdResults.GetItem(j).SetTo(val) && val->v[0] != 0)
				{
					resList->Add(val->Clone());
				}
				j++;
			}
			
			ClearCmdResult();
			return true;
		}
		else
		{
			ClearCmdResult();
			return false;
		}
	}
	else
	{
		ClearCmdResult();
		return false;
	}
}

UnsafeArrayOpt<UTF8Char> IO::ModemController::SendStringCommandDirect(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen, Data::Duration timeout)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, cmdLen, timeout);
	UOSInt i = this->cmdResults.GetCount();
	UOSInt j;
	NN<Text::String> val;
	j = 0;
	while (j < i - 1)
	{
		if (this->cmdResults.GetItem(j).SetTo(val) && val->StartsWith(cmd, cmdLen))
		{
			if (this->cmdResults.GetItem(j + 1).SetTo(val))
			{
				if (val->Equals(UTF8STRC("ERROR")))
				{
					ClearCmdResult();
					return 0;
				}
				else if (val->v[0] != 0)
				{
					UnsafeArray<UTF8Char> sptr = val->ConcatTo(buff);
					ClearCmdResult();
					return sptr;
				}
			}
		}
		j++;
	}
	ClearCmdResult();
	return 0;
}

Bool IO::ModemController::SendStringListCommand(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, cmdLen, 3000);
	UOSInt i = this->cmdResults.GetCount();
	UOSInt j;
	NN<Text::String> val;
	if (i > 1)
	{
		if (this->cmdResults.GetItem(i - 1).SetTo(val) && val->Equals(UTF8STRC("OK")))
		{
			j = 1;
			while (j < i - 1)
			{
				if (this->cmdResults.GetItem(j).SetTo(val))
				{
					sb->Append(val);
					sb->AppendC(UTF8STRC("\r\n"));
				}
				j++;
			}
			
			ClearCmdResult();
			return true;
		}
		else
		{
			ClearCmdResult();
			return false;
		}
	}
	else
	{
		ClearCmdResult();
		return false;
	}
}

Bool IO::ModemController::SendBoolCommandC(UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen)
{
	return SendBoolCommandC(cmd, cmdLen, 3000);
}

Bool IO::ModemController::SendBoolCommandC(UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen, Data::Duration timeout)
{
	Bool isSucc;
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, cmdLen, timeout);
	isSucc = this->IsCmdSucceed();
	ClearCmdResult();
	return isSucc;
}

IO::ModemController::DialResult IO::ModemController::SendDialCommand(UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendDialCommand(this->cmdResults, cmd, cmdLen, 30000);
	UOSInt i = this->cmdResults.GetCount();
	NN<Text::String> val;
	if (i >= 1 && this->cmdResults.GetItem(i - 1).SetTo(val))
	{
		if (val->Equals(UTF8STRC("VCON")))
		{
			ClearCmdResult();
			return DR_CONNECT;
		}
		else if (val->Equals(UTF8STRC("NO DIALTONE")))
		{
			ClearCmdResult();
			return DR_NO_DIALTONE;
		}
		else if (val->Equals(UTF8STRC("NO CARRIER")))
		{
			ClearCmdResult();
			return DR_NO_CARRIER;
		}
		else if (val->Equals(UTF8STRC("BUSY")))
		{
			ClearCmdResult();
			return DR_BUSY;
		}
		else
		{
			ClearCmdResult();
			return DR_ERROR;
		}
	}
	else
	{
		ClearCmdResult();
		return DR_ERROR;
	}
}

IO::ModemController::ModemController(NN<IO::ATCommandChannel> channel, Bool needRelease)
{
	this->channel = channel;
	this->needRelease = needRelease;
}

IO::ModemController::~ModemController()
{
	if (needRelease)
	{
		this->channel.Delete();
	}
}

NN<IO::ATCommandChannel> IO::ModemController::GetChannel() const
{
	return this->channel;
}

Bool IO::ModemController::HangUp()
{
	return this->SendBoolCommandC(UTF8STRC("ATH"));
}

Bool IO::ModemController::ResetModem()
{
	Bool isSucc;
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, UTF8STRC("ATZ"), 3000);
	isSucc = this->IsCmdSucceed();
	ClearCmdResult();
	return isSucc;
}

Bool IO::ModemController::SetEcho(Bool showEcho)
{
	if (showEcho)
	{
		return this->SendBoolCommandC(UTF8STRC("ATE1"));
	}
	else
	{
		return this->SendBoolCommandC(UTF8STRC("ATE0"));
	}
}
