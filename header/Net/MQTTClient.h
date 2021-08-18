#ifndef _SM_NET_MQTTCLIENT
#define _SM_NET_MQTTCLIENT
#include "Data/ArrayList.h"
#include "IO/ProtoHdlr/ProtoMQTTHandler.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Net
{
	class MQTTClient : public IO::IProtocolHandler::DataListener
	{
	public:
		typedef enum
		{
			CS_TIMEDOUT = -1,
			CS_ACCEPTED = 0,
			CS_VERERR = 1,
			CS_CLIENTID_ERR = 2,
			CS_SVR_UNAVAIL = 3,
			CS_BAD_LOGIN = 4,
			CS_NOT_AUTH = 5
		} ConnectStatus;

		typedef struct
		{
			UInt8 packetType;
			UOSInt size;
			UInt8 content[1];
		} PacketInfo;

		typedef void (__stdcall *PublishMessageHdlr)(void *userObj, const UTF8Char *topic, const UInt8 *buff, UOSInt buffSize);
	private:
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		IO::ProtoHdlr::ProtoMQTTHandler *protoHdlr;
		Net::TCPClient *cli;
		void *cliData;
		Bool recvRunning;
		Bool recvStarted;

		Data::ArrayList<PublishMessageHdlr> *hdlrList;
		Data::ArrayList<void *> *hdlrObjList;

		Data::ArrayList<PacketInfo *> *packetList;
		Sync::Mutex *packetMut;
		Sync::Event *packetEvt;
		UInt64 totalUpload;
		UInt64 totalDownload;

		virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
		virtual void DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);
		static UInt32 __stdcall RecvThread(void *userObj);

		void OnPublishMessage(const UTF8Char *topic, const UInt8 *message, UOSInt msgSize);
		PacketInfo *GetNextPacket(UInt8 packetType, UOSInt timeoutMS);
		Bool SendPacket(const UInt8 *packet, UOSInt packetSize);
	public:
		MQTTClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const Net::SocketUtil::AddressInfo *addr, UInt16 port, Bool sslConn);
		virtual ~MQTTClient();

		void HandlePublishMessage(PublishMessageHdlr hdlr, void *userObj);
		Bool IsError();

		Bool SendConnect(UInt8 protoVer, UInt16 keepAliveS, const UTF8Char *clientId, const UTF8Char *userName, const UTF8Char *password);
		Bool SendPublish(const UTF8Char *topic, const UTF8Char *message);
		Bool SendPubAck(UInt16 packetId);
		Bool SendPubRec(UInt16 packetId);
		Bool SendSubscribe(UInt16 packetId, const UTF8Char *topic);
		Bool SendPing();
		Bool SendDisconnect();

		ConnectStatus WaitConnAck(UOSInt timeoutMS);
		UInt8 WaitSubAck(UInt16 packetId, UOSInt timeoutMS); //0x80 = failure
		void ClearPackets();

		UInt64 GetTotalUpload();
		UInt64 GetTotalDownload();

		static Bool PublishMessage(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UTF8Char *username, const UTF8Char *password, const UTF8Char *topic, const UTF8Char *message);
	};
}
#endif
