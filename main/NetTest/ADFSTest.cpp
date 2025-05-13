#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/SAMLHandler.h"
#include "Net/WebServer/PrintLogWebHandler.h"
#include "Net/WebServer/SAMLService.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Parser/FileParser/X509Parser.h"

#define PORTNUM 443
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

Optional<Net::SSLEngine> ssl;
Bool initSucc;
NN<Net::WebServer::PrintLogWebHandler> logHdlr;

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff1[512];
	UTF8Char sbuff2[512];
	UTF8Char sbuff3[512];
	UTF8Char sbuff4[512];
	UTF8Char sbuff5[512];
	UnsafeArray<UTF8Char> sptr1;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr3;
	UnsafeArray<UTF8Char> sptr4;
	UnsafeArray<UTF8Char> sptr5 = sbuff5;
	IO::ConsoleWriter console;
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	initSucc = false;
	ssl = Net::SSLEngineFactory::Create(clif, false);
	sptr3 = IO::Path::GetProcessFileName(sbuff3).Or(sbuff3);
	sptr3 = IO::Path::AppendPath(sbuff3, sptr3, CSTR("SAMLCert.crt"));
	sptr4 = IO::Path::GetProcessFileName(sbuff4).Or(sbuff4);
	sptr4 = IO::Path::AppendPath(sbuff4, sptr4, CSTR("SAMLCert.key"));
	NN<Net::SSLEngine> nnssl;
	if (ssl.SetTo(nnssl))
	{
		sptr1 = IO::Path::GetProcessFileName(sbuff1).Or(sbuff1);
		sptr1 = IO::Path::AppendPath(sbuff1, sptr1, CSTR("ADFSCert.crt"));
		sptr2 = IO::Path::GetProcessFileName(sbuff2).Or(sbuff1);
		sptr2 = IO::Path::AppendPath(sbuff2, sptr2, CSTR("ADFSCert.key"));
		if (nnssl->ServerSetCerts(CSTRP(sbuff1, sptr1), CSTRP(sbuff2, sptr2)))
		{
			Parser::FileParser::X509Parser parser;
			NN<IO::ParsedObject> pobj;
			if (parser.ParseFilePath(CSTRP(sbuff1, sptr1)).SetTo(pobj))
			{
				if (pobj->GetParserType() == IO::ParserType::ASN1Data)
				{
					NN<Net::ASN1Data> asn1 = NN<Net::ASN1Data>::ConvertFrom(pobj);
					if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509 && NN<Crypto::Cert::X509File>::ConvertFrom(asn1)->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
					{
						NN<Crypto::Cert::X509Cert> cert = NN<Crypto::Cert::X509Cert>::ConvertFrom(asn1);
						if (!cert->GetSubjectCN(sbuff5).SetTo(sptr5))
						{
							console.WriteLine(CSTR("Error in getting CN from ADFSCert.crt"));
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
						console.WriteLine(CSTR("ADFSCert.crt is not a cert file"));
					}
				}
				else
				{
					console.WriteLine(CSTR("ADFSCert.crt is not a cert file"));
				}
				pobj.Delete();
			}
			else
			{
				console.WriteLine(CSTR("Error in parsing ADFSCert.crt"));
			}
		}
		else
		{
			console.WriteLine(CSTR("Error in loading ADFSCert.crt/ADFSCert.key file"));
		}
	}
	else
	{
		console.WriteLine(CSTR("Error in initializing SSL Engine"));
	}
	if (initSucc)
	{
		NN<Net::SAMLHandler> samlHdlr;
		NN<Net::WebServer::SAMLService> samlService;
		Net::SAMLConfig cfg;
		cfg.serverHost = CSTRP(sbuff5, sptr5);
		cfg.metadataPath = CSTR("/saml/metadata");
		cfg.logoutPath = CSTR("/saml/SingleLogout");
		cfg.ssoPath = CSTR("/saml/SSO");
		cfg.signCertPath = CSTRP(sbuff3, sptr3);
		cfg.signKeyPath = CSTRP(sbuff4, sptr4);
		NEW_CLASSNN(samlHdlr, Net::SAMLHandler(cfg, ssl));
		NEW_CLASSNN(samlService, Net::WebServer::SAMLService(samlHdlr));
		NEW_CLASSNN(logHdlr, Net::WebServer::PrintLogWebHandler(samlService, console));
		Net::WebServer::WebListener listener(clif, ssl, logHdlr, PORTNUM, 120, 1, 4, CSTR("ADFSTest/1.0"), false, Net::WebServer::KeepAlive::Default, true);
		if (listener.IsError())
		{
			console.WriteLine(CSTR("Error in listening to port " STR(PORTNUM)));
		}
		else
		{
			console.WriteLine(CSTR("Listening to port " STR(PORTNUM) " (https)"));
			progCtrl->WaitForExit(progCtrl);
			console.WriteLine(CSTR("Server stopping"));
		}
		logHdlr.Delete();
	}
	ssl.Delete();
	return 0;
}
