#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/LoggedSocketFactory.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Net::LoggedSocketFactory::LoggedSocketFactory(Net::SocketFactory *sockf, IO::LogTool *log, const UTF8Char *logPrefix) : Net::SocketFactory(false)
{
	this->sockf = sockf;
	this->log = log;
	if (logPrefix)
	{
		this->logPrefix = Text::StrCopyNew(logPrefix);
	}
	else
	{
		this->logPrefix = 0;
	}
}

Net::LoggedSocketFactory::~LoggedSocketFactory()
{
	SDEL_TEXT(this->logPrefix);
}

UInt32 *Net::LoggedSocketFactory::CreateTCPSocketv4()
{
	UInt32 *ret = this->sockf->CreateTCPSocketv4();
	Text::StringBuilderUTF8 sb;
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"Create TCP Socket v4: ");
	if (!this->sockf->SocketIsInvalid(ret))
	{
		sb.Append((const UTF8Char*)"Success");
	}
	else
	{
		sb.Append((const UTF8Char*)"Failed");
	}
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	return ret;
}

UInt32 *Net::LoggedSocketFactory::CreateTCPSocketv6()
{
	UInt32 *ret = this->sockf->CreateTCPSocketv6();
	Text::StringBuilderUTF8 sb;
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"Create TCP Socket v6: ");
	if (!this->sockf->SocketIsInvalid(ret))
	{
		sb.Append((const UTF8Char*)"Success");
	}
	else
	{
		sb.Append((const UTF8Char*)"Failed");
	}
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	return ret;
}

UInt32 *Net::LoggedSocketFactory::CreateUDPSocketv4()
{
	UInt32 *ret = this->sockf->CreateUDPSocketv4();
	Text::StringBuilderUTF8 sb;
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"Create UDP Socket v4: ");
	if (!this->sockf->SocketIsInvalid(ret))
	{
		sb.Append((const UTF8Char*)"Success");
	}
	else
	{
		sb.Append((const UTF8Char*)"Failed");
	}
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	return ret;
}

void Net::LoggedSocketFactory::DestroySocket(UInt32 *socket)
{
	this->sockf->DestroySocket(socket);
	Text::StringBuilderUTF8 sb;
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"Destroy Socket: ");
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
}

Bool Net::LoggedSocketFactory::SocketIsInvalid(UInt32 *socket)
{
	return this->sockf->SocketIsInvalid(socket);
}

Bool Net::LoggedSocketFactory::SocketBindv4(UInt32 *socket, UInt32 ip, UInt16 port)
{
	UTF8Char sbuff[64];
	Bool ret = this->sockf->SocketBindv4(socket, ip, port);
	Net::SocketUtil::GetIPv4Name(sbuff, ip, port);
	Text::StringBuilderUTF8 sb;
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"Bind v4: ");
	sb.Append(sbuff);
	sb.Append((const UTF8Char*)", ");
	if (ret)
	{
		sb.Append((const UTF8Char*)"Success");
	}
	else
	{
		sb.Append((const UTF8Char*)"Failed");
	}
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	return ret;
}

Bool Net::LoggedSocketFactory::SocketListen(UInt32 *socket)
{
	Bool ret = this->sockf->SocketListen(socket);
	Text::StringBuilderUTF8 sb;
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"Listen to socket: ");
	if (ret)
	{
		sb.Append((const UTF8Char*)"Success");
	}
	else
	{
		sb.Append((const UTF8Char*)"Failed");
	}
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	return ret;
}

UInt32 *Net::LoggedSocketFactory::SocketAccept(UInt32 *socket)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"Begin socket accept");
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	UInt32 *ret = this->sockf->SocketAccept(socket);
	sb.ClearStr();
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"End socket accept: ");
	if (ret == 0 || this->sockf->SocketIsInvalid(ret))
	{
		sb.Append((const UTF8Char*)"Failed");
	}
	else
	{
		sb.Append((const UTF8Char*)"Success, remote: ");
		this->sockf->GetRemoteName(sbuff, ret);
		sb.Append(sbuff);
	}
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	return ret;
}

