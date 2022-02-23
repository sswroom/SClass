#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/HOTP.h"
#include "Data/RandomBytesGenerator.h"
#include "IO/ConsoleWriter.h"
#include "Net/GoogleQRCode.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Base32Enc.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 key[20];
	UInt8 decKey[20];
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Data::RandomBytesGenerator random;
	random.NextBytes(key, 10);
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	console.WriteLineC(UTF8STRC("HOTP key:"));
	sb.AppendHexBuff(key, 10, 0, Text::LineBreakType::None);
	console.WriteLineC(sb.ToString(), sb.GetLength());
	Text::TextBinEnc::Base32Enc b32;
	sb.ClearStr();
	sb.AppendC(UTF8STRC("B32: "));
	b32.EncodeBin(&sb, key, 10);
	console.WriteLineC(sb.ToString(), sb.GetLength());
	UOSInt decLen = b32.DecodeBin(sb.ToString() + 5, sb.GetLength() - 5, decKey);
	sb.ClearStr();
	sb.AppendHexBuff(decKey, decLen, 0, Text::LineBreakType::None);
	console.WriteLineC(sb.ToString(), sb.GetLength());
	console.WriteLine();
	
	Crypto::HOTP hotp(key, 10, 1);
	UOSInt i = 0;
	while (i < 10)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Code "));
		sb.AppendUOSInt(i);
		sb.AppendC(UTF8STRC(": "));
		sptr = hotp.CodeString(sbuff, hotp.NextCode());
		sb.AppendP(sbuff, sptr);
		console.WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}
	sb.ClearStr();
	hotp.GenURI(&sb, (const UTF8Char*)"Test By Counter");
	Net::GoogleQRCode qrCode(200, 200, sb.ToCString());
	sb.ClearStr();
	qrCode.GetImageURL(&sb);
	console.WriteLineC(sb.ToString(), sb.GetLength());
	return 0;
}
