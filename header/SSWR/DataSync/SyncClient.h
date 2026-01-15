#ifndef _SM_SSWR_DATASYNC_SYNCCLIENT
#define _SM_SSWR_DATASYNC_SYNCCLIENT
#include "AnyType.h"
#include "Data/DateTime.h"
#include "IO/ProtoHdlr/ProtoSyncHandler.h"
#include "Net/TCPClient.h"
#include "SSWR/DataSync/SyncClientDataMgr.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace SSWR
{
	namespace DataSync
	{
		class SyncClient : public IO::ProtocolHandler::DataListener
		{
		private:
			NN<Net::SocketFactory> sockf;
			IO::ProtoHdlr::ProtoSyncHandler protoHdlr;
			Int32 serverId;
			NN<Text::String> serverName;
			Sync::Mutex cliMut;
			Net::TCPClient *cli;
			NN<Text::String> cliHost;
			UInt16 cliPort;
			Data::DateTime cliKATime;
			Data::Duration timeout;

			Sync::Event recvEvt;
			Sync::Event kaEvt;
			Bool recvRunning;
			Bool kaRunning;
			Bool toStop;

			SSWR::DataSync::SyncClientDataMgr dataMgr;

			static UInt32 __stdcall RecvThread(AnyType userObj);
			static UInt32 __stdcall KAThread(AnyType userObj);

			Bool SendLogin();
			Bool SendKA();
			Bool SendUserData(const UInt8 *data, UIntOS dataSize);
		public:
			SyncClient(NN<Net::SocketFactory> sockf, Int32 serverId, Text::CStringNN serverName, Text::CStringNN clientHost, UInt16 cliPort, Data::Duration timeout);
			virtual ~SyncClient();

			virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UIntOS cmdSize);
			virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UIntOS buffSize);

			void AddUserData(const UInt8 *data, UIntOS dataSize);
		};
	}
}
#endif
