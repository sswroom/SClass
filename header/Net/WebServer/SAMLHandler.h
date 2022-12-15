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
			Text::CString signCertPath;
			Text::CString signKeyPath;
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
		private:
			WebStandardHandler *defHdlr;
			Net::SSLEngine *ssl;
			Text::String *serverHost;
			Text::String *metadataPath;
			Text::String *logoutPath;
			Text::String *ssoPath;
			Crypto::Cert::X509Cert *signCert;
			Crypto::Cert::X509PrivKey *signKey;
			SAMLError initErr;
			SAMLStrFunc rawRespHdlr;
			void *rawRespObj;

		protected:
			virtual ~SAMLHandler();
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);
		public:
			SAMLHandler(SAMLConfig *cfg, Net::SSLEngine *ssl, WebStandardHandler *defHdlr);

			SAMLError GetInitError();
			Bool GetLogoutURL(Text::StringBuilderUTF8 *sb);
			Bool GetMetadataURL(Text::StringBuilderUTF8 *sb);
			Bool GetSSOURL(Text::StringBuilderUTF8 *sb);
			void HandleRAWSAMLResponse(SAMLStrFunc hdlr, void *userObj);
		};
		Text::CString SAMLErrorGetName(SAMLError err);
	}
}
#endif
