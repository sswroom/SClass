#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/JasyptEncryptor.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
/*	Text::CString encPwd = CSTR("test");
	const UTF8Char *userName = (const UTF8Char*)"testing";
	const UTF8Char *password = (const UTF8Char*)"testing";*/
	Text::CString encPwd = CSTR("eRFLDev");
	Text::CStringNN userName = CSTR("Ukq08EI1PQF47DbDfkhYfjF5fCtehqFQwC/rYU+/T57yvkLhuVf49BwHjLK2AzYr");
	Text::CStringNN password = CSTR("VN1iRYekV9DqHzJ/yIcXWslczjhI98SYkXpw58FmBsMWour1vYSWf++Sx7AD26Br");
	UInt8 decBuff[256];
	UOSInt decSize;
	Crypto::Encrypt::JasyptEncryptor enc(Crypto::Encrypt::JasyptEncryptor::KA_PBEWITHHMACSHA512, Crypto::Encrypt::JasyptEncryptor::CA_AES256, encPwd.ToByteArray());
	decSize = enc.DecryptB64(userName, decBuff);
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Username = "));
	sb.AppendC(decBuff, decSize);
	console.WriteLineC(sb.ToString(), sb.GetLength());
	decSize = enc.DecryptB64(password, decBuff);
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Password = "));
	sb.AppendC(decBuff, decSize);
	console.WriteLineC(sb.ToString(), sb.GetLength());
	return 0;
}
