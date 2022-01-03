#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/JasyptEncryptor.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	const UTF8Char *encPwd = (const UTF8Char*)"test";
	const UTF8Char *userName = (const UTF8Char*)"testing";
	const UTF8Char *password = (const UTF8Char*)"testing";
	UInt8 decBuff[256];
	UOSInt decSize;
	Crypto::JasyptEncryptor *enc;
	NEW_CLASS(enc, Crypto::JasyptEncryptor(Crypto::JasyptEncryptor::KA_PBEWITHHMACSHA512, Crypto::JasyptEncryptor::CA_AES256, encPwd, Text::StrCharCnt(encPwd)));
	decSize = enc->DecryptB64(userName, decBuff);
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Username = "));
	sb.AppendC(decBuff, decSize);
	console.WriteLine(sb.ToString());
	decSize = enc->DecryptB64(password, decBuff);
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Password = "));
	sb.AppendC(decBuff, decSize);
	console.WriteLine(sb.ToString());
	DEL_CLASS(enc);
	return 0;
}
