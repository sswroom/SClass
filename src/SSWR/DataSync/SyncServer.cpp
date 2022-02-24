#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoSyncHandler.h"
#include "SSWR/DataSync/SyncServer.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

typedef struct
{
	Int32 serverId;
	void *stmData;
	UOSInt buffSize;
	UInt8 buff[10240];
} ClientData;

void __stdcall SSWR::DataSync::SyncServer::OnClientConn(Socket *s, void *userObj)
{
	SSWR::DataSync::SyncServer *me = (SSWR::DataSync::SyncServer *)userObj;
	Net::TCPClient *cli;
	NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
	ClientData *data = MemAlloc(ClientData, 1);
	data->stmData = me->protoHdlr->CreateStreamData(cli);
	data->buffSize = 0;
	data->serverId = 0;
	me->cliMgr->AddClient(cli, data);
}

void __stdcall SSWR::DataSync::SyncServer::OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	SSWR::DataSync::SyncServer *me = (SSWR::DataSync::SyncServer *)userObj;
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		ClientData *data = (ClientData*)cliData;
		if (data->serverId != 0)
		{
			ServerInfo *svr;
			me->svrMut->LockRead();
			svr = me->svrMap->Get(data->serverId);
			me->svrMut->UnlockRead();
			if (svr)
			{
				Sync::MutexUsage mutUsage(svr->mut);
				if (svr->cli == cli)
				{
					svr->cli = 0;
				}
				mutUsage.EndUse();
			}
		}
		MemFree(data);
		DEL_CLASS(cli);
	}
}

void __stdcall SSWR::DataSync::SyncServer::OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
	SSWR::DataSync::SyncServer *me = (SSWR::DataSync::SyncServer *)userObj;
	ClientData *data = (ClientData*)cliData;
	MemCopyNO(&data->buff[data->buffSize], buff, size);
	data->buffSize += size;
	UOSInt sizeLeft = me->protoHdlr->ParseProtocol(cli, data, data->stmData, data->buff, data->buffSize);
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

void __stdcall SSWR::DataSync::SyncServer::OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData)
{

}

SSWR::DataSync::SyncServer::SyncServer(Net::SocketFactory *sockf, IO::LogTool *log, UInt16 port, Int32 serverId, Text::CString serverName, Text::CString syncClients, DataHandler dataHdlr, void *dataObj)
{
	this->sockf = sockf;
	NEW_CLASS(this->protoHdlr, IO::ProtoHdlr::ProtoSyncHandler(this));
	NEW_CLASS(this->svrMut, Sync::RWMutex());
	NEW_CLASS(this->svrMap, Data::Int32Map<ServerInfo*>());
	NEW_CLASS(this->syncCliList, Data::ArrayList<SyncClient*>());
	this->dataHdlr = dataHdlr;
	this->dataObj = dataObj;
	ServerInfo *svrInfo;
	svrInfo = MemAlloc(ServerInfo, 1);
	NEW_CLASS(svrInfo->mut, Sync::Mutex());
	svrInfo->serverId = serverId;
	svrInfo->serverName = Text::StrCopyNewC(serverName.v, serverName.leng);
	svrInfo->isLocal = true;
	svrInfo->cli = 0;
	this->svrMap->Put(svrInfo->serverId, svrInfo);

	if (syncClients.leng > 0)
	{
		UOSInt i;
		UOSInt j;
		Text::PString sarr[2];
		Text::PString sarr2[2];
		SyncClient *syncCli;
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
				if (Text::StrToUInt16(sarr2[1].v, &port))
				{
					if (port > 0 && port < 65536)
					{
						NEW_CLASS(syncCli, SyncClient(this->sockf, serverId, serverName, sarr2[0].ToCString(), port));
						this->syncCliList->Add(syncCli);
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
	NEW_CLASS(this->svr, Net::TCPServer(sockf, port, log, OnClientConn, this, CSTR("Sync: ")));
}

SSWR::DataSync::SyncServer::~SyncServer()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->cliMgr);
	DEL_CLASS(this->protoHdlr);
	UOSInt i;
	ServerInfo *svrInfo;
	Data::ArrayList<ServerInfo*> *svrList = this->svrMap->GetValues();
	i = svrList->GetCount();
	while (i-- > 0)
	{
		svrInfo = svrList->GetItem(i);
		DEL_CLASS(svrInfo->mut);
		Text::StrDelNew(svrInfo->serverName);
		MemFree(svrInfo);
	}
	DEL_CLASS(this->svrMut);
	DEL_CLASS(this->svrMap);
	SyncClient *syncCli;
	i = this->syncCliList->GetCount();
	while (i-- > 0)
	{
		syncCli = this->syncCliList->GetItem(i);
		DEL_CLASS(syncCli);
	}
	DEL_CLASS(this->syncCliList);
}

Bool SSWR::DataSync::SyncServer::IsError()
{
	return this->svr == 0 || this->svr->IsV4Error();
}

UOSInt SSWR::DataSync::SyncServer::GetServerList(Data::ArrayList<ServerInfo*> *svrList)
{
	UOSInt i = svrList->GetCount();
	svrList->AddAll(this->svrMap->GetValues());
	return svrList->GetCount() - i;
}

void SSWR::DataSync::SyncServer::SendUserData(const UInt8 *dataBuff, UOSInt dataSize)
{
	UOSInt i = this->syncCliList->GetCount();
	while (i-- > 0)
	{
		this->syncCliList->GetItem(i)->AddUserData(dataBuff, dataSize);
	}
}

void SSWR::DataSync::SyncServer::DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
	ClientData *data = (ClientData*)stmObj;
	UInt8 replyBuff[20];
	UOSInt replySize;
	switch (cmdType)
	{
	case 0: //register
		{
			ServerInfo *svr;
			if (data->serverId == 0 && cmdSize > 5 && (UOSInt)(cmd[4] + 4) <= cmdSize)
			{
				Int32 serverId = ReadInt32(cmd);
				this->svrMut->LockRead();
				svr = this->svrMap->Get(serverId);
				this->svrMut->UnlockRead();
				if (svr)
				{
					Sync::MutexUsage mutUsage(svr->mut);
					svr->cli = (Net::TCPClient*)stm;
					mutUsage.EndUse();
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					svr = MemAlloc(ServerInfo, 1);
					svr->serverId = serverId;
					NEW_CLASS(svr->mut, Sync::Mutex());
					sb.AppendC((const UTF8Char*)&cmd[5], cmd[4]);
					svr->serverName = Text::StrCopyNew(sb.ToString());
					svr->isLocal = false;
					svr->cli = (Net::TCPClient*)stm;
					this->svrMut->LockWrite();
					this->svrMap->Put(serverId, svr);
					this->svrMut->UnlockWrite();
				}
				data->serverId = serverId;
			}
			replySize = this->protoHdlr->BuildPacket(replyBuff, 1, seqId, 0, 0, 0);
			stm->Write(replyBuff, replySize);
		}
		break;
	case 2: //ka
		{
			replySize = this->protoHdlr->BuildPacket(replyBuff, 3, seqId, 0, 0, 0);
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

void SSWR::DataSync::SyncServer::DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}
