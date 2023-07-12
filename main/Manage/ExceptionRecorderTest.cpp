#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "Text/MyString.h"

void main5()
{
	Int32 i = 0;
	UTF8Char sbuff[10];

	i = 10 / i;
	Text::StrInt32(sbuff, i);
}

void main4()
{
	main5();
}

void main3()
{
	main4();
}

void main2()
{
	main3();
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Manage::ExceptionRecorder *recorder;
	NEW_CLASS(recorder, Manage::ExceptionRecorder(CSTR("except.txt"), Manage::ExceptionRecorder::EA_CLOSE));

	main2();

	DEL_CLASS(recorder);
	return 0;
}
