#ifndef _SM_NET_MQTTSTATICCLIENT
#define _SM_NET_MQTTSTATICCLIENT
#include "IO/Writer.h"
#include "Net/FailoverChannel.h"
#include "Net/MQTTClient.h"
#include "Net/MQTTConn.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"

namespace Net
{
	class MQTTStaticClient : public Net::FailoverChannel, public Net::MQTTClient
	{
	private:
		Sync::Mutex connMut;
		Net::MQTTConn *conn;
		UInt16 kaSeconds;
		Sync::Thread kaThread;
		UInt16 packetId;
		Sync::Mutex packetIdMut;
		IO::Writer *errLog;

		NotNullPtr<Text::String> clientId;
		Sync::Mutex hdlrMut;
		Data::ArrayList<Net::MQTTConn::PublishMessageHdlr> hdlrList;
		Data::ArrayList<void *> hdlrObjList;
		Sync::Mutex topicMut;
		Data::ArrayList<Text::String*> topicList;

		NotNullPtr<Net::SocketFactory> sockf;
		Net::SSLEngine *ssl;
		Data::Duration connTimeout;
		Text::String *host;
		UInt16 port;
		Text::String *username;
		Text::String *password;
		Bool webSocket;
		Bool autoReconn;

		static void __stdcall KAThread(NotNullPtr<Sync::Thread> thread);
		static void __stdcall OnDisconnect(void *user);
		void Connect();
		UInt16 GetNextPacketId();

		void Init(NotNullPtr<Net::SocketFactory> sockf, Net::MQTTConn::PublishMessageHdlr hdlr, void *hdlrObj, IO::Writer *errLog);
	public:
		MQTTStaticClient(NotNullPtr<Net::SocketFactory> sockf, Net::MQTTConn::PublishMessageHdlr hdlr, void *hdlrObj, IO::Writer *errLog);
		MQTTStaticClient(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, Text::CString username, Text::CString password, Bool webSocket, Net::MQTTConn::PublishMessageHdlr hdlr, void *userObj, UInt16 kaSeconds, IO::Writer *errLog);
		virtual ~MQTTStaticClient();

		Bool IsStarted();
		virtual Bool ChannelFailure();

		virtual void HandlePublishMessage(Net::MQTTConn::PublishMessageHdlr hdlr, void *hdlrObj);
		virtual Bool Subscribe(Text::CString topic);
		virtual Bool Publish(Text::CString topic, Text::CString message);
	};
}
#endif
