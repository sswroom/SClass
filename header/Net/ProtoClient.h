#ifndef _SM_NET_PROTOCLIENT
#define _SM_NET_PROTOCLIENT
#include "IO/IProtocolHandler.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Net
{
	class ProtoClient
	{
	public:
		class IProtoClientHandler
		{
		public:
			virtual void ClientConn() = 0;
			virtual void ClientDisconn() = 0;
		};

	private:
		NN<Net::SocketFactory> sockf;
		Net::TCPClient *cli;
		void *cliData;
		Sync::Mutex cliMut;
		Net::SocketUtil::AddressInfo cliAddr;
		UInt16 cliPort;
		IO::IProtocolHandler *proto;
		IProtoClientHandler *cliHdlr;

		Bool threadRunning;
		Bool threadToStop;
		Sync::Event threadEvt;
		Bool started;
		Bool connected;

		static UInt32 __stdcall ProtoThread(void *userObj);
	public:
		ProtoClient(NN<Net::SocketFactory> sockf, Text::CString cliAddr, UInt16 cliPort, IO::IProtocolHandler *proto, IProtoClientHandler *cliHdlr);
		~ProtoClient();

		void Start();
		void Reconnect();

		Bool IsConnected();
		Bool SendPacket(UInt8 *buff, OSInt buffSize, Int32 cmdType, Int32 seqId);
		Bool SendPacket(UInt8 *buff, OSInt buffSize);
	};
}
#endif
