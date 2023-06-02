#ifndef _SM_NET_MQTTBROKER
#define _SM_NET_MQTTBROKER
#include "Data/FastStringMap.h"
#include "Data/StringUTF8Map.h"
#include "IO/ProtoHdlr/ProtoMQTTHandler.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebSocketHandler.h"
#include "Text/String.h"

namespace Net
{
	class MQTTBroker : public IO::IProtocolHandler::DataListener, public IO::StreamHandler
	{
	public:
		struct Listener
		{
			Net::SSLEngine *ssl;
			Net::TCPServer *svr;
			Net::TCPClientMgr *cliMgr;
			Net::WebServer::WebListener *listener;
			MQTTBroker *me;
		};
		typedef enum
		{
			CS_ACCEPTED = 0,
			CS_UNSPECIFIED_ERROR = 128,
			CS_MALFORMED_PACKET = 129,
			CS_PROTOCOL_ERROR = 130,
			CS_IMPL_ERROR = 131,
			CS_UNSUPP_MQTT_VER = 132,
			CS_CLI_ID_NOT_VALID = 133,
			CS_BAD_USER_NAME_OR_PWD = 134,
			CS_NOT_AUTHORIZED = 135,
			CS_SERVER_UNAVAILABLE = 136,
			CS_SERVER_BUSY = 137
		} ConnectStatus;

		struct TopicInfo
		{
			Text::String *topic;
			UInt8 *message;
			UOSInt msgSize;
		};
		
		struct SubscribeInfo
		{
			Text::String *topic;
			IO::Stream *stm;
			void *cliData;
		};
		

		typedef ConnectStatus (__stdcall *ConnectHandler)(void *userObj, Text::String *clientId, Text::String *userName, Text::String *password, const Net::SocketUtil::AddressInfo *addr);
		typedef void (__stdcall *PublishHandler)(void *userObj, Text::CString topic, UInt16 packetId, const UInt8 *message, UOSInt msgSize);
		typedef ConnectStatus (__stdcall *SubscribeHandler)(void *userObj, Text::String *clientId, Text::CString topic);
		typedef void (__stdcall *TopicUpdateHandler)(void *userObj, Text::CString topic, const UInt8 *message, UOSInt msgSize);
	private:
		Net::SocketFactory *sockf;
		IO::LogTool *log;
		Data::ArrayList<Listener*> listeners;
		IO::ProtoHdlr::ProtoMQTTHandler protoHdlr;
		Net::WebServer::WebSocketHandler wsHdlr;
		Sync::Mutex topicMut;
		Data::FastStringMap<TopicInfo*> topicMap;
		Sync::Mutex subscribeMut;
		Data::ArrayList<SubscribeInfo*> subscribeList;
		Int64 infoTotalRecv;
		UInt64 infoTotalSent;
		Int64 infoCliDisconn;
		UInt64 infoCliMax;
		Int64 infoMsgRecv;
		Int64 infoMsgSent;
		Int64 infoPubRecv;
		Int64 infoPubSent;
		Int64 infoPubDrop;
		Int64 infoStartTime;
		Bool sysInfoRunning;
		Bool sysInfoToStop;
		Sync::Event sysInfoEvt;

		ConnectHandler connHdlr;
		void *connObj;
		PublishHandler publishHdlr;
		void *publishObj;
		SubscribeHandler subscribeHdlr;
		void *subscribeObj;
		TopicUpdateHandler topicUpdHdlr;
		void *topicUpdObj;

		static void __stdcall OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
		static void __stdcall OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);
		static void __stdcall OnClientReady(Net::TCPClient *cli, void *userObj);
		static void __stdcall OnClientConn(Socket *s, void *userObj);
		
		static UInt32 __stdcall SysInfoThread(void *userObj);

		virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
		virtual void DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);
		void UpdateTopic(Text::CString topic, const UInt8 *message, UOSInt msgSize, Bool suppressUnchg);
		Bool TopicSend(IO::Stream *stm, void *stmData, const TopicInfo *topic);

		virtual void *StreamCreated(IO::Stream *stm);
		virtual void StreamData(IO::Stream *stm, void *stmData, const UInt8 *buff, UOSInt size);
		virtual void StreamClosed(IO::Stream *stm, void *stmData);
	public:
		MQTTBroker(Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 port, IO::LogTool *log, Bool sysInfo, Bool autoStart);
		virtual ~MQTTBroker();

		Bool AddListener(Net::SSLEngine *ssl, UInt16 port, Bool autoStart);
		Bool AddWSListener(Net::SSLEngine *ssl, UInt16 port, Bool autoStart);

		Bool Start();
		Bool IsError();
		void HandleConnect(ConnectHandler connHdlr, void *userObj);
		void HandlePublish(PublishHandler publishHdlr, void *userObj);
		void HandleSubscribe(SubscribeHandler subscribeHdlr, void *userObj);
		void HandleTopicUpdate(TopicUpdateHandler topicUpdHdlr, void *userObj);
	};
}
#endif
