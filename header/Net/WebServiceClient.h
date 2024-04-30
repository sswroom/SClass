#ifndef _SM_NET_WEBSERVICECLIENT
#define _SM_NET_WEBSERVICECLIENT
#include "Data/ArrayList.h"
#include "Net/SocketFactory.h"

namespace Net
{
	class WebServiceClient
	{
	public:
		typedef enum
		{
			RT_SOAP11,
			RT_SOAP12,
			RT_HTTPPOST
		} RequestType;

		typedef struct
		{
			NN<Text::String> name;
			NN<Text::String> val;
		} ParamInfo;
	private:
		NN<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		NN<Text::String> serviceAddr;
		NN<Text::String> serviceName;
		NN<Text::String> targetNS;
		Optional<Text::String> soapAction;
		Data::ArrayListNN<ParamInfo> paramList;
		Optional<Text::String> responseVal;

	public:
		WebServiceClient(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN serviceAddr, Text::CStringNN serviceName, Text::CStringNN targetNS);
		~WebServiceClient();

		void AddParam(Text::CStringNN paramName, Text::CStringNN paramVal);
		void SetSOAPAction(Text::CString soapAction);
		Bool Request(RequestType rt);
		const UTF8Char *GetResponseVal();
	};
};
#endif
