#ifndef _SM_NET_WEBSERVER_SAMLHANDLER
#define _SM_NET_WEBSERVER_SAMLHANDLER
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Net/SAMLIdpConfig.h"
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
			Text::CString loginPath;
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
			LoginPath,
			MetadataPath,
			LogoutPath,
			SSOPath,
			SignCert,
			SignKey
		};

		class SAMLHandler : public Net::WebServer::WebStandardHandler
		{
		public:
			typedef void (CALLBACKFUNC SAMLStrFunc)(AnyType userObj, Text::CStringNN msg);
			typedef Bool (CALLBACKFUNC SAMLLoginFunc)(AnyType userObj, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<const SAMLMessage> msg);
		private:
			Optional<WebStandardHandler> defHdlr;
			Optional<Net::SSLEngine> ssl;
			Optional<Text::String> serverHost;
			Optional<Text::String> metadataPath;
			Optional<Text::String> loginPath;
			Optional<Text::String> logoutPath;
			Optional<Text::String> ssoPath;
			Optional<Crypto::Cert::X509Cert> signCert;
			Optional<Crypto::Cert::X509PrivKey> signKey;
			SAMLError initErr;
			SAMLStrFunc rawRespHdlr;
			AnyType rawRespObj;
			SAMLLoginFunc loginHdlr;
			AnyType loginObj;
			Optional<Net::SAMLIdpConfig> idp;
			Sync::Mutex idpMut;

		protected:
			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);

			void SendRedirect(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN url, Text::CStringNN reqContent);
		public:
			SAMLHandler(NN<SAMLConfig> cfg, Optional<Net::SSLEngine> ssl, Optional<WebStandardHandler> defHdlr);
			virtual ~SAMLHandler();

			SAMLError GetInitError();
			Bool GetLoginURL(NN<Text::StringBuilderUTF8> sb);
			Bool GetLogoutURL(NN<Text::StringBuilderUTF8> sb);
			Bool GetMetadataURL(NN<Text::StringBuilderUTF8> sb);
			Bool GetSSOURL(NN<Text::StringBuilderUTF8> sb);
			void HandleRAWSAMLResponse(SAMLStrFunc hdlr, AnyType userObj);
			void HandleLoginRequest(SAMLLoginFunc hdlr, AnyType userObj);
			Optional<Crypto::Cert::X509PrivKey> GetKey();
			void SetIdp(NN<Net::SAMLIdpConfig> idp);
		};
		Text::CStringNN SAMLErrorGetName(SAMLError err);
	}
}
#endif
