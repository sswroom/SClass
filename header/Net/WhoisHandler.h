#ifndef _SM_NET_WHOISHANDLER
#define _SM_NET_WHOISHANDLER
#include "Net/WhoisGlobalClient.h"
#include "Sync/Mutex.h"

namespace Net
{
	class WhoisHandler
	{
	private:
		Data::ArrayList<WhoisRecord*> recordList;
		Sync::Mutex recordMut;
		Net::WhoisGlobalClient client;

	public:
		WhoisHandler(Net::SocketFactory *sockf);
		~WhoisHandler();

		WhoisRecord *RequestIP(UInt32 ip);
	};
}
#endif
