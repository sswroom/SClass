#include "Stdafx.h"
#include "Core/Core.h"
//#include "Data/BTreeMapNative.hpp"
#include "Data/FastStringMapNative.hpp"
#include "Data/StringHashMap.hpp"
#include "Data/StringMapNative.hpp"
#include "Text/MyString.h"

#include <stdio.h>

#define DATACNT 10000
Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
//	Data::StringMapNative<UInt32> map;
	Data::FastStringMapNative<UInt32> map;
//	Data::BTreeMapNative<UInt32> map;
//	Data::StringHashMap<UInt32> map;
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	UInt32 val;
	UInt32 val2;
	NN<Text::String> s;
	val = 0;
	while (val < DATACNT)
	{
		sptr = Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("Text")), val);
		s = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
		map.PutNN(s, val);
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