#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/Path.h"
#include "Net/SyslogServer.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::SyslogServer::OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::SyslogServer> me = userData.GetNN<Net::SyslogServer>();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (data[0] == '<')
	{
		Net::SyslogServer::IPStatus *status = me->GetIPStatus(addr);
		Text::StringBuilderUTF8 sb;

		if (me->log->HasHandler())
		{
			sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC("> "));
			sb.AppendC(&data[0], data.GetSize());
			me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
		}

		if (status)
		{
			sb.ClearStr();
			sb.AppendC(&data[0], data.GetSize());
			status->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);

			if (me->logHdlr)
			{
				me->logHdlr(me->logHdlrObj, status->ip, sb.ToCString());
			}
		}
	}
}

Net::SyslogServer::IPStatus *Net::SyslogServer::GetIPStatus(NN<const Net::SocketUtil::AddressInfo> addr)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;

	if (addr->addrType == Net::AddrType::IPv4)
	{
		UInt32 ip = ReadMUInt32(addr->addr);
		Net::SyslogServer::IPStatus *status;
		Sync::MutexUsage mutUsage(this->ipMut);
		status = this->ipMap.Get(ip);
		if (status)
		{
			return status;
		}
		status = MemAlloc(IPStatus, 1);
		status->ip = ReadNUInt32(addr->addr);
		sptr = this->logPath->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Net::SocketUtil::GetAddrName(sptr, addr).Or(sptr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcatC(sptr, UTF8STRC("Log"));
		NEW_CLASS(status->log, IO::LogTool());
		status->log->AddFileLog(CSTRP(sbuff, sptr), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
		this->ipMap.Put(ip, status);
		return status;
	}
	return 0;
}

Net::SyslogServer::SyslogServer(NN<Net::SocketFactory> sockf, UInt16 port, Text::CStringNN logPath, NN<IO::LogTool> svrLog, Bool redirLog)
{
	this->sockf = sockf;
	this->logPath = Text::String::New(logPath);
	this->log = svrLog;
	this->redirLog = redirLog;
	this->logHdlr = 0;
	this->logHdlrObj = 0;
	NEW_CLASS(this->svr, Net::UDPServer(this->sockf, nullptr, port, nullptr, OnUDPPacket, this, log, CSTR("UDP: "), 2, false));
}

Net::SyslogServer::~SyslogServer()
{
	DEL_CLASS(this->svr);
	this->logPath->Release();
	UIntOS i;
	IPStatus *status;
	i = this->ipMap.GetCount();
	while (i-- > 0)
	{
		status = this->ipMap.GetItem(i);
		DEL_CLASS(status->log);
		MemFree(status);
	}
}

Bool Net::SyslogServer::IsError()
{
	return this->svr->IsError();
}

void Net::SyslogServer::HandleClientLog(ClientLogHandler hdlr, AnyType userObj)
{
	this->logHdlrObj = userObj;
	this->logHdlr = hdlr;
}
