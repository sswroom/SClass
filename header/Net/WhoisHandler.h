#ifndef _SM_NET_WHOISHANDLER
#define _SM_NET_WHOISHANDLER
#include "Net/WhoisGlobalClient.h"
#include "Sync/Mutex.h"

namespace Net
{
	class WhoisHandler
	{
	private:
		Data::ArrayListNN<WhoisRecord> recordList;
		Sync::Mutex recordMut;
		Net::WhoisGlobalClient client;
		Data::Duration timeout;

	public:
		WhoisHandler(NN<Net::SocketFactory> sockf, Data::Duration timeout);
		~WhoisHandler();

		NN<WhoisRecord> RequestIP(UInt32 ip);
	};
}
#endif