OSInt Net::LoggedSocketFactory::SendData(UInt32 *socket, const UInt8 *buff, OSInt buffSize, ErrorType *et)
{
	OSInt ret = this->sockf->SendData(socket, buff, buffSize, et);
	Text::StringBuilderUTF8 sb;
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"Sent ");
	sb.AppendOSInt(ret);
	sb.Append((const UTF8Char*)" bytes");
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	return ret;
}

OSInt Net::LoggedSocketFactory::ReceiveData(UInt32 *socket, UInt8 *buff, OSInt buffSize, ErrorType *et)
{
	OSInt ret = this->sockf->ReceiveData(socket, buff, buffSize, et);
	Text::StringBuilderUTF8 sb;
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"Received ");
	sb.AppendOSInt(ret);
	sb.Append((const UTF8Char*)" bytes");
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	return ret;
}

void *Net::LoggedSocketFactory::BeginReceiveData(UInt32 *socket, UInt8 *buff, OSInt buffSize, Sync::Event *evt, ErrorType *et)
{
	return this->sockf->BeginReceiveData(socket, buff, buffSize, evt, et);
}

OSInt Net::LoggedSocketFactory::EndReceiveData(void *reqData, Bool toWait)
{
	OSInt ret = this->sockf->EndReceiveData(reqData, toWait);
	if (toWait || ret > 0)
	{
		Text::StringBuilderUTF8 sb;
		if (this->logPrefix)
		{
			sb.Append(this->logPrefix);
		}
		sb.Append((const UTF8Char*)"End Received ");
		sb.AppendOSInt(ret);
		sb.Append((const UTF8Char*)" bytes");
		this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	}
	return ret;
}

void Net::LoggedSocketFactory::CancelReceiveData(void *reqData)
{
	this->sockf->CancelReceiveData(reqData);
}

OSInt Net::LoggedSocketFactory::UDPReceive(UInt32 *socket, UInt8 *buff, OSInt buffSize, Net::SocketUtil::AddressInfo *addr, UInt16 *port, ErrorType *et)
{
	OSInt ret = this->sockf->UDPReceive(socket, buff, buffSize, addr, port, et);
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"UDP Receive: ");
	if (ret == 0)
	{
		sb.Append((const UTF8Char*)"Failed");
		if (et)
		{
			sb.Append((const UTF8Char*)", ErrorType = ");
			sb.AppendI32(*et);
		}
	}
	else
	{
		sb.AppendOSInt(ret);
		sb.Append((const UTF8Char*)" bytes from");
		Net::SocketUtil::GetAddrName(sbuff, addr, *port);
		sb.Append(sbuff);
	}
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	return ret;
}

OSInt Net::LoggedSocketFactory::SendTo(UInt32 *socket, const UInt8 *buff, OSInt buffSize, const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	OSInt ret = this->sockf->SendTo(socket, buff, buffSize, addr, port);
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"Send To ");
	Net::SocketUtil::GetAddrName(sbuff, addr, port);
	sb.Append(sbuff);
	sb.Append((const UTF8Char*)": ");
	if (ret == 0)
	{
		sb.Append((const UTF8Char*)"Failed");
	}
	else
	{
		sb.AppendOSInt(ret);
		sb.Append((const UTF8Char*)" bytes");
	}
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	return ret;
}

Bool Net::LoggedSocketFactory::Connect(UInt32 *socket, UInt32 ip, UInt16 port)
{
	Bool ret = this->sockf->Connect(socket, ip, port);
	UTF8Char sbuff[64];
	Text::StringBuilderUTF8 sb;
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"Connect to ");
	Net::SocketUtil::GetIPv4Name(sbuff, ip, port);
	sb.Append((const UTF8Char*)": ");
	if (ret)
	{
		sb.Append((const UTF8Char*)"Success");
	}
	else
	{
		sb.Append((const UTF8Char*)"Failed");
	}
	return ret;
}

void Net::LoggedSocketFactory::ShutdownSend(UInt32 *socket)
{
	this->sockf->ShutdownSend(socket);
	Text::StringBuilderUTF8 sb;
	if (this->logPrefix)
	{
		sb.Append(this->logPrefix);
	}
	sb.Append((const UTF8Char*)"Shutdown Send");
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
}
