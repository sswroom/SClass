#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ModemController.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

void IO::ModemController::ClearCmdResult()
{
	const Char *val;
	UOSInt i;
	i = cmdResults->GetCount();
	while (i-- > 0)
	{
		val = cmdResults->RemoveAt(i);
		MemFree((void*)val);
	}
}

Bool IO::ModemController::IsCmdSucceed()
{
	UOSInt i = this->cmdResults->GetCount();
	if (i <= 0)
		return false;
	return Text::StrCompare(this->cmdResults->GetItem(i - 1), "OK") == 0;
}

UTF8Char *IO::ModemController::SendStringCommand(UTF8Char *buff, const Char *cmd, Int32 timeoutMS)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, timeoutMS);
	UOSInt i = this->cmdResults->GetCount();
	UOSInt j;
	const Char *val;
//	printf("SendStringCommand, count = %d\r\n", i);
	if (i > 1)
	{
		val = this->cmdResults->GetItem(i - 1);
		if (Text::StrCompare(val, "OK") == 0)
		{
			j = i - 2;
			UTF8Char *sptr = buff;
			buff[0] = 0;
			while (j >= 0)
			{
				val = this->cmdResults->GetItem(j);
				if (val[0] != 0)
				{
//					printf("SendStringCommand, Str = %s\r\n", val);
					sptr = Text::StrConcat(buff, (const UTF8Char*)val);
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

Bool IO::ModemController::SendStringCommand(Data::ArrayList<const Char*> *resList, const Char *cmd, Int32 timeoutMS)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, timeoutMS);
	UOSInt i = this->cmdResults->GetCount();
	UOSInt j;
	const Char *val;
	if (i > 1)
	{
		val = this->cmdResults->GetItem(i - 1);
		if (Text::StrCompare(val, "OK") == 0)
		{
			j = 0;
			while (j < i - 1)
			{
				val = this->cmdResults->GetItem(j);
				if (val[0] != 0)
				{
					resList->Add(Text::StrCopyNew(val));
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

UTF8Char *IO::ModemController::SendStringCommandDirect(UTF8Char *buff, const Char *cmd, Int32 timeoutMS)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, timeoutMS);
	UOSInt i = this->cmdResults->GetCount();
	UOSInt j;
	const Char *val;
	j = 0;
	while (j < i - 1)
	{
		val = this->cmdResults->GetItem(j);
		if (Text::StrStartsWith(val, cmd))
		{
			val = this->cmdResults->GetItem(j + 1);
			if (Text::StrEquals(val, "ERROR"))
			{
				ClearCmdResult();
				return 0;
			}
			else if (val[0] != 0)
			{
				UTF8Char *sptr = Text::StrConcat(buff, (const UTF8Char*)val);
				ClearCmdResult();
				return sptr;
			}
		}
		j++;
	}
	ClearCmdResult();
	return 0;
}

Bool IO::ModemController::SendStringListCommand(Text::StringBuilderUTF *sb, const Char *cmd)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, 3000);
	UOSInt i = this->cmdResults->GetCount();
	UOSInt j;
	const Char *val;
	if (i > 1)
	{
		val = this->cmdResults->GetItem(i - 1);
		if (Text::StrCompare(val, "OK") == 0)
		{
			j = 1;
			while (j < i - 1)
			{
				sb->Append((const UTF8Char*)this->cmdResults->GetItem(j));
				sb->Append((const UTF8Char*)"\r\n");
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

Bool IO::ModemController::SendBoolCommand(const Char *cmd)
{
	return SendBoolCommand(cmd, 3000);
}

Bool IO::ModemController::SendBoolCommand(const Char *cmd, Int32 timeoutMS)
{
	Bool isSucc;
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, cmd, timeoutMS);
	isSucc = this->IsCmdSucceed();
	ClearCmdResult();
	return isSucc;
}

IO::ModemController::DialResult IO::ModemController::SendDialCommand(const Char *cmd)
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendDialCommand(this->cmdResults, cmd, 30000);
	UOSInt i = this->cmdResults->GetCount();
	const Char *val;
	if (i >= 1)
	{
		val = this->cmdResults->GetItem(i - 1);
		if (Text::StrCompare(val, "VCON") == 0)
		{
			ClearCmdResult();
			return DR_CONNECT;
		}
		else if (Text::StrCompare(val, "NO DIALTONE") == 0)
		{
			ClearCmdResult();
			return DR_NO_DIALTONE;
		}
		else if (Text::StrCompare(val, "NO CARRIER") == 0)
		{
			ClearCmdResult();
			return DR_NO_CARRIER;
		}
		else if (Text::StrCompare(val, "BUSY") == 0)
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
	NEW_CLASS(cmdResults, Data::ArrayList<const Char*>());
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
	return this->SendBoolCommand("ATH");
}

Bool IO::ModemController::ResetModem()
{
	Bool isSucc;
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, "ATZ", 3000);
	isSucc = this->IsCmdSucceed();
	ClearCmdResult();
	return isSucc;
}

Bool IO::ModemController::SetEcho(Bool showEcho)
{
	if (showEcho)
	{
		return this->SendBoolCommand("ATE1");
	}
	else
	{
		return this->SendBoolCommand("ATE0");
	}
}
