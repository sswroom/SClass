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
			ConfigNotFound,
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
			typedef void (__stdcall *SAMLStrFunc)(void *userObj, Text::CString msg);
			typedef Bool (__stdcall *SAMLLoginFunc)(void *userObj, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, const SAMLMessage *msg);
		private:
			WebStandardHandler *defHdlr;
			Optional<Net::SSLEngine> ssl;
			Text::String *serverHost;
			Text::String *metadataPath;
			Text::String *logoutPath;
			Text::String *ssoPath;
			Crypto::Cert::X509Cert *signCert;
			Crypto::Cert::X509PrivKey *signKey;
			SAMLError initErr;
			SAMLStrFunc rawRespHdlr;
			void *rawRespObj;
			SAMLLoginFunc loginHdlr;
			void *loginObj;

		protected:
			virtual Bool ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);
		public:
			SAMLHandler(SAMLConfig *cfg, Optional<Net::SSLEngine> ssl, WebStandardHandler *defHdlr);
			virtual ~SAMLHandler();

			SAMLError GetInitError();
			Bool GetLogoutURL(NotNullPtr<Text::StringBuilderUTF8> sb);
			Bool GetMetadataURL(NotNullPtr<Text::StringBuilderUTF8> sb);
			Bool GetSSOURL(NotNullPtr<Text::StringBuilderUTF8> sb);
			void HandleRAWSAMLResponse(SAMLStrFunc hdlr, void *userObj);
			void HandleLoginRequest(SAMLLoginFunc hdlr, void *userObj);
			Crypto::Cert::X509PrivKey *GetKey();
		};
		Text::CString SAMLErrorGetName(SAMLError err);
	}
}
#endif
