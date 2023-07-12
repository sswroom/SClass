#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/BTreeMap.h"
#include "Data/FastStringMap.h"
#include "Data/StringHashMap.h"
#include "Data/StringMap.h"
#include "Text/MyString.h"

#include <stdio.h>

#define DATACNT 10000
Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
//	Data::StringMap<UInt32> map;
	Data::FastStringMap<UInt32> map;
//	Data::BTreeMap<UInt32> map;
//	Data::StringHashMap<UInt32> map;
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	UInt32 val;
	UInt32 val2;
	Text::String *s;
	val = 0;
	while (val < DATACNT)
	{
		sptr = Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("Text")), val);
		s = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
		map.Put(s, val);
		s->Release();
		val++;
	}

	val = 0;
	while (val < DATACNT)
	{
		sptr = Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("Text")), val);
		val2 = map.GetC(CSTRP(sbuff, sptr));
		if (val != val2)
		{
			return 1;
		}
		val++;
	}

	return 0;
}