#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/LoggedSocketFactory.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Net::LoggedSocketFactory::LoggedSocketFactory(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log, Text::CString logPrefix) : Net::SocketFactory(false)
{
	this->sockf = sockf;
	this->log = log;
	this->logPrefix = Text::String::NewOrNull(logPrefix);
}

Net::LoggedSocketFactory::~LoggedSocketFactory()
{
	OPTSTR_DEL(this->logPrefix);
}

Optional<Socket> Net::LoggedSocketFactory::CreateTCPSocketv4()
{
	Optional<Socket> ret = this->sockf->CreateTCPSocketv4();
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Create TCP Socket v4: "));
	if (ret.NotNull())
	{
		sb.AppendC(UTF8STRC("Success"));
	}
	else
	{
		sb.AppendC(UTF8STRC("Failed"));
	}
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	return ret;
}

Optional<Socket> Net::LoggedSocketFactory::CreateTCPSocketv6()
{
	Optional<Socket> ret = this->sockf->CreateTCPSocketv6();
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Create TCP Socket v6: "));
	if (ret.NotNull())
	{
		sb.AppendC(UTF8STRC("Success"));
	}
	else
	{
		sb.AppendC(UTF8STRC("Failed"));
	}
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	return ret;
}

Optional<Socket> Net::LoggedSocketFactory::CreateUDPSocketv4()
{
	Optional<Socket> ret = this->sockf->CreateUDPSocketv4();
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Create UDP Socket v4: "));
	if (ret.NotNull())
	{
		sb.AppendC(UTF8STRC("Success"));
	}
	else
	{
		sb.AppendC(UTF8STRC("Failed"));
	}
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	return ret;
}

void Net::LoggedSocketFactory::DestroySocket(NN<Socket> socket)
{
	this->sockf->DestroySocket(socket);
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Destroy Socket: "));
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
}

Bool Net::LoggedSocketFactory::SocketBindv4(NN<Socket> socket, UInt32 ip, UInt16 port)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Bool ret = this->sockf->SocketBindv4(socket, ip, port);
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip, port);
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Bind v4: "));
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC(", "));
	if (ret)
	{
		sb.AppendC(UTF8STRC("Success"));
	}
	else
	{
		sb.AppendC(UTF8STRC("Failed"));
	}
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	return ret;
}

Bool Net::LoggedSocketFactory::SocketListen(NN<Socket> socket)
{
	Bool ret = this->sockf->SocketListen(socket);
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Listen to socket: "));
	if (ret)
	{
		sb.AppendC(UTF8STRC("Success"));
	}
	else
	{
		sb.AppendC(UTF8STRC("Failed"));
	}
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	return ret;
}

Optional<Socket> Net::LoggedSocketFactory::SocketAccept(NN<Socket> socket)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Begin socket accept"));
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	Optional<Socket> ret = this->sockf->SocketAccept(socket);
	sb.ClearStr();
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("End socket accept: "));
	NN<Socket> soc;
	if (!ret.SetTo(soc))
	{
		sb.AppendC(UTF8STRC("Failed"));
	}
	else
	{
		sb.AppendC(UTF8STRC("Success, remote: "));
		sptr = this->sockf->GetRemoteName(sbuff, soc);
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	}
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	return ret;
}

UOSInt Net::LoggedSocketFactory::SendData(NN<Socket> socket, const UInt8 *buff, UOSInt buffSize, OptOut<ErrorType> et)
{
	UOSInt ret = this->sockf->SendData(socket, buff, buffSize, et);
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Sent "));
	sb.AppendOSInt(ret);
	sb.AppendC(UTF8STRC(" bytes"));
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	return ret;
}

UOSInt Net::LoggedSocketFactory::ReceiveData(NN<Socket> socket, UInt8 *buff, UOSInt buffSize, OptOut<ErrorType> et)
{
	UOSInt ret = this->sockf->ReceiveData(socket, buff, buffSize, et);
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Received "));
	sb.AppendOSInt(ret);
	sb.AppendC(UTF8STRC(" bytes"));
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	return ret;
}

void *Net::LoggedSocketFactory::BeginReceiveData(NN<Socket> socket, UInt8 *buff, UOSInt buffSize, Sync::Event *evt, OptOut<ErrorType> et)
{
	return this->sockf->BeginReceiveData(socket, buff, buffSize, evt, et);
}

UOSInt Net::LoggedSocketFactory::EndReceiveData(void *reqData, Bool toWait, OutParam<Bool> incomplete)
{
	UOSInt ret = this->sockf->EndReceiveData(reqData, toWait, incomplete);
	if (toWait || ret > 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendOpt(this->logPrefix);
		sb.AppendC(UTF8STRC("End Received "));
		sb.AppendOSInt(ret);
		sb.AppendC(UTF8STRC(" bytes"));
		this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	}
	return ret;
}

void Net::LoggedSocketFactory::CancelReceiveData(void *reqData)
{
	this->sockf->CancelReceiveData(reqData);
}

UOSInt Net::LoggedSocketFactory::UDPReceive(NN<Socket> socket, UInt8 *buff, UOSInt buffSize, NN<Net::SocketUtil::AddressInfo> addr, OutParam<UInt16> port, OptOut<ErrorType> et)
{
	ErrorType etTmp;
	UInt16 portTmp;
	UOSInt ret = this->sockf->UDPReceive(socket, buff, buffSize, addr, portTmp, etTmp);
	et.Set(etTmp);
	port.Set(portTmp);
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("UDP Receive: "));
	if (ret == 0)
	{
		sb.AppendC(UTF8STRC("Failed"));
		sb.AppendC(UTF8STRC(", ErrorType = "));
		sb.AppendI32(etTmp);
	}
	else
	{
		sb.AppendOSInt(ret);
		sb.AppendC(UTF8STRC(" bytes from"));
		sptr = Net::SocketUtil::GetAddrName(sbuff, addr, portTmp);
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	}
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	return ret;
}

UOSInt Net::LoggedSocketFactory::SendTo(NN<Socket> socket, const UInt8 *buff, UOSInt buffSize, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port)
{
	UOSInt ret = this->sockf->SendTo(socket, buff, buffSize, addr, port);
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Send To "));
	sptr = Net::SocketUtil::GetAddrName(sbuff, addr, port);
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC(": "));
	if (ret == 0)
	{
		sb.AppendC(UTF8STRC("Failed"));
	}
	else
	{
		sb.AppendUOSInt(ret);
		sb.AppendC(UTF8STRC(" bytes"));
	}
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	return ret;
}

Bool Net::LoggedSocketFactory::Connect(NN<Socket> socket, UInt32 ip, UInt16 port, Data::Duration timeout)
{
	Bool ret = this->sockf->Connect(socket, ip, port, timeout);
	UTF8Char sbuff[64];
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Connect to "));
	Net::SocketUtil::GetIPv4Name(sbuff, ip, port);
	sb.AppendC(UTF8STRC(": "));
	if (ret)
	{
		sb.AppendC(UTF8STRC("Success"));
	}
	else
	{
		sb.AppendC(UTF8STRC("Failed"));
	}
	return ret;
}

void Net::LoggedSocketFactory::ShutdownSend(NN<Socket> socket)
{
	this->sockf->ShutdownSend(socket);
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Shutdown Send"));
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
}

void Net::LoggedSocketFactory::ShutdownSocket(NN<Socket> socket)
{
	this->sockf->ShutdownSocket(socket);
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->logPrefix);
	sb.AppendC(UTF8STRC("Shutdown Socket"));
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
}
