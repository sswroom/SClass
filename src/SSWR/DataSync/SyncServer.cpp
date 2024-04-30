#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoSyncHandler.h"
#include "SSWR/DataSync/SyncServer.h"
#include "Sync/MutexUsage.h"
#include "Sync/RWMutexUsage.h"
#include "Text/StringBuilderUTF8.h"

#define BUFFSIZE 10240

typedef struct
{
	Int32 serverId;
	AnyType stmData;
	UOSInt buffSize;
	UInt8 buff[BUFFSIZE];
} ClientData;

void __stdcall SSWR::DataSync::SyncServer::OnClientConn(Socket *s, AnyType userObj)
{
	NN<SSWR::DataSync::SyncServer> me = userObj.GetNN<SSWR::DataSync::SyncServer>();
	NN<Net::TCPClient> cli;
	NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
	ClientData *data = MemAlloc(ClientData, 1);
	data->stmData = me->protoHdlr.CreateStreamData(cli);
	data->buffSize = 0;
	data->serverId = 0;
	me->cliMgr->AddClient(cli, data);
}

void __stdcall SSWR::DataSync::SyncServer::OnClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	NN<SSWR::DataSync::SyncServer> me = userObj.GetNN<SSWR::DataSync::SyncServer>();
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		NN<ClientData> data = cliData.GetNN<ClientData>();
		if (data->serverId != 0)
		{
			NN<ServerInfo> svr;
			Sync::RWMutexUsage svrMutUsage(me->svrMut, false);
			if (me->svrMap.Get(data->serverId).SetTo(svr))
			{
				svrMutUsage.EndUse();
				Sync::MutexUsage mutUsage(svr->mut);
				if (svr->cli == cli.Ptr())
				{
					svr->cli = 0;
				}
				mutUsage.EndUse();
			}
		}
		MemFreeNN(data);
		cli.Delete();
	}
}

void __stdcall SSWR::DataSync::SyncServer::OnClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &srcBuff)
{
	NN<SSWR::DataSync::SyncServer> me = userObj.GetNN<SSWR::DataSync::SyncServer>();
	NN<ClientData> data = cliData.GetNN<ClientData>();
	Data::ByteArrayR buff = srcBuff;
	UOSInt sizeLeft;
	while (data->buffSize + buff.GetSize() > BUFFSIZE)
	{
		MemCopyNO(&data->buff[data->buffSize], buff.Ptr(), BUFFSIZE - data->buffSize);
		buff += BUFFSIZE - data->buffSize;
		data->buffSize = BUFFSIZE;

		sizeLeft = me->protoHdlr.ParseProtocol(cli, data, data->stmData, Data::ByteArrayR(data->buff, data->buffSize));
		if (sizeLeft == 0)
		{
			data->buffSize = 0;
		}
		else if (sizeLeft <= data->buffSize)
		{
			MemCopyO(data->buff, &data->buff[data->buffSize - sizeLeft], sizeLeft);
			data->buffSize = sizeLeft;
		}
	}
	MemCopyNO(&data->buff[data->buffSize], buff.Ptr(), buff.GetSize());
	data->buffSize += buff.GetSize();
	sizeLeft = me->protoHdlr.ParseProtocol(cli, data, data->stmData, Data::ByteArrayR(data->buff, data->buffSize));
	if (sizeLeft == 0)
	{
		data->buffSize = 0;
	}
	else if (sizeLeft <= data->buffSize)
	{
		MemCopyO(data->buff, &data->buff[data->buffSize - sizeLeft], sizeLeft);
		data->buffSize = sizeLeft;
	}
}

void __stdcall SSWR::DataSync::SyncServer::OnClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{

}

