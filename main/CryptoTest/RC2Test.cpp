#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/RC2Cipher.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 keyBuff[8] = {0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	Crypto::Encrypt::RC2Cipher rc2(keyBuff, 1);
	IO::ConsoleWriter console;
	rc2.SetKey(keyBuff, 1, 64);

	UInt8 dataBuff[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	UInt8 outBuff[32];
	UOSInt outSize;
	outSize = rc2.Encrypt(dataBuff, 8, outBuff, 0);
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(outBuff, outSize, ' ', Text::LineBreakType::None);
	console.WriteLineC(sb.ToString(), sb.GetLength());
	return 0;
}
