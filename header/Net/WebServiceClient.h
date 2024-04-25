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
			const UTF8Char *name;
			const UTF8Char *val;
		} ParamInfo;
	private:
		NN<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		Text::String *serviceAddr;
		const UTF8Char *serviceName;
		const UTF8Char *targetNS;
		const UTF8Char *soapAction;
		Data::ArrayList<ParamInfo*> *paramList;
		const UTF8Char *responseVal;

	public:
		WebServiceClient(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CString serviceAddr, const UTF8Char *serviceName, const UTF8Char *targetNS);
		~WebServiceClient();

		void AddParam(const UTF8Char *paramName, const UTF8Char *paramVal);
		void SetSOAPAction(const UTF8Char *soapAction);
		Bool Request(RequestType rt);
		const UTF8Char *GetResponseVal();
	};
};
#endif
