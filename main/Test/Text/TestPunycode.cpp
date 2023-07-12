#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextBinEnc/Punycode.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UTF8Char refbuff[256];
	UTF8Char *refPtr;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	WChar wbuff[256];
	refPtr = Text::StrWChar_UTF8(refbuff, L"そのスピードで");

	sptr = Text::TextBinEnc::Punycode::Decode(sbuff, UTF8STRC("xn--d9juau41awczczp"));
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), refbuff, (UOSInt)(refPtr - refbuff)))
		return 1;

	Text::TextBinEnc::Punycode::Decode(wbuff, L"xn--d9juau41awczczp");
	sptr = Text::StrWChar_UTF8(sbuff, wbuff);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), refbuff, (UOSInt)(refPtr - refbuff)))
		return 1;

	sptr = Text::TextBinEnc::Punycode::Decode(sbuff, (const UTF8Char*)"xn--d9juau41awczczpabcdef", 19);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), refbuff, (UOSInt)(refPtr - refbuff)))
		return 1;
	return 0;
}
