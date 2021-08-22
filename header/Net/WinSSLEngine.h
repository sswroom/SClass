#ifndef _SM_NET_WINSSLENGINE
#define _SM_NET_WINSSLENGINE
#include "Net/SSLEngine.h"

namespace Net
{
	class WinSSLEngine : public Net::SSLEngine
	{
	private:
		struct ClassData;
		struct ClassData *clsData;
		Bool skipCertCheck;

		void DeinitClient();
		Bool InitClient(Method method, void *cred);
		Bool InitServer(Method method, void *cred);
		virtual Net::SSLClient *CreateServerConn(UInt32 *s);
	public:
		WinSSLEngine(Net::SocketFactory *sockf, Method method);
		virtual ~WinSSLEngine();
		
		virtual void SetSkipCertCheck(Bool skipCertCheck);
		virtual Bool IsError();
		virtual Bool SetServerCertsASN1(Crypto::Cert::X509File *certASN1, Crypto::Cert::X509File *keyASN1);
		virtual Bool SetClientCertASN1(Crypto::Cert::X509File *certASN1, Crypto::Cert::X509File *keyASN1);
		virtual UTF8Char *GetErrorDetail(UTF8Char *sbuff);
		virtual Net::SSLClient *Connect(const UTF8Char *hostName, UInt16 port, ErrorType *err);
		virtual Bool GenerateCert(const UTF8Char *country, const UTF8Char *company, const UTF8Char *commonName, Crypto::Cert::X509File **certASN1, Crypto::Cert::X509File **keyASN1);
	};
}
#endif
