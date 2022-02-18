#ifndef _SM_NET_MQTTBROKER
#define _SM_NET_MQTTBROKER
#include "Data/FastStringMap.h"
#include "Data/StringUTF8Map.h"
#include "IO/ProtoHdlr/ProtoMQTTHandler.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"
#include "Text/String.h"

namespace Net
{
	class MQTTBroker : public IO::IProtocolHandler::DataListener
	{
	public:
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
			Net::TCPClient *cli;
			void *cliData;
		};
		

		typedef ConnectStatus (__stdcall *ConnectHandler)(void *userObj, Text::String *clientId, Text::String *userName, Text::String *password, const Net::SocketUtil::AddressInfo *addr);
		typedef void (__stdcall *PublishHandler)(void *userObj, Text::CString topic, UInt16 packetId, const UInt8 *message, UOSInt msgSize);
		typedef ConnectStatus (__stdcall *SubscribeHandler)(void *userObj, Text::String *clientId, Text::CString topic);
		typedef void (__stdcall *TopicUpdateHandler)(void *userObj, Text::CString topic, const UInt8 *message, UOSInt msgSize);
	private:
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		IO::LogTool *log;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;
		IO::ProtoHdlr::ProtoMQTTHandler *protoHdlr;
		Sync::Mutex *topicMut;
		Data::FastStringMap<TopicInfo*> *topicMap;
		Sync::Mutex *subscribeMut;
		Data::ArrayList<SubscribeInfo*> *subscribeList;
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
		Sync::Event *sysInfoEvt;

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
	public:
		MQTTBroker(Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 port, IO::LogTool *log, Bool sysInfo);
		virtual ~MQTTBroker();

		Bool IsError();
		void HandleConnect(ConnectHandler connHdlr, void *userObj);
		void HandlePublish(PublishHandler publishHdlr, void *userObj);
		void HandleSubscribe(SubscribeHandler subscribeHdlr, void *userObj);
		void HandleTopicUpdate(TopicUpdateHandler topicUpdHdlr, void *userObj);
	};
}
#endif
