#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebServer/PrintLogWebHandler.h"
#include "Net/WebServer/SAMLHandler.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Parser/FileParser/X509Parser.h"

#define PORTNUM 443
#define TO_STRING(a) #a

Net::SSLEngine *ssl;
Bool initSucc;
Net::WebServer::PrintLogWebHandler *logHdlr;

class MyADFSService : public Net::WebServer::WebServiceHandler
{
public:
	MyADFSService()
	{

	}

	virtual ~MyADFSService()
	{

	}
};

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff1[512];
	UTF8Char sbuff2[512];
	UTF8Char sbuff3[512];
	UTF8Char sbuff4[512];
	UTF8Char sbuff5[512];
	UTF8Char *sptr1;
	UTF8Char *sptr2;
	UTF8Char *sptr3;
	UTF8Char *sptr4;
	UTF8Char *sptr5 = 0;
	IO::ConsoleWriter console;
	Net::OSSocketFactory sockf(true);
	initSucc = false;
	ssl = Net::SSLEngineFactory::Create(&sockf, false);
	sptr3 = IO::Path::GetProcessFileName(sbuff3);
	sptr3 = IO::Path::AppendPath(sbuff3, sptr3, CSTR("SAMLCert.crt"));
	sptr4 = IO::Path::GetProcessFileName(sbuff4);
	sptr4 = IO::Path::AppendPath(sbuff4, sptr4, CSTR("SAMLCert.key"));
	if (ssl)
	{
		sptr1 = IO::Path::GetProcessFileName(sbuff1);
		sptr1 = IO::Path::AppendPath(sbuff1, sptr1, CSTR("ADFSCert.crt"));
		sptr2 = IO::Path::GetProcessFileName(sbuff2);
		sptr2 = IO::Path::AppendPath(sbuff2, sptr2, CSTR("ADFSCert.key"));
		if (ssl->SetServerCerts(CSTRP(sbuff1, sptr1), CSTRP(sbuff2, sptr2)))
		{
			Parser::FileParser::X509Parser parser;
			IO::ParsedObject *pobj = parser.ParseFilePath(CSTRP(sbuff1, sptr1));
			if (pobj)
			{
				if (pobj->GetParserType() == IO::ParserType::ASN1Data)
				{
					Net::ASN1Data *asn1 = (Net::ASN1Data*)pobj;
					if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509 && ((Crypto::Cert::X509File*)asn1)->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
					{
						Crypto::Cert::X509Cert *cert = (Crypto::Cert::X509Cert*)asn1;
						sptr5 = cert->GetSubjectCN(sbuff5);
						if (sptr5 == 0)
						{
							console.WriteLineC(UTF8STRC("Error in getting CN from ADFSCert.crt"));
						}
						else
						{
							if (PORTNUM != 443)
							{
								*sptr5++ = ':';
								sptr5 = Text::StrUInt16(sptr5, PORTNUM);
							}
							initSucc = true;
						}
					}
					else
					{
						console.WriteLineC(UTF8STRC("ADFSCert.crt is not a cert file"));
					}
				}
				else
				{
					console.WriteLineC(UTF8STRC("ADFSCert.crt is not a cert file"));
				}
				DEL_CLASS(pobj);
			}
			else
			{
				console.WriteLineC(UTF8STRC("Error in parsing ADFSCert.crt"));
			}
		}
		else
		{
			console.WriteLineC(UTF8STRC("Error in loading ADFSCert.crt/ADFSCert.key file"));
		}
	}
	else
	{
		console.WriteLineC(UTF8STRC("Error in initializing SSL Engine"));
	}
	if (initSucc)
	{
		MyADFSService *svcHdlr;
		Net::WebServer::SAMLHandler *samlHdlr;
		Net::WebServer::SAMLConfig cfg;
		cfg.serverHost = CSTRP(sbuff5, sptr5);
		cfg.metadataPath = CSTR("/saml/metadata");
		cfg.logoutPath = CSTR("/saml/SingleLogout");
		cfg.ssoPath = CSTR("/saml/SSO");
		cfg.signCertPath = CSTRP(sbuff3, sptr3);
		cfg.signKeyPath = CSTRP(sbuff4, sptr4);
		NEW_CLASS(svcHdlr, MyADFSService());
		NEW_CLASS(samlHdlr, Net::WebServer::SAMLHandler(&cfg, ssl, svcHdlr));
		NEW_CLASS(logHdlr, Net::WebServer::PrintLogWebHandler(samlHdlr, &console));
		Net::WebServer::WebListener listener(&sockf, ssl, logHdlr, PORTNUM, 120, 4, CSTR("ADFSTest/1.0"), false, true);
		if (listener.IsError())
		{
			console.WriteLineC(UTF8STRC("Error in listening to port " TO_STRING(PORTNUM)));
		}
		else
		{
			console.WriteLineC(UTF8STRC("Listening to port " TO_STRING(PORTNUM) " (https)"));
			progCtrl->WaitForExit(progCtrl);
			console.WriteLineC(UTF8STRC("Server stopping"));
		}
		logHdlr->Release();
		svcHdlr->Release();
	}
	SDEL_CLASS(ssl);
	return 0;
}
