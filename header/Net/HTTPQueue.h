#ifndef _SM_NET_HTTPQUEUE
#define _SM_NET_HTTPQUEUE
#include "Data/StringMapNN.hpp"
#include "Net/HTTPClient.h"
#include "Net/SSLEngine.h"
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
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		Data::StringMapNN<DomainStatus> statusMap;
		Sync::Mutex statusMut;
		Sync::Event statusEvt;

	public:
		HTTPQueue(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
		~HTTPQueue();

		NN<Net::HTTPClient> MakeRequest(Text::CStringNN url, Net::WebUtil::RequestMethod method, Bool noShutdown);
		void EndRequest(NN<Net::HTTPClient> cli);
		void Clear();
	};
}
#endif
