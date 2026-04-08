#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Cert/X509CertReq.h"
#include "IO/StmData/FileData.h"
#include "Parser/FileParser/X509Parser.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<Crypto::Cert::X509CertReq> cerReq;
	Parser::FileParser::X509Parser parser;
	IO::StmData::FileData fd(CSTR("0_doc/Macau/USR2/20120705/certreq.req"), false);
	if (Optional<Crypto::Cert::X509CertReq>::ConvertFrom(parser.ParseFile(fd, nullptr, IO::ParserType::ASN1Data)).SetTo(cerReq))
	{
		printf("Parse cert req OK\r\n");
		cerReq.Delete();
	}
	else
	{
		printf("Parse cert req failed\r\n");
	}
	return 0;
}
