#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/HOTP.h"
#include "Data/RandomBytesGenerator.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Base32Enc.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 key[20];
	Data::RandomBytesGenerator random;
	random.NextBytes(key, 10);
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	console.WriteLine((const UTF8Char*)"HOTP key:");
	sb.AppendHexBuff(key, 10, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());
	Text::TextBinEnc::Base32Enc b32;
	sb.ClearStr();
	sb.Append((const UTF8Char*)"B32: ");
	b32.EncodeBin(&sb, key, 10);
	console.WriteLine(sb.ToString());
	
	Crypto::HOTP hotp(key, 10, 1);
	UOSInt i = 0;
	while (i < 10)
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Code ");
		sb.AppendUOSInt(i);
		sb.Append((const UTF8Char*)": ");
		sb.AppendU32(hotp.NextCode());
		console.WriteLine(sb.ToString());
		i++;
	}
	return 0;
}
