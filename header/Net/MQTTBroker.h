#ifndef _SM_NET_MQTTBROKER
#define _SM_NET_MQTTBROKER
#include "AnyType.h"
#include "Data/FastStringMapNN.hpp"
#include "Data/StringUTF8Map.hpp"
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
	class MQTTBroker : public IO::ProtocolHandler::DataListener, public IO::StreamHandler
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
			UIntOS msgSize;
		};
		
		struct SubscribeInfo
		{
			NN<Text::String> topic;
			NN<IO::Stream> stm;
			AnyType cliData;
		};
		

		typedef ConnectStatus (CALLBACKFUNC ConnectHandler)(AnyType userObj, NN<Text::String> clientId, Optional<Text::String> userName, Optional<Text::String> password, NN<const Net::SocketUtil::AddressInfo> addr);
		typedef void (CALLBACKFUNC PublishHandler)(AnyType userObj, Text::CStringNN topic, UInt16 packetId, UnsafeArray<const UInt8> message, UIntOS msgSize);
		typedef ConnectStatus (CALLBACKFUNC SubscribeHandler)(AnyType userObj, NN<Text::String> clientId, Text::CStringNN topic);
		typedef void (CALLBACKFUNC TopicUpdateHandler)(AnyType userObj, Text::CStringNN topic, UnsafeArray<const UInt8> message, UIntOS msgSize);
	private:
		NN<Net::TCPClientFactory> clif;
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
		static void __stdcall OnClientConn(NN<Socket> s, AnyType userObj);
		
		static UInt32 __stdcall SysInfoThread(AnyType userObj);

		virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UIntOS cmdSize);
		virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UIntOS buffSize);
		void UpdateTopic(Text::CStringNN topic, UnsafeArray<const UInt8> message, UIntOS msgSize, Bool suppressUnchg);
		Bool TopicSend(NN<IO::Stream> stm, AnyType stmData, NN<const TopicInfo> topic);

		virtual AnyType StreamCreated(NN<IO::Stream> stm);
		virtual void StreamData(NN<IO::Stream> stm, AnyType stmData, const Data::ByteArrayR &buff);
		virtual void StreamClosed(NN<IO::Stream> stm, AnyType stmData);
	public:
		MQTTBroker(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, UInt16 port, NN<IO::LogTool> log, Bool sysInfo, Bool autoStart);
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
