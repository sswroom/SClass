#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Cert/CertUtil.h"
#include "Crypto/Cert/X509Key.h"
#include "Exporter/PEMExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/OSSocketFactory.h"
#include "Parser/FileParser/X509Parser.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 buff[4096];
	UTF8Char sbuff[512];
	IO::ConsoleWriter console;
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"ACMEKey.pem");
	UOSInt buffSize = IO::FileStream::LoadFile(sbuff, buff, 4096);
	if (buffSize == 0)
	{
		console.WriteLine((const UTF8Char*)"Error in loading ACMEKey.pem");
		return 0;
	}
	Parser::FileParser::X509Parser parser;
	Crypto::Cert::X509File *x509 = parser.ParseBuff(buff, buffSize, sbuff);
	if (x509 == 0)
	{
		console.WriteLine((const UTF8Char*)"Error in parsing ACMEKey.pem");
		return 0;
	}
	if (x509->GetFileType() != Crypto::Cert::X509File::FileType::Key)
	{
		console.WriteLine((const UTF8Char*)"ACMEKey.pem is not a key file");
		DEL_CLASS(x509);
		return 0;
	}
	Net::SocketFactory *sockf;
	Net::SSLEngine *ssl;
	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	ssl = Net::DefaultSSLEngine::Create(sockf, true);
	Crypto::Cert::X509Key *key = (Crypto::Cert::X509Key*)x509;
	Crypto::Cert::CertUtil::CertNames names;
	Crypto::Cert::CertUtil::ReqExtensions ext;
	MemClear(&names, sizeof(names));
	MemClear(&ext, sizeof(ext));
	names.countryName = (const UTF8Char*)"HK";
	names.stateOrProvinceName = (const UTF8Char*)"Hong Kong";
	names.localityName = (const UTF8Char*)"Hong Kong";
	names.organizationName = (const UTF8Char*)"Simon Software Working Room";
	names.organizationUnitName = (const UTF8Char*)"sswr";
	names.commonName = (const UTF8Char*)"sswroom.no-ip.org";
	names.emailAddress = (const UTF8Char*)"sswroom@yahoo.com";

	NEW_CLASS(ext.subjectAltName, Data::ArrayList<const UTF8Char*>());
	ext.subjectAltName->Add(Text::StrCopyNew((const UTF8Char*)"sswroom.no-ip.org"));
	Crypto::Cert::X509CertReq *csr = Crypto::Cert::CertUtil::CertReqCreate(ssl, &names, key, &ext);
	if (csr)
	{
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"CSRTestOut.pem");
		Exporter::PEMExporter::ExportFile(sbuff, csr);
		DEL_CLASS(csr);
	}
	else
	{
		console.WriteLine((const UTF8Char*)"Error in creating csr");
	}
	LIST_FREE_FUNC(ext.subjectAltName, Text::StrDelNew);
	DEL_CLASS(ext.subjectAltName);
	DEL_CLASS(key);
	SDEL_CLASS(ssl);
	DEL_CLASS(sockf);
	return 0;
}