SSWR::DataSync::SyncServer::SyncServer(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log, UInt16 port, Int32 serverId, Text::CString serverName, Text::CString syncClients, DataHandler dataHdlr, AnyType dataObj, Bool autoStart, Data::Duration cliTimeout) : protoHdlr(*this)
{
	this->sockf = sockf;
	this->dataHdlr = dataHdlr;
	this->dataObj = dataObj;
	NN<ServerInfo> svrInfo;
	NEW_CLASSNN(svrInfo, ServerInfo());
	svrInfo->serverId = serverId;
	svrInfo->serverName = Text::StrCopyNewC(serverName.v, serverName.leng).Ptr();
	svrInfo->isLocal = true;
	svrInfo->cli = 0;
	this->svrMap.Put(svrInfo->serverId, svrInfo);

	if (syncClients.leng > 0)
	{
		UOSInt i;
		UOSInt j;
		Text::PString sarr[2];
		Text::PString sarr2[2];
		NN<SyncClient> syncCli;
		Text::StringBuilderUTF8 sb;
		UInt16 port;
		sb.Append(syncClients);
		sarr[1] = sb;
		while (true)
		{
			i = Text::StrSplitP(sarr, 2, sarr[1], ',');
			j = Text::StrSplitP(sarr2, 2, sarr[0], ':');
			if (j == 2)
			{
				if (Text::StrToUInt16(sarr2[1].v, port))
				{
					if (port > 0)
					{
						NEW_CLASSNN(syncCli, SyncClient(this->sockf, serverId, serverName, sarr2[0].ToCString(), port, cliTimeout));
						this->syncCliList.Add(syncCli);
					}
				}
			}
			if (i != 2)
			{
				break;
			}
		}
	}
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(240, OnClientEvent, OnClientData, this, 2, OnClientTimeout));
	NEW_CLASS(this->svr, Net::TCPServer(sockf, 0, port, log, OnClientConn, this, CSTR("Sync: "), autoStart));
}

SSWR::DataSync::SyncServer::~SyncServer()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->cliMgr);
	UOSInt i;
	NN<ServerInfo> svrInfo;
	i = this->svrMap.GetCount();
	while (i-- > 0)
	{
		svrInfo = this->svrMap.GetItemNoCheck(i);
		Text::StrDelNew(svrInfo->serverName);
		svrInfo.Delete();
	}
	NN<SyncClient> syncCli;
	i = this->syncCliList.GetCount();
	while (i-- > 0)
	{
		syncCli = this->syncCliList.GetItemNoCheck(i);
		syncCli.Delete();
	}
}

Bool SSWR::DataSync::SyncServer::Start()
{
	return this->svr != 0 && this->svr->Start();
}

Bool SSWR::DataSync::SyncServer::IsError()
{
	return this->svr == 0 || this->svr->IsV4Error();
}

UOSInt SSWR::DataSync::SyncServer::GetServerList(NN<Data::ArrayListNN<ServerInfo>> svrList)
{
	UOSInt i = svrList->GetCount();
	svrList->AddAll(this->svrMap);
	return svrList->GetCount() - i;
}

void SSWR::DataSync::SyncServer::SendUserData(const UInt8 *dataBuff, UOSInt dataSize)
{
	UOSInt i = this->syncCliList.GetCount();
	while (i-- > 0)
	{
		this->syncCliList.GetItemNoCheck(i)->AddUserData(dataBuff, dataSize);
	}
}

void SSWR::DataSync::SyncServer::DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
	NN<ClientData> data = stmObj.GetNN<ClientData>();
	UInt8 replyBuff[20];
	UOSInt replySize;
	switch (cmdType)
	{
	case 0: //register
		{
			NN<ServerInfo> svr;
			if (data->serverId == 0 && cmdSize > 5 && (UOSInt)(cmd[4] + 4) <= cmdSize)
			{
				Int32 serverId = ReadInt32(cmd);
				Sync::RWMutexUsage svrMutUsage(this->svrMut, false);
				if (this->svrMap.Get(serverId).SetTo(svr))
				{
					svrMutUsage.EndUse();
					Sync::MutexUsage mutUsage(svr->mut);
					svr->cli = (Net::TCPClient*)stm.Ptr();
					mutUsage.EndUse();
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					NEW_CLASSNN(svr, ServerInfo());
					svr->serverId = serverId;
					sb.AppendC((const UTF8Char*)&cmd[5], cmd[4]);
					svr->serverName = Text::StrCopyNew(sb.ToString()).Ptr();
					svr->isLocal = false;
					svr->cli = (Net::TCPClient*)stm.Ptr();
					this->svrMap.Put(serverId, svr);
					svrMutUsage.EndUse();
				}
				data->serverId = serverId;
			}
			replySize = this->protoHdlr.BuildPacket(replyBuff, 1, seqId, 0, 0, 0);
			stm->Write(replyBuff, replySize);
		}
		break;
	case 2: //ka
		{
			replySize = this->protoHdlr.BuildPacket(replyBuff, 3, seqId, 0, 0, 0);
			stm->Write(replyBuff, replySize);
		}
		break;
	case 4: //User Data
		{
			if (this->dataHdlr)
			{
				this->dataHdlr(this->dataObj, cmd, cmdSize);
			}
		}
		break;
	}
}

void SSWR::DataSync::SyncServer::DataSkipped(NN<IO::Stream> stm, AnyType stmObj, const UInt8 *buff, UOSInt buffSize)
{
}
