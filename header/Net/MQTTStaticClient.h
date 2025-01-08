#ifndef _SM_NET_MQTTSTATICCLIENT
#define _SM_NET_MQTTSTATICCLIENT
#include "AnyType.h"
#include "Data/CallbackStorage.h"
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
		Optional<Net::MQTTConn> conn;
		UInt16 kaSeconds;
		Sync::Thread kaThread;
		UInt16 packetId;
		Sync::Mutex packetIdMut;
		Optional<IO::Writer> errLog;

		NN<Text::String> clientId;
		Sync::Mutex hdlrMut;
		Data::ArrayList<Data::CallbackStorage<Net::MQTTConn::PublishMessageHdlr>> hdlrList;
		Sync::Mutex topicMut;
		Data::ArrayListNN<Text::String> topicList;

		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		Data::Duration connTimeout;
		Optional<Text::String> host;
		UInt16 port;
		Optional<Text::String> username;
		Optional<Text::String> password;
		Bool webSocket;

		static void __stdcall KAThread(NN<Sync::Thread> thread);
		static void __stdcall OnDisconnect(AnyType userObj);
		void Connect();
		UInt16 GetNextPacketId();

		void Init(NN<Net::TCPClientFactory> clif, Net::MQTTConn::PublishMessageHdlr hdlr, AnyType hdlrObj, Optional<IO::Writer> errLog);
	public:
		MQTTStaticClient(NN<Net::TCPClientFactory> clif, Net::MQTTConn::PublishMessageHdlr hdlr, AnyType hdlrObj, Optional<IO::Writer> errLog);
		MQTTStaticClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, Text::CString username, Text::CString password, Bool webSocket, Net::MQTTConn::PublishMessageHdlr hdlr, AnyType userObj, UInt16 kaSeconds, Optional<IO::Writer> errLog);
		virtual ~MQTTStaticClient();

		Bool IsStarted();
		virtual Bool ChannelFailure();

		virtual void HandlePublishMessage(Net::MQTTConn::PublishMessageHdlr hdlr, AnyType hdlrObj);
		virtual Bool Subscribe(Text::CStringNN topic);
		virtual Bool Publish(Text::CStringNN topic, Text::CStringNN message);
	};
}
#endif
