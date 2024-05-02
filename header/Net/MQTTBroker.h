#ifndef _SM_NET_MQTTBROKER
#define _SM_NET_MQTTBROKER
#include "AnyType.h"
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
			Optional<Net::SSLEngine> ssl;
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
			NN<Text::String> topic;
			UInt8 *message;
			UOSInt msgSize;
		};
		
		struct SubscribeInfo
		{
			NN<Text::String> topic;
			NN<IO::Stream> stm;
			AnyType cliData;
		};
		

		typedef ConnectStatus (__stdcall *ConnectHandler)(AnyType userObj, Text::String *clientId, Text::String *userName, Text::String *password, const Net::SocketUtil::AddressInfo *addr);
		typedef void (__stdcall *PublishHandler)(AnyType userObj, Text::CString topic, UInt16 packetId, const UInt8 *message, UOSInt msgSize);
		typedef ConnectStatus (__stdcall *SubscribeHandler)(AnyType userObj, Text::String *clientId, Text::CString topic);
		typedef void (__stdcall *TopicUpdateHandler)(AnyType userObj, Text::CString topic, const UInt8 *message, UOSInt msgSize);
	private:
		NN<Net::SocketFactory> sockf;
		NN<IO::LogTool> log;
		Data::ArrayListNN<Listener> listeners;
		IO::ProtoHdlr::ProtoMQTTHandler protoHdlr;
		Net::WebServer::WebSocketHandler wsHdlr;
		Sync::Mutex topicMut;
		Data::FastStringMapNN<TopicInfo> topicMap;
		Sync::Mutex subscribeMut;
		Data::ArrayListNN<SubscribeInfo> subscribeList;
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
		AnyType connObj;
		PublishHandler publishHdlr;
		AnyType publishObj;
		SubscribeHandler subscribeHdlr;
		AnyType subscribeObj;
		TopicUpdateHandler topicUpdHdlr;
		AnyType topicUpdObj;

		static void __stdcall OnClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall OnClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
		static void __stdcall OnClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
		static void __stdcall OnClientReady(NN<Net::TCPClient> cli, AnyType userObj);
		static void __stdcall OnClientConn(Socket *s, AnyType userObj);
		
		static UInt32 __stdcall SysInfoThread(AnyType userObj);

		virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
		virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, const UInt8 *buff, UOSInt buffSize);
		void UpdateTopic(Text::CStringNN topic, const UInt8 *message, UOSInt msgSize, Bool suppressUnchg);
		Bool TopicSend(NN<IO::Stream> stm, AnyType stmData, NN<const TopicInfo> topic);

		virtual AnyType StreamCreated(NN<IO::Stream> stm);
		virtual void StreamData(NN<IO::Stream> stm, AnyType stmData, const Data::ByteArrayR &buff);
		virtual void StreamClosed(NN<IO::Stream> stm, AnyType stmData);
	public:
		MQTTBroker(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, UInt16 port, NN<IO::LogTool> log, Bool sysInfo, Bool autoStart);
		virtual ~MQTTBroker();

		Bool AddListener(Optional<Net::SSLEngine> ssl, UInt16 port, Bool autoStart);
		Bool AddWSListener(Optional<Net::SSLEngine> ssl, UInt16 port, Bool autoStart);

		Bool Start();
		Bool IsError();
		void HandleConnect(ConnectHandler connHdlr, AnyType userObj);
		void HandlePublish(PublishHandler publishHdlr, AnyType userObj);
		void HandleSubscribe(SubscribeHandler subscribeHdlr, AnyType userObj);
		void HandleTopicUpdate(TopicUpdateHandler topicUpdHdlr, AnyType userObj);
	};
}
#endif
