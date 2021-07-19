#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextEnc/Punycode.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[256];
	WChar wbuff[256];
	IO::ConsoleWriter console;
	Text::TextEnc::Punycode::Decode(sbuff, (const UTF8Char*)"xn--d9juau41awczczp");
	console.Write((const UTF8Char*)"UTF8: ");
	console.WriteLine(sbuff);

	Text::TextEnc::Punycode::Decode(wbuff, L"xn--d9juau41awczczp");
	console.Write((const UTF8Char*)"WChar: ");
	Text::StrWChar_UTF8(sbuff, wbuff);
	console.WriteLine(sbuff);
	return 0;
}
