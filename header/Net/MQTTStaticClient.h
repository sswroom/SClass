#ifndef _SM_NET_MQTTSTATICCLIENT
#define _SM_NET_MQTTSTATICCLIENT
#include "IO/Writer.h"
#include "Net/FailoverChannel.h"
#include "Net/MQTTClient.h"
#include "Net/MQTTConn.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Net
{
	class MQTTStaticClient : public Net::FailoverChannel, public Net::MQTTClient
	{
	private:
		Sync::Mutex *connMut;
		Net::MQTTConn *conn;
		UInt16 kaSeconds;
		Bool kaRunning;
		Bool kaToStop;
		Sync::Event *kaEvt;
		UInt16 packetId;
		Sync::Mutex *packetIdMut;
		IO::Writer *errLog;

		Text::String *clientId;
		Sync::Mutex *hdlrMut;
		Data::ArrayList<Net::MQTTConn::PublishMessageHdlr> *hdlrList;
		Data::ArrayList<void *> *hdlrObjList;
		Sync::Mutex *topicMut;
		Data::ArrayList<Text::String*> *topicList;

		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Text::String *host;
		UInt16 port;
		Text::String *username;
		Text::String *password;
		Bool autoReconn;

		static UInt32 __stdcall KAThread(void *userObj);
		static void __stdcall OnDisconnect(void *user);
		void Connect();
		UInt16 GetNextPacketId();
	public:
		MQTTStaticClient(Net::MQTTConn::PublishMessageHdlr hdlr, void *hdlrObj, IO::Writer *errLog);
		MQTTStaticClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, Text::CString username, Text::CString password, Net::MQTTConn::PublishMessageHdlr hdlr, void *userObj, UInt16 kaSeconds, IO::Writer *errLog);
		virtual ~MQTTStaticClient();

		Bool IsStarted();
		virtual Bool ChannelFailure();

		virtual void HandlePublishMessage(Net::MQTTConn::PublishMessageHdlr hdlr, void *hdlrObj);
		virtual Bool Subscribe(Text::CString topic);
		virtual Bool Publish(Text::CString topic, Text::CString message);
	};
}
#endif
