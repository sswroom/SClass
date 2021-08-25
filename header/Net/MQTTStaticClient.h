#ifndef _SM_NET_MQTTSTATICCLIENT
#define _SM_NET_MQTTSTATICCLIENT
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

		const UTF8Char *clientId;
		Sync::Mutex *hdlrMut;
		Data::ArrayList<Net::MQTTConn::PublishMessageHdlr> *hdlrList;
		Data::ArrayList<void *> *hdlrObjList;
		Sync::Mutex *topicMut;
		Data::ArrayList<const UTF8Char*> *topicList;

		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		const UTF8Char *host;
		UInt16 port;
		const UTF8Char *username;
		const UTF8Char *password;

		static UInt32 __stdcall KAThread(void *userObj);
		static void __stdcall OnDisconnect(void *user);
		void Connect();
		UInt16 GetNextPacketId();
	public:
		MQTTStaticClient(Net::MQTTConn::PublishMessageHdlr hdlr, void *hdlrObj);
		MQTTStaticClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *host, UInt16 port, const UTF8Char *username, const UTF8Char *password, Net::MQTTConn::PublishMessageHdlr hdlr, void *userObj, UInt16 kaSeconds);
		virtual ~MQTTStaticClient();

		Bool IsStarted();
		virtual Bool ChannelFailure();

		virtual void HandlePublishMessage(Net::MQTTConn::PublishMessageHdlr hdlr, void *hdlrObj);
		virtual Bool Subscribe(const UTF8Char *topic);
		virtual Bool Publish(const UTF8Char *topic, const UTF8Char *message);
	};
}
#endif
