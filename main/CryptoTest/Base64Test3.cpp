#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/Base64.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	char text[] = "MIIBrzCCARgCAQAwbzEQMA4GA1UEBhMHVW5rbm93bjEMMAoGA1UECBMDdHN0MREwDwYDVQQHEwhob25na29uZzEPMA0GA1UEChMGcm9kc3VtMRAwDgYDVQQLEwdVbmtub3duMRcwFQYDVQQDEw4xOTIuMTY4Ljk5LjIyMjCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAgaBHMsLYGSCgY82An95Bh/Y238amSeaVuMF8etBDOjo6cPJNhfmyIQ8aTAwLRKt3RQnXi1zWCDzqjtXb1Y8AILOxfH2aD+lZiDC7jQNYk0sz7Gvkn2R6dGov0giOENZMuSiOqDwrrspLPHLyA3DxJS7DsxwUkMt6EiZpwNPdHukCAwEAAaAAMA0GCSqGSIb3DQEBBQUAA4GBAFL9UtHSocPnqGL8sVBrsha8f/AJt0drtaVjAQQx9YxqmFM+VGR078NnBLUvUOQMiLwK78M5Pnf/iFYWDNYTgDHTW2vAeRjJb1qitnvLhbRYhFuZ9LmJJmi43s0Q0iQ5uYJO8j9JDWiKA8kXdrd/klABBvg57nnGZhGoOS288aFG";
	UInt8 buff[512];
	UIntOS decSize;
	Crypto::Encrypt::Base64 *b64;
	NEW_CLASS(b64, Crypto::Encrypt::Base64());
	decSize = b64->Decrypt((UInt8*)text, sizeof(text) - 1, buff);
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Buff:\r\n"));
	sb.AppendHexBuff(Data::ByteArrayR(buff, decSize), ' ', Text::LineBreakType::CRLF);
	printf("%s\r\n", sb.ToString().Ptr());
	DEL_CLASS(b64);
	return 0;
}
