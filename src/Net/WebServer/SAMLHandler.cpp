#include "Stdafx.h"
#include "Net/WebServer/SAMLHandler.h"
#include "Parser/FileParser/X509Parser.h"

Net::WebServer::SAMLHandler::~SAMLHandler()
{
	SDEL_STRING(this->serverHost);
	SDEL_STRING(this->metadataPath);
	SDEL_STRING(this->logoutPath);
	SDEL_STRING(this->ssoPath);
	SDEL_CLASS(this->signCert);
	SDEL_CLASS(this->signKey);
}

Bool Net::WebServer::SAMLHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, UOSInt subReqLen)
{
	if (this->initErr == SAMLError::None)
	{
		//////////////////////////////////
	}

	if (this->DoRequest(req, resp, subReq, subReqLen))
	{
		return true;
	}
	if (this->defHdlr)
	{
		return this->defHdlr->ProcessRequest(req, resp, subReq, subReqLen);
	}
	return false;
}

Net::WebServer::SAMLHandler::SAMLHandler(SAMLConfig *cfg, Net::SSLEngine *ssl, WebStandardHandler *defHdlr)
{
	this->defHdlr = defHdlr;
	this->ssl = ssl;
	this->serverHost = 0;
	this->metadataPath = 0;
	this->logoutPath = 0;
	this->ssoPath = 0;
	this->signCert = 0;
	this->signKey = 0;
	if (cfg == 0)
	{
		this->initErr = SAMLError::ConfigNotFound;
		return;
	}
	if (cfg->serverHost.leng == 0)
	{
		this->initErr = SAMLError::ServerHost;
		return;
	}
	this->serverHost = Text::String::New(cfg->serverHost);
	if (cfg->metadataPath.leng == 0 || cfg->metadataPath.v[0] != '/')
	{
		this->initErr = SAMLError::MetadataPath;
		return;
	}
	this->metadataPath = Text::String::New(cfg->metadataPath);
	if (cfg->logoutPath.leng == 0 || cfg->logoutPath.v[0] != '/')
	{
		this->initErr = SAMLError::LogoutPath;
		return;
	}
	this->logoutPath = Text::String::New(cfg->logoutPath);
	if (cfg->ssoPath.leng == 0 || cfg->ssoPath.v[0] != '/')
	{
		this->initErr = SAMLError::SSOPath;
		return;
	}
	this->ssoPath = Text::String::New(cfg->ssoPath);
	Parser::FileParser::X509Parser parser;
	if (cfg->signCertPath.leng == 0)
	{
		this->initErr = SAMLError::SignCert;
		return;
	}
	Crypto::Cert::X509File *x509;
	x509 = Parser::FileParser::X509Parser::ToType(parser.ParseFilePath(cfg->signCertPath), Crypto::Cert::X509File::FileType::Cert);
	if (x509 == 0)
	{
		this->initErr = SAMLError::SignCert;
		return;
	}
	this->signCert = (Crypto::Cert::X509Cert*)x509;

	if (cfg->signKeyPath.leng == 0)
	{
		this->initErr = SAMLError::SignCert;
		return;
	}
	x509 = Parser::FileParser::X509Parser::ToType(parser.ParseFilePath(cfg->signKeyPath), Crypto::Cert::X509File::FileType::PrivateKey);
	if (x509 == 0)
	{
		this->initErr = SAMLError::SignKey;
		return;
	}
	this->signKey = (Crypto::Cert::X509PrivKey*)x509;

	this->initErr = SAMLError::None;
}

Net::WebServer::SAMLError Net::WebServer::SAMLHandler::GetInitError()
{
	return this->initErr;
}
