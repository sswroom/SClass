#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/BTreeMap.h"
#include "Data/FastStringMap.h"
#include "Data/StringHashMap.h"
#include "Data/StringMap.h"
#include "Text/MyString.h"

#include <stdio.h>

#define DATACNT 10000
Int32 MyMain(Core::IProgControl *progCtrl)
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
		sptr = Text::StrUInt32(Text::StrConcat(sbuff, (const UTF8Char*)"Text"), val);
		s = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
		map.Put(s, val);
		s->Release();
		val++;
	}

	val = 0;
	while (val < DATACNT)
	{
		sptr = Text::StrUInt32(Text::StrConcat(sbuff, (const UTF8Char*)"Text"), val);
		s = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
		val2 = map.Get(s);
		s->Release();
		if (val != val2)
		{
			printf("Get value error: %s returns %d\r\n", sbuff, val2);
			break;
		}
		val++;
	}

	return 0;
}