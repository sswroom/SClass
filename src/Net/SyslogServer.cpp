#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Net/SyslogServer.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::SyslogServer::OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::SyslogServer *me = (Net::SyslogServer*)userData;
	UTF8Char sbuff[64];
	if (buff[0] == '<')
	{
		Net::SyslogServer::IPStatus *status = me->GetIPStatus(addr);
		Text::StringBuilderUTF8 sb;

		if (me->log)
		{
			Net::SocketUtil::GetAddrName(sbuff, addr);
			sb.Append(sbuff);
			sb.AppendC(UTF8STRC("> "));
			sb.AppendC(buff, dataSize);
			me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_COMMAND);
		}

		if (status)
		{
			sb.ClearStr();
			sb.AppendC(buff, dataSize);
			status->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_COMMAND);

			if (me->logHdlr)
			{
				me->logHdlr(me->logHdlrObj, status->ip, sb.ToString());
			}
		}
	}
}

Net::SyslogServer::IPStatus *Net::SyslogServer::GetIPStatus(const Net::SocketUtil::AddressInfo *addr)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	if (addr->addrType == Net::AddrType::IPv4)
	{
		UInt32 ip = ReadMUInt32(addr->addr);
		Net::SyslogServer::IPStatus *status;
		Sync::MutexUsage mutUsage(this->ipMut);
		status = this->ipMap->Get(ip);
		if (status)
		{
			return status;
		}
		status = MemAlloc(IPStatus, 1);
		status->ip = ReadNUInt32(addr->addr);
		sptr = Text::StrConcat(sbuff, this->logPath);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Net::SocketUtil::GetAddrName(sptr, addr);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"Log");
		NEW_CLASS(status->log, IO::LogTool());
		status->log->AddFileLog(sbuff, IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_RAW, "yyyy-MM-dd HH:mm:ss.fff", false);
		this->ipMap->Put(ip, status);
		return status;
	}
	return 0;
}

Net::SyslogServer::SyslogServer(Net::SocketFactory *sockf, UInt16 port, const UTF8Char *logPath, IO::LogTool *svrLog, Bool redirLog)
{
	this->sockf = sockf;
	this->logPath = Text::StrCopyNew(logPath);
	this->log = svrLog;
	this->redirLog = redirLog;
	this->logHdlr = 0;
	this->logHdlrObj = 0;
	NEW_CLASS(this->ipMut, Sync::Mutex());
	NEW_CLASS(this->ipMap, Data::UInt32Map<Net::SyslogServer::IPStatus*>());
	NEW_CLASS(this->svr, Net::UDPServer(this->sockf, 0, port, 0, OnUDPPacket, this, log, (const UTF8Char*)"UDP: ", 2, false));
}

Net::SyslogServer::~SyslogServer()
{
	DEL_CLASS(this->svr);
	Text::StrDelNew(this->logPath);
	UOSInt i;
	Data::ArrayList<Net::SyslogServer::IPStatus*> *ipList = this->ipMap->GetValues();
	IPStatus *status;
	i = ipList->GetCount();
	while (i-- > 0)
	{
		status = ipList->GetItem(i);
		DEL_CLASS(status->log);
		MemFree(status);
	}
	DEL_CLASS(this->ipMap);
	DEL_CLASS(this->ipMut);
}

Bool Net::SyslogServer::IsError()
{
	return this->svr->IsError();
}

void Net::SyslogServer::HandleClientLog(ClientLogHandler hdlr, void *userObj)
{
	this->logHdlrObj = userObj;
	this->logHdlr = hdlr;
}
