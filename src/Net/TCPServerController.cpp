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
	UIntOS copySize;

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

Net::TCPServerController::TCPServerController(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log, UInt16 port, Text::CString prefix, UIntOS maxBuffSize, Net::TCPServerController::TCPServerHandler *hdlr, UIntOS workerCnt, Int32 timeoutSec, Bool autoStart)
{
	this->sockf = sockf;
	this->maxBuffSize = maxBuffSize;
	this->hdlr = hdlr;
	NEW_CLASSNN(this->cliMgr, Net::TCPClientMgr(timeoutSec, EventHdlr, DataHdlr, this, workerCnt, TimeoutHdlr));
	NN<Net::TCPServer> svr;
	NEW_CLASSNN(svr, Net::TCPServer(sockf, nullptr, port, log, ConnHdlr, this, prefix, autoStart));
	if (svr->IsV4Error())
	{
		svr.Delete();
		this->svr = nullptr;
		return;
	}
	else
	{
		this->svr = svr;
	}
}

Net::TCPServerController::~TCPServerController()
{
	this->svr.Delete();
	this->cliMgr.Delete();
}

Bool Net::TCPServerController::Start()
{
	NN<Net::TCPServer> svr;
	return this->maxBuffSize > 0 && this->svr.SetTo(svr) && svr->Start();
}

Bool Net::TCPServerController::IsError()
{
	return this->svr.IsNull() || this->maxBuffSize <= 0;
}

void Net::TCPServerController::UseGetCli(NN<Sync::MutexUsage> mutUsage)
{
	this->cliMgr->UseGetClient(mutUsage);
}

UIntOS Net::TCPServerController::GetCliCount()
{
	return this->cliMgr->GetClientCount();
}

Optional<Net::TCPClient> Net::TCPServerController::GetClient(UIntOS index, OutParam<AnyType> cliObj)
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
