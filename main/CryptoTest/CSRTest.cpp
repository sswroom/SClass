#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Cert/X509Key.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
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
	Crypto::Cert::X509Key *key = (Crypto::Cert::X509Key*)x509;
	DEL_CLASS(key);
	return 0;
}