#ifndef _SM_NET_HTTPQUEUE
#define _SM_NET_HTTPQUEUE
#include "Data/StringUTF8Map.h"
#include "Net/HTTPClient.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Net
{
	class HTTPQueue
	{
	private:
		typedef struct
		{
			Net::HTTPClient *req1;
			Net::HTTPClient *req2;
		} DomainStatus;
	private:
		Net::SocketFactory *sockf;
		Data::StringUTF8Map<DomainStatus*> *statusMap;
		Sync::Mutex *statusMut;
		Sync::Event *statusEvt;

	public:
		HTTPQueue(Net::SocketFactory *sockf);
		~HTTPQueue();

		Net::HTTPClient *MakeRequest(const UTF8Char *url, const Char *method, Bool noShutdown);
		void EndRequest(Net::HTTPClient *cli);
	};
};
#endif
