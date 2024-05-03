#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextBinEnc/Punycode.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	WChar wbuff[256];
	IO::ConsoleWriter console;
	sptr = Text::TextBinEnc::Punycode::Decode(sbuff, UTF8STRC("xn--d9juau41awczczp"));
	console.Write(CSTR("UTF8: "));
	console.WriteLine(CSTRP(sbuff, sptr));

	Text::TextBinEnc::Punycode::Decode(wbuff, L"xn--d9juau41awczczp");
	console.Write(CSTR("WChar: "));
	sptr = Text::StrWChar_UTF8(sbuff, wbuff);
	console.WriteLine(CSTRP(sbuff, sptr));
	return 0;
}
