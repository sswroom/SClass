#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Net/LogServer.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

#define MAXSIZE 4096
#define MTU 2048
#define BUFFSIZE (MAXSIZE + MTU)

void __stdcall Net::LogServer::ConnHdlr(NN<Socket> s, AnyType userObj)
{
	NN<Net::LogServer> me = userObj.GetNN<Net::LogServer>();
	NN<Net::TCPClient> cli;
	ClientStatus *cliStatus;
	Net::SocketUtil::AddressInfo addr;
	NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
	cliStatus = MemAlloc(ClientStatus, 1);
	cliStatus->buff = MemAlloc(UInt8, BUFFSIZE);
	cliStatus->buffSize = 0;
	cli->GetRemoteAddr(addr);
	cliStatus->status = me->GetIPStatus(addr);
	me->cliMgr->AddClient(cli, cliStatus);
}

void __stdcall Net::LogServer::ClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	NN<Net::LogServer> me = userObj.GetNN<Net::LogServer>();
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		NN<ClientStatus> cliStatus = cliData.GetNN<ClientStatus>();
		me->log->LogMessage(CSTR("Client Disconnected"), IO::LogHandler::LogLevel::Command);
		MemFree(cliStatus->buff);
		MemFree(cliStatus.Ptr());
		cli.Delete();
	}
	else if (evtType == Net::TCPClientMgr::TCP_EVENT_HASDATA)
	{
	}
}

void __stdcall Net::LogServer::ClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff)
{
	NN<Net::LogServer> me = userObj.GetNN<Net::LogServer>();
	NN<ClientStatus> cliStatus = cliData.GetNN<ClientStatus>();
	if (buff.GetSize() > BUFFSIZE)
	{
		MemCopyNO(cliStatus->buff, &buff[buff.GetSize() - BUFFSIZE], BUFFSIZE);
		cliStatus->buffSize = BUFFSIZE;
	}
	else if (cliStatus->buffSize + buff.GetSize() > BUFFSIZE)
	{
		MemCopyO(cliStatus->buff, &cliStatus->buff[cliStatus->buffSize - BUFFSIZE + buff.GetSize()], BUFFSIZE - buff.GetSize());
		MemCopyNO(&cliStatus->buff[BUFFSIZE - buff.GetSize()], buff.Arr().Ptr(), buff.GetSize());
		cliStatus->buffSize = BUFFSIZE;
	}
	else
	{
		MemCopyNO(&cliStatus->buff[cliStatus->buffSize], buff.Arr().Ptr(), buff.GetSize());
		cliStatus->buffSize += buff.GetSize();
	}

	UOSInt sizeLeft = me->protoHdlr.ParseProtocol(cli, cliStatus, 0, Data::ByteArrayR(cliStatus->buff, cliStatus->buffSize));
	if (sizeLeft <= 0)
	{
		cliStatus->buffSize = 0;
	}
	else if (sizeLeft < cliStatus->buffSize)
	{
		MemCopyO(cliStatus->buff, &cliStatus->buff[cliStatus->buffSize - sizeLeft], sizeLeft);
		cliStatus->buffSize = sizeLeft;
	}
}

void __stdcall Net::LogServer::ClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{
}

Optional<Net::LogServer::IPStatus> Net::LogServer::GetIPStatus(NN<const Net::SocketUtil::AddressInfo> addr)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;

	if (addr->addrType == Net::AddrType::IPv4)
	{
		UInt32 ip = ReadMUInt32(addr->addr);
		NN<IPStatus> status;
		Sync::MutexUsage mutUsage(this->ipMut);
		if (this->ipMap.Get(ip).SetTo(status))
		{
			return status;
		}
		status = MemAllocNN(IPStatus);
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
		mutUsage.EndUse();
		return status;
	}
	return 0;
}

