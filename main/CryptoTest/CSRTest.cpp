#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Cert/CertUtil.h"
#include "Crypto/Cert/X509Key.h"
#include "Exporter/PEMExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Parser/FileParser/X509Parser.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UInt8 buff[4096];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::ConsoleWriter console;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("ACMEKey.pem"));
	UOSInt buffSize = IO::FileStream::LoadFile(CSTRP(sbuff, sptr), buff, 4096);
	if (buffSize == 0)
	{
		console.WriteLineC(UTF8STRC("Error in loading ACMEKey.pem"));
		return 0;
	}
	Parser::FileParser::X509Parser parser;
	NotNullPtr<Text::String> s = Text::String::NewP(sbuff, sptr);
	Crypto::Cert::X509File *x509 = parser.ParseBuff(Data::ByteArray(buff, buffSize), s);
	s->Release();
	if (x509 == 0)
	{
		console.WriteLineC(UTF8STRC("Error in parsing ACMEKey.pem"));
		return 0;
	}
	if (x509->GetFileType() != Crypto::Cert::X509File::FileType::Key)
	{
		console.WriteLineC(UTF8STRC("ACMEKey.pem is not a key file"));
		DEL_CLASS(x509);
		return 0;
	}
	Net::SSLEngine *ssl;
	Net::OSSocketFactory sockf(false);
	ssl = Net::SSLEngineFactory::Create(sockf, true);
	Crypto::Cert::X509Key *key = (Crypto::Cert::X509Key*)x509;
	Crypto::Cert::CertNames names;
	Crypto::Cert::CertExtensions ext;
	MemClear(&names, sizeof(names));
	MemClear(&ext, sizeof(ext));
	names.countryName = Text::String::New(UTF8STRC("HK")).Ptr();
	names.stateOrProvinceName = Text::String::New(UTF8STRC("Hong Kong")).Ptr();
	names.localityName = Text::String::New(UTF8STRC("Hong Kong")).Ptr();
	names.organizationName = Text::String::New(UTF8STRC("Simon Software Working Room")).Ptr();
	names.organizationUnitName = Text::String::New(UTF8STRC("sswr")).Ptr();
	names.commonName = Text::String::New(UTF8STRC("sswroom.no-ip.org")).Ptr();
	names.emailAddress = Text::String::New(UTF8STRC("sswroom@yahoo.com")).Ptr();

	NEW_CLASS(ext.subjectAltName, Data::ArrayListNN<Text::String>());
	ext.subjectAltName->Add(Text::String::New(UTF8STRC("sswroom.no-ip.org")));
	Crypto::Cert::X509CertReq *csr = Crypto::Cert::CertUtil::CertReqCreate(ssl, &names, key, &ext);
	if (csr)
	{
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("CSRTestOut.pem"));
		Exporter::PEMExporter::ExportFile(CSTRP(sbuff, sptr), csr);
		DEL_CLASS(csr);
	}
	else
	{
		console.WriteLineC(UTF8STRC("Error in creating csr"));
	}
	LIST_FREE_STRING(ext.subjectAltName);
	DEL_CLASS(ext.subjectAltName);
	Crypto::Cert::CertNames::FreeNames(&names);
	DEL_CLASS(key);
	SDEL_CLASS(ssl);
	return 0;
}