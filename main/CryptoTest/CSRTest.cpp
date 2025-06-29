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

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt8 buff[4096];
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	IO::ConsoleWriter console;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("ACMEKey.pem"));
	UOSInt buffSize = IO::FileStream::LoadFile(CSTRP(sbuff, sptr), buff, 4096);
	if (buffSize == 0)
	{
		console.WriteLine(CSTR("Error in loading ACMEKey.pem"));
		return 0;
	}
	Parser::FileParser::X509Parser parser;
	NN<Text::String> s = Text::String::NewP(sbuff, sptr);
	NN<Crypto::Cert::X509File> x509;
	if (!parser.ParseBuff(Data::ByteArray(buff, buffSize), s).SetTo(x509))
	{
		s->Release();
		console.WriteLine(CSTR("Error in parsing ACMEKey.pem"));
		return 0;
	}
	s->Release();
	if (x509->GetFileType() != Crypto::Cert::X509File::FileType::Key)
	{
		console.WriteLine(CSTR("ACMEKey.pem is not a key file"));
		x509.Delete();
		return 0;
	}
	Optional<Net::SSLEngine> ssl;
	Net::OSSocketFactory sockf(false);
	Net::TCPClientFactory clif(sockf);
	ssl = Net::SSLEngineFactory::Create(clif, true);
	NN<Crypto::Cert::X509Key> key = NN<Crypto::Cert::X509Key>::ConvertFrom(x509);
	Crypto::Cert::CertNames names;
	Crypto::Cert::CertExtensions ext;
	MemClear(&names, sizeof(names));
	MemClear(&ext, sizeof(ext));
	names.countryName = Text::String::New(UTF8STRC("HK"));
	names.stateOrProvinceName = Text::String::New(UTF8STRC("Hong Kong"));
	names.localityName = Text::String::New(UTF8STRC("Hong Kong"));
	names.organizationName = Text::String::New(UTF8STRC("Simon Software Working Room"));
	names.organizationUnitName = Text::String::New(UTF8STRC("sswr"));
	names.commonName = Text::String::New(UTF8STRC("sswroom.no-ip.org"));
	names.emailAddress = Text::String::New(UTF8STRC("sswroom@yahoo.com"));

	NN<Data::ArrayListStringNN> nameList;
	NEW_CLASSNN(nameList, Data::ArrayListStringNN());
	ext.subjectAltName = nameList;
	nameList->Add(Text::String::New(UTF8STRC("sswroom.no-ip.org")));
	NN<Crypto::Cert::X509CertReq> csr;
	NN<Net::SSLEngine> nnssl;
	if (ssl.SetTo(nnssl) && Crypto::Cert::CertUtil::CertReqCreate(nnssl, names, key, ext).SetTo(csr))
	{
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("CSRTestOut.pem"));
		Exporter::PEMExporter::ExportFile(CSTRP(sbuff, sptr), csr);
		csr.Delete();
	}
	else
	{
		console.WriteLine(CSTR("Error in creating csr"));
	}
	nameList->FreeAll();
	ext.subjectAltName.Delete();
	Crypto::Cert::CertNames::FreeNames(names);
	key.Delete();
	ssl.Delete();
	return 0;
}