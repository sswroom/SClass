#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/Base64.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Char b64str[] = "CzUAANsHBRIJDhwAAAAAAAAAAOCTBA==";
	UInt8 obuff[128];
	UIntOS oSize;

	Crypto::Encrypt::Base64 *b64;
	NEW_CLASS(b64, Crypto::Encrypt::Base64());
	obuff[oSize = b64->Decrypt((UInt8*)b64str, sizeof(b64str) - 1, obuff)] = 0;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Buff:\r\n"));
	sb.AppendHexBuff(Data::ByteArrayR(obuff, oSize), ' ', Text::LineBreakType::CRLF);
	printf("%s\r\n", sb.ToString().Ptr());
	DEL_CLASS(b64);
	return 0;
}
