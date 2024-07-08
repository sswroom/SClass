#ifndef _SM_NET_IAMSMARTAPI
#define _SM_NET_IAMSMARTAPI
#include "Data/RandomBytesGenerator.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngine.h"

namespace Net
{
	class IAMSmartAPI
	{
	private:
		NN<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		NN<Text::String> domain;
		NN<Text::String> clientID;
		NN<Text::String> clientSecret;
		Data::RandomBytesGenerator rand;

		void InitHTTPClient(NN<Net::HTTPClient> cli, Text::CStringNN content);
	public:
		IAMSmartAPI(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN domain, Text::CStringNN clientID, Text::CStringNN clientSecret);
		~IAMSmartAPI();

		Bool GetKey();

	};
}
#endif
