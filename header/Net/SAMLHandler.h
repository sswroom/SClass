#ifndef _SM_NET_SAMLHANDLER
#define _SM_NET_SAMLHANDLER
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Net/SAMLAuthMethod.h"
#include "Net/SAMLIdpConfig.h"
#include "Net/SAMLLogoutRequest.h"
#include "Net/SAMLLogoutResponse.h"
#include "Net/SAMLSSOResponse.h"
#include "Net/SSLEngine.h"
#include "Net/WebServer/WebRequest.h"
#include "Net/WebServer/WebResponse.h"
#include "Text/CString.h"

namespace Net
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

	enum class SAMLInitError
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

	enum class SAMLSignError
	{
		Valid,
		SignatureInvalid,
		CertMissing,
		SignatureMissing,
		SigAlgMissing,
		SigAlgNotSupported,
		QueryStringGetError,
		QueryStringSearchError,
		KeyError
	};

	class SAMLHandler
	{
	private:
		Optional<Net::SSLEngine> ssl;
		Optional<Text::String> serverHost;
		Optional<Text::String> metadataPath;
		Optional<Text::String> loginPath;
		Optional<Text::String> logoutPath;
		Optional<Text::String> ssoPath;
		Optional<Crypto::Cert::X509Cert> signCert;
		Optional<Crypto::Cert::X509PrivKey> signKey;
		Crypto::Hash::HashType hashType;
		SAMLInitError initErr;
		Optional<Net::SAMLIdpConfig> idp;
		Sync::Mutex idpMut;
		Text::EncodingFactory encFact;
		UnsafeArray<SAMLAuthMethod> authMethods;
		UIntOS nAuthMethods;

	protected:
		void SendRedirect(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN url, Text::CStringNN reqContent, Crypto::Hash::HashType hashType, Bool response);
		Bool BuildRedirectURL(NN<Text::StringBuilderUTF8> sb, Text::CStringNN url, Text::CStringNN reqContent, Crypto::Hash::HashType hashType, Bool response);
		static SAMLSignError VerifyHTTPRedirect(NN<Net::SSLEngine> ssl, NN<SAMLIdpConfig> idp, NN<Net::WebServer::WebRequest> req);
		void ParseSAMLLogoutResponse(NN<SAMLLogoutResponse> saml, NN<IO::Stream> stm);
		void ParseSAMLLogoutRequest(NN<SAMLLogoutRequest> saml, NN<IO::Stream> stm);

	public:
		SAMLHandler(NN<SAMLConfig> cfg, Optional<Net::SSLEngine> ssl);
		~SAMLHandler();

		SAMLInitError GetInitError();
		Optional<Text::String> GetMetadataPath() const { return this->metadataPath; }
		Optional<Text::String> GetLoginPath() const { return this->loginPath; }
		Optional<Text::String> GetLogoutPath() const { return this->logoutPath; }
		Optional<Text::String> GetSSOPath() const { return this->ssoPath; }
		Data::DataArray<SAMLAuthMethod> GetAuthMethods() const { return Data::DataArray<SAMLAuthMethod>(this->authMethods, this->nAuthMethods); }
		Bool GetLoginURL(NN<Text::StringBuilderUTF8> sb);
		Bool GetLogoutURL(NN<Text::StringBuilderUTF8> sb);
		Bool GetMetadataURL(NN<Text::StringBuilderUTF8> sb);
		Bool GetSSOURL(NN<Text::StringBuilderUTF8> sb);
		Optional<Crypto::Cert::X509PrivKey> GetKey();
		void SetIdp(NN<Net::SAMLIdpConfig> idp);
		void SetHashType(Crypto::Hash::HashType hashType);
		void SetAuthMethod(SAMLAuthMethod authMethod) { this->nAuthMethods = 1; this->authMethods[0] = authMethod; }
		void SetAuthMethods(UnsafeArray<SAMLAuthMethod> authMethods, UIntOS nAuthMethods);

		Bool GetLoginMessageURL(NN<Text::StringBuilderUTF8> sb);
		Bool GetLogoutMessageURL(NN<Text::StringBuilderUTF8> sb, Text::CString nameID, Text::CString sessionId);
		Bool GetLogoutResponse(NN<Text::StringBuilderUTF8> sb, Text::CStringNN id, SAMLStatusCode status);
		Bool GetMetadataXML(NN<Text::StringBuilderUTF8> sb);
		Bool DoLoginGet(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		Bool DoLogoutGet(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CString nameID, Text::CString sessionId);
		Bool DoMetadataGet(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		NN<Net::SAMLSSOResponse> DoSSOPost(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		NN<Net::SAMLLogoutResponse> DoLogoutResp(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		NN<Net::SAMLLogoutRequest> DoLogoutReq(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
	};
	Text::CStringNN SAMLInitErrorGetName(SAMLInitError err);
}
#endif
