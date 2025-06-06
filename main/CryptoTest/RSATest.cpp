#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/RSAEnc.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CString keyFile = CSTR("/home/sswroom/Progs/Temp/saml_token.key");
	Crypto::Encrypt::RSAEnc rsa;
	rsa.LoadPublicKeyFile(keyFile);
	return 0;
}
