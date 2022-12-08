#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/TCPServerController.h"

void __stdcall Net::TCPServerController::ConnHdlr(Socket *s, void *userObj)
{
	Net::TCPClient *cli;
	Net::TCPServerController::ClientData *data;
	Net::TCPServerController *me = (Net::TCPServerController *)userObj;
	NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
	data = MemAlloc(Net::TCPServerController::ClientData, 1);
	data->buffSize = 0;
	data->buff = MemAlloc(UInt8, me->maxBuffSize);
	me->cliMgr->AddClient(cli, data);
	data->cliObj = me->hdlr->NewConn(cli);
}

void __stdcall Net::TCPServerController::EventHdlr(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	Net::TCPServerController *me = (Net::TCPServerController*)userObj;
	Net::TCPServerController::ClientData *data = (Net::TCPServerController::ClientData*)cliData;

	if (evtType == Net::TCPClientMgr::TCP_EVENT_HASDATA)
	{
	}
	else if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		me->hdlr->EndConn(cli, data->cliObj);
		MemFree(data->buff);
		MemFree(data);
		DEL_CLASS(cli);
	}
}

void __stdcall Net::TCPServerController::DataHdlr(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
	Net::TCPServerController *me = (Net::TCPServerController*)userObj;
	Net::TCPServerController::ClientData *data = (Net::TCPServerController::ClientData*)cliData;
	UOSInt copySize;

	while (size > 0)
	{
		copySize = size;
		if (copySize + data->buffSize > me->maxBuffSize)
		{
			copySize = me->maxBuffSize - data->buffSize;
		}
		MemCopyNO(&data->buff[data->buffSize], buff, copySize);
		size -= copySize;
		buff += copySize;
		data->buffSize += copySize;

		copySize = me->hdlr->ReceivedData(cli, data->cliObj, data->buff, data->buffSize);
		if (copySize >= me->maxBuffSize || copySize <= 0)
		{
			data->buffSize = 0;
		}
		else if (copySize != data->buffSize)
		{
			MemCopyO(data->buff, &data->buff[data->buffSize - copySize], copySize);
			data->buffSize = copySize;
		}
	}
}

void __stdcall Net::TCPServerController::TimeoutHdlr(Net::TCPClient *cli, void *userObj, void *cliData)
{
}

Net::TCPServerController::TCPServerController(Net::SocketFactory *sockf, IO::LogTool *log, UInt16 port, Text::CString prefix, UOSInt maxBuffSize, Net::TCPServerController::TCPServerHandler *hdlr, UOSInt workerCnt, Int32 timeoutSec, Bool autoStart)
{
	this->cliMgr = 0;
	this->sockf = sockf;
	this->maxBuffSize = maxBuffSize;
	this->hdlr = hdlr;
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(timeoutSec, EventHdlr, DataHdlr, this, workerCnt, TimeoutHdlr));
	NEW_CLASS(this->svr, Net::TCPServer(sockf, port, log, ConnHdlr, this, prefix, autoStart));
	if (this->svr->IsV4Error())
	{
		DEL_CLASS(this->svr);
		this->svr = 0;
		return;
	}
}

Net::TCPServerController::~TCPServerController()
{
	if (this->svr)
	{
		DEL_CLASS(this->svr);
		this->svr = 0;
	}
	if (this->cliMgr)
	{
		DEL_CLASS(this->cliMgr);
		this->cliMgr = 0;
	}
}

Bool Net::TCPServerController::Start()
{
	return this->maxBuffSize > 0 && this->svr != 0 && this->svr->Start();
}

Bool Net::TCPServerController::IsError()
{
	return this->svr == 0 || this->maxBuffSize <= 0;
}

void Net::TCPServerController::UseGetCli(Sync::MutexUsage *mutUsage)
{
	this->cliMgr->UseGetClient(mutUsage);
}

UOSInt Net::TCPServerController::GetCliCount()
{
	return this->cliMgr->GetClientCount();
}

Net::TCPClient *Net::TCPServerController::GetClient(UOSInt index, void **cliObj)
{
	void *cliData;
	Net::TCPServerController::ClientData *data;
	Net::TCPClient *cli = this->cliMgr->GetClient(index, &cliData);
	if (cli == 0)
		return 0;
	data = (Net::TCPServerController::ClientData*)cliData;
	*cliObj = data->cliObj;
	return cli;
}
