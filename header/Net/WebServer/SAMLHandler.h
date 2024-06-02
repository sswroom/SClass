#ifndef _SM_NET_WEBSERVER_SAMLHANDLER
#define _SM_NET_WEBSERVER_SAMLHANDLER
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Net/SSLEngine.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Text/CString.h"

namespace Net
{
	namespace WebServer
	{
		struct SAMLConfig
		{
			Text::CString serverHost;
			Text::CString metadataPath;
			Text::CString logoutPath;
			Text::CString ssoPath;
			Text::CStringNN signCertPath;
			Text::CStringNN signKeyPath;
		};

		struct SAMLMessage
		{
			Text::CString rawMessage;
		};

		enum class SAMLError
		{
			None,
			ServerHost,
			MetadataPath,
			LogoutPath,
			SSOPath,
			SignCert,
			SignKey
		};

		class SAMLHandler : public Net::WebServer::WebStandardHandler
		{
		public:
			typedef void (__stdcall *SAMLStrFunc)(AnyType userObj, Text::CString msg);
			typedef Bool (__stdcall *SAMLLoginFunc)(AnyType userObj, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, const SAMLMessage *msg);
		private:
			WebStandardHandler *defHdlr;
			Optional<Net::SSLEngine> ssl;
			Text::String *serverHost;
			Text::String *metadataPath;
			Text::String *logoutPath;
			Text::String *ssoPath;
			Optional<Crypto::Cert::X509Cert> signCert;
			Optional<Crypto::Cert::X509PrivKey> signKey;
			SAMLError initErr;
			SAMLStrFunc rawRespHdlr;
			AnyType rawRespObj;
			SAMLLoginFunc loginHdlr;
			AnyType loginObj;

		protected:
			virtual Bool ProcessRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);
		public:
			SAMLHandler(NN<SAMLConfig> cfg, Optional<Net::SSLEngine> ssl, WebStandardHandler *defHdlr);
			virtual ~SAMLHandler();

			SAMLError GetInitError();
			Bool GetLogoutURL(NN<Text::StringBuilderUTF8> sb);
			Bool GetMetadataURL(NN<Text::StringBuilderUTF8> sb);
			Bool GetSSOURL(NN<Text::StringBuilderUTF8> sb);
			void HandleRAWSAMLResponse(SAMLStrFunc hdlr, AnyType userObj);
			void HandleLoginRequest(SAMLLoginFunc hdlr, AnyType userObj);
			Optional<Crypto::Cert::X509PrivKey> GetKey();
		};
		Text::CStringNN SAMLErrorGetName(SAMLError err);
	}
}
#endif