Net::LogServer::LogServer(NN<Net::SocketFactory> sockf, UInt16 port, Text::CStringNN logPath, NN<IO::LogTool> svrLog, Bool redirLog, Bool autoStart) : protoHdlr(*this)
{
	this->sockf = sockf;
	this->logPath = Text::String::New(logPath);
	this->log = svrLog;
	this->redirLog = redirLog;
	this->logHdlr = 0;
	this->logHdlrObj = 0;
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(240, ClientEvent, ClientData, this, 4, ClientTimeout));
	NEW_CLASS(this->svr, Net::TCPServer(this->sockf, 0, port, log, ConnHdlr, this, CSTR_NULL, autoStart));
}

Net::LogServer::~LogServer()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->cliMgr);
	this->logPath->Release();
	UOSInt i;
	NN<IPStatus> status;
	i = this->ipMap.GetCount();
	while (i-- > 0)
	{
		status = this->ipMap.GetItemNoCheck(i);
		DEL_CLASS(status->log);
		MemFreeNN(status);
	}
}

Bool Net::LogServer::Start()
{
	return this->svr->Start();
}

Bool Net::LogServer::IsError()
{
	return this->svr->IsV4Error();
}

void Net::LogServer::HandleClientLog(ClientLogHandler hdlr, AnyType userObj)
{
	this->logHdlrObj = userObj;
	this->logHdlr = hdlr;
}

void Net::LogServer::DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
	UInt8 reply[18];
	UOSInt replySize;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<IPStatus> status;
	NN<ClientStatus> cliStatus = stmObj.GetNN<ClientStatus>();
	switch (cmdType)
	{
	case 0: //KA
		replySize = this->protoHdlr.BuildPacket(reply, 1, seqId, 0, 0, 0);
		stm->Write(Data::ByteArrayR(reply, replySize));
		break;
	case 2: //Log Message
		replySize = this->protoHdlr.BuildPacket(reply, 3, seqId, cmd, 8, 0);
		stm->Write(Data::ByteArrayR(reply, replySize));
		if (this->redirLog)
		{
			Text::StringBuilderUTF8 sb;
			if (this->log->HasHandler())
			{
				sbuff[0] = 0;
				sptr = ((Net::TCPClient *)stm.Ptr())->GetRemoteName(sbuff).Or(sbuff);
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("> "));
				sb.AppendC(&cmd[8], cmdSize - 8);
				this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
			}

			if (cliStatus->status.SetTo(status))
			{
				sb.ClearStr();
				sb.AppendC(&cmd[8], cmdSize - 8);
				status->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);

				if (this->logHdlr)
				{
					this->logHdlr(this->logHdlrObj, status->ip, sb.ToCString());
				}
			}
		}
		else if (cmdSize - 8 < 256)
		{
			if (cliStatus->status.SetTo(status))
			{
				MemCopyNO(sbuff, &cmd[8], cmdSize - 8);
				sbuff[cmdSize - 8] = 0;
				status->log->LogMessage({sbuff, cmdSize - 8}, IO::LogHandler::LogLevel::Command);

				if (this->logHdlr)
				{
					this->logHdlr(this->logHdlrObj, status->ip, {sbuff, cmdSize - 8});
				}
			}
		}
		else
		{
			if (cliStatus->status.SetTo(status))
			{
				UInt8 *tmpPtr = MemAlloc(UInt8, cmdSize - 8 + 1);
				MemCopyNO(tmpPtr, &cmd[8], cmdSize - 8);
				tmpPtr[cmdSize - 8] = 0;
				status->log->LogMessage({tmpPtr, cmdSize - 8}, IO::LogHandler::LogLevel::Command);

				if (this->logHdlr)
				{
					this->logHdlr(this->logHdlrObj, status->ip, {tmpPtr, cmdSize - 8});
				}
				MemFree(tmpPtr);
			}
		}
		break;
	}
}

void Net::LogServer::DataSkipped(NN<IO::Stream> stm, AnyType stmObj, const UInt8 *buff, UOSInt buffSize)
{

}
