#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/Locale.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::Locale *locale;
	NEW_CLASS(locale, Text::Locale());
	DEL_CLASS(locale);
	return 0;
}
