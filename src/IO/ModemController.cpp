#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ModemController.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

void IO::ModemController::ClearCmdResult()
{
	Text::String *val;
	UOSInt i;
	i = cmdResults->GetCount();
	while (i-- > 0)
	{
		val = cmdResults->RemoveAt(i);
		val->Release();
	}
}

Bool IO::ModemController::IsCmdSucceed()
{
	UOSInt i = this->cmdResults->GetCount();
	if (i <= 0)
		return false;
	return this->cmdResults->GetItem(i - 1)->Equals(UTF8STRC("OK"));
}

UTF8Char *IO::ModemController::SendStringCommand(UTF8Char *buff, const UTF8Char *cmd, UOSInt cmdLen, Int32 timeoutMS)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, cmdLen, timeoutMS);
	UOSInt i = this->cmdResults->GetCount();
	UOSInt j;
	Text::String *val;
//	printf("SendStringCommand, count = %d\r\n", i);
	if (i > 1)
	{
		val = this->cmdResults->GetItem(i - 1);
		if (val->Equals(UTF8STRC("OK")) == 0)
		{
			j = i - 2;
			UTF8Char *sptr = buff;
			buff[0] = 0;
			while (j >= 0)
			{
				val = this->cmdResults->GetItem(j);
				if (val->v[0] != 0)
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

Bool IO::ModemController::SendStringCommand(Data::ArrayList<Text::String*> *resList, const UTF8Char *cmd, UOSInt cmdLen, Int32 timeoutMS)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, cmdLen, timeoutMS);
	UOSInt i = this->cmdResults->GetCount();
	UOSInt j;
	Text::String *val;
	if (i > 1)
	{
		val = this->cmdResults->GetItem(i - 1);
		if (val->Equals(UTF8STRC("OK")))
		{
			j = 0;
			while (j < i - 1)
			{
				val = this->cmdResults->GetItem(j);
				if (val->v[0] != 0)
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

UTF8Char *IO::ModemController::SendStringCommandDirect(UTF8Char *buff, const UTF8Char *cmd, UOSInt cmdLen, Int32 timeoutMS)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, cmdLen, timeoutMS);
	UOSInt i = this->cmdResults->GetCount();
	UOSInt j;
	Text::String *val;
	j = 0;
	while (j < i - 1)
	{
		val = this->cmdResults->GetItem(j);
		if (val->StartsWith(cmd, cmdLen))
		{
			val = this->cmdResults->GetItem(j + 1);
			if (val->Equals(UTF8STRC("ERROR")))
			{
				ClearCmdResult();
				return 0;
			}
			else if (val->v[0] != 0)
			{
				UTF8Char *sptr = val->ConcatTo(buff);
				ClearCmdResult();
				return sptr;
			}
		}
		j++;
	}
	ClearCmdResult();
	return 0;
}

Bool IO::ModemController::SendStringListCommand(Text::StringBuilderUTF *sb, const UTF8Char *cmd, UOSInt cmdLen)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, cmdLen, 3000);
	UOSInt i = this->cmdResults->GetCount();
	UOSInt j;
	Text::String *val;
	if (i > 1)
	{
		val = this->cmdResults->GetItem(i - 1);
		if (val->Equals(UTF8STRC("OK")))
		{
			j = 1;
			while (j < i - 1)
			{
				sb->Append(this->cmdResults->GetItem(j));
				sb->AppendC(UTF8STRC("\r\n"));
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

Bool IO::ModemController::SendBoolCommandC(const UTF8Char *cmd, UOSInt cmdLen)
{
	return SendBoolCommandC(cmd, cmdLen, 3000);
}

Bool IO::ModemController::SendBoolCommandC(const UTF8Char *cmd, UOSInt cmdLen, Int32 timeoutMS)
{
	Bool isSucc;
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, cmdLen, timeoutMS);
	isSucc = this->IsCmdSucceed();
	ClearCmdResult();
	return isSucc;
}

IO::ModemController::DialResult IO::ModemController::SendDialCommand(const UTF8Char *cmd, UOSInt cmdLen)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendDialCommand(this->cmdResults, cmd, cmdLen, 30000);
	UOSInt i = this->cmdResults->GetCount();
	Text::String *val;
	if (i >= 1)
	{
		val = this->cmdResults->GetItem(i - 1);
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

IO::ModemController::ModemController(IO::ATCommandChannel *channel, Bool needRelease)
{
	this->channel = channel;
	this->needRelease = needRelease;
	NEW_CLASS(cmdResults, Data::ArrayList<Text::String*>());
	NEW_CLASS(cmdMut, Sync::Mutex());
}

IO::ModemController::~ModemController()
{
	DEL_CLASS(cmdMut);
	DEL_CLASS(cmdResults);
	if (needRelease)
	{
		DEL_CLASS(this->channel);
	}
}

IO::ATCommandChannel *IO::ModemController::GetChannel()
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
