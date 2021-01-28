#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ModemController.h"
#include "Text/MyString.h"

void IO::ModemController::ClearCmdResult()
{
	const Char *val;
	OSInt i;
	i = cmdResults->GetCount();
	while (i-- > 0)
	{
		val = cmdResults->RemoveAt(i);
		MemFree((void*)val);
	}
}

Bool IO::ModemController::IsCmdSucceed()
{
	OSInt i = this->cmdResults->GetCount();
	if (i <= 0)
		return false;
	return Text::StrCompare(this->cmdResults->GetItem(i - 1), "OK") == 0;
}

UTF8Char *IO::ModemController::SendStringCommand(UTF8Char *buff, const Char *cmd, Int32 timeoutMS)
{
	this->cmdMut->Lock();
	this->channel->SendATCommand(this->cmdResults, cmd, timeoutMS);
	OSInt i = this->cmdResults->GetCount();
	OSInt j;
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
			this->cmdMut->Unlock();
			return sptr;
		}
		else
		{
			ClearCmdResult();
			this->cmdMut->Unlock();
			return 0;
		}
	}
	else
	{
		ClearCmdResult();
		this->cmdMut->Unlock();
		return 0;
	}
}

Bool IO::ModemController::SendStringCommand(Data::ArrayList<const Char*> *resList, const Char *cmd, Int32 timeoutMS)
{
	this->cmdMut->Lock();
	this->channel->SendATCommand(this->cmdResults, cmd, timeoutMS);
	OSInt i = this->cmdResults->GetCount();
	OSInt j;
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
			this->cmdMut->Unlock();
			return true;
		}
		else
		{
			ClearCmdResult();
			this->cmdMut->Unlock();
			return false;
		}
	}
	else
	{
		ClearCmdResult();
		this->cmdMut->Unlock();
		return false;
	}
}

UTF8Char *IO::ModemController::SendStringCommandDirect(UTF8Char *buff, const Char *cmd, Int32 timeoutMS)
{
	this->cmdMut->Lock();
	this->channel->SendATCommand(this->cmdResults, cmd, timeoutMS);
	OSInt i = this->cmdResults->GetCount();
	OSInt j;
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
				this->cmdMut->Unlock();
				return 0;
			}
			else if (val[0] != 0)
			{
				UTF8Char *sptr = Text::StrConcat(buff, (const UTF8Char*)val);
				ClearCmdResult();
				this->cmdMut->Unlock();
				return sptr;
			}
		}
		j++;
	}
	ClearCmdResult();
	this->cmdMut->Unlock();
	return 0;
}

Bool IO::ModemController::SendStringListCommand(Text::StringBuilderUTF *sb, const Char *cmd)
{
	this->cmdMut->Lock();
	this->channel->SendATCommand(this->cmdResults, cmd, 3000);
	OSInt i = this->cmdResults->GetCount();
	OSInt j;
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
			this->cmdMut->Unlock();
			return true;
		}
		else
		{
			ClearCmdResult();
			this->cmdMut->Unlock();
			return false;
		}
	}
	else
	{
		ClearCmdResult();
		this->cmdMut->Unlock();
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
	this->cmdMut->Lock();
	this->channel->SendATCommand(this->cmdResults, cmd, timeoutMS);
	isSucc = this->IsCmdSucceed();
	ClearCmdResult();
	this->cmdMut->Unlock();
	return isSucc;
}

IO::ModemController::DialResult IO::ModemController::SendDialCommand(const Char *cmd)
{
	this->cmdMut->Lock();
	this->channel->SendDialCommand(this->cmdResults, cmd, 30000);
	OSInt i = this->cmdResults->GetCount();
	const Char *val;
	if (i >= 1)
	{
		val = this->cmdResults->GetItem(i - 1);
		if (Text::StrCompare(val, "VCON") == 0)
		{
			ClearCmdResult();
			this->cmdMut->Unlock();
			return DR_CONNECT;
		}
		else if (Text::StrCompare(val, "NO DIALTONE") == 0)
		{
			ClearCmdResult();
			this->cmdMut->Unlock();
			return DR_NO_DIALTONE;
		}
		else if (Text::StrCompare(val, "NO CARRIER") == 0)
		{
			ClearCmdResult();
			this->cmdMut->Unlock();
			return DR_NO_CARRIER;
		}
		else if (Text::StrCompare(val, "BUSY") == 0)
		{
			ClearCmdResult();
			this->cmdMut->Unlock();
			return DR_BUSY;
		}
		else
		{
			ClearCmdResult();
			this->cmdMut->Unlock();
			return DR_ERROR;
		}
	}
	else
	{
		ClearCmdResult();
		this->cmdMut->Unlock();
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
	this->cmdMut->Lock();
	this->channel->SendATCommand(this->cmdResults, "ATZ", 3000);
	isSucc = this->IsCmdSucceed();
	ClearCmdResult();
	this->cmdMut->Unlock();
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
