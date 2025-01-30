#ifndef _SM_NET_WEBDRIVER
#define _SM_NET_WEBDRIVER
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Text/JSONBuilder.h"

namespace Net
{
	class WebDriverParam
	{
	public:
		virtual ~WebDriverParam() {};
		virtual Text::JSONBuilder::ObjectType GetObjectType() const = 0;
		virtual void BuildJSON(NN<Text::JSONBuilder> builder) const = 0;
	};

	class WebDriver
	{
	public:
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		NN<Text::String> url;
		Optional<Text::String> sessId;

	public:
		WebDriver(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, NN<WebDriverParam> param);
		~WebDriver();

		Bool HasSession() const;
	};
}
#endif
