#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextBinEnc/Punycode.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	WChar wbuff[256];
	IO::ConsoleWriter console;
	sptr = Text::TextBinEnc::Punycode::Decode(sbuff, UTF8STRC("xn--d9juau41awczczp"));
	console.WriteStrC(UTF8STRC("UTF8: "));
	console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));

	Text::TextBinEnc::Punycode::Decode(wbuff, L"xn--d9juau41awczczp");
	console.WriteStrC(UTF8STRC("WChar: "));
	sptr = Text::StrWChar_UTF8(sbuff, wbuff);
	console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	return 0;
}
