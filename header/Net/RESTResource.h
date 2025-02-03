#ifndef _SM_NET_RESTRESOURCE
#define _SM_NET_RESTRESOURCE
#include "Net/HTTPClient.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Net/WebStatus.h"

namespace Net
{
	class RESTParam
	{
	public:
		~RESTParam() {};

		virtual void WriteParam(NN<Net::HTTPClient> cli) const = 0;
	};

	class RESTResource
	{
	private:
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		NN<Text::String> url;
		Optional<Net::HTTPClient> cli;
		Data::Duration timeout;

		void BuildURL(NN<Text::StringBuilderUTF8> sb, Text::CStringNN command) const;
		Net::WebStatus::StatusCode DoRequest(Text::CStringNN command, Net::WebUtil::RequestMethod method, Optional<RESTParam> param, NN<IO::Stream> respStm);
	public:
		RESTResource(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url);
		~RESTResource();

		void SetTimeout(Data::Duration timeout);

		NN<RESTResource> CreateSubResource(Text::CStringNN subPath);
		Net::WebStatus::StatusCode Get(Text::CStringNN command, NN<IO::Stream> respStm);
		Net::WebStatus::StatusCode Post(Text::CStringNN command, NN<RESTParam> param, NN<IO::Stream> respStm);
		Net::WebStatus::StatusCode Put(Text::CStringNN command, NN<RESTParam> param, NN<IO::Stream> respStm);
		Net::WebStatus::StatusCode Delete(Text::CStringNN command, Optional<RESTParam> param, NN<IO::Stream> respStm);
	};
}
#endif
