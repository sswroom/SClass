#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/ExceptionLogger.h"
#include "Manage/ExceptionRecorder.h"
#include "Text/MyString.h"

NN<Text::String> Manage::ExceptionRecorder::fileName;
Manage::ExceptionRecorder::ExceptionAction Manage::ExceptionRecorder::exAction;
Int32 (__stdcall *ExceptionRecorder_Handler)(void *);

Text::CStringNN Manage::ExceptionRecorder::GetExceptionCodeName(UInt32 exCode)
{
	return CSTR("Unknown Exception");
}

Int32 __stdcall Manage::ExceptionRecorder::ExceptionHandler(void *exInfo)
{
	return 0;
}

Manage::ExceptionRecorder::ExceptionRecorder(Text::CStringNN fileName, ExceptionAction exAction)
{
	this->fileName = Text::String::New(fileName);
}

Manage::ExceptionRecorder::~ExceptionRecorder()
{
	this->fileName->Release();
}
