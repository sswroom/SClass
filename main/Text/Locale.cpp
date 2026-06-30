#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/Locale.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<Text::Locale> locale;
	NEW_CLASSNN(locale, Text::Locale());
	locale.Delete();
	return 0;
}
