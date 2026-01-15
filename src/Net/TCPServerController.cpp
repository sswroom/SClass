#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/TCPServerController.h"

void __stdcall Net::TCPServerController::ConnHdlr(NN<Socket> s, AnyType userObj)
{
	NN<Net::TCPClient> cli;
	Net::TCPServerController::ClientData *data;
	NN<Net::TCPServerController> me = userObj.GetNN<Net::TCPServerController>();
	NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
	data = MemAlloc(Net::TCPServerController::ClientData, 1);
	data->buffSize = 0;
	data->buff = MemAlloc(UInt8, me->maxBuffSize);
	me->cliMgr->AddClient(cli, data);
	data->cliObj = me->hdlr->NewConn(cli);
}

void __stdcall Net::TCPServerController::EventHdlr(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	NN<Net::TCPServerController> me = userObj.GetNN<Net::TCPServerController>();
	NN<Net::TCPServerController::ClientData> data = cliData.GetNN<Net::TCPServerController::ClientData>();

	if (evtType == Net::TCPClientMgr::TCP_EVENT_HASDATA)
	{
	}
	else if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		me->hdlr->EndConn(cli, data->cliObj);
		MemFree(data->buff);
		MemFree(data.Ptr());
		cli.Delete();
	}
}

void __stdcall Net::TCPServerController::DataHdlr(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &srcBuff)
{
	NN<Net::TCPServerController> me = userObj.GetNN<Net::TCPServerController>();
	NN<Net::TCPServerController::ClientData> data = cliData.GetNN<Net::TCPServerController::ClientData>();
	UOSInt copySize;

	Data::ByteArrayR buff = srcBuff;
	while (buff.GetSize() > 0)
	{
		copySize = buff.GetSize();
		if (copySize + data->buffSize > me->maxBuffSize)
		{
			copySize = me->maxBuffSize - data->buffSize;
		}
		MemCopyNO(&data->buff[data->buffSize], buff.Arr().Ptr(), copySize);
		buff += copySize;
		data->buffSize += copySize;

		copySize = me->hdlr->ReceivedData(cli, data->cliObj, Data::ByteArrayR(data->buff, data->buffSize));
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

void __stdcall Net::TCPServerController::TimeoutHdlr(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{
}

Net::TCPServerController::TCPServerController(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log, UInt16 port, Text::CString prefix, UOSInt maxBuffSize, Net::TCPServerController::TCPServerHandler *hdlr, UOSInt workerCnt, Int32 timeoutSec, Bool autoStart)
{
	this->cliMgr = 0;
	this->sockf = sockf;
	this->maxBuffSize = maxBuffSize;
	this->hdlr = hdlr;
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(timeoutSec, EventHdlr, DataHdlr, this, workerCnt, TimeoutHdlr));
	NEW_CLASS(this->svr, Net::TCPServer(sockf, nullptr, port, log, ConnHdlr, this, prefix, autoStart));
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

void Net::TCPServerController::UseGetCli(NN<Sync::MutexUsage> mutUsage)
{
	this->cliMgr->UseGetClient(mutUsage);
}

UOSInt Net::TCPServerController::GetCliCount()
{
	return this->cliMgr->GetClientCount();
}

Optional<Net::TCPClient> Net::TCPServerController::GetClient(UOSInt index, OutParam<AnyType> cliObj)
{
	AnyType cliData;
	NN<Net::TCPServerController::ClientData> data;
	NN<Net::TCPClient> cli;
	if (!this->cliMgr->GetClient(index, cliData).SetTo(cli))
		return nullptr;
	data = cliData.GetNN<Net::TCPServerController::ClientData>();
	cliObj.Set(data->cliObj);
	return cli;
}
