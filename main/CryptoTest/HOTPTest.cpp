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
	Data::RandomBytesGenerator random;
	random.NextBytes(key, 10);
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	console.WriteLine((const UTF8Char*)"HOTP key:");
	sb.AppendHexBuff(key, 10, 0, Text::LineBreakType::None);
	console.WriteLine(sb.ToString());
	Text::TextBinEnc::Base32Enc b32;
	sb.ClearStr();
	sb.AppendC(UTF8STRC("B32: "));
	b32.EncodeBin(&sb, key, 10);
	console.WriteLine(sb.ToString());
	UOSInt decLen = b32.DecodeBin(sb.ToString() + 5, decKey);
	sb.ClearStr();
	sb.AppendHexBuff(decKey, decLen, 0, Text::LineBreakType::None);
	console.WriteLine(sb.ToString());
	console.WriteLine();
	
	Crypto::HOTP hotp(key, 10, 1);
	UOSInt i = 0;
	while (i < 10)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Code "));
		sb.AppendUOSInt(i);
		sb.AppendC(UTF8STRC(": "));
		hotp.CodeString(sbuff, hotp.NextCode());
		sb.Append(sbuff);
		console.WriteLine(sb.ToString());
		i++;
	}
	sb.ClearStr();
	hotp.GenURI(&sb, (const UTF8Char*)"Test By Counter");
	Net::GoogleQRCode qrCode(200, 200, sb.ToString());
	sb.ClearStr();
	qrCode.GetImageURL(&sb);
	console.WriteLine(sb.ToString());
	return 0;
}
